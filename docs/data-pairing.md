# Data pairing

objdiff credits data only when target and base sections pair and match as a
whole. Several actor units are below 100% data because the delinked target's
section layout differs from the compiler's, not because the bytes are wrong.

## ACTOR

- `.data`/`.rdata` now pair after naming the unit's string literals and name
  arrays in `config/symbols.txt`: the five arrays are `_s_TypeNames` at
  `0x668ba8`, `_s_behaviorNames` at `0x668c60`, `_s_activationNames` at
  `0x668d38`, `_s_deactivationNames` at `0x668dd0` and
  `_s_pathBehaviorNames` at `0x668e30`; `0x668ba0` is `"remove"`. The target
  splits each named string into its own section, so the base's COMDAT
  sections pair with them.
- What keeps the unit below 100% data is now two image watermarks, not
  section layout: `0x6bb290-0x6bb29f` in
  `?s_properties@ACTOR@@2PAUPROPENTRY@@A` (+0x790) and
  `0x669913-0x669922` in the `"TestOnActivate"` literal (see
  [image-patches.md](image-patches.md)). The base bytes there are correct for
  the source; the image has 16 opaque bytes.
- `"BNMD"` at `0x6690c0` still loses three bytes: Ghidra also names the
  `"NMD"` suffix at `0x6690c1`, which cuts the target string to one byte even
  after clearing the bogus pointer reference (`config/norefs.txt`). This is a
  delinker symbol-extent artifact, not a source difference.
- `tools/fix_data_icf.py` was also tried without the `??_7` vtable filter so
  it would rewrite relocations in every same-named data symbol. This changed
  no exact data percentage: relocation names are not what blocks these
  sections.

## RESOURCES and RESOURCE

- **RESOURCES** (23.35% data): the base `.bss` is one 5516-byte section, while
  the delinked target has a 3933-byte `.bss` plus a 1580-byte `.bss` whose
  globals come from other translation units. `.data` and `.rdata` already pair;
  the `.bss` split cannot: objdiff pairs data section-wise, and the target
  section boundaries are fixed by the image.
- **RESOURCE** (2.90% data): base has `.rdata` in 19 COMDAT sections and one
  `.data` (336 bytes) plus `.bss` (64 bytes, both command tables). The target
  groups `.rdata` differently and has `.data` 368 bytes (the master command
  table sits after the property tables) plus `.bss` 32 bytes. The section
  shapes do not pair, so the matching bytes are not credited. The
  `?s_commands@RESOURCEMASTER` image entry also contains nonzero fields while
  our source table is all-zero, so its bytes are not yet recovered.

## PATH and EMITTER

Their `.data` sections include the property tables already recorded in
`config/image_patches.txt` (`0x6bc290` and `0x6bd290`); those 16-byte
watermarks make the containing data symbols, and so the section, permanently
partial.

### EMITTER

The report combines every data section with the same base name, so the whole
`.data`, `.rdata` and `.bss` groups are scored as one section each:

- `.rdata` combines to 2480 bytes and pairs at 100%, and the 32-byte `.bss`
  (`s_commands`) pairs too.
- `.data` combines to 3216 bytes and contains the 16 watermarked bytes in
  `?s_properties@EMITTERMASTER` (`0x6bd290-0x6bd29f`), so it can never reach
  100%. That caps `matched_data` at 2512/5728 (43.85%).

Code is capped by the `_$E3` code watermark at `0x60e4ed-0x60e4fc`: the
function is 5506 bytes, so at most 18329/23835 (76.87%) of the unit's code can
be exact. The remaining non-watermark gaps are `EMITTER::Update`,
`PARTICLE::Update`, both constructors and `LoadPicture`; see the matching note
in [toolchain.md](toolchain.md#matching-notes).
