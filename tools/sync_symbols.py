#!/usr/bin/env python3
"""Copy names of referenced data and functions from compiled objects to
config/symbols.txt.

When a unit's code matches byte-for-byte, the relocations in our compiled
object and in the delinked target sit at the same offsets. Each target
relocation that points at an auto-named symbol (DAT_00684b60,
s_foo_00665254, FUN_00579047...) is given the name our compiler used, so
that objdiff can pair them. Run tools/delink.py again afterwards.

    python3 tools/sync_symbols.py lib/zlib/inflate
"""

import argparse
import json
import re
import sys
from pathlib import Path

from find_objects import Coff, ROOT

AUTO_NAME = re.compile(r"^(?:DAT|s|PTR|FUN|LAB|u|switchdataD)_.*?([0-9a-f]{8})$")


def reloc_names(obj: Coff) -> dict:
    """Map (function name, offset in function) -> referenced symbol name."""
    out = {}
    for index, sec in enumerate(obj.sections):
        if not sec["name"].startswith(".text"):
            continue
        funcs = list(obj.functions(index))
        for vaddr, symidx, _ in sec["relocs"]:
            for name, start, end in funcs:
                if start <= vaddr < end:
                    sym = obj.by_index[symidx]
                    out[(name, vaddr - start)] = (sym["name"], sym["type"] == 0x20)
                    break
    return out


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("units", nargs="+")
    args = parser.parse_args()

    units = {u["name"]: u for u in json.loads((ROOT / "config" / "units.json").read_text())["units"]}
    found = {}
    for name in args.units:
        unit = units[name]
        base = reloc_names(Coff(ROOT / "build" / Path(unit["source"]).with_suffix(".obj")))
        target = reloc_names(Coff(ROOT / unit["target"]))
        for key, (tname, _) in target.items():
            bname, is_func = base.get(key, (None, False))
            m = AUTO_NAME.match(tname)
            if bname and m and not bname.startswith("$"):
                found[int(m.group(1), 16)] = (bname, is_func)

    # A base reference with an addend (crc_table + 0x400) shows up in the
    # target as a separate auto-named symbol; keep only the lowest address.
    lowest = {}
    for addr, (name, is_func) in sorted(found.items()):
        lowest.setdefault(name, (addr, is_func))
    found = {addr: (name, is_func) for name, (addr, is_func) in lowest.items()}

    path = ROOT / "config" / "symbols.txt"
    lines = path.read_text().splitlines()
    header = [l for l in lines if l.startswith("#")]
    body = {int(l.split()[0], 16): l for l in lines if l and not l.startswith("#")}
    added = 0
    for addr, (name, is_func) in found.items():
        if addr in body:
            continue
        body[addr] = f"0x{addr:08x} {name} {'function' if is_func else 'data'}"
        added += 1
    path.write_text("\n".join(header + [body[a] for a in sorted(body)]) + "\n")
    print(f"Added {added} names to config/symbols.txt")
    return 0


if __name__ == "__main__":
    sys.exit(main())
