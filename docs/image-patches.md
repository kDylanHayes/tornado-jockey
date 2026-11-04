# Image watermarks

The `20bf7200...` ISO-extracted `Tornado.exe` contains short opaque byte runs
(13–19 bytes, 16 typical) in both `.text` and `.rdata`. They are part of the
image itself: a fresh Ghidra analysis, the shared analysed project and the raw
file all show the same bytes, and the decompiler turns them into nonsense
(`cld`/`fild`/`ret 0xc945`, or string heads replaced by random bytes).

They look like installer/DRM watermarks rather than compiler output, so they
cannot be recovered from source. Do not write them into C++ or fabricate data
to make objdiff pass: units whose code or data overlaps a watermark stay
partial.

`../config/image_patches.txt` records every known range with its unit, symbol
and kind. The starts are irregular — no fixed address stride or modulus was
found — so new ones have to be identified from a target/base diff. Ranges
marked `observed` came from byte-level target/base comparisons; `reported`
ones came from other workers and are listed at their reported start address.

A separate, fixable issue is the delinker putting relocations on short strings
whose bytes read as an address (`"set"` is `0x00746573`). That is handled in
`tools/ghidra/FixReferences.java` and `config/norefs.txt`, not by this list.
