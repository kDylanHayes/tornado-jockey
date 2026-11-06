// Tongas2/INTRO -- the intro screen object at 0x004f9280.
//
// Discovery scaffold: the first function, which builds the intro screen and
// calls MAPLOADER's map list. The body is empty until the unit is matched;
// it has its exact target size in config/symbols.txt so a later delink cannot
// cut the target short.
#include "Tongas2/INTRO.h"

void INTRO::Load(const char *name, const char *path)
{
    (void)name;
    (void)path;
}
