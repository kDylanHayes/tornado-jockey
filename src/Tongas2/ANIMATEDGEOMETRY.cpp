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

// ---------------------------------------------------------------------------
// The geometry-track object behind retail 0x0052f8c0: a pointer to the
// keyframe source at +0x0c and a container at +0x20.  Its members read the
// source through the pointer and hand it the address of the +0x20 container.
// 0x0052f8c0 itself is the constructor (six arguments).
// ---------------------------------------------------------------------------
struct G2TFGET
{
    void *Get5c(int index);
    // retail 0x0052cd50: five arguments, returns int.
    int Make5(int a, int b, void *p, int c, int d);
    // retail 0x0052aa60: three arguments.
    void Make3(int a, int b, void *p);
};

class TRINNER
{
public:
    int M10(int a, int b);
    int M11(int a, int b, int c, int d);
    // retail 0x0052fff0: deletes every queued element and clears the queue.
    void Clear();

    char m_000[0xc];
    G2TFGET *m_p;
    int m_010;
    int m_014;
    int m_018;
    int m_01c;
    std::deque<G2TFGET *> m_queue;
};

int TRINNER::M10(int a, int b)
{
    m_p->Make3(a, b, &m_queue);
    return a;
}

int TRINNER::M11(int a, int b, int c, int d)
{
    return m_p->Make5(a, b, &m_queue, c, d);
}

// Residue: retail is 183 bytes, this compiles to 179.  The four-byte
// difference is the `if (p != 0)` guard MSVC keeps in front of the operator
// delete call at retail +0x67..+0x6a (`85 c9 74 09 51`); here the compiler
// folds it because it already knows m_queue[i] is non-null.  The register
// pick at +0x3f (`pop ebx` here, `pop edi` at retail) follows from that.
void TRINNER::Clear()
{
    for (int i = 0; i < m_queue.size(); i++) {
        if (m_queue[i]) {
            delete m_queue[i];
            m_queue[i] = 0;
        }
    }
    m_queue.clear();
}

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
    int Find(const std::string &name);
    float Calc(const std::string &name);
    int Lookup(const std::string &name);
    // retail 0x0052fad0, in this unit's tail.
    int M2(int index);

    char m_000[0x10];
    std::vector<void *> m_v;
};

void *GD52ff00::Get(int index)
{
    return (char *)m_v[index] + 0x20;
}

int GD52ff00::Find(const std::string &name)
{
    for (int i = 0; i < m_v.size(); i++) {
        GD52ff00Elem *e = (GD52ff00Elem *)m_v[i];
        if (e->m_name.compare(name) == 0)
            return i;
    }
    return -1;
}

float GD52ff00::Calc(const std::string &name)
{
    int i = Find(name);
    if (i >= m_v.size() || i < 0)
        return 0.0f;
    GD52ff00Elem *e = (GD52ff00Elem *)m_v[i];
    return (float)(unsigned int)e->m_44 / e->m_48;
}

int GD52ff00::Lookup(const std::string &name)
{
    return M2(Find(name));
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
    m_40(value);
    m_v1.push_back(extra);
}

// ---------------------------------------------------------------------------
// retail 0x00530120 / 0x00530190 -- the length_error throws of the standard
// deque and vector.  They are emitted by the deque<T *> instantiation above
// (through _Growmap) and by vector<int>'s _Insert_n, so no body is needed.
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// retail 0x00530540 -- std::deque<T *>::push_front, emitted standalone because
// the unit pushes into the queue from several places.
// ---------------------------------------------------------------------------
template void std::deque<G2TFGET *>::push_front(G2TFGET *const &);
