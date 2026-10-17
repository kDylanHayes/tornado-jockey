# Roadmap

1. **Skeleton** (done): target hash, build generator, objdiff config, docs, CI.
2. **Analysis baseline**: Ghidra project via rea; apply library signatures (CRT, STL, ODE, zlib, libjpeg, libpng) so the remaining unknown code is the game and engine.
3. **First matching unit**: delink one small unit, write its source, and match it 100% in objdiff. This confirms the compiler flags and the delink → compile → diff loop.
4. **Libraries**: build ODE, zlib 1.2.2, libjpeg 6b and libpng from upstream source with matching flags, and mark those units complete.
5. **Progress reporting**: `objdiff-cli report generate` in CI and a decomp.dev listing.
6. **Engine and game code**: decompile Tongas2 and game units.
7. **Full link**: link all objects with link 7.10.3077 and reproduce `Tornado.exe` byte-for-byte.
