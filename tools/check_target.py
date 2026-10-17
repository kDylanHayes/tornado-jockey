#!/usr/bin/env python3
"""Verify that orig/Tornado.exe is the expected target build."""

import hashlib
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent


def main() -> int:
    target = json.loads((ROOT / "config" / "target.json").read_text())
    exe = ROOT / "orig" / target["name"]
    if not exe.is_file():
        print(f"error: {exe.relative_to(ROOT)} not found; see orig/README.md", file=sys.stderr)
        return 1

    data = exe.read_bytes()
    sha1 = hashlib.sha1(data).hexdigest()
    if sha1 == target["sha1"]:
        print(f"OK: {target['name']} {target['version']} ({sha1})")
        return 0

    print(f"error: SHA-1 mismatch for {exe.relative_to(ROOT)}", file=sys.stderr)
    print(f"  expected {target['sha1']}", file=sys.stderr)
    print(f"  got      {sha1}", file=sys.stderr)
    if len(data) == target["size"] and not any(data[0x1000:0x2000]):
        print(
            "  The code sections are empty: this is the copy-protected shell from the "
            "retail installer, not the complete executable.",
            file=sys.stderr,
        )
    return 1


if __name__ == "__main__":
    sys.exit(main())
