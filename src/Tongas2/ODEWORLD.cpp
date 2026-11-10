// Tongas2/ODEWORLD -- the ODE world object.
//
// Discovery scaffold: bodies are empty until the unit is matched.
#include "Tongas2/ODEWORLD.h"

#include "Tongas2/ACTOR.h"
#include "Tongas2/CMDBASE.h"
#include "Tongas2/CPRIMITIVE3.h"
#include "Tongas2/RESOURCES.h"

void ODEWORLDMASTER::RegisterCommands()
{
}

class GD548620 : public ACTOR
{
public:
    // Out of line at 0x00548460.
    GD548620();

    virtual ~GD548620();
    virtual int GetClassId();
    virtual int GetParentClassId();
    virtual const char *GetClassName();

    static int s_classId;

    V3 m_gravity;
    float m_erp;
    float m_cfm;
    float m_contactMaxCorrectingVel;
    float m_contactSurfaceLayer;
    int m_maxContacts;
    int m_maxContactsPerBody;
    int m_maxPairs;
    V3 m_cf4;
    V3 m_d00;
    bool m_d0c;
    int m_d10;
    float m_d14;
};

GD548620::GD548620()
    : m_gravity(0.0f, 0.0f, 0.0f),
      m_cf4(0.0f, 0.0f, 0.0f),
      m_d00(0.0f, 0.0f, 0.0f)
{
    m_gravity = V3(0.0f, -9.81f, 0.0f);
    m_cf4 = V3(100.0f, 100.0f, 100.0f);
    m_erp = 0.2f;
    m_cfm = 0.0001f;
    m_contactMaxCorrectingVel = 0.1f;
    m_contactSurfaceLayer = 0.001f;
    m_maxContacts = 4;
    m_maxContactsPerBody = 10000;
    m_maxPairs = 5;
    m_d00 = V3(0.0f, 0.0f, 0.0f);
    m_d0c = true;
    m_d10 = 20;
    m_d14 = 0.01f;
}

int GD548620::GetClassId()
{
    return s_classId;
}

int GD548620::GetParentClassId()
{
    return g_classParents[s_classId];
}

const char *GD548620::GetClassName()
{
    return g_classNames[s_classId];
}

class ODEWORLDR
{
public:
    virtual int GetClassId();
    virtual int GetFactoryFlags();

    static int s_classId;
};

int ODEWORLDR::GetClassId()
{
    return s_classId;
}

int ODEWORLDR::GetFactoryFlags()
{
    return g_factoryFlags[s_classId];
}

__declspec(noinline) GD548620::~GD548620()
{
}

struct G2ODEWGET
{
    float Get78();
    float Get7c();
    float Get80();
    float Get84();
    float Get88();
};

float G2ODEWGET::Get78()
{
    return *(float *)(*(char **)((char *)this + 0x28) + 0x78);
}

float G2ODEWGET::Get7c()
{
    return *(float *)(*(char **)((char *)this + 0x28) + 0x7c);
}

float G2ODEWGET::Get80()
{
    return *(float *)(*(char **)((char *)this + 0x28) + 0x80);
}

float G2ODEWGET::Get84()
{
    return *(float *)(*(char **)((char *)this + 0x28) + 0x84);
}

float G2ODEWGET::Get88()
{
    return *(float *)(*(char **)((char *)this + 0x28) + 0x88);
}

struct G2ODEINIT
{
    G2ODEINIT *Init();
    int m_00;
    int m_04;
    int m_08;
    int m_0c;
    int m_10;
    int m_14;
    int m_18;
    int m_1c;
    int m_20;
};

G2ODEINIT *G2ODEINIT::Init()
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
    return this;
}

struct G2ODESWAP
{
    void Swap();
};

void G2ODESWAP::Swap()
{
    char *p = *(char **)((char *)this + 0x43c);
    for (int i = 0; i < *(int *)((char *)this + 0x440); i++, p += 0xc) {
        int a = *(int *)p;
        int b = *(int *)(p + 4);
        *(int *)p = b;
        *(int *)(p + 4) = a;
    }
}

struct ODEWORLDM1
{
    void SetD0C(const bool &value);
    void SetD14(const int &value);
};

void ODEWORLDM1::SetD0C(const bool &value)
{
    *(bool *)((char *)this + 0xd0c) = value;
}

void ODEWORLDM1::SetD14(const int &value)
{
    *(int *)((char *)this + 0xd14) = value;
}

extern "C" void dWorldSetERP(void *world, float value);
extern "C" void dWorldSetCFM(void *world, float value);
extern "C" void dWorldSetContactSurfaceLayer(void *world, float value);
extern "C" void dWorldSetContactMaxCorrectingVel(void *world, float value);
extern "C" void dWorldSetQuickStepNumIterations(void *world, int value);
extern "C" void dWorldSetGravity(void *world, float x, float y, float z);

extern void *g_pOdeWorldX;

struct ODEWORLDM2
{
    void SetCD8(const float &value);
    void SetCDC(const float &value);
    void SetCE4(const float &value);
    void SetCE0(const float &value);
    void SetD10(const int &value);
    void SetCCC(const V3 &value);
    void SetCF0(const float &value);
    void SetCE8(const int &value);
    void SetCEC(const int &value);
    void SetCF4(const V3 &value);
    void SetD00(const V3 &value);
};

void ODEWORLDM2::SetCD8(const float &value)
{
    *(float *)((char *)this + 0xcd8) = value;
    dWorldSetERP(*(void **)((char *)g_pOdeWorldX + 0x5c), value);
}

void ODEWORLDM2::SetCDC(const float &value)
{
    *(float *)((char *)this + 0xcdc) = value;
    dWorldSetCFM(*(void **)((char *)g_pOdeWorldX + 0x5c), value);
}

void ODEWORLDM2::SetCE4(const float &value)
{
    *(float *)((char *)this + 0xce4) = value;
    dWorldSetContactSurfaceLayer(*(void **)((char *)g_pOdeWorldX + 0x5c), value);
}

void ODEWORLDM2::SetCE0(const float &value)
{
    *(float *)((char *)this + 0xce0) = value;
    dWorldSetContactMaxCorrectingVel(*(void **)((char *)g_pOdeWorldX + 0x5c), value);
}

void ODEWORLDM2::SetD10(const int &value)
{
    *(int *)((char *)this + 0xd10) = value;
    dWorldSetQuickStepNumIterations(*(void **)((char *)g_pOdeWorldX + 0x5c), value);
}

void ODEWORLDM2::SetCCC(const V3 &value)
{
    *(V3 *)((char *)this + 0xccc) = value;
    dWorldSetGravity(*(void **)((char *)g_pOdeWorldX + 0x5c),
                     value.x, value.y, value.z);
}

void ODEWORLDM2::SetCF0(const float &value)
{
    *(float *)((char *)this + 0xcf0) = value;
    *(bool *)((char *)g_pOdeWorldX + 0x68) = false;
}

void ODEWORLDM2::SetCE8(const int &value)
{
    *(int *)((char *)this + 0xce8) = value;
    *(int *)((char *)g_pOdeWorldX + 0x80) = value;
}

void ODEWORLDM2::SetCEC(const int &value)
{
    *(int *)((char *)this + 0xcec) = value;
    *(int *)((char *)g_pOdeWorldX + 0x84) = value;
}

void ODEWORLDM2::SetCF4(const V3 &value)
{
    *(V3 *)((char *)this + 0xcf4) = value;
    *(bool *)((char *)g_pOdeWorldX + 0x68) = false;
}

void ODEWORLDM2::SetD00(const V3 &value)
{
    *(V3 *)((char *)this + 0xd00) = value;
    *(bool *)((char *)g_pOdeWorldX + 0x68) = false;
}

// The ODE geometry resource a world actor makes its bodies from.
class ODEGEOMRESOURCE : public CPRIMITIVE3RESOURCE
{
public:
    virtual void SetRelAngles(ANG angles);
};

void ODEGEOMRESOURCE::SetRelAngles(ANG angles)
{
    m_relAngles = angles;
    if (*(bool *)((char *)this + 0x424)) {
        RESHANDLE *handle = (RESHANDLE *)((char *)this + 0x400);
        for (int i = 0; i < CPRIMITIVE3_MAX_HELPERS; i++) {
            OBJECT *copy = (OBJECT *)g_pResources->GetResource(handle[i]);
            if (copy)
                copy->SetRelAngles(angles);
        }
    }
    *(bool *)((char *)this + 0x431) = true;
}
