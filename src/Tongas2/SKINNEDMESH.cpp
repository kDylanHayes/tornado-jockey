// Tongas2/SKINNEDMESH -- the CSkinnedMesh object and its helpers.
//
// Discovery scaffold: bodies are empty until the unit is matched; each
// function has its exact target size in config/symbols.txt so a later delink
// cannot cut the target short. The sizes of RemoveAnimations and
// LoadExtraAnimations include their exception-handler tails.
#include "Tongas2/SKINNEDMESH.h"

int SKINNEDMESH::GetAnimationIndex(const char *name)
{
    (void)name;
    return 0;
}

const char *SKINNEDMESH::GetAnimationName(unsigned int index)
{
    (void)index;
    return 0;
}

int SKINNEDMESH::RemoveAnimations(const char *name)
{
    (void)name;
    return 0;
}

int SKINNEDMESH::PlayAnimation(int index)
{
    (void)index;
    return 0;
}

int SKINNEDMESH::BlendAnimation(int index)
{
    (void)index;
    return 0;
}

int SKINNEDMESH::LoadExtraAnimations(const char *name)
{
    (void)name;
    return 0;
}

class GD515e70
{
public:
    virtual ~GD515e70();
};

__declspec(noinline) GD515e70::~GD515e70()
{
}

struct G2SKMGET
{
    void * Get14(int index);
    void * Get4(int index);
};

void * G2SKMGET::Get14(int index)
{
    void **table = *(void ***)((char *)this + 0x14);
    table += index;
    return *table;
}

void * G2SKMGET::Get4(int index)
{
    void **table = *(void ***)((char *)this + 4);
    table += index;
    return *table;
}

struct G2SKMLIST
{
    void Add(void *item);
};

struct G2SKMADD
{
    void *Add(void *item);
};

void *G2SKMADD::Add(void *item)
{
    ((G2SKMLIST *)*(void **)((char *)this + 0x14))->Add(item);
    return item;
}

struct G2SKMCLEAR
{
    void Clear();
};

struct G2SKMTHUNK
{
    void Clear();
};

void G2SKMTHUNK::Clear()
{
    ((G2SKMCLEAR *)*(void **)((char *)this + 0x14))->Clear();
}
