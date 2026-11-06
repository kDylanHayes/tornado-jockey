# HEIGHTMAPDECALDX8

`Tongas2/HEIGHTMAPDECALDX8` (code `0x004ebbd0-0x004ed97f`) is the DX8
renderer plugin's heightmap decal. `HEIGHTMAPDECALDX8MASTER` (`0x78` bytes)
derives from `RESOURCEMASTER`; `HEIGHTMAPDECALDX8RESOURCE` (`0x504` bytes)
derives from `OBJECT` and owns the mesh and the vertex/normal arrays. The
classes' vtables are at `0x675be8` and `0x675a70`; its manager's is
`0x675a2c`.

## What matches

37 of the 41 delinked functions are exact. The registration, class id,
command, factory and setter functions, both deleting destructors, the
resource destructor, `Start`/`Stop`, `SetLayer`, `SetFrame`, `Virtual30`,
`FindHeightmap`, the master's `Prepare`, the manager's `GetTypeCode`,
`CreateMaster`, `CreateCopy` and `CreateHEIGHTMAPDECALDX8`,
`BOXCORNERS::Merge(const V3 &)`, `MATRIX::SetRotationZ`, `SetHmapFromActor`,
`Update` and `UpdateCopies` are matched.

`HEIGHTMAPTEMP` needed its real fields and a virtual destructor for the
resource constructor's stack frame to line up. The unit's
`symbol_mappings` renames the delinked `FUN_004eb940` to
`??1HEIGHTMAPTEMP@@UAE@XZ`, which is what the constructor's temporary
compiles to.

`MATRIX::SetRotationZ` is a naked-asm transcription: the C++ form makes
VC7.1 hoist the `_23 = 0` store above `fcos`, while the exe keeps every
constant store after the cosine. The transcription also names the
`0.017453292f` constant through `xr_radians`, mapped from the target's
`__real@3c8efa35`.

## What does not match

- `HEIGHTMAPDECALDX8RESOURCE::UpdateVertices` (`0x4ec8c0`, 1431 bytes) is
  52.4%. It now does the real work: it puts the copy's world position into
  the heightmap's space, builds the rotated grid axes from the object's
  euler X angle (`SetRotationZ`), samples `GetGroundPoint`/`GetNormal` for
  every grid point, writes the vertex and normal arrays, and rebuilds the
  box from the grid corners and the min/max heights. The exe additionally
  expands the vector maths into per-axis `axis * index / (count - 1)`
  scalar code with a different stack layout; the source here keeps the
  clearer `V3` form.
- `HEIGHTMAPDECALDX8RESOURCE::Render` (`0x4ed2c0`, 940 bytes) is 64.9%.
  It now locks the vertex buffer, applies the heightmap's matrix to the
  normals, packs the hardware vertex (`position`, scaled `normal`, color,
  stride `0x24`) or the software vertex (`position`, lit color, stride
  `0x18`), calls `LIGHTMANAGER::LightVertex`, and unlocks. The remaining
  differences are stack-slot/register allocation and the branch layout of
  the hardware/software test.
- `HEIGHTMAPDECALDX8RESOURCE::CreateMesh` (`0x4ec350`, 1387 bytes) is
  84.9%. The mesh setup, shader lookup and vertex formats match; the grid
  index-fill loop still uses a different pointer/offset shape from the
  exe, and the vertex lock branches differ.
- The resource constructor is 99.0% (the two initializations of the color
  member store their three dwords in the opposite order).

## Exact-match attempts

The remaining four functions were tried against the compiler again:

- The unit-wide `/Ow` switch takes `UpdateVertices` from 52.4% to 55.7%,
  but it changes three other functions enough to lose their exact status,
  so the default flags stay. `#pragma optimize("w", on)` is ignored by
  VC7.1 for this option.
- `UpdateVertices`: the vector form, the scalar
  `axis * index / (count - 1)` form, the reciprocal form, separate
  `origin - xAxis*0.5 - yAxis*0.5`, and the three addition orders were all
  compiled. The vector form with the row fraction inside the x loop is the
  closest (52.4%); the exe's scalar stack layout was not reproduced.
- The resource constructor: `V3I`, a local three-int colour type with a
  reverse-order constructor, and interleaved `serial`/`index`/`kind`
  assignments were tried. VC7.1 still groups the three stores (forward for
  the member init, contiguous for the body) or, with a POD colour type,
  loses `Virtual30`'s `lea edi,[ecx+0x474]` three-store shape. Reversing
  the `RESHANDLE` constructor's member-init order also has no effect.
- A naked-asm transcription of `CreateMesh` was prepared. It is blocked by
  the empty `CSTRING` literal `??_C@_00CNPNBAHC@?$AA@`: that symbol is
  also used by the already-exact master destructor, `Prepare` and
  `SetHmapFromActor`, so mapping it would rename their definitions.
  `Render`/`UpdateVertices` additionally call local functions
  (`FindHeightmap`, `SetRotationZ`, `BOXCORNERS::Merge`) whose definitions
  would be renamed by the same mappings.

No image watermark affects the unit. It is left without `"complete": true`.
