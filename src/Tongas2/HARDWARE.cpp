// Tongas2/HARDWARE -- hardware detection and the D3D capability cache.
//
// Discovery scaffold: bodies are empty until the unit is matched; each
// function has its exact target size in config/symbols.txt so a later delink
// cannot cut the target short. 0x00504dd0 and 0x00504ff0 are followed by
// embedded switch data, so they keep Ghidra's function size.
#include "Tongas2/HARDWARE.h"

int HardwareA()
{
    return 0;
}

int HardwareC()
{
    return 0;
}

int HardwareD()
{
    return 0;
}

HARDWARE::HARDWARE()
{
}

void HARDWARE::Detect()
{
}
