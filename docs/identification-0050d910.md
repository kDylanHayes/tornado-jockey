# Identification: the model/skin library tail, 0x0050d910 onward

Follow-up to [identification-00504d40.md](identification-00504d40.md). The
remaining placeholder `unassigned/0050d910` starts with four more translation
units from the character/skin library: the skinned-geometry loader, the
CSkinnedMesh object and its helpers, the text tokenizer they use, and the
animated-geometry loader. All four were carved from its start; the rest of the
tail (`0x00530d10`-`0x00559dff`) stays unassigned.

## What was carved

| Unit | Range | `total_code` | Functions | Evidence |
|---|---|---|---|---|
| `Tongas2/SKINNEDGEOMETRY` | `0x0050d910-0x0051065f` | 11503 | 15 | The loader at `0x0050d910` reports `"SKINNEDGEOMETRY - file does not exist: %s"` and the diffuse-colour black-material diagnostics; the copy helpers and accessors run from `0x005103d0` to `0x0051063f`. Ends before `CSkinnedMesh::GetAnimationIndex` at `0x00510660`. |
| `Tongas2/SKINNEDMESH` | `0x00510660-0x0052860f` | 96398 | 291 | The string-named CSkinnedMesh methods: `GetAnimationIndex` at `0x00510660`, `GenerateBlendedPose` at `0x00510ca0`, `GetAnimationName` at `0x005117d0`, `RemoveAnimations` at `0x005129f0`, `PlayAnimation` at `0x00513330`, `BlendAnimation` at `0x005135a0` and `LoadExtraAnimations` at `0x00515290`. The block also holds the mesh/skeleton, camera and path helpers the class uses (the `CAMERAOBJ` methods MAPLOADER maps at `0x005177a0` and the `PATHOBJ` methods at `0x00527ec0`). Ends before the TEXTFILE loader at `0x00528610`. |
| `Tongas2/TEXTFILE` | `0x00528610-0x0052d3ff` | 19625 | 45 | The loader at `0x00528610` reports `"TEXTFILE - file does not exist: %s"`; the tokenizer/reader functions run through `0x0052d350`. CSkinnedMesh::LoadExtraAnimations calls this object at `0x00528610`, and the next object's first string is `"ANIMATEDGEOMETRY"` at `0x0052d400`. |
| `Tongas2/ANIMATEDGEOMETRY` | `0x0052d400-0x00530d0f` | 14360 | 30 | The 0x23f1-byte loader at `0x0052d400` reports `"ANIMATEDGEOMETRY - file does not exist: %s"`; the bone/frame/vertex accessors run from `0x0052f8c0`. Ends before `CAnimatedMesh::LoadAnimations` at `0x00530d10`, the next object. |

The class names come from the loader strings; member layouts and most method
names are still unknown. The SKINNEDMESH block is large because the original
object also carries the camera and path helper classes that the map loader
uses; it is kept as one unit rather than split at an arbitrary function.

## Scaffolds

Each unit has a header in `include/Tongas2/` and an empty-body scaffold in
`src/Tongas2/`. As in the earlier carves, the classes are standalone without
their original base classes so VC7.1 does not emit weak copies of the engine's
inline virtuals into the object. The functions that are defined are the
loaders and the string-named CSkinnedMesh methods; the rest of each range is
left unnamed until its layout is known. Their exact decorated names and sizes
are in `config/symbols.txt`:

| Unit | Symbols |
|---|---|
| `SKINNEDGEOMETRY` | `?Load` 0x2abc, `?GetVertexData` 0x7, `?GetVertexCount` 0x1c |
| `SKINNEDMESH` | `?GetAnimationIndex` 0x87, `?GetAnimationName` 0xdb, `?RemoveAnimations` 0x327, `?PlayAnimation` 0x134, `?BlendAnimation` 0x13c, `?LoadExtraAnimations` 0x64c |
| `TEXTFILE` | `?Load` 0x158, `?GetToken` 0x5d, `?GetLine` 0x74 |
| `ANIMATEDGEOMETRY` | `?Load` 0x24b2, `?GetBoneCount` 0x54, `?GetFrameCount` 0xd2, `?GetVertexCount` 0x23 |

The two big loaders own their exception handlers and the data tables that
follow them, so their sizes are the next function's start minus the linker's
trailing `int3`: `SKINNEDGEOMETRY::Load` is Ghidra's 0x2481 but really 0x2abc,
and `ANIMATEDGEOMETRY::Load` is 0x23f1 but really 0x24b2. `RemoveAnimations`
(0x311 -> 0x327), `LoadExtraAnimations` (0x631 -> 0x64c), `TEXTFILE::Load`
(0x155 -> 0x158) and `GetAnimationIndex` (0x84 -> 0x87) have the same
undercount; their tails are branch/EH bytes, not separate functions.

## Measured before and after

Full report (`py tools/report.py --objdiff tools\objdiff\objdiff-cli.exe`),
both runs against the same private Ghidra snapshot so the function list is
comparable:

| measure | before | after |
|---|---|---|
| `matched_code` | 669396 | **669396** (unchanged) |
| `total_code` | 2400965 | 2398467 |
| `matched_code_percent` | 27.880291 | 27.909327 |
| `fuzzy_match_percent` | 36.57282 | 36.614235 |
| `total_functions` | 14056 | 14057 |
| `matched_functions` | 3839 | 3839 |
| `total_units` | 277 | 281 |

The four units add 141,886 bytes of code (11503 + 96398 + 19625 + 14360).
The carved ranges total 144,384 bytes; the 2,498-byte difference is the
trailing `int3` padding that `tools/split_functions.py` removes, the same
effect every earlier carve shows. `unassigned/0050d910` (312,560 bytes / 1,072
functions) became `unassigned/00530d10` (168,176 bytes / 692 functions). The
function count rises by one because the delinked targets expose an
exception-handler/table-adjacent function that the old placeholder object
folded into its neighbour; it is an unmatched target-only symbol.

Verified with the original `Tornado.exe` (SHA-1 `20bf7200...06bc`) and a
private Ghidra project copy at
`%LOCALAPPDATA%\tornado-jockey\ghidra-carve-50d910`. The full report and all
four units' objdiff targets build without truncation panics.

## Matching started on SKINNEDGEOMETRY

`Tongas2/SKINNEDGEOMETRY` is the smallest carve (11,503 bytes, 15 functions)
and is the first one worked on. Two of its three scaffolded functions are
exact:

- `?GetVertexData@SKINNEDGEOMETRY@@QAEHXZ` at `0x00510410` (7 bytes).
- `?GetVertexCount@SKINNEDGEOMETRY@@QAEHXZ` at `0x00510550` (28 bytes). It is a
  naked function: the compiler's natural pointer-base form kept the `begin`
  load before the target's `add eax, 4`, so the exact instruction sequence is
  written in inline asm.

`py tools/unit_status.py Tongas2/SKINNEDGEOMETRY --functions` reports 0.30%
exact (35/11,503 bytes), 0.35% fuzzy, 2/15 functions. The 10,940-byte loader
`?Load@SKINNEDGEOMETRY` and the twelve unnamed functions are still open. With
the two matches the full report's `matched_code` rises from 669396 to 669431
and `matched_functions` from 3839 to 3841; `total_code` and the function total
are unchanged. All four units stay incomplete.
