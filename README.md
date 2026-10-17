# Tornado Jockey decompilation

A work-in-progress matching decompilation of **Tornado Jockey** (Wanako Games / WildTangent, 2006) for Windows.

The goal is C++ source that, compiled with the original toolchain, reproduces the game's code byte-for-byte, in the spirit of [n64decomp/sm64](https://github.com/n64decomp/sm64). Progress is measured with [objdiff](https://github.com/encounter/objdiff); analysis is done in Ghidra, driven by humans or by agents through [rea](https://github.com/morluto/rea).

**This repository contains no game code or assets.** You must supply your own copy of the game.

## Target

| | |
|---|---|
| File | `Tornado.exe` |
| Version | 1.0.0.67 (English, Windows) |
| Linked | 2006-07-14 21:22:54 UTC |
| SHA-1 | `71be8eb97a8fb0aa161af7a2af14bed63af2c6de` |
| Compiler | Microsoft Visual C++ 7.1 (`cl` 13.10.3077), linker 7.10.3077 |
| Libraries | Direct3D 8 / D3DX8, DirectInput 8, FMOD 3.74, ODE, zlib 1.2.2, libjpeg 6b, libpng 1.2.8 and 1.0.5, FreeImage, Dinkumware STL, ATL |

The retail installer ships `Tornado.exe` with its code sections empty; the WildTangent launcher (`Tornado-WT.exe`) fills them in at runtime. The target is the complete, unprotected executable with the SHA-1 above. This project does not provide that file or any means of removing copy protection.

## Status

FreeImage's bundled zlib 1.2.2, libpng 1.2.8 and libjpeg 6b match 100% (59 units, 131 KB of code). Next up: the older libpng copy, FreeImage itself and ODE. See [docs/roadmap.md](docs/roadmap.md).

## Getting started

Requirements:

- Python 3.10+
- [ninja](https://ninja-build.org/)
- Wine (Linux/macOS) to run the compiler, or Windows
- Microsoft Visual C++ Toolkit 2003 (`cl` 13.10.3077). It is not redistributable; bring your own `VCToolkitSetup.exe` and unpack it with `tools/extract_vctoolkit.py`, see [docs/toolchain.md](docs/toolchain.md)
- [objdiff](https://github.com/encounter/objdiff/releases) (GUI) and `objdiff-cli`
- Ghidra 12.1.4 plus the delinker extension, to produce target objects; see [docs/delinking.md](docs/delinking.md)

What's known about the binary's layout is in [docs/regions.md](docs/regions.md).

Steps:

```sh
cp /path/to/your/Tornado.exe orig/
python3 tools/check_target.py          # verifies the SHA-1
export GHIDRA_INSTALL_DIR=/path/to/ghidra_12.1.4_PUBLIC
python3 tools/analyze.py               # Ghidra project + auto-analysis
python3 tools/delink.py                # target objects into build/target/
python3 configure.py --msvc tools/vc71
ninja                                  # compiles src/ into build/src/
objdiff                                # open the project folder in the GUI
```

## Repository layout

```
orig/        your Tornado.exe goes here (git-ignored)
config/      target hash, symbols, and translation-unit splits
src/         decompiled C++ source, one file per original translation unit
include/     shared headers
tools/       helper scripts
docs/        contributor docs
build/       generated objects (git-ignored)
```

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md).

## Legal

This is a clean-room reimplementation for preservation, interoperability and education. No copyrighted code, executables or assets from the game are included or accepted. Tornado Jockey is the property of its respective owners.
