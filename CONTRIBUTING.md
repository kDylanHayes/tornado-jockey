# Contributing

## Ground rules

- **Never commit game files**: no executables, DLLs, assets, delinked objects or large disassembly dumps. Disassembly may appear only as short excerpts in issues or comments.
- Source must be written by you. Don't paste decompiler output wholesale; clean it up into readable C++ with real names.
- Only claim a unit as matching when objdiff shows 100% for it.

## Workflow

1. Set up the toolchain and target (see the [README](README.md)).
2. Pick a unit in `config/units.json`, or propose a new split in an issue.
3. Add or edit its source file under `src/`, mirroring the original path where known (e.g. `src/Tongas2/Renderer.cpp`).
4. Run `ninja` and compare in objdiff. Set `"complete": true` once every function in the unit matches.
5. Open a pull request. Mention the unit and its match percentage.

## Style

- C++ as written for MSVC 7.1 (C++98 with that compiler's quirks). No C++11 features.
- 4-space indentation, Wanako-style class names where they are known from strings (`RENDERER`, `ACTOR`, `RESOURCEMANAGER`).
- Shared declarations go in `include/`.

## Tools

- [docs/toolchain.md](docs/toolchain.md): compiler and flags
- [docs/delinking.md](docs/delinking.md): producing target objects
- [docs/rea.md](docs/rea.md): agent-assisted analysis with rea
