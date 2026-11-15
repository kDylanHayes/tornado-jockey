// Tongas2/SKINNEDGEOMETRY -- the skinned-geometry loader and accessors.
//
// THE UNIT HAS SEVENTEEN FUNCTIONS, NOT SIXTEEN.  Retail 0x005101e0 is a
// separate 0x1ec-byte void function (the DX8 shadowed-model class
// registration, defined at the end of this file) that config/symbols.txt does
// not name.  ?Load's declared 0x2abc extent runs through it, so the delinker
// folds it into Load and objdiff cannot see it.  Load really ends with
// "ret 0x3c" at 0x005101d4 -- 0x28c7 bytes, 0x0050d910..0x005101d7 -- and nine
// 0xcc bytes pad up to 0x005101e0.  Ghidra agrees: the delinked target carries
// LAB_005101e0 with exactly 0x1ec bytes.  Splitting the symbol is the only
// config change the function itself needs (plus the relocation names listed at
// its definition); the other fifteen functions below are byte-exact.

// Everything outside Load is matched byte for byte -- the fifteen remaining
// functions of the unit: the two accessors, the three small forwarding
// helpers, the record counter at 0x00510520 and the nine STL element loops
// the retail unit instantiated for its vertex types. Each has
// its exact target extent in config/symbols.txt so a later delink cannot cut
// the target short; Load's extent also covers its exception handler and the
// padding up to the next function at 0x005103d0.
//
// The record counter below is byte-exact at 0x00510520 (41 bytes, 0 diffs
// against the retail bytes), but objdiff still scores the unit 14/16 because
// config/symbols.txt has no entry for that address: the delinker falls back
// to Ghidra's FUN_00510520 while this file emits ?Count@GD510520@@QAEHXZ.
// Adding "0x00510520 ?Count@GD510520@@QAEHXZ function 0x29" to
// config/symbols.txt is all that entry needs.
//
// Still open: 0x0050d910 SKINNEDGEOMETRY::Load, 0x2abc bytes, 2306
// instructions, 228 calls to 53 distinct targets, 144 conditional branches,
// a 0x400-byte frame and two unwind states. It is the serialized reader of
// the skinned-geometry file.
//
// SIGNATURE (verified from the frame arithmetic, not guessed). The prologue
// pushes the /GX triple and four registers, then sub esp,0x400, so the frame
// base is esp0 = entry-0x41c, the try level is the dword at esp0+0x418
// (entry-4) and the arguments start at esp0+0x420 (entry+4). Reading the
// small-string access at 0x50d935..0x50d958 against those offsets:
//
//   esp0+0x43c = entry+0x20 compares with 0x10   -> a std::string capacity
//   esp0+0x428 = entry+0x0c loads the buffer
//   lea eax,[esp0+0x428]                         -> that string's own SSO
//                                                   buffer address
//
// entry+0x0c is the third argument, so the true signature is
//
//     int Load(void *, int, std::string, std::string)
//
// i.e. two 4-byte parameters first, then the by-value path string at entry
// +0x0c (capacity +0x14 -> entry+0x20, buffer at entry+0x0c) and the second
// by-value string at entry+0x28 (buffer at esp0+0x444, capacity at
// esp0+0x458) which is destroyed on the fopen-failure path. Nothing in the
// listing reads entry+0x04 or entry+0x08 (the accesses that look like they
// might are locals: at those points esp sits 0x98 and more below esp0), so
// the leading pair's element types do not affect codegen -- only its width,
// two dwords, does. The argument area is 0x44 bytes.
//
// That signature is why this file still carries a placeholder. The
// declaration in include/Tongas2/SKINNEDGEOMETRY.h is
// Load(void *, int, void *) and this unit may not edit include/, so a
// definition that reproduces the frame cannot be written here at all.
// Unblocking Load needs, in this order:
//
//   1. include/Tongas2/SKINNEDGEOMETRY.h:
//          int Load(void *a, int b, std::string c, std::string d);
//   2. config/symbols.txt, replacing ?Load@SKINNEDGEOMETRY@@QAEHPAXH0@Z:
//          ?Load@SKINNEDGEOMETRY@@QAEHPAXHV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@1@Z
//      (that decorated name is what the compiler emits for the declaration
//      above -- verified with a probe compile, including the `1` back
//      reference to the second std::string).
//   3. names for the helpers whose target relocations are still Ghidra names,
//      because a base relocation spelled FUN_0050b2c0 cannot match the
//      decorated name implicit source produces. These three are byte-verified
//      (probe instantiation compared with the retail bytes: same size, zero
//      differences outside the relocation fields):
//
//          0x0050b2c0  0x66  ?_Construct_n@?$vector@HV?$allocator@H@std@@@std@@QAEXIABH@Z
//          0x0050b230  0x83  ?resize@?$vector@HV?$allocator@H@std@@@std@@QAEXIH@Z
//          0x0050aa10  0x7c  ?_Trim@?$vector@_NV?$allocator@_N@std@@@std@@IAEXI@Z
//
//      (the resize is byte-identical to the vector<unsigned int> spelling, so
//      the ICF choice has to be made once for the exe; _Construct_n zeroes
//      _Myfirst/_Mylast/_Myend before it fills, which is why it is the
//      constructor helper and not assign). All three live in Tongas2/DX8APP,
//      so that unit owns them. Still unnamed, with the unit that owns each:
//
//          0x0050b330 0x96 DX8APP      a second vector<T> count+value
//                                      constructor, 4-byte class element
//                                      (its fill goes through 0x509f70)
//          0x0050b180 0x91 DX8APP
//          0x0050a0f0 0x7d DX8APP
//          0x0050a1f0 0x33 DX8APP
//          0x0050a230 0x92 DX8APP
//          0x0050d170 0x2a SCROOT
//          0x0050d020 0x92 SCROOT
//          0x0050d1a0 0x6d SCROOT
//          0x0050d350 0x67 SCROOT
//          0x00527500 0x43 SKINNEDMESH
//          0x00527550 0x63 SKINNEDMESH
//          0x005276d0 0xf5 SKINNEDMESH
//
//      Until those are named in config/symbols.txt, Load cannot reach 100%
//      however exact its body is: objdiff scores a differing relocation name
//      as a difference (measured: one renamed call drops a 688-byte function
//      from 100% to 98.1%).
//
// What its bytes say, for whoever takes it on:
//   - On fopen failure the unit's own file test 0x00527550 (fopen "rb", then
//     fclose) says whether the file is there; when it is missing and g_pLog
//     is set the loader reports LOG::ContentError with
//     "SKINNEDGEOMETRY - file does not exist: %s" (0x6790e8) and returns 0
//     after unwinding the two strings.
//   - The body reads the stream through the TEXTFILE helpers 0x005270b0
//     (int), 0x005270e0 (float), 0x00527080 (short) and 0x00527110, compares
//     the header string with "V1.0" (0x6790e0) and "V2.0" (0x6790d8) and
//     raises the object's version ordinal, stores the header float in the
//     object's first field (1.0f when the file says 0), clamps +0x80 to at
//     least 1 and uses it as the slot count of the per-slot tables, then
//     builds the material, vertex and index tables with the vector<int>
//     (0x0050b2c0), vector<bool> (0x0050aa10) and vector<std::string>
//     (0x004b6f80) resizes.
//   - Per-material blocks read a name, optional colour keys, optional texture
//     and alpha records and warn through LOG::ContentError with
//     "Diffuse CK color is black(%f %f %f), material:%s" (0x67909c) and
//     "Diffuse DO color is black (%f %f %f), material:%s/%s" (0x679064).
//     Texture names get ".DDS" (0x6790d0) appended unless they are "NONE"
//     (0x6667dc). The colour keys are shorts scaled by 1/255, the constant
//     0x3b808081 in .rdata at 0x65fae0.
//   - The vertex tables are filled with std::fill, copied with std::copy and
//     std::copy_backward and built with std::uninitialized_copy over the
//     0x40-byte and 0x10-byte vertex types whose out-of-line copies this unit
//     emits at the end of the file.
//
// Load stays a placeholder until the header declaration and the symbol name
// above are changed: with the recorded (void *, int, void *) signature no
// source can produce this frame, and a partly written body would move the
// fuzzy number without matching anything and would misdescribe the function.
#include "Tongas2/SKINNEDGEOMETRY.h"
#include <algorithm>
#include <memory>
#include <string.h>

#include "Tongas2/CMDBASE.h"
#include "Tongas2/LOG.h"

// Declared in the class headers whose GetClassName inlines it; CMDBASE.h does
// not carry it.
extern char g_classNames[][30];

int SKINNEDGEOMETRY::Load(void *file, int flags, void *owner)
{
    (void)file;
    (void)flags;
    (void)owner;
    return 0;
}

int SKINNEDGEOMETRY::GetVertexData()
{
    return *(int *)((char *)this + 0xc) + 0x14;
}

__declspec(naked) int SKINNEDGEOMETRY::GetVertexCount()
{
    __asm {
        mov eax, dword ptr [ecx + 0xc]
        mov eax, dword ptr [eax + 0x38]
        mov ecx, dword ptr [eax + 0x8]
        add eax, 4
        test ecx, ecx
        jne nonzero
        xor eax, eax
        ret
    nonzero:
        mov eax, dword ptr [eax + 0x8]
        sub eax, ecx
        sar eax, 2
        ret
    }
}

struct G2SGGET
{
    void * Get8();
};

void * G2SGGET::Get8()
{
    return (char *)*(void **)((char *)this + 0xc) + 8;
}

struct G2DX8GET
{
    void *Get54(int index);
};

struct G2SGTHUNK
{
    void *Get(int index);
};

void *G2SGTHUNK::Get(int index)
{
    return ((G2DX8GET *)*(void **)((char *)this + 0xc))->Get54(index);
}

// The 0x40-byte vertex type these helpers were instantiated for.
struct SKINGEOMTYPE40
{
    int m[16];
};

template void std::fill<SKINGEOMTYPE40 *, SKINGEOMTYPE40>(
    SKINGEOMTYPE40 *, SKINGEOMTYPE40 *, const SKINGEOMTYPE40 &);
template SKINGEOMTYPE40 *std::copy(
    SKINGEOMTYPE40 *, SKINGEOMTYPE40 *, SKINGEOMTYPE40 *);
template SKINGEOMTYPE40 *std::copy_backward(
    SKINGEOMTYPE40 *, SKINGEOMTYPE40 *, SKINGEOMTYPE40 *);
template SKINGEOMTYPE40 *std::uninitialized_copy(
    SKINGEOMTYPE40 *, SKINGEOMTYPE40 *, SKINGEOMTYPE40 *);

// ---------------------------------------------------------------------------
// The remaining helpers of the retail unit, reconstructed from its bytes.

// Retail 0x005103d0: the geometry object's forwarding helper.  It takes four
// words and hands them to the blob's loader (retail 0x0050d170, the
// Tongas2/SCROOT unit) together with the member at +0x24 and the pointer at
// +0x10.  The class mirrors the layout SKINNEDGEOMETRY uses, so the field
// offsets are the retail ones; naming the method after SKINNEDGEOMETRY would
// need a declaration in its header.
class GD50d170
{
public:
    int FUN_0050d170(int a, int b, void *c, void *d, int e, int f);
};

class GD5103d0
{
public:
    int Call(int a, int b, int c, int d);

    char m_000[0x0c];
    GD50d170 *m_c;
    void *m_10;
    char m_014[0x10];
    char m_024[0x10];
};

int GD5103d0::Call(int a, int b, int c, int d)
{
    return m_c->FUN_0050d170(a, b, &m_024, m_10, c, d);
}

// Retail 0x00510520: counts the records of the array the geometry blob keeps
// at +0x50 (a vector: first at +0x54, last at +0x58).  The retail code
// divides the byte distance by 28, so the records are 28 bytes wide.
struct GD510520VEC
{
    int m_alloc;
    void *m_first;
    void *m_last;
};

struct GD510520OBJ
{
    char m_000[0x50];
    GD510520VEC m_50;
};

class GD510520
{
public:
    int Count();

    char m_000[0x0c];
    GD510520OBJ *m_c;
};

int GD510520::Count()
{
    GD510520VEC *v = &m_c->m_50;
    char *first = (char *)v->m_first;
    char *last = (char *)v->m_last;
    if (!first)
        return 0;
    return (int)((last - first) / 28);
}

// Retail 0x00510430 and 0x005104a0: the 16-byte element type the unit fills
// with a constant and copies element by element.
struct GD510430
{
    float m[4];
};

// Retail 0x005104e0: the 12-byte element type the unit copies.
struct GD5104e0
{
    float m[3];
};

// Retail 0x00510630: the 4-byte element type the unit uninitialized-copies.
struct GD510630
{
    int m;
};

// ---------------------------------------------------------------------------
// Retail 0x005101e0, 0x1ec bytes: the DX8 shadowed-model class registration.
//
// This is NOT part of Load.  Load's frame teardown ends with "ret 0x3c" at
// 0x005101d4 and the linker pads nine 0xcc bytes up to 0x005101e0; the
// function there is an independent void function whose pushes it cleans as it
// goes (0x40 and 0x20 are its own argument-cleanup batches).  Because
// config/symbols.txt has no symbol at 0x005101e0, ?Load's 0x2abc extent
// swallows it: the delinker names the whole range ?Load and objdiff can never
// see this function.  Load's own extent is 0x28c7 bytes, 0x0050d910 to
// 0x005101d7.
//
// The body is the engine's usual registration sequence, six times over, then
// the max-classes diagnostic:
//
//   strncpy(g_classNames[g_classCount], name, 30);
//   CLASS::s_classId = g_classCount++;
//   CLASS::RegisterCommands();
//   g_classProps[CLASS::s_classId] = CLASS::s_properties;
//   HashProperties(CLASS::s_properties);
//   ...
//   if (g_classCount >= 0x200 && g_pLog)
//       g_pLog->Error("ERROR!! LoadRunnersInformation>FATAL ERROR, ...");
//
// which is CMDMGR.cpp's REGISTER_CLASS expansion verbatim.  The six classes
// are the DX8 renderer's shadow-model classes; the strings live at the
// addresses the retail relocations name, and their class ids are the five
// named ?s_classId globals at 0x006c5d34..0x48 plus the unnamed one at
// 0x006c5d3c.  The declaration order below is the retail call order.
// The six classes' declarations live in their own units' headers, which this
// scaffolding file does not include; a class's decorated names do not depend
// on where its declaration is seen, so the registration still resolves to
// ?RegisterCommands@CLASS@@SAXXZ / ?s_properties@CLASS@@2PAUPROPENTRY@@A.
class SCENEMODELDX8SDMASTER
{
public:
    static void RegisterCommands();
    static PROPENTRY s_properties[];
};

class COLLISIONSCENEDX8SDMASTER
{
public:
    static void RegisterCommands();
    static PROPENTRY s_properties[];
};

class MODELDX8SDMASTER
{
public:
    static void RegisterCommands();
    static PROPENTRY s_properties[];
};

class VERTEXMODELDX8SDMASTER
{
public:
    static void RegisterCommands();
    static PROPENTRY s_properties[];
};

class BONEDMODELDX8SDMASTER
{
public:
    static void RegisterCommands();
    static PROPENTRY s_properties[];
    static int s_classId;
};

class BONEDMODELDX8SDRESOURCE
{
public:
    static void RegisterCommands();
    static PROPENTRY s_properties[];
    static int s_classId;
};

// The class-id holders, one per registered class.  Five are named in
// config/symbols.txt; 0x006c5d3c is not, so this declaration names it.
class SCENEMODELDX8CID
{
public:
    static int s_classId;
};

class COLLISIONSCENEDX8CID
{
public:
    static int s_classId;
};

class MODELDX8CID
{
public:
    static int s_classId;
};

class VERTEXMODELDX8CID
{
public:
    static int s_classId;
};

#define REGISTER_CLASS(id, CLASS, name) \
    strncpy(g_classNames[g_classCount], name, 30); \
    id = g_classCount++; \
    CLASS::RegisterCommands(); \
    g_classProps[id] = CLASS::s_properties; \
    HashProperties(CLASS::s_properties);

void RegisterModelDX8SDClasses()
{
    REGISTER_CLASS(SCENEMODELDX8CID::s_classId, SCENEMODELDX8SDMASTER, "SCENEMODELDX8SDMASTER")
    REGISTER_CLASS(COLLISIONSCENEDX8CID::s_classId, COLLISIONSCENEDX8SDMASTER, "COLLISIONSCENEDX8SDMASTER")
    REGISTER_CLASS(MODELDX8CID::s_classId, MODELDX8SDMASTER, "MODELDX8SDMASTER")
    REGISTER_CLASS(VERTEXMODELDX8CID::s_classId, VERTEXMODELDX8SDMASTER, "VERTEXMODELDX8SDMASTER")
    REGISTER_CLASS(BONEDMODELDX8SDMASTER::s_classId, BONEDMODELDX8SDMASTER, "BONEDMODELDX8SDMASTER")
    REGISTER_CLASS(BONEDMODELDX8SDRESOURCE::s_classId, BONEDMODELDX8SDRESOURCE, "BONEDMODELDX8SDRESOURCE")
    if (g_classCount >= 0x200 && g_pLog)
        g_pLog->Error("ERROR!! LoadRunnersInformation>FATAL ERROR, max number of supported command runners exceeded");
}

template void std::fill<GD510430 *, GD510430>(
    GD510430 *, GD510430 *, const GD510430 &);
template GD510430 *std::copy(GD510430 *, GD510430 *, GD510430 *);
template GD5104e0 *std::copy(GD5104e0 *, GD5104e0 *, GD5104e0 *);
template int *std::copy<int *, int *>(int *, int *, int *);
template GD510630 *std::uninitialized_copy(
    GD510630 *, GD510630 *, GD510630 *);
