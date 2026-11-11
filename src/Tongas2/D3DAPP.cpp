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

// The display object game code reaches through g_pDisplay+0x44. Only the
// resolution setter is in this block.
class G2CORE4D3DAPP
{
public:
    void SetResolution(int index);
    // Rebuilds the swap chain for the stored mode (0x507df0).
    void SetMode(int width, int height);
    void SetSize(int width, int height);

    char pad_00[4];
    int m_4;
    char pad_08[0x1d0 - 0x08];
    int m_1d0;
    int m_1d4;
};

void G2CORE4D3DAPP::SetSize(int width, int height)
{
    m_1d0 = width;
    m_1d4 = height;
}

void G2CORE4D3DAPP::SetResolution(int index)
{
    m_4 = index;
    SetMode(m_1d0, m_1d4);
}
