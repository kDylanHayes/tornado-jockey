// Tongas2/MODELDX8SD -- the DX8 shadowed model resources.
//
// Discovery scaffold: class registration, the manager's type code and the
// manager factory. Bodies are empty until the unit is matched; each function
// has its exact target size in config/symbols.txt so a later delink cannot
// cut the target short.
#include "Tongas2/MODELDX8SD.h"

void MODELDX8SDMASTER::RegisterCommands()
{
}

const char *MODELDX8SDMANAGER::GetTypeCode()
{
    return "";
}

RESOURCEMANAGER *CreateMODELDX8SD()
{
    return 0;
}
