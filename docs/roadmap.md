# Roadmap

1. **Skeleton** (done): target hash, build generator, objdiff config, docs, CI.
2. **Analysis baseline** (done): scripted Ghidra analysis and delinking, region map in [regions.md](regions.md), first target object (zlib `adler32`) delinked and loaded in objdiff.
3. **First matching unit**: compile zlib 1.2.2 `adler32.c` with cl 13.10.3077 and match it 100%. This confirms the compiler setup and the delink → compile → diff loop. Blocked on the compiler.
4. **Libraries**: build ODE, zlib 1.2.2, libjpeg 6b and libpng from upstream source with matching flags, and mark those units complete.
5. **Progress reporting**: `objdiff-cli report generate` in CI and a decomp.dev listing.
6. **Engine and game code**: decompile Tongas2 and game units.
7. **Full link**: link all objects with link 7.10.3077 and reproduce `Tornado.exe` byte-for-byte.
