# Contributing

Thanks for helping. The goal is C++ that compiles, with the original Visual C++ 7.1 compiler, to exactly the bytes in `Tornado.exe`. objdiff is the judge: a function counts when objdiff shows it at 100%.

## Ground rules

- **Never commit game files**: no executables, DLLs, game assets, delinked target objects, Ghidra projects or large disassembly dumps. Short excerpts of disassembly in an issue or review comment are fine. CI rejects executables and the game's asset types.
- **Never ask for or share the complete exe** or ways around its copy protection. Bring your own copy (see [docs/getting-started.md](docs/getting-started.md)).
- **Write the source yourself.** Decompiler output is a starting point, not a submission: give things real names and types, and make it read like code a person wrote in 2006.
- **Mark a unit complete only at 100%**, code and data, in objdiff.

## Set up

Follow [docs/getting-started.md](docs/getting-started.md). At the end, every unit already in the repository shows 100% in objdiff.

## Pick something to work on

- **Library code** (zlib, libpng, libjpeg, FreeImage, ODE) is matched by compiling the right upstream version, not by hand. See "Placing library code" in [docs/delinking.md](docs/delinking.md).
- **Engine and game code** (Tongas2, the game itself) is the real decompilation work. Error strings like `"RENDERER::Init> Error ..."` name a class's methods and roughly where its source file starts; [docs/toolchain.md](docs/toolchain.md#matching-notes) explains how.
- [docs/regions.md](docs/regions.md) maps the exe; `config/units.json` lists the units defined so far.

Open an issue (or comment on one) saying which unit or class you're taking, so two people don't do the same file.

## Match a unit

1. Add the unit to `config/units.json`: its address `ranges`, `target` (`build/target/...obj`), `source` (mirror the original path where strings reveal it, e.g. `src/Tongas2/Renderer.cpp`), `category` (`game`, `engine`, `lib`, `sdk` or `crt`) and `cflags` if it isn't built with the defaults.
2. Write the source, and put shared declarations in `include/`.
3. Add every symbol the unit defines or references to `config/symbols.txt` with its MSVC decorated name; compiling first and reading names from your object is the easy way (`objdump -t build/src/...obj`).
4. `python3 tools/delink.py <unit>`, then `ninja`, then check the unit in objdiff. Iterate.
5. When every function and data item matches, set `"complete": true`.

Partial units are welcome too: leave `complete` off and say in the pull request which functions match.

## Open a pull request

- One unit, or a few related ones, per pull request.
- Fill in the template: which units, their match percentage, and anything learned about flags or quirks (add those to [docs/toolchain.md](docs/toolchain.md) as well).
- CI checks that no game files are committed and that the config is valid. On pull requests from this repository it also builds everything and reports overall progress and which functions changed; on forks it can't (it has no game files), so paste the output of `tools/progress.py` from your machine instead.

## Style

- C++ as written for MSVC 7.1: C++98 with that compiler's quirks. No C++11.
- 4-space indentation. Use Wanako's class names where strings reveal them (`RENDERER`, `ACTOR`, `RESOURCEMANAGER`).
- Keep library code as close to its upstream source as possible; note any change Wanako made.

## Reference

- [docs/getting-started.md](docs/getting-started.md): setup, building, measuring progress
- [docs/toolchain.md](docs/toolchain.md): compiler, flags and matching notes
- [docs/delinking.md](docs/delinking.md): producing target objects and naming symbols
- [docs/regions.md](docs/regions.md): what's where in the exe
- [docs/rea.md](docs/rea.md): agent-assisted analysis with rea
- [docs/maintainers.md](docs/maintainers.md): CI secrets and the progress page
