#include "Tongas2/MESHDX8SD.h"

RESOURCEMANAGER *CreateMESHDX8SD()
{
    return new MESHDX8SDMANAGER;
}

const char *MESHDX8SDMANAGER::GetTypeCode()
{
    return "MESH";
}
