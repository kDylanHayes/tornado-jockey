#include <stdlib.h>
// Tongas2/SKINNEDMESH -- the CSkinnedMesh object and its helpers.
//
// Discovery scaffold: bodies are empty until the unit is matched; each
// function has its exact target size in config/symbols.txt so a later delink
// cannot cut the target short. The sizes of RemoveAnimations and
// LoadExtraAnimations include their exception-handler tails.
#include <string.h>

#include "Tongas2/MATHS.h"
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

// The engine class GD515e70 derives from; its destructor is at 0x4e6e00 and
// has no name in the symbol file, so the unit maps it onto this one.
class GD515e70BASE
{
public:
    virtual ~GD515e70BASE();
};

class GD515e70 : public GD515e70BASE
{
public:
    virtual ~GD515e70() throw();
};

__declspec(noinline) GD515e70::~GD515e70() throw()
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
    return table[index];
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

struct G2SKMLISTNODE
{
    void *m_value;
    G2SKMLISTNODE *m_next;
    G2SKMLISTNODE *m_prev;
};

struct G2SKMCLEAR
{
    void Clear();

    G2SKMLISTNODE *m_head;
};

void G2SKMCLEAR::Clear()
{
    G2SKMLISTNODE *node = m_head;
    while (node) {
        G2SKMLISTNODE *next = node->m_next;
        node->m_next = 0;
        node->m_prev = 0;
        free(node);
        node = next;
    }
    m_head = 0;
}

struct G2SKMTHUNK
{
    void Clear();
};

void G2SKMTHUNK::Clear()
{
    ((G2SKMCLEAR *)*(void **)((char *)this + 0x14))->Clear();
}

struct SKA
{
    char pad[0x38];
    void *m_38;
};

struct SKARR
{
    int *m_data;
    int &operator[](int index) { return m_data[index]; }
};

struct SKB
{
    char pad[0x18];
    SKARR m_18;
};

struct SKD
{
    char pad[4];
    void *m_4;
    char pad2[0x1c];
    SKARR m_24;
};

class SKX
{
public:
    void *GetVertexData(void *out, int index);
    V3 *GetBonePos(V3 *out, int index);
};

void *SKX::GetVertexData(void *out, int index)
{
    SKA *a = (SKA *)*(void **)((char *)this + 0xc);
    SKB *b = (SKB *)a->m_38;
    int offset = b->m_18[index];
    SKD *d = (SKD *)*(void **)((char *)this + 0x10);
    memcpy(out, (char *)d->m_4 + offset * 0x40, 0x40);
    return out;
}

V3 *SKX::GetBonePos(V3 *out, int index)
{
    SKA *a = (SKA *)*(void **)((char *)this + 0xc);
    SKB *b = (SKB *)a->m_38;
    int bone = b->m_18[index];
    SKD *d = (SKD *)*(void **)((char *)this + 0x10);
    V3 *src = (V3 *)((char *)d->m_24.m_data + bone * 0xc);
    *out = *src;
    return out;
}
