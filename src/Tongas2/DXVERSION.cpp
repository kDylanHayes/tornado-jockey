// Tongas2/DXVERSION -- the DirectX version detector.
//
// Discovery scaffold: bodies are empty until the unit is matched; each
// function has its exact target size in config/symbols.txt so a later delink
// cannot cut the target short. GetDirectXVersion's extent includes its jump
// tables and exception handler, up to the next function at 0x00539660.
#include "Tongas2/DXVERSION.h"

int GetDirectXVersion()
{
    return 0;
}

const char *FormatVersion(int major, int minor, char suffix)
{
    (void)major;
    (void)minor;
    (void)suffix;
    return 0;
}
