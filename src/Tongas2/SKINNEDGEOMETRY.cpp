// Tongas2/SKINNEDGEOMETRY -- the skinned-geometry loader and accessors.
//
// Sixteen of the unit's seventeen bodies are byte-exact: the two accessors, the
// three small forwarding helpers, the record counter at 0x00510520, the DX8
// class registration at 0x005101e0 (0x1ec bytes, paired as
// ?RegisterModelDX8SDClasses@@YAXXZ), and the nine STL element loops the retail
// unit instantiated for its vertex types.
//
// Still open: 0x0050d910 ?Load, 0x28c7 bytes (0x0050d910..0x005101d7, ending
// "ret 0x3c" and nine 0xcc pad bytes before the registration function at
// 0x005101e0). 2306 instructions, 228 calls to 53 distinct targets, 280
// relocations to 70 distinct symbols, 144 conditional branches, a 0x400-byte
// frame.
//
// ---------------------------------------------------------------------------
// SIGNATURE: int Load(void *, std::string, std::string).  Verified three ways.
//
//   * The frame. The prologue pushes the /GX triple and four registers, then
//     "sub esp,0x400", so esp0 = entry-0x41c; the try level is the dword at
//     esp0+0x418 (entry-4), and the epilogue's "add esp,0x40c" + "ret 0x3c"
//     pops 0x3c bytes of parameters.
//
//   * The caller. The single call site in the image, 0x00500cfc, is
//     "sub esp,0x1c" (string A), "sub esp,0x1c" (string B), "push <dword>",
//     "call 0x0050d910".  0x1c + 0x1c + 4 = 0x3c.  So the parameters are one
//     four-byte value followed by two by-value strings -- three parameters,
//     not four.
//
//   * The body.  The string it fopen()s has its buffer at entry+0x0c and its
//     capacity at entry+0x20; the second string has them at entry+0x28 and
//     entry+0x3c.  With this build's std::string (see below, 0x1c = 28 bytes,
//     buffer at +4, size at +0x14, capacity at +0x18) those are the objects at
//     entry+0x08 and entry+0x24, i.e. parameters 2 and 3.  entry+0x04 and
//     entry+0x08-as-a-word are never read: the first parameter is unused by the
//     body and its element type cannot be observed, only its width (one word).
//
// The decorated name the compiler emits for that declaration -- checked with a
// probe compile of this toolchain, not guessed -- is
//
//   ?Load@SKINNEDGEOMETRY@@QAEHPAXV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@1@Z
//
// ---------------------------------------------------------------------------
// WHAT HAS TO CHANGE OUTSIDE THIS FILE.  Until then no source can produce this
// frame and the body below stays a placeholder.
//
// 1. include/Tongas2/SKINNEDGEOMETRY.h -- replace
//
//        int Load(void *file, int flags, void *owner);
//
//    with
//
//        int Load(void *file, std::string path, std::string other);
//
//    (the image's single call site, 0x00500cfc, already passes two strings, so
//    it is this declaration that is wrong, not the caller).
//
// 2. config/symbols.txt -- replace line 7614
//
//        0x0050d910 ?Load@SKINNEDGEOMETRY@@QAEHPAXH0@Z function 0x28c7
//
//    with
//
//        0x0050d910 ?Load@SKINNEDGEOMETRY@@QAEHPAXV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@1@Z function 0x28c7
//
//    (keep the 0x28c7 extent: it is the retail body's, and the delinker uses
//    it to split 0x005101e0 off as RegisterModelDX8SDClasses).
//
// 3. config/symbols.txt -- the relocations that still point at Ghidra or
//    synthetic names.  Every one of them has to be named or the body scores
//    99.x however exact it is, because objdiff compares relocation names.
//    No row is needed for the STL, CRT or already-decorated targets: the
//    compiler emits std::string/std::vector members, operator+, operator
//    new/delete, fopen/fread/fseek/ftell/fclose, the textfile readers
//    ?GD5270b0/?GD5270e0/?GD527080/?GD527110, ?FUN_004f82a0, the SCROOT
//    members (?Init@SCROOT, ??0SCROOT@, ?CreateSCROOT), ??0GD53bae0 and
//    ?Add@GD50d0c0 under exactly the names the target already carries; those
//    only need declarations with the same signatures.
//
//      address    size  target symbol in the delinked object   owner
//      0x004fe350 0x8c0 _m_55                                    (C name, keep)
//      0x004fec10 0x3e3 _m_56                                    (C name, keep)
//      0x00501350 0x12  _m_81                                    (C name, keep)
//      0x00501370 0x26  _m_82                                    (C name, keep)
//      0x005013a0 0x25  _m_83                                    (C name, keep)
//      0x005013d0 0x52  _m_84                                    (C name, keep)
//      0x005027c0 0x1ad _m_98                                    (C name, keep)
//      0x0050a0f0 0x7d  FUN_0050a0f0                            DX8APP
//      0x0050a1f0 0x33  FUN_0050a1f0                            DX8APP
//      0x0050a230 0x92  FUN_0050a230                            DX8APP
//      0x0050b180 0x91  FUN_0050b180                            DX8APP
//      0x0050b330 0x96  FUN_0050b330  (vector<T>(n, value), 4-byte element)
//                                                              DX8APP
//      0x0050d020 0x92  FUN_0050d020                            SCROOT
//      0x0050d1a0 0x6d  FUN_0050d1a0                            SCROOT
//      0x0050d350 0x67  FUN_0050d350                            SCROOT
//      0x00527500 0x43  FUN_00527500                            SKINNEDMESH
//      0x00527550 0x63  FUN_00527550  (the fopen/fclose file test)
//                                                              SKINNEDMESH
//      0x005276d0 0xf5  FUN_005276d0  (reads one string record)
//                                                              SKINNEDMESH
//
//    The _m_NN ones are already C symbols in the binary (build/functions.txt
//    carries them with those spellings), so a plain "extern \"C\"" declaration
//    reproduces the target's relocation name exactly -- they need no config
//    row, only a declaration in the unit that defines them.
//
//    Call shapes seen at the Load call sites, so the owning unit can be given
//    the signature that produces the target's relocation name:
//      0x00527550  bool f(std::string *path, bool *exists)
//                  (push &bool / push &path; return in al)
//      0x005276d0  std::string *f(std::string *out, FILE *file)
//                  (push file / push out; returns out in eax)
//      0x0050d350  ?f@CLASS@@QAEXH@Z -- thiscall, ecx = the 0x... table
//                  object, one int stack argument (the slot count)
//      0x0050b2c0  vector<int>::_Construct_n, 0x0050b230 vector<int>::resize,
//                  0x0050aa10 vector<bool>::_Trim, 0x0050d860
//                  vector<vector<int> >::resize -- already named.
//
// 4. config/symbols.txt -- five .rdata constants whose target symbols are
//    still Ghidra labels.  Our compiler names a literal from its bytes
//    ("rb" pairs as ??_C@_02JDPG@rb?$AA@ today, so the convention already
//    works), so each needs a row giving the literal's own symbol at the
//    address:
//
//        0x006790d0  ".DDS"
//        0x006790d8  "V2.0"
//        0x006790e0  "V1.0"
//        0x0067909c  "Diffuse CK color is black(%f %f %f), material:%s"
//        0x00679064  "Diffuse DO color is black (%f %f %f), material:%s/%s"
//
//    (0x006790e8, "SKINNEDGEOMETRY - file does not exist: %s", is already
//    named, as are 0x00661eac "rb", 0x006667dc "NONE" and the empty string.)
//
// ---------------------------------------------------------------------------
// ABI NOTE, useful for every unit that passes std::string by value: this
// build's basic_string is 0x1c bytes with _Bx (the 16-byte small-string
// buffer) at +4, _Mysize at +0x14 and _Myres at +0x18.  Verified by probe
// compiling the same cl 13.10.3077 and diffing the argument copy against the
// retail call site at 0x00500cc6: both emit
//     sub esp,0x1c / mov ecx,esp / mov [ecx+0x18],0xf / mov [ecx+0x14],0 /
//     mov byte ptr [ecx+4],0
// so a by-value string costs 0x1c bytes of argument area, and the sum of the
// parameter sizes is what the callee pops with "ret N".
//
// WHAT THE BODY DOES, for whoever writes it (all addresses are retail):
//   - m_00 is set to 1.0f, the path string (parameter 2) is fopen()ed "rb";
//     on failure 0x00527550 tests for the file and, when it is missing and
//     g_pLog is set, the loader reports LOG::ContentError with
//     0x006790e8 and returns 0.
//   - A std::string holds "V1.0"; 0x005276d0 reads the header record into a
//     second string; the header is compared with "V2.0" (0x006790d8) and the
//     version ordinal (a local, 1 or 2) selects the rest of the parse.
//   - 0x0050d350 constructs the per-slot table object from the slot count read
//     from the file (this+0x80, clamped to at least 1), then the vector<int>,
//     vector<bool> and vector<std::string> members are sized with
//     _Construct_n / _Trim / resize.
//   - The material loop reads a name, colour key records (shorts scaled by the
//     constant at 0x0065fae0, 0x3b808081 = 1.0f/255.0f), the diffuse and
//     ambient colours, and warns through LOG::ContentError with 0x0067909c and
//     0x00679064; texture names get ".DDS" (0x006790d0) appended unless they
//     are "NONE".  Each material record comes from _m_55/_m_56; the record
//     pointer goes into a per-material pointer array on the stack, and a
//     parallel stack vector<int> holds the record's kind (0, 2, 4..0xc) while
//     stack vector<bool>s hold the per-material seen-flags.
//   - The geometry pass seeks by the offsets read earlier, reads a 0x10-byte
//     vertex record per vertex (three coordinates plus a slot index, pushed
//     into vector<GD50ce90> after clamping the index to the slot count),
//     8-byte index records (pushed into vector<V3>), then per-slot SCROOT
//     objects (0x134 bytes, ctor ??0SCROOT@@QAE@HPBX@Z) filled through
//     GD50d0c0::Add.
//   - The tail closes the file, calls 0x0050a1f0/0x0050a230, and stores
//     this+0x38 (the CreateSCROOT result) into the mesh object at this+0x4c,
//     offset +0x34.
#include "Tongas2/SKINNEDGEOMETRY.h"
#include <algorithm>
#include <memory>
#include <string.h>

#include "Tongas2/CMDBASE.h"
#include "Tongas2/LOG.h"

// Declared in the class headers whose GetClassName inlines it; CMDBASE.h does
// not carry it.
extern char g_classNames[][30];

int SKINNEDGEOMETRY::Load(void *file, std::string path, std::string other)
{
    (void)file;
    (void)path;
    (void)other;
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
