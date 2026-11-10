// Tongas2/ODEGEOM -- the ODE geometry resources.
//
// The unit is being filled in function by function; each function has its
// exact target size in config/symbols.txt.
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

// The ODE geometry resource: an object whose collision shape can show a
// helper model. GD549b40 adds the fields the ODE geometry keeps past it.
class ODEHELPNODE : public OBJECT
{
public:
    void ShowHelpers(bool *visible);

    // The helper model shown while the collision shape is.
    RESHANDLE m_helper;
    char pad_40c[0x18];
    bool m_helpersShown;
    char pad_425[3];
    void *m_428;
    void *m_42c;
    char pad_430[4];
    void *m_434;
    void *m_438;
    // The custom trimesh: its faces (three indices each), and its vertices.
    void *m_indices;
    int m_numFaces;
    void *m_vertices;
    int m_numVertices;
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
            ODEHELPNODE *next = (ODEHELPNODE *)node->m_next;
            if (node->m_active)
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

    // The colour the helpers are drawn in unless the actor colours them.
    V3I &GetColor() { return *(V3I *)((char *)this + 0x6c); }

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

class GD549b40 : public ODEHELPNODE
{
public:
    virtual ~GD549b40() throw();

    void Cleanup();
    void ShowHelpers(int arg);
    void AddToSpace(void *space);
    bool IsAtOrigin();

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
    if (m_vertices) {
        delete[] (char *)m_vertices;
        m_vertices = 0;
    }
    if (m_indices) {
        delete[] (char *)m_indices;
        m_indices = 0;
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


extern RESOURCEMANAGER *g_pSIMPLEMODEL;

// The primitive types, in the order s_typeNames names them. The ray has no
// helper.
enum
{
    ODEGEOM_BOX,
    ODEGEOM_SPHERE,
    ODEGEOM_PLANE,
    ODEGEOM_CAPPEDCYLINDER,
    ODEGEOM_RAY,
    ODEGEOM_TRIMESH,
    ODEGEOM_NUMTYPES
};

// The primitive types' names for the log, and for the master's
// PrimitiveType property.
static const char s_typeNames[][30] = {
    "BOX",
    "SPHERE",
    "PLANE",
    "CAPPEDCYLINDER",
    "RAY",
    "TRIMESH",
    "",
};

// A type's name for the log.
static inline const char *TypeName(int type)
{
    return type >= 0 && type < ODEGEOM_NUMTYPES ? s_typeNames[type] : "Not valid primitive";
}

// A helper model: a copy of an engine simple model. Only the methods the
// geometry helpers use are declared here; the engine class has one more
// virtual after them.
class ODEHELPMODEL : public OBJECT
{
public:
    virtual void SetColor(V3I color);
    virtual void SetCustomData(void *vertices, int numVertices, void *indices, int numFaces);
    virtual void Virtual72(int a, int b, int c, int d) {}

    V3 m_pivot;
    V3 m_40c;
};

// Defined elsewhere: the simple models' master, which makes their copies.
class ODEHELPMODELMASTER : public RESOURCEMASTER
{
public:
    virtual RESOURCE *CreateCopy(const RESHANDLE &handle);
};

// Defined in ODEWORLD: turns each face of the custom trimesh the other way
// round for ODE.
struct G2ODESWAP
{
    void Swap();
};

void ODEHELPNODE::ShowHelpers(bool *visible)
{
    WTACTOR *actor = g_pScene->GetActor(m_parent);
    bool show = *visible;
    if (actor)
        show &= actor->m_a8;
    if (show) {
        if (m_helpersShown)
            return;
        ODEHELPMODEL *helper;
        ODEGEOMRESOURCEMASTERX *master = (ODEGEOMRESOURCEMASTERX *)m_master;
        switch (master ? master->m_60 : -1) {
        case ODEGEOM_SPHERE:
            helper = (ODEHELPMODEL *)g_pSIMPLEMODEL->GetResource("sph");
            helper->SetScaleFactor(GetSize().x, GetSize().x, GetSize().x);
            break;
        case ODEGEOM_PLANE:
            helper = (ODEHELPMODEL *)g_pSIMPLEMODEL->GetResource("box");
            helper->SetScaleFactor(100.0f, 0.01f, 100.0f);
            break;
        case ODEGEOM_BOX:
            helper = (ODEHELPMODEL *)g_pSIMPLEMODEL->GetResource("box");
            helper->SetScaleFactorV(GetSize());
            break;
        case ODEGEOM_CAPPEDCYLINDER:
            helper = (ODEHELPMODEL *)g_pSIMPLEMODEL->GetResource("cylinderZ");
            helper->SetScaleFactorV(V3(GetSize().x * 2.0f, GetSize().x * 2.0f,
                                       GetSize().z * 2.0f));
            break;
        case ODEGEOM_TRIMESH:
            helper = (ODEHELPMODEL *)g_pSIMPLEMODEL->GetResource("custom");
            ((G2ODESWAP *)this)->Swap();
            helper->SetCustomData(m_vertices, m_numVertices, m_indices,
                                  m_numFaces);
            ((G2ODESWAP *)this)->Swap();
            break;
        default:
            if (g_pLog)
                g_pLog->Error("Error - ODEGEOMRESOURCE::ShowHelpers> Primitive %s not supported",
                              TypeName(master->m_60));
            return;
        }
        V3I color;
        if (actor && actor->m_98)
            color = actor->m_9c;
        else
            color = ((ODEGEOMRESOURCEMASTERX *)m_master)->GetColor();
        helper->SetColor(color);
        helper->SetAlphaFactor(0.5f);
        helper->SetOffsetV(GetOffset());
        helper->SetRelAngles(GetRelAngles());
        helper->SetParent(m_parent);
        helper->m_a8 = false;
        helper->SetRelLayer(125);
        WTACTOR *attached = (WTACTOR *)g_pResources->GetResource(m_attached);
        if (attached)
            helper->AttachTo(attached, m_tagName);
        helper->SetActive(true);
        m_helper = helper->m_handle;
        m_helpersShown = true;
    } else if (m_helpersShown) {
        RESOURCE *helper = g_pResources->GetResource(m_helper);
        if (helper)
            helper->Virtual20();
        m_helper.Invalidate();
        m_helpersShown = false;
    }
}
