#!/usr/bin/env python3
"""Unpack VCToolkitSetup.exe (Visual C++ Toolkit 2003) without running it.

The installer is an InstallShield launcher whose payload files are lightly
obfuscated. This script decodes the toolkit's MSI and extracts it with
msiextract (from msitools), giving a folder with bin/cl.exe and include/.

    python3 tools/extract_vctoolkit.py VCToolkitSetup.exe tools/vc71
"""

import argparse
import shutil
import struct
import subprocess
import sys
import tempfile
from pathlib import Path

MAGIC = b"InstallShield\0"
KEY_MASK = (0x13, 0x35, 0x86, 0x07)
ENTRY_HEADER = 312  # 260-byte name + 52 bytes of fields
TOOLKIT_MSI = b"Microsoft Visual C++ Toolkit 2003.msi"


def decode(data: bytes, name: bytes) -> bytes:
    key = bytes(c ^ KEY_MASK[i % 4] for i, c in enumerate(name))
    # Each byte is nibble-swapped, XORed with the key and inverted. Build a
    # translation table per key byte so this runs at C speed.
    tables = [
        bytes((~(k ^ (((b << 4) | (b >> 4)) & 0xFF))) & 0xFF for b in range(256))
        for k in key
    ]
    out = bytearray(len(data))
    for i, table in enumerate(tables):
        out[i::len(key)] = data[i::len(key)].translate(table)
    return bytes(out)


def entries(exe: bytes, start: int):
    count = struct.unpack_from("<H", exe, start + len(MAGIC))[0]
    offset = start + 0x2E
    for _ in range(count):
        name = exe[offset:offset + 260].split(b"\0")[0]
        size = struct.unpack_from("<I", exe, offset + 268)[0]
        yield name, offset + ENTRY_HEADER, size
        offset += ENTRY_HEADER + size


def find_msi(exe: bytes) -> bytes:
    # The launcher's own code also contains the magic string; the payload
    # header is the occurrence that lists the toolkit MSI.
    start = exe.find(MAGIC)
    while start >= 0:
        try:
            for name, offset, size in entries(exe, start):
                if name == TOOLKIT_MSI:
                    return decode(exe[offset:offset + size], name)
        except struct.error:
            pass
        start = exe.find(MAGIC, start + 1)
    sys.exit("error: toolkit MSI not found in installer")


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("installer", type=Path)
    parser.add_argument("output", type=Path)
    args = parser.parse_args()

    if shutil.which("msiextract") is None:
        sys.exit("error: msiextract not found; install msitools")

    msi = find_msi(args.installer.read_bytes())
    with tempfile.TemporaryDirectory() as tmp:
        msi_path = Path(tmp) / "toolkit.msi"
        msi_path.write_bytes(msi)
        subprocess.run(["msiextract", "-C", tmp, str(msi_path)], check=True, stdout=subprocess.DEVNULL)
        root = Path(tmp) / "Program Files" / "Microsoft Visual C++ Toolkit 2003"
        if args.output.exists():
            sys.exit(f"error: {args.output} already exists")
        shutil.copytree(root, args.output)

    print(f"Extracted to {args.output}; check with: wine {args.output / 'bin' / 'cl.exe'}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
