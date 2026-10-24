# Delinking: producing target objects

objdiff compares **object files**, but the original game exists only as a linked exe. Before a unit can be diffed, its code and data have to be cut back out of `Tornado.exe` into a relocatable COFF object (the "target"). Our compiled source produces the matching "base" object.

`Tornado.exe` has no relocation table (it always loads at 0x400000), so every pointer has to be recovered from analysis. That's why this step goes through Ghidra, the same Ghidra analysis that rea uses.

## Setup

1. Install [Ghidra 12.1.4](https://github.com/NationalSecurityAgency/ghidra/releases) and JDK 21. Set `GHIDRA_INSTALL_DIR` to the install folder.
2. Download the [ghidra-delinker-extension 0.9.2 release](https://github.com/boricj/ghidra-delinker-extension/releases/tag/v0.9.2) built for Ghidra 12.1.4 (`ghidra_12.1.4_PUBLIC_20260926_ghidra-delinker-extension.zip`) and unzip it into Ghidra's own `Extensions` folder:
   ```sh
   unzip ghidra_12.1.4_PUBLIC_20260926_ghidra-delinker-extension.zip -d "$GHIDRA_INSTALL_DIR/Ghidra/Extensions"
   ```
   (Unzipping it into your user folder, `~/.config/ghidra/ghidra_12.1.4_PUBLIC/Extensions` or `~/Library/ghidra/ghidra_12.1.4_PUBLIC/Extensions` on macOS, works too, as does building it from source with Gradle.)

## Steps

```sh
python3 tools/analyze.py      # creates build/ghidra and runs auto-analysis (~3 min)
python3 tools/delink.py       # exports every unit with "ranges" to build/target/
python3 configure.py
```

`analyze.py` only needs to run again after `config/symbols.txt` changes a lot. `delink.py` applies `config/symbols.txt` first, so target objects use our names, e.g. `_adler32` instead of `FUN_0047e661`. It runs the delinker's relocation synthesizer on each unit's ranges (`tools/ghidra/DelinkUnit.java`) and writes a COFF object to the unit's `target` path. Pass unit names to delink only those units. A log is written to `build/delink.log`.

The game was compiled with `/Gy`, so the compiler put every function and every string literal in its own section, and the linker padded functions to 16 bytes with `int3`. `delink.py` therefore post-processes each target with `tools/split_functions.py`, which gives each function and each data symbol its own section, drops the `int3` padding, and resolves PC-relative relocations inside a function. Without this, objdiff counts the padding as mismatched code and calls between functions of the same unit as mismatched arguments.

For C++ code it also does what the exe can't show: it adds the `__except_list` relocation to every `fs:[0]` access (the exe holds a plain 0), keeps catch blocks inside their function, and renames Ghidra's labels there (`Catch@...`, `DAT_...`) to the compiler's `$L...` names taken from our object, so objdiff sees the same functions on both sides. Where a function's size is known (from `config/symbols.txt`, or from our object), the size decides where padding starts, so the `int3` the compiler leaves after a call that never returns (`_CxxThrowException`) is kept. A size taken from our object is wrong while our function doesn't match yet; when ours is shorter, the target's function gets cut and objdiff can crash on it, so such functions get their size in `symbols.txt`.

## Naming symbols

objdiff pairs functions and relocations by name, so every symbol a unit defines or references needs its MSVC decorated name in `config/symbols.txt`. The easy way to get them is to compile the unit first and read the names out of our object (`objdump -t build/src/...obj`), including string literals (`??_C@_0...`) and compiler constants (`__real@00000000`).

Before delinking, `ApplySymbols.java` names functions and sets their extent from the size column: it removes functions analysis split off inside the range, ends a function analysis ran past a neighbour's start, and disassembles code only reached through pointers or EH tables (catch blocks, `png_error` handlers). Then `FixReferences.java` corrects bad references: indexed operands such as `jmp [eax*4 + table]` that Ghidra points at `table + n*4` (the delinker would emit no relocation); immediates into data or into their own function (a catch block's continuation address) that have no reference; short strings analysis typed as pointers (`"PNG"` reads as 0x00474e50); and constants that merely look like addresses (listed in `config/norefs.txt`, which `sync_symbols.py` fills in). It also types the pointers to strings and functions that data holds and analysis missed (`CMDMGR`'s create function is only reached through such a pointer): `CMDBASE`'s command table starts with a pointer to `"set"`, whose bytes read as the pointer 0x00746573, so analysis typed the string as a pointer and left the table untyped. Last, `FixJumpTables.java` types every entry of a switch table as a pointer: when analysis can't recover a switch, entries pointer scanning missed stay raw bytes and get no relocation.

After delinking, `tools/split_functions.py` splits each function into its own section and `tools/fix_data_symbols.py` tidies data the way the compiler writes it: labels analysis left inside a named variable (the high half of a `LARGE_INTEGER`) are hidden and references to them become variable plus offset; a reference just before a variable (`[eax + s_line - 1]`) is moved onto it; zero-filled `.data` becomes `.bss`, since the exe keeps uninitialized variables at the end of its `.data`; and vtable slots naming the scalar deleting destructor `??_G` point at `??_E` instead, the weak external MSVC writes there.

Target objects are derived from the game, so they stay in `build/` and are never committed.

## Placing library code

For code built from upstream source (zlib, libpng...), compile the source first, then let the tools find it and copy names over:

```sh
ninja
python3 tools/find_objects.py --functions build/src/lib/zlib/*.obj     # where each function sits
python3 tools/find_objects.py --functions --layout 0x582971-0x586fc2 build/src/lib/zlib/*.obj  # suggest unit ranges
python3 tools/find_objects.py --functions --write build/src/lib/zlib/*.obj  # name them in symbols.txt
python3 tools/delink.py
python3 tools/sync_symbols.py lib/zlib/inflate lib/zlib/trees          # name referenced data
python3 tools/delink.py
```

`find_objects.py` searches the exe for each function's bytes with relocated bytes wildcarded; `--write` names functions found exactly once inside their unit's range. `sync_symbols.py` pairs relocations in a matching unit and gives the target's auto-named data (`DAT_…`, `s_…`) the names our compiler used, so objdiff can pair them. Functions the linker dropped (`/OPT:REF`) simply aren't found; identical functions the linker folded (`/OPT:ICF`, e.g. zlib's `zcfree`) are found at several addresses. A folded function carries one name in the target; when it sits in another unit, map it with `"symbol_mappings": {"_zcfree": "_jpeg_free_small"}` on that unit in `config/units.json`.

## Finding unit boundaries

See [regions.md](regions.md) for the current map. Useful clues:

- MSVC places functions in source order within an object, and objects in link order.
- Library code (zlib, libpng, ODE, CRT) sits in contiguous blocks and matches upstream source.
- Assert strings and `__FILE__` paths (e.g. `C:\Wanako\Tongas2\SDKs\ode\ode\src\joint.cpp`) name the source file of nearby code.
- A change between 16-byte-aligned and packed function starts marks objects built with different flags.
