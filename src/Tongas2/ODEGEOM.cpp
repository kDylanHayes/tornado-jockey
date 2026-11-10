// Tongas2/ODEGEOM -- the ODE geometry resources.
//
// Discovery scaffold: bodies are empty until the unit is matched; each
// function has its exact target size in config/symbols.txt.
#include "Tongas2/ODEGEOM.h"
#include "Tongas2/MATHS.h"

void ODEGEOMMASTER::RegisterCommands()
{
}

void DYNAMICARRAY::Insert(void *item)
{
    (void)item;
}

void *CreateODEGEOM()
{
    return 0;
}

class GD548fc0
{
public:
    virtual ~GD548fc0();
};

__declspec(noinline) GD548fc0::~GD548fc0()
{
}

class GD548f90
{
public:
    virtual ~GD548f90();
};

__declspec(noinline) GD548f90::~GD548f90()
{
}

class GD549b40
{
public:
    virtual ~GD549b40();
};

__declspec(noinline) GD549b40::~GD549b40()
{
}

extern "C" char DAT_0067c1dc;
void *GetD0067c1dc()
{
    return &DAT_0067c1dc;
}

extern int g_classParents[];
extern char g_classNames[][30];

class ODEGEOMCID
{
public:
    virtual int GetClassId();
    virtual int GetParentClassId();
    virtual const char *GetClassName();
    void Set6C(const V3 &value);

    static int s_classId;
};

int ODEGEOMCID::GetClassId()
{
    return s_classId;
}

int ODEGEOMCID::GetParentClassId()
{
    return g_classParents[s_classId];
}

const char *ODEGEOMCID::GetClassName()
{
    return g_classNames[s_classId];
}

void ODEGEOMCID::Set6C(const V3 &value)
{
    *(V3 *)((char *)this + 0x6c) = value;
}
