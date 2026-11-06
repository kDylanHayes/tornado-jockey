// Tongas2/LIGHTDX8 -- the DX8 renderer plugin's lights.
//
// Discovery scaffold: the names come from the LIGHT.txt manager string, the
// LIGHTDX8RESOURCE log string and the LIGHT / LIGHTMASTER constructor calls
// in the decompiler. The bodies are empty until the unit is matched; every
// function here has its exact target size in config/symbols.txt so a later
// delink cannot cut the target short.
#include "Tongas2/LIGHTDX8.h"

int PackColor(const V3 &color)
{
    (void)color;
    return 0;
}

void UnpackColor(V3 *out, const int *color)
{
    (void)out;
    (void)color;
}

LIGHTDX8RESOURCE::LIGHTDX8RESOURCE(LIGHTMASTER *master, const RESHANDLE &handle)
{
    (void)master;
    (void)handle;
}

void LIGHTDX8RESOURCE::Start()
{
}

void LIGHTDX8RESOURCE::Update(bool force)
{
    (void)force;
}

void LIGHTDX8RESOURCE::Stop()
{
}

LIGHTDX8MANAGER::LIGHTDX8MANAGER()
{
}

LIGHTDX8MANAGER::~LIGHTDX8MANAGER()
{
}

RESOURCEMANAGER *CreateLIGHTDX8()
{
    return 0;
}
