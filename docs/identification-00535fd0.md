# Identification: the SD renderer tail, 0x00535fd0 onward

Follow-up to [identification-00530d10.md](identification-00530d10.md). The
remaining placeholder `unassigned/00535fd0` starts with three more DX8 shadow
renderer registrations and the DirectX version detector. All four were carved
from its start; the rest of the tail (`0x00539750`-`0x00559dff`) stays
unassigned.

## What was carved

| Unit | Range | `total_code` | Functions | Evidence |
|---|---|---|---|---|
| `Tongas2/VERTEXMODELDX8SD` | `0x00535fd0-0x005366bf` | 1700 | 10 | `RegisterClassName` at `0x00535fd0` names `VERTEXMODELDX8SDMASTER` with parent `BONEDMODELMASTER`; the manager factory at `0x00536030` creates the `VERTEXMODELDX8SD.txt` manager. Ends before the next registration at `0x005366c0`. |
| `Tongas2/COLLISIONSCENEDX8SD` | `0x005366c0-0x00536cbf` | 1441 | 13 | `RegisterClassName` at `0x005366c0` names `COLLISIONSCENEDX8SDMASTER` with parent `RESOURCEMASTER`; the manager factory at `0x00536710` creates the `COLLISIONSCENEDX8SD.txt` manager. Ends before the next registration at `0x00536cc0`. |
| `Tongas2/SCENEMODELDX8SD` | `0x00536cc0-0x00538cef` | 7961 | 42 | `RegisterClassName` at `0x00536cc0` names `SCENEMODELDX8SDMASTER` with parent `RESOURCEMASTER`; the manager factory at `0x00536d10` creates the `SCENEMODELDX8SD.txt` manager. The block also carries the scene-model resource methods and their vector helpers through `0x00538cc0`. Ends before the version detector at `0x00538cf0`. |
| `Tongas2/DXVERSION` | `0x00538cf0-0x0053974f` | 2633 | 2 | The large function at `0x00538cf0` loads `\ddraw.dll`, `\dinput.dll` and `\dpnet.dll` and works out the installed DirectX version; `0x00539660` formats it with `"%d.%d%c"` / `"%d.%d"`. Ends before the data pointer at `0x00539750`. |

The first three names come from `RegisterClassName`; `DXVERSION` is
descriptive, since only the DLL names and format strings identify it. The
range stops at `0x0053974f`: `0x00539750` is a data pointer and `0x005398d0`
is a factory for the next (RENDERTEXTURE) object. Trying to include them made
`fix_data_symbols.py` read a relocation past the end of the `FormatVersion`
section, so they stay with the next carve.

## Scaffolds

Each unit has a header in `include/Tongas2/` and an empty-body scaffold in
`src/Tongas2/`. The three SD units follow the earlier `MODELDX8SD` pattern:
the registration and the manager factory are the only defined functions, with
their exact target sizes in `config/symbols.txt`.

| Unit | Symbols |
|---|---|
| `VERTEXMODELDX8SD` | `?RegisterCommands` 0x3f, `?CreateVERTEXMODELDX8SD` 0x80 |
| `COLLISIONSCENEDX8SD` | `?RegisterCommands` 0x3f, `?CreateCOLLISIONSCENEDX8SD` 0x80 |
| `SCENEMODELDX8SD` | `?RegisterCommands` 0x3f, `?CreateSCENEMODELDX8SD` 0x80 |
| `DXVERSION` | `?GetDirectXVersion` 0x965, `?FormatVersion` 0xe4 |

The registration bodies are Ghidra's 0x3f bytes; the small string-return
helpers that follow them in the image are separate functions and are left
unnamed. `GetDirectXVersion` is Ghidra's 0x946 but really 0x965: the extra
0x1f bytes are its jump/decision tables and handler tail, up to the next
function at `0x00539660`. `FormatVersion` is a clean 0xe4.

## Measured before and after

Full report (`py tools/report.py --objdiff tools\objdiff\objdiff-cli.exe`),
both runs against the same private Ghidra snapshot so the function list is
comparable:

| measure | before | after |
|---|---|---|
| `matched_code` | 670012 | **670012** (unchanged) |
| `total_code` | 2401404 | 2400931 |
| `matched_code_percent` | 27.900845 | 27.90634 |
| `fuzzy_match_percent` | 36.613167 | 36.62235 |
| `total_functions` | 14076 | 14079 |
| `matched_functions` | 3850 | 3850 |
| `total_units` | 286 | 290 |

The four units add 13,735 bytes of code (1700 + 1441 + 7961 + 2633). The
carved ranges total 14,208 bytes; the 473-byte difference is the trailing
`int3` padding that `tools/split_functions.py` removes, the same effect every
earlier carve shows. `unassigned/00535fd0` (146,992 bytes / 633 functions)
became `unassigned/00539750` (132,784 bytes / 569 functions). The function
count rises by three because the delinked targets expose tiny
registration-tail helpers that the old placeholder object folded into its
neighbours; they are unmatched target-only symbols.

Verified with the original `Tornado.exe` (SHA-1 `20bf7200...06bc`) and a
private Ghidra project copy at
`%LOCALAPPDATA%\tornado-jockey\ghidra-carve-535fd0`. The full report and all
four units' objdiff targets build without truncation panics.

## Matching

None of the four units is matched yet. `Tongas2/COLLISIONSCENEDX8SD` is the
smallest (1,441 bytes, 13 functions) and is the first candidate. All four are
recorded without `"complete": true`.
