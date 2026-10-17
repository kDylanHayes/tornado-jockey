#!/usr/bin/env python3
"""Find where compiled objects' code sits in orig/Tornado.exe.

For each COFF object, every .text section (or each function, with
--functions) is turned into a byte pattern with relocated bytes wildcarded
and searched for in the exe. Useful for placing library code built from
upstream source:

    python3 tools/find_objects.py build/src/lib/zlib/*.obj

With --functions --write, each function found exactly once inside its
unit's range (config/units.json) is named in config/symbols.txt.

With --functions --layout LO-HI, objects are ordered by where their
functions sit inside that address range and a contiguous range is
suggested for each, ready for config/units.json.
"""

import argparse
import json
import re
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
IMAGE_BASE = 0x400000

# i386 relocation types and the number of bytes each one patches.
RELOC_SIZES = {0x06: 4, 0x07: 4, 0x14: 4, 0x0A: 2, 0x0B: 4}


class Coff:
    def __init__(self, path: Path):
        self.data = data = path.read_bytes()
        nsec, _, symptr, nsym, optsize = struct.unpack_from("<HIIIH", data, 2)
        strtab = symptr + nsym * 18
        self.sections = []
        for i in range(nsec):
            o = 20 + optsize + 40 * i
            name = data[o:o + 8].rstrip(b"\0").decode("latin-1")
            size, ptr, relptr, _, nrel = struct.unpack_from("<IIIIH", data, o + 16)
            flags = struct.unpack_from("<I", data, o + 36)[0]
            relocs = [struct.unpack_from("<IIH", data, relptr + 10 * r) for r in range(nrel)]
            self.sections.append(dict(name=name, size=size, ptr=ptr, flags=flags, relocs=relocs))
        self.symbols = []
        self.by_index = {}
        i = 0
        while i < nsym:
            o = symptr + 18 * i
            raw, value, secnum, typ, cls, naux = struct.unpack_from("<8sIhHBB", data, o)
            if raw[:4] == b"\0\0\0\0":
                off = struct.unpack_from("<I", raw, 4)[0]
                name = data[strtab + off:data.index(b"\0", strtab + off)].decode("latin-1")
            else:
                name = raw.rstrip(b"\0").decode("latin-1")
            sym = dict(name=name, value=value, section=secnum, type=typ, cls=cls, index=i)
            self.symbols.append(sym)
            self.by_index[i] = sym
            i += 1 + naux

    def pattern(self, index: int, start: int = 0, end: int | None = None) -> bytes:
        sec = self.sections[index]
        end = sec["size"] if end is None else end
        code = bytearray(self.data[sec["ptr"]:sec["ptr"] + sec["size"]])
        mask = bytearray(b"\1" * len(code))
        for vaddr, _, typ in sec["relocs"]:
            for k in range(RELOC_SIZES.get(typ, 4)):
                if vaddr + k < len(mask):
                    mask[vaddr + k] = 0
        out = b"".join(re.escape(bytes([c])) if m else b"." for c, m in zip(code[start:end], mask[start:end]))
        return out

    def functions(self, index: int):
        syms = sorted(
            # $L symbols are catch blocks, part of the function before them
            (s for s in self.symbols
             if s["section"] == index + 1 and s["type"] == 0x20 and not s["name"].startswith("$L")),
            key=lambda s: s["value"],
        )
        size = self.sections[index]["size"]
        for n, s in enumerate(syms):
            end = syms[n + 1]["value"] if n + 1 < len(syms) else size
            yield s["name"], s["value"], end


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("objects", nargs="+", type=Path)
    parser.add_argument("--exe", type=Path, default=ROOT / "orig" / "Tornado.exe")
    parser.add_argument("--functions", action="store_true", help="search each function separately")
    parser.add_argument("--write", action="store_true", help="name located functions in config/symbols.txt")
    parser.add_argument("--layout", metavar="LO-HI", help="suggest unit ranges inside this address range")
    args = parser.parse_args()
    layout_lo, layout_hi = (int(x, 16) for x in args.layout.split("-")) if args.layout else (0, 0)
    placed = {}  # object stem -> [(address, size)] of functions found once in the layout range

    ranges = {}
    for unit in json.loads((ROOT / "config" / "units.json").read_text())["units"]:
        spans = [tuple(int(x, 16) for x in r.split("-")) for r in unit["ranges"]]
        ranges[(ROOT / "build" / Path(unit["source"]).with_suffix(".obj")).resolve()] = spans
    names = {}

    exe = args.exe.read_bytes()
    for path in args.objects:
        obj = Coff(path)
        limits = ranges.get(path.resolve())
        # Functions with identical code (e.g. png_write_oFFs and
        # png_write_pHYs) all hit the same addresses; they're named in
        # source order when there are as many hits as functions.
        groups = {}
        for index, sec in enumerate(obj.sections):
            if not sec["name"].startswith(".text") or sec["size"] == 0:
                continue
            if args.functions:
                items = list(obj.functions(index))
            else:
                funcs = [name for name, _, _ in obj.functions(index)]
                items = [(funcs[0] if len(funcs) == 1 else f"{sec['name']} ({len(funcs)} functions)", 0, sec["size"])]
            for name, start, end in items:
                pat = obj.pattern(index, start, end)
                hits = [m.start() for m in re.finditer(pat, exe, re.DOTALL)]
                where = ", ".join(f"0x{IMAGE_BASE + h:08x}" for h in hits[:4]) or "not found"
                more = f" (+{len(hits) - 4} more)" if len(hits) > 4 else ""
                print(f"{path.stem:12s} {name:40s} size 0x{end - start:<5x} {where}{more}")
                in_layout = [IMAGE_BASE + h for h in hits if layout_lo <= IMAGE_BASE + h <= layout_hi]
                # Template instances (std::string...) are COMDATs shared by
                # many objects, so they say nothing about which object sits where.
                if len(in_layout) == 1 and "?$" not in name and "@std@@" not in name:
                    placed.setdefault(path.stem, []).append((in_layout[0], end - start))
                inside = [IMAGE_BASE + h for h in hits if any(a <= IMAGE_BASE + h <= b for a, b in limits or [])]
                if args.functions and inside:
                    groups.setdefault(tuple(inside), []).append(f"{name} function 0x{end - start:x}")
        for inside, entries in groups.items():
            if len(inside) == len(entries):
                for addr, entry in zip(inside, entries):
                    names[addr] = f"0x{addr:08x} {entry}"

    if args.layout and placed:
        order = sorted(placed, key=lambda stem: min(placed[stem]))
        starts = [min(placed[stem])[0] for stem in order]
        last = max(a + size for a, size in placed[order[-1]])
        print("\nSuggested ranges (first function found to the next object's):")
        for stem, start, nxt in zip(order, starts, starts[1:] + [last]):
            print(f"  {stem:12s} 0x{start:08x}-0x{nxt - 1:08x}  ({len(placed[stem])} functions placed)")

    if args.write:
        path = ROOT / "config" / "symbols.txt"
        lines = path.read_text().splitlines()
        header = [l for l in lines if l.startswith("#")]
        body = {int(l.split()[0], 16): l for l in lines if l and not l.startswith("#")}
        body.update(names)
        path.write_text("\n".join(header + [body[a] for a in sorted(body)]) + "\n")
        print(f"Wrote {len(names)} names to config/symbols.txt")
    return 0


if __name__ == "__main__":
    sys.exit(main())
