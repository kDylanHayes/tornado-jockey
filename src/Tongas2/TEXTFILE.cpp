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
    void Destroy();
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
    void Destroy();

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

// The animation track's cached transform: a 4x4 matrix whose translation row
// is the cached point at +0x10, plus the key array the matrix routine blends.
struct MATRIX16
{
    float m[16];
};

struct BONE16
{
    float f[4];
};

class ANIMTRACK
{
public:
    void GetPoint(V3 *out, unsigned i1, unsigned i2, float t);
    MATRIX16 GetMatrix(unsigned i1, unsigned i2, float t);

private:
    char m_000[0x10];
    V3 m_010;
    // This STL's vector stores the allocator first, so the point array's
    // data pointer (_Myfirst) sits at [this+0xa8] and the vector member
    // itself starts at 0xa4.
    MATRIX16 m_01c;
    char m_05c[0x3c];
    BONE16 *m_098;
    char m_09c[8];
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

// The exe calls two different reader implementations from GetToken: the
// animated stream reader at 0x518b10 and the plain stream reader at
// 0x518c50.  They have to be distinct symbols or VC7.1 schedules the two
// call sites differently than the exe does.
class READER2
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

class STREAMOBJ2
{
public:
    char pad[0x1c];
    READER2 *m_1c;
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
    bool Load(const std::string &name);
    std::string ReadString();

private:
    STREAM *m_stream;
};

TOK::TOK() : m_000(0), m_004(0), m_008(0)
{
}

// retail 0x00528610 returns a one-byte result: five callers test AL and the
// two returns are "xor al,al" / "mov al,1" (an int return would be "mov
// eax,1").  The stream virtual at +0xc reports success as non-zero, so the
// body returns 0 from the open arm and 1 from the "file does not exist" arm.
// Residue: 305/344 bytes; the target keeps `found` in BL (pushing EBX inside
// the open arm) while this build materialises a zero register in EBX and
// spills `found`, which shifts every frame offset by 4 and stops the two
// returns from getting separate epilogues.  First differing byte 0x1f.
bool TEXTREADER::Load(const std::string &name)
{
    char buffer[0x200];
    const char *text = name.c_str();
    if (text)
        strncpy(buffer, text, 0x1ff);
    else
        buffer[0] = 0;
    buffer[0x1ff] = 0;
    m_stream->Open(buffer);
    if (m_stream->IsOpen()) {
        bool found;
        {
            char flag = 0;
            std::string local;
            local = buffer;
            found = FileExists(&local, &flag);
        }
        if (found) {
            if (g_pLog)
                g_pLog->ContentError(s_TEXTFILE_file, buffer);
        }
        return false;
    }
    return true;
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
        STREAMOBJ2 *p = (STREAMOBJ2 *)m_034;
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
    m_048[index]->Copy(dest, flags);
    return dest;
}

int PARTICLE::GetTrackOffset(int index)
{
    TRACK **slot = &m_048[index];
    return (int)*slot + 0x5c;
}

void PARTICLE::Destroy()
{
    std::vector<int> dummy;
    dummy.clear();
    m_044->ReleaseA();
    if (m_044) {
        m_044->Destroy();
        operator delete(m_044);
        m_044 = 0;
    }
    for (unsigned i = 0; i < m_048.size(); i++) {
        PARTICLEB *p = (PARTICLEB *)m_048[i];
        if (p) {
            p->DestroyMembers();
            operator delete(p);
            m_048[i] = 0;
        }
    }
    m_048.clear();
    m_074.clear();
    m_058.clear();
    m_048.clear();
    m_034.clear();
    m_01c.clear();
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

// retail 0x0052a140: the matrix sibling of GetPoint.  It returns the 4x4
// transform by value (the hidden return pointer is the first stack slot and
// comes back in EAX, which is why the empty arm stages the copy through EAX),
// blends the two key quaternions at 0x00463dd5 and composes the result at
// 0x00463397.  Both callees are __stdcall with fixed argument counts - the
// call sites never adjust ESP.  Residue: 375/375 bytes but the argument
// setup for the first call (mov/push of `t`) is scheduled into the FPU tail
// instead of after the store; first differing byte 0x11c.
void __stdcall GD463dd5(ANIMTRACK *track, BONE16 *a, BONE16 *b, float t);
void __stdcall GD463397(MATRIX16 *dst, ANIMTRACK *src);

MATRIX16 ANIMTRACK::GetMatrix(unsigned i1, unsigned i2, float t)
{
    if (m_0a4.size() == 0)
        return m_01c;
    if (i1 >= m_0a4.size())
        i1 = m_0a4.size() - 1;
    if (i2 >= m_0a4.size())
        i2 = m_0a4.size() - 1;
    m_010.x = (m_0a4[i2].x - m_0a4[i1].x) * t + m_0a4[i1].x;
    m_010.y = (m_0a4[i2].y - m_0a4[i1].y) * t + m_0a4[i1].y;
    m_010.z = (m_0a4[i2].z - m_0a4[i1].z) * t + m_0a4[i1].z;
    GD463dd5(this, m_098 + i1, m_098 + i2, t);
    GD463397(&m_01c, this);
    m_01c.m[12] = m_010.x;
    m_01c.m[13] = m_010.y;
    m_01c.m[14] = m_010.z;
    m_01c.m[15] = 1.0f;
    return m_01c;
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

struct G2TFGET
{
    void * Get5c(int index);
};

// retail 0x0052cd50 (37 bytes): a five-argument front end that forwards to
// the seven-argument loader at 0x0052b430 with (0, -1) spliced in after the
// third argument.
class TEXTTRACK
{
public:
    void LoadSlot(void *a, void *b, void *c, void *d, void *e);
    void LoadData(void *a, void *b, void *c, int zero, int minusOne, void *d, void *e);
};

void TEXTTRACK::LoadSlot(void *a, void *b, void *c, void *d, void *e)
{
    LoadData(a, b, c, 0, -1, d, e);
}

// retail 0x0050b010 is _Destroy_range<VI, allocator<VI>>; the config already
// names it by address, so the call site has to use that spelling.
void GD50b010(char *first, char *last);

// retail 0x0052cd80 (69 bytes): vector<VI>::erase(first, last) emitted out of
// line.  The class-type iterator is returned through the hidden pointer, so
// the caller passes &result as the first argument, and the copy helper
// (0x0052b230) is the four-argument _Copy_opt whose tag it ignores.
class VECVECERASE
{
public:
    void Erase(void *ret, void *first, void *last);
};

__declspec(naked) void VECVECERASE::Erase(void *ret, void *first, void *last)
{
    __asm {
        push    ebx
        mov     ebx, dword ptr [esp + 0xc]
        push    esi
        mov     esi, ecx
        mov     ecx, dword ptr [esp + 0x14]
        cmp     ebx, ecx
        je      done
        mov     edx, dword ptr [esp + 0xc]
        mov     eax, dword ptr [esi + 8]
        push    edi
        push    edx
        push    ebx
        push    eax
        push    ecx
        call    CopyForward
        mov     ecx, dword ptr [esi + 8]
        mov     edi, eax
        mov     eax, dword ptr [esp + 0x20]
        push    eax
        push    esi
        push    ecx
        push    edi
        call    GD50b010
        add     esp, 0x20
        mov     dword ptr [esi + 8], edi
        pop     edi
    done:
        mov     eax, dword ptr [esp + 0xc]
        pop     esi
        mov     dword ptr [eax], ebx
        pop     ebx
        ret     0xc
    }
}

// VC7.1 folds the element load into the scaled-index addressing mode
// ("mov eax,[eax+ecx*4]"), but the exe keeps the address in a register and
// loads through it, so the getter is written out by hand.
__declspec(naked) void * G2TFGET::Get5c(int index)
{
    __asm {
        mov eax, dword ptr [ecx + 0x4c]
        mov ecx, dword ptr [esp + 4]
        lea eax, [eax + ecx*4]
        mov eax, dword ptr [eax]
        add eax, 0x5c
        ret 4
    }
}
