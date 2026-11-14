#include <deque>
#include <vector>
#include <string>
#include <string.h>
#include <stdexcept>

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

// ---------------------------------------------------------------------------
// retail 0x0052ff00 / 0x0052ff20 / 0x005300d0 -- the element table of an
// animated geometry: a vector of element pointers that starts at +0x10 of
// the owning object.  Each element carries its name string at +0x20, a count
// at +0x44 and a divisor at +0x48.
// ---------------------------------------------------------------------------
struct GD52ff00Elem
{
    char m_000[0x20];
    std::string m_name;
    char m_03c[8];
    int m_44;
    float m_48;
};

class GD52ff00
{
public:
    void *Get(int index);
    int Find(const std::string *name);
    float Calc(const std::string *name);

    char m_000[0x10];
    std::vector<void *> m_v;
};

void *GD52ff00::Get(int index)
{
    return (char *)m_v[index] + 0x20;
}

int GD52ff00::Find(const std::string *name)
{
    int i;
    int count = (int)m_v.size();
    for (i = 0; i < count; i++) {
        GD52ff00Elem *e = (GD52ff00Elem *)m_v[i];
        if (e->m_name.compare(*name) == 0)
            return i;
    }
    return -1;
}

float GD52ff00::Calc(const std::string *name)
{
    int i = Find(name);
    if (i < 0 || i >= (int)m_v.size())
        return 0.0f;
    GD52ff00Elem *e = (GD52ff00Elem *)m_v[i];
    return (float)(unsigned int)e->m_44 / e->m_48;
}

// ---------------------------------------------------------------------------
// retail 0x005309f0 -- std::vector<int>::push_back, inlined into a member of
// a class whose vector starts at offset 0.
// ---------------------------------------------------------------------------
class GD5309f0
{
public:
    void Push(const int &value);

    std::vector<int> m_v;
};

void GD5309f0::Push(const int &value)
{
    m_v.push_back(value);
}

// ---------------------------------------------------------------------------
// retail 0x0052faa0 -- block move helper: copies the whole 4-byte groups of
// [first, last) to the tail of the destination block.
// ---------------------------------------------------------------------------
void GD52faa0(void *first, void *last, void *end)
{
    int count = (int)((char *)last - (char *)first);
    int words = count >> 2;
    memmove((char *)end - words * 4, first, words * 4);
}

// ---------------------------------------------------------------------------
// retail 0x00530cb0 -- appends a value to the second vector of the object.
// The helper at retail 0x0044a2a0 lives in another unit.
// ---------------------------------------------------------------------------
class GD530cb0
{
public:
    int m_40(int value);
    void Add(int value, int extra);

    std::vector<int> m_v0;
    std::vector<int> m_v1;
};

void GD530cb0::Add(int value, int extra)
{
    (void)extra;
    m_40(value);
    m_v1.push_back(value);
}

// ---------------------------------------------------------------------------
// retail 0x00530120 / 0x00530190 -- the length_error throws of the standard
// deque and vector.
// ---------------------------------------------------------------------------
void GD530120()
{
    throw std::length_error("deque<T> too long");
}

void GD530190()
{
    throw std::length_error("vector<T> too long");
}
