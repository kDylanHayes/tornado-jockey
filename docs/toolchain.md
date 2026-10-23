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
- **Constant tables.** Reading a member of a `const` table at a constant index gets folded to an immediate, but copying a whole element doesn't, and a table whose element type isn't plain data goes in `.data` instead of `.rdata`. `APPOPTIONS::GetResolution` copies whole elements from an `.rdata` table into a struct returned through a hidden pointer, so the table holds a plain `DISPLAYMODE` and the result is a `RESOLUTION` built from one, the way ATL's `CSize` is built from a `SIZE`.
- **Floating-point expressions.** The compiler sometimes reorders the terms of a float sum on its own: in `TriangleNormal`, every ordering of the terms compiles to the same code. What did change it there and in the spline functions: the order the parameters are declared in, splitting a long expression into named locals (`TriangleNormal` only matches with each component computed into its own `float` first), and the order of locals (in `CatmullRom`, declaring `t3 = t*t*t` before `t2 = t*t` is what makes it spill `t2` to the stack). Results written through an out-pointer parameter are copied with `eax` alone, while `V3 r = ...; return r;` goes through `mov ecx,eax`, since VC7.1 has no named return value optimization.
- **What the compiler can see doesn't throw.** A call to an inline function whose body the compiler can see (and that calls nothing that throws) gets no EH state, even when it isn't inlined. The `RESOURCES` constructor builds a temporary `CSTRING` around a call to `CSTRING::operator!=` without a state change, so `operator!=` is defined inline in the header with `strcmp`, though it stays out of line in the exe.
- **Class statics vs. file statics.** The compiler moves a load of a file-`static` variable ahead of stores through `this`, because nothing outside the file can alias it. Variables the exe loads after those stores (`RESOURCES::s_numManagers` and the manager tables) are class statics, with external linkage.
- **Inlining is decided per call site.** In one function the same inline method can be inlined at one call and called at another. `RESOURCES::GetResource(RESHANDLE)` calls `m_masters.Get` but has the `m_copies` lookup inlined, so that lookup is written out by hand.
- **Member pointers to an incomplete class.** Pointer-to-member typedefs declared while `CMDBASE` is still incomplete use the general representation: 16 bytes for a member function pointer and 12 for a data member pointer (a null one is `{0, 0, -1}`). The command and property tables are built from these. Calls through a virtual member function pointer go through class-agnostic thunks named `??_9@$B<offset>@AE`.
- **Dynamic initializers.** A global that needs code to initialize gets a `_$E<n>` function in `.text$yc`, which the exe places from 0x60c9a0, and the `atexit` destructor helpers go in `.text$yd`, from 0x654470. How much of a table goes into code depends on its type. In a plain struct only the member pointers are stored in code, but once a member pointer sits in a union, every member from the first one that isn't constant to the end of the table is. The property tables (`PROPENTRY`) are like that: their field is a union of a data member pointer and a getter, so only the first entry's name, hash, type and the int after it stay in the data. A member left out of a brace initializer is written as a whole word of zeros, so the terminator spells out its `false` to get the byte store the exe has.
- **Returning a call's result.** `return (this->*f)(args);` builds the result into a temporary and copy-constructs the return value from it (`RESOURCEMASTER::Execute`). The exe tracks only the temporary in its EH table, which needs `SRESULT::FreeValue` declared `throw()`: otherwise the compiler adds a state and a flag for the return value, since the temporary's destructor could throw after it is built. `GLOBALDEFSMASTER::Execute` and `ACTORRESOURCESMASTER::Execute` do have that state and flag, so those two files define `SRESULT_FREEVALUE_MAY_THROW`, which drops the `throw()`. Why the original's objects differ here is not known; it may be a different header version or include order.
- **Member or free operators.** With `V3`'s `+` and componentwise `*` as members, the exe's sums load their operands the other way round; as free functions `operator+(a, b)` and `operator*(a, b)` they match, and so do `-`, `* float` and `float *` (which multiplies `s * v.x`, the order `EMITTER` needs). A free operator's arguments are evaluated right to left, so where the exe calls the left operand first (`GetPosition() + GetAngles().Transform(p)` in the particle constructor), the left side goes into a named local. `+=` works component by component, which keeps the sum in the variable instead of a temporary (`OBJECT::LocalToWorld`).
- **Member initializers come before the vtable pointer.** VC7.1 stores the vtable pointer after a constructor's member initializer list and before its body, so a constructor that sets members both before and after that store (`SOUND`'s) initializes all of them in the list.
- **An array of members with an inline constructor is built in place.** A two-element array whose type has an inline constructor came out as the original's unrolled pointer loop; an explicit loop over the array in the owner's constructor did not.
- **Copy a pointer before stepping it.** A loop that writes through `out` directly kept a second copy of its end store; `char *p = out;` with `*p++` in both branches matched.
- **`~STRING` doesn't throw.** The original's destructors run a `STRING` member's destructor without an exception frame around it, so `~STRING` is declared `throw()`; `GLOBALDEFSMASTER`'s implicit destructor, with no vtable pointer store, matched only then.
- **The STL is the Toolkit's own.** `std::map` from the Toolkit's headers compiles to the exe's tree code byte for byte, including the instantiations the linker kept in an earlier object (the tree rotations and `erase`, named in `symbols.txt`).
- **Inlining stops in big functions.** In functions as large as `PARTICLE::Update`, VC7.1 stops inlining small helpers, and the exe calls out-of-line copies of `V3` operators, `V3::Length` and `ANG::GetEuler*`. The exe's copies of those helpers are named in `symbols.txt` so the calls pair up. `EMITRANGE::Random` is `__forceinline` because it stays inlined in the exe there.
- **Log calls compiled out.** `OBJECT::UpdateAngles` is nothing but argument evaluation: a call to an empty inline logging method that the release build drops, while the inline getters in its arguments (which update the angles) stay. A variadic function is never inlined, so the stand-in, `LOG::Debug`, takes fixed arguments.
- **Named locals for call results.** A struct returned by a call and passed straight to another call is copied through `eax`, the returned pointer; one stored in a named local first is copied from the local's address. That decides whether two branches ending in the same call get tail-merged (`OBJECT::FollowParent`).
- **Calls the compiler must assume can throw.** A constructor body that calls a function after its base class is built needs an EH state to destroy the base, and VC7.1 won't inline such a constructor. Declaring `SafeSprintf` `throw()` is what lets `DECALMANAGER`'s constructor inline into its create function, as in the original.
- **Store order of zeroed members.** `x = y = z = 0.0f` stores z first; a small struct whose constructor does that (`DECALCOLOR`) reproduces a member zeroed back to front. Copying an array one component at a time (`v.x = a[0]; v.y = a[1]; ...`) keeps each load next to its store, where assigning a constructed `V3` goes through the stack.
- **Static functions get their own calling convention.** At /O2 VC7.1 passes a `static` function's arguments in registers of its choosing. `PATH`'s `Hermite` is called the normal way in the exe, so it is not `static`.
- **Unsigned counters passed by `const int &`.** Passing an `unsigned int` loop counter to a `const int &` parameter makes a temporary copy on every call, and its tests compile to `jb`/`jbe`. That is what `PATHMASTER::SetDeletePoint` does in the exe.
- **Implicit conversions run later.** `SetCommand(i, "")` builds the `STRING` temporary after the other arguments are evaluated, where `SetCommand(i, STRING(""))` builds it first.
- **CRT names.** The game calls `_strdup`, `_stricmp` and `_strnicmp`, which link to `__strdup` and so on.
- **Tables in headers.** A `static const` table defined in a header is emitted in every file that includes it, under the static name `_s_TypeNames`, which is why `SRESULT`'s type names appear twice in `.rdata`.
- **No Platform SDK.** The Toolkit has no `windows.h`; `include/windows.h` is a minimal stand-in with the SDK's declarations. Add to it as units need more.
