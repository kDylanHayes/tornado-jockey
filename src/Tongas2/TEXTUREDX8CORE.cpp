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


// Stores the helper's result: the object at +0x4 produces it,
// the member at +0x24 keeps it.
class M58TARGET
{
public:
    void *m_58(void *item);
};

class GD543080
{
public:
    void Set(void *item);

    char pad_0[4];
    M58TARGET *m_obj;
    char pad_mid[28];
    void *m_result;
};

void GD543080::Set(void *item)
{
    if (item)
        m_result = m_obj->m_58(item);
}


// Releases the two textures at +0x98 and +0x9c through the helper, and
// marks the object idle.
class GD541030
{
public:
    void Release();

    char pad_0[4];
    M40TARGET *m_obj;
    char pad_8[0xc - 8];
    int m_c;
    char pad_10[0x98 - 0x10];
    void *m_98;
    void *m_9c;
};

void GD541030::Release()
{
    m_c = 0;
    if (m_98) {
        m_obj->m_40(m_98);
        m_98 = 0;
    }
    if (m_9c) {
        m_obj->m_40(m_9c);
        m_9c = 0;
    }
}


// 0x00541080 ?SetA@SHADERSETTERITEM@@QAEXPAX@Z
// 0x005410a0 ?SetB@SHADERSETTERITEM@@QAEXPAX@Z
// The shader setter's two texture slots, filled through the same out-of-line
// helper (0x004ff160) the GD543080::Set shape already uses; the results live
// at +0x98 and +0x9c.
class SHADERSETTERITEM
{
public:
    void SetA(void *texture);
    void SetB(void *texture);

    char pad_00[4];
    M58TARGET *m_4;
    char pad_08[0x98 - 8];
    void *m_98;
    void *m_9c;
};

void SHADERSETTERITEM::SetA(void *texture)
{
    if (texture)
        m_98 = m_4->m_58(texture);
}

void SHADERSETTERITEM::SetB(void *texture)
{
    if (texture)
        m_9c = m_4->m_58(texture);
}


// 0x00543030: two arguments into +0x4/+0x8 with the flag at +0 and the
// tail words (+0x24, +0x28, +0x2c, +0x38, +0x3c) cleared.
class GD543030
{
public:
    void Init(int a, int b);

    bool m_0;
    char pad_1[3];
    int m_4;
    int m_8;
    char pad_c[0x24 - 0xc];
    int m_24;
    int m_28;
    int m_2c;
    char pad_30[0x38 - 0x30];
    int m_38;
    int m_3c;
};

void GD543030::Init(int a, int b)
{
    m_4 = a;
    m_0 = true;
    m_8 = b;
    m_24 = 0;
    m_28 = 0;
    m_2c = 0;
    m_38 = 0;
    m_3c = 0;
}


// 0x00540fe0: the same shape with three arguments and the two trailing
// triples (+0x98 and +0xb0) cleared.
class GD540fe0
{
public:
    void Init(int a, int b, int c);

    bool m_0;
    char pad_1[3];
    int m_4;
    int m_8;
    int m_c;
    char pad_10[0x98 - 0x10];
    int m_98;
    int m_9c;
    int m_a0;
    char pad_a4[0xb0 - 0xa4];
    int m_b0;
    int m_b4;
    int m_b8;
};

void GD540fe0::Init(int a, int b, int c)
{
    m_4 = a;
    m_c = c;
    m_0 = true;
    m_8 = b;
    m_98 = 0;
    m_9c = 0;
    m_a0 = 0;
    m_b0 = 0;
    m_b4 = 0;
    m_b8 = 0;
}
