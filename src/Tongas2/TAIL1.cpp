#include <string>
#include <stdlib.h>
#include <stdexcept>

#include "Tongas2/MATHS.h"
#include "Tongas2/FILELOADER.h"

// Tongas2/TAIL1 -- discovery scaffold for the remaining 0x54eec0-range code.

class GD556050
{
public:
    virtual ~GD556050();
};

class V5547C4
{
public:
    virtual ~V5547C4();

    int m_4;
};

class GD54f0b0
{
public:
    virtual ~GD54f0b0();
};

__declspec(noinline) GD54f0b0::~GD54f0b0()
{
}

__declspec(noinline) V5547C4::~V5547C4()
{
}

class GD550820 : public GD556050
{
public:
    virtual ~GD550820() throw();
};

__declspec(noinline) GD550820::~GD550820() throw()
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

class GD5517a0 : public GD556050
{
public:
    virtual ~GD5517a0() throw();
};

__declspec(noinline) GD5517a0::~GD5517a0() throw()
{
}

class GD551920 : public GD556050
{
public:
    virtual ~GD551920() throw();
};

__declspec(noinline) GD551920::~GD551920() throw()
{
}

class GD551d70BASE
{
public:
    virtual ~GD551d70BASE();

    char pad_04[8];
};

class GD551d70 : public GD551d70BASE
{
public:
    virtual ~GD551d70();

    std::string m_10;
};

GD551d70::~GD551d70()
{
}

class G2CORE1K4 : public GD551d70BASE
{
public:
    virtual ~G2CORE1K4();

    std::string m_10;
};

G2CORE1K4::~G2CORE1K4()
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
extern int g_classParents[];
extern char g_classNames[][30];

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

struct TAIL1M5
{
    void Set828(const V3 &value);
};

void TAIL1M5::Set828(const V3 &value)
{
    *(V3 *)((char *)this + 0x828) = value;
}

class TAIL1CID
{
public:
    virtual int GetParentClassId();
    virtual const char *GetClassName();
};

int TAIL1CID::GetParentClassId()
{
    return g_classParents[(int)g_6c65e4];
}

const char *TAIL1CID::GetClassName()
{
    return g_classNames[(int)g_6c65e4];
}

class TAIL1X
{
public:
    bool HasC();
};

bool TAIL1X::HasC()
{
    return *(int *)((char *)this + 0xc) != 0;
}

class TAIL1ERR : public std::logic_error
{
public:
    TAIL1ERR(const TAIL1ERR &other);
};

TAIL1ERR::TAIL1ERR(const TAIL1ERR &other) : std::logic_error(other)
{
}

bool FILELOADER::IsOpen()
{
    return m_fp != 0;
}

class WTINFO : public V5547C4
{
public:
    ~WTINFO();

    std::string m_08;
    std::string m_24;
};

WTINFO::~WTINFO()
{
}
