#include <new>

// Tongas2/TAIL3 -- discovery scaffold for the remaining 0x54eec0-range code.

class V5547C0
{
public:
    virtual ~V5547C0();
};

V5547C0::~V5547C0()
{
}

class V554E60
{
public:
    virtual ~V554E60();
};

V554E60::~V554E60()
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

class GD5545d0
{
public:
    virtual ~GD5545d0();
};

__declspec(noinline) GD5545d0::~GD5545d0()
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
    void Call();
};

struct G2TL3TG2
{
    void Call();
};

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
    ((G2TL3TGT1 *)((char *)this + 0xc))->Call();
}

void G2TL3TH2::Call()
{
    ((G2TL3TG2 *)((char *)this + 0xc))->Call();
}

struct G2TL3ALLOC
{
    void *Alloc(unsigned int n);
};

void *G2TL3ALLOC::Alloc(unsigned int n)
{
    return operator new(n * 2);
}
