#!/usr/bin/env python3
"""List a unit's functions by exact objdiff match percentage.

Usage:
  py tools/function_status.py Tongas2/GEOMETRY
  py tools/function_status.py Tongas2/GEOMETRY --all

Unlike tools/unit_status.py, which reads the aggregate report, this runs
objdiff-cli's one-shot diff for the unit and reads the per-symbol match
percentage, so 100.00% here is byte-exact code for that function. Functions
are printed worst first. Run ninja first so build/src is current.

The diff also needs the delinked target in build/target; run tools/delink.py
for the unit when config/symbols.txt or config/units.json changed.
"""

import argparse
import json
import subprocess
import sys
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
DEFAULT_OBJDIFF = ROOT / "tools" / "objdiff" / "objdiff-cli.exe"


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("unit", help="unit name, e.g. Tongas2/GEOMETRY")
    parser.add_argument("--objdiff", type=Path, default=DEFAULT_OBJDIFF,
                        help="objdiff-cli executable (default: tools/objdiff)")
    parser.add_argument("--all", action="store_true", help="include exact functions")
    args = parser.parse_args()
    if not (ROOT / "objdiff.json").is_file():
        sys.exit("error: objdiff.json not found; run configure.py first")
    with tempfile.NamedTemporaryFile(suffix=".json", delete=False) as tmp:
        out = Path(tmp.name)
    try:
        result = subprocess.run(
            [str(args.objdiff), "diff", "-p", str(ROOT), "-u", args.unit,
             "--format", "json", "-o", str(out)],
            cwd=ROOT, capture_output=True, text=True)
        if result.returncode:
            sys.exit(result.stderr.strip() or f"error: objdiff exited {result.returncode}")
        diff = json.loads(out.read_text())
    finally:
        out.unlink(missing_ok=True)

    symbols = [s for s in diff.get("left", {}).get("symbols", [])
               if s.get("kind") == "SYMBOL_FUNCTION"
               and s.get("match_percent") is not None]
    if not symbols:
        sys.exit(f"error: no functions found for {args.unit}")
    symbols.sort(key=lambda s: s["match_percent"])
    for s in symbols:
        if s["match_percent"] >= 100 and not args.all:
            continue
        size = int(s.get("size") or 0)
        print(f"{s['match_percent']:7.2f}% {size:6d}  {s['name']}")
    exact = sum(1 for s in symbols if s["match_percent"] >= 100)
    print(f"\n{args.unit}: {exact}/{len(symbols)} functions exact")
    return 0


if __name__ == "__main__":
    sys.exit(main())
