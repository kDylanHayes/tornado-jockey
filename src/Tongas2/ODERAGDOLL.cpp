// Tongas2/ODERAGDOLL -- the ODE ragdoll resources.
//
// Discovery scaffold: bodies are empty until the unit is matched; each
// function has its exact target size in config/symbols.txt. _BuildDoll's
// extent includes its switch tables.
#include "Tongas2/ODERAGDOLL.h"
#include "Tongas2/RESOURCE.h"


void ODERAGDOLLRESOURCE::_BuildDoll()
{
}

void *CreateODERAGDOLL()
{
    return new ODERAGDOLLMASTER();
}

class GD54dcf0
{
public:
    virtual ~GD54dcf0();
};

__declspec(noinline) GD54dcf0::~GD54dcf0()
{
}

// The two dynamic-array members of the ragdoll: a vtable and three pointers,
// freed with delete[] in the destructor.
class GD54d390
{
public:
    __forceinline virtual ~GD54d390()
    {
        if (m_74)
            delete[] (char *)m_74;
        m_74 = 0;
        m_7c = 0;
        m_78 = 0;
    }

    char pad_4[0x70];
    void *m_74;
    void *m_78;
    void *m_7c;
};

class GD54d3c0
{
public:
    __forceinline virtual ~GD54d3c0()
    {
        if (m_8)
            delete[] (char *)m_8;
        m_8 = 0;
        m_10 = 0;
        m_c = 0;
    }

    char pad_4[4];
    void *m_8;
    void *m_c;
    void *m_10;
};

// The ragdoll's primary base. It is 0x400 bytes; the second base starts there.
class G2RDBASE2 : public OBJECT
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

// The second base at +0x400: vtable, one word, no destructor of its own.
class RDBASEB
{
public:
    virtual void V0();
    virtual void V1();
    virtual void V2();

    int m_4;
};

class GD54de10 : public G2RDBASE2, public RDBASEB
{
public:
    virtual ~GD54de10();

    GD54d390 m_408;
    char pad_488[8];
    GD54d3c0 m_490;
};

__declspec(noinline) GD54de10::~GD54de10()
{
    Cleanup();
}

class G2RDX : public G2RDBASE2
{
public:
    void Run();
};

void G2RDX::Run()
{
    Cleanup();
    Virtual20();
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

// The exe's destructor at 0x54dcf0 leaves the vptr alone; the master is a
// member and the name is at 0x6c.
class GD54dcf0X
{
public:
    ~GD54dcf0X();

    RESOURCEMASTER m_master;
    char pad_6c[0x6c - sizeof(RESOURCEMASTER)];
    CSTRING m_6c;
};

__declspec(noinline) GD54dcf0X::~GD54dcf0X()
{
}

CMDENTRY ODERAGDOLLMASTER::s_commands[] = {
    { 0, 0, 0, 0, 0 },
};

void ODERAGDOLLMASTER::RegisterCommands()
{
    g_classParents[ODERAGDOLLCID::s_classId] = RESOURCEMASTER::s_classId;
    g_classCommands[ODERAGDOLLCID::s_classId] = s_commands;
    HashCommands(s_commands);
    RegisterClassName(s_commands, "ODERAGDOLLMASTER", "RESOURCEMASTER");
}
