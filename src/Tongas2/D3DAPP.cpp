// Tongas2/D3DAPP -- the Direct3D 8 application framework.
//
// Discovery scaffold: bodies are empty until the unit is matched; each
// function has its exact target size in config/symbols.txt so a later delink
// cannot cut the target short. Create's extent includes its exception handler
// block at 0x005088b7.
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
