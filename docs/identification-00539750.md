# Identification: the SD renderer/sound tail, 0x00539750 onward

Follow-up to [identification-00535fd0.md](identification-00535fd0.md). The
remaining placeholder `unassigned/00539750` starts with the SD renderer's
render textures, its shaders, the DX8 texture core and the FMOD sound wrapper.
All four were carved out; the tail from `0x00548420` onward stays unassigned.

## What was carved

| Unit | Range | `total_code` | Functions | Evidence |
|---|---|---|---|---|
| `Tongas2/RENDERTEXTUREDX8SD` | `0x005398d0-0x0053a97f` | 4143 | 15 | The manager factory at `0x00539950` creates `"RENDERTEXTURE.txt"`; `0x005398d0` creates the 0x90-byte object with vtable `0x0067a760`; `0x00539a10` is the first resource method. This is the SD plugin's render-texture object, distinct from the earlier `Tongas2/RENDERTEXTURE`. Ends before the SHADER factory at `0x0053a980`. |
| `Tongas2/SHADERDX8SD` | `0x0053a980-0x0053dfdf` | 13552 | 66 | The factory at `0x0053a980` creates `"SHADER.txt"`; `0x0053aa50` is the 0x880-byte `SHADERDX8SDRESOURCE::Apply` whose errors are `"Error: blending operation not valid"`, `"could not set the shader"` and `"master null"`. Ends before the texture core's vector helper at `0x0053dfe0`. |
| `Tongas2/TEXTUREDX8CORE` | `0x0053dfe0-0x005478df` | 38439 | 108 | The block carries the vector/deque helpers at `0x0053dfe0`, `0x00542000` and `0x00546c50`, and the texture loader at `0x00546ea0` whose errors are `"Texture:%s could not be created"`, `"PNG FORMAT NOT SUPPORTED"` and `"Texture is now power of 2"`. Ends before the FMOD error strings at `0x005478e0`. |
| `Tongas2/SOUNDFMOD` | `0x005478e0-0x0054841f` | 2788 | 14 | `0x005478e0` maps FMOD results to the messages (`"Error initializing output device"`, `"Soundcard does not support the features..."`); `0x00547a50`/`0x005480e0` are the volume and sample paths (`"Can't change volume 2 of '%s'"`, `"Error playing sample:%s"`). Ends before the ACTOR/ODEWORLD object at `0x00548420`. |

`0x00539750` is a data pointer followed by padding, and the next function is at
`0x005398d0`. The pointer and its 0x180-byte gap stay in a tiny
`unassigned/00539750` placeholder because `split_functions.py` rejects a unit
section with contents before its first symbol; the RENDERTEXTUREDX8SD unit
therefore starts at `0x005398d0`.

`LoadMasterSound` at `0x00547d20` is deliberately left unnamed: its real
extent runs into the one-byte `?term_source` JPEG mislabel at `0x00547f50`,
which belongs to another unit.

## Scaffolds

Each unit has a header in `include/Tongas2/` and an empty-body scaffold in
`src/Tongas2/`. The exact decorated names and sizes are in
`config/symbols.txt`:

| Unit | Symbols |
|---|---|
| `RENDERTEXTUREDX8SD` | `?CreateRENDERTEXTUREDX8SDMaster` 0x80, `?CreateRENDERTEXTUREDX8SD` 0x86, `?GetRENDERTEXTUREFormat` 0x70 |
| `SHADERDX8SD` | `?CreateSHADERDX8SD` 0x86, `?Apply@SHADERDX8SDRESOURCE` 0x880 |
| `TEXTUREDX8CORE` | `?CreateTexture` 0x9d4, `?TextureFormatA` 0x36, `?TextureFormatB` 0x1b |
| `SOUNDFMOD` | `?FMODErrorString` 0xe0, `?SetVolume2` 0x2ce, `?PlaySample` 0x20a |

Several functions own their jump/decision tables, so their symbol sizes are
the next function's start minus the trailing `int3`, not Ghidra's first-`ret`
extent: `SHADERDX8SDRESOURCE::Apply` is 0x83e but really 0x880,
`CreateTexture` 0x99e but really 0x9d4, `FMODErrorString` 0x8f but really
0xe0, and `GetRENDERTEXTUREFormat` 0x63 but really 0x70. Without the table
bytes objdiff's x86 analyser reads past the function section and panics.

## Measured before and after

Full report (`py tools/report.py --objdiff tools\objdiff\objdiff-cli.exe`),
both runs against the same private Ghidra snapshot so the function list is
comparable:

| measure | before | after |
|---|---|---|
| `matched_code` | 670007 | **670007** (unchanged) |
| `total_code` | 2400931 | 2399613 |
| `matched_code_percent` | 27.906132 | 27.921463 |
| `fuzzy_match_percent` | 36.622143 | 36.644314 |
| `total_functions` | 14079 | 14084 |
| `matched_functions` | 3849 | 3849 |
| `total_units` | 290 | 295 |

The four units add 58,922 bytes of code (4143 + 13552 + 38439 + 2788). The
carved ranges total 60,240 bytes; the 1,318-byte difference is the trailing
`int3` padding that `tools/split_functions.py` removes, the same effect every
earlier carve shows. `unassigned/00539750` (132,784 bytes / 569 functions)
became `unassigned/00539750` (384 bytes / 1 function) plus
`unassigned/00548420` (72,160 bytes / 370 functions): 72,544 bytes and 371
functions remain. The function count rises by five because the delinked
targets expose catch/table-adjacent helpers that the old placeholder object
folded into its neighbours; they are unmatched target-only symbols.

Verified with the original `Tornado.exe` (SHA-1 `20bf7200...06bc`) and a
private Ghidra project copy at
`%LOCALAPPDATA%\tornado-jockey\ghidra-carve-539750`. The full report and all
four units' objdiff targets build without truncation panics.

## Matching

None of the four units is matched yet. `Tongas2/SOUNDFMOD` is the smallest
(2,788 bytes, 14 functions) and is the first candidate. All four are recorded
without `"complete": true`.
