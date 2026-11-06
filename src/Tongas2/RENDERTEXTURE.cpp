// Tongas2/RENDERTEXTURE -- the DX8 renderer plugin's render textures.
//
// Discovery scaffold: class registration, manager factory and the
// constructor/destructor pair the strings and decompiler give. The bodies
// are empty until the unit is matched; every function here has its exact
// target size in config/symbols.txt so a later delink cannot cut the target
// short.
#include "Tongas2/RENDERTEXTURE.h"

void RENDERTEXTUREMASTER::RegisterCommands()
{
}

RESOURCEMANAGER *CreateRENDERTEXTURE()
{
    return 0;
}

RENDERTEXTUREMASTER::RENDERTEXTUREMASTER(RESOURCEMANAGER *manager, const char *name)
{
    (void)manager;
    (void)name;
}

RENDERTEXTUREMASTER::~RENDERTEXTUREMASTER()
{
}

RENDERTEXTURE::RENDERTEXTURE(RENDERTEXTUREMASTER *master, const RESHANDLE &handle)
{
    (void)master;
    (void)handle;
}

RESOURCEMASTER *RENDERTEXTUREMANAGER::CreateMaster(const char *name)
{
    (void)name;
    return 0;
}
