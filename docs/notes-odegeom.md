# Tongas2/ODEGEOM notes

## GD549b40::UpdateGeom (0x00549c40)

**True extent: 0x179 (377 bytes), 0x00549c40-0x00549db8.** The bytes at
0x00549db9-0x00549dbf are the linker's `int3` padding; 0x00549dc0 starts a
different function (it calls `ShowHelpers` and moves the transform). The
0x6b0 symbol in `config/symbols.txt` is only a scaffold: it covers the
following functions and their switch jump table
(`switchdataD_0054a2d4`), because shrinking it makes objdiff panic while
diffing `Tongas2/ODEGEOM`:

```
range start index 1403 out of range for slice of length 88
```

The table is a data symbol inside a `.text` section that the delinker creates
for the 0x00549dc0+ region. Keep the 0x6b0 scaffold (or give those functions
symbols whose sizes include the table) before setting 0x179.

### Reconstructed shape

The body is:

- bail unless `*(bool *)(this + 0x431)`;
- clear it, then `IsAtOrigin()`;
- not at the origin: if `m_42c`, `ODEGEOMRESOURCE::RemoveGeom()`,
  `dGeomDestroy(m_42c)`, `m_42c = 0`, `AddToSpace(g_pOdeWorldX->[0x60])`;
- at the origin with no `m_42c`: `RemoveGeom()`, then
  `dCreateGeomTransform(0)`, `dGeomTransformSetGeom`, `dGeomTransformSetInfo(1)`,
  `dGeomSetData(this)`, `dGeomSetCategoryBits(master->m_64)`,
  `dGeomSetCollideBits(master->m_68)`, `AddToSpace`;
- with `m_42c` and an actor:
  `V3 position = m_offset * actor->GetWorldScale()` (virtual slot 0x170),
  `dGeomSetPosition(m_428, position.x, position.y, position.z)`, then a
  `ANG::GetMatrix()` / `MATRIX::Transpose()` pair and `dGeomSetRotation`.

Compiling it in a TU that defines `TONGAS2_CORE_OUTOFLINE` (out-of-line
`ANG::GetMatrix`) and `MATRIX_TRANSPOSE_OUTOFLINE` reaches 95.65%
(381 bytes against 377).

### Bounded residue

The last 0x10 of difference is storage allocation, not expression shape:

- the retail frame is 0x88 and its two `MATRIX` temporaries overlap
  (`GetMatrix` writes at `[esp+0x4c]`, the transpose output is at
  `[esp+0x1c]`); VC7.1 7.1 in our TU reserves a separate slot, giving a
  0x98 frame and 381 bytes;
- the position products are loaded as `fld [esi+0x3c] / fmul [eax]` (the
  scale through the pointer returned by `GetWorldScale`) while ours loads
  the scale from its stack slot first.

Forms tried without improving the match: `m_offset * actor->GetWorldScale()`,
`actor->GetWorldScale() * m_offset`, a `const V3 &` binding, and field-by-field
assignment. The best honest source is the field-by-field form; the figures
above are its residue.
