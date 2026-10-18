# Tornado Jockey decompilation

[![Progress](https://img.shields.io/endpoint?url=https%3A%2F%2Fkdylanhayes.github.io%2Ftornado-jockey%2Fbadge.json)](https://kdylanhayes.github.io/tornado-jockey/)

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
| Libraries | Direct3D 8 / D3DX8, DirectInput 8, FMOD 3.74, ODE, zlib 1.2.2, libjpeg 6b, libpng 1.2.8 (plus D3DX8's own libpng 1.0.5 and zlib), FreeImage, Dinkumware STL, ATL |

The retail installer ships `Tornado.exe` with its code sections empty; the WildTangent launcher (`Tornado-WT.exe`) fills them in at runtime. The target is the complete, unprotected executable with the SHA-1 above. This project does not provide that file or any means of removing copy protection.

## Status

FreeImage 3.7.0 matches 100%: its own C++ code (21 units) and its bundled zlib 1.2.2, libpng 1.2.8 and libjpeg 6b (59 units). So does ODE 0.5 with OPCODE (49 units), the first six engine units, `SRESULT`, `SYSMONITOR`, `TOKENS`, `FILELOADER`, `FILELOADERWKZ` and `WKZ`, and the three units of WildTangent's leaderboard SDK: 138 units in all. The older libpng copy turned out to be part of Microsoft's prebuilt D3DX8 library, so it gets linked rather than rebuilt. See [docs/roadmap.md](docs/roadmap.md).

## Getting started

You need your own copy of the complete `Tornado.exe` and of Microsoft's free Visual C++ Toolkit 2003, plus Python, ninja, Wine (on Linux and macOS), Ghidra 12.1.4 with the delinker extension, and objdiff. [docs/getting-started.md](docs/getting-started.md) walks through installing them. Then:

```sh
cp /path/to/your/Tornado.exe orig/
python3 tools/check_target.py          # verifies the SHA-1
python3 tools/extract_vctoolkit.py /path/to/VCToolkitSetup.exe tools/vc71
export GHIDRA_INSTALL_DIR=/path/to/ghidra_12.1.4_PUBLIC
python3 tools/analyze.py               # Ghidra project + auto-analysis
python3 tools/delink.py                # target objects into build/target/
python3 configure.py --msvc tools/vc71
ninja                                  # compiles src/ into build/src/
objdiff                                # open the project folder in the GUI
python3 tools/report.py && python3 tools/progress.py build/report.json   # overall progress
```

What's known about the binary's layout is in [docs/regions.md](docs/regions.md).

## Progress

Progress is the share of the exe's code (2.3 MB in `.text`) that objdiff reports as matching. The measured report is committed in `progress/`, and CI publishes it on the [progress page](https://kdylanhayes.github.io/tornado-jockey/), split into game and engine code, libraries, the C/C++ runtime and SDK libraries. See [docs/getting-started.md](docs/getting-started.md#progress-tracking).

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
