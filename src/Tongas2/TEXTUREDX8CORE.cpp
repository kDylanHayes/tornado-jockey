// Tongas2/TEXTUREDX8CORE -- the DX8 texture core and its helpers.
//
// Discovery scaffold: bodies are empty until the unit is matched; each
// function has its exact target size in config/symbols.txt so a later delink
// cannot cut the target short. CreateTexture's extent includes its switch
// table.
#include <deque>
#include <memory>
#include <algorithm>
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

__declspec(noinline) void G2DX8DEVICE::ReleaseTexture()
{
    if (m_0) {
        G2DX8IFACE *texture = m_0;
        texture->Release();
        m_0 = 0;
    }
    m_4 = false;
}

__declspec(noinline) void G2DX8DEVICE::CreateTexture(G2DX8OBJECT *object)
{
    if (m_4)
        return;
    m_4 = true;
    if (object->CreateTexture(0x5dc00, 0x208, 0x112, 0, this) < 0)
        m_4 = false;
}

// A texture staging block: an owned buffer and the scanline queue.
class TEXTUREDX8SDTEXTUREMEMBER
{
public:
    ~TEXTUREDX8SDTEXTUREMEMBER();

    char *m_0;
    std::deque<unsigned short> m_4;
};

TEXTUREDX8SDTEXTUREMEMBER::~TEXTUREDX8SDTEXTUREMEMBER()
{
    if (m_0) {
        delete[] m_0;
        m_0 = 0;
    }
}

// Two more wrappers over the texture device: the first pair holds the device
// at +0x40 and a flag at +0x5c, the second at +8 and +0x25.
class TEXWRAPB1
{
public:
    void Call();

    char pad_00[0x40];
    G2DX8DEVICE *m_40;
    char pad_44[0x5c - 0x44];
    bool m_5c;
};

class TEXWRAPB2
{
public:
    void Call(int arg);

    char pad_00[0x40];
    G2DX8DEVICE *m_40;
    char pad_44[0x5c - 0x44];
    bool m_5c;
};

void TEXWRAPB1::Call()
{
    if (m_5c)
        m_40->ReleaseTexture();
}

void TEXWRAPB2::Call(int arg)
{
    if (m_5c)
        m_40->CreateTexture((G2DX8OBJECT *)arg);
}

class TEXWRAPA1
{
public:
    void Call();

    char pad_00[8];
    G2DX8DEVICE *m_8;
    char pad_c[0x25 - 0xc];
    bool m_25;
};

class TEXWRAPA2
{
public:
    void Call(int arg);

    char pad_00[8];
    G2DX8DEVICE *m_8;
    char pad_c[0x25 - 0xc];
    bool m_25;
};

void TEXWRAPA1::Call()
{
    if (m_25)
        m_8->ReleaseTexture();
}

void TEXWRAPA2::Call(int arg)
{
    if (m_25)
        m_8->CreateTexture((G2DX8OBJECT *)arg);
}

// The stage block: five cleared words (the word at +4 is not touched here).
class TEXSTAGE
{
public:
    TEXSTAGE();

    int m_0;
    int m_4;
    int m_8;
    int m_c;
    int m_10;
    int m_14;
};

TEXSTAGE::TEXSTAGE()
{
    m_8 = 0;
    m_c = 0;
    m_10 = 0;
    m_14 = 0;
    m_0 = 0;
}

// A table of byte cells reached through the pointer array at +0x84.
class TEXSTAGES
{
public:
    void SetValue(int index, char value);

    char pad_00[0x84];
    char **m_84;
};

void TEXSTAGES::SetValue(int index, char value)
{
    char *&cell = m_84[index];
    *cell = value;
}

// The 0x20-byte element these helpers were instantiated for.
struct TEXTURETYPE20
{
    int m[8];
};

template TEXTURETYPE20 *std::copy_backward(
    TEXTURETYPE20 *, TEXTURETYPE20 *, TEXTURETYPE20 *);
template TEXTURETYPE20 *std::uninitialized_copy(
    TEXTURETYPE20 *, TEXTURETYPE20 *, TEXTURETYPE20 *);


// Releases the object held at +0x24 through the helper at +0x8.
class M40TARGET
{
public:
    void m_40(void *item);
};

class GD543060
{
public:
    void Release();

    char pad_0[4];
    M40TARGET *m_obj;
    char pad_mid[28];
    void *m_item;
};

void GD543060::Release()
{
    if (m_item)
        m_obj->m_40(m_item);
}
