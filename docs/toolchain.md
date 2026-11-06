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

The toolkit is not redistributable, so this repository cannot include it. Get `VCToolkitSetup.exe` yourself (SHA-1 `50dc6cca1503f08a41d56816f3f1abcaf347b0bd`), then unpack it without running the installer. On Windows the extractor uses 7-Zip and the MSI tables; on Linux and macOS it uses `msiextract` from msitools. Nothing from the package is executed.

On Windows:

```bat
py tools\extract_vctoolkit.py C:\path\to\VCToolkitSetup.exe tools\vc71
tools\vc71\bin\cl.exe
rem Microsoft (R) 32-bit C/C++ Optimizing Compiler Version 13.10.3077 for 80x86
```

On Linux or macOS:

```sh
python3 tools/extract_vctoolkit.py /path/to/VCToolkitSetup.exe tools/vc71
wine tools/vc71/bin/cl.exe
# Microsoft (R) 32-bit C/C++ Optimizing Compiler Version 13.10.3077 for 80x86
```

`tools/vc71/` is git-ignored. You can also run the installer normally and point `--msvc` at the folder that contains `bin\cl.exe`.

Then configure the project with that folder:

```bat
py configure.py --msvc tools\vc71
```

You can also set `TJ_MSVC` instead of passing `--msvc`. On Linux and macOS `configure.py` runs the compiler through `wine` by default; on Windows it calls `cl.exe` directly, so no `--wine` is needed.

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

The engine units were swept against alternatives, and the defaults are the ones that match:

| Unit | Tried | Result |
|---|---|---|
| `Tongas2/CAMERA` | `/G7` (Pentium 4 scheduling) | 98.57% -> 95.47% |
| `Tongas2/CAMERA` | `/Oa` (assume no aliasing) | 98.57% -> 92.73% (`MATRIX::View` 92% -> 48%) |
| `Tongas2/CAMERA` | `/Ob1` | 98.57% -> 94.25% |
| `Tongas2/SCENE` | `/Ob1` | 97.63% -> 96.60% |
| `Tongas2/DECAL` | `/Ob1` | 97.45% -> 96.51% |
| `Tongas2/CPRIMITIVE3` | `/Ob1` | 98.74% -> 98.14% |
| `Tongas2/HEIGHTMAPDX8` | `/Ob1` | 97.55% -> 96.19% (`UpdateVertices` 86% -> 79%) |
| `Tongas2/GEOMETRY` | `/Ob2` (the default) | 95.07% -> 94.21%, so `GEOMETRY` keeps its `/Ob1` (`CollideBoxSegment` 72% -> 67%) |

The scheduling differences left in the terrain, geometry and camera functions are therefore not a flag choice: they come from the source the original was written in (see the register notes below).

## Matching notes

Things learned while matching engine code with `/O2`:

- **Inline vs. out of line.** A small function that gets inlined in some callers and called in others (e.g. `SRESULT::GetTypeName`) was an inline member defined in the header. Making it a normal function changes register allocation in its callers: the compiler knows which registers a function defined earlier in the same file uses, but can't assume that for an inline function, because the linker may pick another file's copy.
- **`bool` results.** `return x ? true : false;` or `x == 0 ? false : true` compile to `cmp [mem], 0` / `setne al`; `return x != 0;` adds an `xor eax, eax`.
- **Small structs returned by value.** Whether a struct has a user-written copy constructor changes which registers copy it (`V2` has one, `V3` doesn't).
- **Finding units.** Game classes log errors as `"CLASS::Method> Error ..."`, so string references (exported by `tools/ghidra/ExportStringRefs.java`) give a class's methods and roughly where its file starts and ends. The class's own `.rdata` strings sit together in the same order.
- **Case order.** MSVC lays out a switch's case bodies in source order, so the order of the blocks in the exe gives the order of the `case` labels (`TOKENS::Parse` lists its states 0, 9, 3, 4, 5, 2, 1, 10, 6, 7, 8).
- **Case order can differ from the enum order.** The jump table still indexes by case value, so the enum order says nothing about where the bodies sit. Sort the target jump table's entries by body address to recover the original source order (the `LAB_switchD_...::switchdataD_...` name gives the table's address). In `CMDBASE`, `GetShortStringAttribute` and `ResizeArrays` put their `PROP_V2`/`PROP_V3` cases before the string cases, not after `PROP_CHARS`; `SetAttribute` and `SetAttributeEval` put `PROP_V3` before `PROP_V2`, and `SetAttributeEval` puts `PROP_ENUM` before `PROP_INT`. Reordering the source to match took `GetShortStringAttribute` from 63.8% to 82.6% and `ResizeArrays` from 67.0% to 84.6%.
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
- **Returning a call's result.** `return (this->*f)(args);` builds the result into a temporary and copy-constructs the return value from it (`RESOURCEMASTER::Execute`). The exe tracks only the temporary in its EH table, which needs `SRESULT::FreeValue` declared `throw()`: otherwise the compiler adds a state and a flag for the return value, since the temporary's destructor could throw after it is built. `GLOBALDEFSMASTER::Execute` and `ACTORRESOURCESMASTER::Execute` do have that state and flag, so those two files define `SRESULT_FREEVALUE_MAY_THROW`, which drops the `throw()`. Why the original's objects differ here is not known; it may be a different header version or include order. `CMDMGR` likewise was built seeing `STRINGMANAGER::Free` as not throwing (its last `STRING` is destroyed with no state change), so it defines `STRINGMANAGER_FREE_NOTHROW`; the other units weren't. It also expands `SRESULT::FreeValue` inline in `CMDMGR::Update` but calls it in `CMDMGR::Run`, so the file defines `FreeValue` `inline` itself: the compiler then decides per call site, as the original's did. Writing out a case for each type, rather than one shared case, gives the direct jump table the exe has. The GUI behaviors (`GUIBEHAVIOR.cpp` and the four units after it) were built seeing their own 2D vector operators: `V2` scaled by a float taken by value, `+=`, `!=`, and the divide by zero check inline, where `GUICONTROL` calls it; their files define `V2_GUIBEHAVIOR` for those. `GUIBEHAVIOR::SetParams` also calls `STRING::Free` where every other unit inlines it, and destroys its tokens by clearing their string, so its file defines `STRING_FREE_OUTOFLINE`, `TOKENS_INLINE_DTOR_CLEAR` and `STRINGMANAGER_FREE_NOTHROW`.
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
- **Chained assignments store the innermost target first.** VC7.1 evaluates `m_time = m_duration = value` right to left, so the object the exe stores first is the right-hand one. `TIMEDBEHAVIOR`'s constructor writes `m_time` (the time left) before `m_duration`, which is `m_duration = m_time = atof(...)`, not the other way round; both spell the same values, so only the byte order tells them apart.
- **Static functions get their own calling convention.** At /O2 VC7.1 passes a `static` function's arguments in registers of its choosing. `PATH`'s `Hermite` is called the normal way in the exe, so it is not `static`.
- **Unsigned counters passed by `const int &`.** Passing an `unsigned int` loop counter to a `const int &` parameter makes a temporary copy on every call, and its tests compile to `jb`/`jbe`. That is what `PATHMASTER::SetDeletePoint` does in the exe.
- **Implicit conversions run later.** `SetCommand(i, "")` builds the `STRING` temporary after the other arguments are evaluated, where `SetCommand(i, STRING(""))` builds it first.
- **CRT names.** The game calls `_strdup`, `_stricmp` and `_strnicmp`, which link to `__strdup` and so on.
- **Tables in headers.** A `static const` table defined in a header is emitted in every file that includes it, under the static name `_s_TypeNames`, which is why `SRESULT`'s type names appear twice in `.rdata`.
- **Declared but never defined.** `FIXEDSTRING::operator+=` is only declared in the header: the exe calls its one out-of-line copy everywhere, and with the body visible VC7.1 inlined it into `CMDBASE::DumpProperties`.
- **Early return for a null check.** `CWSTRING::Length()` is `if (!m_str) return 0; return wcslen(m_str);`: the compiler then returns the null pointer already in `eax` as the 0, as the exe does, where the ternary adds an `xor eax, eax`.
- **The frame size tells how many temporaries there are.** In `CMDBASE::SetAttributeEval` the setter branch builds a `CWSTRING`, `FIXEDWSTRING` and so on from the value, but the branch without a setter assigns the raw text to the field. Building the temporary in both made the frame 0x630 bytes bigger than the exe's.
- **Loop conditions are tested in order.** `for (i = 0; i < 1 && i < count; i++)` tests `count` at the top and `i < 1` at the bottom, as `CMDBASE::DumpProperties` does; the other order tests both at the bottom.
- **A base class's members come before the vtable pointer too.** `CMDCONTEXT`'s constructor zeroes two members, then stores its vtable pointer, so those two belong to an inline base class constructor (whose own vtable store the compiler drops).
- **No Platform SDK.** The Toolkit has no `windows.h`; `include/windows.h` is a minimal stand-in with the SDK's declarations. Add to it as units need more.
- **A symbol-choice mismatch can block a whole function.** `CURVE::SetNumPoints` compares a loop pointer against `s_indices + 0xc02`, two bytes past the array and one past `s_indicesReady` (0x9eed18). Our object writes it relative to `s_indices`; the delinked target writes the same address (0x9eed1a) as `s_indicesReady + 2`, because Ghidra has to pick a nearby symbol and only exact symbol starts are used. Neither a `symbol_mappings` entry nor widening `s_indices`' size in `symbols.txt` changes it, since the addends differ (0xc02 vs 2). Leave such a function partial rather than invent source that references the neighbouring variable.
- **Switch lowering can differ for the same source.** `PATHMASTER::GetSegmentLength` switches on `m_curvature`; the exe has a four-entry jump table (0,1,2 call `MeasureCurve`, 3 skips it) while VC7.1 emits a range check for the natural three-case source, and splitting the three labels into separate identical case bodies only turns that into a compare chain. A jump table needs four *distinct* bodies at the point the compiler lowers the switch, which this source cannot express without changing what the case does. The function stays partial unless someone finds a construction that forces the table.
- **Tail duplication at a loop entry.** In `CONSOLE::Update` the target's first loop exits to the second loop's shared condition header (`cmp line, CONSOLE_LINES`), but VC7.1 tail-duplicates our source's zero-iteration path straight to the body, so the `je` lands 5 bytes later (`0xe5` instead of `0xe0`). The instruction addresses are otherwise identical and the raw exe has the same bytes, so this is not delinker padding. `#pragma optimize("t", off)` wrecks the function and `("s", off)` does not change it; making the bound `volatile` restores the header but adds loads (95.1% instead of 99.98%). Left as the last 0.02% of the unit.
- **A call's result compared to zero.** `if (!strstr(text, "GVIS:dplogo"))` compares the call result directly and compiles to `test eax, eax`, but the exe has `cmp eax, esi` against the function's zero register (`RESOURCEMANAGER::IsListLoaded`). Naming the result first (`char *found = strstr(...); if (!found ...)`) makes the comparison a variable test and gives the `cmp`.
- **A constant in a callee-saved register, or an immediate.** The exe often keeps `0` or `-1` in a callee-saved register for a whole function (`SCENE::Update` starts `xor ebp, ebp` and later does `cmp ecx, ebp` and `mov [ebx+0xec8c], ebp`), where our build rematerialises the immediate (`test ecx, ecx`, `mov dword ptr [...], 0`). The two forms differ in one instruction, so objdiff then sees every branch displacement in the function move. It is a register-pressure outcome: the exe recomputes a member's address with a scratch register (`lea ecx, [ebx+0xec90]`) where our object keeps it in a callee-saved one, leaving that register free for the constant. Changing the loop shape (`for` to `while`), naming the address or hoisting locals did not move it in `SCENE::MessageSpawned` (0%), `SCENE::Update` (92%) or `GUIGROUP::Update`; treat it as a watermark unless the original statement order can be recovered.
- **`-1` shared by a temporary and a loop test.** `SCENE::MessageSpawned` keeps the `-1` of `RESHANDLE()` and of `i != -1` in one register (`or edi, 0xffffffff`) where our object stores and compares the immediate. Every block after the constructors then differs, so the function scores 0% even though the code does the same thing.
- **vtable stores with a `__real@...` addend.** Template ctors and dtors that store their vtable show the target as `mov [esi], __real@c3b40000+4` against our `mov [esi], ??_7<vtable>`. The delinker names the address by the nearest symbol it has; adding the vtable itself to `symbols.txt` at its address is what would let the two agree, and `symbol_mappings` cannot (the addends differ).
- **EH unwind-state stores.** `HEIGHTMAPDX8::ParseTilesInfo` is ten instructions off: three `local_4 = 5/6/7` stores before the three `TOKENS` destructors are missing from our build, plus four addressing forms written `[edx+eax+0x2]` where the exe has `[eax+edx+0x2]` (same registers, the other SIB base/index choice). Making `~TOKENS` out of line removes the inlining and drops the function to 92.78%.
- **Some functions hold bytes no source can produce.** The retail image keeps a few code runs unpatched where the launcher normally writes the real code in, so they decode as instructions no C++ compiler emits (`aad`, `bound`, `cli`, `salc`, `int3` inside the body). `tools/ghidra/FindJunk.java` lists every function holding one. Those functions can never reach 100%, whatever the source: `OOBOX3::IntersectFace` has 25 such bytes at `0x4da0b7` (`call` to a target outside the image, then `aad`/`bound`), `HEIGHTMAPDX8MASTER::LoadImages` at `0x4e3d4e`, `CPRIMITIVE3RESOURCE::ShowHelper` at `0x4bf221`, and `SAFEINDEX::New` at `0x490a50`. Leave them partial; matching the code around the run is still worth doing.
- **Four separate locals, not an array.** `OOBOX3::IntersectFace` fills four corners the switch picks; written as a `PRIMV3 c[4]` array VC7.1 emits the EH vector-constructor helper (`??_H`) for it, which the exe does not have. Four plain `V3 c0..c3` locals match the exe's shape and take the function from 64.57% to 67.91%.
- **A vtable the delinker names by its neighbour.** When a ctor or dtor stores a vtable and `symbols.txt` has no name at the vtable's address, the delinked target writes the store relative to whatever symbol is nearest (`__real@c3b40000+0x4`, `??_C@_08FDCJCDOL@FileName?$AA@+0xcc`), and no source can match it. Naming the vtable itself at its address fixes it, because the delinker then emits the store with addend 0. Five were missing: `??_7?$LIFO@URESHANDLE@@$0BDII@@@6B@` at `0x667ed0`, `??_7?$LIFO@PAVACTOR@@$0BDII@@@6B@` at `0x667ed4` and `??_7?$SAFEINDEX@VACTOR@@$0BDII@$0PL@$00@@6B@` at `0x667f5c` (all three right after `__real@c3b40000` at `0x667ecc` and the `"OneNode"` string), and `??_7SOUND@@6B@` at `0x66c270` and `??_7SOUNDMASTER@@6B@` at `0x66c390` (after the `"FileName"` string at `0x66c1a4`). That took ten functions from 97.50-99.87% to 100% with no other change. `tools/ghidra/FindSymbol.java` prints a symbol's address and the bytes there; `tools/ghidra/vt_sweep`-style scanning of our objects found twelve more vtables `symbols.txt` does not name.
- **A relocation the delinker never emits.** Ghidra's listing sometimes stops a function at a `call` and leaves the rest of its body undefined (`??1SOUNDMANAGER` at `0x4b39d3`-`0x4b39f2`, with the `call ~RESOURCEMANAGER` at `0x4b39ee`). The bytes are exported without a relocation, so objdiff shows the call as a raw number (`call 0xfffef040`). Disassembling the gap does create the reference (`UNCONDITIONAL_CALL -> 0x4a29c0`), but the exporter still emits no relocation for it, and disassembling gaps across the whole program costs 17 matches elsewhere (`3306` exact against `3323`). Not a source problem and not fixed; 21 functions across the image show this shape.
- **A relocation addend is kept only for a symbol defined in the exported unit.** `dCollideRTL`/`dCollideSTL` reach the `dxTriMesh` collider statics (`?_RayCollider@dxTriMesh@@2V...`, `?Faces@...`) which live in `collision_trimesh`'s data, so in those units they are undefined and the delinker writes the operand as symbol+0: `mov ecx, [?_RayCollider...]` against our `[?_RayCollider...+0x4]`. The reference itself is right (`READ -> 0xa0fa1c`, which is `_PlanesCollider+4`); only the exported addend is lost. Giving the referring unit the data range as well (`0x00a0fa18-0x00a0fe97`) makes the exporter define those symbols, so the addends come back and both functions match. The same trick fixes `??0dxTriMesh` in the owning unit, but there the data then gets compared and `?_OBBCollider` is 308 bytes in our object against the image's 360 (a separate `Opcode::OBBCollider` layout gap), which costs more than the function gains; leave that one for whoever fixes the class size. `tools/ghidra/ProbeRefs.java` prints an instruction's references for this kind of check.
- **`CMDENTRY` is 32 bytes, and `PROPENTRY` 56 — already.** A claim that the command tables are 32 bytes per entry while ours are 20 does not hold: compiling `sizeof` with the Toolkit (`cl /c /Iinclude`, headers as they are in `include/Tongas2/CMDBASE.h`) gives `CMDENTRY 0x20`, `PROPENTRY 0x38`, `CMDFUNC 0x10`, `PROPFIELD 0xc`. `CMDBASE.h` forward-declares `class CMDBASE` and then declares the four member-pointer typedefs before the class body, so VC7.1 uses the general representation (16-byte member function pointer, 12-byte data member pointer) exactly as the exe does, and `CMDENTRY` is `name, hash, minArgs, maxArgs` plus those 16 bytes. The image agrees entry for entry: `?s_commands@CMDBASE` is 0xa0 (5 entries), `?s_commands@RESOURCES` 0xc0 (6), `?s_commands@ACTOR` 0x2c0 (22), the one-command tables (`DECALMASTER`, `PATHMASTER`, `EMITTERMASTER`, `RESOURCEMASTER`, `RESOURCE`, `SOUNDMASTER`, all at `0x7e0cf0`-`0x7e0d90`) 0x20 each. An entry at `0x6ba840` reads `name, hash=0, min, max,` then sixteen zero bytes — the member pointer is filled by the `_$E` dynamic initializer, which is why the image's function dwords look empty — and our object's table at the same stride holds the same bytes apart from the name relocations. objdiff already scores `?s_commands@CMDBASE` (160 bytes) and `?s_commands@RESOURCES` (192) at 100%, `?s_properties@RESOURCES` (168) at 100%, `?s_properties@ACTOR` (3528) at 99.55%, and `?s_commands@ACTOR` (704) at 90.86% on entry contents, not layout. Comparing every `s_commands`/`s_properties` symbol in all 219 units (129 tables) finds no size mismatch on either side — the eleven tables below 100% differ in entry values, never in stride. Nothing in the shared structures is left to fix, so the register and frame differences in `CMDBASE`, `KERNELCOMMANDS`, `ACTOR` and `RESOURCES` have another cause.
