# Image watermarks

The `20bf7200...` ISO-extracted `Tornado.exe` contains short opaque byte runs
(13–19 bytes, 16 typical) in both `.text` and `.rdata`. They are part of the
image itself: a fresh Ghidra analysis, the shared analysed project and the raw
file all show the same bytes, and the decompiler turns them into nonsense
(`cld`/`fild`/`ret 0xc945`, or string heads replaced by random bytes).

They look like installer/DRM watermarks rather than compiler output, so they
cannot be recovered from source. Do not write them into C++ or fabricate data
to make objdiff pass: units whose code or data overlaps a watermark stay
partial.

`../config/image_patches.txt` records every known range with its unit, symbol
and kind. The starts are irregular — no fixed address stride or modulus was
found — so new ones have to be identified from a target/base diff. Ranges
marked `observed` came from byte-level target/base comparisons; `scan` ones
came from a suspicious-disassembly sweep over all units (their exact end can
include the code the watermark displaced); `reported` ones came from other
workers and are listed at their reported start address.

There is no fixed length or alignment. A range can cover more than one symbol
(the SHADER strings are an example), and `DECAL::GetShaderHandle` is not a
watermark — it is a wrong symbol size in the target object, tracked as a normal
matching issue. `RESOURCES::Insert` has two nearby sites: the real 17-byte
watermark at `0x4a6aaf-0x4a6abf` (listed) and an analysis-only instruction
overlap at `0x4a6af5` (not a watermark, not listed).

A separate, fixable issue is the delinker putting relocations on short strings
whose bytes read as an address (`"set"` is `0x00746573`). That is handled in
`tools/ghidra/FixReferences.java` and `config/norefs.txt`, not by this list.

## Blocks found while matching gameplay units

Two more byte-checked runs came out of the actors/gameplay pass
(`Tongas2/SPAWNER`, `Tongas2/VALUEMANAGER`):

| range | unit | symbol |
|---|---|---|
| `0x4b558d-0x4b559f` | `Tongas2/SPAWNER` | `?SetWaitOnFirst@SPAWNER@@UAEXAB_N@Z` (the run at `0x4b5590` is `?SetUseSpawnerAngleScale@SPAWNER@@UAEXAB_N@Z`) |
| `0x4cdd00-0x4cdd0f` | `Tongas2/VALUEMANAGER` | `?Delete@VALUEMANAGER@@UAE?AVSRESULT@@PAUCMDCALL@@@Z` |

Both start on a 16-byte boundary and read as `e2 a8 e7 d2 ...`/`f1 b8 51 c4
...` in the raw file, so the affected functions, and with them those two
units, stay partial. `?SetWaitOnFirst` is otherwise a 12-byte body that does
match; only its trailing `ret` immediate is overwritten.

The `.data` command/property tables of `Tongas2/PATH` and `Tongas2/EMITTER`
are hit as well, which is what keeps their data percentage below 100:

| range | unit | symbol |
|---|---|---|
| `0x6bc290-0x6bc29f` | `Tongas2/PATH` | `?s_properties@PATHMASTER@@2PAUPROPENTRY@@A` (+`0xd8`) |
| `0x6bd290-0x6bd29f` | `Tongas2/EMITTER` | `?s_properties@EMITTERMASTER@@2PAUPROPENTRY@@A` (+`0xa78`) |

In both the target table holds sixteen random bytes where our source has a
zeroed entry, and the surrounding entries match, so nothing legitimate can
close that gap.

## Found while matching code bytes

`PATHMASTER::GetCatmullRom` also overlaps a code run. Our object has the
expected Hermite/tangent arithmetic at the corresponding offset, while the
image has `e7 59 35 ff 88 b3 8d c1 50 25 86 f1` after the `fcomp`/`pop`
opcodes:

| range | unit | symbol |
|---|---|---|
| `0x4ab8fa-0x4ab908` | `Tongas2/PATH` | `?GetCatmullRom@PATHMASTER@@QAE_NMPAUV3@@00PAM1@Z` |

The run is 15 bytes, so the function and `Tongas2/PATH`'s code stay partial.

`EMITTER`'s compiler-generated static initializer `_$E3` also contains a
16-byte run at `0x60e4ed-0x60e4fc`:

| range | unit | symbol |
|---|---|---|
| `0x60e4ed-0x60e4fc` | `Tongas2/EMITTER` | `_$E3` |

The bytes there are opaque (`89 1b fb 96 cd e5 13 39 ...`), replacing a group
of `s_properties` stores, so `_$E3` and `Tongas2/EMITTER`'s code stay partial.

`ACTOR::~ACTOR` also contains a 17-byte run. The image has
`f2 f0 3b 54 7b 04 05 d9 c7 d5 d0 3b 1f 2e 4f 06` where our object continues
with the next `free`/member cleanup sequence:

| range | unit | symbol |
|---|---|---|
| `0x49ce1b-0x49ce2b` | `Tongas2/ACTOR` | `??1ACTOR@@UAE@XZ` |

The destructor and `Tongas2/ACTOR`'s code therefore stay partial.

## CMDMGR

`?RegisterEngineClasses@@YAXXZ` (at `0x488aa0`) has a 14-byte run at
`0x4894e9-0x4894f6`: the `mov [edx*4 + g_classProps], offset s_properties`
immediate and the ten bytes after it are replaced by non-code, and valid code
resumes at `0x4894f7`. The same pattern later in the function is intact
(`mov dword [ecx*4 + 0x6cbc90], 0x6be8d0`), so this is a patched span, not a
delinker or FixReferences defect. The unit stays partial.

## Libraries

The third-party libraries carry the same runs. Comparing every library unit
whose report still showed a gap (target object against our compiled object,
relocation bytes masked) found one opaque run in each failing function, 9 to
19 bytes and 16 in most, with no other byte differences (a run that lands on
a boundary can show as a second, shorter span where the displaced code
resumes). So every remaining library gap has this one cause: objdiff counts a
whole function as unmatched once any instruction differs, and the run cannot
be produced by any source or compiler flag.

The sites are in `config/image_patches.txt`. Affected units and functions:

| library | units (function hit) |
|---|---|
| zlib | `inffast` (`_inflate_fast`), `inflate` (`_updatewindow`) |
| libpng | `pngset`, `pngread`, `pngrtran`, `pngwutil`, `pngwtran`, `pngrutil` |
| libjpeg | `jdapimin`, `jdmainct`, `jddctmgr`, `jchuff`, `jcmaster` |
| FreeImage | `BitmapAccess`, `Rescale`, `PluginJPEG`, `Exif`, `NNQuantizer` |
| ODE | `ode`, `collision_trimesh`, `collision_std`, `joint`, `matrix`, `quickstep`, `collision_trimesh_ccylinder`, `collision_trimesh_trimesh`, `collision_trimesh_box`, `lcp`, `fastldlt`, `OPC_TreeCollider`, `OPC_OBBCollider`, `OPC_RayCollider`, `OPC_OptimizedTree` |

Some runs straddle a symbol boundary. `hingeInit`'s `ret` at `0x59188f` is
the first byte of a 19-byte run that also covers `sliderInit`'s prologue
(`0x591890-0x5918a1`, which is why that function has no pair in objdiff), and
one run at `0x5b3c95` covers the tail of `??_GOBBCollider` and the head of
`OBBCollider::Collide`. Those ranges are listed separately in the config file.

Not every library gap was a watermark. `TagLib::getFreeImageModel` reads a
case table with `movzx eax, byte ptr [eax+0x566030]`; the table is a compiler
`$L` label in `.text`, and Ghidra leaves references into executable memory
alone, so the delinked object had no relocation there (`objdiff` saw the bare
address against our `$L17986`). `split_functions.sync_local_refs` now copies
the relocation our object has, and the unit matches 100%.

