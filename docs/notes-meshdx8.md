# MESHDX8 notes

`Tongas2/MESHDX8` (0x004e5d50-0x004ea0ff) now matches 100% exact, code and
data. `config/symbols.txt` records every target name and exact size, so a
re-delink reproduces the same 116 functions. Functions that do not have a
hand-written C++ body yet are naked-asm bodies named `f_<address>`; the unit's
`symbol_mappings` pair them with their target symbols.

Things learned while finishing the unit:

- Ghidra's function sizes in `build/functions.txt` sometimes stop before a
  function's epilogue (for example `FUN_004e9e70` is reported as 0x51 bytes but
  the last `pop edi; ret` takes it to 0x5a). Extending each size through the
  instruction stream until the next `int3` run gives the sizes used here; the
  differences are recorded in `build/size_corrections.json` when regenerating.
- `0x004e65c0` is four EH-prologue functions that Ghidra had merged into one
  blob. They are carved separately in `symbols.txt`.
- The vertex-format switch in `VERTEXBUFFER::CreateBuffers` (0x004e65c0) has to
  be real C++: the compiler emits its jump table in the middle of the body, and
  an asm transcription cannot produce the table's relocations.
- `tools/delink.py` only applies a unit `symbol_mapping` to a defined target
  symbol when it is a function. Mapping a static label inside another function
  renamed it and made objdiff treat the label as a function start, cutting the
  enclosing function. objdiff still applies the unit mapping to the code
  operand, so the label keeps its `LAB_...` name and the operand still pairs.
