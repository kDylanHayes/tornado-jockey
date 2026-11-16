"""Gate check: two symbols.txt rows for one address, where the later row wins.

ApplySymbols.java applies rows in file order, so a second row for an address
silently replaces the first. That is how ?UpdateTextures lost its pairing for two
commits: 0x00504c90 carried both the real name (with a size) and a stale
?Update@MUSICX@@QAEXXZ row, and the stale one won.

For every duplicate address this reports which unit's range owns it, which of the
names the owning unit's object actually defines, and whether the name that wins
(file order) is that one.

usage: py build/check_symbols.py [--verbose]
"""
import json
import struct
import sys
from collections import defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / 'tools'))
import split_functions as s


def ranges():
    out = []
    for u in json.loads((ROOT / 'config/units.json').read_text())['units']:
        for r in u.get('ranges', []):
            lo, hi = r.split('-')
            out.append((int(lo, 16), int(hi, 16), u['name'], u.get('source')))
    return out


def owner(addr, rs):
    for lo, hi, name, src in rs:
        if lo <= addr <= hi:
            return name, src
    return None, None


def defined(src):
    obj = ROOT / 'build' / Path(src).with_suffix('.obj')
    if src is None or not obj.is_file():
        return set()
    try:
        c = s.Coff(open(obj, 'rb').read())
    except Exception:
        return set()
    names = set()
    for i, r, aux in c.symbols:
        secno, typ = struct.unpack_from('<hH', r, 12)
        if secno > 0 and typ == 0x20:
            names.add(c.name(r) or '')
    return names


def main():
    verbose = '--verbose' in sys.argv
    rs = ranges()
    rows = defaultdict(list)
    for i, line in enumerate((ROOT / 'config' / 'symbols.txt').read_text().splitlines(), 1):
        f = line.split()
        if len(f) < 3 or line.startswith('#'):
            continue
        try:
            addr = int(f[0], 16)
        except ValueError:
            continue
        rows[addr].append((i, f[1], f[2], f[3] if len(f) > 3 else None))

    dup = {a: v for a, v in rows.items() if len(v) > 1}
    bad = []
    for addr, v in sorted(dup.items()):
        unit, src = owner(addr, rs)
        defs = defined(src) if src else set()
        winner = v[-1][1]
        owns = [n for _, n, _, _ in v if n in defs]
        status = 'ok'
        if owns and winner not in owns:
            status = 'WINS-WRONG'
            bad.append((addr, unit, winner, owns, v))
        elif not owns and unit:
            status = 'no-owner-def'
        if status != 'ok' or verbose:
            print('0x%08x %-28s %-16s winner=%-40s defined=%s' %
                  (addr, unit or '(no unit)', status, winner[:40],
                   ','.join(n[:28] for n in owns) or '-'))
    print('duplicate addresses: %d, wins-wrong: %d' % (len(dup), len(bad)))


if __name__ == '__main__':
    main()
