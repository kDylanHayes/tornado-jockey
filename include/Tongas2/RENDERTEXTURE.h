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

#include "Tongas2/SAFEINDEX.h"

class RESOURCE;
class RESOURCEMANAGER;
class RESOURCEMASTER;

class RENDERTEXTUREMASTER
{
public:
    RENDERTEXTUREMASTER(RESOURCEMANAGER *manager, const char *name);
    ~RENDERTEXTUREMASTER();

    static void RegisterCommands();
};

class RENDERTEXTURE
{
public:
    RENDERTEXTURE(RENDERTEXTUREMASTER *master, const RESHANDLE &handle);
};

class RENDERTEXTUREMANAGER
{
public:
    RESOURCEMASTER *CreateMaster(const char *name);
};

RESOURCEMANAGER *CreateRENDERTEXTURE();

#endif
