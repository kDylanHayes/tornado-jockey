// Tongas2/TEXTFILE -- the text tokenizer used by the skinned objects.
//
// Discovery scaffold: bodies are empty until the unit is matched; each
// function has its exact target size in config/symbols.txt so a later delink
// cannot cut the target short.
#include "Tongas2/TEXTFILE.h"
#include "Tongas2/LOG.h"

#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>

typedef std::vector<int> VI;
typedef std::vector<VI> VV;

extern "C" char s_TEXTFILE_file[] = "TEXTFILE - file does not exist: %s";
bool FileExists(std::string *path, char *flag);

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

struct V3
{
    float x, y, z;
    V3() : x(0), y(0), z(0) {}
};

class PARTICLE
{
public:
    PARTICLE(int a, int b);
    void ReleaseA();
    void ReleaseB();
    int FindKey(int value);
    int *CopyTrack(int *dest, int index, int flags);
    int GetTrackOffset(int index);

private:
    char m_000;
    char pad0[3];
    V3 m_004;
    V3 m_010;
    std::vector<int> m_01c;
    int m_02c;
    int m_030;
    std::vector<std::string> m_034;
    REFOBJ *m_044;
    std::vector<TRACK *> m_048;
    std::vector<VI> m_058;
    float m_068;
    int m_06c;
    int m_070;
    std::vector<int> m_074;
};

class PARTICLEB
{
public:
    PARTICLEB(const std::string &name);
    void DestroyMembers();

private:
    char m_000[0x5c];
    std::string m_05c;
    std::string m_078;
    std::vector<int> m_094;
    std::vector<int> m_0a4;
};

class ANIMTRACK
{
public:
    void GetPoint(V3 *out, unsigned i1, unsigned i2, float t);

private:
    char m_000[0x10];
    V3 m_010;
    char m_01c[0x88];
    std::vector<V3> m_0a4;
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

class READER
{
public:
    void Read(void *buf, int size, int flag, void *tok);
};

class STREAMOBJ
{
public:
    char pad[0x1c];
    READER *m_1c;
};

class STREAM
{
public:
    virtual void v00();
    virtual void Open(const char *name);
    virtual void v08();
    virtual int IsOpen();
    virtual void v10();
    virtual void v14();
    virtual void v18();
    virtual void v1c();
    virtual char *GetString();
};

class TEXTREADER
{
public:
    int Load(const std::string &name);
    std::string ReadString();

private:
    STREAM *m_stream;
};

TOK::TOK() : m_000(0), m_004(0), m_008(0)
{
}

int TEXTREADER::Load(const std::string &name)
{
    char buffer[0x200];
    const char *text = name.c_str();
    if (text)
        strncpy(buffer, text, 0x1ff);
    else
        buffer[0] = 0;
    buffer[0x1ff] = 0;
    m_stream->Open(buffer);
    if (m_stream->IsOpen())
        return 1;
    bool found;
    {
        std::string local;
        local = buffer;
        char flag = 0;
        found = FileExists(&local, &flag) != 0;
    }
    if (found) {
        if (g_pLog)
            g_pLog->ContentError(s_TEXTFILE_file, buffer);
    }
    return 0;
}

std::string TEXTREADER::ReadString()
{
    char *mem = m_stream->GetString();
    std::string tmp;
    if (mem) {
        tmp = mem;
        free(mem);
    }
    return tmp;
}

TEXTFILE::TEXTFILE() : m_030(0), m_034(0), m_080(0)
{
}

void TEXTFILE::GetToken(void *buf, int size)
{
    m_084.Init(0, 0);
    if (m_030) {
        if (m_030->m_1c)
            m_030->m_1c->Read(buf, size, 1, &m_084);
    }
    else {
        STREAMOBJ *p = m_034;
        if (p->m_1c)
            p->m_1c->Read(buf, size, 1, &m_084);
    }
}

PARTICLE::PARTICLE(int a, int b)
    : m_000(0), m_02c(a), m_030(b), m_068(1.0f), m_06c(0), m_070(0)
{
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
    TRACK **slot = &m_048[index];
    TRACK *track = *slot;
    track->Copy(dest, flags);
    return dest;
}

int PARTICLE::GetTrackOffset(int index)
{
    TRACK **slot = &m_048[index];
    return (int)*slot + 0x5c;
}

void ANIMTRACK::GetPoint(V3 *out, unsigned i1, unsigned i2, float t)
{
    if (m_0a4.size() == 0) {
        *out = m_010;
        return;
    }
    if (i1 >= m_0a4.size())
        i1 = m_0a4.size() - 1;
    if (i2 >= m_0a4.size())
        i2 = m_0a4.size() - 1;
    m_010.x = (m_0a4[i2].x - m_0a4[i1].x) * t + m_0a4[i1].x;
    m_010.y = (m_0a4[i2].y - m_0a4[i1].y) * t + m_0a4[i1].y;
    m_010.z = (m_0a4[i2].z - m_0a4[i1].z) * t + m_0a4[i1].z;
    *out = m_010;
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

PARTICLEB::PARTICLEB(const std::string &name) : m_05c(name)
{
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
