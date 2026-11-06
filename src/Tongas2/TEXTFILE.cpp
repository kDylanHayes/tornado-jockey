// Tongas2/TEXTFILE -- the text tokenizer used by the skinned objects.
//
// Discovery scaffold: bodies are empty until the unit is matched; each
// function has its exact target size in config/symbols.txt so a later delink
// cannot cut the target short.
#include "Tongas2/TEXTFILE.h"

#include <string>
#include <vector>

typedef std::vector<int> VI;
typedef std::vector<VI> VV;

class REFOBJ
{
public:
    void ReleaseA();
    void ReleaseB();
};

class TRACK
{
public:
    void Copy(int *dest, int flags);
};

class PARTICLE
{
public:
    void ReleaseA();
    void ReleaseB();
    int FindKey(int value);
    int *CopyTrack(int *dest, int index, int flags);
    int GetTrackOffset(int index);

private:
    char m_000[0x44];
    REFOBJ *m_044;
    int m_048;
    TRACK **m_04c;
    char m_050[0x24];
    std::vector<int> m_074;
};

class PARTICLEB
{
public:
    void DestroyMembers();

private:
    char m_000[0x5c];
    std::string m_05c;
    std::string m_078;
    std::vector<int> m_094;
    std::vector<int> m_0a4;
};

class VECINT : public std::vector<int>
{
public:
    void PushBack(const int &value);
};

class VECVEC : public std::vector<VI>
{
public:
    void PushBack(const VI &value);
};

int TEXTFILE::Load(const char *name)
{
    (void)name;
    return 0;
}

int TEXTFILE::GetToken()
{
    return 0;
}

int TEXTFILE::GetLine()
{
    return 0;
}

void PARTICLE::ReleaseA()
{
    m_044->ReleaseA();
}

void PARTICLE::ReleaseB()
{
    m_044->ReleaseB();
}

int PARTICLE::FindKey(int value)
{
    int result = 0;
    for (unsigned i = 0; i < m_074.size(); i++) {
        if (m_074[i] <= value)
            result = i;
    }
    return result;
}

int *PARTICLE::CopyTrack(int *dest, int index, int flags)
{
    TRACK **slot = &m_04c[index];
    TRACK *track = *slot;
    track->Copy(dest, flags);
    return dest;
}

int PARTICLE::GetTrackOffset(int index)
{
    TRACK **slot = &m_04c[index];
    return (int)*slot + 0x5c;
}

void __stdcall DestroyIntVector(VI *v)
{
    struct LAYOUT
    {
        int alloc;
        void *first;
        void *last;
        void *end;
    } *p = (LAYOUT *)v;
    if (p->first)
        operator delete(p->first);
    p->first = 0;
    p->last = 0;
    p->end = 0;
}

void PARTICLEB::DestroyMembers()
{
    m_0a4.~vector();
    m_094.~vector();
    m_078.~basic_string();
    m_05c.~basic_string();
}

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

void VECINT::PushBack(const int &value)
{
    push_back(value);
}

void VECVEC::PushBack(const VI &value)
{
    push_back(value);
}
