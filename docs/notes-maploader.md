# MAPLOADER

`Tongas2/MAPLOADER` is the map-list object at `0x004f8350`. Three functions:
`LoadMap` (`0xb30`), `Update` (`0x38c`) and the out-of-line
`std::vector<T>` length check `VectorTooLong` (`0x6a`, exact).

## Recovered layout

`MAPLOADER` holds the map at `0x20`, the camera at `0x24`, a second camera
holder at `0x38`, and the map's loaded sky/glow resources at `0x18`/`0x1c`.
Its own `std::string` is at `0x20c`; the timer is `0x0c`, the start flag
`0x10`, the path times `0x228`/`0x22c`, the shared time `0x230`, the target
point `0x234`, the two paths `0x240`/`0x244`, the elapsed time `0x248` and
the flags at `0x24c`/`0x258`. The target's `Update` frame is `0x84`,
`LoadMap`'s is `0x10c`.

The map object behind `0x20` keeps three pointer vectors at `+4` (objects),
`+0x14` (paths) and `+0x24` (models), each with its end pointer at `+8`.
They are read through the target's own accessors
(`FUN_00517df0`/`FUN_00517e00`/`FUN_00517e10`), mapped in
`config/units.json`. A map object entry has its name at `+0`, its type at
`+0x1c`, its point at `+0x38` and another string at `+0x20c`.

## Functions

`LoadMap` uppercases the path names and matches `CAMERAPATH` / `WINGPATH`;
it walks the model list and gives each model to the camera holder. The
model is a `BONEDMODELDX8PARTICLEDESC` (`0xd0` bytes, real constructor and
copy constructor at `0x4f7f40`/`0x4f8030` in `BONEDMODELDX8SD`): the map
copies each model through its copy constructor. It then walks the object
list and handles `WING`, `LOGO`, `CAMERA`, `TARGET`, `SKYBOX` and
`GLOWSKYBOX`. The wing and logo cases build a 0x60-byte decoration from
the object's name and set its name to the loader's `0x20c` string plus the
object name. The sky and glow cases ask `FUN_00527550` whether the combined
name exists and then load it through the manager at `0x04`
(`FUN_004ff8c0`) into `0x18`/`0x1c`.

`Update` runs the 1-second intro timer, clears the camera holder's target
list (`0x517aa0`, a thunk to the holder's list-release routine), creates the
`"SUN"` particle descriptor (material 2, layer 0, position `V3(0,0,-15)`,
direction the camera's `0x188` position, color `V3(30,30,30)`) and gives it
to the second camera holder, advances the shared time `0x230` at 0.4/sec up
to 2, follows the camera path at `0x240` into the camera's
`0x1a8`/`0x1b4`, and builds the wing path's inverse view matrix at `0x244`.

The `substr` instantiation the parser inlines is named at `0x004497c0` in
`config/symbols.txt`; `UpperString` and `FindMapResource` are paired with
their target `FUN_` symbols through `config/units.json`.

## Progress

- `Update`: **90.66%** fuzzy (908 bytes; base 849). The camera-holder call
  turned out to be a target-list release, not a position setter, and the
  sun descriptor's direction is the camera position with a `(30,30,30)`
  color. What remains is codegen only: the `V3` temporary/`point` stack
  slots are swapped (target puts `point` at the lowest slot), plus the
  target's SEH prologue instruction scheduling.
- `LoadMap`: **67.44%** fuzzy (2864 bytes; base 2587). All three list walks
  and the six name cases are translated. The remaining base is smaller
  because the target's `MAPMANAGER::Load` call takes a by-value
  `std::string` resource path in addition to the map name; adding it
  reproduces the target's code but makes objdiff lose the alignment for
  this function, so the simpler call is kept for now. The rest of the gap
  is `std::string` temporary stack offsets and loop register allocation.
- `VectorTooLong`: 100%.

Unit: 2.73% exact, 73.77% fuzzy, 106/3878 bytes, 1/3 functions.

No image watermark affects the unit. It is left without `"complete": true`.
