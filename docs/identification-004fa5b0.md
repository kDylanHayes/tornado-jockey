# Identification: the renderer-plugin tail, 0x004fa5b0 onward

Follow-up to [identification-004f8350.md](identification-004f8350.md). The
remaining placeholder `unassigned/004fa5b0` starts with four more translation
units. All four were carved from its start; the rest of the tail
(`0x00504d40`-`0x00559dff`) stays unassigned.

## What was carved

| Unit | Range | `total_code` | Functions | Evidence |
|---|---|---|---|---|
| `Tongas2/SETTINGS` | `0x004fa5b0-0x004fb45f` | 3708 | 5 | The constructor at `0x004fa5b0` checks the global settings enumeration and reports `"Settings enumeration is out of order!"` / `"SETUP ERROR"`; the large parser at `0x004fa830` reads `config.dat`, `JOYAXISMAP` and `JOYBUTTONMAP` and logs `"Overriding Wanako settings with config.dat file"`. Ends at the `int3` run before `0x004fb460`. |
| `Tongas2/MODEL` | `0x004fb460-0x00502e2f` | 30331 | 104 | A model/material/texture container cluster: the constructor at `0x004fb4a0` initialises eight `std::string` members, the destructor at `0x004fcbe0` walks twelve vectors, `0x004fbc80` looks an entry up by four string keys, and the loaders at `0x004fe350`, `0x004fec10`, `0x00500900`, `0x00500b20` and `0x00500de0` build model, mesh and texture objects. The texture constructor at `0x00501a50` initialises seven strings and allocates the texture resizer; code at `0x00501100` reports `"SCREWDRIVER ERROR ..."`, and the `"AVI"`/`"DDS"` loaders follow. Ends before the next `RegisterClassName`. |
| `Tongas2/MODELDX8SD` | `0x00502e30-0x0050420f` | 4966 | 19 | `RegisterClassName` at `0x00502e30` names `MODELDX8SDMASTER` with parent `RESOURCEMASTER`; `0x00502e70` returns the `"MDL3"` type code; the manager factory at `0x00502e80` creates the `MODELDX8SD.txt` manager; the master constructor/destructor and material/texture methods run to `0x00504170`. Ends before the `TEXTUREDX8SDMASTER::BuildDiffuse...` strings. |
| `Tongas2/TEXTUREDX8SD` | `0x00504210-0x00504d3f` | 2773 | 18 | The first function at `0x00504210` references `TEXTUREDX8SDMASTER::BuildDiffuseColorTexture ...` and `TEXTUREDX8SDMASTER::BuildDiffuseTexture ...`; the master destructor at `0x005043c0` sets the vtable at `0x00677f08`, and the manager destructor at `0x00504360` sets the vtable at `0x00677e34`. Ends before `0x00504d40`, which starts the renderer setup code (`"VRAM requested before D3D setup completed"`, `"DETECTION ERROR"`). |

The names are descriptive, not recovered from strings: only
`MODELDX8SDMASTER` and `TEXTUREDX8SDMASTER` are named by the binary (the
first by `RegisterClassName`, the second by its error messages). The
`MODEL` cluster is a single contiguous object by function order and vtable
data, but it clearly covers several related model/texture classes.

## Scaffolds

Each unit has a header in `include/Tongas2/` and an empty-body scaffold in
`src/Tongas2/`. As in the earlier carves, the scaffolds declare standalone
classes without their original base classes so VC7.1 does not emit weak
copies of the engine's inline virtuals into the object. The functions that
are defined are the constructor/destructor, lookup/load/create helpers,
registrations and manager factory. Their exact target sizes are in
`config/symbols.txt`:

| Unit | Symbols |
|---|---|
| `SETTINGS` | `??0SETTINGS` 0x16a, `?PopFront` 0x55, `?ClearAll` 0x93, `?Destroy` 0x5, `?Load` 0xc25 |
| `MODEL` | `??0MODEL` 0x73, `?Find` 0x2d6, `??1MODEL` 0x93f, `?Load` 0x8c0, `?Create` 0x3e3 |
| `MODELDX8SD` | `?RegisterCommands` 0x3f, `?GetTypeCode` 0x6, `?CreateMODELDX8SD` 0x80 |
| `TEXTUREDX8SD` | `?BuildDiffuseColorTexture` 0x113, `??1TEXTUREDX8SDMANAGER` 0xb, `??1TEXTUREDX8SDMASTER` 0x6a |

The sizes are the next function's start minus the linker's trailing `int3`
padding where Ghidra undercounts, notably the SETTINGS constructor
(0x164 -> 0x16a) and parser (0xbf8 -> 0xc25), and the MODEL destructor
(0x8ec -> 0x93f) and loaders. The existing `vector<PATHREPOPOINT>` destructor
at `0x004fbbb0` was also given its real size (0x27 -> 0x2a). Functions that
are followed by an embedded jump table (for example `0x00502ff0` and
`0x00503dd0`) keep Ghidra's size; walking back from the next function would
swallow the table.

`0x00502e70` (`MODELDX8SDMANAGER::GetTypeCode`, returns `"MDL3"`) was not a
Ghidra function at all: it sits between the registration and the factory and
was inside the previous function's extent. It is now named and defined, and
is the only exact match so far (6 bytes).

## Measured before and after

Full report (`py tools/report.py --project
"%LOCALAPPDATA%\tornado-jockey\ghidra-carve-tail" --objdiff
tools\objdiff\objdiff-cli.exe`):

| measure | before | after |
|---|---|---|
| `matched_code` | 630360 | **630366** |
| `total_code` | 2403048 | 2401930 |
| `matched_code_percent` | 26.231686 | 26.244146 |
| `fuzzy_match_percent` | 34.87742 | 34.896667 |
| `total_functions` | 14155 | 14156 |
| `matched_functions` | 3639 | 3640 |
| `total_units` | 269 | 273 |

The four units account for 41,778 bytes of code (3708 + 30331 + 4966 + 2773).
`unassigned/004fa5b0` (391,248 bytes / 1,340 functions) became
`unassigned/00504d40` (348,352 bytes / 1,195 functions). The four carved
ranges total 42,896 bytes; the 1,118-byte difference is trailing `int3`
padding removed by `tools/split_functions.py`. The function count rises by
one because `0x00502e70` was not a Ghidra function in the placeholder.

Verified with the original `Tornado.exe` (SHA-1 `20bf7200...06bc`) and the
private Ghidra project copy at
`%LOCALAPPDATA%\tornado-jockey\ghidra-carve-tail`. The report and all four
objdiff targets build without truncation panics.

## SETTINGS matching

The settings object shares its class name with the table object inside
`INTRO`; both defined `SETTINGS::ClearAll`, so the delinker rejected the
target with a duplicate-symbol error. The table object in `INTRO` was renamed
`SETTINGSTABLE` (its methods and the `UpdateQuality` mapping moved with it);
`SETTINGS` is now the queue/config object at `0x004fa5b0`.

Three of the five functions are exact:

- `?PopFront@SETTINGS@@QAEXXZ` (`0x004fa720`, 100%)
- `?ClearAll@SETTINGS@@QAEXXZ` (`0x004fa780`, 100%)
- `?Destroy@SETTINGS@@QAEXXZ` (`0x004fa820`, 100%, the `jmp ClearAll`)

`??0SETTINGS` is 96.44% fuzzy: its length already matches the target's
`0x16a` bytes, and the two `std::string` arguments, arrays and
`SETTINGSTABLE` calls are in place; the remaining difference is the
compiler's register allocation around `new std::string[2]` and the string
assignments. `?Load@SETTINGS@@QAEXPBD@Z` is still
the empty scaffold; it is the 0xc25-byte config.dat/JOYAXISMAP/JOYBUTTONMAP
parser.

The remaining blockers are the exact constructor argument order/register
allocation and the parser's `std::string` token loop. The parser's helper
cluster is still in the uncovered tail: `0x005285c0`-`0x00528770` is a
`TEXTFILE` object (the only string there is `"TEXTFILE - file does not
exist: %s"`), with `0x00528610` the open/check, `0x005285c0` the constructor,
`0x005285d0`/`0x005285e0` the line and EOF checks, and `0x00528820` the
tokenizer. It needs its own class/unit before the config.dat parser can be
written exactly. The constructor is otherwise byte-for-byte except for the
eight-byte `new std::string[2]` null-check shape (`m_1038` is stored through
`eax` in our object and through `edi` in the target). The unit stays
incomplete.

## Matching TEXTUREDX8SD

`TEXTUREDX8SD` is the smallest carve (2,750 bytes in the final report). Ten
of its eighteen functions are exact:

- `?GetTexture@TEXTUREDX8SDMASTER@@QAEPAXXZ`
- `?GetSurface@TEXTUREDX8SDMASTER@@QAEPAXXZ`
- `?ReleaseTexture@@YGXPBURESHANDLE@@@Z`
- `??1TEXTUREDX8SDMANAGER@@UAE@XZ`
- `??_GTEXTUREDX8SDMANAGER@@UAEPAXI@Z`
- `??1TEXTUREDX8SDMASTER@@UAE@XZ`
- `??_GTEXTUREDX8SDMASTER@@UAEPAXI@Z`
- `?CreateMaster@TEXTUREDX8SDMANAGER@@UAEPAVRESOURCEMASTER@@PBD@Z`
- `?GetTextureResource@@YGPAVTEXTUREDX8SDMASTER@@PBURESHANDLE@@@Z`
- `?GetHandleRGB@TEXTUREDX8SDMANAGER@@QAE?AURESHANDLE@@HHPBHPBD@Z`

`py tools/unit_status.py Tongas2/TEXTUREDX8SD` reports **22.04% exact
(606/2,750 bytes)**, **24.88% fuzzy**, and **10/18 functions exact**. The
unit stays incomplete.

Remaining blockers:

- The master constructor (`0x00504570`, 0x21f bytes) still has a stub body; its
  flag/layout work decides `Load` and most of the texture builders.
- `BuildDiffuseColorTexture` (`0x00504210`), `Build` (`0x00504bb0`),
  `Load` (`0x00504800`) and `MergeAlpha` (`0x00504990`) need the renderer and
  texture-object call shapes transcribed. `Build` also needs a real 0x8f-byte
  body before its symbol can be added without objdiff panicking.
- `UpdateTextures` (`0x00504c90`) and `GetHandle` (`0x00504330`) are close but
  not byte-exact yet: 0% and 19.74% respectively.
- `GetPhysicalMemoryMB` (`0x00504d10`) is 41%; the target has an extra
  `push edi`/`xor ecx,ecx` sequence around the imported
  `GlobalMemoryStatus` call that plain C++ does not reproduce.

`MODELDX8SD` was completed by another thread and is 100% in main; its source
and ranges were left untouched here.
