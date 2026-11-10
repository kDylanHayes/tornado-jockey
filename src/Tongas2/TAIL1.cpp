#include <stdlib.h>

// Tongas2/TAIL1 -- discovery scaffold for the remaining 0x54eec0-range code.

class GD54f0b0
{
public:
    virtual ~GD54f0b0();
};

__declspec(noinline) GD54f0b0::~GD54f0b0()
{
}

class GD5545a0
{
public:
    virtual ~GD5545a0();
};

__declspec(noinline) GD5545a0::~GD5545a0()
{
}

class GD550820
{
public:
    virtual ~GD550820();
};

__declspec(noinline) GD550820::~GD550820()
{
}

class GD550e70
{
public:
    virtual ~GD550e70();
};

__declspec(noinline) GD550e70::~GD550e70()
{
}

class GD5517a0
{
public:
    virtual ~GD5517a0();
};

__declspec(noinline) GD5517a0::~GD5517a0()
{
}

class GD551920
{
public:
    virtual ~GD551920();
};

__declspec(noinline) GD551920::~GD551920()
{
}

class GD551d70
{
public:
    virtual ~GD551d70();
};

__declspec(noinline) GD551d70::~GD551d70()
{
}

struct G2TL1FREE
{
    void Free();
};

void G2TL1FREE::Free()
{
    free(*(void **)this);
}

class G2TL1BASE
{
public:
    G2TL1BASE();
    virtual void V();
};

class G2TL1C1 : public G2TL1BASE
{
public:
    G2TL1C1();
};

G2TL1C1::G2TL1C1()
{
}

class G2TL1C2 : public G2TL1BASE
{
public:
    G2TL1C2();
};

G2TL1C2::G2TL1C2()
{
}

extern void *g_6c65e4;

void *GetGlobal()
{
    return g_6c65e4;
}

struct G2TL1SELF
{
    void *Self(int arg);
};

void *G2TL1SELF::Self(int arg)
{
    return this;
}

extern "C" char DAT_0067d0dc;
void *GetD0067d0dc()
{
    return &DAT_0067d0dc;
}

struct TAIL1M1
{
    void Set825(const bool &value);
    void Set834(const bool &value);
    void Set820(const int &value);
    void Set824(const bool &value);
};

void TAIL1M1::Set825(const bool &value)
{
    *(bool *)((char *)this + 0x825) = value;
}

void TAIL1M1::Set834(const bool &value)
{
    *(bool *)((char *)this + 0x834) = value;
}

void TAIL1M1::Set820(const int &value)
{
    *(int *)((char *)this + 0x820) = value;
}

void TAIL1M1::Set824(const bool &value)
{
    *(bool *)((char *)this + 0x824) = value;
}
