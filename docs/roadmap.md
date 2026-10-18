# Roadmap

1. **Skeleton** (done): target hash, build generator, objdiff config, docs, CI.
2. **Analysis baseline** (done): scripted Ghidra analysis and delinking, region map in [regions.md](regions.md), first target object (zlib `adler32`) delinked and loaded in objdiff.
3. **First matching unit** (done): zlib 1.2.2 `adler32.c` compiled with cl 13.10.3077 `/O1` matches 100% in objdiff, proving the delink → compile → diff loop.
4. **Libraries** (in progress): build ODE, zlib, libjpeg 6b and libpng from upstream source with matching flags, and mark those units complete. FreeImage's zlib 1.2.2 (8 units), libpng 1.2.8 (14) and libjpeg 6b (37) are done at 100%, and so is FreeImage 3.7.0's own C++ code (21 units, 0x559e00–0x56717b).
5. **Progress reporting** (done, pending CI secrets): `tools/report.py` reports progress over all of `.text`, CI runs it on every push, and `main` publishes a progress page and badge. A decomp.dev listing is a sign-in away; see [maintainers.md](maintainers.md).
6. **Engine and game code** (started): decompile Tongas2 and game units. First unit: `SRESULT` (script result values), 19 functions, 100%.
7. **Full link**: link all objects with link 7.10.3077 and reproduce `Tornado.exe` byte-for-byte.
