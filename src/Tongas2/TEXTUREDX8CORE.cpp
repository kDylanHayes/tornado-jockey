// Tongas2/TEXTUREDX8CORE -- the DX8 texture core and its helpers.
//
// Discovery scaffold: bodies are empty until the unit is matched; each
// function has its exact target size in config/symbols.txt so a later delink
// cannot cut the target short. CreateTexture's extent includes its switch
// table.
#include <deque>
#include <memory>
#include <algorithm>
#include <vector>
#include "Tongas2/TEXTUREDX8CORE.h"

// The frame triangle counter at 0x009fef40.
extern int g_numTriangles;

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

// A table of byte cells reached through the vector of cell pointers at +0x84.
class TEXSTAGECELL
{
public:
    char m_0;
};

class TEXSTAGES
{
public:
    void SetValue(int index, char value);

    char pad_00[0x80];
    std::vector<TEXSTAGECELL *> m_80;
};

void TEXSTAGES::SetValue(int index, char value)
{
    m_80[index]->m_0 = value;
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

__declspec(noinline) void GD543080::Set(void *item)
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
    GD543030(int a, int b);

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

GD543030::GD543030(int a, int b)
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
    GD540fe0(int a, int b, int c);

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

GD540fe0::GD540fe0(int a, int b, int c)
    : m_0(true), m_4(a), m_8(b), m_c(c), m_98(0), m_9c(0), m_a0(0),
      m_b0(0), m_b4(0), m_b8(0)
{
}


// 0x00541e60 / 0x00544560: the indexed Set the two shader lists share. The
// vector object starts at +0x68 in the first list and at +0x80 in the second
// (so its _Myfirst is at +0x6c / +0x84); only the element type differs.
class GD541e60
{
public:
    void Set(void *item, int index);

    char pad_00[0x68];
    std::vector<GD543080 *> m_68;
};

void GD541e60::Set(void *item, int index)
{
    if ((unsigned)index < m_68.size())
        m_68[index]->Set(item);
}

// The release call the second list makes; declared only, it lives in the
// texture-setter unit at 0x0053cac0.
class GD53cac0
{
public:
    void Set(void *item);
};

class GD544560
{
public:
    void Set(void *item, int index);

    char pad_00[0x80];
    std::vector<GD53cac0 *> m_80;
};

void GD544560::Set(void *item, int index)
{
    if ((unsigned)index < m_80.size())
        m_80[index]->Set(item);
}


// The DX8 device vtable as these helpers see it: `this` travels as the first
// stack argument, so a call site loads the vtable into a scratch register and
// pushes the object.
struct DX8DEVICEOBJ;

struct DX8DEVICEVT
{
    void *pad_000[42];                                  // 0x000 - 0x0a7
    void (__stdcall *m_a8)(DX8DEVICEOBJ *, void *);     // 0x0a8
    void *pad_0ac[28];                                  // 0x0ac - 0x11b
    void (__stdcall *m_11c)(DX8DEVICEOBJ *, int, int, int, int, int);
    void *pad_120[11];                                  // 0x120 - 0x14b
    void (__stdcall *m_14c)(DX8DEVICEOBJ *, int, int, int);
    void *pad_150[1];
    void (__stdcall *m_154)(DX8DEVICEOBJ *, int, int);
};

struct DX8DEVICEOBJ
{
    DX8DEVICEVT *m_vt;
};


// 0x005410c0: hands the two words at +0x14/+0x18 of the shader block to the
// device's effect setters.
class GD5410c0SHADER
{
public:
    int m_0;
    char pad_4[0x14 - 4];
    int m_14;
    int m_18;
};

class GD5410c0
{
public:
    void Apply(DX8DEVICEOBJ *device);

    char pad_00[0xc];
    GD5410c0SHADER *m_c;
};

void GD5410c0::Apply(DX8DEVICEOBJ *device)
{
    device->m_vt->m_14c(device, 0, m_c->m_14, m_c->m_0);
    device->m_vt->m_154(device, m_c->m_18, 0);
}


// 0x00541840: one indexed strip, counted into the frame's triangle total.
class GD541840
{
public:
    void Submit(DX8DEVICEOBJ *device);

    char pad_00[0xa0];
    int m_a0;
    int m_a4;
    char pad_a8[0xb0 - 0xa8];
    int m_b0;
    int m_b4;
};

void GD541840::Submit(DX8DEVICEOBJ *device)
{
    device->m_vt->m_11c(device, 4, m_b0, m_b4 - m_b0 + 1, m_a4, m_a0);
    g_numTriangles += m_a0;
}


// A texture-ish block whose +0x101 byte is its "ready" flag and whose payload
// starts at +4.
class GD543750TEX
{
public:
    char pad_00[0x101];
    bool m_101;
};

// 0x00543750: binds the optional texture, then submits the strip.
class GD543750
{
public:
    void Submit(DX8DEVICEOBJ *device);

    char pad_00[0x24];
    GD543750TEX *m_24;
    int m_28;
    int m_2c;
    char pad_30[0x38 - 0x30];
    int m_38;
    int m_3c;
};

void GD543750::Submit(DX8DEVICEOBJ *device)
{
    if (m_24 && m_24->m_101)
        device->m_vt->m_a8(device, (char *)m_24 + 4);
    device->m_vt->m_11c(device, 4, m_38, m_3c - m_38 + 1, m_2c, m_28);
    g_numTriangles += m_28;
}


// The display-side object the shader state queries: the caps helper hangs off
// +0xff0.
class GD504f10
{
public:
    int Get(int id);
};

class GD541100CAPS
{
public:
    char pad_000[0xff0];
    GD504f10 *m_ff0;
};

class GD541100TEX
{
public:
    char pad_000[0x101];
    bool m_101;
};

// 0x00541100: ready when both textures are loaded and the device reports the
// two shader versions.
class GD541100
{
public:
    bool Ready();

    char pad_00[8];
    GD541100CAPS *m_8;
    char pad_c[0x98 - 0xc];
    GD541100TEX *m_98;
    GD541100TEX *m_9c;
};

bool GD541100::Ready()
{
    if (!(m_98 && m_98->m_101 && m_9c && m_9c->m_101))
        return true;
    if (m_8->m_ff0->Get(4) >= 3 && m_8->m_ff0->Get(3) >= 3)
        return true;
    return false;
}
