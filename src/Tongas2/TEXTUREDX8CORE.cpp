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
#include <stdexcept>
#include "Tongas2/TEXTUREDX8CORE.h"
#include "Tongas2/PATHREPOSITORY.h"
#include "Tongas2/APPWINDOW.h"
#include "lib/freeimage/FreeImage.h"

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
    __declspec(noinline) void Release();

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

// 0x0053caa0: the release of the record the second list's destructor frees.
class GD53caa0
{
public:
    void Release();
};

// ---------------------------------------------------------------------------
// 0x00541e90: the destructor of the first shader list.  Its device sits at +8
// (with the "in use" byte at +0x25), the two interfaces at +0/+4, and five
// four-word containers start at +0x28; the last of them, at +0x68, holds the
// shader setters the body frees.  The other containers are only torn down by
// their own destructors, which is why their storage is zeroed after the
// device release.  Retail's redundant second test around `delete item` means
// the body kept the element in its own local; the local for the device is what
// puts the epilogue pops where retail has them.
// ---------------------------------------------------------------------------
class GD541e90
{
public:
    ~GD541e90();

    G2DX8IFACE *m_0;
    G2DX8IFACE *m_4;
    G2DX8DEVICE *m_8;
    char pad_c[0x24 - 0xc];
    bool m_24;
    bool m_25;
    char pad_26[0x28 - 0x26];
    std::vector<int> m_28;
    std::vector<int> m_38;
    std::vector<int> m_48;
    std::vector<int> m_58;
    std::vector<GD543060 *> m_68;
};

GD541e90::~GD541e90()
{
    if (m_25)
        m_8->ReleaseTexture();
    for (unsigned i = 0; i < m_68.size(); i++) {
        if (m_68[i]) {
            GD543060 *item = m_68[i];
            if (item) {
                item->Release();
                delete item;
            }
            m_68[i] = 0;
        }
    }
    m_68.clear();
    G2DX8DEVICE *device = m_8;
    if (device) {
        device->ReleaseTexture();
        delete device;
        m_8 = 0;
    }
    if (m_0) {
        m_0->Release();
        m_0 = 0;
    }
    if (m_4) {
        m_4->Release();
        m_4 = 0;
    }
}

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
    void *pad_0ac[7];                                   // 0x0ac - 0x0c7
    void (__stdcall *m_c8)(DX8DEVICEOBJ *, int, int);   // 0x0c8
    void *pad_0cc[10];                                  // 0x0cc - 0x0f3
    void (__stdcall *m_f4)(DX8DEVICEOBJ *, int, int);   // 0x0f4
    void *pad_0f8[1];                                   // 0x0f8 - 0x0fb
    void (__stdcall *m_fc)(DX8DEVICEOBJ *, int, int, int);
    void *pad_100[7];                                   // 0x100 - 0x11b
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

// ---------------------------------------------------------------------------
// 0x00540e80: fills the eight corner vectors of the box at +0x38 from the two
// vectors at +0x20 and +0x2c.
struct V3F
{
    float x, y, z;
};

class GD540e80
{
public:
    void FillCorners();

    char pad_00[0x20];
    V3F m_20;
    V3F m_2c;
    V3F m_box[8];
};

void GD540e80::FillCorners()
{
    for (int i = 0; i < 8; ++i) {
        V3F v;
        v.x = (i & 1) ? m_20.x : m_2c.x;
        v.y = (i & 2) ? m_20.y : m_2c.y;
        v.z = (i & 4) ? m_20.z : m_2c.z;
        m_box[i] = v;
    }
}

// ---------------------------------------------------------------------------
// 0x005430a0: one shader-stage apply.  The flags at +0x100.. of the texture at
// +0x24 select the render-state block.
class GD5430a0TEX
{
public:
    bool Check(int id);

    char pad_000[0x100];
    bool m_100;
    bool m_101;
    char pad_102[0x105 - 0x102];
    bool m_105;
    bool m_106;
    char pad_107[0x108 - 0x107];
    bool m_108;
    char pad_109[0x10b - 0x109];
    bool m_10b;
    bool m_10c;
};

class SETTINGSTABLE
{
public:
    int GetInt(int id);

    char pad_000[0xff0];
    GD504f10 *m_ff0;
};

class GD5430a0
{
public:
    void Apply(DX8DEVICEOBJ *device);

    char pad_00[8];
    SETTINGSTABLE *m_8;
    char pad_c[0x24 - 0xc];
    GD5430a0TEX *m_24;
};

void GD5430a0::Apply(DX8DEVICEOBJ *device)
{
    int flag;

    if (!m_24 || !m_24->m_101)
        return;
    if (m_24->Check(0)) {
        flag = 0;
        if (m_8->GetInt(0xe) > 0)
            if (m_24->m_10b)
                flag = 1;
        device->m_vt->m_fc(device, flag, 0x18, 0);
    }
    if (m_24->m_106) {
        device->m_vt->m_c8(device, 0xe, 1);
        device->m_vt->m_c8(device, 0x1b, 0);
        device->m_vt->m_c8(device, 0x14, 1);
        return;
    }
    if (!m_24->m_105)
        return;
    if (!m_24->m_100)
        device->m_vt->m_c8(device, 0xe, 1);
    device->m_vt->m_c8(device, 0x1b, 0);
    device->m_vt->m_c8(device, 0x13, 2);
    device->m_vt->m_c8(device, 0x14, 1);
}

// ---------------------------------------------------------------------------
// 0x00541730: the same stage apply with the texture at +0x98 reached through
// the settings probe first.
class GD541730
{
public:
    void Apply(DX8DEVICEOBJ *device);

    char pad_00[8];
    SETTINGSTABLE *m_8;
    char pad_c[0x98 - 0xc];
    GD5430a0TEX *m_98;
};

void GD541730::Apply(DX8DEVICEOBJ *device)
{
    bool b = m_8->GetInt(0xe) > 0;

    if (m_98 && (m_98->m_108 || m_98->m_10c)) {
        if (m_98->m_106) {
            device->m_vt->m_c8(device, 0xe, 1);
            device->m_vt->m_c8(device, 0x1b, 0);
            device->m_vt->m_c8(device, 0x14, 1);
        } else if (m_98->m_105) {
            if (!m_98->m_100)
                device->m_vt->m_c8(device, 0xe, 1);
            device->m_vt->m_c8(device, 0x1b, 0);
            device->m_vt->m_c8(device, 0x13, 2);
            device->m_vt->m_c8(device, 0x14, 1);
        }
    }
    if (m_98 && m_98->m_101 && m_98->Check(0)) {
        int flag = 0;
        if (b && m_98->m_10b)
            flag = 1;
        device->m_vt->m_fc(device, flag, 0x18, 0);
    }
}

// ---------------------------------------------------------------------------
// 0x005412c0: the two-texture variant of the stage apply; the shader level it
// picks is clamped to 1 unless both shader versions are supported.
class HARDWARE
{
public:
    bool IsSupported(int id);
};

class GD6d09b8APP
{
public:
    char pad_000[0x2b8];
    HARDWARE *m_2b8;
};

class GD5412c0TEX
{
public:
    char pad_000[0x101];
    bool m_101;
    bool m_102;
    char pad_103[0x108 - 0x103];
    bool m_108;
    char pad_109[0x10b - 0x109];
    bool m_10b;
    bool m_10c;
};

class GD5412c0
{
public:
    void Apply(DX8DEVICEOBJ *device);

    char pad_00[8];
    SETTINGSTABLE *m_8;
    char pad_c[0x98 - 0xc];
    GD5412c0TEX *m_98;
    GD5412c0TEX *m_9c;
};

void GD5412c0::Apply(DX8DEVICEOBJ *device)
{
    bool ok;
    unsigned level;

    if (!((GD6d09b8APP *)g_pDisplay)->m_2b8->IsSupported(0x17))
        return;
    if (!(m_98 && m_98->m_101 && m_9c && m_9c->m_101))
        return;
    bool hi = m_8->GetInt(0xe) > 0;
    ok = true;
    if (m_8->m_ff0->Get(4) < 3 || m_8->m_ff0->Get(3) < 3) {
        ok = false;
        hi = false;
    }
    level = 1;
    if (hi && m_98->m_10b)
        level = 2;
    if (m_98->m_10c)
        level = 2;
    else if (m_98->m_108 && m_98->m_102)
        level = 2;
    if (level > 1 && !ok)
        level = 1;
    if (m_9c && m_9c->m_101) {
        device->m_vt->m_fc(device, level, 0xb, 0);
        device->m_vt->m_f4(device, level, 0);
        device->m_vt->m_fc(device, level, 1, 1);
    }
}

// ---------------------------------------------------------------------------
// 0x00541160: the stage apply with the texture-address probe and the extra
// clamp pair.
class GD541160TEX
{
public:
    int Get(int id);
    int Level();

    char pad_000[0x101];
    bool m_101;
    bool m_102;
    char pad_103[0x108 - 0x103];
    bool m_108;
    char pad_109[0x10b - 0x109];
    bool m_10b;
    bool m_10c;
};

class GD541160
{
public:
    void Apply(DX8DEVICEOBJ *device);

    char pad_00[8];
    SETTINGSTABLE *m_8;
    char pad_c[0x98 - 0xc];
    GD541160TEX *m_98;
    GD541160TEX *m_9c;
};

void GD541160::Apply(DX8DEVICEOBJ *device)
{
    bool ok;
    unsigned level;

    if (!((GD6d09b8APP *)g_pDisplay)->m_2b8->IsSupported(0x17))
        return;
    if (!(m_98 && m_98->m_101 && m_9c && m_9c->m_101))
        return;
    bool hi = m_8->GetInt(0xe) > 0;
    ok = true;
    if (m_8->m_ff0->Get(4) < 3 || m_8->m_ff0->Get(3) < 3) {
        ok = false;
        hi = false;
    }
    level = 1;
    if (hi && m_98->m_10b)
        level = 2;
    if (m_98->m_10c)
        level = 2;
    else if (m_98->m_108 && m_98->m_102)
        level = 2;
    if (level > 1 && !ok)
        level = 1;
    if (m_9c && m_9c->m_101) {
        device->m_vt->m_fc(device, level, 0xb, 1);
        device->m_vt->m_f4(device, level, m_9c->Level());
        device->m_vt->m_fc(device, level, 2, 2);
        device->m_vt->m_fc(device, level, 1, 5);
    }
}


// ---------------------------------------------------------------------------
// 0x005424c0: the stage-list constructor.  Four standard containers sit at
// +0x28..+0x67 (their _Myfirst lands at +0x2c..) and two three-word blocks at
// +0x6c and +0x78.
struct TXV3I
{
    int x, y, z;
};

class GD5424c0
{
public:
    GD5424c0();

    int m_0;
    int m_4;
    int m_8;
    char pad_0c[0x24 - 0x0c];
    bool m_24;
    bool m_25;
    char pad_26[0x28 - 0x26];
    std::vector<int> m_28;
    std::vector<int> m_38;
    std::vector<int> m_48;
    std::vector<int> m_58;
    char pad_68[0x6c - 0x68];
    TXV3I m_6c;
    TXV3I m_78;
};

GD5424c0::GD5424c0()
    : m_0(0), m_4(0), m_8(0), m_24(false), m_25(false)
{
    m_6c.x = 0;
    m_6c.y = 0;
    m_6c.z = 0;
    m_78.x = 0;
    m_78.y = 0;
    m_78.z = 0;
}


// ---------------------------------------------------------------------------
// 0x00544730: the shader-list constructor: six words, a standard string at
// +0x18, four more words, a flag at +0x5c and three containers from +0x60.
// Its destructor (0x005445c0) frees the shader records it owns, releases the
// device it built at +0x40 and the two interfaces at +0x38/+0x3c, then lets
// the members' own destructors run.
class GD544730
{
public:
    GD544730();
    ~GD544730();

    int m_0;
    int m_4;
    int m_8;
    int m_c;
    int m_10;
    int m_14;
    std::string m_18;
    int m_34;
    G2DX8IFACE *m_38;
    G2DX8IFACE *m_3c;
    G2DX8DEVICE *m_40;
    char pad_44[0x5c - 0x44];
    bool m_5c;
    char pad_5d[0x60 - 0x5d];
    std::vector<int> m_60;
    std::vector<int> m_70;
    std::vector<GD53caa0 *> m_80;
};

GD544730::GD544730()
    : m_0(0), m_4(0), m_8(0), m_c(0), m_10(0), m_14(0),
      m_34(0), m_38(0), m_3c(0), m_40(0), m_5c(false)
{
}

// 0x005445c0: the shader list's destructor.  As in the 0x00541e90 shape below,
// the loop keeps the record in a local (retail frees it through a second,
// redundant null test) and the device is released through a local, which is
// what makes the compiler emit the release/delete/null triple retail shows.
GD544730::~GD544730()
{
    for (unsigned i = 0; i < m_80.size(); i++) {
        if (m_80[i]) {
            GD53caa0 *item = m_80[i];
            if (item) {
                item->Release();
                delete item;
            }
            m_80[i] = 0;
        }
    }
    m_80.clear();
    G2DX8DEVICE *device = m_40;
    if (device) {
        device->ReleaseTexture();
        delete device;
        m_40 = 0;
    }
    if (m_38) {
        m_38->Release();
        m_38 = 0;
    }
    if (m_3c) {
        m_3c->Release();
        m_3c = 0;
    }
}

// ---------------------------------------------------------------------------
// 0x0053f060: five empty containers from +0x34, a size word at +0 and the
// tail word at +0x84.
class GD53f060
{
public:
    GD53f060();

    int m_0;
    char pad_04[0x08 - 0x04];
    int m_8;
    int m_c;
    int m_10;
    int m_14;
    int m_18;
    char pad_1c[0x34 - 0x1c];
    std::vector<int> m_34;
    std::vector<int> m_44;
    std::vector<int> m_54;
    std::vector<int> m_64;
    std::vector<int> m_74;
    int m_84;
};

GD53f060::GD53f060()
    : m_0(0x18), m_8(0), m_c(0), m_10(0), m_14(0), m_18(0), m_84(0)
{
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// 0x0053f0e0 / 0x0053f170 / 0x0053f200: the standard push_back the three
// shader-record vectors share, and the fill-construct beside it.  The 24-byte
// record is PATHREPOPOINT; the shader records are 32, 36 and 44 bytes.
struct SHADERTYPE20
{
    int m[8];
};

struct SHADERTYPE24
{
    int m[9];
};

struct SHADERTYPE2C
{
    int m[11];
};

template void std::vector<PATHREPOPOINT>::push_back(const PATHREPOPOINT &);
template void std::vector<SHADERTYPE24>::push_back(const SHADERTYPE24 &);
template void std::vector<SHADERTYPE2C>::push_back(const SHADERTYPE2C &);

template void std::vector<PATHREPOPOINT>::_Construct_n(size_t, const PATHREPOPOINT &);
template void std::vector<SHADERTYPE24>::_Construct_n(size_t, const SHADERTYPE24 &);
template void std::vector<SHADERTYPE2C>::_Construct_n(size_t, const SHADERTYPE2C &);
template void std::vector<SHADERTYPE20>::_Construct_n(size_t, const SHADERTYPE20 &);
template void std::vector<TEXTURETYPE20>::_Construct_n(size_t, const TEXTURETYPE20 &);


// ---------------------------------------------------------------------------
// 0x00546bc0: the loader's bitmap queue.  The owning object carries a word at
// +0 and a std::deque<FIBITMAP *> at +4 (its _Map at +8, _Mapsize at +0xc,
// _Myoff at +0x10, _Mysize at +0x14); the loop below frees every queued image
// and pops it, with front()/pop_front() inlined into the body.
// ---------------------------------------------------------------------------
class GD546bc0
{
public:
    void FreeAll();

    int m_0;
    std::deque<FIBITMAP *> m_4;
};

void GD546bc0::FreeAll()
{
    while (!m_4.empty()) {
        FreeImage_Unload(m_4.front());
        m_4.pop_front();
    }
}

// 0x00546e20  ?push_back@?$deque@PAUFIBITMAP@@...
// 0x00546cc0  ?_Growmap@?$deque@PAUFIBITMAP@@...
// 0x00546c50  ?_Xlen@?$deque@PAUFIBITMAP@@...
// The three stand-alone deque<FIBITMAP *> members, emitted by the instantiation
// the queue above needs; no bodies are written for them.
template void std::deque<FIBITMAP *>::push_back(FIBITMAP *const &);


// ---------------------------------------------------------------------------
// config/symbols.txt changes this unit still needs (all measured against
// orig/Tornado.exe, image base 0x400000, compared with build/cmp_func.py):
//
//   0x00546bc0  ?FreeAll@GD546bc0@@QAEXXZ                            0x6a
//        The bitmap queue's "free every queued image" loop: word at +0,
//        deque<FIBITMAP *> at +4.
//   0x00546e20  ?push_back@?$deque@PAUFIBITMAP@@V?$allocator@PAUFIBITMAP@@@std@@@std@@QAEXABQAUFIBITMAP@@@Z   0x78
//   0x00546cc0  ?_Growmap@?$deque@PAUFIBITMAP@@V?$allocator@PAUFIBITMAP@@@std@@@std@@IAEXI@Z                0x153
//   0x00546c50  ?_Xlen@?$deque@PAUFIBITMAP@@V?$allocator@PAUFIBITMAP@@@std@@@std@@IBEXXZ                   0x6a
//        (functions.txt has 0x69 for this one; our COMDAT is 0x6a because of
//        the trailing int3, see Tongas2/ANIMATEDGEOMETRY's note.)
//   0x005445c0  ??1GD544730@@QAE@XZ                                  0x16a
//        The shader list's destructor (its constructor at 0x00544730 is
//        already matched).  GD544560's Set at 0x00544560 is a member of the
//        same retail class, so both placeholder names should become one.
//   0x00541e90  ??1GD541e90@@QAE@XZ                                  0x169
//        The other shader list's destructor; ?Set@GD541e60@@ is a member of
//        the same retail class.
//
// The four _Construct_n / two _Xlen throws below are the vector instantiations
// the file already carried: their bytes are exact, they only need the name.
//
//   0x0053dfe0  ?_Xlen@?$vector@UPATHREPOPOINT@@V?$allocator@UPATHREPOPOINT@@@std@@@std@@IBEXXZ          0x6a
//   0x00542000  ?_Xlen@?$vector@USHADERTYPE20@@V?$allocator@USHADERTYPE20@@@std@@@std@@IBEXXZ           0x6a
//   0x0053ec50  ?_Construct_n@?$vector@UPATHREPOPOINT@@V?$allocator@UPATHREPOPOINT@@@std@@@std@@QAEXIABUPATHREPOPOINT@@@Z  0xa6
//   0x0053ed70  ?_Construct_n@?$vector@USHADERTYPE20@@V?$allocator@USHADERTYPE20@@@std@@@std@@QAEXIABUSHADERTYPE20@@@Z     0xa3
//   0x0053ee20  ?_Construct_n@?$vector@USHADERTYPE24@@V?$allocator@USHADERTYPE24@@@std@@@std@@QAEXIABUSHADERTYPE24@@@Z     0xa6
//   0x0053ef40  ?_Construct_n@?$vector@USHADERTYPE2C@@V?$allocator@USHADERTYPE2C@@@std@@@std@@QAEXIABUSHADERTYPE2C@@@Z     0xa3
//   0x00542410  ?_Construct_n@?$vector@UTEXTURETYPE20@@V?$allocator@UTEXTURETYPE20@@@std@@@std@@QAEXIABUTEXTURETYPE20@@@Z   0xa3
//
// The two 0x00542xxx _Insert_n copies (0x0053e320 and 0x00542070, both 0x250)
// call the no-allocator ??$_Uninit_copy overload, so they are not the vectors
// this file instantiates; their element types are still open.
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// The two vector length throwers this object leaves behind: std::vector's
// _Xlen is "throw length_error("vector<T> too long")", and the queue loader's
// template instantiations put one copy in the unit per element type that
// needs it (PATHREPOPOINT's list and the shader list's 32-byte record).
// ---------------------------------------------------------------------------
void VectorXlenPathRepo()
{
    throw std::length_error("vector<T> too long");
}

void VectorXlenShader()
{
    throw std::length_error("vector<T> too long");
}


// ---------------------------------------------------------------------------
// 0x00545b24: the sixteen-bit blend the texture loaders run over a locked
// surface.  The destination surface descriptor arrives in ecx (its word at +0
// is the byte pitch, its dword at +4 the bits) and the source descriptor is
// the third stack argument; width and height are the two word counts the rows
// are clipped to.  Each row mixes the low nibble of the source word into the
// destination's first byte:
//
//     dst[0] = src[0] ^ ((src[0] ^ dst[1]) & 0x0f)
//
// The row advance is the usual (pitch/4 - width) * 2 the other blitters use.
// 0x00545b20's recorded extent is four bytes early: the two loads at
// 0x00545b24 belong to this function (see the unit notes).
// ---------------------------------------------------------------------------
class TEXSURF16
{
public:
    int pitch;
    void *bits;
};

void __fastcall BlendNibble16(TEXSURF16 *dst, int unused, int width, int height,
                              TEXSURF16 *src)
{
    int dstHalf = dst->pitch / 2;
    int srcHalf = src->pitch / 2;
    unsigned char *dp = (unsigned char *)dst->bits;
    const unsigned char *sp = (const unsigned char *)src->bits;
    unsigned short rows = (unsigned short)height;
    dstHalf = (dstHalf >> 1) - width;
    srcHalf = (srcHalf >> 1) - width;
    if (rows == 0)
        return;
    int dstGap = (unsigned short)dstHalf * 2;
    int srcGap = (unsigned short)srcHalf * 2;
    (void)unused;
    while (rows) {
        int n = (unsigned short)width;
        if (n != 0) {
            do {
                *dp = *sp ^ ((*sp ^ dp[1]) & 0x0f);
                dp += 2;
                sp += 2;
            } while (--n);
        }
        dp += dstGap;
        sp += srcGap;
        --rows;
    }
}


// ---------------------------------------------------------------------------
// 0x00545ba0: the same row walk with a threshold instead of a blend.  Where the
// source word's low five bits stayed under 0x12 the destination's high byte
// loses its alpha bit; the source is only read, never written.
// ---------------------------------------------------------------------------
void MaskAlpha16(int width, int height, TEXSURF16 *dst, TEXSURF16 *src)
{
    int dstHalf = dst->pitch / 2;
    int srcHalf = src->pitch / 2;
    unsigned char *dp = (unsigned char *)dst->bits;
    const unsigned char *sp = (const unsigned char *)src->bits;
    unsigned short rows = (unsigned short)height;
    dstHalf = (dstHalf >> 1) - width;
    srcHalf = (srcHalf >> 1) - width;
    if (rows == 0)
        return;
    int dstGap = (unsigned short)dstHalf * 2;
    int srcGap = (unsigned short)srcHalf * 2;
    while (rows) {
        if ((unsigned short)width != 0) {
            int n = (unsigned short)width;
            do {
                if ((*sp & 0x1f) < 0x12)
                    dp[1] &= 0x7f;
                dp += 2;
                sp += 2;
            } while (--n);
        }
        dp += dstGap;
        sp += srcGap;
        --rows;
    }
}


// ---------------------------------------------------------------------------
// 0x00545c20: the thirty-two bit copy the same loaders use.  One byte per
// pixel crosses over - the source's second byte lands in the destination's
// alpha slot - and the row advance is (pitch/4 - width) * 4.
// ---------------------------------------------------------------------------
void CopyAlpha32(int width, int height, TEXSURF16 *dst, TEXSURF16 *src)
{
    int dstQ = dst->pitch / 4;
    int srcQ = src->pitch / 4;
    unsigned char *dp = (unsigned char *)dst->bits;
    const unsigned char *sp = (const unsigned char *)src->bits;
    unsigned short rows = (unsigned short)height;
    dstQ -= width;
    srcQ -= width;
    if (rows == 0)
        return;
    int dstGap = (unsigned short)dstQ * 4;
    int srcGap = (unsigned short)srcQ * 4;
    while (rows) {
        if ((unsigned short)width != 0) {
            int n = (unsigned short)width;
            do {
                dp[3] = sp[1];
                dp += 4;
                sp += 4;
            } while (--n);
        }
        dp += dstGap;
        sp += srcGap;
        --rows;
    }
}


// ---------------------------------------------------------------------------
// 0x005465d0: whether an eight-bit bitmap uses any pixel whose palette entry
// matches the low three bytes of the colour (red in bits 16..23).  The scan
// walks row by row through FreeImage_GetScanLine and compares the palette
// record the pixel indexes; anything that is not eight bits is refused before
// the walk starts.
// ---------------------------------------------------------------------------
bool __stdcall PaletteUsesColor(FIBITMAP *dib, unsigned color)
{
    int width = FreeImage_GetWidth(dib);
    int height = FreeImage_GetHeight(dib);
    RGBQUAD *palette = FreeImage_GetPalette(dib);
    if (FreeImage_GetBPP(dib) != 8)
        return false;
    for (int y = 0; y < height; y++) {
        BYTE *bits = FreeImage_GetScanLine(dib, y);
        for (int x = 0; x < width; x++) {
            RGBQUAD *entry = &palette[bits[x]];
            if (entry->rgbRed == ((color >> 16) & 0xff) &&
                entry->rgbGreen == ((color >> 8) & 0xff) &&
                entry->rgbBlue == (color & 0xff))
                return true;
        }
    }
    return false;
}


// ---------------------------------------------------------------------------
// State of the surface blitters above, measured against orig/Tornado.exe with
// build/tx_cmp.py (which masks our relocation sites on both sides):
//
//   0x00545b24  0x72  byte exact for the body, but our copy is 0x85 bytes: the
//        ecx-destination __fastcall shape is confirmed (a scratch copy of the
//        same statements compiled to 0x6a), the unit build only allocates the
//        frame differently.  Retail's recorded start 0x00545b20 is four bytes
//        early; 0x545b16..0x545b23 is not code of the neighbour either, so the
//        delinker extent should become 0x00545b24 + 0x72.
//   0x00545ba0  0x77  our size matches (0x77); first differing byte 0x0.
//        Residual: parameter load order (retail pushes ebx and takes width
//        first, then the destination descriptor; ours takes the descriptor
//        first) plus one extra movzx for the inner count - register colour.
//   0x00545c20  0x76  ours 0x7f; first differing byte 0x0, the same prologue
//        and count-register residue.
//   0x00545ca0 / 0x00545db0 / 0x00545ec0  not reconstructed: they walk a
//        descriptor {pitch; bits} row by row through FreeImage_GetBits,
//        FreeImage_GetBPP and FreeImage_GetScanLine (and 0x0055db90 for the
//        24/32-bit pixel fetch), with a palette path under the eight-bit test.
// ---------------------------------------------------------------------------
