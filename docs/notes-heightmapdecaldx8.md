# HEIGHTMAPDECALDX8

`Tongas2/HEIGHTMAPDECALDX8` (code `0x004ebbd0-0x004ed97f`) is the DX8
renderer plugin's heightmap decal. `HEIGHTMAPDECALDX8MASTER` (`0x78` bytes)
derives from `RESOURCEMASTER`; `HEIGHTMAPDECALDX8RESOURCE` (`0x504` bytes)
derives from `OBJECT` and owns the mesh and the vertex/normal arrays. The
classes' vtables are at `0x675be8` and `0x675a70`; its manager's is
`0x675a2c`.

## What matches

34 of the 41 delinked functions are exact. The registration, class id,
command, factory and setter functions, both deleting destructors, the
resource constructor, the resource destructor, `Start`/`Stop`, `SetLayer`,
`SetFrame`, `Virtual30`, `FindHeightmap`, the master's `Prepare` and
`UpdateCopies`, the manager's `GetTypeCode`, `CreateMaster`, `CreateCopy`
and `CreateHEIGHTMAPDECALDX8`, `BOXCORNERS::Merge(const V3 &)`,
`MATRIX::SetRotationZ` (except for one store the scheduler moves) and
`SetHmapFromActor` are matched.

`HEIGHTMAPTEMP` needed its real fields and a virtual destructor for the
resource constructor's stack frame to line up. The unit's
`symbol_mappings` renames the delinked `FUN_004eb940` to
`??1HEIGHTMAPTEMP@@UAE@XZ`, which is what the constructor's temporary
compiles to.

## What does not match

- `HEIGHTMAPDECALDX8RESOURCE::UpdateVertices` (`0x4ec8c0`, 1431 bytes) and
  `Render` (`0x4ed2c0`, 940 bytes) are not reconstructed; they are
  noinline one-instruction placeholders so `Update` keeps its calls.
- `HEIGHTMAPDECALDX8RESOURCE::CreateMesh` (`0x4ec350`, 1387 bytes) is at
  84.9%. The mesh setup matches, but the grid's index-fill loop chooses a
  different register/offset shape than the exe's loop, and the vertices'
  lock and lighting branches still differ.
- `MATRIX::SetRotationZ` (`0x4ec120`) is at 92.3%: the scheduler hoists one
  of the cleared matrix elements across `fcos`; the exe keeps every store
  after the cosine.
- The resource constructor is 99.0% (store order in the two handle
  initializations), `Update` is 99.2% (the exe reloads `ecx` before its
  second `FindHeightmap` because the real `UpdateVertices` is opaque), and
  `UpdateCopies` is 99.4% (`xor eax,eax`/`mov eax,1` versus the byte
  forms).

No image watermark affects the unit. It is left without `"complete": true`.
