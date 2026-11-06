#!/usr/bin/env python3
"""Delink units from orig/Tornado.exe into target COFF objects using Ghidra.

Requires Ghidra with the ghidra-delinker-extension installed, and a Ghidra
project created by tools/analyze.py.
"""

import argparse
import json
import os
import struct
import subprocess
import sys
import tempfile
from pathlib import Path

import fix_data_icf
import fix_data_symbols
import split_functions as split
from find_objects import Coff


def base_function_sizes(path: Path) -> dict:
    obj = Coff(path)
    sizes = {}
    for index, sec in enumerate(obj.sections):
        if sec["flags"] & 0x20:  # code
            for name, start, end in obj.functions(index):
                sizes.setdefault(name, end - start)
    return sizes

ROOT = Path(__file__).resolve().parent.parent


def _dot_prefixed(path: Path) -> bool:
    """True if any element of an absolute path starts with '.'.

    Ghidra refuses to create a project below such a path
    ("Path element starting with '.' is not permitted"), which bites when the
    checkout lives under a hidden folder such as ~/.local or an agent
    worktree's .ao directory.
    """
    return any(
        part not in (path.anchor, os.sep) and part.startswith(".")
        for part in path.parts
    )


def default_project() -> Path:
    """Project folder inside the checkout, or a dot-free fallback."""
    project = ROOT / "build" / "ghidra"
    if not _dot_prefixed(project):
        return project
    candidates = []
    local = os.environ.get("LOCALAPPDATA")
    if local:
        candidates.append(Path(local) / "tornado-jockey" / "ghidra")
    candidates.append(Path(tempfile.gettempdir()) / "tornado-jockey-ghidra")
    for candidate in candidates:
        if not _dot_prefixed(candidate):
            return candidate
    return candidates[-1]


DEFAULT_PROJECT = default_project()
PROJECT_FALLBACK = DEFAULT_PROJECT != ROOT / "build" / "ghidra"


def analyze_headless(ghidra: Path) -> Path:
    script = ghidra / "support" / ("analyzeHeadless.bat" if os.name == "nt" else "analyzeHeadless")
    if not script.is_file():
        sys.exit(f"error: {script} not found; pass --ghidra or set GHIDRA_INSTALL_DIR")
    return script


# cmd.exe refuses an input line longer than 8191 characters. The arguments
# here are not the whole line: analyzeHeadless.bat -> launch.bat re-emits them
# on a java command line that also carries ~800 characters of VM arguments,
# -cp, and class names, so keep our own arguments well under the limit.
# Every unit adds a target path and its ranges to that invocation, so large
# selections are delinked in several headless runs that each fit.
WINDOWS_COMMAND_LIMIT = 7000
POSIX_COMMAND_LIMIT = 100_000  # ARG_MAX is much larger, but stay well clear


def command_size(args: list[str]) -> int:
    """Rough rendered length of an argument list (quoting and separators)."""
    return sum(len(arg) + 3 for arg in args)


def apply_symbol_mappings(path: Path, mappings: dict) -> int:
    """Give referenced, undefined symbols the names our object uses.

    objdiff pairs code relocations through the unit's symbol_mappings, but it
    compares a data section's relocations by the names the target object
    carries, so a vtable slot the linker folded onto another unit's identical
    function stays unmatched however the mapping reads. Renaming the target's
    undefined symbol (the fold target lives outside this unit, so it has no
    body here) makes both sides agree. Defined symbols are left alone: objdiff
    already pairs those.
    """
    if not mappings:
        return 0
    coff = split.Coff(path.read_bytes())
    by_index = {i: rec for i, rec, _ in coff.symbols}
    taken = {coff.name(rec) for _, rec, _ in coff.symbols}
    renamed = 0
    for sec in coff.sections:
        for rel in sec["relocs"]:
            rec = by_index.get(rel[1])
            if rec is None:
                continue
            name = coff.name(rec)
            # Mapping a label that is defined inside this object would make
            # objdiff treat it as a function start and cut the enclosing
            # function. Defined function symbols still take the mapping (the
            # folded-thunk case), but static labels keep their names; objdiff
            # applies the mapping to code operands itself.
            secno, typ = struct.unpack_from("<hH", rec, 12)
            if secno > 0 and typ != 0x20:
                continue
            if name in mappings and mappings[name] not in taken:
                coff.set_name(rec, mappings[name])
                taken.add(mappings[name])
                renamed += 1
    if renamed:
        path.write_bytes(coff.write())
    return renamed


def unit_args(unit: dict) -> list[str]:
    target = ROOT / unit["target"]
    return [str(target), *unit["ranges"], "--"]


def batches(units: list[dict], base: list[str], limit: int) -> list[list[list[str]]]:
    """Split units into headless runs whose command lines fit limit."""
    result = []
    batch: list[list[str]] = []
    size = command_size(base)
    for unit in units:
        args = unit_args(unit)
        args_size = command_size(args)
        if batch and size + args_size > limit:
            result.append(batch)
            batch, size = [], command_size(base)
        batch.append(args)
        size += args_size
    if batch:
        result.append(batch)
    return result


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--ghidra", type=Path, default=os.environ.get("GHIDRA_INSTALL_DIR"),
                        help="Ghidra install folder (or set GHIDRA_INSTALL_DIR)")
    parser.add_argument("--project", type=Path, default=DEFAULT_PROJECT, help="Ghidra project folder")
    parser.add_argument("units", nargs="*", help="Unit names to delink (default: all with ranges)")
    args = parser.parse_args()
    if args.ghidra is None:
        sys.exit("error: pass --ghidra or set GHIDRA_INSTALL_DIR")
    if PROJECT_FALLBACK and args.project == DEFAULT_PROJECT:
        print(f"note: {ROOT} is under a dot-prefixed folder, which Ghidra rejects; "
              f"using {args.project}", file=sys.stderr)

    units = json.loads((ROOT / "config" / "units.json").read_text())["units"]
    selected = [u for u in units if u.get("ranges") and (not args.units or u["name"] in args.units)]
    if not selected:
        print("No units to delink.")
        return 0

    for unit in selected:
        target = ROOT / unit["target"]
        target.parent.mkdir(parents=True, exist_ok=True)
        if target.exists():
            target.unlink()

    # The COFF exporter refuses an object with two symbols of the same name.
    names = {}
    sizes = []  # (name, address, size) of functions with a known size
    variables = []  # (name, address) of data symbols
    sized = []  # (name, address, size) of data symbols with a known size
    for line in (ROOT / "config" / "symbols.txt").read_text().splitlines():
        parts = line.split()
        if len(parts) >= 2 and not line.startswith("#") and "::" not in parts[1]:
            names.setdefault(parts[1], []).append(int(parts[0], 16))
        if len(parts) >= 4 and parts[2] == "function":
            sizes.append((parts[1], int(parts[0], 16), int(parts[3], 16)))
        if len(parts) >= 3 and parts[2] == "data":
            variables.append((parts[1], int(parts[0], 16)))
        if len(parts) >= 4 and parts[2] == "data":
            sized.append((parts[1], int(parts[0], 16), int(parts[3], 16)))
    for unit in selected:
        spans = [tuple(int(x, 16) for x in r.split("-")) for r in unit["ranges"]]
        for name, addrs in names.items():
            inside = [a for a in addrs if any(lo <= a <= hi for lo, hi in spans)]
            if len(inside) > 1:
                print(f"warning: {name} is named {len(inside)} times inside {unit['name']}", file=sys.stderr)

    base = [
        str(analyze_headless(args.ghidra)), str(args.project), "TornadoJockey",
        "-process", "Tornado.exe", "-noanalysis", "-readOnly",
        "-scriptPath", str(ROOT / "tools" / "ghidra"),
        "-preScript", "ApplySymbols.java", str(ROOT / "config" / "symbols.txt"),
        "-preScript", "FixReferences.java", str(ROOT / "config" / "norefs.txt"),
        str(ROOT / "config" / "symbols.txt"),
        "-preScript", "FixJumpTables.java",
        "-postScript", "DelinkUnit.java",
    ]
    limit = WINDOWS_COMMAND_LIMIT if os.name == "nt" else POSIX_COMMAND_LIMIT
    runs = batches(selected, base, limit)
    output = []
    returncode = 0
    for index, run in enumerate(runs, 1):
        if len(runs) > 1:
            print(f"delink: headless run {index}/{len(runs)}, {len(run)} unit(s)",
                  file=sys.stderr)
        cmd = base + [arg for args in run for arg in args]
        result = subprocess.run(cmd, capture_output=True, text=True)
        returncode = returncode or result.returncode
        output.append(result.stdout + result.stderr)
    log_text = "\n".join(output)
    log = ROOT / "build" / "delink.log"
    log.write_text(log_text)
    missing = [u["name"] for u in selected if not (ROOT / u["target"]).is_file()]
    for line in log_text.splitlines():
        if "DELINK FAILED" in line:
            print(line[line.index("DELINK FAILED"):].replace(" (GhidraScript)", "").strip(), file=sys.stderr)
    if returncode != 0 or missing or "SCRIPT ERROR" in log_text:
        print(f"error: delinking failed ({', '.join(missing) or 'see log'}); log in {log.relative_to(ROOT)}",
              file=sys.stderr)
    for unit in selected:
        if unit["name"] in missing:
            continue
        base_addr = int(unit["ranges"][0].split("-")[0], 16) if len(unit["ranges"]) == 1 else None
        ours = ROOT / "build" / Path(unit["source"]).with_suffix(".obj") if "source" in unit else None
        spans = [tuple(int(x, 16) for x in r.split("-")) for r in unit["ranges"]]
        # Sizes from our object where symbols.txt has none: a function ending
        # in a call that never returns keeps the int3 the compiler put after it.
        unit_sizes = base_function_sizes(ours) if ours is not None and ours.is_file() else {}
        unit_sizes.update({n: sz for n, a, sz in sizes if any(lo <= a <= hi for lo, hi in spans)})
        # Data sizes too: symbols.txt's size cuts the linker's alignment
        # padding off the item, the way it does for functions. split_functions
        # takes these separately, because it may only trim data when no other
        # symbol starts or refers inside the part the size would cut off.
        data_sizes = {n: sz for n, a, sz in sized if any(lo <= a <= hi for lo, hi in spans)}
        split.main_path(ROOT / unit["target"], base=base_addr, ours=ours, sizes=unit_sizes,
                        data_sizes=data_sizes)
        fix_data_symbols.main_path(ROOT / unit["target"],
                                   {n: a for n, a in variables if any(lo <= a <= hi for lo, hi in spans)},
                                   [v for v in sized if not any(lo <= v[1] <= hi for lo, hi in spans)])
        apply_symbol_mappings(ROOT / unit["target"], unit.get("symbol_mappings", {}))
        if ours is not None and ours.is_file():
            fix_data_icf.main_path(ROOT / unit["target"], ours)
    if returncode != 0 or missing or "SCRIPT ERROR" in log_text:
        return 1
    print(f"Delinked {len(selected)} unit(s)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
