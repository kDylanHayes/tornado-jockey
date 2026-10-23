#!/usr/bin/env python3
"""Delink units from orig/Tornado.exe into target COFF objects using Ghidra.

Requires Ghidra with the ghidra-delinker-extension installed, and a Ghidra
project created by tools/analyze.py.
"""

import argparse
import json
import os
import subprocess
import sys
from pathlib import Path

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
DEFAULT_PROJECT = ROOT / "build" / "ghidra"


def analyze_headless(ghidra: Path) -> Path:
    script = ghidra / "support" / ("analyzeHeadless.bat" if os.name == "nt" else "analyzeHeadless")
    if not script.is_file():
        sys.exit(f"error: {script} not found; pass --ghidra or set GHIDRA_INSTALL_DIR")
    return script


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--ghidra", type=Path, default=os.environ.get("GHIDRA_INSTALL_DIR"),
                        help="Ghidra install folder (or set GHIDRA_INSTALL_DIR)")
    parser.add_argument("--project", type=Path, default=DEFAULT_PROJECT, help="Ghidra project folder")
    parser.add_argument("units", nargs="*", help="Unit names to delink (default: all with ranges)")
    args = parser.parse_args()
    if args.ghidra is None:
        sys.exit("error: pass --ghidra or set GHIDRA_INSTALL_DIR")

    units = json.loads((ROOT / "config" / "units.json").read_text())["units"]
    selected = [u for u in units if u.get("ranges") and (not args.units or u["name"] in args.units)]
    if not selected:
        print("No units to delink.")
        return 0

    script_args = []
    for unit in selected:
        target = ROOT / unit["target"]
        target.parent.mkdir(parents=True, exist_ok=True)
        if target.exists():
            target.unlink()
        script_args += [str(target), *unit["ranges"], "--"]

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

    cmd = [
        str(analyze_headless(args.ghidra)), str(args.project), "TornadoJockey",
        "-process", "Tornado.exe", "-noanalysis", "-readOnly",
        "-scriptPath", str(ROOT / "tools" / "ghidra"),
        "-preScript", "ApplySymbols.java", str(ROOT / "config" / "symbols.txt"),
        "-preScript", "FixReferences.java", str(ROOT / "config" / "norefs.txt"),
        "-preScript", "FixJumpTables.java",
        "-postScript", "DelinkUnit.java", *script_args,
    ]
    result = subprocess.run(cmd, capture_output=True, text=True)
    log = ROOT / "build" / "delink.log"
    log.write_text(result.stdout + result.stderr)
    missing = [u["name"] for u in selected if not (ROOT / u["target"]).is_file()]
    for line in (result.stdout + result.stderr).splitlines():
        if "DELINK FAILED" in line:
            print(line[line.index("DELINK FAILED"):].replace(" (GhidraScript)", "").strip(), file=sys.stderr)
    if result.returncode != 0 or missing or "SCRIPT ERROR" in result.stdout + result.stderr:
        print(f"error: delinking failed ({', '.join(missing) or 'see log'}); log in {log.relative_to(ROOT)}",
              file=sys.stderr)
    for unit in selected:
        if unit["name"] in missing:
            continue
        base = int(unit["ranges"][0].split("-")[0], 16) if len(unit["ranges"]) == 1 else None
        ours = ROOT / "build" / Path(unit["source"]).with_suffix(".obj") if "source" in unit else None
        spans = [tuple(int(x, 16) for x in r.split("-")) for r in unit["ranges"]]
        # Sizes from our object where symbols.txt has none: a function ending
        # in a call that never returns keeps the int3 the compiler put after it.
        unit_sizes = base_function_sizes(ours) if ours is not None and ours.is_file() else {}
        unit_sizes.update({n: sz for n, a, sz in sizes if any(lo <= a <= hi for lo, hi in spans)})
        split.main_path(ROOT / unit["target"], base=base, ours=ours, sizes=unit_sizes)
        fix_data_symbols.main_path(ROOT / unit["target"],
                                   {n: a for n, a in variables if any(lo <= a <= hi for lo, hi in spans)},
                                   [v for v in sized if not any(lo <= v[1] <= hi for lo, hi in spans)])
    if result.returncode != 0 or missing or "SCRIPT ERROR" in result.stdout + result.stderr:
        return 1
    print(f"Delinked {len(selected)} unit(s)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
