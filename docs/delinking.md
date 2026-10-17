# Delinking: producing target objects

objdiff compares **object files**, but the original game exists only as a linked exe. Before a unit can be diffed, its code and data have to be cut back out of `Tornado.exe` into a relocatable COFF object (the "target"). Our compiled source produces the matching "base" object.

`Tornado.exe` has no relocation table (it always loads at 0x400000), so every pointer has to be recovered from analysis. That's why this step goes through Ghidra, the same Ghidra analysis that rea uses.

## Setup

1. Install [Ghidra 12.1.4](https://github.com/NationalSecurityAgency/ghidra/releases) and JDK 21. Set `GHIDRA_INSTALL_DIR` to the install folder.
2. Build the [ghidra-delinker-extension](https://github.com/boricj/ghidra-delinker-extension) for that Ghidra version and install it:
   ```sh
   git clone --recursive https://github.com/boricj/ghidra-delinker-extension
   cd ghidra-delinker-extension
   gradle installStandaloneDeps && gradle buildExtension -Pstandalone
   mkdir -p ~/.config/ghidra/ghidra_12.1.4_PUBLIC/Extensions
   unzip dist/*.zip -d ~/.config/ghidra/ghidra_12.1.4_PUBLIC/Extensions
   ```
   (On macOS the user folder is `~/Library/ghidra/ghidra_12.1.4_PUBLIC/Extensions`.)

## Steps

```sh
python3 tools/analyze.py      # creates build/ghidra and runs auto-analysis (~3 min)
python3 tools/delink.py       # exports every unit with "ranges" to build/target/
python3 configure.py
```

`analyze.py` only needs to run again after `config/symbols.txt` changes a lot. `delink.py` applies `config/symbols.txt` first, so target objects use our names, e.g. `_adler32` instead of `FUN_0047e661`. It runs the delinker's relocation synthesizer on each unit's ranges (`tools/ghidra/DelinkUnit.java`) and writes a COFF object to the unit's `target` path. Pass unit names to delink only those units. A log is written to `build/delink.log`.

The game was compiled with `/Gy`, so the compiler put every function and every string literal in its own section, and the linker padded functions to 16 bytes with `int3`. `delink.py` therefore post-processes each target with `tools/split_functions.py`, which gives each function and each data symbol its own section, drops the `int3` padding, and resolves PC-relative relocations inside a function. Without this, objdiff counts the padding as mismatched code and calls between functions of the same unit as mismatched arguments.

## Naming symbols

objdiff pairs functions and relocations by name, so every symbol a unit defines or references needs its MSVC decorated name in `config/symbols.txt`. The easy way to get them is to compile the unit first and read the names out of our object (`objdump -t build/src/...obj`), including string literals (`??_C@_0...`) and compiler constants (`__real@00000000`).

Target objects are derived from the game, so they stay in `build/` and are never committed.

## Finding unit boundaries

See [regions.md](regions.md) for the current map. Useful clues:

- MSVC places functions in source order within an object, and objects in link order.
- Library code (zlib, libpng, ODE, CRT) sits in contiguous blocks and matches upstream source.
- Assert strings and `__FILE__` paths (e.g. `C:\Wanako\Tongas2\SDKs\ode\ode\src\joint.cpp`) name the source file of nearby code.
- A change between 16-byte-aligned and packed function starts marks objects built with different flags.
