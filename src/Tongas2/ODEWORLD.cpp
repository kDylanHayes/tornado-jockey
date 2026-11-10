// Tongas2/ODEWORLD -- the ODE world object.
//
// Discovery scaffold: bodies are empty until the unit is matched.
#include "Tongas2/ODEWORLD.h"

void ODEWORLDMASTER::RegisterCommands()
{
}

class GD548620
{
public:
    virtual ~GD548620();
};

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
