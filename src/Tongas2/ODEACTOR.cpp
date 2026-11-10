// Tongas2/ODEACTOR -- the ODE actor object.
//
// Discovery scaffold: bodies are empty until the unit is matched.
#include "Tongas2/ODEACTOR.h"

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
