// Tongas2/CANIMATEDMESH -- the animated-mesh object.
//
// Discovery scaffold: bodies are empty until the unit is matched; each
// function has its exact target size in config/symbols.txt so a later delink
// cannot cut the target short. LoadAnimations' size includes its exception
// handler and the data that follows it, up to the next function.
#include "Tongas2/CANIMATEDMESH.h"
#include <vector>

int CANIMATEDMESH::LoadAnimations(const char *name)
{
    (void)name;
    return 0;
}

int CANIMATEDMESH::Load(const char *name)
{
    (void)name;
    return 0;
}

typedef std::vector<int> VI;

VI *CopyForward(VI *first, VI *last, VI *dest)
{
    for (; first != last; ++first, ++dest)
        *dest = *first;
    return dest;
}

VI *CopyBackward(VI *first, VI *last, VI *dest)
{
    while (first != last) {
        --last;
        --dest;
        *dest = *last;
    }
    return dest;
}

void FillVectors(VI *first, VI *last, const VI &value)
{
    for (; first != last; ++first)
        *first = value;
}
