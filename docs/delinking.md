# Delinking: producing target objects

objdiff compares **object files**, but the original game exists only as a linked exe. Before a unit can be diffed, its code and data have to be cut back out of `Tornado.exe` into a relocatable COFF object (the "target"). Our compiled source produces the matching "base" object.

`Tornado.exe` has no relocation table (it always loads at 0x400000), so the delinker has to recover every pointer from analysis. That is why this step goes through Ghidra, whose reference analysis is also what rea uses.

## Steps

1. Import `orig/Tornado.exe` into a Ghidra project and run auto-analysis. Apply `config/symbols.txt`.
2. Install the [Ghidra delinker extension](https://github.com/boricj/ghidra-delinker-extension).
3. For each unit in `config/units.json`, select the unit's address ranges and export them as a COFF i386 object to the unit's `target` path, under `build/target/`.
4. Run `python3 configure.py` and open the project in objdiff.

Exporting will be scripted in `tools/` (Ghidra headless) once the first unit's split is known. Target objects are derived from the game, so they live in `build/` and are never committed.

## Finding unit boundaries

MSVC places functions in the order they appear in each source file, and object files in link order. Useful clues:

- Library code (ODE, zlib, libjpeg, libpng, CRT) sits in contiguous blocks and matches upstream source, which makes those boundaries easy to find.
- Assert strings and `__FILE__` paths (for example `C:\Wanako\Tongas2\SDKs\ode\ode\src\joint.cpp`) name the source file of nearby code.
- Changes in alignment padding (`int3`/`0xCC` runs) and string-pool order in `.rdata` often mark object boundaries.
