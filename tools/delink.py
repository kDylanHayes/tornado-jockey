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
        target.unlink(missing_ok=True)  # so a failed export can't leave a stale object
        script_args += [",".join(unit["ranges"]), str(target)]

    # The COFF exporter refuses an object with two symbols of the same name
    # and doesn't say which, so catch that here.
    seen = {}
    for line in (ROOT / "config" / "symbols.txt").read_text().splitlines():
        parts = line.split()
        if len(parts) >= 2 and not line.startswith("#") and "::" not in parts[1]:
            seen.setdefault(parts[1], []).append(parts[0])
    for name, addrs in seen.items():
        if len(addrs) > 1:
            print(f"warning: {name} is named at {', '.join(addrs)}; delinking fails if two land in one unit",
                  file=sys.stderr)

    cmd = [
        str(analyze_headless(args.ghidra)), str(args.project), "TornadoJockey",
        "-process", "Tornado.exe", "-noanalysis", "-readOnly",
        "-scriptPath", str(ROOT / "tools" / "ghidra"),
        "-preScript", "ApplySymbols.java", str(ROOT / "config" / "symbols.txt"),
        "-preScript", "FixReferences.java", str(ROOT / "config" / "norefs.txt"),
        "-postScript", "DelinkUnits.java", *script_args,
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
        return 1
    print(f"Delinked {len(selected)} unit(s)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
