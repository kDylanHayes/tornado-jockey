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
the flags at `0x24c`/`0x258`.

The map object behind `0x20` keeps three pointer vectors at `+4` (objects),
`+0x14` (paths) and `+0x24` (models), each with its end pointer at `+8`.
They are read through the target's own accessors
(`FUN_00517df0`/`FUN_00517e00`/`FUN_00517e10`), mapped in
`config/units.json`. A map object entry has its name at `+0`, its type at
`+0x1c`, its point at `+0x38` and another string at `+0x20c`.

`LoadMap` uppercases the path names and matches `CAMERAPATH` / `WINGPATH`;
it creates one `MAPRESOURCE` (`0xd0` bytes) for each model and gives it to
the camera holder; then it walks the object list and handles `WING`, `LOGO`,
`CAMERA`, `TARGET`, `SKYBOX` and `GLOWSKYBOX`. The wing and logo cases build
a 0x60-byte decoration from the object's name and set its name to the
loader's `0x20c` string plus the object name. The sky and glow cases ask
`FUN_00527550` whether the combined name exists and then load it through
the manager at `0x04` (`FUN_004ff8c0`) into `0x18`/`0x1c`.

## Progress

- `Update`: **64.34%** fuzzy. The timer, sun resource creation (the target
  builds `V3(0,0,-15)`, `V3(30,30,30)`, `V3(0,0,-15)` and `"SUN"`), camera
  path following and the inverse view matrix are in place. The remaining
  gap is stack layout/register allocation: the target reuses one `MATRIX`
  in place, reads the first camera target from an uninitialized `V3` slot
  at `[esp+0xc]`, and lays the sun's `V3` arguments out differently.
- `LoadMap`: **63.69%** fuzzy. All three list walks and the six name cases
  are translated. The remaining differences are the `std::string`
  temporaries: the target expands `substr`/`operator+`/`UpperString` into
  a slightly different sequence of local strings and calls, and the
  generated register allocation shifts the surrounding blocks.
- `VectorTooLong`: 100%.

No image watermark affects the unit. It is left without `"complete": true`.
