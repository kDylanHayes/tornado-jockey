// Tongas2/SKINNEDGEOMETRY -- the skinned-geometry loader and accessors.
//
// Discovery scaffold: bodies are empty until the unit is matched; each
// function has its exact target size in config/symbols.txt so a later delink
// cannot cut the target short. Load's extent includes its exception handler
// and the data that follows it, up to the next function at 0x005103d0.
#include "Tongas2/SKINNEDGEOMETRY.h"

int SKINNEDGEOMETRY::Load(void *file, int flags, void *owner)
{
    (void)file;
    (void)flags;
    (void)owner;
    return 0;
}

int SKINNEDGEOMETRY::GetVertexData()
{
    return 0;
}

int SKINNEDGEOMETRY::GetVertexCount()
{
    return 0;
}
