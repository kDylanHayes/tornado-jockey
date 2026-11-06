# SIMPLEMODELDX8 notes

`Tongas2/SIMPLEMODELDX8` (0x004ef090-0x004eff6f) is the DX8 renderer plugin's
simple-model type. It derives `SIMPLEMODELDX8RESOURCE` from the engine
`SIMPLEMODEL` and `SIMPLEMODELDX8MASTER` from `SIMPLEMODELMASTER`; the
resource keeps the custom vertex data, the mesh handle and the copy handle,
and the master keeps the shader name/handle.

When the unit was carved, `build/functions.txt` had fourteen functions and
two of them (`0x004efb20` and `0x004efb70`) were still inside the huge
`AddMeshPart` body. Two more functions in the range were missing entirely:
`0x004efd70` (`IsLoaded`) and `0x004efe00` (`SetLayer`). `config/symbols.txt`
now records all eighteen, with `0x004ef720` split at `0x004efb20` and
`0x004efb70`; `0x004efb70` is 0x59 bytes (its `ret 0x10` was cut off by the
old 0x57 size). The unit is still not `complete`.

## Matching

- Fifteen of the eighteen functions are byte-exact. The master/resource
  constructors and deleting destructors, `CreateCopy`, `CreateMaster`,
  `Update`, `UpdateCopies`, `Start`, `SetLayer`, `CreateMesh`, `IsLoaded` and
  both custom-data wrappers are done.
- `@SetCustomData` (0x004ef090, 714 bytes) is 92.33%. The original copies the
  caller's `V3`s into the `DECALVERTEX` array with a manually unrolled
  four-vertex loop, writing the source through a `float *` and reloading
  `m_customVertices` before every store. The body now matches; the loop
  preheader does not. The exe computes the trip count as
  `((numVertices - 4) >> 2) + 1` and never tests it for zero after the
  `numVertices >= 4` guard. Every source form tried here (`for` with
  `i <= n - 4`, `i < n - 3`, a count `do`/`while`, explicit fields) either
  keeps a redundant `cmp n - 4, 0` or changes the loop bodies. The current
  `float *src` form is the closest.
- `@AddMeshPart` (0x004ef720, 1011 bytes) is 99.58%. The only instruction
  mismatch is the order of `m_lastLayer = -1` against the six
  `m_lastPosition`/`m_lastScale` float stores: the exe stores the layer
  first, VC7.1 here always sinks it after the floats. Explicit `.x/.y/.z`
  assignment (instead of `V3` assignment) removed the rest of the tail
  differences.
- `@SetShader` (0x004efbf0, 249 bytes) is 99.30%. Every instruction matches
  except that the exe keeps `this` in `ebx` and `m_shaderName` in `ebp`; this
  compiler keeps them the other way round. Local aliases, `self`, direct
  `m_buf` and swapping the `strcmp` arguments all reproduce the same swap.

## Build flag

The unit is compiled with `/Op` (see `config/units.json`). All the floating
point code is x87 either way, but `/Op` changes the register allocation in
`AddMeshPart` enough to take it from 93.5% to 99.58% (and the unit fuzzy
match from 92.4% to 98.3%). `/Ob1`, `/Oa`, `/Oi-`, `/Oy-`, `/G7`, `/Os`,
`/Ot` and `/Ow` were also tried; they are equal or worse.

There is no image watermark for this unit in `config/image_patches.txt`.
