# Using kuna

[kuna](https://github.com/Noelo-Lab/kuna) is a decompiler written in Rust, ported from Ghidra's and tuned for agents. It is a single binary with a CLI: no Java, no Ghidra project, and no state kept between runs. In this project it can stand in for the pseudocode half of [rea](rea.md). It does not delink, build or measure matching, so `tools/analyze.py`, `tools/delink.py`, ninja and objdiff stay as they are.

## Install

Download the `kuna` binary for your OS from the [latest release](https://github.com/Noelo-Lab/kuna/releases). It carries its processor specs, so nothing else is needed. On macOS, clear the download quarantine with `xattr -d com.apple.quarantine kuna`.

To build it instead you need only a Rust toolchain:

```sh
git clone https://github.com/Noelo-Lab/kuna && cd kuna
make binaries specs       # decompiler/target/release/kuna, about 3 minutes
```

A source build reads its specs from the checkout, so run it from there or set `KUNA_SPECS=<checkout>/specs`.

`kuna install-skill` installs kuna's own usage guide for Claude Code, Codex and OpenCode.

## Give it our function list

On its own kuna finds about 11,000 functions in `Tornado.exe` and names none of them. `tools/kuna_defs.py` writes the functions from `config/symbols.txt`, with their decorated names, to `build/kuna-functions.txt`. If `tools/report.py` has run, Ghidra's full function list in `build/functions.txt` is included too, with its bounds.

```sh
python3 tools/kuna_defs.py
```

Pass the file to every command with `--define-function @build/kuna-functions.txt`.

## Use

Always pass `--mode reliable`. `Tornado.exe` is over 2 MB, so kuna's default picks its `fast` preset, which finds a tenth as many functions. For a single hard function, `--mode aggressive` recovers more (return values that `reliable` sometimes drops, for one) at about 5 seconds a function.

```sh
D="--mode reliable --define-function @build/kuna-functions.txt"

# one function, by name or address
kuna decompile orig/Tornado.exe '?RunProcess@@YAHPBDPAD@Z' $D
kuna decompile orig/Tornado.exe 0x4dc160 --addr $D

# the whole exe: .c, .h and .asm in build/kuna/, a minute or two on 4 cores
kuna decompile-project orig/Tornado.exe --jobs auto $D -o build/kuna

# strings and references
kuna strings orig/Tornado.exe --filter 'RENDERER::' --json
kuna xrefs orig/Tornado.exe --to 0x4dc160 --json
```

Everything goes in `build/`, which is git-ignored. The exports are derived from the game's code, so never commit them.

## Known weak spots in this exe

- **Large stack frames.** Functions that call `__chkstk` (frames of a page or more, such as `WKZ::Compress`) lose their stack: calls come out with no arguments. Use the assembly, or Ghidra, for those.
- **Virtual calls lose `this`.** A call through a vtable passes `this` in ECX, and kuna drops it: `(*vtbl[1])()` where the source has `obj->Method()`. Arguments pushed on the stack are kept.
- **Strings print as addresses.** A string argument shows as `s_673a54`; look the text up with `kuna strings` or in `.asm`.

As always, the output is a starting point. objdiff decides what matches.
