// Tongas2/TEXTUREDX8SD -- the DX8 shadowed textures.
//
// The manager destructor, master bookkeeping and texture getters are matched;
// the texture builders and the copy class are still scaffolds.
#include <windows.h>

#include "Tongas2/TEXTUREDX8SD.h"

extern RESOURCES *g_pResources;

int TEXTUREDX8SDMASTER::Build()
{
    if (m_60) {
        delete m_60;
        m_60 = 0;
    }

    int mode = 0;
    if (!*(bool *)((char *)this + 0x68)) {
        mode = (*(bool *)((char *)this + 0x480) != 0) + 2;
    } else if (*(bool *)((char *)this + 0x480)) {
        mode = 1;
    }

    switch (mode) {
    case 0:
        BuildDiffuseColorTexture();
        return 1;
    case 1:
        if (BuildDiffuseColorTexture())
            MergeAlpha();
        return 1;
    case 2:
        Load(true);
        return 1;
    case 3:
        if (Load(false))
            MergeAlpha();
        return 1;
    }
    return 1;
}

bool TEXTUREDX8SDMASTER::Load(bool primary)
{
    (void)primary;
    return false;
}

void TEXTUREDX8SDMASTER::MergeAlpha()
{
}

void TEXTUREDX8SDMANAGER::UpdateTextures()
{
    TEXTUREDX8SDMASTER *master = (TEXTUREDX8SDMASTER *)m_first;
    while (master) {
        bool build = *(bool *)((char *)master + 0x488);
        TEXTUREDX8SDMASTER *next = (TEXTUREDX8SDMASTER *)master->m_next;
        if (build)
            master->Build();
        master = next;
    }
}

RESHANDLE TEXTUREDX8SDMANAGER::GetHandle(const char *name, const char *suffix)
{
    const char *sfx = suffix;
    char buffer[0x200];
    if (sfx) {
        buffer[0] = 0;
        buffer[0x1ff] = 0;
        SafeSprintf(buffer, ";%s;%s", name, sfx);
        name = buffer;
    }
    return RESOURCEMANAGER::GetHandle(name);
}

RESHANDLE TEXTUREDX8SDMANAGER::GetHandleRGB(int a, int b, const int *rgb, const char *suffix)
{
    char buffer[0x200];
    buffer[0] = 0;
    buffer[0x1ff] = 0;
    if (!suffix)
        SafeSprintf(buffer, "RGB:%d %d %d %d %d", a, b, rgb[0], rgb[1], rgb[2]);
    else
        SafeSprintf(buffer, ";RGB:%d %d %d %d %d;%s", a, b, rgb[0], rgb[1], rgb[2], suffix);
    return RESOURCEMANAGER::GetHandle(buffer);
}

TEXTUREDX8SDMASTER *__stdcall GetTextureResource(const RESHANDLE *handle)
{
    TEXTUREDX8SDMASTER *master = (TEXTUREDX8SDMASTER *)g_pResources->GetMaster(*handle);
    if (master && ++master->m_64 > 0 && master->m_60 == 0)
        master->Build();
    return master;
}

void __stdcall ReleaseTexture(const RESHANDLE *handle)
{
    TEXTUREDX8SDMASTER *master = (TEXTUREDX8SDMASTER *)g_pResources->GetMaster(*handle);
    if (master)
        master->m_64--;
}

extern "C" void *PTR_GlobalMemoryStatus_00656130;

__declspec(naked) unsigned int GetPhysicalMemoryMB()
{
    __asm {
        sub esp, 0x20
        push edi
        mov eax, 0x20
        lea edi, [esp + 4]
        mov [esp + 4], eax
        mov eax, edi
        push eax
        xor ecx, ecx
        call dword ptr [PTR_GlobalMemoryStatus_00656130]
        mov eax, [esp + 0xc]
        shr eax, 0x14
        pop edi
        add esp, 0x20
        ret
    }
}

TEXTUREDX8SDMASTER::TEXTUREDX8SDMASTER(RESOURCEMANAGER *manager, const char *name)
    : RESOURCEMASTER(manager, name)
{
}

TEXTUREDX8SDMASTER::~TEXTUREDX8SDMASTER()
{
    if (m_60) {
        delete m_60;
        m_60 = 0;
    }
}

TEXTUREDX8SDMANAGER::~TEXTUREDX8SDMANAGER()
{
}

void *TEXTUREDX8SDMASTER::GetTexture()
{
    return m_60 ? m_60->m_texture : 0;
}

void *TEXTUREDX8SDMASTER::GetSurface()
{
    return m_60 ? m_60->GetSurface() : 0;
}

bool TEXTUREDX8SDMASTER::BuildDiffuseColorTexture()
{
    return false;
}

RESOURCEMASTER *TEXTUREDX8SDMANAGER::CreateMaster(const char *name)
{
    return new TEXTUREDX8SDMASTER(this, name);
}
