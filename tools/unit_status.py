#!/usr/bin/env python3
"""Show a configured unit's match numbers from build/report.json.

Usage:
  py tools/unit_status.py [UNIT ...] [--all] [--functions]

With no unit names, prints every configured unit that is not complete.
`--functions` also lists the unit's functions that are not an exact match,
worst first, as one-shot objdiff reports them. Run tools/report.py first so
build/report.json is current.
"""

import argparse
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
REPORT = ROOT / "build" / "report.json"


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("units", nargs="*", help="unit names, e.g. Tongas2/GEOMETRY")
    parser.add_argument("--all", action="store_true", help="include complete units")
    parser.add_argument("--functions", action="store_true", help="list unmatched functions")
    args = parser.parse_args()
    if not REPORT.is_file():
        sys.exit("error: build/report.json not found; run tools/report.py first")
    report = json.loads(REPORT.read_text())
    units = {u["name"]: u for u in report.get("units", []) if not u.get("metadata", {}).get("auto_generated")}
    selected = args.units or [name for name, unit in units.items() if args.all or not unit.get("metadata", {}).get("complete")]
    missing = [name for name in selected if name not in units]
    if missing:
        sys.exit(f"error: unknown unit(s): {', '.join(missing)}")
    for name in selected:
        unit = units[name]
        m = unit["measures"]
        print(f"{name}: {m.get('matched_code_percent', 0):.2f}% exact, "
              f"{m.get('fuzzy_match_percent', 0):.2f}% fuzzy, "
              f"{m.get('matched_code', 0)}/{m.get('total_code', 0)} bytes, "
              f"{m.get('matched_functions', 0)}/{m.get('total_functions', 0)} functions")
        if args.functions:
            functions = [f for f in unit.get("functions", [])
                         if f.get("fuzzy_match_percent", 100) < 100]
            for f in sorted(functions, key=lambda f: f.get("fuzzy_match_percent", 100)):
                print(f"  {f.get('fuzzy_match_percent', 0):6.2f}% {int(f.get('size', 0)):5d}  {f['name']}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
