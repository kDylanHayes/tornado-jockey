// Tongas2/ODERAGDOLL -- the ODE ragdoll resources.
//
// Discovery scaffold: bodies are empty until the unit is matched; each
// function has its exact target size in config/symbols.txt. _BuildDoll's
// extent includes its switch tables.
#include "Tongas2/ODERAGDOLL.h"
#include "Tongas2/RESOURCE.h"
#include "Tongas2/SCENE.h"
#include "Tongas2/LOG.h"
#include "Tongas2/MATHS.h"


// _BuildDoll is defined in full in the retail object; this unit only calls it.

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
// Element type of the ragdoll's array (0x70 bytes, constructor elsewhere).
class ODEWORLDRESET
{
public:
    ODEWORLDRESET();

    char pad[0x70];
};

class GD54d390
{
public:
    __forceinline virtual ~GD54d390()
    {
        if (m_74)
            delete[] m_74;
        m_74 = 0;
        m_7c = 0;
        m_78 = 0;
    }

    void EnsureIndex(unsigned int index);

    char pad_4[0x70];
    ODEWORLDRESET *m_74;
    unsigned int m_78;
    unsigned int m_7c;
};

class GD54d6d0;

// The 0x70-byte master the ragdoll manager makes.
class GD54d5d0 : public RESOURCEMASTER
{
public:
    GD54d5d0(RESOURCEMANAGER *manager, const char *name);

    int m_60;
    float m_64;
    float m_68;
    CSTRING m_6c;
};

GD54d5d0::GD54d5d0(RESOURCEMANAGER *manager, const char *name)
    : RESOURCEMASTER(manager, name)
{
    m_60 = 10;
    m_64 = 0.1f;
    m_68 = 0.5f;
    m_6c = "";
}

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

    GD54d5d0 *CreateMaster(const char *name);
    GD54d6d0 *CreateResource(const char *name);

    char pad_4[4];
    void *m_8;
    void *m_c;
    void *m_10;
};

GD54d5d0 *GD54d3c0::CreateMaster(const char *name)
{
    return new GD54d5d0((RESOURCEMANAGER *)this, name);
}

// The 0x4bc-byte resource the same manager makes.
class GD54d6d0
{
public:
    GD54d6d0(RESOURCEMANAGER *manager, const char *name);

    char pad[0x4bc];
};

GD54d6d0 *GD54d3c0::CreateResource(const char *name)
{
    return new GD54d6d0((RESOURCEMANAGER *)this, name);
}

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


// The actor hook the ragdoll calls after finding each actor.
class GD54de10ACTORVT
{
public:
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
    virtual void V21();
    virtual void V22();
    virtual void V23();
    virtual void V24();
    virtual void V25();
    virtual void V26();
    virtual void V27();
    virtual void V28();
    virtual void V29();
    virtual void V30();
    virtual void V31();
    virtual void V32();
    virtual void V33();
    virtual void V34();
    virtual void V35();
    virtual void V36();
    virtual void V37();
    virtual void V38();
    virtual void V39();
    virtual void V40();
    virtual void V41();
    virtual void V42();
    virtual void V43();
    virtual void V44();
    virtual void V45();
    virtual void V46();
    virtual void V47();
    virtual void V48();
    virtual void V49();
    virtual void V50();
    virtual void V51();
    virtual void V52();
    virtual void V53();
    virtual void V54();
    virtual void V55();
    virtual void V56();
    virtual void V57();
    virtual void V58();
    virtual void V59();
    virtual void V60();
    virtual void V61();
    virtual void V62();
    virtual void V63();
    virtual void V64();
    virtual void V65();
    virtual void V66();
    virtual void V67();
    virtual void V68();
    virtual void V69();
    virtual void V70();
    virtual void V71();
    virtual void V72();
    virtual void V73();
    virtual void V74();
    virtual void V75();
    virtual void V76();
    virtual void V77();
    virtual void V78();
    virtual void V79();
    virtual void V80();
    virtual void V81();
    virtual void V82();
    virtual void V83();
    virtual void V84();
    virtual void V85();
    virtual void V86();
    virtual void V87();
    virtual void V88();
    virtual void V89(int value);
};

class GD54de10 : public G2RDBASE2, public RDBASEB
{
public:
    virtual ~GD54de10();
    void Activate();
    void AddReset(int value, void *source, struct ODERAGDOLLPARAM *param, int extra);

    GD54d390 m_408;
    char pad_488[8];
    GD54d3c0 m_490;
};

__declspec(noinline) GD54de10::~GD54de10()
{
    Cleanup();
}

struct ODERAGDOLLPARAM
{
    float m_0;
    float m_4;
    float m_8;
    MATRIX m_matrix;
    char m_4c;
    char pad_4d[3];
};

struct ODERAGDOLLRESETPOD
{
    int m_0;
    int m_4;
    int m_8;
    int m_c;
    int m_10;
    V3 m_14;
    char m_20[0x50];
};

void GD54de10::AddReset(int value, void *source, ODERAGDOLLPARAM *param, int extra)
{
    if (g_pLog) {
        if (param->m_4c & 2) {
            param->m_matrix.ToEuler(&param->m_8, &param->m_4, &param->m_0);
            param->m_4c &= ~2;
        }
        if (param->m_4c & 2) {
            param->m_matrix.ToEuler(&param->m_8, &param->m_4, &param->m_0);
            param->m_4c &= ~2;
        }
        if (param->m_4c & 2) {
            param->m_matrix.ToEuler(&param->m_8, &param->m_4, &param->m_0);
            param->m_4c &= ~2;
        }
    }

    ODERAGDOLLRESETPOD entry;
    entry.m_0 = -1;
    entry.m_4 = -1;
    entry.m_8 = 0;
    entry.m_c = value;
    entry.m_10 = extra;
    entry.m_14 = *(V3 *)source;
    memcpy(entry.m_20, param, 0x50);

    GD54d390 *list = &m_408;
    if (list->m_78 + 1 > list->m_7c)
        list->EnsureIndex(list->m_7c * 2 + 1);
    memcpy(&list->m_74[list->m_78], &entry, 0x70);
    list->m_78++;
}

void GD54de10::Activate()
{
    ((ODERAGDOLLRESOURCE *)this)->_BuildDoll();
    int count = (int)m_408.m_78;
    if (count <= 0)
        return;
    GD54d390 *list = &m_408;
    unsigned int offset = 0;
    for (unsigned int i = 1; count > 0; i++, offset += 0x70, count--) {
        if (i > list->m_78) {
            list->EnsureIndex(i);
            if (i <= list->m_7c)
                list->m_78 = i;
            else
                list->m_78 = list->m_7c;
        }
        RESHANDLE handle = *(RESHANDLE *)((char *)list->m_74 + offset);
        WTACTOR *actor = g_pScene->GetActor(handle);
        if (actor && !*(bool *)((char *)actor + 0x9a0))
            ((GD54de10ACTORVT *)actor)->V89(1);
    }
}

class G2RDX : public G2RDBASE2
{
public:
    void Run();
};

void GD54d390::EnsureIndex(unsigned int index)
{
    if (index <= m_7c)
        return;
    ODEWORLDRESET *data = new ODEWORLDRESET[index];
    if (m_74) {
        if (m_78 > 0)
            memcpy(data, m_74, m_78 * sizeof(ODEWORLDRESET));
        delete[] m_74;
    }
    m_74 = data;
    m_7c = index;
}

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
