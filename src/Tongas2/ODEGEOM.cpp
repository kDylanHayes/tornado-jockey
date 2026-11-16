// Tongas2/ODEGEOM -- the ODE geometry resources.
//
// The unit is being filled in function by function; each function has its
// exact target size in config/symbols.txt.
#define MATRIX_TRANSPOSE_OUTOFLINE

#include "Tongas2/ODEGEOM.h"
#include "Tongas2/MATHS.h"
#include "Tongas2/DYNAMICARRAY.h"
#include "Tongas2/RESOURCES.h"

class GD549b40;

// An item of the helper table: -1, -1 then zeros.
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

// The helper table of one geometry: a DYNAMICARRAY of G2ODEINIT items. Its
// items are the helper models the geometry shows while its shape is.
class GD548fc0
{
public:
    GD548fc0();
    virtual ~GD548fc0() throw();

    // The items, the slots given back, and the slots in use.
    char *m_4;
    char *m_8;
    char *m_c;
    int m_capacity;
    int m_count;
    int m_freeTop;
    int m_next;
    G2ODEINIT m_empty;
    int m_growBy;
};

GD548fc0::GD548fc0()
{
    m_4 = 0;
    m_8 = 0;
    m_c = 0;
    m_capacity = 0;
    m_growBy = 0;
    ((DYNAMICARRAY<G2ODEINIT> *)this)->Clear();
}

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
    bool TestGeomCollision(GD549b40 *a, GD549b40 *b, const V3 &dir,
                           const V3 &scale, int value);

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
    RESHANDLE m_40c;
    RESHANDLE m_418;
    bool m_helpersShown;
    bool m_425;
    char pad_426[2];
    void *m_428;
    void *m_42c;
    bool m_430;
    bool m_431;
    char pad_432[2];
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
    virtual RESOURCE *CreateCopy(const RESHANDLE &handle);

    V3I &GetColor() { return m_color; }

    int m_60;
    int m_64;
    int m_68;
    V3I m_color;
    float m_78;
    float m_7c;
    float m_80;
    float m_84;
    float m_88;
};

ODEGEOMRESOURCEMASTERX::ODEGEOMRESOURCEMASTERX(RESOURCEMANAGER *manager, const char *name)
    : RESOURCEMASTER(manager, name)
{
    m_60 = 0;
    m_64 = 0;
    m_68 = 0;
    m_color = V3I(0xff, 100, 200);
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
extern "C" void dGeomDisable(void *geom);
extern "C" void *dGeomGetSpace(void *geom);
extern "C" void dSpaceRemove(void *space, void *geom);

extern "C" void *dCreateBox(void *space, float x, float y, float z);
extern "C" void *dCreateSphere(void *space, float radius);
extern "C" void *dCreatePlane(void *space, float a, float b, float c, float d);
extern "C" void *dCreateCCylinder(void *space, float radius, float length);
extern "C" void *dCreateRay(void *space, float length);
extern "C" void *dCreateTriMesh(void *space, void *data, void *callback,
                                void *arrayCallback, void *rayCallback);
extern "C" void dGeomSetCategoryBits(void *geom, int bits);
extern "C" void dGeomSetCollideBits(void *geom, int bits);
extern "C" void dGeomSetData(void *geom, void *data);
extern "C" void *dCreateGeomTransform(int space);
extern "C" void dGeomTransformSetGeom(void *transform, void *geom);
extern "C" void dGeomTransformSetInfo(void *transform, int info);
extern "C" void dGeomSetPosition(void *geom, float x, float y, float z);
extern "C" void dGeomSetRotation(void *geom, const void *matrix);

// The geometry object itself, as the ODE world sees it.
class ODEGEOMRESOURCE
{
public:
    void RemoveGeom();
};

class ODEWORLDFIND
{
public:
    WTACTOR *FindActor();
};


// The default collision shapes the geometry falls back on, and the trimesh.
extern int DAT_006c6358;
extern int DAT_006c63a8;
extern int DAT_006c631c[];
extern int DAT_006c6360[];

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

class GD549b40 : public OBJECT
{
public:
    GD549b40(RESOURCEMASTER *master, const RESHANDLE &handle);
    virtual ~GD549b40() throw();

    void Cleanup();
    void Build(int vertexCount, const void *vertices, int triangleCount,
               const void *indices, bool swap);
    void AddHelper(void *source, const V3 &dir, const V3 &scale, int value);
    void ShowHelpers(int arg);
    void AddToSpace(void *space);
    bool IsAtOrigin();
    void UpdateGeom();
    virtual bool IsLoaded() { return true; }
    virtual V3 GetScaleFactor() { return GetScale(); }

    // The same fields ODEHELPNODE gives this object.
    RESHANDLE m_handles[3];
    bool m_helpersShown;
    bool m_425;
    char pad_426[2];
    void *m_428;
    void *m_42c;
    bool m_430;
    bool m_431;
    char pad_432[2];
    void *m_434;
    void *m_438;
    void *m_indices;
    int m_numFaces;
    void *m_vertices;
    int m_numVertices;
    bool m_44c;
    char pad_44d[3];
    GD548fc0 m_holder;
    bool m_49c;
};

GD549b40::GD549b40(RESOURCEMASTER *master, const RESHANDLE &handle)
    : OBJECT(master, handle)
{
    m_helpersShown = false;
    m_428 = 0;
    m_42c = 0;
    m_431 = false;
    m_indices = 0;
    m_vertices = 0;
    m_434 = 0;
    m_438 = 0;
    m_44c = false;
    m_49c = false;
    if (!m_master)
        return;
    int type = ((ODEGEOMRESOURCEMASTERX *)m_master)->m_60;
    switch (type) {
    case ODEGEOM_BOX:
        m_428 = dCreateBox(0, 1.0f, 1.0f, 1.0f);
        break;
    case ODEGEOM_SPHERE:
        m_428 = dCreateSphere(0, 0.5f);
        break;
    case ODEGEOM_PLANE:
        m_428 = dCreatePlane(0, 0.0f, 0.0f, 0.0f, 0.0f);
        break;
    case ODEGEOM_CAPPEDCYLINDER:
        m_428 = dCreateCCylinder(0, 0.5f, 1.0f);
        break;
    case ODEGEOM_RAY:
        m_428 = dCreateRay(0, 1.0f);
        break;
    case ODEGEOM_TRIMESH:
        Build(DAT_006c6358, DAT_006c631c, DAT_006c63a8, DAT_006c6360, false);
        m_438 = m_434;
        m_428 = dCreateTriMesh(0, m_434, 0, 0, 0);
        break;
    default:
        if (g_pLog)
            g_pLog->Error("Error - ODEGEOMRESOURCE> Primitive %s not supported for attaching to actor",
                          TypeName(type));
        break;
    }
    if (m_428) {
        dGeomDisable(m_428);
        dGeomSetCategoryBits(m_428, ((ODEGEOMRESOURCEMASTERX *)master)->m_64);
        dGeomSetCollideBits(m_428, ((ODEGEOMRESOURCEMASTERX *)master)->m_68);
        dGeomSetData(m_428, this);
    }
}

RESOURCE *ODEGEOMRESOURCEMASTERX::CreateCopy(const RESHANDLE &handle)
{
    return new GD549b40(this, handle);
}

// The ODE actor's collision test, at slot 141 (byte offset 0x234) of the
// actor table. Only the one virtual this unit calls is named.
class ODEACTORX
{
public:
    virtual void V00(); virtual void V01(); virtual void V02(); virtual void V03(); virtual void V04(); virtual void V05(); virtual void V06(); virtual void V07();
    virtual void V08(); virtual void V09(); virtual void V10(); virtual void V11(); virtual void V12(); virtual void V13(); virtual void V14(); virtual void V15();
    virtual void V16(); virtual void V17(); virtual void V18(); virtual void V19(); virtual void V20(); virtual void V21(); virtual void V22(); virtual void V23();
    virtual void V24(); virtual void V25(); virtual void V26(); virtual void V27(); virtual void V28(); virtual void V29(); virtual void V30(); virtual void V31();
    virtual void V32(); virtual void V33(); virtual void V34(); virtual void V35(); virtual void V36(); virtual void V37(); virtual void V38(); virtual void V39();
    virtual void V40(); virtual void V41(); virtual void V42(); virtual void V43(); virtual void V44(); virtual void V45(); virtual void V46(); virtual void V47();
    virtual void V48(); virtual void V49(); virtual void V50(); virtual void V51(); virtual void V52(); virtual void V53(); virtual void V54(); virtual void V55();
    virtual void V56(); virtual void V57(); virtual void V58(); virtual void V59(); virtual void V60(); virtual void V61(); virtual void V62(); virtual void V63();
    virtual void V64(); virtual void V65(); virtual void V66(); virtual void V67(); virtual void V68(); virtual void V69(); virtual void V70(); virtual void V71();
    virtual void V72(); virtual void V73(); virtual void V74(); virtual void V75(); virtual void V76(); virtual void V77(); virtual void V78(); virtual void V79();
    virtual void V80(); virtual void V81(); virtual void V82(); virtual void V83(); virtual void V84(); virtual void V85(); virtual void V86(); virtual void V87();
    virtual void V88(); virtual void V89(); virtual void V90(); virtual void V91(); virtual void V92(); virtual void V93(); virtual void V94(); virtual void V95();
    virtual void V96(); virtual void V97(); virtual void V98(); virtual void V99(); virtual void V100(); virtual void V101(); virtual void V102(); virtual void V103();
    virtual void V104(); virtual void V105(); virtual void V106(); virtual void V107(); virtual void V108(); virtual void V109(); virtual void V110(); virtual void V111();
    virtual void V112(); virtual void V113(); virtual void V114(); virtual void V115(); virtual void V116(); virtual void V117(); virtual void V118(); virtual void V119();
    virtual void V120(); virtual void V121(); virtual void V122(); virtual void V123(); virtual void V124(); virtual void V125(); virtual void V126(); virtual void V127();
    virtual void V128(); virtual void V129(); virtual void V130(); virtual void V131(); virtual void V132(); virtual void V133(); virtual void V134(); virtual void V135();
    virtual void V136(); virtual void V137(); virtual void V138(); virtual void V139(); virtual void V140();
    virtual bool TestGeom(GD549b40 *other, const V3 &dir, const V3 &scale,
                          int value);
};

bool GD548f90::TestGeomCollision(GD549b40 *a, GD549b40 *b, const V3 &dir,
                                 const V3 &scale, int value)
{
    if (!a || !b)
        return false;
    bool same = false;
    if (a->GetParent() == b->GetParent())
        same = true;
    V3 neg = -1.0f * scale;
    WTACTOR *actor = ((ODEWORLDFIND *)a)->FindActor();
    if (actor) {
        if (!((ODEACTORX *)actor)->TestGeom(b, dir, scale, value))
            return false;
    }
    actor = ((ODEWORLDFIND *)b)->FindActor();
    if (actor) {
        if (!((ODEACTORX *)actor)->TestGeom(a, dir, neg, value))
            return false;
    }
    if (!same || a->m_49c)
        a->AddHelper(b, dir, scale, value);
    if (!same || b->m_49c)
        b->AddHelper(a, dir, neg, value);
    return true;
}

__declspec(noinline) GD549b40::~GD549b40() throw()
{
    if (m_42c)
        dGeomDestroy(m_42c);
    Cleanup();
    dGeomDestroy(m_428);
    if (m_438)
        dGeomTriMeshDataDestroy(m_438);
}

void GD549b40::ShowHelpers(int arg)
{
    m_44c = true;
    DYNAMICARRAY<G2ODEINIT> *array = (DYNAMICARRAY<G2ODEINIT> *)((char *)this + 0x450);
    array->Clear();
    array->Resize(arg);
}

void GD549b40::UpdateGeom()
{
    if (!m_431)
        return;
    m_431 = false;
    if (!IsAtOrigin()) {
        if (m_42c) {
            ((ODEGEOMRESOURCE *)this)->RemoveGeom();
            dGeomDestroy(m_42c);
            m_42c = 0;
            AddToSpace(*(void **)((char *)g_pOdeWorldX + 0x60));
        }
    } else if (!m_42c) {
        ((ODEGEOMRESOURCE *)this)->RemoveGeom();
        RESOURCEMASTER *master = m_master;
        m_42c = dCreateGeomTransform(0);
        dGeomTransformSetGeom(m_42c, m_428);
        dGeomTransformSetInfo(m_42c, 1);
        dGeomSetData(m_42c, this);
        dGeomSetCategoryBits(m_42c, ((ODEGEOMRESOURCEMASTERX *)master)->m_64);
        dGeomSetCollideBits(m_42c, ((ODEGEOMRESOURCEMASTERX *)master)->m_68);
        AddToSpace(*(void **)((char *)g_pOdeWorldX + 0x60));
    }
    if (!m_42c)
        return;
    WTACTOR *actor = ((ODEWORLDFIND *)this)->FindActor();
    if (!actor)
        return;
    V3 position = m_offset * actor->GetWorldScale();
    dGeomSetPosition(m_428, position.x, position.y, position.z);
    float matrix[12];
    m_relAngles.GetMatrix().Transpose((MATRIX *)matrix);
    dGeomSetRotation(m_428, matrix);
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
    char pad_3c[4];
    ODEHELPNODE *m_40;

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

void ODEHELPNODE2::Visit(int arg)
{
    ODEHELPNODE *node = m_40;
    while (node) {
        ODEHELPNODE *next = (ODEHELPNODE *)node->m_next;
        if (node->m_active) {
            if (arg == 0) {
                void *geom = node->m_42c ? node->m_42c : node->m_428;
                dGeomSetBody(geom, 0);
                dGeomDisable(geom);
                dSpaceRemove(dGeomGetSpace(geom), geom);
                --*(int *)((char *)g_pActorResourceManager2 + 0x228);
            } else {
                ((GD549b40 *)node)->AddToSpace((void *)arg);
            }
        }
        node = next;
    }
}

__declspec(noinline) void GD549b40::AddToSpace(void *space)
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
    if (m_offset.x != 0.0f || m_offset.y != 0.0f || m_offset.z != 0.0f ||
        !m_relAngles.IsZero()) {
        WTACTOR *actor = ((ODEWORLDFIND *)this)->FindActor();
        if (actor)
            return true;
    }
    return false;
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
