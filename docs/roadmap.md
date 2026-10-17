# Roadmap

1. **Skeleton** (done): target hash, build generator, objdiff config, docs, CI.
2. **Analysis baseline** (done): scripted Ghidra analysis and delinking, region map in [regions.md](regions.md), first target object (zlib `adler32`) delinked and loaded in objdiff.
3. **First matching unit** (done): zlib 1.2.2 `adler32.c` compiled with cl 13.10.3077 `/O1` matches 100% in objdiff, proving the delink → compile → diff loop.
4. **Libraries** (in progress): build ODE, zlib, libjpeg 6b and libpng from upstream source with matching flags, and mark those units complete. zlib 1.2.2 is done (8 units, 100%).
5. **Progress reporting**: `objdiff-cli report generate` in CI and a decomp.dev listing.
6. **Engine and game code**: decompile Tongas2 and game units.
7. **Full link**: link all objects with link 7.10.3077 and reproduce `Tornado.exe` byte-for-byte.
