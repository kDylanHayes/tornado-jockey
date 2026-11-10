// Tongas2/RENDERTEXTUREDX8SD -- the SD renderer's render textures.
//
// Discovery scaffold: bodies are empty until the unit is matched; each
// function has its exact target size in config/symbols.txt so a later delink
// cannot cut the target short.
#include "Tongas2/RENDERTEXTUREDX8SD.h"
#include "Tongas2/APPWINDOW.h"

RENDERTEXTUREDX8SDMASTER *CreateRENDERTEXTUREDX8SDMaster()
{
    return 0;
}

void *CreateRENDERTEXTUREDX8SD()
{
    return 0;
}

int GetRENDERTEXTUREFormat()
{
    return 0;
}

class GD539e70
{
public:
    virtual ~GD539e70();
};

__declspec(noinline) GD539e70::~GD539e70()
{
}

struct G2RTDSET
{
    void Set31d0(int value);
};

void G2RTDSET::Set31d0(int value)
{
    *(int *)((char *)this + 0x31d0) = value;
}

class RTKbc
{
public:
    virtual void d0(); virtual void d1(); virtual void d2(); virtual void d3(); virtual void d4(); virtual void d5(); virtual void d6(); virtual void d7();
    virtual void d8(); virtual void d9(); virtual void d10(); virtual void d11(); virtual void d12(); virtual void d13(); virtual void d14(); virtual void d15();
    virtual void d16(); virtual void d17(); virtual void d18(); virtual void d19(); virtual void d20(); virtual void d21(); virtual void d22(); virtual void d23();
    virtual void d24(); virtual void d25(); virtual void d26(); virtual void d27(); virtual void d28(); virtual void d29(); virtual void d30(); virtual void d31();
    virtual void d32(); virtual void d33(); virtual void d34(); virtual void d35(); virtual void d36(); virtual void d37(); virtual void d38(); virtual void d39();
    virtual void d40(); virtual void d41(); virtual void d42(); virtual void d43(); virtual void d44(); virtual void d45(); virtual void d46();
    virtual void T();
    void Thunk();
};

void RTKbc::Thunk()
{
    T();
}

class RTKc0
{
public:
    virtual void d0(); virtual void d1(); virtual void d2(); virtual void d3(); virtual void d4(); virtual void d5(); virtual void d6(); virtual void d7();
    virtual void d8(); virtual void d9(); virtual void d10(); virtual void d11(); virtual void d12(); virtual void d13(); virtual void d14(); virtual void d15();
    virtual void d16(); virtual void d17(); virtual void d18(); virtual void d19(); virtual void d20(); virtual void d21(); virtual void d22(); virtual void d23();
    virtual void d24(); virtual void d25(); virtual void d26(); virtual void d27(); virtual void d28(); virtual void d29(); virtual void d30(); virtual void d31();
    virtual void d32(); virtual void d33(); virtual void d34(); virtual void d35(); virtual void d36(); virtual void d37(); virtual void d38(); virtual void d39();
    virtual void d40(); virtual void d41(); virtual void d42(); virtual void d43(); virtual void d44(); virtual void d45(); virtual void d46(); virtual void d47();
    virtual void T();
    void Thunk();
};

void RTKc0::Thunk()
{
    T();
}

extern "C" char DAT_0067abb8;
void *GetD0067abb8()
{
    return &DAT_0067abb8;
}

class SETTINGSTABLE
{
public:
    int GetInt(int index);
};

bool DisplaySetting10()
{
    SETTINGSTABLE *table = *(SETTINGSTABLE **)((char *)g_pDisplay + 0x2c0);
    return table->GetInt(10) != 0;
}

void *DisplayFieldEC()
{
    return *(void **)((char *)g_pDisplay + 0xec);
}
