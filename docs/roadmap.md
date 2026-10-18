# Roadmap

1. **Skeleton** (done): target hash, build generator, objdiff config, docs, CI.
2. **Analysis baseline** (done): scripted Ghidra analysis and delinking, region map in [regions.md](regions.md), first target object (zlib `adler32`) delinked and loaded in objdiff.
3. **First matching unit** (done): zlib 1.2.2 `adler32.c` compiled with cl 13.10.3077 `/O1` matches 100% in objdiff, proving the delink → compile → diff loop.
4. **Libraries** (in progress): build ODE, zlib, libjpeg 6b and libpng from upstream source with matching flags, and mark those units complete. FreeImage's zlib 1.2.2 (8 units), libpng 1.2.8 (14) and libjpeg 6b (37) are done at 100%, and so are FreeImage 3.7.0's own C++ code (21 units, 0x559e00–0x56717b) and ODE 0.5 with OPCODE (49 units, 0x586fd0–0x5bda8f). The libpng 1.0.5 and zlib copies in block 1 belong to the prebuilt D3DX8 library and are linked from `d3dx8.lib`.
5. **Progress reporting**: `objdiff-cli report generate` in CI and a decomp.dev listing.
6. **Engine and game code** (started): decompile Tongas2 and game units. Matched so far, all 100%: `SRESULT` (script result values, 19 functions), `SYSMONITOR` (heap and CPU usage sampling, 5 functions), `TOKENS` (the script tokenizer, 7 functions) and `FILELOADER` (plain file reading, 15 functions).
7. **Full link**: link all objects with link 7.10.3077 and reproduce `Tornado.exe` byte-for-byte.
