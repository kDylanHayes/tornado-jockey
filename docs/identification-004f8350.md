# Identification: the map/intro tail, 0x004f8350 onward

Follow-up to [identification-004f2420.md](identification-004f2420.md). The
remaining placeholder `unassigned/004f8350` starts with two more objects. Both
were carved from its start; the rest of the tail (`0x004fa5b0` onward) stays
unassigned.

## What was carved

| Unit | Range | `total_code` | Functions | Evidence |
|---|---|---|---|---|
| `Tongas2/MAPLOADER` | `0x004f8350-0x004f927f` | 3877 | 3 | The large function at `0x004f8350` walks a map object list and compares the special names `TARGET`, `CAMERA`, `KYBOX`, `WINGPATH` and `CAMERAPATH`; `0x004f8e80` is its per-frame update. The `vector<T> too long` helper at `0x004f9210` belongs to the same object's `std::vector` use. Ends before the next object at `0x004f9280`. |
| `Tongas2/INTRO` | `0x004f9280-0x004fa5af` | 4790 | 19 | The first function builds the intro screen around `data/art/models/interface/wtintro/intrologo.png` and calls the map-list function at `0x004f8350`; the methods run through `0x004fa4b0`. Ends before `0x004fa5b0`, which starts the settings/config object (`"Settings enumeration is out of order!"`). |

The class names for these two objects are not recovered yet. `MAPLOADER` and
`INTRO` are descriptive names, and the method names `LoadMap`, `Update` and
`Load` are likewise from what the decompiler shows, not from strings. The
member layouts and the remaining methods are still unknown.

## Scaffolds

`include/Tongas2/MAPLOADER.h` and `src/Tongas2/MAPLOADER.cpp` define the map
list and its update. `include/Tongas2/INTRO.h` and `src/Tongas2/INTRO.cpp`
define the intro entry point. Bodies are empty; the functions have their exact
target sizes in `config/symbols.txt` (`0xb30`, `0x38c` and `0x3dd`), so a later
delink cannot cut the target short. Both units are recorded without
`"complete": true`.

## Measured before and after

| measure | before | after |
|---|---|---|
| `matched_code` | 615936 | **615936** (unchanged) |
| `total_code` | 2402928 | 2402795 |
| `matched_code_percent` | 25.632728 | 25.634147 |
| `total_functions` | 14028 | 14028 |
| `matched_functions` | 3551 | 3551 |
| `total_units` | 267 | 269 |

The two units add 8667 bytes (3877 + 4790). `unassigned/004f8350`
(400,048 bytes / 1,353 functions) became `unassigned/004fa5b0`
(391,248 bytes / 1,331 functions). The 133-byte drop in global `total_code`
is trailing `int3` padding removed by `tools/split_functions.py`.

Verified with the original `Tornado.exe` (SHA-1 `20bf7200...06bc`); the report
and both units' objdiff targets build without truncation panics.

## Matching progress

`Tongas2/MAPLOADER` has one exact function:

- `?VectorTooLong@@YAXXZ` at `0x004f9210` (the `std::vector<T>` length
  check, 100%). The Dinkumware template emits exactly the target code when a
  vector operation in this unit forces `_Xlen` out of line. Its symbol size is
  `0x6a`, not Ghidra's `0x69`: the compiler's trailing `int3` is part of the
  function section and is what makes objdiff report 100% instead of 95.65%.

`?Update@MAPLOADER@@QAEXM@Z` has a working translation of the decompiled
timer, camera and path logic and scores 60.86% fuzzy; it is still partial.
`?LoadMap@MAPLOADER@@QAEXPBD@Z` is still the empty scaffold. The unit is
currently 2.73% exact, 17.08% fuzzy, 1/3 functions.

The blockers are the same for both remaining functions: the class's identity
and base class are unknown, and the two paths and the camera holder are
opaque helper objects whose methods and types live in uncarved units. The
`Update` translation uses descriptive helper names (`CAMERAOBJ`, `PATHOBJ`,
`MAPRESOURCE`) and pairs them with the target's `FUN_` names through
`symbol_mappings`; the stack layout of `MAPRESOURCE`'s 0xd0-byte construction
and the matrix helpers are still guesses, and `LoadMap` is a 2,864-byte
`std::string`-heavy parser that needs the same class layout before it can be
written exactly.

## INTRO matching

`Tongas2/INTRO` turned out to contain a second, self-contained settings
object at `0x004f97d0`-`0x004fa4af`: four 0xff-entry tables at `0x000`,
`0x3fc`, `0x7f8` and `0xbf4`, the int/float settings pointers at `0x1030` and
`0x1034`, and the changed flag at `0xff4`. Twelve of its functions are exact:

- `GetInt`, `SetInt`, `GetFloat`, `SetFloat`, `Changed`
- `Clear`, `Reset`, `RemovePair`, `AddPair`, `ReplacePair`
- `ClearAll`, `ResetKeys`

`Init` (`0x004f97d0`) is 99.23%: only its two initial `xor` instructions are
in the other order. The three tiny accessors at `0x004f9830` etc. are already
matched.

The unit is 15.41% exact, 24.22% fuzzy, 12/19 functions (`function_status.py`
counts 12/16 paired functions). Still open: `INTRO::Load` (the 989-byte intro
screen), the `SETTINGS` constructor at `0x004f9660`, the 0x708-byte quality
update at `0x004f9c40`, `SetQuality` at `0x004fa360` (written but not yet
paired; its jump table extends past Ghidra's 0x130-byte extent), and the
destructor at `0x004fa4b0` (now written but only 74.50%: the target drops the
last string's field resets after freeing its heap buffer, while our
compiler-generated destructor keeps them).

The remaining blockers are the incomplete object layout around
`0x1018`-`0x1038` (two `std::string`s overlapping the settings pointers), the
unknown quality/hardware helpers, and the intro screen's own resource
construction. The constructor needs the `??_L`/`std::string[2]` allocation
shape that keeps the pointer in `edi`; our compiler stores it through `eax`
and is eight bytes longer.
