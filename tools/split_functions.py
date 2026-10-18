#!/usr/bin/env python3
"""Give each function of a delinked COFF object its own .text section.

The game was compiled with /Gy, so every function sat in its own section and
the linker padded them to 16 bytes with int3. A delinked target has a single
.text holding all functions back to back with that padding. This rewrites it
the way the compiler produced it:

- one .text section per function, with the trailing int3 padding removed
  (when the exe address is known, only before a 16-byte boundary: code
  built without /Gy, like the C libraries, has no padding, and an int3
  after a call that never returns is real code);
- PC-relative relocations within a function (which the compiler resolves
  itself) are applied and dropped; calls between functions keep theirs;
- catch blocks stay inside their function, and labels the compiler named
  ($L..., catch blocks and their continuations) take our object's names;
- fs:[0] accesses get the __except_list relocation the compiler emits.

Usage: split_functions.py <in.obj> [<out.obj>]   (default: rewrite in place)
"""

import struct
import sys

IMAGE_SCN_CNT_CODE = 0x20
IMAGE_SCN_CNT_INITIALIZED_DATA = 0x40
IMAGE_REL_I386_DIR32 = 0x06
IMAGE_REL_I386_REL32 = 0x14
IMAGE_SYM_CLASS_EXTERNAL = 2
IMAGE_SYM_CLASS_STATIC = 3
FUNCTION_TYPE = 0x20


class Coff:
    def __init__(self, data: bytes):
        (self.machine, nsec, self.timestamp, symptr, nsym, optsize,
         self.flags) = struct.unpack_from("<HHIIIHH", data, 0)
        if optsize:
            raise ValueError("object files have no optional header")
        self.sections = []
        for i in range(nsec):
            o = 20 + 40 * i
            name = data[o:o + 8]
            (vsize, vaddr, rawsize, rawptr, relptr, _lnptr, nrel, _nln,
             chars) = struct.unpack_from("<IIIIIIHHI", data, o + 8)
            relocs = [struct.unpack_from("<IIH", data, relptr + 10 * r) for r in range(nrel)]
            self.sections.append({
                "name": name, "vsize": vsize, "vaddr": vaddr, "chars": chars,
                "data": bytearray(data[rawptr:rawptr + rawsize]) if rawptr else b"",
                "bss": 0 if rawptr else rawsize,  # size of an uninitialized section
                "relocs": [list(r) for r in relocs],
            })
        # Symbols as raw 18-byte records; aux records stay attached to their
        # primary so indices don't move.
        self.symbols = []
        i = 0
        while i < nsym:
            o = symptr + 18 * i
            rec = bytearray(data[o:o + 18])
            naux = rec[17]
            aux = [bytearray(data[o + 18 * (k + 1):o + 18 * (k + 2)]) for k in range(naux)]
            self.symbols.append((i, rec, aux))
            i += 1 + naux
        strptr = symptr + 18 * nsym
        self.strtab = data[strptr:]

    def name(self, rec) -> str:
        if rec[:4] == b"\0\0\0\0":
            off = struct.unpack_from("<I", rec, 4)[0]
            end = self.strtab.index(b"\0", off)
            return self.strtab[off:end].decode("latin-1")
        return rec[:8].rstrip(b"\0").decode("latin-1")

    def set_name(self, rec, name: str) -> None:
        raw = name.encode("latin-1")
        if len(raw) <= 8:
            rec[:8] = raw.ljust(8, b"\0")
            return
        if not self.strtab:
            self.strtab = struct.pack("<I", 4)
        off = len(self.strtab)
        self.strtab = self.strtab + raw + b"\0"
        self.strtab = struct.pack("<I", len(self.strtab)) + self.strtab[4:]
        rec[:8] = struct.pack("<II", 0, off)

    def add_symbol(self, name: str, value: int, secno: int, typ: int, sclass: int) -> int:
        index = sum(1 + len(aux) for _, _, aux in self.symbols)
        rec = bytearray(18)
        self.set_name(rec, name)
        struct.pack_into("<IhHBB", rec, 8, value, secno, typ, sclass, 0)
        self.symbols.append((index, rec, []))
        return index

    def write(self) -> bytes:
        nsec = len(self.sections)
        header_size = 20 + 40 * nsec
        body = bytearray()
        table = bytearray()
        pos = header_size
        for s in self.sections:
            raw = bytes(s["data"])
            rawptr = pos if raw else 0
            body += raw
            pos += len(raw)
            relptr = pos if s["relocs"] else 0
            for r in s["relocs"]:
                body += struct.pack("<IIH", *r)
            pos += 10 * len(s["relocs"])
            table += s["name"].ljust(8, b"\0")[:8]
            table += struct.pack("<IIIIIIHHI", s["vsize"], s["vaddr"], len(raw) or s.get("bss", 0), rawptr,
                                 relptr, 0, len(s["relocs"]), 0, s["chars"])
        symtab = bytearray()
        nsym = 0
        for _, rec, aux in self.symbols:
            symtab += rec
            for a in aux:
                symtab += a
            nsym += 1 + len(aux)
        header = struct.pack("<HHIIIHH", self.machine, nsec, self.timestamp, pos, nsym, 0, self.flags)
        return bytes(header + table + body + symtab + self.strtab)


def string_literal_size(name: str):
    """Size in bytes of an MSVC string literal symbol (??_C@_0CP@...@), or None."""
    if not name.startswith("??_C@_") or len(name) < 8:
        return None
    enc = name[7:]
    if enc[0].isdigit():
        return int(enc[0]) + 1
    value = 0
    for c in enc:
        if c == "@":
            return value
        if not "A" <= c <= "P":
            return None
        value = value * 16 + ord(c) - ord("A")
    return None


def is_catch_block(name: str) -> bool:
    return name.startswith("$L") or name.startswith("Catch@")


def add_except_list(coff: Coff) -> int:
    """Relocate fs:[0] accesses to __except_list, as the compiler emits them.

    The exe has a plain 0 there, so the delinker can't know about it."""
    index = None
    added = 0
    by_index = {i: rec for i, rec, _ in coff.symbols}
    for sec in coff.sections:
        if not sec["chars"] & IMAGE_SCN_CNT_CODE:
            continue
        data = sec["data"]
        # The delinker relocates any absolute 0 to __except_list (it sits at
        # address 0 in symbols.txt); keep that only on fs: accesses, so a
        # deliberate write to address 0 (ODE's fastldlt.c) stays plain.
        sec["relocs"] = [r for r in sec["relocs"]
                         if r[1] not in by_index or coff.name(by_index[r[1]]) != "__except_list"
                         or 0x64 in data[max(r[0] - 3, 0):r[0] - 1]]
        taken = {r[0] for r in sec["relocs"]}
        for i in range(len(data) - 5):
            if data[i] != 0x64:
                continue
            op = data[i + 1]
            if op in (0xA1, 0xA3):
                disp = i + 2
            elif op in (0x89, 0x8B) and i + 2 < len(data) and data[i + 2] & 0xC7 == 0x05:
                disp = i + 3
            else:
                continue
            if disp + 4 > len(data) or data[disp:disp + 4] != b"\0\0\0\0":
                continue
            if any(t <= disp + 3 and disp <= t + 3 for t in taken):
                continue
            if index is None:
                index = coff.add_symbol("__except_list", 0, 0, 0, IMAGE_SYM_CLASS_EXTERNAL)
            sec["relocs"].append([disp, index, IMAGE_REL_I386_DIR32])
            taken.add(disp)
            added += 1
        sec["relocs"].sort(key=lambda r: r[0])
    return added


def local_labels(coff: Coff, ours: bool) -> dict:
    """(owning function, offset) -> static label record in code.

    In our objects only the compiler's $L labels (catch blocks, their
    continuations, jump tables); in a target, any static label."""
    by_section = {}
    for _, rec, _ in coff.symbols:
        secno = struct.unpack_from("<h", rec, 12)[0]
        if secno > 0 and coff.sections[secno - 1]["chars"] & IMAGE_SCN_CNT_CODE:
            by_section.setdefault(secno, []).append(rec)
    out = {}
    for recs in by_section.values():
        heads = [r for r in recs if struct.unpack_from("<I", r, 8)[0] == 0
                 and struct.unpack_from("<H", r, 14)[0] == FUNCTION_TYPE
                 and not is_catch_block(coff.name(r))]
        if not heads:
            continue
        owner = coff.name(heads[0])
        for r in recs:
            name = coff.name(r)
            if r is heads[0] or r[16] != IMAGE_SYM_CLASS_STATIC or name.startswith("."):
                continue
            if ours and not name.startswith("$L"):
                continue
            out.setdefault((owner, struct.unpack_from("<I", r, 8)[0]), r)
    return out


def name_local_labels(coff: Coff, base: Coff) -> int:
    """Give Ghidra's labels (Catch@..., DAT_...) the compiler's $L names
    from our object, so objdiff sees the same functions on both sides."""
    ours = {key: rec for key, rec in local_labels(base, True).items()}
    renamed = 0
    for key, rec in local_labels(coff, False).items():
        if key in ours and coff.name(rec) != base.name(ours[key]):
            coff.set_name(rec, base.name(ours[key]))
            struct.pack_into("<H", rec, 14, struct.unpack_from("<H", ours[key], 14)[0])
            renamed += 1
    return renamed


def split(coff: Coff, base: int | None = None, sizes: dict | None = None) -> int:
    """sizes: known function sizes by name; they decide where padding starts."""
    by_index = {i: (rec, aux) for i, rec, aux in coff.symbols}
    split_count = 0
    for secno in range(1, len(coff.sections) + 1):
        sec = coff.sections[secno - 1]
        is_code = bool(sec["chars"] & IMAGE_SCN_CNT_CODE)
        if not is_code and not sec["chars"] & IMAGE_SCN_CNT_INITIALIZED_DATA:
            continue
        syms = [(i, rec) for i, rec, _ in coff.symbols
                if struct.unpack_from("<h", rec, 12)[0] == secno]
        if is_code:
            # Functions; labels inside them stay where they are.
            # Catch blocks ($L... in our objects, Catch@... from Ghidra) are
            # static function symbols inside the function that owns them.
            starts = {struct.unpack_from("<I", rec, 8)[0] for _, rec in syms
                      if struct.unpack_from("<H", rec, 14)[0] == FUNCTION_TYPE
                      and not is_catch_block(coff.name(rec))}
        else:
            starts = {struct.unpack_from("<I", rec, 8)[0] for _, rec in syms
                      if rec[16] == IMAGE_SYM_CLASS_EXTERNAL}
        starts = sorted(starts)
        if not starts:
            continue
        if starts[0] != 0:
            raise ValueError(f"section {secno} has contents before its first symbol")
        data = sec["data"]
        names = {}
        # Prefer a function's name to the section symbol at the same offset.
        for _, rec in sorted(syms, key=lambda ir: struct.unpack_from("<H", ir[1], 14)[0] != FUNCTION_TYPE):
            names.setdefault(struct.unpack_from("<I", rec, 8)[0], coff.name(rec))
        bounds = []
        for n, start in enumerate(starts):
            end = starts[n + 1] if n + 1 < len(starts) else len(data)
            body_end = end
            known = sizes.get(names.get(start, "")) if sizes else None
            if is_code and known is not None and start + known <= end:
                body_end = start + known
            elif is_code and (base is None or (base + end) % 16 == 0):
                while body_end > start and end - body_end < 15 and data[body_end - 1] == 0xCC:
                    body_end -= 1
            else:
                size = string_literal_size(names.get(start, ""))
                if size is not None and start + size <= end:
                    body_end = start + size
            bounds.append((start, end, body_end))

        def owner(offset):
            for n, (start, end, _) in enumerate(bounds):
                if start <= offset < end:
                    return n
            raise ValueError(f"offset {offset:#x} outside any function")

        # Section number for each function: the first keeps the original slot.
        numbers = [secno] + [len(coff.sections) + k + 1 for k in range(len(bounds) - 1)]
        relocs = [[] for _ in bounds]
        for vaddr, symidx, rtype in sec["relocs"]:
            n = owner(vaddr)
            rec, _ = by_index[symidx]
            tsec = struct.unpack_from("<h", rec, 12)[0]
            tval = struct.unpack_from("<I", rec, 8)[0]
            if rtype == IMAGE_REL_I386_REL32 and tsec == secno and owner(tval) == n:
                addend = struct.unpack_from("<i", data, vaddr)[0]
                struct.pack_into("<i", data, vaddr, tval + addend - (vaddr + 4))
                continue
            relocs[n].append([vaddr - bounds[n][0], symidx, rtype])

        for _, rec in syms:
            value = struct.unpack_from("<I", rec, 8)[0]
            sclass = rec[16]
            if sclass == IMAGE_SYM_CLASS_STATIC and value == 0 and coff.name(rec) == sec["name"].rstrip(b"\0").decode():
                continue  # the section symbol; fixed up below
            n = owner(value)
            struct.pack_into("<I", rec, 8, value - bounds[n][0])
            struct.pack_into("<h", rec, 12, numbers[n])

        new_sections = []
        for n, (start, _, body_end) in enumerate(bounds):
            new_sections.append({
                "name": sec["name"], "vsize": 0, "vaddr": 0, "chars": sec["chars"],
                "data": bytearray(data[start:body_end]), "relocs": relocs[n],
            })
        coff.sections[secno - 1] = new_sections[0]
        coff.sections += new_sections[1:]
        # Keep the section symbol's aux record in step with its section.
        for _, rec, aux in coff.symbols:
            if struct.unpack_from("<h", rec, 12)[0] == secno and rec[16] == IMAGE_SYM_CLASS_STATIC and aux \
                    and coff.name(rec) == sec["name"].rstrip(b"\0").decode():
                struct.pack_into("<IH", aux[0], 0, len(new_sections[0]["data"]), len(relocs[0]))
        split_count += len(bounds)
    return split_count


def main_path(src, dst=None, base=None, ours=None, sizes=None) -> int:
    """base: exe address of the start of the object's code, if known.
    ours: our compiled object, to take $L label names from."""
    with open(src, "rb") as f:
        coff = Coff(f.read())
    n = split(coff, base, sizes)
    add_except_list(coff)
    if ours is not None and ours.is_file():
        with open(ours, "rb") as f:
            name_local_labels(coff, Coff(f.read()))
    # objdiff treats any other label in code as the start of a symbol (and
    # hangs on one in the middle of a jump table, "PTR_caseD_2_0055cd38+3");
    # it skips LAB_ labels.
    for _, rec, _ in coff.symbols:
        secno = struct.unpack_from("<h", rec, 12)[0]
        name = coff.name(rec)
        if (rec[16] == IMAGE_SYM_CLASS_STATIC and secno > 0
                and coff.sections[secno - 1]["chars"] & IMAGE_SCN_CNT_CODE
                and struct.unpack_from("<H", rec, 14)[0] != FUNCTION_TYPE
                and not name.startswith(("$L", "LAB_", "."))):
            coff.set_name(rec, "LAB_" + name.replace("+", "_"))
    with open(dst or src, "wb") as f:
        f.write(coff.write())
    return n


def main() -> int:
    if len(sys.argv) not in (2, 3):
        print(__doc__, file=sys.stderr)
        return 2
    dst = sys.argv[2] if len(sys.argv) == 3 else sys.argv[1]
    n = main_path(sys.argv[1], dst)
    print(f"{dst}: {n} functions in their own sections")
    return 0


if __name__ == "__main__":
    sys.exit(main())
