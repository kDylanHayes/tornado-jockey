// Tongas2/RENDERTEXTURE -- the DX8 renderer plugin's render textures.
//
// Discovery scaffold: class registration, manager factory and the
// constructor/destructor pair the strings and decompiler give. The bodies
// are empty until the unit is matched; every function here has its exact
// target size in config/symbols.txt so a later delink cannot cut the target
// short.
#include "Tongas2/RENDERTEXTURE.h"

class RENDERTEXTUREMANAGER : public RESOURCEMANAGER
{
public:
    RENDERTEXTUREMANAGER()
    {
        SafeSprintf(m_fileName, "RENDERTEXTURE.txt");
        m_8 = false;
        m_18 = true;
    }

    virtual RESOURCEMASTER *CreateMaster(const char *name) { return new RENDERTEXTUREMASTER(this, name); }
};

int RENDERTEXTUREMASTER::s_classId = -1;

CMDENTRY RENDERTEXTUREMASTER::s_commands[] = {
    { 0, 0, 0, 0, 0 },
};

void RENDERTEXTUREMASTER::RegisterCommands()
{
    g_classParents[s_classId] = RESOURCEMASTER::s_classId;
    g_classCommands[s_classId] = s_commands;
    HashCommands(s_commands);
    RegisterClassName(s_commands, "RENDERTEXTUREMASTER", "RESOURCEMASTER");
}

RESOURCEMANAGER *CreateRENDERTEXTURE()
{
    return new RENDERTEXTUREMANAGER;
}

RENDERTEXTUREMASTER::RENDERTEXTUREMASTER(RESOURCEMANAGER *manager, const char *name)
    : RESOURCEMASTER(manager, name)
{
}

RENDERTEXTUREMASTER::~RENDERTEXTUREMASTER()
{
}

RENDERTEXTURE::RENDERTEXTURE(RENDERTEXTUREMASTER *master, const RESHANDLE &handle)
{
    (void)master;
    (void)handle;
}
