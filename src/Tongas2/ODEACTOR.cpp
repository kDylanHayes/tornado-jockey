// Tongas2/ODEACTOR -- the ODE actor object.
//
// Discovery scaffold: bodies are empty until the unit is matched.
#include "Tongas2/ODEACTOR.h"
#include "Tongas2/CMDBASE.h"

extern char g_classNames[][30];

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
