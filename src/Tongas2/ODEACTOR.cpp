// Tongas2/ODEACTOR -- the ODE actor object.
//
// Discovery scaffold: bodies are empty until the unit is matched.
#include "Tongas2/ODEACTOR.h"
#include "Tongas2/CMDBASE.h"

extern char g_classNames[][30];
extern int g_factoryFlags[];

class EMITTERMASTER
{
public:
    void SetForce(const V3 &value);
};

class ACTORX
{
public:
    void SetResourcesPos(const int &index, const V3 &position);
    void SetResourcesAng(const int &index, const ANG &angles);
    void SetResourcesScale(const int &index, const V3 &scale);
};

void ODEACTORMASTER::RegisterCommands()
{
}

void ODEACTOR::SetFlag(const char *flag)
{
    (void)flag;
}

class GD54c7b0
{
public:
    virtual ~GD54c7b0();
};

__declspec(noinline) GD54c7b0::~GD54c7b0()
{
}

class GD54c9b0
{
public:
    virtual ~GD54c9b0();
};

__declspec(noinline) GD54c9b0::~GD54c9b0()
{
}

class GD54cb00
{
public:
    virtual ~GD54cb00();
};

__declspec(noinline) GD54cb00::~GD54cb00()
{
}

struct G2ODEACTORTGT
{
    void Call();
};

struct G2ODEACTORX
{
    void Check();
};

void G2ODEACTORX::Check()
{
    if (!*(void **)((char *)this + 0xccc))
        ((G2ODEACTORTGT *)this)->Call();
}

struct G2ODEADJ
{
    void Call(void *arg);
};

struct G2ODEADJTGT
{
    void Call(void *arg);
};

void G2ODEADJ::Call(void *arg)
{
    ((G2ODEADJTGT *)((char *)this + 4))->Call((char *)arg + 8);
}

class G2ODEACTOR
{
public:
    virtual int GetClassId();
    virtual int GetParentClassId();
    virtual const char *GetClassName();

    static int s_classId;
};

int G2ODEACTOR::GetClassId()
{
    return s_classId;
}

int G2ODEACTOR::GetParentClassId()
{
    return g_classParents[s_classId];
}

const char *G2ODEACTOR::GetClassName()
{
    return g_classNames[s_classId];
}

struct ODEACTORM1
{
    void SetForceAndClear(const V3 &value);
    void SetResourcesPosAndClear(int index, const V3 &position);
    void SetResourcesAngAndClear(int index, const ANG &angles);
    void SetResourcesScaleAndClear(int index, const V3 &scale);
};

void ODEACTORM1::SetForceAndClear(const V3 &value)
{
    ((EMITTERMASTER *)this)->SetForce(value);
    *(bool *)((char *)this + 0xd14) = false;
}

void ODEACTORM1::SetResourcesPosAndClear(int index, const V3 &position)
{
    ((ACTORX *)this)->SetResourcesPos(index, position);
    *(bool *)((char *)this + 0xd14) = false;
}

void ODEACTORM1::SetResourcesAngAndClear(int index, const ANG &angles)
{
    ((ACTORX *)this)->SetResourcesAng(index, angles);
    *(bool *)((char *)this + 0xd14) = false;
}

void ODEACTORM1::SetResourcesScaleAndClear(int index, const V3 &scale)
{
    ((ACTORX *)this)->SetResourcesScale(index, scale);
    *(bool *)((char *)this + 0xd14) = false;
}

extern void *g_pOdeWorldX;

class ODEWORLDX
{
public:
    virtual ~ODEWORLDX();
    void Shutdown();
};

int __stdcall FirstOfTwo(int first, int second)
{
    return first;
}

void ShutdownOdeWorld()
{
    ((ODEWORLDX *)g_pOdeWorldX)->Shutdown();
}

void DestroyOdeWorld()
{
    if (g_pOdeWorldX) {
        delete (ODEWORLDX *)g_pOdeWorldX;
        g_pOdeWorldX = 0;
    }
}

struct ODEWORLDRESET
{
    int m_0;
    int m_4;
    int m_8;
    int m_c;
    int m_10;
    int m_14;
    int m_18;
    int m_1c;
    int m_20;
    int m_24;
    int m_28;
    char m_2c[0x6c - 0x2c];
    bool m_6c;

    ODEWORLDRESET();
};

ODEWORLDRESET::ODEWORLDRESET()
{
    m_8 = 0;
    m_0 = -1;
    m_4 = -1;
    m_14 = 0;
    m_18 = 0;
    m_1c = 0;
    m_20 = 0;
    m_24 = 0;
    m_28 = 0;
    m_6c = true;
    m_c = -1;
    m_10 = -1;
}

struct G2CORE2M7
{
    void SetD18(const float &value);
};

void G2CORE2M7::SetD18(const float &value)
{
    *(float *)((char *)this + 0xd18) = value;
    *(bool *)((char *)this + 0xd14) = false;
}

extern "C" void dBodySetGravityMode(void *body, int mode);

struct ODEACTORM2
{
    void SetD1C(const bool &enabled);
};

void ODEACTORM2::SetD1C(const bool &enabled)
{
    *(bool *)((char *)this + 0xd1c) = enabled;
    void *body = *(void **)((char *)this + 0xccc);
    if (body)
        dBodySetGravityMode(body, enabled);
}

class ODEACTORM3
{
public:
    virtual int GetClassId();
    virtual int GetFactoryFlags();

    bool Predicate(int a, int b, int c, int d);

    static int s_classId;
};

int ODEACTORM3::GetClassId()
{
    return s_classId;
}

int ODEACTORM3::GetFactoryFlags()
{
    return g_factoryFlags[s_classId];
}

bool ODEACTORM3::Predicate(int a, int b, int c, int d)
{
    return true;
}
