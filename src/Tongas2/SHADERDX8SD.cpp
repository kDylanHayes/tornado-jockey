// Tongas2/SHADERDX8SD -- the SD renderer's shaders.
//
// Discovery scaffold: bodies are empty until the unit is matched; each
// function has its exact target size in config/symbols.txt so a later delink
// cannot cut the target short. Apply's extent includes its switch tables.
#include "Tongas2/SHADERDX8SD.h"

void SHADERDX8SDRESOURCE::Apply()
{
}

void *CreateSHADERDX8SD()
{
    return 0;
}

class GD53aa10
{
public:
    virtual ~GD53aa10();
};

__declspec(noinline) GD53aa10::~GD53aa10()
{
}

class GD53b2d0
{
public:
    virtual ~GD53b2d0();
};

__declspec(noinline) GD53b2d0::~GD53b2d0()
{
}

class GD53b5a0
{
public:
    virtual ~GD53b5a0();
};

__declspec(noinline) GD53b5a0::~GD53b5a0()
{
}

struct G2SD8GET
{
    void * Get3c(int index);
    void * Get2c(int index);
    void * Get1c(int index);
};

void * G2SD8GET::Get3c(int index)
{
    return (char *)*(void **)((char *)this + 0x3c) + index * 0xc;
}

void * G2SD8GET::Get2c(int index)
{
    return (char *)*(void **)((char *)this + 0x2c) + index * 0xc;
}

void * G2SD8GET::Get1c(int index)
{
    return (char *)*(void **)((char *)this + 0x1c) + index * 0xc;
}
