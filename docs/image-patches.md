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
matching issue.

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
