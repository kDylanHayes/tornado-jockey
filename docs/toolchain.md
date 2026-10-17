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
| zlib 1.2.2 | `/O1` | `adler32` matches 100% (byte-identical `.text`) |
