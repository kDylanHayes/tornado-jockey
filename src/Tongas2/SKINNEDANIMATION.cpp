// Tongas2/SKINNEDANIMATION -- the skinned-animation loader and accessors.
//
// Discovery scaffold: bodies are empty until the unit is matched; each
// function has its exact target size in config/symbols.txt so a later delink
// cannot cut the target short. Load's extent includes its exception handler
// and the data that follows it.
#include "Tongas2/SKINNEDANIMATION.h"

int SKINNEDANIMATION::Load(const char *name)
{
    (void)name;
    return 0;
}

int SKINNEDANIMATION::GetBoneCount()
{
    return 0;
}

int SKINNEDANIMATION::GetFrameCount()
{
    return 0;
}
