#!/usr/bin/env python3
"""Copy code relocations our object has that the target object is missing.

The delinker takes relocations from Ghidra's reference table, and a call whose
reference analysis lost can end up with none, which caps the function below 100%
however correct the source is (?UpdateDecal@PROGRESSBAR in Tongas2/GAMECORE3 is
the known case: ours carries ?GetResource@RESOURCES@@... at +0x59, the target
carries nothing).

For every function symbol the target and the base object share by name, this
copies a REL32 relocation the base has at a function offset where the target has
none, provided the target's byte before it is a call or jmp opcode. New relocation
tables are appended to the object and the section headers repointed, so the rest
of the file is untouched.

usage: py tools\\fix_code_relocs.py <unit>
"""

import argparse
import json
import struct
from pathlib import Path

from fix_data_icf import IMAGE_SYM_CLASS_EXTERNAL, IMAGE_SYM_UNDEFINED, Obj, symbol_extent

ROOT = Path(__file__).resolve().parent.parent
IMAGE_REL_I386_REL32 = 0x14
FUNCTION_TYPE = 0x20
CALL_JMP = (0xE8, 0xE9)


def rewrite(target, base) -> int:
    t = Obj(target)
    b = Obj(base)
    original_symtab = bytes(t.data[t.symptr:t.symptr + t.nsym * 18])
    added = []
    added_names = []
    name_index = dict(t.name_of)

    def index_for(name: str) -> int:
        if name in name_index:
            return name_index[name]
        index = t.nsym + sum(len(rec) // 18 for rec in added)
        raw_name = name.encode("latin1")
        if len(raw_name) <= 8:
            name_field = raw_name.ljust(8, b"\0")
        else:
            offset = len(t.strings) + 4 + sum(len(n) + 1 for n in added_names)
            name_field = b"\0\0\0\0" + struct.pack("<I", offset)
            added_names.append(name)
        rec = name_field + struct.pack("<IhHBB", 0, IMAGE_SYM_UNDEFINED, 0,
                                       IMAGE_SYM_CLASS_EXTERNAL, 0)
        added.append(rec)
        name_index[name] = index
        return index

    relocs = {}
    have = set()
    for si, sec in enumerate(t.sections, 1):
        rows = []
        for k in range(sec["nrel"]):
            vaddr, symidx, rtype = struct.unpack_from("<IIH", t.data, sec["relptr"] + k * 10)
            rows.append([vaddr, symidx, rtype])
            have.add((si, vaddr))
        relocs[si] = rows

    changed = 0
    for sym in t.symbols:
        if sym["type"] != FUNCTION_TYPE or not (0 < sym["section"] <= len(t.sections)):
            continue
        bsym = b.by_name.get(sym["name"])
        if bsym is None or not (0 < bsym["section"] <= len(b.sections)):
            continue
        tsec = t.sections[sym["section"] - 1]
        bsec = b.sections[bsym["section"] - 1]
        if not t.is_code(tsec) or not b.is_code(bsec):
            continue
        extent = symbol_extent(b, bsym)
        if extent <= 0:
            continue
        for (bsi, bvaddr), (bname, btype) in b.reloc_at.items():
            if bsi != bsym["section"] or btype != IMAGE_REL_I386_REL32:
                continue
            if not (bsym["value"] <= bvaddr < bsym["value"] + extent):
                continue
            off = bvaddr - bsym["value"]
            tvaddr = sym["value"] + off
            if tvaddr < 1 or tvaddr + 4 > tsec["rsz"]:
                continue
            if (sym["section"], tvaddr) in have:
                continue
            if not bname or bname.startswith("."):
                continue
            if t.data[tsec["rptr"] + tvaddr - 1] not in CALL_JMP:
                continue
            relocs[sym["section"]].append([tvaddr, index_for(bname), IMAGE_REL_I386_REL32])
            have.add((sym["section"], tvaddr))
            # The delinker writes an undefined relocation with a zeroed
            # displacement and lets the linker fill it in; objdiff reads a
            # non-zero displacement as an addend and refuses to pair the call.
            start = tsec["rptr"] + tvaddr
            t.data[start:start + 4] = b"\0\0\0\0"
            changed += 1

    if not changed:
        return 0

    prefix = bytearray(t.data[:t.symptr])
    blob = bytearray()
    for si, sec in enumerate(t.sections, 1):
        rows = relocs[si]
        if len(rows) == sec["nrel"]:
            continue
        rows.sort()
        offset = len(prefix) + len(blob)
        for vaddr, symidx, rtype in rows:
            blob += struct.pack("<IIH", vaddr, symidx, rtype)
        struct.pack_into("<I", prefix, sec["hdr"] + 24, offset)
        struct.pack_into("<H", prefix, sec["hdr"] + 32, len(rows))

    new_symptr = len(prefix) + len(blob)
    new_strings = t.strings + b"".join(n.encode("latin1") + b"\0" for n in added_names)
    strtab = struct.pack("<I", len(new_strings) + 4) + new_strings
    out = bytearray(prefix + blob + original_symtab + b"".join(added) + strtab)
    struct.pack_into("<I", out, 8, new_symptr)
    struct.pack_into("<I", out, 12, t.nsym + len(added))
    target.write_bytes(out)
    return changed


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("units", nargs="+")
    args = parser.parse_args()
    units = json.loads((ROOT / "config" / "units.json").read_text())["units"]
    total = 0
    for name in args.units:
        u = next((x for x in units if x["name"] == name), None)
        if u is None or "source" not in u:
            print(f"{name}: no source object; skipped")
            continue
        target = ROOT / u["target"]
        base = ROOT / "build" / Path(u["source"]).with_suffix(".obj")
        if not target.is_file() or not base.is_file():
            print(f"{name}: missing object; skipped")
            continue
        n = rewrite(target, base)
        print(f"{name}: added {n} code relocation(s)")
        total += n
    print(f"added {total} code relocation(s)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
