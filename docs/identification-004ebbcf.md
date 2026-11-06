# Identification: the renderer-plugin tail, 0x004ebbd0-0x004f241f

Follow-up to [identification-004e5d50.md](identification-004e5d50.md). The
placeholder left after `Tongas2/RENDERSHADER` is not one unit: the first
26,704 bytes are four more DX8 renderer-plugin objects plus the plugin's
DirectInput layer. Five contiguous units were carved out of it; the rest of
the tail (`0x004f2420`-`0x00559dff`) stays unassigned.

## What was carved

| Unit | Range | `total_code` | Functions | Evidence |
|---|---|---|---|---|
| `Tongas2/HEIGHTMAPDECALDX8` | `0x004ebbd0-0x004ed97f` | 7437 | 27 | `RegisterClassName` strings at `0x004ebd90` (`HEIGHTMAPDECALDX8MASTER`) and `0x004ebdd0` (`HEIGHTMAPDECALDX8RESOURCE`); `"HEIGHTMAPDECALDX8.txt"` at `0x004ec190`; the cluster builds a `MESHDX8` from heightmap samples and ends before the next registration. |
| `Tongas2/RENDERTEXTURE` | `0x004ed980-0x004ee0af` | 1734 | 17 | `RENDERTEXTUREMASTER` registration at `0x004ed980`; `"RENDERTEXTURE.txt"` manager factory at `0x004edcf0`; render-target state machine from `0x004ed9c0` to `0x004eebc0`. |
| `Tongas2/DXINPUT` | `0x004ee0b0-0x004ef08f` | 3905 | 24 | `INPUTDEVICE::Init` / `CreateKeyboard` / `CreateGamepad` / `CreateMouse` and `DXINPUT::StepMouse` log strings from `0x004ee0b0` to `0x004ee780`; ends where `SIMPLEMODELDX8` begins. |
| `Tongas2/SIMPLEMODELDX8` | `0x004ef090-0x004eff6f` | 3705 | 14 | `"SIMPLEMODELDX8RESOURCE::AddMeshPart> null master"` at `0x004ef720`; the object builds a `SIMPLEMODEL` and the master constructor at `0x004efe40` calls `SIMPLEMODELMASTER`; ends where the light helpers begin. |
| `Tongas2/LIGHTDX8` | `0x004eff70-0x004f241f` | 9107 | 47 | `PackColor`/`UnpackColor` at `0x004eff70`/`0x004effd0`; `LIGHTMASTER` constructor call at `0x004f0140`; `"LIGHT.txt"` manager at `0x004f0f40`; `"LIGHTDX8RESOURCE::OnActivate - Light type not supported"` at `0x004f10b0`; ends before the next `RegisterClassName` at `0x004f2420`. |

`docs/identification-004e5d50.md` guessed that `SCREENDECALMASTER`,
`BONEDMODELRESOURCE` and `CPRIMITIVE3RESOURCE` named further units. In this
range they do not: `?SetUserFolder@SCREENDECALMASTER` and
`?SetCorrectSizeInWidescreen@SCREENDECALMASTER` at `0x004ecf10`/`0x004ecf20`
are the `HEIGHTMAPDECALDX8MASTER` setters folded with the base-class copies,
and `?Virtual81@BONEDMODELRESOURCE` at `0x004ede40` is a three-byte folded
function inside the `RENDERTEXTURE` cluster. `CPRIMITIVE3RESOURCE`'s folded
`GetScaleFactor` is much later, at `0x00548d70`, still in the remaining
placeholder. The real derived classes here are `HEIGHTMAPDECALDX8`,
`RENDERTEXTURE`, `INPUTDEVICE`/`DXINPUT`, `SIMPLEMODELDX8` and `LIGHTDX8`.

## Scaffolds

Each unit has a header in `include/Tongas2/` and an empty-body scaffold in
`src/Tongas2/`. The scaffolds deliberately declare standalone classes
without their original base classes for now: inheriting from the engine
classes would make VC7.1 emit weak copies of their inline virtuals into the
object, and a later delink would take those short copies' sizes and cut the
target. The functions that are defined are the registrations, manager
factories, constructors/destructors and the handful of methods whose names
the strings give. Their exact decorated names and sizes are in
`config/symbols.txt`.

Ghidra's function sizes are not always the real body length. Several
tail-merged destructors and branches stop at the first `jz`, and the rest of
the body is not part of Ghidra's function. `??1RENDERTEXTUREMASTER` at
`0x004ede50` is reported as `0x2b` bytes but is really `0x3f` bytes; delinking
with `0x2b` made objdiff panic with `range end index 45 out of range for
slice of length 43`. The scaffold symbols therefore use the next function's
start minus the linker's trailing `int3` padding, not `build/functions.txt`
verbatim.

## Measured before and after

Full report (`py tools/report.py --objdiff tools\objdiff\objdiff-cli.exe`):

| measure | before | after |
|---|---|---|
| `matched_code` | 600559 | **600559** (unchanged) |
| `total_code` | 2404235 | 2403419 |
| `matched_code_percent` | 24.979214 | 24.987694 |
| `fuzzy_match_percent` | 33.929325 | 33.94673 |
| `total_functions` | 14015 | 14016 |
| `matched_functions` | 3456 | 3456 |
| `total_units` | 259 | 265 |

The five new units account for 25,888 bytes of code (7437 + 1734 + 3905 +
3705 + 9107). The placeholder `unassigned/004ebbcf` (451,121 bytes, 1556
functions) became a 1-byte gap at `0x004ebbcf` (the `int3` before the first
function) plus `unassigned/004f2420` (424,416 bytes, 1427 functions). The
remaining 816-byte difference in global `total_code` is the trailing `int3`
padding that `tools/split_functions.py` removes from each delinked unit, the
same effect earlier carves show.

## Matching started on RENDERTEXTURE

`RENDERTEXTURE` is the smallest carve and is the first one worked on. Four of
its seventeen functions are exact so far:

- `?RegisterCommands@RENDERTEXTUREMASTER@@SAXXZ` at `0x004ed980`
- `?CreateRENDERTEXTURE@@YAPAVRESOURCEMANAGER@@XZ` at `0x004edcf0`
- `?CreateMaster@RENDERTEXTUREMANAGER@@UAEPAVRESOURCEMASTER@@PBD@Z` at `0x004edf50`
- `??1RENDERTEXTUREMASTER@@UAE@XZ` at `0x004ede50`

`py tools/unit_status.py Tongas2/RENDERTEXTURE --functions` reports 20.70%
exact (359/1734 bytes), 23.08% fuzzy, 4/17 functions. The unit stays
incomplete. The remaining functions are the constructor
`??0RENDERTEXTUREMASTER` (its two `CSTRING` members and twelve scalar fields
are in place; the constructor's inlined empty-string handling is not written
yet), `??0RENDERTEXTURE`, the render-target state machine at `0x004ed9c0`,
`0x004eda30`, `0x004edb10`, `0x004edb80` and `0x004edbc0`, and the
`RENDERTEXTUREMANAGER` reference-count helpers at `0x004ede90`, `0x004edef0`
and `0x004edf20`.

The four matches needed one source-shape hint: the exe calls the large
`RENDERTEXTUREMASTER` constructor out of line from `CreateMaster`, while our
five-byte scaffold constructor was inlined and used different registers.
`__declspec(noinline)` on the scaffold constructor restores the call and
`CreateMaster` matches 100%; it can come off once the constructor itself is
written.

None of the five units is complete. The other four are still at their
scaffold percentages (0.37%-1.53% fuzzy), and all five are recorded without
`"complete": true`.

Verified with the original `Tornado.exe` (SHA-1 `20bf7200...06bc`) and a
private Ghidra project copy at `%LOCALAPPDATA%\tornado-jockey\ghidra-carve-engine`.
