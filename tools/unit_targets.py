"""List the still-unmatched target functions of one unit.

usage: py build/unit_targets.py Tongas2/GAMECORE1 [max-size]
"""
import json
import sys

unit = sys.argv[1]
limit = int(sys.argv[2], 0) if len(sys.argv) > 2 else 0x400

# Restrict to the unit's own address ranges: functions.txt names collide across
# the image (every `_$E3` initializer stub is literally called "_$E3"), so a name
# match alone lists functions that belong to other units.
spans = []
for u in json.load(open('config/units.json'))['units']:
    if u['name'] != unit:
        continue
    for r in u.get('ranges', []):
        lo, hi = r.split('-')
        spans.append((int(lo, 16), int(hi, 16)))


def inside(addr):
    return any(lo <= addr <= hi for lo, hi in spans)


report = json.load(open('build/report.json'))
sizes = {}
for u in report['units']:
    if u['name'] != unit:
        continue
    for f in u.get('functions', []):
        n = f.get('name')
        if n:
            sizes[n] = (f.get('fuzzy_match_percent'), int(f.get('size') or 0))

rows = []
for line in open('build/functions.txt'):
    p = line.split()
    if len(p) < 4 or p[2] != 'function':
        continue
    try:
        addr = int(p[0], 16)
        size = int(p[3], 16)
    except ValueError:
        continue
    name = p[1]
    if name not in sizes or not inside(addr):
        continue
    pct, rsize = sizes[name]
    if pct is not None and float(pct) >= 100.0:
        continue
    if size == 0 or size > limit:
        continue
    if name.startswith(('code_', 'LAB_', 'switchD_', 'Catch@', '__ehhandler')):
        continue
    rows.append((size, addr, name, pct))

rows.sort()
print('unmatched in %s: %d (size <= 0x%x)' % (unit, len(rows), limit))
for size, addr, name, pct in rows:
    print('%08x %4d %-28s fuzzy=%s' % (addr, size, name[:28], pct))
