#!/usr/bin/env python3
"""Generate an objdiff progress report that covers all of Tornado.exe's code.

objdiff only counts code in the units listed in objdiff.json, so a report of
the configured units alone says ~100% while most of the exe is still
untouched. This adds a placeholder unit for every stretch of .text that no
unit covers yet: a target object holding that code (one symbol per function
Ghidra found) and no source. objdiff then counts it as unmatched code, in the
progress category config/regions.json gives its region.

Needs the Ghidra project from tools/analyze.py, delinked targets from
tools/delink.py, compiled objects from ninja, and objdiff-cli on PATH (or
--objdiff). Writes build/report/objdiff.json and build/report.json.
"""

import argparse
import json
import os
import shutil
import struct
import subprocess
import sys
from pathlib import Path

from delink import DEFAULT_PROJECT, ROOT, analyze_headless

BUILD = ROOT / "build"
PROJECT_DIR = BUILD / "report"
GAP_DIR = PROJECT_DIR / "gaps"

IMAGE_FILE_MACHINE_I386 = 0x14C
TEXT_FLAGS = 0x60500020  # code, execute, read, 16-byte alignment
IMAGE_SYM_CLASS_EXTERNAL = 2
FUNCTION_TYPE = 0x20


def parse_range(text: str) -> tuple[int, int]:
    start, end = text.split("-")
    return int(start, 16), int(end, 16) + 1  # config ranges are inclusive


def export_functions(ghidra: Path, project: Path, out: Path) -> None:
    """Write Ghidra's function list (with our symbols applied) to out."""
    cmd = [
        str(analyze_headless(ghidra)), str(project), "TornadoJockey",
        "-process", "Tornado.exe", "-noanalysis", "-readOnly",
        "-scriptPath", str(ROOT / "tools" / "ghidra"),
        "-preScript", "ApplySymbols.java", str(ROOT / "config" / "symbols.txt"),
        "-postScript", "ExportSymbols.java", str(out),
    ]
    print("Exporting Ghidra's function list...")
    with open(BUILD / "report.log", "w") as log:
        if subprocess.run(cmd, stdout=log, stderr=subprocess.STDOUT).returncode or not out.is_file():
            sys.exit(f"error: function export failed, see {(BUILD / 'report.log').relative_to(ROOT)}")


def read_functions(path: Path) -> dict[int, str]:
    functions = {}
    for line in path.read_text().splitlines():
        fields = line.split()
        if len(fields) >= 3 and not line.startswith("#") and fields[2] == "function":
            functions[int(fields[0], 16)] = fields[1]
    return functions


def write_gap_object(path: Path, code: bytes, symbols: list[tuple[int, str]]) -> None:
    """Write a COFF object with one .text section and a symbol per function."""
    strtab = bytearray(4)
    records = bytearray()
    for offset, name in symbols:
        raw = name.encode("latin-1")
        if len(raw) <= 8:
            name_field = raw.ljust(8, b"\0")
        else:
            name_field = struct.pack("<II", 0, len(strtab))
            strtab += raw + b"\0"
        records += name_field + struct.pack("<IhHBB", offset, 1, FUNCTION_TYPE, IMAGE_SYM_CLASS_EXTERNAL, 0)
    struct.pack_into("<I", strtab, 0, len(strtab))
    raw_ptr = 20 + 40
    sym_ptr = raw_ptr + len(code)
    header = struct.pack("<HHIIIHH", IMAGE_FILE_MACHINE_I386, 1, 0, sym_ptr, len(symbols), 0, 0)
    section = b".text\0\0\0" + struct.pack("<IIIIIIHHI", 0, 0, len(code), raw_ptr, 0, 0, 0, 0, TEXT_FLAGS)
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_bytes(header + section + code + records + strtab)


def gaps(text_start: int, text_end: int, covered: list[tuple[int, int]], regions: list[dict]):
    """Yield (start, end, category) for uncovered .text, split at region edges."""
    edges = sorted({text_start, text_end, *(parse_range(r["range"])[0] for r in regions)})
    cursor = text_start
    in_text = [(max(s, text_start), min(e, text_end)) for s, e in covered if s < text_end and e > text_start]
    for start, end in sorted(in_text) + [(text_end, text_end)]:
        if start > cursor:
            cuts = [cursor] + [e for e in edges if cursor < e < start] + [start]
            for a, b in zip(cuts, cuts[1:]):
                yield a, b, region_category(a, regions)
        cursor = max(cursor, end)


def region_category(address: int, regions: list[dict]) -> str:
    for region in regions:
        start, end = parse_range(region["range"])
        if start <= address < end:
            return region["category"]
    sys.exit(f"error: 0x{address:08x} is in no region in config/regions.json")


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--ghidra", type=Path, default=os.environ.get("GHIDRA_INSTALL_DIR"),
                        help="Ghidra install folder (or set GHIDRA_INSTALL_DIR)")
    parser.add_argument("--project", type=Path, default=DEFAULT_PROJECT, help="Ghidra project folder")
    parser.add_argument("--objdiff", default=os.environ.get("OBJDIFF_CLI", "objdiff-cli"),
                        help="objdiff-cli executable (or set OBJDIFF_CLI)")
    parser.add_argument("-o", "--output", type=Path, default=BUILD / "report.json", help="Report file")
    args = parser.parse_args()
    if args.ghidra is None:
        sys.exit("error: pass --ghidra or set GHIDRA_INSTALL_DIR")
    if shutil.which(args.objdiff) is None:
        sys.exit(f"error: {args.objdiff} not found; pass --objdiff or set OBJDIFF_CLI")

    target = json.loads((ROOT / "config" / "target.json").read_text())
    regions = json.loads((ROOT / "config" / "regions.json").read_text())["regions"]
    objdiff_path = ROOT / "objdiff.json"
    if not objdiff_path.is_file():
        sys.exit("error: objdiff.json not found; run configure.py first")
    objdiff = json.loads(objdiff_path.read_text())
    units = json.loads((ROOT / "config" / "units.json").read_text())["units"]

    text = next(s for s in target["sections"] if s["name"] == ".text")
    text_start = int(text["va"], 16)
    text_end = text_start + int(text["size"], 16)
    exe = (ROOT / "orig" / target["name"]).read_bytes()
    # .text is the first section and starts at file offset 0x1000 like its RVA.
    file_offset = text_start - int(target["image_base"], 16)

    functions_txt = BUILD / "functions.txt"
    export_functions(args.ghidra, args.project, functions_txt)
    functions = read_functions(functions_txt)

    covered = [parse_range(r) for u in units for r in u.get("ranges", [])]
    if GAP_DIR.exists():
        shutil.rmtree(GAP_DIR)
    gap_units = []
    for start, end, category in gaps(text_start, text_end, covered, regions):
        code = exe[file_offset + start - text_start:file_offset + end - text_start]
        starts = sorted(a for a in functions if start <= a < end)
        if not starts or starts[0] != start:
            starts.insert(0, start)
        symbols = [(a - start, functions.get(a, f"code_{a:08x}")) for a in starts]
        name = f"unassigned/{start:08x}"
        obj = GAP_DIR / f"{start:08x}.obj"
        write_gap_object(obj, code, symbols)
        gap_units.append({
            "name": name,
            "target_path": obj.relative_to(PROJECT_DIR).as_posix(),
            "metadata": {"complete": False, "progress_categories": [category], "auto_generated": True},
        })

    # The report project lives in build/report so contributors' objdiff GUI
    # doesn't list hundreds of placeholder units; point its paths back up.
    up = os.path.relpath(ROOT, PROJECT_DIR)
    for unit in objdiff["units"]:
        for key in ("target_path", "base_path"):
            if key in unit:
                unit[key] = f"{up}/{unit[key]}"
    objdiff["units"] += gap_units
    for key in ("custom_make", "watch_patterns"):
        objdiff.pop(key, None)
    objdiff["build_base"] = False
    PROJECT_DIR.mkdir(parents=True, exist_ok=True)
    (PROJECT_DIR / "objdiff.json").write_text(json.dumps(objdiff, indent=2) + "\n")

    print(f"Added {len(gap_units)} placeholder units for code no unit covers yet")
    output = args.output.resolve()
    result = subprocess.run([args.objdiff, "report", "generate", "-p", str(PROJECT_DIR), "-o", str(output)])
    if result.returncode:
        return result.returncode
    print(f"Wrote {output.relative_to(ROOT) if output.is_relative_to(ROOT) else output}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
