// Tongas2/SKINNEDGEOMETRY -- the skinned-geometry loader and accessors.
//
// Discovery scaffold: bodies are empty until the unit is matched; each
// function has its exact target size in config/symbols.txt so a later delink
// cannot cut the target short. Load's extent includes its exception handler
// and the data that follows it, up to the next function at 0x005103d0.
#include "Tongas2/SKINNEDGEOMETRY.h"

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
