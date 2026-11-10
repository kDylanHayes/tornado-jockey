// The DX8 shadowed scene-model resources: SCENEMODELDX8SDMASTER, from
// SCENEMODELDX8SD.txt.
//
// Discovery scaffold. The class name comes from the RegisterClassName string
// at 0x00536cc0 and the manager string at 0x00536d10. The object also carries
// the scene-model rendering helpers through 0x00538cef. Bodies are empty
// until the unit is matched; each function's exact target size is in
// config/symbols.txt.
#ifndef TONGAS2_SCENEMODELDX8SD_H
#define TONGAS2_SCENEMODELDX8SD_H

#include "Tongas2/CMDBASE.h"

class RESOURCEMANAGER;

class SCENEMODELDX8SDMASTER
{
public:
    static void RegisterCommands();

    static CMDENTRY s_commands[];
};

RESOURCEMANAGER *CreateSCENEMODELDX8SD();

#endif
