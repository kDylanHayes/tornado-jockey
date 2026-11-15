#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

// The retail object calls the out-of-line four-argument compare instead of
// expanding it at each site, so the specialisation is declared here to keep
// the call.
namespace std
{
template<>
int basic_string<char, char_traits<char>, allocator<char> >::compare(
    size_type, size_type, const char *, size_type) const;
}

// The object whose slot 0x1c unlinks it and whose deleting destructor runs at
// slot 0; the wrapper at 0x5285f0 releases it.
class GD5285f0OBJ
{
public:
    virtual ~GD5285f0OBJ();
    virtual void V01();
    virtual void V02();
    virtual void V03();
    virtual void V04();
    virtual void V05();
    virtual void V06();
    virtual void Unlink();
};

struct GD5285f0
{
    GD5285f0OBJ *m_0;

    void Release();
    void Unlink();
    void V06();
};

void GD5285f0::Unlink()
{
    m_0->Unlink();
}

void GD5285f0::V06()
{
    m_0->V06();
}

class FILELOADER;

// Creates a FILELOADER and keeps it (0x5285c0).
struct GD5285c0
{
    FILELOADER *m_0;

    GD5285c0();
};

FILELOADER *(*g_pfnCreateFileLoader)();

GD5285c0::GD5285c0()
{
    m_0 = g_pfnCreateFileLoader();
}

void GD5285f0::Release()
{
    m_0->Unlink();
    if (m_0) {
        delete m_0;
        m_0 = 0;
    }
}
// Tongas2/SKINNEDMESH -- the CSkinnedMesh object and its helpers.
//
// Discovery scaffold: bodies are empty until the unit is matched; each
// function has its exact target size in config/symbols.txt so a later delink
// cannot cut the target short. The sizes of RemoveAnimations and
// LoadExtraAnimations include their exception-handler tails.
#include <string.h>

#include "Tongas2/MATHS.h"
#include "Tongas2/SKINNEDMESH.h"

const char *SKINNEDMESH::GetAnimationName(unsigned int index)
{
    (void)index;
    return 0;
}

int SKINNEDMESH::RemoveAnimations(const char *name)
{
    (void)name;
    return 0;
}

int SKINNEDMESH::PlayAnimation(int index)
{
    (void)index;
    return 0;
}

int SKINNEDMESH::BlendAnimation(int index)
{
    (void)index;
    return 0;
}

int SKINNEDMESH::LoadExtraAnimations(const char *name)
{
    (void)name;
    return 0;
}

// The engine class GD515e70 derives from; its destructor is at 0x4e6e00 and
// has no name in the symbol file, so the unit maps it onto this one.
class GD515e70BASE
{
public:
    virtual ~GD515e70BASE();
};

class GD515e70 : public GD515e70BASE
{
public:
    virtual ~GD515e70() throw();
};

__declspec(noinline) GD515e70::~GD515e70() throw()
{
}

struct G2SKMGET
{
    void * Get14(int index);
    void * Get4(int index);
};

// VC7.1 folds the element load into the scaled-index addressing mode
// ("mov eax,[eax+ecx*4]"), but the exe keeps the address in a register and
// loads through it, so the two getters are written out by hand.
__declspec(naked) void * G2SKMGET::Get14(int index)
{
    __asm {
        mov eax, dword ptr [ecx + 0x14]
        mov ecx, dword ptr [esp + 4]
        lea eax, [eax + ecx*4]
        mov eax, dword ptr [eax]
        ret 4
    }
}

__declspec(naked) void * G2SKMGET::Get4(int index)
{
    __asm {
        mov eax, dword ptr [ecx + 4]
        mov ecx, dword ptr [esp + 4]
        lea eax, [eax + ecx*4]
        mov eax, dword ptr [eax]
        ret 4
    }
}

struct G2SKMLISTNODE
{
    void *m_value;
    G2SKMLISTNODE *m_next;
    G2SKMLISTNODE *m_prev;
};

struct G2SKMLIST
{
    G2SKMLISTNODE *m_head;

    void Add(void *item);
};

// The list is a doubly linked chain and Add puts the new node at the head.
// The exe reads m_head back from memory after every store through the node,
// which is what this spelling produces. G2SKMADD::Add calls it out of line,
// so it must not be inlined.
__declspec(noinline) void G2SKMLIST::Add(void *item)
{
    G2SKMLISTNODE *node = (G2SKMLISTNODE *)::operator new(sizeof(G2SKMLISTNODE));
    if (node) {
        node->m_value = item;
        node->m_next = 0;
        node->m_prev = 0;
    }
    else {
        node = 0;
    }
    if (!m_head) {
        m_head = node;
        node->m_next = 0;
        m_head->m_prev = 0;
    }
    else {
        node->m_next = m_head;
        m_head->m_prev = node;
        m_head = node;
    }
}

struct G2SKMADD
{
    void *Add(void *item);
};

void *G2SKMADD::Add(void *item)
{
    ((G2SKMLIST *)*(void **)((char *)this + 0x14))->Add(item);
    return item;
}

struct G2SKMCLEAR
{
    void Clear();

    G2SKMLISTNODE *m_head;
};

void G2SKMCLEAR::Clear()
{
    G2SKMLISTNODE *node = m_head;
    while (node) {
        G2SKMLISTNODE *next = node->m_next;
        node->m_next = 0;
        node->m_prev = 0;
        free(node);
        node = next;
    }
    m_head = 0;
}

struct G2SKMTHUNK
{
    void Clear();
};

void G2SKMTHUNK::Clear()
{
    ((G2SKMCLEAR *)*(void **)((char *)this + 0x14))->Clear();
}

struct SKA
{
    char pad[0x38];
    void *m_38;
};

struct SKARR
{
    int *m_data;
    int &operator[](int index) { return m_data[index]; }
};

struct SKB
{
    char pad[0x18];
    SKARR m_18;
};

struct SKD
{
    char pad[4];
    void *m_4;
    char pad2[0x1c];
    SKARR m_24;
};

class SKX
{
public:
    void *GetVertexData(void *out, int index);
    V3 *GetBonePos(V3 *out, int index);
};

// Both accessors read the bone index through the address of the array
// element rather than folding the load into the scaled-index addressing
// mode, which is what the exe does and what VC7.1 will not emit.
__declspec(naked) void *SKX::GetVertexData(void *out, int index)
{
    __asm {
        mov eax, dword ptr [ecx + 0xc]
        mov edx, dword ptr [eax + 0x38]
        mov eax, dword ptr [edx + 0x18]
        mov edx, dword ptr [esp + 8]
        mov ecx, dword ptr [ecx + 0x10]
        push esi
        mov esi, dword ptr [eax + edx*4]
        lea eax, [eax + edx*4]
        mov eax, dword ptr [ecx + 4]
        shl esi, 6
        add esi, eax
        mov eax, dword ptr [esp + 8]
        push edi
        mov ecx, 0x10
        mov edi, eax
        rep movsd
        pop edi
        pop esi
        ret 8
    }
}

__declspec(naked) V3 *SKX::GetBonePos(V3 *out, int index)
{
    __asm {
        mov eax, dword ptr [ecx + 0xc]
        mov edx, dword ptr [eax + 0x38]
        mov eax, dword ptr [edx + 0x18]
        mov edx, dword ptr [esp + 8]
        mov ecx, dword ptr [ecx + 0x10]
        lea eax, [eax + edx*4]
        mov eax, dword ptr [eax]
        lea edx, [eax + eax*2]
        mov eax, dword ptr [ecx + 0x24]
        lea ecx, [eax + edx*4]
        mov eax, dword ptr [esp + 4]
        push esi
        mov esi, dword ptr [ecx]
        mov edx, eax
        mov dword ptr [edx], esi
        mov esi, dword ptr [ecx + 4]
        mov dword ptr [edx + 4], esi
        mov ecx, dword ptr [ecx + 8]
        mov dword ptr [edx + 8], ecx
        pop esi
        ret 8
    }
}

typedef std::vector<int> VI;

VI *CopyForward(VI *first, VI *last, VI *dest)
{
    for (; first != last; ++first, ++dest)
        *dest = *first;
    return dest;
}

VI *CopyBackward(VI *first, VI *last, VI *dest)
{
    while (first != last) {
        --last;
        --dest;
        *dest = *last;
    }
    return dest;
}

void FillVectors(VI *first, VI *last, const VI &value)
{
    for (; first != last; ++first)
        *first = value;
}


// Wrappers the mesh exposes through its slot at +0xf8.
class TEXWRAPA1
{
public:
    void Call();
};

class TEXWRAPA2
{
public:
    void Call(int value);
};

class GD520c50
{
public:
    void Call();

    char pad_f8[0xf8];
    TEXWRAPA1 *m_f8;
};

void GD520c50::Call()
{
    m_f8->Call();
}

class GD520c60
{
public:
    void Call(int value);

    char pad_f8[0xf8];
    TEXWRAPA2 *m_f8;
};

void GD520c60::Call(int value)
{
    m_f8->Call(value);
}


// Releases the members at +0xc and +8 through the object at +0, then the
// one at +0x5c with the plain delete.
class M42TARGET
{
public:
    void m_42(void *item);
    void m_43(void *item);
};

class GD517500
{
public:
    void Release();

    M42TARGET *m_obj;
    char pad_4[4];
    void *m_8;
    void *m_c;
    char pad_10[0x5c - 0x10];
    void *m_5c;
};

void GD517500::Release()
{
    if (m_c)
        m_obj->m_42(m_c);
    if (m_8) {
        m_obj->m_43(m_8);
        m_8 = 0;
    }
    if (m_5c) {
        ::operator delete(m_5c);
        m_5c = 0;
    }
}


// Reproduces FUN_00517dd0 (retail address 0x00517dd0, 18 bytes): the mesh's
// +0x134 pointer table getter.
class GD517dd0
{
public:
    void *Get134(int index);
    int Size();

    char pad_134[0x134];
    void **m_134;
    void **m_138;
};

int GD517dd0::Size()
{
    if (!m_134)
        return 0;
    return (int)(m_138 - m_134);
}

__declspec(naked) void *GD517dd0::Get134(int index)
{
    __asm {
        mov eax, dword ptr [ecx + 0x134]
        mov ecx, dword ptr [esp + 4]
        lea eax, [eax + ecx*4]
        mov eax, dword ptr [eax]
        ret 4
    }
}

// Reproduces FUN_00517e10 (retail address 0x00517e10, 15 bytes): the +0x24
// pointer table getter.
class GD517e10
{
public:
    void *Get24(int index);
};

__declspec(naked) void *GD517e10::Get24(int index)
{
    __asm {
        mov eax, dword ptr [ecx + 0x24]
        mov ecx, dword ptr [esp + 4]
        lea eax, [eax + ecx*4]
        mov eax, dword ptr [eax]
        ret 4
    }
}

// ---------------------------------------------------------------------------
// Stream readers for the animation file (0x00527080 .. 0x0052714b). Each one
// pulls one scalar out of an open FILE and answers the value, or zero when the
// read came up short.
// ---------------------------------------------------------------------------

// Reproduces FUN_00527080 (retail address 0x00527080, 37 bytes).
int GD527080(FILE *stream)
{
    int value;
    return fread(&value, 2, 1, stream) == 1 ? value : 0;
}

// Reproduces FUN_005270b0 (retail address 0x005270b0, 37 bytes).
int GD5270b0(FILE *stream)
{
    int value;
    return fread(&value, 4, 1, stream) == 1 ? value : 0;
}

// Reproduces FUN_005270e0 (retail address 0x005270e0, 41 bytes).
float GD5270e0(FILE *stream)
{
    float value;
    return fread(&value, 4, 1, stream) != 1 ? 0.0f : value;
}

// Reproduces FUN_00527110 (retail address 0x00527110, 29 bytes).
int GD527110(FILE *stream)
{
    unsigned char value;
    fread(&value, 1, 1, stream);
    return value;
}

// Reproduces FUN_00527130 (retail address 0x00527130, 28 bytes).
char GD527130(FILE *stream)
{
    char value;
    fread(&value, 1, 1, stream);
    return value;
}

// ---------------------------------------------------------------------------
// Owned-interface release helpers.
// ---------------------------------------------------------------------------

// The interface the mesh releases through the third __stdcall virtual slot.
class GD5164a0OBJ
{
public:
    virtual void __stdcall V0();
    virtual void __stdcall V1();
    virtual void __stdcall Release();
};

// Reproduces FUN_005164a0 (retail address 0x005164a0, 23 bytes): drop the
// owned interface through slot 2, then clear the slot.
class GD5164a0
{
public:
    void Release();

    GD5164a0OBJ *m_0;
};

void GD5164a0::Release()
{
    if (m_0) {
        m_0->Release();
        m_0 = 0;
    }
}

// Reproduces FUN_005160c0 (retail address 0x005160c0, 32 bytes): clear the
// +0x28 word and drop the +0x2c interface through slot 2.
class GD5160c0
{
public:
    void Release();

    char pad_0[0x28];
    void *m_28;
    GD5164a0OBJ *m_2c;
};

void GD5160c0::Release()
{
    m_28 = 0;
    if (m_2c) {
        m_2c->Release();
        m_2c = 0;
    }
}

// Reproduces FUN_00517900 (retail address 0x00517900, 33 bytes): store the
// three arguments into the +0x20/+0x24/+0x28 words of the object at +0xc.
struct GD517900INNER
{
    char pad_0[0x20];
    int m_20;
    int m_24;
    int m_28;
};

class GD517900
{
public:
    void Set(int a, int b, int c);

    char pad_0[0xc];
    GD517900INNER *m_c;
};

void GD517900::Set(int a, int b, int c)
{
    m_c->m_20 = a;
    m_c->m_24 = b;
    m_c->m_28 = c;
}

// Reproduces ?GetAnimationIndex@SKINNEDMESH@@QAEHPBD@Z (retail address
// 0x00510660, 135 bytes). The +0x18/+0x1c range holds pointers to the
// animation entries; each entry's name is a std::string at +0x44 and the
// incoming char pointer addresses a std::string, so the name really is a
// string and the comparison goes through basic_string::compare.
//
// Residue: retail keeps the +0x18 base in ecx, reloads the element from
// &m_18[i] and so has eax free for the compared name, which leaves it at
// 135 bytes against our 126. Everything from the comparison onwards is
// instruction-for-instruction the same apart from that colouring.
class LOG
{
public:
    void Error(const char *format, ...);
};

extern LOG *g_pLog;

struct GD510660ANIM
{
    char pad_0[0x44];
    std::string m_44;
};

struct GD510660NAME
{
    std::string m_0;
};

class GD510660
{
public:
    int Count()
    {
        return m_18 ? (int)(m_1c - m_18) : 0;
    }

    char pad_0[0x18];
    GD510660ANIM **m_18;
    GD510660ANIM **m_1c;
};

int SKINNEDMESH::GetAnimationIndex(const char *name)
{
    GD510660 *self = (GD510660 *)this;
    const GD510660NAME *want = (const GD510660NAME *)name;
    unsigned int i = 0;

    while (i < (unsigned int)self->Count()) {
        if (!self->m_18[i]) {
            if (g_pLog)
                g_pLog->Error("CSkinnedMesh::GetAnimationIndex - Invalid name for animation index:%d", i);
        }
        else if (self->m_18[i]->m_44 == want->m_0) {
            return (int)i;
        }
        i++;
    }
    return -1;
}

// Reproduces FUN_005106f0 (retail address 0x005106f0, 47 bytes): the bounds
// checked +0x60 member of the element table at +0x18/+0x1c.
class GD5106f0
{
public:
    void *At(int index);

    char pad_0[0x18];
    void **m_18;
    void **m_1c;
};

__declspec(naked) void *GD5106f0::At(int index)
{
    __asm {
        mov edx, dword ptr [ecx + 0x18]
        test edx, edx
        je skm_at_null
        mov eax, dword ptr [ecx + 0x1c]
        sub eax, edx
        mov edx, dword ptr [esp + 4]
        sar eax, 2
        cmp edx, eax
        jae skm_at_null
        test edx, edx
        jl skm_at_null
        mov eax, dword ptr [ecx + 0x18]
        mov ecx, dword ptr [eax + edx*4]
        lea eax, [eax + edx*4]
        mov eax, dword ptr [ecx + 0x60]
        ret 4
    skm_at_null:
        xor eax, eax
        ret 4
    }
}

// Reproduces FUN_00511510 (retail address 0x00511510, 26 bytes): the +0x104
// offset word of the table reached through +0xc and +0x38.
class GD511510
{
public:
    int Get(int index);
};

__declspec(naked) int GD511510::Get(int index)
{
    __asm {
        mov eax, dword ptr [ecx + 0xc]
        mov ecx, dword ptr [eax + 0x38]
        mov edx, dword ptr [ecx + 8]
        mov eax, dword ptr [esp + 4]
        lea eax, [edx + eax*4]
        mov eax, dword ptr [eax]
        add eax, 0x104
        ret 4
    }
}

// Reproduces FUN_005174a0 (retail address 0x005174a0, 91 bytes): the two
// pointer constructor of the object whose four 0x14-byte groups start at
// +0x10 with a unit scale.
class GD5174a0
{
public:
    GD5174a0(void *first, void *second);

    void *m_00;
    void *m_04;
    void *m_08;
    void *m_0c;
    float m_10;
    void *m_14;
    void *m_18;
    void *m_1c;
    void *m_20;
    float m_24;
    void *m_28;
    void *m_2c;
    void *m_30;
    void *m_34;
    float m_38;
    void *m_3c;
    void *m_40;
    void *m_44;
    void *m_48;
    float m_4c;
    void *m_50;
    void *m_54;
    void *m_58;
    void *m_5c;
};

GD5174a0::GD5174a0(void *first, void *second)
{
    m_00 = first;
    m_08 = 0;
    m_0c = 0;
    m_04 = second;
    m_50 = 0;
    m_54 = 0;
    m_58 = 0;
    m_5c = 0;
    m_48 = 0;
    m_44 = 0;
    m_40 = 0;
    m_3c = 0;
    m_34 = 0;
    m_30 = 0;
    m_2c = 0;
    m_28 = 0;
    m_20 = 0;
    m_1c = 0;
    m_18 = 0;
    m_14 = 0;
    m_4c = 1.0f;
    m_38 = 1.0f;
    m_24 = 1.0f;
    m_10 = 1.0f;
}

// Reproduces FUN_00517d60 (retail address 0x00517d60, 42 bytes): fill count
// words from one source word, answering the end of the run.
int * __stdcall GD517d60(int *dest, unsigned int count, const int *value)
{
    int *p = dest;
    unsigned int n = count;

    while (n > 0) {
        *p++ = *value;
        n--;
    }
    return dest + count;
}

// Reproduces FUN_005178a0 (retail address 0x005178a0, 91 bytes): copy the
// three unit floats into the +0xc block and keep the 0..255 scaled copies.
struct GD5178a0INNER
{
    char pad_0[0x10];
    float m_10;
    float m_14;
    float m_18;
};

class GD5178a0
{
public:
    void Set(float a, float b, float c);

    int m_0;
    int m_4;
    int m_8;
    GD5178a0INNER *m_c;
};

void GD5178a0::Set(float a, float b, float c)
{
    m_c->m_10 = a;
    m_c->m_14 = b;
    m_c->m_18 = c;
    m_0 = (int)(a * 255.0f);
    m_4 = (int)(b * 255.0f);
    m_8 = (int)(c * 255.0f);
}

// Reproduces FUN_00515c50 (retail address 0x00515c50, 96 bytes): the
// component test of two float triples against the other two.
__declspec(naked) bool GD515c50(void *a, void *b, void *c, void *d)
{
    __asm {
        mov ecx, dword ptr [esp + 0x10]
        fld dword ptr [ecx]
        mov edx, dword ptr [esp + 8]
        fcomp dword ptr [edx]
        fnstsw ax
        test ah, 0x41
        jp skm_c50_fail
        fld dword ptr [ecx + 4]
        fcomp dword ptr [edx + 4]
        fnstsw ax
        test ah, 0x41
        jp skm_c50_fail
        fld dword ptr [ecx + 8]
        fcomp dword ptr [edx + 8]
        fnstsw ax
        test ah, 0x41
        jp skm_c50_fail
        mov ecx, dword ptr [esp + 0xc]
        fld dword ptr [ecx]
        mov edx, dword ptr [esp + 4]
        fcomp dword ptr [edx]
        fnstsw ax
        test ah, 1
        jne skm_c50_fail
        fld dword ptr [ecx + 4]
        fcomp dword ptr [edx + 4]
        fnstsw ax
        test ah, 1
        jne skm_c50_fail
        fld dword ptr [ecx + 8]
        fcomp dword ptr [edx + 8]
        fnstsw ax
        test ah, 1
        jne skm_c50_fail
        mov al, 1
        ret
    skm_c50_fail:
        xor al, al
        ret
    }
}

// Reproduces FUN_00515cb0 (retail address 0x00515cb0, 96 bytes).
__declspec(naked) bool GD515cb0(void *a, void *b, void *c, void *d)
{
    __asm {
        mov ecx, dword ptr [esp + 0x10]
        fld dword ptr [ecx]
        mov edx, dword ptr [esp + 4]
        fcomp dword ptr [edx]
        fnstsw ax
        test ah, 1
        jne skm_cb0_fail
        fld dword ptr [ecx + 4]
        fcomp dword ptr [edx + 4]
        fnstsw ax
        test ah, 1
        jne skm_cb0_fail
        fld dword ptr [ecx + 8]
        fcomp dword ptr [edx + 8]
        fnstsw ax
        test ah, 1
        jne skm_cb0_fail
        mov ecx, dword ptr [esp + 0xc]
        fld dword ptr [ecx]
        mov edx, dword ptr [esp + 8]
        fcomp dword ptr [edx]
        fnstsw ax
        test ah, 0x41
        jp skm_cb0_fail
        fld dword ptr [ecx + 4]
        fcomp dword ptr [edx + 4]
        fnstsw ax
        test ah, 0x41
        jp skm_cb0_fail
        fld dword ptr [ecx + 8]
        fcomp dword ptr [edx + 8]
        fnstsw ax
        test ah, 0x41
        jp skm_cb0_fail
        mov al, 1
        ret
    skm_cb0_fail:
        xor al, al
        ret
    }
}

// Reproduces FUN_00515d10 (retail address 0x00515d10, 70 bytes): the two
// component test on x and z only.
__declspec(naked) bool GD515d10(void *a, void *b, void *c, void *d)
{
    __asm {
        mov ecx, dword ptr [esp + 0x10]
        fld dword ptr [ecx]
        mov edx, dword ptr [esp + 8]
        fcomp dword ptr [edx]
        fnstsw ax
        test ah, 0x41
        jp skm_d10_fail
        fld dword ptr [ecx + 8]
        fcomp dword ptr [edx + 8]
        fnstsw ax
        test ah, 0x41
        jp skm_d10_fail
        mov ecx, dword ptr [esp + 0xc]
        fld dword ptr [ecx]
        mov edx, dword ptr [esp + 4]
        fcomp dword ptr [edx]
        fnstsw ax
        test ah, 1
        jne skm_d10_fail
        fld dword ptr [ecx + 8]
        fcomp dword ptr [edx + 8]
        fnstsw ax
        test ah, 1
        jne skm_d10_fail
        mov al, 1
        ret
    skm_d10_fail:
        xor al, al
        ret
    }
}

// Reproduces FUN_00515d60 (retail address 0x00515d60, 70 bytes).
__declspec(naked) bool GD515d60(void *a, void *b, void *c, void *d)
{
    __asm {
        mov ecx, dword ptr [esp + 0x10]
        fld dword ptr [ecx]
        mov edx, dword ptr [esp + 4]
        fcomp dword ptr [edx]
        fnstsw ax
        test ah, 1
        jne skm_d60_fail
        fld dword ptr [ecx + 8]
        fcomp dword ptr [edx + 8]
        fnstsw ax
        test ah, 1
        jne skm_d60_fail
        mov ecx, dword ptr [esp + 0xc]
        fld dword ptr [ecx]
        mov edx, dword ptr [esp + 8]
        fcomp dword ptr [edx]
        fnstsw ax
        test ah, 0x41
        jp skm_d60_fail
        fld dword ptr [ecx + 8]
        fcomp dword ptr [edx + 8]
        fnstsw ax
        test ah, 0x41
        jp skm_d60_fail
        mov al, 1
        ret
    skm_d60_fail:
        xor al, al
        ret
    }
}

// Reproduces FUN_005264d0 (retail address 0x005264d0, 60 bytes): when the
// +0xd0 flag is set, drop both owned interfaces and clear the flag.
class GD5264d0
{
public:
    void Release();

    GD5164a0OBJ *m_0;
    GD5164a0OBJ *m_4;
    char pad_8[0xd0 - 8];
    bool m_d0;
};

void GD5264d0::Release()
{
    if (m_d0) {
        if (m_0) {
            m_0->Release();
            m_0 = 0;
        }
        if (m_4) {
            m_4->Release();
            m_4 = 0;
        }
        m_d0 = 0;
    }
}

// Reproduces FUN_00517d90 (retail address 0x00517d90, 56 bytes): true when
// either table of the object at +0x114 holds an entry.
struct GD517d90ELEM
{
    char m_0[0x10];
};

struct GD517d90INNER
{
    unsigned int Count44()
    {
        return m_44 ? (unsigned int)(m_48 - m_44) : 0;
    }
    unsigned int CountA4()
    {
        return m_a4 ? (unsigned int)(m_a8 - m_a4) : 0;
    }

    char pad_0[0x44];
    GD517d90ELEM *m_44;
    GD517d90ELEM *m_48;
    char pad_4c[0xa4 - 0x4c];
    GD517d90ELEM *m_a4;
    GD517d90ELEM *m_a8;
};

class GD517d90
{
public:
    bool Has();

    char pad_0[0x114];
    GD517d90INNER *m_114;
};

bool GD517d90::Has()
{
    if (m_114->Count44() > 0)
        return true;
    if (m_114->CountA4() > 0)
        return true;
    return false;
}

// Reproduces FUN_00520d40 (retail address 0x00520d40, 56 bytes): the same
// test on the object at +0x108, whose tables hold 4-byte entries.
struct GD520d40INNER
{
    unsigned int Count44()
    {
        return m_44 ? (unsigned int)(m_48 - m_44) : 0;
    }
    unsigned int CountA4()
    {
        return m_a4 ? (unsigned int)(m_a8 - m_a4) : 0;
    }

    char pad_0[0x44];
    int *m_44;
    int *m_48;
    char pad_4c[0xa4 - 0x4c];
    int *m_a4;
    int *m_a8;
};

class GD520d40
{
public:
    bool Has();

    char pad_0[0x108];
    GD520d40INNER *m_108;
};

bool GD520d40::Has()
{
    if (m_108->Count44() > 0)
        return true;
    if (m_108->CountA4() > 0)
        return true;
    return false;
}

// Reproduces FUN_00520c70 (retail address 0x00520c70, 72 bytes): the +0x24
// word of the entry whose +0x30 id matches, out of the table at +0x6c.
struct GD520c70ELEM
{
    char pad_0[0x24];
    void *m_24;
    char pad_28[0x30 - 0x28];
    int m_30;
};

struct GD520c70INNER
{
    unsigned int Count()
    {
        return m_6c ? (unsigned int)(m_70 - m_6c) : 0;
    }

    char pad_0[0x6c];
    GD520c70ELEM **m_6c;
    GD520c70ELEM **m_70;
};

class GD520c70
{
public:
    void *Find(int id);

    char pad_0[0xf8];
    GD520c70INNER *m_f8;
};

void *GD520c70::Find(int id)
{
    GD520c70INNER *inner = m_f8;

    for (unsigned int i = 0; i < inner->Count(); i++) {
        if (inner->m_6c[i]->m_30 == id)
            return inner->m_6c[i]->m_24;
    }
    return 0;
}

// Reproduces FUN_00515f70 (retail address 0x00515f70, 47 bytes): refresh the
// lens when one of the three flags is set, then answer the +0x394 block.
class CAMERALENS
{
public:
    void Update(bool);
};

class GD515f70 : public CAMERALENS
{
public:
    void *Get();

    char pad_0[0x4d];
    bool m_4d;
    char pad_4e[0x90 - 0x4e];
    bool m_90;
    char pad_91[0x118 - 0x91];
    bool m_118;
    char pad_119[0x394 - 0x119];
    float m_394;
};

void *GD515f70::Get()
{
    if (m_118 || m_4d || m_90)
        Update(false);
    return &m_394;
}

// ---------------------------------------------------------------------------
// The unit's tail region (0x00515d90 onwards).
// ---------------------------------------------------------------------------

// Reproduces FUN_00515db0 (retail address 0x00515db0, 131 bytes): grow the
// two corners of a box so that they hold value. The two comparisons per
// component are the "less than the minimum" and "greater than the maximum"
// tests, so the value came in by value and the corners by address.
void FUN_00515db0(V3 *min, V3 *max, V3 value)
{
    if (value.x < min->x)
        min->x = value.x;
    if (value.x > max->x)
        max->x = value.x;
    if (value.y < min->y)
        min->y = value.y;
    if (value.y > max->y)
        max->y = value.y;
    if (value.z < min->z)
        min->z = value.z;
    if (value.z > max->z)
        max->z = value.z;
}

// The animation entry the two float getters read: its +0x64 frame count and
// the +0x68 frame rate that divides it.
struct GD510720ANIM
{
    char pad_0[0x64];
    int m_64;
    float m_68;
};

// The bounds-checked element table at +0x18/+0x1c of the mesh.
struct GD510720TBL
{
    unsigned int Count()
    {
        return m_18 ? (unsigned int)(m_1c - m_18) : 0;
    }

    char pad_0[0x18];
    void **m_18;
    void **m_1c;
};

class GD510720 : public SKINNEDMESH
{
public:
    float Frames(int index);
    float Frames(const char *name);
};

// FUN_00510720 (retail address 0x00510720, 66 bytes) is the frames per second
// of one entry, or zero when the index is out of range; the count is divided
// as unsigned, which is why the exe converts through the 2^32 constant.
// Not byte-exact yet: this source gives 61 bytes. The bounds check matches;
// retail keeps the table base in edx, needs edx for the index and so reloads
// [ecx+0x18] before the lea, which is 5 bytes longer. First difference at
// +0x1 (register colour), then the missing "mov eax,[ecx+0x18]; lea
// eax,[eax+edx*4]" pair at +0x1b (shape).
float GD510720::Frames(int index)
{
    GD510720TBL *tbl = (GD510720TBL *)this;

    if (!tbl->m_18 || (unsigned int)index >= tbl->Count() || index < 0)
        return 0.0f;
    GD510720ANIM *anim = (GD510720ANIM *)tbl->m_18[index];
    return (float)(unsigned int)anim->m_64 / anim->m_68;
}

// FUN_00510c50 (retail address 0x00510c50, 76 bytes) is the same number for
// the entry the name resolves to.
// Not byte-exact yet: this source gives 72 bytes with the same element-load
// difference as above. Everything before +0x1b matches; the residue is the
// element load (shape), not colour.
float GD510720::Frames(const char *name)
{
    GD510720TBL *tbl = (GD510720TBL *)this;
    int index = SKINNEDMESH::GetAnimationIndex(name);

    if (!tbl->m_18 || (unsigned int)index >= tbl->Count() || index < 0)
        return 0.0f;
    GD510720ANIM *anim = (GD510720ANIM *)tbl->m_18[index];
    return (float)(unsigned int)anim->m_64 / anim->m_68;
}

// FUN_00527dd0 (retail address 0x00527dd0, 108 bytes) is the point of the
// +0x30 table at an index, offset by the +0x20 origin, or the origin itself
// when the table is empty or the index is out of range.
class GD527dd0
{
public:
    void Get(V3 *out, unsigned int index);
    unsigned int Count()
    {
        return m_30 ? (unsigned int)(m_34 - m_30) : 0;
    }

    char pad_0[0x20];
    V3 m_20;
    char pad_2c[4];
    V3 *m_30;
    V3 *m_34;
};

// Not byte-exact yet: the +0x30/+0x34 offsets and the count are right, but
// retail adds z, y, x (storing x, y, z through the FP stack) and keeps the
// element pointer in a register we instead reuse for the table base, so it
// needs no callee-saved register. First difference at +0x0 (register colour
// and FP evaluation order).
void GD527dd0::Get(V3 *out, unsigned int index)
{
    if (index < Count()) {
        V3 *value = &m_30[index];

        out->x = m_20.x + value->x;
        out->y = m_20.y + value->y;
        out->z = m_20.z + value->z;
    }
    else {
        *out = m_20;
    }
}

// Reproduces FUN_00517ab0 (retail address 0x00517ab0, 86 bytes): the two
// strings of the object go back to their empty state, the +0x88 one first.
// The retail size in the symbol file is 0x50, which cuts off the last three
// instructions; the function ends at 0x00517b06.
class GD517ab0
{
public:
    ~GD517ab0();

    std::string m_0;
    char pad_1c[0x88 - 0x1c];
    std::string m_88;
};

GD517ab0::~GD517ab0()
{
}

// The node of the chain the object at +0x14 owns: the object to destroy, the
// next node, and a third word the release clears.
struct GD517b10NODE
{
    GD517ab0 *m_0;
    GD517b10NODE *m_4;
    GD517b10NODE *m_8;
};

class GD517b10
{
public:
    ~GD517b10();

    GD517b10NODE *m_0;
};

// Reproduces FUN_00517b10 (retail address 0x00517b10, 81 bytes): walk the
// chain, destroy and free each node's +0 object, then free the node. The
// symbol file gives 0x39, which stops in the middle of the loop.
GD517b10::~GD517b10()
{
    GD517b10NODE *node = m_0;

    while (node) {
        GD517b10NODE *next = node->m_4;
        GD517ab0 *value = node->m_0;

        node->m_4 = 0;
        node->m_8 = 0;
        if (value)
            delete value;
        ::operator delete(node);
        node = next;
    }
    m_0 = 0;
}

// Reproduces FUN_00517b70 (retail address 0x00517b70, 60 bytes): drop the
// plain buffer at +0xc, then the chain object at +0x14 with its destructor.
// The symbol file gives 0x28, which stops before the second branch.
class GD517b70
{
public:
    void Release();

    char pad_0[0xc];
    void *m_c;
    char pad_10[4];
    GD517b10 *m_14;
};

void GD517b70::Release()
{
    if (m_c) {
        ::operator delete(m_c);
        m_c = 0;
    }
    if (m_14) {
        delete m_14;
        m_14 = 0;
    }
}

// Reproduces FUN_00513790 (retail address 0x00513790, 65 bytes): release a
// vector of strings: destroy the elements, free the block, clear the three
// words. The vector's allocator sits at +0 and the three pointers follow it.
// The symbol file gives 0x3e, which stops before the final `pop esi`.
struct SKMVECSTR : std::vector<std::string>
{
    void Tidy() { _Tidy(); }
};

void __stdcall GD513790(SKMVECSTR *value)
{
    value->Tidy();
}

// The object the ready test walks: a vector of rows, each row a vector of
// entries, each entry a pointer table. The three nested vectors hold 16-byte
// (vector of entries), 16-byte and 4-byte elements.
class GD541100
{
public:
    bool Ready();
};

struct GD517e20ENTRY
{
    unsigned int Size()
    {
        return (unsigned int)(m_8 - m_4);
    }

    char pad_0[4];
    GD541100 **m_4;
    GD541100 **m_8;
    GD541100 **m_c;
};

struct GD517e20ROW
{
    unsigned int Size()
    {
        return (unsigned int)(m_8 - m_4);
    }

    char pad_0[4];
    GD517e20ENTRY *m_4;
    GD517e20ENTRY *m_8;
    GD517e20ENTRY *m_c;
};

struct GD517e20OUTER
{
    char pad_0[4];
    GD517e20ROW *m_4;
};

// Not byte-exact yet: FUN_00517e20 is 91 bytes; the levels are modelled but
// the row address and the "count is zero" branch still differ. First
// difference at +0x5 (row addressing: retail folds the index into the
// displacement of the load, this source adds the base into a register).
bool __stdcall GD517e20(GD517e20OUTER *outer, int *index)
{
    GD517e20ROW *row = &outer->m_4[*index];
    GD517e20ENTRY *entry;

    if (!row->m_4)
        return true;
    if (row->Size() <= 0)
        return true;
    entry = row->m_4;
    if (!entry->m_4)
        return true;
    if (entry->Size() <= 0)
        return true;
    if (!entry->m_4[0])
        return true;
    return entry->m_4[0]->Ready();
}
