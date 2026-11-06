// The renderer plugin's render textures: RENDERTEXTUREMASTER, from
// RENDERTEXTURE.txt, and the copies it hands out.
//
// Discovery scaffold. The class name comes from the RegisterClassName string
// at 0x004ed980 and the manager string at 0x004edcf0. The base classes and
// member layouts are deliberately left out for now: keeping the scaffold
// free of the engine's inline virtuals means the object defines only the
// functions whose exact target sizes are recorded in config/symbols.txt, so
// a later delink cannot cut a target function short.
#ifndef TONGAS2_RENDERTEXTURE_H
#define TONGAS2_RENDERTEXTURE_H

#include "Tongas2/CMDBASE.h"
#include "Tongas2/CSTRING.h"
#include "Tongas2/RESOURCES.h"
#include "Tongas2/SAFEINDEX.h"

class RESOURCE;

class RENDERTEXTUREMASTER : public RESOURCEMASTER
{
public:
    __declspec(noinline) RENDERTEXTUREMASTER(RESOURCEMANAGER *manager, const char *name);
    virtual ~RENDERTEXTUREMASTER();

    static void RegisterCommands();

    static int s_classId;
    static CMDENTRY s_commands[];

    // Member offsets from the original constructor; the two CSTRINGs at
    // 0x68 and 0x6c are what its destructor frees.
    bool m_60;
    bool m_61;
    char m_62[2];
    int m_64;
    CSTRING m_68;
    CSTRING m_6c;
    int m_70;
    int m_74;
    int m_78;
    int m_7c;
    int m_80;
    int m_84;
    bool m_88;
    bool m_89;
    char m_8a[2];
};

class RENDERTEXTURE
{
public:
    RENDERTEXTURE(RENDERTEXTUREMASTER *master, const RESHANDLE &handle);
};

RESOURCEMANAGER *CreateRENDERTEXTURE();

#endif
