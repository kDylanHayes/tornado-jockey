#include <deque>

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

struct G2AGTGET
{
    void * Get10();
    void * Get4();
};

void * G2AGTGET::Get10()
{
    return (char *)*(void **)((char *)this + 0xc) + 0x10;
}

void * G2AGTGET::Get4()
{
    return (char *)*(void **)((char *)this + 0xc) + 4;
}

struct G2TFGET
{
    void *Get5c(int index);
};

struct G2AGTTHUNK
{
    void *Get(int index);
};

void *G2AGTTHUNK::Get(int index)
{
    return ((G2TFGET *)*(void **)((char *)this + 0xc))->Get5c(index);
}

class G2AGTDEQ : public std::deque<unsigned short>
{
public:
    void Tidy();
};

void G2AGTDEQ::Tidy()
{
    _Tidy();
}
