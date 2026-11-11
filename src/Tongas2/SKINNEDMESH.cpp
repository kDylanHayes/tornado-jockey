#include <stdlib.h>
#include <vector>

// The object whose slot 0x1c unlinks it and whose deleting destructor runs at
// slot 0; the wrapper at 0x5285f0 releases it.
class GD5285f0OBJ
{
public:
    virtual ~GD5285f0OBJ();
    virtual void V01();
    virtual void V02();
    virtual void V03();
    virtual void V04();
    virtual void V05();
    virtual void V06();
    virtual void Unlink();
};

struct GD5285f0
{
    GD5285f0OBJ *m_0;

    void Release();
    void Unlink();
    void V06();
};

void GD5285f0::Unlink()
{
    m_0->Unlink();
}

void GD5285f0::V06()
{
    m_0->V06();
}

class FILELOADER;

// Creates a FILELOADER and keeps it (0x5285c0).
struct GD5285c0
{
    FILELOADER *m_0;

    GD5285c0();
};

FILELOADER *(*g_pfnCreateFileLoader)();

GD5285c0::GD5285c0()
{
    m_0 = g_pfnCreateFileLoader();
}

void GD5285f0::Release()
{
    m_0->Unlink();
    if (m_0) {
        delete m_0;
        m_0 = 0;
    }
}
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


// Wrappers the mesh exposes through its slot at +0xf8.
class TEXWRAPA1
{
public:
    void Call();
};

class TEXWRAPA2
{
public:
    void Call(int value);
};

class GD520c50
{
public:
    void Call();

    char pad_f8[0xf8];
    TEXWRAPA1 *m_f8;
};

void GD520c50::Call()
{
    m_f8->Call();
}

class GD520c60
{
public:
    void Call(int value);

    char pad_f8[0xf8];
    TEXWRAPA2 *m_f8;
};

void GD520c60::Call(int value)
{
    m_f8->Call(value);
}


// The mesh's pointer array at +0x134, and its size.
class GD517dd0
{
public:
    int Size();

    char pad_134[0x134];
    void **m_134;
    void **m_138;
};

int GD517dd0::Size()
{
    if (!m_134)
        return 0;
    return (int)(m_138 - m_134);
}


// Releases the members at +0xc and +8 through the object at +0, then the
// one at +0x5c with the plain delete.
class M42TARGET
{
public:
    void m_42(void *item);
    void m_43(void *item);
};

class GD517500
{
public:
    void Release();

    M42TARGET *m_obj;
    char pad_4[4];
    void *m_8;
    void *m_c;
    char pad_10[0x5c - 0x10];
    void *m_5c;
};

void GD517500::Release()
{
    if (m_c)
        m_obj->m_42(m_c);
    if (m_8) {
        m_obj->m_43(m_8);
        m_8 = 0;
    }
    if (m_5c) {
        ::operator delete(m_5c);
        m_5c = 0;
    }
}
