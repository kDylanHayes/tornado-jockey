// The DX8 renderer plugin's screen decals: SCREENDECALDX8MASTER derives from
// the engine's SCREENDECALMASTER and loads a picture into a Direct3D
// texture. Discovery scaffold: the class name comes from the RegisterClassName
// string at 0x004f2420 and the manager factory at 0x004f2a80; base classes,
// member layouts and the rest of the methods are still unknown.
#ifndef TONGAS2_SCREENDECALDX8_H
#define TONGAS2_SCREENDECALDX8_H

#include "Tongas2/SAFEINDEX.h"

class RESOURCEMANAGER;

class SCREENDECALDX8MASTER
{
public:
    static void RegisterCommands();
};

RESOURCEMANAGER *CreateSCREENDECALDX8();

#endif
