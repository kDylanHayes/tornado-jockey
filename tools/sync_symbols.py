#!/usr/bin/env python3
"""Copy names of referenced data and functions from compiled objects to
config/symbols.txt.

When a unit's code matches byte-for-byte, the relocations in our compiled
object and in the delinked target sit at the same offsets. Each target
relocation that points at an auto-named symbol (DAT_00684b60,
s_foo_00665254, FUN_00579047...) is given the name our compiler used, so
that objdiff can pair them. Target relocations where our object has the
same bytes but no relocation (a flag mask such as 0x600080 that Ghidra
took for an address) are listed in config/norefs.txt so delinking drops
them. Run tools/delink.py again afterwards.

    python3 tools/sync_symbols.py lib/zlib/inflate
"""

import argparse
import json
import re
import sys
from pathlib import Path

from find_objects import Coff, IMAGE_BASE, ROOT

AUTO_NAME = re.compile(r"^(?:DAT|s|PTR|FUN|LAB|u|switchdataD)_.*?([0-9a-f]{8})$")


def reloc_names(obj: Coff) -> dict:
    """Map (function name, offset in function) -> referenced symbol name."""
    return {key: (name, is_func) for key, (name, is_func, _) in relocs(obj).items()}


def relocs(obj: Coff) -> dict:
    """Map (function name, offset in function) -> (symbol name, is function,
    offset of the relocation in its section)."""
    out = {}
    for index, sec in enumerate(obj.sections):
        if not sec["name"].startswith(".text"):
            continue
        funcs = list(obj.functions(index))
        for vaddr, symidx, _ in sec["relocs"]:
            for name, start, end in funcs:
                if start <= vaddr < end:
                    sym = obj.by_index[symidx]
                    out[(name, vaddr - start)] = (sym["name"], sym["type"] == 0x20, vaddr)
                    break
    return out


def function_bytes(obj: Coff) -> dict:
    out = {}
    for index, sec in enumerate(obj.sections):
        if sec["name"].startswith(".text"):
            for name, start, end in obj.functions(index):
                out[name] = obj.data[sec["ptr"] + start:sec["ptr"] + end]
    return out


_exe = None


def exe_bytes(addr: int) -> bytes:
    """Four bytes at a virtual address of orig/Tornado.exe."""
    global _exe
    if _exe is None:
        _exe = (ROOT / "orig" / "Tornado.exe").read_bytes()
    # Sections are file-aligned at their virtual addresses (checked by
    # tools/find_objects.py, which searches the file directly).
    off = addr - IMAGE_BASE
    return _exe[off:off + 4]


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("units", nargs="+")
    args = parser.parse_args()

    units = {u["name"]: u for u in json.loads((ROOT / "config" / "units.json").read_text())["units"]}
    found = {}
    norefs = set()
    for name in args.units:
        unit = units[name]
        base_obj = Coff(ROOT / "build" / Path(unit["source"]).with_suffix(".obj"))
        target_obj = Coff(ROOT / unit["target"])
        base = reloc_names(base_obj)
        target = relocs(target_obj)
        base_code = function_bytes(base_obj)
        target_code = function_bytes(target_obj)
        unit_start = int(unit["ranges"][0].split("-")[0], 16)
        for key, (tname, _, vaddr) in target.items():
            bname, is_func = base.get(key, (None, False))
            m = AUTO_NAME.match(tname)
            if bname and m and not bname.startswith("$"):
                found[int(m.group(1), 16)] = (bname, is_func)
            func, off = key
            if (bname is None and len(unit["ranges"]) == 1
                    and base_code.get(func) is not None
                    and len(base_code[func]) == len(target_code.get(func, b""))
                    and base_code[func][off:off + 4] == exe_bytes(unit_start + vaddr)):
                norefs.add(unit_start + vaddr)

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
    # A name that's already placed is never added again: a reference with
    # an addend (z_errmsg + 8) would otherwise create a second copy.
    known = {l.split()[1] for l in body.values()}
    added = 0
    for addr, (name, is_func) in found.items():
        if addr in body or name in known:
            continue
        body[addr] = f"0x{addr:08x} {name} {'function' if is_func else 'data'}"
        added += 1
    path.write_text("\n".join(header + [body[a] for a in sorted(body)]) + "\n")
    print(f"Added {added} names to config/symbols.txt")

    path = ROOT / "config" / "norefs.txt"
    lines = path.read_text().splitlines() if path.exists() else [
        "# Locations of constants that are not addresses; see tools/ghidra/FixReferences.java."]
    header = [l for l in lines if l.startswith("#")]
    known = {int(l.split()[0], 16) for l in lines if l and not l.startswith("#")}
    path.write_text("\n".join(header + [f"0x{a:08x}" for a in sorted(known | norefs)]) + "\n")
    print(f"Added {len(norefs - known)} locations to config/norefs.txt")
    return 0


if __name__ == "__main__":
    sys.exit(main())
