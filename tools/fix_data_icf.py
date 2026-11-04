#!/usr/bin/env python3
"""Point target data relocations at the names our base object uses.

The linker folded identical virtual functions in the original executable
(/OPT:ICF), so a target vtable can relocate several slots against one symbol
name while our object still has a distinct name per slot. objdiff compares
data relocations by name and `symbol_mappings` does not apply to data
relocations, so those units could never reach 100% data.

For every data symbol the target and the base object both name (vtables and
similar tables), this walks the target's relocations in that symbol and
rewrites each one to the base object's symbol name at the same offset. Missing
names are added to the target's symbol table as undefined externals; the raw
addend word and relocation type are copied from the base too. COFF is rebuilt
in place: everything before the symbol table keeps its offsets, and only the
symbol table and string table are re-emitted.

Run it after tools/split_functions.py and tools/fix_data_symbols.py (delink.py
does this automatically), or standalone:

    py tools\\fix_data_icf.py Tongas2/SIMPLEMODEL
"""

import argparse
import json
import struct
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent

IMAGE_SCN_CNT_CODE = 0x20
IMAGE_SYM_CLASS_EXTERNAL = 2
IMAGE_SYM_UNDEFINED = 0
IMAGE_REL_I386_DIR32 = 0x06


class Obj:
    """The parts of a COFF object this tool reads."""

    def __init__(self, path: Path):
        self.path = path
        self.data = bytearray(path.read_bytes())
        (self.machine, self.nsec, self.timestamp, self.symptr,
         self.nsym, self.optsz, self.chars) = struct.unpack_from("<HHIIIHH", self.data, 0)
        self.sections = []
        off = 20 + self.optsz
        for _ in range(self.nsec):
            name_raw = bytes(self.data[off:off + 8])
            vsz, va, rsz, rptr, relptr, lnptr, nrel, nline, chars = \
                struct.unpack_from("<IIIIIIHHI", self.data, off + 8)
            self.sections.append({
                "name_raw": name_raw, "name": name_raw.rstrip(b"\0").decode("latin1"),
                "vsize": vsz, "va": va, "rsz": rsz, "rptr": rptr,
                "relptr": relptr, "nrel": nrel, "chars": chars, "hdr": off,
            })
            off += 40
        self.str_off = self.symptr + self.nsym * 18
        self.strings = b""
        if self.str_off + 4 <= len(self.data):
            size = struct.unpack_from("<I", self.data, self.str_off)[0]
            if size >= 4:
                self.strings = bytes(self.data[self.str_off + 4:self.str_off + size])
        self.symbols = []
        self.name_of = {}
        self.by_name = {}
        self.by_index = {}
        i = 0
        while i < self.nsym:
            rec = bytes(self.data[self.symptr + i * 18:self.symptr + (i + 1) * 18])
            raw = rec[:8]
            if raw[:4] == b"\0\0\0\0":
                so = struct.unpack_from("<I", raw, 4)[0]
                end = self.strings.find(b"\0", max(0, so - 4))
                name = self.strings[max(0, so - 4):end].decode("latin1") if end >= 0 else ""
            else:
                name = raw.rstrip(b"\0").decode("latin1")
            value, secno, typ, cls, naux = struct.unpack_from("<IhHBB", rec, 8)
            self.symbols.append({"index": i, "raw": rec, "name": name,
                                 "value": value, "section": secno, "type": typ,
                                 "class": cls, "naux": naux})
            if name:
                self.name_of.setdefault(name, i)
                self.by_name.setdefault(name, self.symbols[-1])
            self.by_index[i] = self.symbols[-1]
            i += 1 + naux
        self.reloc_at = {}  # (section index, vaddr) -> (name, type)
        for si, sec in enumerate(self.sections, 1):
            for k in range(sec["nrel"]):
                vaddr, symidx, rtype = struct.unpack_from(
                    "<IIH", self.data, sec["relptr"] + k * 10)
                sn = self.by_index[symidx]["name"] if symidx in self.by_index else ""
                self.reloc_at[(si, vaddr)] = (sn, rtype)

    def is_code(self, sec) -> bool:
        return bool(sec["chars"] & IMAGE_SCN_CNT_CODE) or sec["name"].startswith(".text")


def symbol_extent(obj: Obj, sym) -> int:
    """A data symbol's byte extent, from its next same-section symbol."""
    sec = obj.sections[sym["section"] - 1]
    later = [s["value"] for s in obj.symbols
             if s["section"] == sym["section"] and s["value"] > sym["value"]
             and s["index"] != sym["index"]]
    end = min(later) if later else sec["rsz"]
    return max(0, end - sym["value"])


def rewrite(target: Path, base: Path) -> int:
    """Rewrite target data relocations to the base object's names."""
    t = Obj(target)
    b = Obj(base)
    original_symtab = bytes(t.data[t.symptr:t.symptr + t.nsym * 18])
    added = []                 # new symbol records
    added_names = []           # strings for the new string table
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

    changed = 0
    for sym in t.symbols:
        name = sym["name"]
        # Primary vtables end in @@6B@; multiple inheritance gives secondary
        # ones such as ??_7SPAWNER@@6BACTORLINK@@@, which also contain @@6B.
        if not (name.startswith("??_7") and "@@6B" in name):
            continue
        if sym["section"] <= 0 or sym["section"] > len(t.sections):
            continue
        bsym = b.by_name.get(sym["name"])
        if bsym is None:
            continue
        tsec = t.sections[sym["section"] - 1]
        if t.is_code(tsec):
            continue
        if bsym["section"] <= 0 or bsym["section"] > len(b.sections):
            continue
        bsec = b.sections[bsym["section"] - 1]
        if b.is_code(bsec):
            continue
        extent = symbol_extent(t, sym)
        if extent <= 0:
            continue
        for k in range(tsec["nrel"]):
            rel = tsec["relptr"] + k * 10
            vaddr, symidx, rtype = struct.unpack_from("<IIH", t.data, rel)
            if not (sym["value"] <= vaddr < sym["value"] + extent):
                continue
            off = vaddr - sym["value"]
            bren = b.reloc_at.get((bsym["section"], bsym["value"] + off))
            if bren is None:
                continue
            bname, btype = bren
            if not bname or bname.startswith("."):
                continue
            new_index = index_for(bname)
            if symidx != new_index or rtype != btype:
                struct.pack_into("<IIH", t.data, rel, vaddr, new_index, btype)
                changed += 1
            # The four-byte addend only means something for a DIR32.
            if btype == IMAGE_REL_I386_DIR32 and vaddr + 4 <= tsec["rsz"] and \
                    bsym["value"] + off + 4 <= bsec["rsz"]:
                t.data[tsec["rptr"] + vaddr:tsec["rptr"] + vaddr + 4] = \
                    b.data[bsec["rptr"] + bsym["value"] + off:
                           bsec["rptr"] + bsym["value"] + off + 4]
        # A dedicated vtable section can carry trailing zero padding past the
        # base object's extent (split_functions kept it with the symbol). Trim
        # it so objdiff pairs the symbols at the same size.
        tsize = symbol_extent(t, sym)
        bsize = symbol_extent(b, bsym)
        only = [s for s in t.symbols if s["section"] == sym["section"]]
        if (bsize and tsize > bsize and sym["value"] == 0 and len(only) == 1
                and tsec["rsz"] == tsize
                and not any(si == sym["section"] and bsize <= v < tsize
                            for si, v in t.reloc_at)
                and not any(t.data[tsec["rptr"] + bsize:tsec["rptr"] + tsize])):
            struct.pack_into("<I", t.data, tsec["hdr"] + 16, bsize)
            if tsec["vsize"]:
                struct.pack_into("<I", t.data, tsec["hdr"] + 8, bsize)
            for s in only:
                if s["class"] == 3 and s["naux"]:
                    struct.pack_into("<I", t.data, t.symptr + (s["index"] + 1) * 18,
                                     bsize)
            tsec["rsz"] = bsize
            changed += 1
    if not changed and not added:
        return 0
    # Rebuild only the symbol table and string table; all earlier offsets stay.
    prefix = bytes(t.data[:t.symptr])
    new_strings = t.strings + b"".join(n.encode("latin1") + b"\0" for n in added_names)
    strtab = struct.pack("<I", len(new_strings) + 4) + new_strings
    out = bytearray(prefix + original_symtab + b"".join(added) + strtab)
    struct.pack_into("<I", out, 12, t.nsym + len(added))
    target.write_bytes(out)
    return changed


def unit_paths(name: str):
    units = json.loads((ROOT / "config" / "units.json").read_text())["units"]
    u = next((x for x in units if x["name"] == name), None)
    if u is None or "source" not in u:
        return None
    base = ROOT / "build" / Path(u["source"]).with_suffix(".obj")
    return ROOT / u["target"], base


def main_path(target: Path, base: Path) -> int:
    if not target.is_file() or not base.is_file():
        return 0
    return rewrite(target, base)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("units", nargs="+", help="unit names from config/units.json")
    args = parser.parse_args()
    total = 0
    for name in args.units:
        paths = unit_paths(name)
        if paths is None:
            print(f"{name}: no source object; skipped")
            continue
        n = main_path(*paths)
        print(f"{name}: rewrote {n} data relocation(s)")
        total += n
    print(f"rewrote {total} data relocation(s)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
