// Tongas2/D3DAPP -- the Direct3D 8 application framework.
//
// Discovery scaffold: bodies are empty until the unit is matched; each
// function has its exact target size in config/symbols.txt so a later delink
// cannot cut the target short. Create's extent includes its exception handler
// block at 0x005088b7.
#include <string>
#include "Tongas2/D3DAPP.h"

D3DAPP::D3DAPP()
{
}

int D3DAPP::Run()
{
    return 0;
}

int D3DAPP::Create(HINSTANCE__ *instance)
{
    (void)instance;
    return 0;
}

int D3DAPP::Initialize3DEnvironment()
{
    return 0;
}

void D3DAPP::Cleanup3DEnvironment()
{
}

class GD507220OBJECT
{
public:
    ~GD507220OBJECT();
};

class GD507220
{
public:
    virtual ~GD507220();

    char pad_04[0xb0 - 0x04];
    GD507220OBJECT *m_b0;
    char pad_b4[0x1b4 - 0xb4];
    std::string m_1b4;
};

GD507220::~GD507220()
{
    if (m_b0) {
        delete m_b0;
        m_b0 = 0;
    }
}
