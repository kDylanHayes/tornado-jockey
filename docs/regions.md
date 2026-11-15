# Code regions in Tornado.exe

Ghidra 12.2 auto-analysis finds about 13,100 functions in `.text` (0x401000–0x655424), including compiler-generated catch/unwind funclets and thunks.

## How the map was made

Two independent signals line up:

1. **Function alignment.** Objects compiled with `/Gy` (function-level linking) put each function in its own COMDAT, which the linker pads to 16 bytes. Objects compiled without it pack their functions back to back. Long runs of 16-byte-aligned or unaligned function starts therefore mark groups of objects built with different settings.
2. **Referenced strings.** Library error messages, `__FILE__` paths and copyright banners identify the code that uses them.

## Map

| Range | Functions | Alignment | Contents |
|---|---|---|---|
| 0x401000–0x4543ff | ~1,170 | aligned | Game and Tongas2 engine code |
| 0x45446b–0x482a14 | ~620 | unaligned | The prebuilt D3DX8 static library's C code (it loads `d3d8.dll`, reads `Software\Microsoft\Direct3D`, checks `DisableD3DXPSGP`), including the libpng 1.0.5 and zlib copies D3DX uses to load textures (png_create_read_struct at 0x4679df; inflate at 0x47de92; `adler32` at 0x47e661 is shared with FreeImage's zlib). Built by Microsoft with a pre-release VC 7.0 compiler (13.00.9178 in the Rich header), so it is linked from `d3dx8.lib` rather than rebuilt; VC 7.1 reproduces only 40 of its libpng functions exactly |
| 0x482cd0–0x559dff | ~3,150 | aligned | Game and Tongas2 engine code (renderer, resources, GUI, input, ACTOR/SCENE). FMOD import thunks at 0x559cc6 |
| 0x559e00–0x56717b | ~220 | aligned | FreeImage 3.7.0's C++ code, all matched: FreeImage.cpp and Plugin.cpp (trimmed to PNG and JPEG), then bitmap access, conversions, the PNG and JPEG plugins, metadata and quantizers, built with `/O2 /GX` |
| 0x56717c–0x586df5 | ~460 | unaligned | FreeImage 3.6.1/3.7.0's bundled libraries, all matched: libpng 1.2.8 (0x56717c–0x574aa4), libjpeg 6b (0x574aa5–0x582970), zlib 1.2.2 (0x582971–0x586fc2; `adler32` is shared with block 1 at 0x47e661). FreeImage's own code is elsewhere |
| 0x586fd0–0x5bda8f | ~530 | aligned | ODE 0.5 (single precision, trimesh collision through OPCODE), built from `C:\Wanako\Tongas2\SDKs\ode`; all matched (49 units, `/O2 /GX`). Objects sit in the order the linker pulled them from the ODE library, not source order. The ISO-extracted target differs from the old retail image in 18 bytes at 0x59188f, inside this block (`lib/ode/joint` matches 99.73%) |
| 0x5bda90–0x5bf241 | ~25 | aligned | Game code: online leaderboard requests ("Secret Key or Leaderboard Key given was invalid."), the DX8 and `.wkz` file loaders, and a zlib wrapper for `.wkz` archives |
| 0x5bf242–0x5e5f66 | ~715 | unaligned | Static MSVC 7.1 runtime: libcmt (CRT), libcpmt (Dinkumware STL), ATL, EH support |
| 0x5e5fa0–0x655400 | ~160 | aligned | Mostly large functions referencing D3DX: the prebuilt D3DX8 static library (the VC6-era objects in the Rich header) |

Boundaries are approximate to within a few functions. Exact unit boundaries are recorded per unit in `config/units.json` as they're confirmed.

## Implications

- About 60% of the functions are game and engine code. That's the real decompilation work.
- zlib, libpng, ODE and FreeImage are open source. They should be matched by compiling the right upstream versions rather than decompiled by hand.
- The CRT, STL, ATL and D3DX8 come from prebuilt Microsoft libraries. They don't need decompiling; they get linked from the original `.lib` files at the full-link stage.
- The unaligned library blocks were built with different flags from the game. zlib matches with `/O1` (see [toolchain.md](toolchain.md)).

## Measured split

`py build/report_regions.py` takes the three library ranges above back out of
`build/report.json`, so progress can be read against the code that is actually
ours:

| | bytes | functions |
|---|---|---|
| whole `.text` | 2,352,965 | 14,944 |
| library blocks (D3DX8, libcmt/libcpmt/ATL) | 599,331 (25.5%) | 7,305 |
| game and engine code | 1,753,634 | 7,639 |
| matched at 34.52% overall | 812,250 | 6,092 |

So the headline percentage is measured against a denominator that is a quarter
prebuilt Microsoft library: **46.3% of our own code is matched**, and 1,547
engine/game functions are left. Ghidra splits library code into many small
functions (7,305 against the ~1,500 hand-sized objects in the table above), which
is why its share of the function count is much larger than its share of bytes.
