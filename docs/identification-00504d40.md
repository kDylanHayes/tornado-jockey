# Identification: the renderer-plugin tail, 0x00504d40 onward

Follow-up to [identification-004fa5b0.md](identification-004fa5b0.md). The
remaining placeholder `unassigned/00504d40` starts with four more translation
units: the hardware/device detector, the Direct3D 8 sample application
framework the plugin is built on, the plugin's concrete application object,
and the SCRoot character object. All four were carved from its start; the rest
of the tail (`0x0050d910`-`0x00559dff`) stays unassigned.

## What was carved

| Unit | Range | `total_code` | Functions | Evidence |
|---|---|---|---|---|
| `Tongas2/HARDWARE` | `0x00504d40-0x0050618f` | 5029 | 7 | `0x00504d40` reports `"VRAM requested before D3D setup completed"` / `"DETECTION ERROR"`; `0x00504d70` reads `Hardware\Description\System\CentralProcessor\0` `~MHz`; `0x00504dd0` maps `GetVersionExA` to an OS id; `0x00504f10`/`0x00504f50` are the capability accessors with the `"D3D Capability ..."` errors; `0x00504f90` is the constructor (`"UNKNOWN"` plus the capability arrays); and the 4403-byte `0x00504ff0` fills the vendor/device name tables (`NVIDIA GEFORCE FX`, `VOODOO3`, `SAVAGE`, ...). Ends before the D3D application object at `0x00506190`. |
| `Tongas2/D3DAPP` | `0x00506190-0x00508a6f` | 10325 | 23 | The base Direct3D 8 sample application: `0x00506360` reports `"FAILED RESTORE"`, `0x00506760` has the standard `"Could not initialize Direct3D..."`, `"No hardware-accelerated Direct3D devices were found."` and `"Could not find any compatible Direct3D devices."` boxes, `0x005082b0` is `Create` (calls `Direct3DCreate8`, registers the `"DX8Game"` window class, reports `"Failed Initialize3DEnvironment"` / `"Failed OneTimeSceneInit"` / `"Failed ChooseInitialD3DSettings"`), and the vtable at `0x00678978` names `0x005082b0`, `0x00508040`, `0x00506c20`, `0x00507b10` and `0x00506510`. The constructor at `0x005081a0` sets the default 0x280x0x1e0 window and `DAT_00a024f4`. Ends before the derived constructor at `0x00508a70`. |
| `Tongas2/DX8APP` | `0x00508a70-0x0050b3df` | 10176 | 66 | The plugin's concrete application: `0x00508a70` chains to `D3DAPP`'s constructor and sets the primary/secondary vtables `0x00678e68`/`0x00678e18`; `0x005098a0` allocates the 0x2c4-byte object; `0x00509900` is the destructor and `0x005099f0` creates `HARDWARE` (0x54 bytes through `0x00504f90`) and `SETTINGS` (0x103c bytes through `0x004fa5b0`); `0x00509d30` is the static registration that installs the plugin's resource-manager factories (it points at `0x00539950`, `0x0053a980`, `0x0053b520`, and the model/scene factories). The template instantiations the class pulls in run through `0x0050b3df`. Ends before the SCRoot constructor at `0x0050b3e0`. |
| `Tongas2/SCROOT` | `0x0050b3e0-0x0050d90f` | 9375 | 27 | `0x0050b3e0` copies three 16-dword blocks and an embedded string into a 0x134-byte object; `0x0050b480` creates one with the literal `"SCRoot"`; `0x0050b600` is the 6065-byte loader/updater that calls the vector helpers at `0x0050a5a0` and `0x0050a670`; the object's small methods run through `0x0050d8ff`. Ends before `0x0050d910`, the `"SKINNEDGEOMETRY - file does not exist"` loader. |

The boundaries are function-cluster boundaries, not recovered source-file
names. They line up with the vtable/constructor chains and the string
allocation order: the hardware strings sit at `0x006780e8`-`0x00678304`, the
sample-application strings at `0x0067830c`-`0x00678f58`, and the next object's
first string is `"SCRoot"` at `0x0067905c`. The D3DAPP/DX8APP split is at the
derived constructor `0x00508a70`, whose first call is the base constructor
`0x005081a0`; `0x00508a00`/`0x00508a50` before it are still base-class
methods (`0x00508a50` is called from `0x00507df0`). The DX8APP/SCROOT split is
at `0x0050b3e0`, the constructor only `0x0050b480` calls; everything before it,
including the vector/map helpers at `0x0050a890`-`0x0050b3df`, is emitted as
the tail of the application object.

## Scaffolds

Each unit has a header in `include/Tongas2/` and an empty-body scaffold in
`src/Tongas2/`. As in the earlier carves, the classes are standalone without
their original base classes so VC7.1 does not emit weak copies of the engine's
inline virtuals into the object. The functions that are defined are the
constructors/destructors, factories, registrations and the largest helpers
whose names the strings give. Their exact decorated names and sizes are in
`config/symbols.txt`:

| Unit | Symbols |
|---|---|
| `HARDWARE` | `?HardwareC` 0x23, `?HardwareA` 0x54, `?HardwareD` 0xd8, `??0HARDWARE` 0x5b, `?Detect@HARDWARE` 0x118f |
| `D3DAPP` | `?Cleanup3DEnvironment` 0x7c, `?Initialize3DEnvironment` 0x3ac, `?Run` 0x158, `??0D3DAPP` 0x107, `?Create` 0x74d |
| `DX8APP` | `??0DX8APP` 0x80, `?CreateDX8APP` 0x59, `??1DX8APP` 0xc9, `?Init@DX8APP` 0x224, `?RegisterDX8Plugin` 0x127 |
| `SCROOT` | `??0SCROOT` 0x9d, `?CreateSCROOT` 0x126, `?Init@SCROOT` 0x4d, `?Load@SCROOT` 0x17b4 |

Several functions own their jump/decision tables, and the Ghidra function
extent stops at the first `ret`. The sizes above include those tables, because
otherwise objdiff's x86 analyser reads a switch target past the function
section and panics (`range start index ... out of range for slice of length
...`): `HardwareD` is Ghidra's 0xc1 plus its five-entry table (0xd8);
`Detect` is 0x1133 plus the pointer and decision tables (0x118f);
`Initialize3DEnvironment` is 0x364 plus its tables (0x3ac). `D3DAPP::Create`
is Ghidra's 0x642 plus the exception-handler block at `0x005088b7` (0x74d).
The destructor `??1DX8APP` is Ghidra's 0xae but really 0xc9, `Init` 0x221 but
really 0x224, `CreateSCROOT` 0x123 but really 0x126, and `Load@SCROOT` 0x17b1
but really 0x17b4; those tails are a branch and its trailing `int3`, not a
table.

## Measured before and after

Full report (`py tools/report.py --objdiff tools\objdiff\objdiff-cli.exe`),
both runs against the same private Ghidra snapshot so the function list is
comparable:

| measure | before | after |
|---|---|---|
| `matched_code` | 645389 | **645389** (unchanged) |
| `total_code` | 2401852 | 2400965 |
| `matched_code_percent` | 26.870474 | 26.880402 |
| `fuzzy_match_percent` | 35.556328 | 35.573257 |
| `total_functions` | 14049 | 14056 |
| `matched_functions` | 3738 | 3738 |
| `total_units` | 273 | 277 |

The four units add 34,905 bytes of code (5029 + 10325 + 10176 + 9375). The
carved ranges total 35,792 bytes; the 887-byte difference is the trailing
`int3` padding that `tools/split_functions.py` removes, the same effect every
earlier carve shows. `unassigned/00504d40` (348,352 bytes / 1,188 functions)
became `unassigned/0050d910` (312,560 bytes / 1,072 functions). The function
count rises by seven because the delinked targets now expose the exception
handler inside `D3DAPP::Create` and a handful of jump-table-adjacent functions
that the old placeholder object folded into their neighbours; all seven are
unmatched target-only symbols.

Verified with the original `Tornado.exe` (SHA-1 `20bf7200...06bc`) and a
private Ghidra project copy at
`%LOCALAPPDATA%\tornado-jockey\ghidra-carve-504d40`. The full report and all
four units' objdiff targets build without truncation panics.

## Matching not started

None of the four units is matched yet. `Tongas2/HARDWARE` is the smallest
(5,029 bytes, 7 functions) and is the first candidate. All four are recorded
without `"complete": true`.
