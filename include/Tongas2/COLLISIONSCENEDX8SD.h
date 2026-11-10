// The DX8 shadowed collision-scene resources: COLLISIONSCENEDX8SDMASTER,
// from COLLISIONSCENEDX8SD.txt.
//
// Discovery scaffold. The class name comes from the RegisterClassName string
// at 0x005366c0 and the manager string at 0x00536710. Bodies are empty until
// the unit is matched; each function's exact target size is in
// config/symbols.txt.
#ifndef TONGAS2_COLLISIONSCENEDX8SD_H
#define TONGAS2_COLLISIONSCENEDX8SD_H

#include "Tongas2/CMDBASE.h"

class RESOURCEMANAGER;

class COLLISIONSCENEDX8SDMASTER
{
public:
    static void RegisterCommands();

    static CMDENTRY s_commands[];
};

RESOURCEMANAGER *CreateCOLLISIONSCENEDX8SD();

#endif
