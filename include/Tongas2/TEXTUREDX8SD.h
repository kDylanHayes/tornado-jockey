// The DX8 shadowed textures: TEXTUREDX8SDMASTER, its copies and the manager.
//
// Class names come from the "TEXTUREDX8SDMASTER::..." error strings at
// 0x00504210 and the manager's type code. The master and copy layouts are
// still being recovered.
#ifndef TONGAS2_TEXTUREDX8SD_H
#define TONGAS2_TEXTUREDX8SD_H

#include "Tongas2/CMDBASE.h"
#include "Tongas2/RESOURCE.h"
#include "Tongas2/RESOURCES.h"

// The Direct3D texture object owned by a master (destructor at 0x00534df0).
class TEXTUREDX8SDTEXTURE
{
public:
    TEXTUREDX8SDTEXTURE(bool scratch);
    ~TEXTUREDX8SDTEXTURE();

    int Create(void *texture);
    void *GetSurface();

    void *m_texture;
    char m_04[0x34];
    bool m_38;
    char m_39[0x54 - 0x39];
};

class TEXTUREDX8SDMASTER : public RESOURCEMASTER
{
public:
    __declspec(noinline) TEXTUREDX8SDMASTER(RESOURCEMANAGER *manager, const char *name);
    virtual ~TEXTUREDX8SDMASTER();

    bool BuildDiffuseColorTexture();
    __declspec(noinline) int Build();
    bool Load(bool primary);
    void MergeAlpha();
    void *GetTexture();
    void *GetSurface();

    TEXTUREDX8SDTEXTURE *m_60;
    int m_64;
    // The original object is 0x68c bytes; the remaining fields are unnamed.
    char m_68[0x68c - 0x68];
};

class TEXTUREDX8SDMANAGER : public RESOURCEMANAGER
{
public:
    void UpdateTextures();
    RESHANDLE GetHandle(const char *name, const char *suffix);
    RESHANDLE GetHandleRGB(int a, int b, const int *rgb, const char *suffix);

    virtual ~TEXTUREDX8SDMANAGER();
    virtual RESOURCEMASTER *CreateMaster(const char *name);
};

TEXTUREDX8SDMASTER *__stdcall GetTextureResource(const RESHANDLE *handle);
void __stdcall ReleaseTexture(const RESHANDLE *handle);
unsigned int GetPhysicalMemoryMB();

#endif
