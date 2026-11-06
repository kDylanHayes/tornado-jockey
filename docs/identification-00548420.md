# Identification: the ODE wrapper tail, 0x00548420 onward

Follow-up to [identification-00539750.md](identification-00539750.md). The
remaining placeholder `unassigned/00548420` starts with the ODE world,
geometry, actor and ragdoll resources. All four were carved out; the tail from
`0x0054eec0` onward stays unassigned.

## What was carved

| Unit | Range | `total_code` | Functions | Evidence |
|---|---|---|---|---|
| `Tongas2/ODEWORLD` | `0x00548420-0x00548e3f` | 2480 | 17 | `RegisterClassName` at `0x00548420` names `ODEWORLD` with parent `ACTOR`; the object's methods run through `0x00548de0`. Ends before `ODEGEOMMASTER` at `0x00548e40`. |
| `Tongas2/ODEGEOM` | `0x00548e40-0x0054ab8f` | 7322 | 37 | `RegisterClassName` at `0x00548e40` names `ODEGEOMMASTER` with parent `RESOURCEMASTER`; the manager factory at `0x00549270` creates `ODEGEOM.txt`. The block carries `ODEGEOMRESOURCE::ShowHelpers` (`0x005493e0`), `DYNAMICARRAY::Insert` (`0x00549960`) and the attach errors at `0x0054a680`. Ends before `ODEACTOR` at `0x0054ab90`. |
| `Tongas2/ODEACTOR` | `0x0054ab90-0x0054d34f` | 9848 | 46 | `RegisterClassName` at `0x0054ab90` names `ODEACTOR` with parent `ACTOR`; the actor/body methods run through `0x0054d340` (including the folded `BONEDMODELRESOURCE` stubs at `0x0054d310`). Ends before `ODERAGDOLLMASTER` at `0x0054d350`. |
| `Tongas2/ODERAGDOLL` | `0x0054d350-0x0054eebf` | 6902 | 21 | `RegisterClassName` at `0x0054d350` names `ODERAGDOLLMASTER` with parent `RESOURCEMASTER`; the manager factory at `0x0054d550` creates `ODERAGDOLL.txt`. `0x0054ded0` is `ODERAGDOLLRESOURCE::_BuildDoll` (the `"not attached to actor"` / `_RGT_%s_%s,%d` errors). Ends before `ODEACTORTEMPLATEMASTER` at `0x0054eec0`. |

The class names come from `RegisterClassName`; the ODE source paths in the
region map match the ODE 0.5 wrapper these units expose.

## Scaffolds

Each unit has a header in `include/Tongas2/` and an empty-body scaffold in
`src/Tongas2/`. The exact decorated names and sizes are in
`config/symbols.txt`:

| Unit | Symbols |
|---|---|
| `ODEWORLD` | `?RegisterCommands@ODEWORLDMASTER` 0x3f |
| `ODEGEOM` | `?RegisterCommands@ODEGEOMMASTER` 0x3f, `?CreateODEGEOM` 0x80, `?Insert@DYNAMICARRAY` 0x13b |
| `ODEACTOR` | `?RegisterCommands@ODEACTORMASTER` 0x3f, `?SetFlag@ODEACTOR` 0x51 |
| `ODERAGDOLL` | `?RegisterCommands@ODERAGDOLLMASTER` 0x3f, `?CreateODERAGDOLL` 0x80, `?_BuildDoll@ODERAGDOLLRESOURCE` 0xf3f |

`ODERAGDOLLRESOURCE::_BuildDoll` is Ghidra's 0xf21 but really 0xf3f: the extra
bytes are its switch tables up to the next function. The other selected
functions are clean Ghidra extents.

## Measured before and after

Full report (`py tools/report.py --objdiff tools\objdiff\objdiff-cli.exe`),
both runs against the same private Ghidra snapshot so the function list is
comparable:

| measure | before | after |
|---|---|---|
| `matched_code` | 670007 | **670007** (unchanged) |
| `total_code` | 2399613 | 2398869 |
| `matched_code_percent` | 27.921463 | 27.93012 |
| `fuzzy_match_percent` | 36.644314 | 36.6575 |
| `total_functions` | 14084 | 14091 |
| `matched_functions` | 3849 | 3849 |
| `total_units` | 295 | 299 |

The four units add 26,552 bytes of code (2480 + 7322 + 9848 + 6902). The
carved ranges total 27,296 bytes; the 744-byte difference is the trailing
`int3` padding that `tools/split_functions.py` removes, the same effect every
earlier carve shows. `unassigned/00548420` (72,160 bytes / 370 functions)
became `unassigned/0054eec0` (44,864 bytes / 256 functions); together with the
existing 384-byte `unassigned/00539750` gap, 45,248 bytes and 257 functions
remain. The function count rises by seven because the delinked targets expose
catch/table-adjacent helpers that the old placeholder object folded into its
neighbours.

Verified with the original `Tornado.exe` (SHA-1 `20bf7200...06bc`) and a
private Ghidra project copy at
`%LOCALAPPDATA%\tornado-jockey\ghidra-carve-548420`. The full report and all
four units' objdiff targets build without truncation panics.

## Matching

None of the four units is matched yet. `Tongas2/ODEWORLD` is the smallest
(2,480 bytes, 17 functions) and is the first candidate. All four are recorded
without `"complete": true`.
