#!/usr/bin/env python3
"""Unpack VCToolkitSetup.exe (Visual C++ Toolkit 2003) without running it.

The installer is an InstallShield launcher whose payload files are lightly
obfuscated. This script decodes the toolkit's MSI and extracts it, giving a
folder with bin/cl.exe and include/.

On Linux and macOS extraction uses msiextract (from msitools). On Windows it
uses 7-Zip to unpack the MSI's embedded cabinet and the Windows Installer API
(read-only) to map the cabinet's opaque member names back to install paths.
Nothing from the package is executed.

    python3 tools/extract_vctoolkit.py VCToolkitSetup.exe tools/vc71
"""

import argparse
import ctypes
import ctypes.wintypes
import os
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
TOOLKIT_DIR = "Microsoft Visual C++ Toolkit 2003"


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


def find_7zip():
    for name in ("7z", "7za"):
        found = shutil.which(name)
        if found:
            return found
    for variable, default in (("ProgramFiles", r"C:\Program Files"),
                              ("ProgramFiles(x86)", r"C:\Program Files (x86)")):
        candidate = Path(os.environ.get(variable, default)) / "7-Zip" / "7z.exe"
        if candidate.is_file():
            return str(candidate)
    return None


def run_7zip(seven: str, archive: Path, destination: Path) -> None:
    subprocess.run([seven, "x", str(archive), f"-o{destination}", "-y"],
                   check=True, stdout=subprocess.DEVNULL)


def _record_string(msi, record, field: int) -> str:
    size = ctypes.wintypes.DWORD(0)
    msi.MsiRecordGetStringW(record, field, None, ctypes.byref(size))
    # The reported size excludes the terminator; make room for it.
    needed = ctypes.wintypes.DWORD(size.value + 1)
    buffer = ctypes.create_unicode_buffer(needed.value)
    msi.MsiRecordGetStringW(record, field, buffer, ctypes.byref(needed))
    return buffer.value


def _query(msi, database, sql: str):
    """Return the query's rows as lists of the first three string fields."""
    view = ctypes.c_uint(0)
    if msi.MsiDatabaseOpenViewW(database, sql, ctypes.byref(view)):
        sys.exit(f"error: MSI query failed: {sql}")
    try:
        msi.MsiViewExecute(view, 0)
        rows = []
        while True:
            record = ctypes.c_uint(0)
            result = msi.MsiViewFetch(view, ctypes.byref(record))
            if result == 259:  # ERROR_NO_MORE_ITEMS
                break
            if result:
                sys.exit(f"error: MSI fetch failed: {sql}")
            try:
                rows.append([_record_string(msi, record, field) for field in (1, 2, 3)])
            finally:
                msi.MsiCloseHandle(record)
        return rows
    finally:
        msi.MsiCloseHandle(view)


def msi_file_map(msi_path: Path):
    """Map cabinet member names to install paths via the MSI's tables.

    Returned as (member name, relative folder, file name) tuples. The database
    is opened read-only; no installer actions run.
    """
    msi = ctypes.WinDLL("msi")
    msi.MsiOpenDatabaseW.argtypes = [
        ctypes.wintypes.LPCWSTR, ctypes.wintypes.LPCWSTR,
        ctypes.POINTER(ctypes.c_uint),
    ]
    msi.MsiDatabaseOpenViewW.argtypes = [
        ctypes.c_uint, ctypes.wintypes.LPCWSTR, ctypes.POINTER(ctypes.c_uint),
    ]
    msi.MsiViewExecute.argtypes = [ctypes.c_uint, ctypes.c_uint]
    msi.MsiViewFetch.argtypes = [ctypes.c_uint, ctypes.POINTER(ctypes.c_uint)]
    msi.MsiRecordGetStringW.argtypes = [
        ctypes.c_uint, ctypes.c_uint, ctypes.wintypes.LPWSTR,
        ctypes.POINTER(ctypes.wintypes.DWORD),
    ]
    msi.MsiCloseHandle.argtypes = [ctypes.c_uint]

    database = ctypes.c_uint(0)
    if msi.MsiOpenDatabaseW(str(msi_path), None, ctypes.byref(database)):
        sys.exit(f"error: cannot open MSI database {msi_path}")
    try:
        directories = {
            directory: (parent, default_dir)
            for directory, parent, default_dir
            in _query(msi, database, "SELECT Directory, Directory_Parent, DefaultDir FROM Directory")
        }
        components = {
            component: directory
            for component, _, directory
            in _query(msi, database, "SELECT Component, ComponentId, Directory_ FROM Component")
        }

        def target_name(default_dir: str) -> str:
            target = default_dir.split(":", 1)[0]
            if target in ("", "."):
                return ""
            return target.split("|", 1)[1] if "|" in target else target

        def relative_path(key: str) -> str:
            parts = []
            seen = 0
            while key and key not in ("INSTALLDIR", "TARGETDIR") and key in directories and seen < 32:
                parent, default_dir = directories[key]
                name = target_name(default_dir)
                if name:
                    parts.insert(0, name)
                key = parent
                seen += 1
            return "\\".join(parts)

        rows = []
        for member, file_name, component in _query(msi, database, "SELECT File, FileName, Component_ FROM File"):
            if "|" in file_name:
                file_name = file_name.split("|", 1)[1]
            rows.append((member, relative_path(components[component]), file_name))
        return rows
    finally:
        msi.MsiCloseHandle(database)


def extract_with_7zip(msi_path: Path, tmp: Path, seven: str) -> Path:
    streams = tmp / "streams"
    run_7zip(seven, msi_path, streams)
    cabinets = sorted(streams.glob("*.cab"), key=lambda p: p.stat().st_size, reverse=True)
    if not cabinets:
        sys.exit("error: no cabinet found in the toolkit MSI")
    members = tmp / "cab"
    run_7zip(seven, cabinets[0], members)

    root = tmp / TOOLKIT_DIR
    missing = 0
    for member, relative, file_name in msi_file_map(msi_path):
        source = members / member
        if not source.is_file():
            print(f"warning: cab member {member} missing", file=sys.stderr)
            missing += 1
            continue
        destination = root / relative if relative else root
        destination.mkdir(parents=True, exist_ok=True)
        shutil.copy2(source, destination / file_name)
    if not (root / "bin" / "cl.exe").is_file():
        sys.exit("error: extraction did not produce bin/cl.exe")
    return root


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("installer", type=Path)
    parser.add_argument("output", type=Path)
    args = parser.parse_args()

    msi = find_msi(args.installer.read_bytes())
    if args.output.exists():
        sys.exit(f"error: {args.output} already exists")
    with tempfile.TemporaryDirectory(ignore_cleanup_errors=True) as tmp:
        msi_path = Path(tmp) / "toolkit.msi"
        msi_path.write_bytes(msi)
        if shutil.which("msiextract"):
            subprocess.run(["msiextract", "-C", tmp, str(msi_path)], check=True, stdout=subprocess.DEVNULL)
            root = Path(tmp) / "Program Files" / TOOLKIT_DIR
        elif os.name == "nt":
            seven = find_7zip()
            if seven is None:
                sys.exit("error: 7-Zip not found; install it or extract the MSI by hand")
            root = extract_with_7zip(msi_path, Path(tmp), seven)
        else:
            sys.exit("error: msiextract not found; install msitools")
        if not root.is_dir():
            sys.exit(f"error: extraction produced no {TOOLKIT_DIR} folder")
        shutil.copytree(root, args.output)

    cl = args.output / "bin" / "cl.exe"
    print(f"Extracted to {args.output}; check with: {cl}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
