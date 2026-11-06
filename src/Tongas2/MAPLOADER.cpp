// Tongas2/MAPLOADER -- the map-list object at 0x004f8350.
//
// Discovery scaffold: the two functions whose names come from what the
// decompiler shows (a map list and its per-frame update). Bodies are empty
// until the unit is matched; each function has its exact target size in
// config/symbols.txt so a later delink cannot cut the target short.
#include "Tongas2/MAPLOADER.h"

#include <stdexcept>

// The out-of-line std::vector length check the map list's vector instantiation
// leaves in this object.
void VectorTooLong()
{
    throw std::length_error("vector<T> too long");
}

void MAPLOADER::LoadMap(const char *name)
{
    (void)name;
}

void MAPLOADER::Update(float dt)
{
    (void)dt;
}
