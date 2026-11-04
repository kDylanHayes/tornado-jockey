# Image-integrity patches

The committed target image (`orig/Tornado.exe`, SHA-1 `20bf7200...`) contains a
few 15-19 byte blocks that overwrite otherwise readable `.rdata` strings,
`_s_TypeNames` copies and one `.text` function. They exist in the exe itself,
not in the delinked target objects: the bytes are opaque in the file.

They are treated as image-integrity exceptions, not decompilation targets.
`config/image_patches.txt` lists every known block with its address range,
owning unit and the symbol it covers.

Rules:

- Do not fabricate the opaque bytes in source, and do not rename symbols to
  match them. That would not be the original C++.
- A unit whose data includes a patch is left partial (its `complete` flag stays
  off) even when all of its code matches.
- Where a patch leaves a readable suffix, use that suffix as evidence for the
  original string (for example `without base type`), not the random prefix.
- Add newly found blocks to `config/image_patches.txt` with the same format.

The delinker itself can also relocate short strings whose four bytes happen to
read as an address (for example `"set"` is `0x00746573`). That is a separate
problem: list such locations in `config/norefs.txt`, and
`tools/ghidra/FixReferences.java` types them as strings so the relocation
synthesizer leaves them alone.
