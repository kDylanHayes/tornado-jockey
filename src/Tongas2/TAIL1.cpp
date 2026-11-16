#include <string>
#include <stdlib.h>
#include <stdexcept>

#include "Tongas2/MATHS.h"
#include "Tongas2/FILELOADER.h"
#include "Tongas2/RESOURCES.h"
#include "Tongas2/CMDBASE.h"
#include "Tongas2/ACTORRESOURCES.h"

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
    // The copy constructor at 0x00551dd0 is not implemented: with the natural
    // shape (base copy ctor declared out of line, then
    // m_10.assign(other.m_10, 0, npos)) it compiles to 0x6a bytes against retail's
    // 0x65, and the whole residue is one trylevel store (`mov byte [esp+0x20], 1`)
    // that retail does not emit - its EH state slot holds 0 instead. Same class as
    // TEXTMASTER::BuildCharacters: EH bookkeeping, not source shape.
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
    ::operator delete(*(void **)this);
}

// The base the two descriptor classes share (its constructor lives with the
// 0x5547b0 block).
class V5547C0
{
public:
    V5547C0();
    virtual ~V5547C0();
    V5547C0(const V5547C0 &other);

    int m_4;
};

class G2TL1C1 : public V5547C0
{
public:
    G2TL1C1();
};

__declspec(noinline) G2TL1C1::G2TL1C1()
{
}

class G2TL1C2 : public V5547C0
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

    bool GetBool(const std::string &key);
    std::string GetString(const std::string &key);

    bool UseTracking();
    std::string ProductDisplayName();
    std::string ProductGUID();
    std::string ExeName();
    std::string LaunchExeName();
    std::string RuntimeExeName();

    std::string m_08;
    std::string m_24;
};

WTINFO::~WTINFO()
{
}

// Retail 0x00551470: build the key name and ask for the value.
bool WTINFO::UseTracking()
{
    return GetBool(std::string("UseTracking"));
}

// Retail 0x005511a0..0x005513e0: the five named-string getters.
std::string WTINFO::ProductDisplayName()
{
    return GetString(std::string("ProductDisplayName"));
}

std::string WTINFO::ProductGUID()
{
    return GetString(std::string("ProductGUID"));
}

std::string WTINFO::ExeName()
{
    return GetString(std::string("ExeName"));
}

std::string WTINFO::LaunchExeName()
{
    return GetString(std::string("LaunchExeName"));
}

std::string WTINFO::RuntimeExeName()
{
    return GetString(std::string("RuntimeExeName"));
}

// The ODE actor-template manager factory (0x54ef70).
class ODEACTORTEMPLATEMANAGER : public RESOURCEMANAGER
{
public:
    ODEACTORTEMPLATEMANAGER()
    {
        SafeSprintf(m_fileName, "ODEACTORTEMPLATE.txt");
        m_8 = false;
        m_18 = true;
    }
};

RESOURCEMANAGER *CreateODEACTORTEMPLATE()
{
    return new ODEACTORTEMPLATEMANAGER();
}

// Two-argument constructors (retail 0x00550dd0 and 0x00550e00): each calls
// its base's default constructor, stores its two arguments at +8/+0xc and
// installs its own vtable (data 0x0067d478 and 0x0067d500).
class GD550dd0 : public V5547C0
{
public:
    GD550dd0(int a, int b);
    virtual ~GD550dd0();

    int m_8;
    int m_c;
};

GD550dd0::GD550dd0(int a, int b) : m_8(a), m_c(b)
{
}

__declspec(noinline) GD550dd0::~GD550dd0()
{
}

class GD550e00 : public G2TL1C1
{
public:
    GD550e00(int a, int b);
    virtual ~GD550e00();

    int m_8;
    int m_c;
};

GD550e00::GD550e00(int a, int b) : m_8(a), m_c(b)
{
}

__declspec(noinline) GD550e00::~GD550e00()
{
}

// The ODE actor-template manager's command registration (retail 0x0054eec0):
// the class id lives in the pointer at 0x006c65e4, the command table at
// 0x00a0f1c0 and the parent name string at 0x0067c5d8.
extern "C" CMDENTRY s_cmdtable_54eec0[];
extern "C" char s_ODEACTORTEMPLATEMASTER_0067c5d8[];

void Register54eec0()
{
    int id = *(int *)&g_6c65e4;

    g_classParents[id] = ACTORRESOURCESMASTER::s_classId;
    g_classCommands[id] = s_cmdtable_54eec0;
    HashCommands(s_cmdtable_54eec0);
    RegisterClassName(s_cmdtable_54eec0, s_ODEACTORTEMPLATEMASTER_0067c5d8,
                      "ACTORRESOURCESMASTER");
}

// 0x0054f150: allocates the 0x838-byte object whose constructor is at
// 0x0054eff0, forwarding this and the single argument.
class GD54f150;

class GD54eff0
{
public:
    GD54eff0(GD54f150 *owner, int arg);

    char pad_838[0x838];
};

class GD54f150
{
public:
    GD54eff0 *Create(int arg);
};

GD54eff0 *GD54f150::Create(int arg)
{
    return new GD54eff0(this, arg);
}

// 0x00551a60: allocates the 0x44-byte object whose constructor is at
// 0x00550520, stores it at +0 and initialises it through 0x005547d0.
class GD550520
{
public:
    GD550520(bool arg);
    void Init();

    char pad_44[0x44];
};

class GD551a60
{
public:
    GD551a60();

    GD550520 *m_0;
};

GD551a60::GD551a60()
{
    m_0 = new GD550520(true);
    if (m_0)
        m_0->Init();
}
