# Tornado Jockey decompilation

[![Progress](https://img.shields.io/endpoint?url=https%3A%2F%2Fraw.githubusercontent.com%2FkDylanHayes%2Ftornado-jockey%2Fmain%2Fprogress%2Fbadge.json)](progress/README.md)

A work-in-progress matching decompilation of **Tornado Jockey** (Wanako Games / WildTangent, 2006) for Windows.

The goal is C++ source that, compiled with the original toolchain, reproduces the game's code byte-for-byte, in the spirit of [n64decomp/sm64](https://github.com/n64decomp/sm64). Progress is measured with [objdiff](https://github.com/encounter/objdiff); analysis is done in Ghidra, driven by humans or by agents through [rea](https://github.com/morluto/rea).

**This repository contains no game code or assets.** You must supply your own copy of the game.

## Target

| | |
|---|---|
| File | `Tornado.exe` |
| Version | 1.0.0.67 (English, Windows) |
| Linked | 2006-07-14 21:22:54 UTC |
| SHA-1 | `20bf7200effe49d2dff295a3b6c546a5804e06bc` |
| Compiler | Microsoft Visual C++ 7.1 (`cl` 13.10.3077), linker 7.10.3077 |
| Libraries | Direct3D 8 / D3DX8, DirectInput 8, FMOD 3.74, ODE, zlib 1.2.2, libjpeg 6b, libpng 1.2.8 (plus D3DX8's own libpng 1.0.5 and zlib), FreeImage, Dinkumware STL, ATL |

The retail installer ships `Tornado.exe` with its code sections empty; the WildTangent launcher (`Tornado-WT.exe`) fills them in at runtime. The target is the complete image extracted from the installer in the game's ISO, with the SHA-1 above. It differs from the earlier retail image (`71be8eb9…`) in 18 bytes at VA `0x59188f`, in `lib/ode/joint` (99.73% of that object); everything else is identical. The old image is no longer available. This project does not provide that file or any means of removing copy protection.

## Status

FreeImage 3.7.0 matches 100%: its own C++ code (21 units) and its bundled zlib 1.2.2, libpng 1.2.8 and libjpeg 6b (59 units). So does ODE 0.5 with OPCODE (49 units), the first seven engine units, `APPOPTIONS`, `SRESULT`, `SYSMONITOR`, `TOKENS`, `FILELOADER`, `FILELOADERWKZ` and `WKZ`, and the three units of WildTangent's leaderboard SDK: 139 units in all. The older libpng copy turned out to be part of Microsoft's prebuilt D3DX8 library, so it gets linked rather than rebuilt. See [docs/roadmap.md](docs/roadmap.md).

## Getting started

You need your own copy of the complete `Tornado.exe` and of Microsoft's free Visual C++ Toolkit 2003, plus Python, ninja, JDK 21+, Ghidra 12.2 with the delinker extension, and objdiff. Wine is only needed on Linux and macOS. [docs/getting-started.md](docs/getting-started.md) walks through installing them. Then, on Windows:

```bat
copy C:\path\to\Tornado.exe orig\
py tools\check_target.py
py tools\extract_vctoolkit.py C:\path\to\VCToolkitSetup.exe tools\vc71
set GHIDRA_INSTALL_DIR=C:\ghidra_12.2_PUBLIC
py tools\analyze.py
py tools\delink.py
py configure.py --msvc tools\vc71
ninja
objdiff
py tools\report.py && py tools\progress.py build\report.json
```

On Linux or macOS, use `python3` with `/` paths and Wine for the compiler; see [docs/getting-started.md](docs/getting-started.md).

What's known about the binary's layout is in [docs/regions.md](docs/regions.md).

## Progress

Progress is the share of the exe's code (2.3 MB in `.text`) that objdiff reports as matching. It is measured with the game files and committed to [progress/](progress/README.md), split into game and engine code, libraries, the C/C++ runtime and SDK libraries. See [docs/getting-started.md](docs/getting-started.md#progress-tracking).

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
