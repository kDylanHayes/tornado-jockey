// Tongas2/SKINNEDGEOMETRY -- the skinned-geometry loader and accessors.
//
// Discovery scaffold: bodies are empty until the unit is matched; each
// function has its exact target size in config/symbols.txt so a later delink
// cannot cut the target short. Load's extent includes its exception handler
// and the data that follows it, up to the next function at 0x005103d0.
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
