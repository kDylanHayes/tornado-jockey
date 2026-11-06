# Identification: unassigned/004e5d50 (region-identify, 2026-11-04)

**Verdict: project code** — Wanako Games' Tongas2 DX8 renderer plugin plus game
code. Not CRT, Windows SDK, D3DX8, or any vendored library in this tree. Full
evidence is in the session artifact `identification-004e5d50.md`.

## What it is

`tools/report.py`'s maximal uncovered `.text` run:
`0x004E5D50` → `0x00559DFF` (475312 bytes, 1677 functions, matched_code 0,
category `game`). It starts one byte after the last delinked unit
`Tongas2/HEIGHTMAPDX8` (`0x004DE6D0-0x004E5D4F`) and ends exactly where
FreeImage 3.7.0 begins (`0x00559E00`). So it is the whole uncarved tail of the
`0x00482CD0-0x00559DFF` game/engine block — many translation units, not one.

## Why project code

- `config/regions.json` / `docs/regions.md`: that block is "Game and Tongas2
  engine code (renderer, resources, GUI, input, ACTOR/SCENE)".
- 140 named functions are MSVC 7.1 C++ Tongas2 classes: `INDEXBUFFER`,
  `VERTEXBUFFER`, `DX8CAMERA`, `MESHDX8`, `RENDERSHADERMANAGER`,
  `RENDERSHADERMASTER`, `HEIGHTMAPTEMP`, `SCREENDECALMASTER`,
  `BONEDMODELRESOURCE`, `CPRIMITIVE3RESOURCE`, `FILELOADER`, `CSTRING`, `WTINFO`.
  No CRT/SDK/library function symbols.
- ~480 referenced strings are game/engine diagnostics and asset paths
  (`Overriding Wanako settings with config.dat file`, `SCREWDRIVER ERROR ...`,
  `RenderMesh::Create - Could not create vertex buffer`, `MODELDX8SD`,
  `data/art/`, `\d3d8.dll`, `FMOD INIT FAILED!`). No library version or
  copyright banners, no library `__FILE__` paths.
- All actual libraries are already matched elsewhere: D3DX8 (sdk),
  FreeImage/libpng/libjpeg/zlib (lib), ODE/OPCODE (lib), MSVC7.1 CRT (crt).
  The `FMOD.DLL::_FSOUND_*` entries are the game's import thunks at
  `0x559CC6`, not FMOD code.
- The lone `?term_source@@YAXPAUjpeg_decompress_struct@@@Z` is a known Ghidra
  mislabel already remapped in `Tongas2/CPRIMITIVE3`'s `symbol_mappings`.

## First carve to make

- **Likely unit:** `Tongas2/MESHDX8` (`src/Tongas2/MESHDX8.cpp`, category
  `engine`). `include/Tongas2/HEIGHTMAPDX8.h` declares a stand-in `MESHDX8`;
  `include/Tongas2/RENDERER.h` marks `VERTEXBUFFER`/`INDEXBUFFER` as "Defined in
  the plugin". No `MESHDX8.cpp` exists yet.
- **First function cluster:** `0x004E5D50-0x004EA0FF` (0x43B0 = 17328 bytes,
  101 symbols ≈ 3.6% of the placeholder): the buffer helpers,
  `INDEXBUFFER`/`VERTEXBUFFER` Validate+ctors, `DX8CAMERA::GetProjection`, and
  the `MESHDX8` method cluster (`SetLightMask` … `SetSubmeshShader`).
- **Boundary:** the next function at `0x004EA100` is
  `RENDERSHADERMANAGER::Get`; `include/Tongas2/RENDERSHADER.h` says the plugin's
  shaders "live in a unit of their own" → `Tongas2/RENDERSHADER` is the natural
  second carve from `0x004EA100`.
- Confirm exact object boundaries with `tools/find_objects.py --functions
  --layout 0x004e5d50-0x00559dff` after the source compiles; do not carve the
  whole 475312 bytes in one unit.

## Carve performed (follow-up task)

`Tongas2/MESHDX8` was carved with exactly `0x004E5D50-0x004EA0FF` in
`config/units.json` (category `engine`, source `src/Tongas2/MESHDX8.cpp`,
target `build/target/Tongas2/MESHDX8.obj`). No other unit was touched; the
remaining `0x004EA100-0x00559DFF` stays uncovered as the new
`unassigned/004ea100` placeholder (457984 bytes).

`src/Tongas2/MESHDX8.cpp` is a compiling scaffold defining the 18 named
non-template functions with their exact decorated names; the ~81 `FUN_004e....`
functions are still unnamed. `config/symbols.txt` gained one entry
(`0x004e5d50 code_004e5d50 function 0x2b0`) for the unit's first function, a
dynamic initializer reached only through a `.CRT$XCU` pointer that Ghidra never
made a function; without it the delinked section has no symbol at offset 0.

Pipeline: `configure.py --msvc <vc71>`, `ninja`, `delink.py Tongas2/MESHDX8`
(delinked before the base object existed, so unmatched stub sizes cannot
truncate the target split), `ninja`, then `report.py` and a unit-only objdiff
project. Verified with the original `Tornado.exe` (SHA-1 `20bf7200...06bc`).

### objdiff numbers (measured)

New unit `Tongas2/MESHDX8`: `.text` 17328 bytes; `total_code` 16600;
`total_functions` 107; **`matched_code` 0**; `matched_functions` 0;
`matched_code_percent` 0; `fuzzy_match_percent` 0.3725594 (a few shared bytes
in paired stubs); data 100%. The bodies are placeholders, so nothing matches
yet.

Global, full canonical report (before -> after the carve):

| measure | before | after |
|---|---|---|
| `matched_code` | 604706 | **604706** (unchanged) |
| `total_code` | 2405555 | 2404827 |
| `matched_code_percent` | 25.137897 | 25.14551 |
| `fuzzy_match_percent` | 33.792374 | 33.805176 |
| `total_functions` | 14004 | 14010 |
| `matched_functions` | 3482 | 3482 |
| `total_units` | 251 | 252 |

`matched_code` for the carved 17328 bytes was 0 before (inside
`unassigned/004e5d50`) and is 0 after (inside `Tongas2/MESHDX8`). The 728-byte
`total_code` drop is the `int3` padding `tools/split_functions.py` removes when
a range is delinked; it is the same effect every other delinked unit has, and it
is why `matched_code_percent` rises slightly despite unchanged `matched_code`.
`unassigned/004e5d50` (475312 bytes) is replaced by
`unassigned/004ea100` (457984 bytes); the rest of the placeholder is untouched.
