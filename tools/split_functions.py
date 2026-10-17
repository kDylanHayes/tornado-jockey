#!/usr/bin/env python3
"""Give each function of a delinked COFF object its own .text section.

The game was compiled with /Gy, so every function sat in its own section and
the linker padded them to 16 bytes with int3. A delinked target has a single
.text holding all functions back to back with that padding. This rewrites it
the way the compiler produced it:

- one .text section per function, with the trailing int3 padding removed;
- PC-relative relocations within a function (which the compiler resolves
  itself) are applied and dropped; calls between functions keep theirs.

Usage: split_functions.py <in.obj> [<out.obj>]   (default: rewrite in place)
"""

import struct
import sys

IMAGE_SCN_CNT_CODE = 0x20
IMAGE_SCN_CNT_INITIALIZED_DATA = 0x40
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
            table += struct.pack("<IIIIIIHHI", s["vsize"], s["vaddr"], len(raw), rawptr,
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


def split(coff: Coff) -> int:
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
            starts = {struct.unpack_from("<I", rec, 8)[0] for _, rec in syms
                      if struct.unpack_from("<H", rec, 14)[0] == FUNCTION_TYPE}
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
        for _, rec in syms:
            names.setdefault(struct.unpack_from("<I", rec, 8)[0], coff.name(rec))
        bounds = []
        for n, start in enumerate(starts):
            end = starts[n + 1] if n + 1 < len(starts) else len(data)
            body_end = end
            if is_code:
                while body_end > start and data[body_end - 1] == 0xCC:
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


def main_path(src, dst=None) -> int:
    with open(src, "rb") as f:
        coff = Coff(f.read())
    n = split(coff)
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
