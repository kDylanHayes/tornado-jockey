#define CSTRING_CTOR_OUTOFLINE
#include <new>
#include <string>
#include <malloc.h>

#include "Tongas2/CSTRING.h"
#include <string>
#include <stdexcept>

// IAT thunks the retail objects call through (InterlockedIncrement/Decrement).
extern "C" void (__stdcall *g_fn6561b4)(void *);
extern "C" long (__stdcall *g_fn6561ac)(long *);

// Tongas2/TAIL3 -- discovery scaffold for the remaining 0x54eec0-range code.

class GD556050
{
public:
    virtual ~GD556050();
};

class V5547C0
{
public:
    V5547C0();
    virtual ~V5547C0();
    V5547C0(const V5547C0 &other);

    int m_4;
};

V5547C0::V5547C0()
{
    m_4 = 0;
}

__declspec(noinline) V5547C0::~V5547C0()
{
}

V5547C0::V5547C0(const V5547C0 &other)
{
    m_4 = other.m_4;
}

class V5547C4 : public V5547C0
{
public:
    virtual ~V5547C4();
    V5547C4(const V5547C4 &other);
    // Drops the reference count; deletes this at zero (0x5547e0).
    void Release();
};

V5547C4::V5547C4(const V5547C4 &other) : V5547C0(other)
{
}

__declspec(noinline) V5547C4::~V5547C4()
{
}

__declspec(noinline) void V5547C4::Release()
{
    if (g_fn6561ac((long *)&m_4) == 0)
        delete this;
}

class V554E60
{
public:
    virtual ~V554E60();
};

__declspec(noinline) V554E60::~V554E60()
{
}

class GD557e70
{
public:
    virtual ~GD557e70();
};

__declspec(noinline) GD557e70::~GD557e70()
{
}

class GD5545d0 : public GD556050
{
public:
    virtual ~GD5545d0() throw();
};

__declspec(noinline) GD5545d0::~GD5545d0() throw()
{
}

struct G2TL3SET
{
    void Set4c(int value);
};

void G2TL3SET::Set4c(int value)
{
    *(int *)((char *)this + 0x4c) = value;
}

extern "C" void (__stdcall *g_fn6561b4)(void *);

struct G2TL3CALL
{
    void Call();
};

void G2TL3CALL::Call()
{
    g_fn6561b4((char *)this + 4);
}

struct G2TL3TGT1
{
    ~G2TL3TGT1();

    std::string m_0;
    V5547C4 *m_1c;
    void *m_20;
};

G2TL3TGT1::~G2TL3TGT1()
{
    V5547C4 *sub = m_1c;
    if (m_20) {
        if (g_fn6561ac((long *)m_20) == 0) {
            delete sub;
            _aligned_free(m_20);
            m_20 = 0;
        }
    }
}

struct G2TL3TG2
{
    ~G2TL3TG2();

    std::string m_0;
    V5547C4 *m_1c;
};

G2TL3TG2::~G2TL3TG2()
{
    if (m_1c)
        m_1c->Release();
}

struct G2TL3TH1
{
    void Call();
};

struct G2TL3TH2
{
    void Call();
};

void G2TL3TH1::Call()
{
    ((G2TL3TGT1 *)((char *)this + 0xc))->~G2TL3TGT1();
}

void G2TL3TH2::Call()
{
    ((G2TL3TG2 *)((char *)this + 0xc))->~G2TL3TG2();
}

struct G2TL3ALLOC
{
    void *Alloc(unsigned int n);
};

void *G2TL3ALLOC::Alloc(unsigned int n)
{
    return operator new(n * 2);
}

struct TL3GET
{
    int Get0(void);
    char Get1(void);
    int Get2(void);
};

int TL3GET::Get0(void)
{
    return *(int *)((char *)this + 0x8);
}

char TL3GET::Get1(void)
{
    return *(char *)((char *)this + 0x50);
}

int TL3GET::Get2(void)
{
    return *(int *)((char *)this + 0x4);
}

struct TL3PTR
{
    void *Get0();
    void *Get1();
};

void *TL3PTR::Get0()
{
    return (char *)this + 0x10;
}

void *TL3PTR::Get1()
{
    return (char *)this + 0x2c;
}

class TAIL3X
{
public:
    void Call4C(int a);
};

void TAIL3X::Call4C(int a)
{
    void (__fastcall *func)(int) = *(void (__fastcall **)(int))((char *)this + 0x4c);
    if (func)
        func(a);
}

CSTRING::CSTRING() : m_str(0)
{
}

class TAIL1ERR : public std::logic_error
{
public:
    TAIL1ERR(const std::string &message) : std::logic_error(message) {}
    TAIL1ERR(const TAIL1ERR &other);
};

void *GetTail3Root();

// The per-user folder under the game's root path (0x554320).
std::string __fastcall GetUserFolder(int folder)
{
    if (folder < 0 || folder >= 2) {
        std::string message("CSaveFile::GetRootPath called with invalid Type");
        throw TAIL1ERR(message);
    }
    std::string result(*(std::string *)((char *)GetTail3Root() + 0x28));
    if (folder == 0)
        result.append("\\Cache", 6);
    else
        result.append("\\Persistent", 11);
    return result;
}
