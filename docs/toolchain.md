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
- **Case order.** MSVC lays out a switch's case bodies in source order, so the order of the blocks in the exe gives the order of the `case` labels (`TOKENS::Parse` lists its states 0, 9, 3, 4, 5, 2, 1, 10, 6, 7, 8).
- **Tail merging.** Identical statement tails get merged into one block, and which copy survives shows in the jumps. Statement order inside a case (`*out++ = c; state = x;` vs. the reverse) decides whether two tails are identical before scheduling, even when the final code is the same.
- **Inline shapes matter.** `STRING::c_str()` is `return !m_buf ? "" : m_buf;` (the reverse ternary lays the branch out the other way), and `STRING::Length()` returns early when the cached length is valid; the nested-`if` form allocates registers differently in callers.
- **Overloaded virtuals.** MSVC puts overloads of a virtual function into the vtable in the reverse of their declaration order, so `FILELOADER` declares `Read(buffer, size, count)` before `Read(buffer, size)`.
- **`new[]` and `delete[]`.** Without `#include <new>`, `new char[n]` calls `operator new` (`??2`); with it, `operator new[]` (`??_U`). The game includes it.
- **Early returns.** `if (!fp) return 0; if (_fstat64(...)) return 0; return st.st_mtime;` lays its blocks out differently from the same test joined with `&&`.
- **Calls into the same file.** The compiler knows which registers a function defined earlier in the same file leaves alone, and keeps values in them across a call. `FILELOADERWKZ::IsError` reloads `g_pLog` after calling `WKZ::ErrorString`, so `WKZ` was compiled in a file of its own.
- **Inline helpers.** An inline member that takes an argument evaluates it before its body runs, which moves loads ahead of the tests (`WKZSTREAM::Reserve` in `WKZ::Decompress`).
- **Calls that can't throw.** With `/GX`, the compiler drops the EH state change around a call it knows can't throw, including a call to a function defined in the same file that throws nothing. `WTLEADERBOARDIMPL::GetSubmitURL` sets a state around its call to `Hash`, so `Hash` (with `Xor` and `Encrypt` next to it) was compiled in another file, `WTCRYPT`.
- **Index variables.** `char c = ...; s += table[c];` gives the `movsx` the exe has; an `int` index doesn't (`Base64Encode`).
- **`/GS`.** A function with a local array reads `___security_cookie` and calls `@__security_check_cookie@4`. `WTLEADERBOARDIMPL` is built with `/GS`; the game's own files so far aren't.
- **Folded functions.** The linker folds identical functions (`/OPT:ICF`), so the empty `WTLEADERBOARDIMPL` constructor and destructor are calls to `??0Point@IceMaths@@QAE@XZ` and `?term_source@@...` in the exe; `symbol_mappings` on the unit pairs them.
- **No Platform SDK.** The Toolkit has no `windows.h`; `include/windows.h` is a minimal stand-in with the SDK's declarations. Add to it as units need more.
