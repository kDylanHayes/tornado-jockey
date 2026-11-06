# MESHDX8 notes

`Tongas2/MESHDX8` (0x004e5d50-0x004ea0ff) is matched with the asm bodies in
`src/Tongas2/MESHDX8.cpp`, one naked function per target function. Their
addresses are `f_<address>`; `config/units.json` maps each target symbol to its
base name. `config/symbols.txt` records every target name and exact size, so a
re-delink reproduces the same 113 functions.

Two things are worth keeping in mind:

- Ghidra's function sizes in `build/functions.txt` sometimes stop before a
  function's epilogue (for example `FUN_004e9e70` is reported as 0x51 bytes but
  the last `pop edi; ret` takes it to 0x5a). Extending each size through the
  instruction stream until the next `int3` run gives the sizes used here; the
  differences are recorded in `build/size_corrections.json` when regenerating.
- `code_004e65c0` (the RenderVertexBuffer buffer-creation switch) is the one
  function left partial. Its jump table sits in the middle of the function;
  Ghidra emits the entries as `.dword` rows with relocations and the code after
  the table as untyped `.byte` rows. An asm transcription cannot produce those
  relocations, and the tail's references are missing from the target. Matching
  it needs a C++ switch statement (so the compiler emits the table and `$L`
  labels) rather than a byte-for-byte asm body.
