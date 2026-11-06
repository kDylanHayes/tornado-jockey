// Tongas2/ANIMATEDGEOMETRY -- the animated-geometry loader and accessors.
//
// Discovery scaffold: bodies are empty until the unit is matched; each
// function has its exact target size in config/symbols.txt so a later delink
// cannot cut the target short. Load's extent includes its exception handler
// and the data that follows it, up to the next function at 0x0052f8c0.
#include "Tongas2/ANIMATEDGEOMETRY.h"

int ANIMATEDGEOMETRY::Load(void *file, int flags, void *owner)
{
    (void)file;
    (void)flags;
    (void)owner;
    return 0;
}

int ANIMATEDGEOMETRY::GetBoneCount()
{
    return 0;
}

int ANIMATEDGEOMETRY::GetFrameCount()
{
    return 0;
}

int ANIMATEDGEOMETRY::GetVertexCount()
{
    return 0;
}
