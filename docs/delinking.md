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

`analyze.py` only needs to run again after `config/symbols.txt` changes a lot. `delink.py` applies `config/symbols.txt` first, so target objects use our names, e.g. `_adler32` instead of `FUN_0047e661`. It runs the delinker's relocation synthesizer on each unit's ranges and writes a COFF object to the unit's `target` path. Pass unit names to delink only those units. A log is written to `build/delink.log`.

Before delinking, `FixReferences.java` corrects two kinds of bad references: indexed operands such as `jmp [eax*4 + table]` that Ghidra points at `table + n*4` (the delinker would emit no relocation), and constants that merely look like addresses (listed in `config/norefs.txt`, which `sync_symbols.py` fills in).

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
