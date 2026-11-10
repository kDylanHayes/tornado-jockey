// Tongas2/ODEGEOM -- the ODE geometry resources.
//
// Discovery scaffold: bodies are empty until the unit is matched; each
// function has its exact target size in config/symbols.txt.
#include "Tongas2/ODEGEOM.h"
#include "Tongas2/MATHS.h"
#include "Tongas2/DYNAMICARRAY.h"
#include "Tongas2/RESOURCES.h"

class GD548fc0
{
public:
    virtual ~GD548fc0() throw();

    char *m_4;
    char *m_8;
    char *m_c;
};

__declspec(noinline) GD548fc0::~GD548fc0() throw()
{
    if (m_c) {
        delete[] m_c;
        m_c = 0;
    }
    if (m_8) {
        delete[] m_8;
        m_8 = 0;
    }
    if (m_4) {
        delete[] m_4;
        m_4 = 0;
    }
}

extern "C" void dCloseODE();
int SafeSprintf(char *buffer, const char *format, ...);

class GD548f90 : public RESOURCEMANAGER
{
public:
    GD548f90()
    {
        SafeSprintf(m_fileName, "ODEGEOM.txt");
        m_8 = 0;
        m_18 = 1;
    }

    virtual RESOURCEMASTER *CreateMaster(const char *name);
    virtual const char *GetTypeCode();
    virtual ~GD548f90();
    virtual void Update();
    virtual void Init();

    void ToggleHelpers();

    bool m_224;
    int m_228;
};

extern void *g_pOdeWorldX;
extern RESOURCEMANAGER *g_pCPRIMITIVE3;

class ODEWORLDX
{
public:
    void Reset();
};

void GD548f90::Init()
{
    RESOURCEMANAGER::Init();
    ((ODEWORLDX *)g_pOdeWorldX)->Reset();
    m_224 = false;
    m_228 = 0;
}

void GD548f90::Update()
{
    RESOURCEMANAGER::Update();
    if (*(bool *)((char *)g_pCPRIMITIVE3 + 0x274) != m_224)
        ToggleHelpers();
}

struct ODEHELPNODE
{
    char pad[0x1c];
    bool m_1c;
    char pad2[3];
    ODEHELPNODE *m_20;

    void ShowHelpers(bool *visible);
};

struct ODEMASTERNODE
{
    char pad[0x38];
    ODEMASTERNODE *m_38;
    char pad2[4];
    ODEHELPNODE *m_40;
};

void GD548f90::ToggleHelpers()
{
    m_224 = !m_224;
    ODEMASTERNODE *master = *(ODEMASTERNODE **)((char *)this + 0xc);
    while (master) {
        ODEMASTERNODE *nextMaster = master->m_38;
        ODEHELPNODE *node = master->m_40;
        while (node) {
            ODEHELPNODE *next = node->m_20;
            if (node->m_1c)
                node->ShowHelpers(&m_224);
            node = next;
        }
        master = nextMaster;
    }
}

__declspec(noinline) GD548f90::~GD548f90()
{
    dCloseODE();
}

void *CreateODEGEOM()
{
    return new GD548f90;
}

class ODEGEOMRESOURCEMASTERX : public RESOURCEMASTER
{
public:
    ODEGEOMRESOURCEMASTERX(RESOURCEMANAGER *manager, const char *name);
    virtual SRESULT Execute(CMDENTRY *command, TOKENS **args);

    int m_60;
    int m_64;
    int m_68;
    int m_6c;
    int m_70;
    int m_74;
    float m_78;
    float m_7c;
    float m_80;
    float m_84;
    float m_88;
};

ODEGEOMRESOURCEMASTERX::ODEGEOMRESOURCEMASTERX(RESOURCEMANAGER *manager, const char *name)
    : RESOURCEMASTER(manager, name)
{
    m_74 = m_70 = m_6c = 0;
    m_60 = m_64 = m_68 = 0;
    m_6c = 0xff;
    m_70 = 100;
    m_74 = 200;
    m_78 = 8000.0f;
    m_7c = 0.3f;
    m_80 = 0.05f;
    m_84 = 0.2f;
    m_88 = 0.0001f;
}

RESOURCEMASTER *GD548f90::CreateMaster(const char *name)
{
    return new ODEGEOMRESOURCEMASTERX(this, name);
}

SRESULT ODEGEOMRESOURCEMASTERX::Execute(CMDENTRY *command, TOKENS **args)
{
    return (this->*(SRESULT (ODEGEOMRESOURCEMASTERX::*)(TOKENS **))command->func)(args);
}


extern "C" void dGeomSetBody(void *geom, void *body);
extern "C" void dSpaceAdd(void *space, void *geom);
extern "C" void dGeomEnable(void *geom);
extern RESOURCEMANAGER *g_pActorResourceManager2;

extern "C" void dGeomDestroy(void *geom);
extern "C" void dGeomTriMeshDataDestroy(void *data);
extern "C" void dGeomTriMeshClearTCCache(void *geom);
extern "C" void dGeomTriMeshSetData(void *geom, void *data);

class GD549b40 : public OBJECT
{
public:
    virtual ~GD549b40() throw();

    void Cleanup();
    void ShowHelpers(int arg);
    void AddToSpace(void *space);
    bool IsAtOrigin();

    char pad_400[0x28];
    void *m_428;
    void *m_42c;
    char pad_430[4];
    void *m_434;
    void *m_438;
    void *m_43c;
    char pad_440[4];
    void *m_444;
    char pad_448[4];
    bool m_44c;
    char pad_44d[3];
    GD548fc0 m_holder;
};

__declspec(noinline) GD549b40::~GD549b40() throw()
{
    if (m_42c)
        dGeomDestroy(m_42c);
    Cleanup();
    dGeomDestroy(m_428);
    if (m_438)
        dGeomTriMeshDataDestroy(m_438);
}

struct G2ODEINIT
{
    G2ODEINIT();
    int m_00;
    int m_04;
    int m_08;
    int m_0c;
    int m_10;
    int m_14;
    int m_18;
    int m_1c;
    int m_20;
    int m_24;
};

G2ODEINIT::G2ODEINIT()
{
    m_00 = -1;
    m_04 = -1;
    m_08 = 0;
    m_0c = 0;
    m_10 = 0;
    m_14 = 0;
    m_18 = 0;
    m_1c = 0;
    m_20 = 0;
}

void GD549b40::ShowHelpers(int arg)
{
    m_44c = true;
    DYNAMICARRAY<G2ODEINIT> *array = (DYNAMICARRAY<G2ODEINIT> *)((char *)this + 0x450);
    array->Clear();
    array->Resize(arg);
}

void GD549b40::Cleanup()
{
    if (!m_master)
        return;
    if (((ODEGEOMRESOURCEMASTERX *)m_master)->m_60 != 5)
        return;
    if (m_434 && m_434 != m_438) {
        dGeomTriMeshClearTCCache(m_428);
        dGeomTriMeshSetData(m_428, m_438);
        dGeomTriMeshDataDestroy(m_434);
        m_434 = 0;
    }
    if (m_444) {
        delete[] (char *)m_444;
        m_444 = 0;
    }
    if (m_43c) {
        delete[] (char *)m_43c;
        m_43c = 0;
    }
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

void RegisterClassName(CMDENTRY *commands, const char *type, const char *parent);

CMDENTRY ODEGEOMMASTER::s_commands[] = {
    { 0, 0, 0, 0, 0 },
};

void ODEGEOMMASTER::RegisterCommands()
{
    g_classParents[ODEGEOMCID::s_classId] = RESOURCEMASTER::s_classId;
    g_classCommands[ODEGEOMCID::s_classId] = s_commands;
    HashCommands(s_commands);
    RegisterClassName(s_commands, "ODEGEOMMASTER", "RESOURCEMASTER");
}

class ODEHELPNODE2
{
public:
    char pad[0x38];
    ODEHELPNODE2 *m_38;

    void Visit(int arg);
};

class ODELISTX
{
public:
    char pad[0xc];
    ODEHELPNODE2 *m_c;

    void VisitAll(int arg);
};

void ODELISTX::VisitAll(int arg)
{
    ODEHELPNODE2 *node = m_c;
    while (node) {
        ODEHELPNODE2 *next = node->m_38;
        node->Visit(arg);
        node = next;
    }
}

class ODEWORLDFIND
{
public:
    WTACTOR *FindActor();
};

void GD549b40::AddToSpace(void *space)
{
    void *geom = m_42c ? m_42c : m_428;
    dSpaceAdd(space, geom);
    dGeomEnable(geom);
    ++*(int *)((char *)g_pActorResourceManager2 + 0x228);
    WTACTOR *actor = ((ODEWORLDFIND *)this)->FindActor();
    if (actor)
        dGeomSetBody(geom, *(void **)((char *)actor + 0xccc));
}

bool GD549b40::IsAtOrigin()
{
    if (m_offset.x != 0.0f)
        return false;
    if (m_offset.y != 0.0f)
        return false;
    if (m_offset.z != 0.0f)
        return false;
    if (m_relAngles.IsZero())
        return false;
    return ((ODEWORLDFIND *)this)->FindActor() != 0;
}

class MATB
{
public:
    void Build(const V3 &position, const MATRIX &matrix);

    MATRIX m;
};

void MATB::Build(const V3 &position, const MATRIX &matrix)
{
    m._11 = matrix._11;
    m._12 = matrix._21;
    m._13 = matrix._31;
    m._14 = 0.0f;
    m._21 = matrix._12;
    m._22 = matrix._22;
    m._23 = matrix._32;
    m._24 = 0.0f;
    m._31 = matrix._13;
    m._32 = matrix._23;
    m._33 = matrix._33;
    m._34 = 0.0f;
    m._41 = position.x;
    m._42 = position.y;
    m._43 = position.z;
    m._44 = 1.0f;
}
