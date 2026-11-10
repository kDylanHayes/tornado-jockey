// Tongas2/ODEACTOR -- the ODE actor object.
//
// Discovery scaffold: bodies are empty until the unit is matched.
#define DYNAMICARRAY_REMOVE
#include "Tongas2/ODEACTOR.h"
#include "Tongas2/ACTOR.h"
#include "Tongas2/CMDBASE.h"
#include "Tongas2/DYNAMICARRAY.h"
#include "Tongas2/RESOURCES.h"
#include "Tongas2/SCENE.h"
#include "Tongas2/LOG.h"

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


void ODEACTOR::SetFlag(const char *flag)
{
    typedef void (__fastcall *FLAGFN)(void *);
    if (*(bool *)((char *)this + 0x9a0)) {
        if (*flag)
            ((FLAGFN)(*(void ***)this)[0x238 / 4])(this);
        else
            ((FLAGFN)(*(void ***)this)[0x23c / 4])(this);
    }
    *(char *)((char *)this + 0xd24) = *flag;
}

class GD54c7b0
{
public:
    virtual ~GD54c7b0() throw();

    char *m_4;
    char *m_8;
    char *m_c;
};

__declspec(noinline) GD54c7b0::~GD54c7b0() throw()
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

class GD54c9b0
{
public:
    virtual ~GD54c9b0() throw();

    char *m_4;
    char *m_8;
    char *m_c;
};

__declspec(noinline) GD54c9b0::~GD54c9b0() throw()
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

extern "C" void _dSpaceDestroy(void *space);
extern "C" void _dJointGroupDestroy(void *group);
extern "C" void _dWorldDestroy(void *world);

class GD54cb00
{
public:
    virtual ~GD54cb00() throw();

    GD54c7b0 m_4;
    char pad_14[0x20];
    GD54c9b0 m_34;
    char pad_44[0x18];
    void *m_5c;
    void *m_60;
    void *m_64;
};

__declspec(noinline) GD54cb00::~GD54cb00() throw()
{
    _dSpaceDestroy(m_60);
    _dJointGroupDestroy(m_64);
    _dWorldDestroy(m_5c);
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

__declspec(noinline) void G2ODEADJ::Call(void *arg)
{
    ((G2ODEADJTGT *)((char *)this + 4))->Call((char *)arg + 8);
}

// A dMass, as the ODE library lays it out: mass, centre of gravity, inertia.
struct G2ODEMASS
{
    int m_words[0x11];

    G2ODEMASS();
};

extern "C" void dMassSetZero(G2ODEMASS *mass);

G2ODEMASS::G2ODEMASS()
{
    dMassSetZero(this);
}

class G2ODEACTOR : public ACTOR
{
public:
    // Out of line at 0x0054aff0.
    G2ODEACTOR();
    virtual ~G2ODEACTOR();

    virtual int GetClassId();
    virtual int GetParentClassId();
    virtual const char *GetClassName();

    void AddForce(float x, float y, float z);

    static int s_classId;
    static int s_factoryId;

    void *m_body;       // 0xccc
    G2ODEMASS m_mass;   // 0xcd0
    bool m_d14;         // 0xd14
    float m_d18;        // 0xd18
    bool m_d1c;         // 0xd1c
    CSTRING m_name;     // 0xd20
    bool m_d24;         // 0xd24
    bool m_d25;         // 0xd25
};

G2ODEACTOR::G2ODEACTOR()
{
    m_d18 = 1.0f;
    m_d1c = true;
    m_name = "";
    m_d25 = false;
    m_body = 0;
    m_d14 = false;
    m_d24 = true;
}

CMDENTRY ODEACTORMASTER::s_commands[] = {
    { 0, 0, 0, 0, 0 },
};

void ODEACTORMASTER::RegisterCommands()
{
    g_classParents[G2ODEACTOR::s_classId] = ACTOR::s_classId;
    g_classCommands[G2ODEACTOR::s_classId] = s_commands;
    HashCommands(s_commands);
    RegisterClassName(s_commands, "ODEACTOR", "ACTOR");
}

class GD548620
{
public:
    // Out of line at 0x00548460.
    GD548620();

    static int s_classId;
    static int s_factoryId;

    char m_pad[0xd18];
};

ACTOR *CreateG2ODEACTOR()
{
    return (ACTOR *)new G2ODEACTOR;
}

ACTOR *CreateGD548620()
{
    return (ACTOR *)new GD548620;
}

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

extern "C" void _dBodySetLinearVel(void *body, float x, float y, float z);
extern "C" void _dBodySetAngularVel(void *body, float x, float y, float z);
extern "C" void _dBodyDisable(void *body);

extern RESOURCES *g_pResources;
extern RESOURCEMANAGER *g_pActorResourceManager2;

class RESX
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
};

class ODEACTORM4
{
public:
    virtual void Stop();
};

void ODEACTORM4::Stop()
{
    if (*(bool *)((char *)this + 0xd24)) {
        *(bool *)((char *)this + 0xd24) = false;
        _dBodySetLinearVel(*(void **)((char *)this + 0xccc), 0.0f, 0.0f, 0.0f);
        _dBodySetAngularVel(*(void **)((char *)this + 0xccc), 0.0f, 0.0f, 0.0f);
        _dBodyDisable(*(void **)((char *)this + 0xccc));
        RESHANDLE *handles = (RESHANDLE *)((char *)this + 0xb58);
        for (int i = 16; i != 0; i--) {
            if (handles->IsValid()) {
                RESOURCE *resource = g_pResources->GetResource(*handles);
                if (resource) {
                    int managerId = g_pActorResourceManager2->m_id;
                    if (resource->GetManagerId() == managerId) {
                        RESX *x = (RESX *)resource;
                        if (*(bool *)((char *)resource + 0x1c)) {
                            x->V23();
                            *(bool *)((char *)resource + 0x1c) = false;
                        }
                    }
                }
            }
            handles++;
        }
    }
}

extern RESOURCEMANAGER *g_pActorResourceManagerX;

class ODEV
{
public:
    virtual void D000();
    virtual void D001();
    virtual void D002();
    virtual void D003();
    virtual void D004();
    virtual void D005();
    virtual void D006();
    virtual void D007();
    virtual void D008();
    virtual void D009();
    virtual void D010();
    virtual void D011();
    virtual void D012();
    virtual void D013();
    virtual void D014();
    virtual void D015();
    virtual void D016();
    virtual void D017();
    virtual void D018();
    virtual void D019();
    virtual void D020();
    virtual void D021();
    virtual void D022();
    virtual void D023();
    virtual void D024();
    virtual void D025();
    virtual void D026();
    virtual void D027();
    virtual void D028();
    virtual void S74(const void *p);
    virtual void S78(const void *p);
    virtual void S7C(const void *p);
    virtual void D032();
    virtual void D033();
    virtual void D034();
    virtual void D035();
    virtual void D036();
    virtual void D037();
    virtual void D038();
    virtual void D039();
    virtual void D040();
    virtual void D041();
    virtual void D042();
    virtual void D043();
    virtual void D044();
    virtual void D045();
    virtual void D046();
    virtual void D047();
    virtual void D048();
    virtual void D049();
    virtual void D050();
    virtual void D051();
    virtual void D052();
    virtual void D053();
    virtual void D054();
    virtual void D055();
    virtual void D056();
    virtual void D057();
    virtual void D058();
    virtual void D059();
    virtual void D060();
    virtual void D061();
    virtual void D062();
    virtual void D063();
    virtual void D064();
    virtual void D065();
    virtual void D066();
    virtual void D067();
    virtual void D068();
    virtual void D069();
    virtual void D070();
    virtual void D071();
    virtual void D072();
    virtual void D073();
    virtual void D074();
    virtual void D075();
    virtual void D076();
    virtual void D077();
    virtual void D078();
    virtual void D079();
    virtual void D080();
    virtual void D081();
    virtual void D082();
    virtual void D083();
    virtual void D084();
    virtual void D085();
    virtual void D086();
    virtual void D087();
    virtual void D088();
    virtual void D089();
    virtual void D090();
    virtual void D091();
    virtual void D092();
    virtual void D093();
    virtual void D094();
    virtual void D095();
    virtual void D096();
    virtual void D097();
    virtual void D098();
    virtual void D099();
    virtual void D100();
    virtual void D101();
    virtual void D102();
    virtual void D103();
    virtual void D104();
    virtual void D105();
    virtual void D106();
    virtual void D107();
    virtual void D108();
    virtual void D109();
    virtual void D110();
    virtual void D111();
    virtual void D112();
    virtual void D113();
    virtual void D114();
    virtual void D115();
    virtual void D116();
    virtual void D117();
    virtual void D118();
    virtual void D119();
    virtual void D120();
    virtual void D121();
    virtual void D122();
    virtual void D123();
    virtual void D124();
    virtual void D125();
    virtual void D126();
    virtual void D127();
    virtual void D128();
    virtual void D129();
    virtual void D130();
    virtual void D131();
    virtual void D132();
    virtual void D133();
    virtual void D134();
    virtual void S21C(const void *p);
    virtual void D136();
    virtual void D137();
    virtual void D138();
    virtual void D139();
    virtual void D140();
    virtual void D141();
    virtual void D142();
    virtual void D143();
    virtual void D144();
    virtual void D145();
    virtual void D146();
    virtual void S24C(const void *p);
    virtual void S250(const void *p);
};

class ODEACTORM5
{
public:
    void SetTemplate(const CSTRING &name);
};

void ODEACTORM5::SetTemplate(const CSTRING &name)
{
    *(CSTRING *)((char *)this + 0xd20) = name;
    RESOURCEMASTER *master =
        g_pActorResourceManagerX->GetMaster(name.c_str());
    if (master) {
        ODEV *v = (ODEV *)this;
        v->S24C((char *)master + 0x820);
        v->S250((char *)master + 0x824);
        v->S7C((char *)master + 0x834);
        v->S78((char *)master + 0x828);
        v->S74((char *)master + 0x825);
        v->S21C(master);
    }
}

void RegisterODEActors()
{
    g_factoryCreators[g_numFactories] = CreateG2ODEACTOR;
    g_factoryFlags[g_numFactories] = 1;
    g_factoryClassIds[g_numFactories] = G2ODEACTOR::s_classId;
    g_classFactoryIds[G2ODEACTOR::s_classId] = g_numFactories;
    strncpy(g_factoryNames[g_numFactories], "ODEACTOR", 30);
    G2ODEACTOR::s_factoryId = g_numFactories++;

    g_factoryCreators[g_numFactories] = CreateGD548620;
    g_factoryFlags[g_numFactories] = 1;
    g_factoryClassIds[g_numFactories] = GD548620::s_classId;
    g_classFactoryIds[GD548620::s_classId] = g_numFactories;
    strncpy(g_factoryNames[g_numFactories], "ODEWORLD", 30);
    GD548620::s_factoryId = g_numFactories++;

    if (g_numFactories >= MAX_ACTOR_FACTORIES && g_pLog)
        g_pLog->Error("ERROR!! SCENE::LoadActorsInformation>FATAL ERROR, max number of supported actor types exceeded");
}

extern "C" void dBodyAddForce(void *body, float x, float y, float z);

void G2ODEACTOR::AddForce(float x, float y, float z)
{
    if (m_body)
        dBodyAddForce(m_body, x, y, z);
}

// Kept abstract here so this unit does not emit the class's vtable.
class BONEDMODELRESOURCE : public OBJECT
{
public:
    virtual ~BONEDMODELRESOURCE();
    virtual V3 Virtual82(int value);
    virtual void Update();
};

void BONEDMODELRESOURCE::Update()
{
    OBJECT::Update();
}

V3 BONEDMODELRESOURCE::Virtual82(int value)
{
    return V3(0.0f, 0.0f, 0.0f);
}

extern "C" void dBodyDestroy(void *body);

__declspec(noinline) G2ODEACTOR::~G2ODEACTOR()
{
    if (m_body)
        dBodyDestroy(m_body);
    ((G2ODEADJ *)g_pOdeWorldX)->Call(this);
}

void DeleteG2ODEACTOR(G2ODEACTOR *actor)
{
    delete actor;
}

// The retail unit instantiates this table; emit its members so the delinked
// symbols can be compared (the call sites are not part of this unit yet).
class G2CORE1R1
{
public:
    G2CORE1R1();

    float m_0;
    float m_4;
    float m_8;
};

template G2CORE1R1 *DYNAMICARRAY<G2CORE1R1>::Set(const G2CORE1R1 &item, const int &slot);
template G2CORE1R1 *DYNAMICARRAY<G2CORE1R1>::Insert(const G2CORE1R1 &item);
template <>
void DYNAMICARRAY<G2CORE1R1>::Remove(const int &slot)
{
    if (!IsValid(slot))
        return;
    m_used[slot] = false;
    m_data[slot] = m_empty;
    if (IsValid(++m_freeTop))
        m_free[m_freeTop] = slot;
    m_count--;
}


