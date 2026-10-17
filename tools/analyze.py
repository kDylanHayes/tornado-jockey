#!/usr/bin/env python3
"""Create the Ghidra project for orig/Tornado.exe and run auto-analysis."""

import argparse
import os
import subprocess
import sys
from pathlib import Path

from delink import DEFAULT_PROJECT, ROOT, analyze_headless


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--ghidra", type=Path, default=os.environ.get("GHIDRA_INSTALL_DIR"),
                        help="Ghidra install folder (or set GHIDRA_INSTALL_DIR)")
    parser.add_argument("--project", type=Path, default=DEFAULT_PROJECT, help="Ghidra project folder")
    args = parser.parse_args()
    if args.ghidra is None:
        sys.exit("error: pass --ghidra or set GHIDRA_INSTALL_DIR")

    args.project.mkdir(parents=True, exist_ok=True)
    cmd = [
        str(analyze_headless(args.ghidra)), str(args.project), "TornadoJockey",
        "-import", str(ROOT / "orig" / "Tornado.exe"), "-overwrite",
        "-scriptPath", str(ROOT / "tools" / "ghidra"),
        "-postScript", "ApplySymbols.java", str(ROOT / "config" / "symbols.txt"),
    ]
    print("Analyzing Tornado.exe (takes a few minutes)...")
    return subprocess.run(cmd).returncode


if __name__ == "__main__":
    sys.exit(main())
