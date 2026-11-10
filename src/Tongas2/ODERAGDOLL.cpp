// Tongas2/ODERAGDOLL -- the ODE ragdoll resources.
//
// Discovery scaffold: bodies are empty until the unit is matched; each
// function has its exact target size in config/symbols.txt. _BuildDoll's
// extent includes its switch tables.
#include "Tongas2/ODERAGDOLL.h"

void ODERAGDOLLMASTER::RegisterCommands()
{
}

void ODERAGDOLLRESOURCE::_BuildDoll()
{
}

void *CreateODERAGDOLL()
{
    return 0;
}

class GD54dcf0
{
public:
    virtual ~GD54dcf0();
};

__declspec(noinline) GD54dcf0::~GD54dcf0()
{
}

class GD54de10
{
public:
    virtual ~GD54de10();
};

__declspec(noinline) GD54de10::~GD54de10()
{
}

class G2RDBASE2
{
public:
    void Cleanup();
    virtual void V00();
    virtual void V01();
    virtual void V02();
    virtual void V03();
    virtual void V04();
    virtual void V05();
    virtual void V06();
    virtual void V07();
    virtual void V08();
    virtual void V09();
    virtual void V10();
    virtual void V11();
    virtual void V12();
    virtual void V13();
    virtual void V14();
    virtual void V15();
    virtual void V16();
    virtual void V17();
    virtual void V18();
    virtual void V19();
    virtual void V20();
};

class G2RDX : public G2RDBASE2
{
public:
    void Run();
};

void G2RDX::Run()
{
    Cleanup();
    V20();
}

extern "C" char DAT_0067cda8;
void *GetD0067cda8()
{
    return &DAT_0067cda8;
}

extern int g_classParents[];
extern char g_classNames[][30];

class ODERAGDOLLCID
{
public:
    virtual int GetClassId();
    virtual int GetParentClassId();
    virtual const char *GetClassName();

    static int s_classId;
};

int ODERAGDOLLCID::GetClassId()
{
    return s_classId;
}

int ODERAGDOLLCID::GetParentClassId()
{
    return g_classParents[s_classId];
}

const char *ODERAGDOLLCID::GetClassName()
{
    return g_classNames[s_classId];
}
