// Tongas2/DX8APP -- the plugin's concrete Direct3D application object.
//
// Discovery scaffold: bodies are empty until the unit is matched; each
// function has its exact target size in config/symbols.txt so a later delink
// cannot cut the target short. The destructor is virtual in the target
// (0x00509900 is the scalar-deleting destructor's non-deleting half).
#include "Tongas2/DX8APP.h"

DX8APP::DX8APP()
{
}

DX8APP::~DX8APP()
{
}

void DX8APP::Init()
{
}

DX8APP *CreateDX8APP()
{
    return 0;
}

void RegisterDX8Plugin()
{
}

struct G2DX8GET
{
    void * Get54(int index);
};

void * G2DX8GET::Get54(int index)
{
    return (char *)*(void **)((char *)this + 0x54) + index * 0x1c;
}

struct G2DX8T1
{
    void Call();
};

struct G2DX8T2
{
    void Call();
};

struct G2DX8TH1
{
    void Call();
};

struct G2DX8TH2
{
    void Call();
};

void G2DX8TH1::Call()
{
    ((G2DX8T1 *)*(void **)((char *)this + 0x4c))->Call();
}

void G2DX8TH2::Call()
{
    ((G2DX8T2 *)*(void **)((char *)this + 0x4c))->Call();
}

struct DX8APPM1
{
    float Get258();
};

float DX8APPM1::Get258()
{
    return *(float *)((char *)this + 0x258);
}
