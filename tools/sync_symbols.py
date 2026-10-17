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

DIR32 = 0x06
AUTO_NAME = re.compile(r"^(?:DAT|s|PTR|FUN|LAB|u|switchdataD)_.*?([0-9a-f]{8})$")


def reloc_names(obj: Coff) -> dict:
    """Map (function name, offset in function) -> referenced symbol name."""
    return {key: (name, is_func) for key, (name, is_func, _, _) in relocs(obj).items()}


def relocs(obj: Coff) -> dict:
    """Map (function name, offset in function) -> (symbol name, is function,
    offset of the relocation in its section)."""
    out = {}
    for index, sec in enumerate(obj.sections):
        if not sec["name"].startswith(".text"):
            continue
        funcs = list(obj.functions(index))
        for vaddr, symidx, rtype in sec["relocs"]:
            for name, start, end in funcs:
                if start <= vaddr < end:
                    sym = obj.by_index[symidx]
                    out[(name, vaddr - start)] = (sym["name"], sym["type"] == 0x20, vaddr, rtype)
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
    # Where each target function sits, to locate relocations in the exe.
    # Static functions can share a name across units, so pick the one
    # inside the unit being synced.
    func_addrs = {}
    for line in (ROOT / "config" / "symbols.txt").read_text().splitlines():
        parts = line.split()
        if len(parts) >= 2 and not line.startswith("#"):
            func_addrs.setdefault(parts[1], []).append(int(parts[0], 16))

    def address_of(func, spans):
        m = AUTO_NAME.match(func)
        if m:
            return int(m.group(1), 16)
        inside = [a for a in func_addrs.get(func, []) if any(lo <= a <= hi for lo, hi in spans)]
        return inside[0] if len(inside) == 1 else None

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
        spans = [tuple(int(x, 16) for x in r.split("-")) for r in unit["ranges"]]
        for key, (tname, _, _, rtype) in target.items():
            func, off = key
            faddr = address_of(func, spans)
            bname, is_func = base.get(key, (None, False))
            m = AUTO_NAME.match(tname)
            if bname and (m or tname == bname) and not bname.startswith("$"):
                addr = int(m.group(1), 16) if m else None
                if rtype == DIR32 and faddr is not None and func in base_code:
                    # The exe holds symbol + addend; our object holds the
                    # addend (crc_table + 0x400, _iob + 0x40 for stderr).
                    value = int.from_bytes(exe_bytes(faddr + off), "little")
                    addr = value - int.from_bytes(base_code[func][off:off + 4], "little", signed=True)
                # A function name on a label inside another function means
                # the code around the reference differs; don't trust it.
                if addr is not None and not (is_func and tname.startswith("LAB_")):
                    found[addr] = (bname, is_func)
            if (bname is None and faddr is not None
                    and base_code.get(func) is not None
                    and len(base_code[func]) == len(target_code.get(func, b""))
                    and base_code[func][off:off + 4] == exe_bytes(faddr + off)):
                norefs.add(faddr + off)

    path = ROOT / "config" / "symbols.txt"
    lines = path.read_text().splitlines()
    header = [l for l in lines if l.startswith("#")]
    body = {int(l.split()[0], 16): l for l in lines if l and not l.startswith("#")}

    # A data name placed where no reference points (an earlier run that
    # ignored addends) is moved to where the references do point. Statics
    # can legitimately share a name (extend_test in jdhuff and jdphuff).
    found_at = {}
    for addr, (name, _) in found.items():
        found_at.setdefault(name, set()).add(addr)
    moved = 0
    for addr, line in list(body.items()):
        parts = line.split()
        if parts[1] in found_at and addr not in found_at[parts[1]] and parts[2:3] == ["data"]:
            del body[addr]
            moved += 1

    added = 0
    for addr, (name, is_func) in found.items():
        if addr in body:
            continue
        body[addr] = f"0x{addr:08x} {name} {'function' if is_func else 'data'}"
        added += 1
    path.write_text("\n".join(header + [body[a] for a in sorted(body)]) + "\n")
    print(f"Added {added} names to config/symbols.txt ({moved} moved)")

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
