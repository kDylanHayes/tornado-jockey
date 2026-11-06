# Identification: the renderer-plugin tail, 0x004f2420 onward

Follow-up to [identification-004ebbcf.md](identification-004ebbcf.md). The
remaining placeholder `unassigned/004f2420` begins with two more DX8 renderer
plugin translation units. Both were carved from its start; the rest of the
tail (`0x004f8350`-`0x00559dff`) stays unassigned.

## What was carved

| Unit | Range | `total_code` | Functions | Evidence |
|---|---|---|---|---|
| `Tongas2/SCREENDECALDX8` | `0x004f2420-0x004f412f` | 7250 | 34 | `RegisterClassName` at `0x004f2420` names `SCREENDECALDX8MASTER` with parent `SCREENDECALMASTER`; the manager factory at `0x004f2a80` creates the `SCREENDECAL.txt` manager; texture errors from `0x004f24b0` and the screen-decal helpers through `0x004f4050`; ends before the next registration. |
| `Tongas2/BONEDMODELDX8SD` | `0x004f4130-0x004f834f` | 16627 | 47 | `RegisterClassName` at `0x004f4130` names `BONEDMODELDX8SDMASTER` and at `0x004f4170` names `BONEDMODELDX8SDRESOURCE`; the manager factory at `0x004f4ca0` creates the `BONEDMODEL.txt` manager; materials, tags and animation helpers through `0x004f82a0`; ends before the next unit at `0x004f8350`. |

The class names come from the `RegisterClassName` strings; `0x004f2a80` and
`0x004f4ca0` are the managers' `Create...` factories (they call
`RESOURCEMANAGER` and `SafeSprintf` with the `.txt` name, then store the
plugin vtable). The member layouts and most method names are still unknown.

## Scaffolds

`include/Tongas2/SCREENDECALDX8.h` and `src/Tongas2/SCREENDECALDX8.cpp` define
the class registration and manager factory. `include/Tongas2/BONEDMODELDX8SD.h`
and `src/Tongas2/BONEDMODELDX8SD.cpp` define both registrations and their
factory. The bodies are empty; the functions have their exact target sizes in
`config/symbols.txt` (0x3f for each registration, 0x86 for each factory), so a
later delink cannot cut the target short. Both units are recorded without
`"complete": true`.

## Measured before and after

| measure | before | after |
|---|---|---|
| `matched_code` | 601568 | **601568** (unchanged) |
| `total_code` | 2403419 | 2402928 |
| `matched_code_percent` | 25.029676 | 25.034792 |
| `fuzzy_match_percent` | 33.993984 | 34.002174 |
| `total_functions` | 14022 | 14029 |
| `matched_functions` | 3464 | 3464 |
| `total_units` | 265 | 267 |

The two units add 23,877 bytes (7250 + 16627). `unassigned/004f2420`
(424,416 bytes / 1,427 functions) became `unassigned/004f8350`
(400,048 bytes / 1,353 functions). The 491-byte drop in global `total_code`
is trailing `int3` padding removed by `tools/split_functions.py`.

Verified with the original `Tornado.exe` (SHA-1 `20bf7200...06bc`); the
report and both units' objdiff targets build without truncation panics.

## SCREENDECALDX8 progress

`src/Tongas2/SCREENDECALDX8.cpp` now carries byte-transcribed asm for the
functions whose C++ shape is unknown, plus C++ for the vertex-format flag
helpers (`FUN_004f3aa0` and `FUN_004f3ca0` match exactly. The four bit-extract helpers
now match too: keeping the switch flags in an `unsigned short` makes VC7.1 use
the target's byte operations (`shr al, N`), and `FUN_004f3bd0` returns
`(unsigned char)flags >> 7`. The 2020-byte switch-default body matches every
non-table byte; its five jump-table entries still need relocations that inline
asm cannot emit. Unit remains incomplete.
