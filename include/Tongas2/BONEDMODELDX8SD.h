// The DX8 renderer plugin's skinned boned models: BONEDMODELDX8SDMASTER and
// BONEDMODELDX8SDRESOURCE derive from the engine's BONEDMODEL classes and
// draw through the DX8 skinned mesh. Discovery scaffold: the class names come
// from the RegisterClassName strings at 0x004f4130 and 0x004f4170 and the
// manager factory at 0x004f4ca0; base classes, member layouts and the rest of
// the methods are still unknown.
#ifndef TONGAS2_BONEDMODELDX8SD_H
#define TONGAS2_BONEDMODELDX8SD_H

#include "Tongas2/SAFEINDEX.h"

class RESOURCEMANAGER;

class BONEDMODELDX8SDMASTER
{
public:
    static void RegisterCommands();
};

class BONEDMODELDX8SDRESOURCE
{
public:
    static void RegisterCommands();
};

RESOURCEMANAGER *CreateBONEDMODELDX8SD();

#endif
