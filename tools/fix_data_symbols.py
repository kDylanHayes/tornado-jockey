#!/usr/bin/env python3
"""Tidy data symbols in a delinked object (run by tools/delink.py).

- Analysis leaves DAT_ labels inside variables named in config/symbols.txt
  (s_line + 1 in FILELOADER, the high half of a LARGE_INTEGER). objdiff
  would cut the variable at them, so they get a LAB_ prefix, which objdiff
  skips, and references to them move onto the variable plus an offset.
- Code that indexes just before a variable (`lea eax, [eax + s_line - 1]`
  after a strlen) refers to an address the exe has no symbol for, so the
  delinker relocates it against an undefined DAT_<address>. When a named
  variable of this object starts at most 4 bytes after that address, the
  relocation is moved onto the variable with the difference as addend, the
  way the compiler wrote it.
- A reference into another unit's variable past its start (the y of a V3)
  is relocated against an undefined DAT_<address> too; when symbols.txt
  gives that variable's size, the relocation moves onto it plus an offset.
"""

import re
import struct

import split_functions as split

MAX_BEFORE = 4
IMAGE_REL_I386_DIR32 = 0x06
IMAGE_SCN_CNT_INITIALIZED_DATA = 0x40
IMAGE_SCN_CNT_UNINITIALIZED_DATA = 0x80
IMAGE_SYM_CLASS_EXTERNAL = 2


def fix(coff: split.Coff, variables: dict, externals=()) -> int:
    """variables: name -> exe address of the unit's data symbols.
    externals: (name, address, size) of other units' sized data symbols."""
    by_index = {i: rec for i, rec, _ in coff.symbols}
    defined = {}
    for i, rec, _ in coff.symbols:
        name = coff.name(rec)
        if struct.unpack_from("<h", rec, 12)[0] > 0 and name in variables:
            defined[variables[name]] = i
    changed = 0
    # Defined DAT_ labels inside a variable: (index) -> (variable index, offset).
    inside = {}
    starts = {}
    for i, rec, _ in coff.symbols:
        secno = struct.unpack_from("<h", rec, 12)[0]
        if i in defined.values():
            starts.setdefault(secno, []).append((struct.unpack_from("<I", rec, 8)[0], i))
    for i, rec, _ in coff.symbols:
        name = coff.name(rec)
        secno = struct.unpack_from("<h", rec, 12)[0]
        if secno > 0 and name.startswith("DAT_") and not coff.sections[secno - 1]["chars"] & split.IMAGE_SCN_CNT_CODE:
            value = struct.unpack_from("<I", rec, 8)[0]
            owner = max((s for s in starts.get(secno, []) if s[0] <= value), default=None)
            if owner is not None:
                inside[i] = (owner[1], value - owner[0])
            coff.set_name(rec, "LAB_" + name)
            changed += 1
    for sec in coff.sections:
        if (sec["name"].rstrip(b"\0") == b".data" and sec["data"] and not sec["relocs"]
                and not any(sec["data"])):
            sec["bss"] = len(sec["data"])
            sec["data"] = b""
            sec["name"] = b".bss"
            sec["chars"] = (sec["chars"] & ~IMAGE_SCN_CNT_INITIALIZED_DATA) | IMAGE_SCN_CNT_UNINITIALIZED_DATA
            changed += 1
        for reloc in sec["relocs"]:
            vaddr, symidx, rtype = reloc
            if rtype != IMAGE_REL_I386_DIR32:
                continue
            if symidx in inside:
                # A reference to the high half of a LARGE_INTEGER, say.
                reloc[1], offset = inside[symidx]
                value = struct.unpack_from("<i", sec["data"], vaddr)[0]
                struct.pack_into("<i", sec["data"], vaddr, value + offset)
                changed += 1
                continue
            rec = by_index[symidx]
            m = re.fullmatch(r"DAT_([0-9a-f]{8})", coff.name(rec))
            if not m or struct.unpack_from("<h", rec, 12)[0] != 0:
                continue
            addr = int(m[1], 16)
            owner = next(((n, a) for n, a, size in externals if a < addr < a + size), None)
            if owner is not None:
                index = next((i for i, r, _ in coff.symbols if coff.name(r) == owner[0]), None)
                if index is None:
                    index = coff.add_symbol(owner[0], 0, 0, 0, IMAGE_SYM_CLASS_EXTERNAL)
                    by_index[index] = coff.symbols[-1][1]
                reloc[1] = index
                value = struct.unpack_from("<i", sec["data"], vaddr)[0]
                struct.pack_into("<i", sec["data"], vaddr, value + addr - owner[1])
                changed += 1
                continue
            for back in range(1, MAX_BEFORE + 1):
                if addr + back in defined:
                    reloc[1] = defined[addr + back]
                    value = struct.unpack_from("<i", sec["data"], vaddr)[0]
                    struct.pack_into("<i", sec["data"], vaddr, value - back)
                    changed += 1
                    break
    vector_dtors = {}
    for sec in coff.sections:
        if sec["chars"] & split.IMAGE_SCN_CNT_CODE:
            continue
        for reloc in sec["relocs"]:
            name = coff.name(by_index[reloc[1]])
            if name.startswith("??_G"):
                if name not in vector_dtors:
                    vector_dtors[name] = coff.add_symbol("??_E" + name[4:], 0, 0, split.FUNCTION_TYPE,
                                                         IMAGE_SYM_CLASS_EXTERNAL)
                reloc[1] = vector_dtors[name]
                changed += 1
    return changed


def main_path(path, variables: dict, externals=()) -> int:
    with open(path, "rb") as f:
        coff = split.Coff(f.read())
    n = fix(coff, variables, externals)
    if n:
        with open(path, "wb") as f:
            f.write(coff.write())
    return n
