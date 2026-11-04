#!/usr/bin/env python3
"""Write decompiled C for a unit from the Ghidra project.

Reads the unit's address ranges from config/units.json and runs analyzeHeadless
read-only with tools/ghidra/DumpDecomp.java, which prints every function that
intersects those ranges. Requires a project created by tools/analyze.py.

    py tools/decompile.py Tongas2/LOG
    py tools/decompile.py Tongas2/LOG --out build/LOG.c

On Linux and macOS, use python3; --ghidra or GHIDRA_INSTALL_DIR selects the
Ghidra install.
"""

import argparse
import json
import os
import subprocess
import sys
from pathlib import Path

from delink import DEFAULT_PROJECT, PROJECT_FALLBACK, ROOT, analyze_headless


def main() -> int:
    parser = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter
    )
    parser.add_argument("unit", help="unit name from config/units.json")
    parser.add_argument("--ghidra", type=Path, default=os.environ.get("GHIDRA_INSTALL_DIR"),
                        help="Ghidra install folder (or set GHIDRA_INSTALL_DIR)")
    parser.add_argument("--project", type=Path, default=DEFAULT_PROJECT,
                        help="Ghidra project folder")
    parser.add_argument("--out", type=Path,
                        help="output file (default: build/decompile/<unit>.c)")
    args = parser.parse_args()
    if args.ghidra is None:
        sys.exit("error: pass --ghidra or set GHIDRA_INSTALL_DIR")
    if PROJECT_FALLBACK and args.project == DEFAULT_PROJECT:
        print(f"note: {ROOT} is under a dot-prefixed folder, which Ghidra rejects; "
              f"using {args.project}", file=sys.stderr)
    if not args.project.is_dir():
        sys.exit(f"error: {args.project} not found; run tools/analyze.py first")

    units = json.loads((ROOT / "config" / "units.json").read_text())["units"]
    matches = [u for u in units if u["name"] == args.unit]
    if not matches:
        matches = [u for u in units if u["name"].lower() == args.unit.lower()]
    if not matches:
        sys.exit(f"error: no unit named {args.unit!r} in config/units.json")
    unit = matches[0]
    ranges = unit.get("ranges") or []
    if not ranges:
        sys.exit(f"error: unit {unit['name']} has no ranges")

    out = args.out or ROOT / "build" / "decompile" / (unit["name"].replace("/", "_") + ".c")
    out.parent.mkdir(parents=True, exist_ok=True)
    out = out.resolve()

    cmd = [
        str(analyze_headless(args.ghidra)), str(args.project), "TornadoJockey",
        "-process", "Tornado.exe", "-noanalysis", "-readOnly",
        "-scriptPath", str(ROOT / "tools" / "ghidra"),
        "-preScript", "ApplySymbols.java", str(ROOT / "config" / "symbols.txt"),
        "-postScript", "DumpDecomp.java", *ranges, "--out", str(out),
    ]
    result = subprocess.run(cmd, capture_output=True, text=True)

    if not out.is_file():
        sys.stderr.write(result.stdout + result.stderr)
        sys.exit(f"error: decompilation produced no output for {unit['name']}")
    text = out.read_text()
    functions = text.count("// ==== ")
    if result.returncode != 0 or functions == 0:
        sys.stderr.write(result.stdout + result.stderr)
        sys.exit(f"error: decompilation failed for {unit['name']}; output in {out}")
    print(f"{unit['name']}: {functions} function(s) -> {out}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
