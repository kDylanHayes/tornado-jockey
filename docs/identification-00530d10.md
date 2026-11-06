# Identification: the animated-mesh/texture tail, 0x00530d10 onward

Follow-up to [identification-0050d910.md](identification-0050d910.md). The
remaining placeholder `unassigned/00530d10` starts with four more translation
units: CAnimatedMesh, SKINNEDANIMATION, the texture object (CTexture plus
TEXTUREDX8SDTEXTURE) and TEXTURERESIZER. All four were carved from its start;
the rest of the tail (`0x00535fd0`-`0x00559dff`) stays unassigned.

## What was carved

| Unit | Range | `total_code` | Functions | Evidence |
|---|---|---|---|---|
| `Tongas2/CANIMATEDMESH` | `0x00530d10-0x00532baf` | 7637 | 32 | The loader at `0x00530d10` reports `"CAnimatedMesh::LoadAnimations - Try to load empty file for animation"` and the TEXTFILE error; its methods build from ANIMATEDGEOMETRY (`0x0052f920`) and call LoadAnimations (`0x00530d10`). Ends before the SKINNEDANIMATION loader at `0x00532bb0`. |
| `Tongas2/SKINNEDANIMATION` | `0x00532bb0-0x0053464f` | 6744 | 10 | The loader at `0x00532bb0` reports `"SKINNEDANIMATION - file does not exist: %s"` and `"EMPTY"`; the accessors run through `0x00534630`. Ends before `TEXTUREDX8SDTEXTURE::GetSurface` at `0x00534650`. |
| `Tongas2/TEXTURE` | `0x00534650-0x005358ef` | 4688 | 10 | `TEXTUREDX8SDTEXTURE::GetSurface` and its destructor at `0x00534650`/`0x00534df0`; the `CTexture::ConvertToFormat` error strings at `0x00534660`, `CTexture::MergeAlpha` at `0x00534a00`, and the texture-creation errors at `0x00534e70`. Ends before the TEXTURERESIZER updater at `0x005358f0`. |
| `Tongas2/TEXTURERESIZER` | `0x005358f0-0x00535fcf` | 1722 | 7 | The updater at `0x005358f0` reports the `"TEXTURERESIZER::UpdateTextures"` errors; the remaining methods run through `0x00535fcf`. Ends before the `VERTEXMODELDX8SDMASTER` registration at `0x00535fd0`. |

The class names come from the loader and error strings; member layouts and
most method names are still unknown. `TEXTURE` reuses the existing
`TEXTUREDX8SDTEXTURE` declaration in `TEXTUREDX8SD.h`, whose two out-of-line
methods are compiled in this object, and adds the separate `CTEXTURE` class.

## Scaffolds

Each unit has a header in `include/Tongas2/` and an empty-body scaffold in
`src/Tongas2/`. As in the earlier carves, the classes are standalone without
their original base classes so VC7.1 does not emit weak copies of the engine's
inline virtuals into the object. Their exact decorated names and sizes are in
`config/symbols.txt`:

| Unit | Symbols |
|---|---|
| `CANIMATEDMESH` | `?LoadAnimations` 0x7a2, `?Load` 0x230 |
| `SKINNEDANIMATION` | `?Load` 0xfdd, `?GetBoneCount` 0xba, `?GetFrameCount` 0xb5 |
| `TEXTURE` | `?GetSurface@TEXTUREDX8SDTEXTURE` 0x4, `?ConvertToFormat@CTEXTURE` 0x29c, `??1TEXTUREDX8SDTEXTURE` 0x74 |
| `TEXTURERESIZER` | `?UpdateTextures` 0x1fb, `?Create` 0x78, `?Destroy` 0x6c |

The loaders own their exception handlers and data tables, so their sizes are
the next function's start minus the linker's trailing `int3`:
`CAnimatedMesh::LoadAnimations` is Ghidra's 0x783 but really 0x7a2,
`SKINNEDANIMATION::Load` is 0xf9d but really 0xfdd, `CAnimatedMesh::Load` is
0x21c but really 0x230, and `~TEXTUREDX8SDTEXTURE` is 0x71 but really 0x74.
The other selected functions are clean Ghidra extents.

## Measured before and after

Full report (`py tools/report.py --objdiff tools\objdiff\objdiff-cli.exe`),
both runs against the same private Ghidra snapshot so the function list is
comparable:

| measure | before | after |
|---|---|---|
| `matched_code` | 670012 | **670012** (unchanged) |
| `total_code` | 2401797 | 2401404 |
| `matched_code_percent` | 27.896278 | 27.900845 |
| `fuzzy_match_percent` | 36.605373 | 36.613167 |
| `total_functions` | 14076 | 14076 |
| `matched_functions` | 3850 | 3850 |
| `total_units` | 282 | 286 |

The four units add 20,791 bytes of code (7637 + 6744 + 4688 + 1722). The
carved ranges total 21,184 bytes; the 393-byte difference is the trailing
`int3` padding that `tools/split_functions.py` removes, the same effect every
earlier carve shows. `unassigned/00530d10` (168,176 bytes / 692 functions)
became `unassigned/00535fd0` (146,992 bytes / 633 functions). The function
count is conserved exactly: the four units carry 59 of the old placeholder's
functions.

Verified with the original `Tornado.exe` (SHA-1 `20bf7200...06bc`) and a
private Ghidra project copy at
`%LOCALAPPDATA%\tornado-jockey\ghidra-carve-530d10`. The full report and all
four units' objdiff targets build without truncation panics.

## Matching

None of the four units is matched yet. `Tongas2/TEXTURERESIZER` is the
smallest (1,722 bytes, 7 functions) and is the first candidate. All four are
recorded without `"complete": true`.
