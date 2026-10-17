# Toolchain

## Identifying the compiler

The Rich header in `Tornado.exe` lists the tools that produced each object file:

| Tool | Build | Objects |
|---|---|---|
| C++ compiler (`cl`, C++ mode) | 13.10.3077 | 374 |
| C compiler (`cl`, C mode) | 13.10.3077 | 272 |
| MASM | 7.10.3077 | 46 (mostly the CRT) |
| Linker | 7.10.3077 | 1 |
| Resource converter | 7.10.3052 | 1 |
| Older objects (VC6-era, builds 8447–9210) | — | ~100, most likely prebuilt DirectX 8 / D3DX8 SDK libraries |

13.10.3077 is Visual C++ .NET 2003 (7.1) RTM, not SP1 (13.10.6030). The same compiler shipped in the free **Microsoft Visual C++ Toolkit 2003**.

## Installing

The toolkit is not redistributable, so this repository cannot include it. Get `VCToolkitSetup.exe` yourself (SHA-1 `50dc6cca1503f08a41d56816f3f1abcaf347b0bd`), then unpack it without running the installer (needs `msitools`):

```sh
python3 tools/extract_vctoolkit.py /path/to/VCToolkitSetup.exe tools/vc71
wine tools/vc71/bin/cl.exe
# Microsoft (R) 32-bit C/C++ Optimizing Compiler Version 13.10.3077 for 80x86
```

`tools/vc71/` is git-ignored. On Windows you can also just run the installer.

Then configure the project with that folder:

```sh
python3 configure.py --msvc tools/vc71
```

You can also set `TJ_MSVC` instead of passing `--msvc`. On Windows, pass `--wine ""`.

The toolkit contains the compiler, linker, static CRT (`libcmt.lib`, `libcpmt.lib`) and the standard headers, which is everything needed to compile matching objects. DirectX 8 headers are not needed for most game code; when they are, declare what's needed in `include/` rather than depending on an SDK install.

## Compiler flags

The default flags in `configure.py` (`/O2 /MT /GX`) are a starting guess based on the binary:

- `/MT`: the CRT is linked statically (the exe imports no `msvcr71.dll`).
- `/GX`: C++ exception handling is used (Dinkumware `std::exception` classes are present).
- `/O2`: release build.

These are confirmed or corrected as game units match. Libraries were built with their own flags, so units can override `cflags` in `config/units.json`.

Confirmed so far:

| Code | Flags | Evidence |
|---|---|---|
| zlib 1.2.2 | `/O1` | all 8 linked objects (`adler32`, `crc32`, `deflate`, `inflate`, `inffast`, `inftrees`, `trees`, `zutil`) match 100% |
| libpng 1.2.8 (FreeImage's copy) | `/O1 /DPNG_iTXt_SUPPORTED` | all 14 linked objects match 100%. FreeImage's `pngconf.h` adds the iTXt define; it changes `sizeof(png_text)` and adds `png_handle_iTXt`/`png_write_iTXt` |
| libjpeg 6b (FreeImage 3.7.0's copy) | `/O1` | all 37 linked objects match 100% |
| Tongas2 engine | `/O2 /GX` (the defaults) | `SRESULT` matches 100%, code and data |

FreeImage's own project files build its libraries with `/O2 /Ob2 /Oi /Ot /G7`, but the copies in `Tornado.exe` match `/O1`: Wanako rebuilt FreeImage with their own settings.

## Matching notes

Things learned while matching engine code with `/O2`:

- **Inline vs. out of line.** A small function that gets inlined in some callers and called in others (e.g. `SRESULT::GetTypeName`) was an inline member defined in the header. Making it a normal function changes register allocation in its callers: the compiler knows which registers a function defined earlier in the same file uses, but can't assume that for an inline function, because the linker may pick another file's copy.
- **`bool` results.** `return x ? true : false;` or `x == 0 ? false : true` compile to `cmp [mem], 0` / `setne al`; `return x != 0;` adds an `xor eax, eax`.
- **Small structs returned by value.** Whether a struct has a user-written copy constructor changes which registers copy it (`V2` has one, `V3` doesn't).
- **Finding units.** Game classes log errors as `"CLASS::Method> Error ..."`, so string references (exported by `tools/ghidra/ExportStringRefs.java`) give a class's methods and roughly where its file starts and ends. The class's own `.rdata` strings sit together in the same order.
