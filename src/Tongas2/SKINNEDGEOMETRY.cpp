// Tongas2/SKINNEDGEOMETRY -- the skinned-geometry loader and accessors.
//
// Discovery scaffold: bodies are empty until the unit is matched; each
// function has its exact target size in config/symbols.txt so a later delink
// cannot cut the target short. Load's extent includes its exception handler
// and the data that follows it, up to the next function at 0x005103d0.
//
// Everything outside Load is matched byte for byte now: the two accessors,
// the two forwarding helpers and the five STL element loops the retail unit
// instantiated for its vertex types.
#include "Tongas2/SKINNEDGEOMETRY.h"
#include <algorithm>
#include <memory>

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

template void std::fill<GD510430 *, GD510430>(
    GD510430 *, GD510430 *, const GD510430 &);
template GD510430 *std::copy(GD510430 *, GD510430 *, GD510430 *);
template GD5104e0 *std::copy(GD5104e0 *, GD5104e0 *, GD5104e0 *);
template int *std::copy<int *, int *>(int *, int *, int *);
template GD510630 *std::uninitialized_copy(
    GD510630 *, GD510630 *, GD510630 *);
