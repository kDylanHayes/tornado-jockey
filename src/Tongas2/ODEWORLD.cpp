// Tongas2/ODEWORLD -- the ODE world object.
//
// Discovery scaffold: bodies are empty until the unit is matched.
#include "Tongas2/ODEWORLD.h"

#include "Tongas2/ACTOR.h"
#include "Tongas2/RESOURCES.h"
#include "Tongas2/CMDBASE.h"

void ODEWORLDMASTER::RegisterCommands()
{
}

class GD548620 : public ACTOR
{
public:
    GD548620();
    virtual ~GD548620();
    virtual int GetClassId();
    virtual int GetParentClassId();
    virtual const char *GetClassName();

    static int s_classId;

    ZEROV3 m_gravity;   // 0xccc
    float m_cd8;        // 0xcd8
    float m_cdc;        // 0xcdc
    float m_ce0;        // 0xce0
    float m_ce4;        // 0xce4
    int m_ce8;          // 0xce8
    int m_cec;          // 0xcec
    int m_cf0;          // 0xcf0
    ZEROV3 m_high;      // 0xcf4
    ZEROV3 m_low;       // 0xd00
    bool m_d0c;         // 0xd0c
    int m_d10;          // 0xd10
    float m_d14;        // 0xd14
};

GD548620::GD548620()
{
    m_gravity = V3(0.0f, -9.81f, 0.0f);
    m_high = V3(100.0f, 100.0f, 100.0f);
    m_low = V3(0.0f, 0.0f, 0.0f);
    m_cd8 = 0.2f;
    m_cdc = 0.0001f;
    m_ce0 = 0.1f;
    m_ce4 = 0.001f;
    m_ce8 = 4;
    m_cec = 10000;
    m_cf0 = 5;
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

struct ODEWORLDDATA
{
    int m_words[0x14];
};

class GD549b40
{
public:
    virtual void D00();
    virtual void D01();
    virtual void D02();
    virtual void D03();
    virtual void D04();
    virtual void D05();
    virtual void D06();
    virtual void D07();
    virtual void D08();
    virtual void D09();
    virtual void D10();
    virtual void D11();
    virtual void D12();
    virtual void D13();
    virtual void D14();
    virtual void D15();
    virtual void D16();
    virtual void D17();
    virtual void D18();
    virtual void D19();
    virtual void D20();
    virtual void D21();
    virtual void D22();
    virtual void D23();
    virtual void D24();
    virtual void D25();
    virtual void D26();
    virtual void D27();
    virtual void D28();
    virtual void D29();
    virtual void D30();
    virtual void D31();
    virtual void D32();
    virtual void D33();
    virtual void D34();
    virtual void D35();
    virtual void D36();
    virtual void D37();
    virtual void D38();
    virtual void D39();
    virtual void D40();
    virtual void D41();
    virtual void D42();
    virtual void SetData(ODEWORLDDATA data);
};

void GD549b40::SetData(ODEWORLDDATA data)
{
    *(ODEWORLDDATA *)((char *)this + 0x58) = data;
    if (*(bool *)((char *)this + 0x424)) {
        RESHANDLE *handles = (RESHANDLE *)((char *)this + 0x400);
        for (int i = 3; i != 0; i--) {
            RESOURCE *resource = g_pResources->GetResource(*handles);
            if (resource)
                ((GD549b40 *)resource)->SetData(data);
            handles++;
        }
    }
    *(bool *)((char *)this + 0x431) = true;
}
