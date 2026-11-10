// Tongas2/TEXTUREDX8CORE -- the DX8 texture core and its helpers.
//
// Discovery scaffold: bodies are empty until the unit is matched; each
// function has its exact target size in config/symbols.txt so a later delink
// cannot cut the target short. CreateTexture's extent includes its switch
// table.
#include "Tongas2/TEXTUREDX8CORE.h"

int CreateTexture()
{
    return 0;
}

int TextureFormatA()
{
    return 0;
}

int TextureFormatB()
{
    return 0;
}

// The texture device the shader helpers hold at +0x30.
class G2DX8IFACE
{
public:
    virtual void __stdcall V0();
    virtual void __stdcall V1();
    virtual void __stdcall Release();
};

class G2DX8DEVICE;

// The direct3D-ish object the device creates its texture through; the create
// call sits at vtable slot 0x5c.
class G2DX8OBJECT
{
public:
    virtual void __stdcall V00(); virtual void __stdcall V01(); virtual void __stdcall V02(); virtual void __stdcall V03();
    virtual void __stdcall V04(); virtual void __stdcall V05(); virtual void __stdcall V06(); virtual void __stdcall V07();
    virtual void __stdcall V08(); virtual void __stdcall V09(); virtual void __stdcall V10(); virtual void __stdcall V11();
    virtual void __stdcall V12(); virtual void __stdcall V13(); virtual void __stdcall V14(); virtual void __stdcall V15();
    virtual void __stdcall V16(); virtual void __stdcall V17(); virtual void __stdcall V18(); virtual void __stdcall V19();
    virtual void __stdcall V20(); virtual void __stdcall V21(); virtual void __stdcall V22();
    virtual int __stdcall CreateTexture(int a, int b, int c, int d, G2DX8DEVICE *device);
};

class G2DX8DEVICE
{
public:
    void ReleaseTexture();
    void CreateTexture(G2DX8OBJECT *object);

    G2DX8IFACE *m_0;
    bool m_4;
};

void G2DX8DEVICE::ReleaseTexture()
{
    if (m_0) {
        G2DX8IFACE *texture = m_0;
        texture->Release();
        m_0 = 0;
    }
    m_4 = false;
}

void G2DX8DEVICE::CreateTexture(G2DX8OBJECT *object)
{
    if (m_4)
        return;
    m_4 = true;
    if (object->CreateTexture(0x5dc00, 0x208, 0x112, 0, this) < 0)
        m_4 = false;
}
