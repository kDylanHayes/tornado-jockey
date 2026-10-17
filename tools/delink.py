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

import split_functions as split

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

    cmd = [
        str(analyze_headless(args.ghidra)), str(args.project), "TornadoJockey",
        "-process", "Tornado.exe", "-noanalysis", "-readOnly",
        "-scriptPath", str(ROOT / "tools" / "ghidra"),
        "-preScript", "ApplySymbols.java", str(ROOT / "config" / "symbols.txt"),
        "-postScript", "DelinkUnit.java", *script_args,
    ]
    result = subprocess.run(cmd, capture_output=True, text=True)
    log = ROOT / "build" / "delink.log"
    log.write_text(result.stdout + result.stderr)
    missing = [u["name"] for u in selected if not (ROOT / u["target"]).is_file()]
    if result.returncode != 0 or missing:
        print(f"error: delinking failed ({', '.join(missing) or 'see log'}); log in {log.relative_to(ROOT)}",
              file=sys.stderr)
        return 1
    for unit in selected:
        split.main_path(ROOT / unit["target"])
    print(f"Delinked {len(selected)} unit(s)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
