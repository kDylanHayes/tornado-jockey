// Tongas2/RENDERSHADER -- DX8 renderer plugin shaders.

#include "Tongas2/RENDERSHADER.h"

struct RESOURCES;
extern "C" RESOURCES* g_pResources;

extern "C" void* get_master_resources(RESOURCES*, int, int, int) { return 0; }
extern "C" void helper9e70() {}

__declspec(naked) RENDERSHADER *RENDERSHADERMANAGER::Get(RESHANDLE handle) {
    (void)handle;
    __asm {
        mov ecx, [esp+4]
        mov edx, [esp+8]
        sub esp, 0x0c
        mov eax, esp
        mov [eax], ecx
        mov ecx, [esp+0x18]
        mov [eax+4], edx
        mov [eax+8], ecx
        mov ecx, g_pResources
        call get_master_resources
        test eax, eax
        je skip
        mov ecx, eax
        call helper9e70
    skip:
        ret 0x0c
    }
}

unsigned int RENDERSHADER::GetWidth()
{
    return 0;
}

unsigned int RENDERSHADER::GetHeight()
{
    return 0;
}

bool RENDERSHADERMASTER::HasAlpha()
{
    return false;
}
