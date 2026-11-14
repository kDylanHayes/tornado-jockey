// Tongas2/CANIMATEDMESH -- the animated-mesh object.
//
// Discovery scaffold: bodies are empty until the unit is matched; each
// function has its exact target size in config/symbols.txt so a later delink
// cannot cut the target short. LoadAnimations' size includes its exception
// handler and the data that follows it, up to the next function.
#include "Tongas2/CANIMATEDMESH.h"
#include "Tongas2/MATHS.h"
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

// ---------------------------------------------------------------------------
// The track wrappers.
//
// The three GetPoint wrappers at 0x00531df0, 0x00531e20 and 0x00531e50 all
// read a pointer array at +4 of their object, pick one element with the
// caller's index and forward the call to that element's own GetPoint.

// The 16-byte payload the track objects write into the caller's buffer.
struct GD4F
{
    float x, y, z, w;
};

// The track object whose GetPoint lives at 0x00531c90.
class GD531c90
{
public:
    void GetPoint(GD4F *out, int i, int j, float scale);
};

// The track object at 0x0052a140, in another unit.
class GD52a140
{
public:
    void GetPoint(GD4F *out, int i, int j, float scale);
};

// The engine's animation track, in another unit.
class ANIMTRACK
{
public:
    void GetPoint(V3 *out, int i, int j, float scale);
};

// The holder of the track pointers; the wrapper at 0x00531df0.
class GD531df0
{
public:
    void *m_0;
    GD531c90 **m_4;

    GD4F *GetPoint(GD4F *out, int index, int i, int j, float scale);
};

// The holder of the track pointers; the wrapper at 0x00531e20.
class GD531e20
{
public:
    void *m_0;
    GD52a140 **m_4;

    GD4F *GetPoint(GD4F *out, int index, int i, int j, float scale);
};

// The holder of the track pointers; the wrapper at 0x00531e50.
class GD531e50
{
public:
    void *m_0;
    ANIMTRACK **m_4;

    V3 *GetPoint(V3 *out, int index, int i, int j, float scale);
};

GD4F *GD531df0::GetPoint(GD4F *out, int index, int i, int j, float scale)
{
    GD531c90 *track = m_4[index];
    track->GetPoint(out, i, j, scale);
    return out;
}

GD4F *GD531e20::GetPoint(GD4F *out, int index, int i, int j, float scale)
{
    m_4[index]->GetPoint(out, i, j, scale);
    return out;
}

V3 *GD531e50::GetPoint(V3 *out, int index, int i, int j, float scale)
{
    m_4[index]->GetPoint(out, i, j, scale);
    return out;
}
