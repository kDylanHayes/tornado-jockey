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

void __stdcall FUNC527150(int value);

void InitGraphics6()
{
    FUNC527150(6);
}

extern "C" __declspec(dllimport) int __stdcall ShowWindow(void *hwnd, int cmd);
extern "C" __declspec(dllimport) long __stdcall SendMessageA(void *hwnd, unsigned msg, unsigned wparam, long lparam);

class DX8SUB
{
public:
    virtual void V0();
    virtual void V1();
    virtual void V2();
    virtual void V3();
    virtual void V4();
    virtual void V5();
    virtual void V6();
    virtual void V7();
    virtual void V8();
    virtual void V9();
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
    void Tail();
};

class DX8APPX
{
public:
    void ShowWindowCommand(int cmd);
    void SubVirtualCommand();
    void CloseCommand();
    void SubTailCommand();
    int SubAdjustedCommand();
};

void DX8APPX::ShowWindowCommand(int cmd)
{
    ShowWindow(*(void **)((char *)this + 0x1ec), cmd);
}

void DX8APPX::SubVirtualCommand()
{
    ((DX8SUB *)((char *)this + 0x44))->V1();
}

void DX8APPX::CloseCommand()
{
    *(bool *)((char *)this + 0x10) = true;
    SendMessageA(*(void **)((char *)this + 0x1ec), 0x10, 0, 0);
    *(bool *)((char *)this + 0x248) = true;
}

void DX8APPX::SubTailCommand()
{
    DX8SUB *sub = (DX8SUB *)((char *)this + 0x44);
    sub->V11();
    sub->Tail();
}

int DX8APPX::SubAdjustedCommand()
{
    ((DX8SUB *)((char *)this - 0x44))->V22();
    return 0;
}
