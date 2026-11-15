// Tongas2/SHADERDX8SD -- the SD renderer's shaders.
//
// Matched so far (retail addresses): the manager's CreateMaster 0x53b390, the
// master's shader factory 0x53b330, the APP-record factory 0x53b520, the
// 0x78-byte shader record's constructor 0x53bae0, the four-vector block's
// destructor 0x53b6d0 and copy constructor 0x53bb50, the parameter lists'
// destructor 0x53d220, the copy release 0x53aa20, the master's binding pass
// 0x53b2e0, the indexed-primitive draw 0x53d190 and the render-state setup
// 0x53cae0.
#include "Tongas2/SHADERDX8SD.h"

#include "d3d8.h"
#include "Tongas2/MATHS.h"
#include "Tongas2/SAFEINDEX.h"
#include <vector>
#include <memory>
#include <string>
#include <string.h>

// Same layout as PATHREPOSITORY's PATHREPOPOINT (two 12-byte vectors).
struct PATHREPOPOINT
{
    float m_pos[3];
    float m_step[3];
};

// The retail unit emitted this vector member out of line (0x53dad0).
template unsigned int std::vector<PATHREPOPOINT>::size() const;

// And this STL fill (0x53db30): a 0x18-byte element copy loop.
template void std::fill<PATHREPOPOINT *, PATHREPOPOINT>(
    PATHREPOPOINT *, PATHREPOPOINT *, const PATHREPOPOINT &);

// The larger vertex types this unit fills with rep movsd.
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

template void std::fill<SHADERTYPE20 *, SHADERTYPE20>(
    SHADERTYPE20 *, SHADERTYPE20 *, const SHADERTYPE20 &);
template void std::fill<SHADERTYPE24 *, SHADERTYPE24>(
    SHADERTYPE24 *, SHADERTYPE24 *, const SHADERTYPE24 &);
template void std::fill<SHADERTYPE2C *, SHADERTYPE2C>(
    SHADERTYPE2C *, SHADERTYPE2C *, const SHADERTYPE2C &);

// Two more vector element sizes the retail unit instantiated size() for.
template unsigned int std::vector<SHADERTYPE24>::size() const;
template unsigned int std::vector<SHADERTYPE2C>::size() const;

// Their copy/construct companions.
template SHADERTYPE24 *std::copy_backward(
    SHADERTYPE24 *, SHADERTYPE24 *, SHADERTYPE24 *);
template SHADERTYPE2C *std::copy_backward(
    SHADERTYPE2C *, SHADERTYPE2C *, SHADERTYPE2C *);
template SHADERTYPE24 *std::uninitialized_copy(
    SHADERTYPE24 *, SHADERTYPE24 *, SHADERTYPE24 *);
template SHADERTYPE2C *std::uninitialized_copy(
    SHADERTYPE2C *, SHADERTYPE2C *, SHADERTYPE2C *);
// The retail fill helpers call the allocator flavour of _Uninit_fill_n
// (five arguments: the allocator is passed by reference); the wrappers
// below instantiate it.
template void std::uninitialized_fill_n(
    SHADERTYPE20 *, unsigned int, const SHADERTYPE20 &);
template void std::uninitialized_fill_n(
    SHADERTYPE24 *, unsigned int, const SHADERTYPE24 &);
template void std::uninitialized_fill_n(
    SHADERTYPE2C *, unsigned int, const SHADERTYPE2C &);


// The manager base, defined in another unit; only its virtual order and the
// fields the factory touches matter here.
class RESOURCE;
class RESOURCEMASTER
{
public:
    virtual ~RESOURCEMASTER();

    // Defined in the resource core: gives a copy of this master back.
    void ReleaseCopy(RESOURCE *copy);
};

class FILELOADER;
class RESOURCEMANAGER
{
public:
    RESOURCEMANAGER();

    virtual RESOURCEMASTER *CreateMaster(const char *name);
    virtual const char *GetTypeCode();
    virtual ~RESOURCEMANAGER();
    virtual void Update();
    virtual bool Load();
    virtual bool IsMyType(const char *name);
    virtual void Virtual06();
    virtual void Init();

    int m_id;
    bool m_8;
    RESOURCEMASTER *m_first;
    RESOURCEMASTER *m_last;
    int m_count;
    bool m_18;
    FILELOADER *m_loader;
    bool m_loaded;
    bool m_initialized;
    char m_fileName[0x200];
};

// The plain shader manager and its SD subclass; the factory builds the latter
// (0x53a980).
class SHADERMANAGER : public RESOURCEMANAGER
{
public:
    SHADERMANAGER();
};

SHADERMANAGER::SHADERMANAGER()
{
    SafeSprintf(m_fileName, "SHADER.txt");
    m_8 = false;
    m_18 = true;
}

class SHADERDX8SDMANAGER : public SHADERMANAGER
{
public:
    SHADERDX8SDMANAGER()
    {
    }

    virtual RESOURCEMASTER *CreateMaster(const char *name);
};

void SHADERDX8SDRESOURCE::Apply()
{
}

void *CreateSHADERDX8SD()
{
    return new SHADERDX8SDMANAGER();
}

// The engine bases; only their destructors matter here.
class RESOURCE
{
public:
    virtual ~RESOURCE();
};

struct RESHANDLE;
class SHADER;

class SHADERMASTER : public RESOURCEMASTER
{
public:
    SHADERMASTER(RESOURCEMANAGER *manager, const char *name);
    virtual ~SHADERMASTER();

    // The binding passes live in the texture unit (0x0053a560, 0x0053a690 and
    // 0x0053a6d0); the SD shader master shares their fields.
    void ResetConstants();
    void ReleaseBindings();
    void BuildBindings();

    // The retail SHADERMASTER (SHADER.h) ends at 0x150; the SD master keeps
    // the number of texture bindings at 0x74.
    char m_pad[0x74 - 4];
    int m_count;
    char m_more[0x150 - 0x78];
};

class GD53aa10 : public RESOURCE
{
public:
    virtual ~GD53aa10() throw();
};

__declspec(noinline) GD53aa10::~GD53aa10() throw()
{
}

// One 12-byte shader binding record: two -1 markers and a zero.
struct GD53b2d0REC
{
    int m_a;
    int m_b;
    int m_c;

    GD53b2d0REC()
    {
        m_c = 0;
        m_a = -1;
        m_b = -1;
    }
};

class GD53b2d0 : public SHADERMASTER
{
public:
    __forceinline GD53b2d0(RESOURCEMANAGER *manager, const char *name)
        : SHADERMASTER(manager, name)
    {
        m_150 = false;
    }
    virtual ~GD53b2d0() throw();

    SHADER *Create(const RESHANDLE &handle);
    void Prepare();

    bool m_150;
    char m_151[3];
    GD53b2d0REC m_154[2];
    char m_16c[0x1b0 - 0x16c];
};

// The texture manager the binding pass asks for its textures (its slot is
// named g_pMusicX in the symbol file).
class TEXTUREDX8SDMASTER;
class GD53b2e0MANAGER
{
public:
    TEXTUREDX8SDMASTER *GetTextureResource(const RESHANDLE *handle);
};

extern void *g_pMusicX;

// 0x0053b2e0: the first copy builds the master's texture bindings: the
// binding pass, one texture reference per 12-byte handle at 0x154, then the
// constant block; the flag keeps it to once.
void GD53b2d0::Prepare()
{
    if (m_150)
        return;
    BuildBindings();
    for (int i = 0; i < m_count; i++)
        ((GD53b2e0MANAGER *)g_pMusicX)->GetTextureResource((const RESHANDLE *)&m_154[i]);
    ResetConstants();
    m_150 = true;
}

__declspec(noinline) GD53b2d0::~GD53b2d0() throw()
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

// The texture device the shader helpers talk to; its two texture calls live
// at 0x5478c0 and 0x547880 in the texture unit.
class G2DX8OBJECT;

class G2DX8DEVICE
{
public:
    void ReleaseTexture();
    void CreateTexture(G2DX8OBJECT *object);

    char pad_00[4];
    bool m_4;
};

// The two shader variants both hold the device at +0x30 and a flag at +0x54.
class G2DX8T1
{
public:
    void Call();

    char pad_00[0x30];
    G2DX8DEVICE *m_30;
    char pad_34[0x54 - 0x34];
    bool m_54;
};

class G2DX8T2
{
public:
    void Call(int index);

    char pad_00[0x30];
    G2DX8DEVICE *m_30;
    char pad_34[0x54 - 0x34];
    bool m_54;
};

void G2DX8T1::Call()
{
    if (m_54)
        m_30->ReleaseTexture();
}

void G2DX8T2::Call(int index)
{
    if (m_54)
        m_30->CreateTexture((G2DX8OBJECT *)index);
}

class SHADERITEMX
{
public:
    char pad_0[0x10b];
    char m_10b;
};

class SHADERITEM
{
public:
    char pad_0[0x98];
    SHADERITEMX *m_98;
};

class SHADERLIST
{
public:
    bool Any();

    // The retail array lives at +0x78; the vector object itself sits at +0x74
    // (its allocator takes the first four bytes).
    char pad_0[0x74];
    std::vector<SHADERITEM *> m_74;
};

bool SHADERLIST::Any()
{
    for (unsigned int i = 0; i < m_74.size(); i++) {
        SHADERITEM *item = m_74[i];
        if (item->m_98 && item->m_98->m_10b)
            return true;
    }
    return false;
}


class SHADERSETTERITEM
{
public:
    char m_0;
    void SetA(void *value);
    void SetB(void *value);
};

class SHADERSETTERLIST
{
public:
    void Set(unsigned int index, char value);
    void SetA(void *value, unsigned int index);
    void SetB(void *value, unsigned int index);

    char pad_0[0x74];
    std::vector<SHADERSETTERITEM *> m_78;
};

void SHADERSETTERLIST::Set(unsigned int index, char value)
{
    m_78[index]->m_0 = value;
}

void SHADERSETTERLIST::SetA(void *value, unsigned int index)
{
    m_78[index]->SetA(value);
}

void SHADERSETTERLIST::SetB(void *value, unsigned int index)
{
    m_78[index]->SetB(value);
}


// The setter list of a second shader type: its vector object starts at
// +0x68, so the element array is at +0x6c (retail 0x53b6b0).
class GD53b6b0
{
public:
    void Set(unsigned int index, char value);

    char pad_0[0x68];
    std::vector<SHADERSETTERITEM *> m_68;
};

void GD53b6b0::Set(unsigned int index, char value)
{
    m_68[index]->m_0 = value;
}


// Releases the object held at +0x10 through the helper at +0x8.
class M40TARGET
{
public:
    void m_40(void *item);
};

// The constructor at 0x53ca70: a flag byte, five cleared pointers and two
// members the constructor leaves alone (floats at +0x18 and +0x1c).
class GD53ca70
{
public:
    GD53ca70(void *a, void *b);

    bool m_0;
    void *m_4;
    void *m_8;
    void *m_c;
    void *m_10;
    void *m_14;
    float m_18;
    float m_1c;
    void *m_20;
    void *m_24;
};

GD53ca70::GD53ca70(void *a, void *b)
    : m_0(true), m_4(0), m_8(a), m_c(b), m_10(0), m_14(0), m_20(0), m_24(0)
{
}

class GD53caa0
{
public:
    void Release();

    char pad_0[8];
    M40TARGET *m_obj;
    char pad_mid[4];
    void *m_item;
};

void GD53caa0::Release()
{
    if (m_item)
        m_obj->m_40(m_item);
}


// Stores the helper's result: the object at +0x8 produces it,
// the member at +0x10 keeps it.
class M58TARGET
{
public:
    void *m_58(void *item);
};

class GD53cac0
{
public:
    void Set(void *item);

    char pad_0[8];
    M58TARGET *m_obj;
    char pad_mid[4];
    void *m_result;
};

void GD53cac0::Set(void *item)
{
    if (item)
        m_result = m_obj->m_58(item);
}


// The three raw-array fill helpers at the end of the retail unit
// (0x53de00, 0x53de30 and 0x53de60). Each one fills `count` elements of
// its own element size and returns the end of the filled range; the
// allocator sits at offset 0, so the helper is handed the object itself.
class GD53de00
{
public:
    SHADERTYPE20 *Fill(SHADERTYPE20 *first, int count, const SHADERTYPE20 &val);

    std::allocator<SHADERTYPE20> m_alloc;
};

SHADERTYPE20 *GD53de00::Fill(SHADERTYPE20 *first, int count, const SHADERTYPE20 &val)
{
    std::_Uninitialized_fill_n(first, (unsigned int)count, val, m_alloc);
    return first + count;
}

class GD53de30
{
public:
    SHADERTYPE24 *Fill(SHADERTYPE24 *first, int count, const SHADERTYPE24 &val);

    std::allocator<SHADERTYPE24> m_alloc;
};

SHADERTYPE24 *GD53de30::Fill(SHADERTYPE24 *first, int count, const SHADERTYPE24 &val)
{
    std::_Uninitialized_fill_n(first, (unsigned int)count, val, m_alloc);
    return first + count;
}

class GD53de60
{
public:
    SHADERTYPE2C *Fill(SHADERTYPE2C *first, int count, const SHADERTYPE2C &val);

    std::allocator<SHADERTYPE2C> m_alloc;
};

SHADERTYPE2C *GD53de60::Fill(SHADERTYPE2C *first, int count, const SHADERTYPE2C &val)
{
    std::_Uninitialized_fill_n(first, (unsigned int)count, val, m_alloc);
    return first + count;
}


// The two vector setters at 0x53d5d0 and 0x53d630: they append a value
// when the index is -1, otherwise they overwrite the entry at that index.
// The element type is the engine's 12-byte V3.
class GD53d5d0
{
public:
    GD53d5d0();
    ~GD53d5d0();
    void SetA(const V3 *value, int index);
    void SetB(const V3 *value, int index);
    void SetC(unsigned int a, unsigned int b, unsigned int c, int d, int index);
    void SetD(unsigned int a, unsigned int b, unsigned int c, int index);

    void *m_0;
    void *m_4;
    void *m_8;
    void *m_c;
    void *m_10;
    void *m_14;
    std::vector<int> m_18;
    std::vector<int> m_28;
    std::vector<V3> m_38;
    std::vector<V3> m_48;
    std::vector<int> m_58;
    std::vector<int> m_68;
    std::vector<int> m_78;
    std::vector<int> m_88;
    std::vector<int> m_98;
};

// The constructor at 0x53d520: it clears the six plain pointers and lets
// the ten vectors clear their own storage.
GD53d5d0::GD53d5d0()
    : m_0(0), m_4(0), m_8(0), m_c(0), m_10(0), m_14(0)
{
}

GD53d5d0::~GD53d5d0()
{
}

void GD53d5d0::SetA(const V3 *value, int index)
{
    if (index == -1) {
        m_38.push_back(*value);
    } else if (index < (int)m_38.size()) {
        m_38[index] = *value;
    }
}

void GD53d5d0::SetB(const V3 *value, int index)
{
    if (index == -1) {
        m_48.push_back(*value);
    } else if (index < (int)m_48.size()) {
        m_48[index] = *value;
    }
}

// The four parallel integer lists at +0x68, +0x78, +0x88 and +0x98.
// An index of -1 appends, any other index overwrites.
void GD53d5d0::SetC(unsigned int a, unsigned int b, unsigned int c, int d, int index)
{
    if (index == -1) {
        m_98.push_back(d);
        m_68.push_back(a);
        m_78.push_back(b);
        m_88.push_back(c);
    } else {
        if (index < (int)m_68.size()) {
            m_68[index] = a;
            m_78[index] = b;
            m_88[index] = c;
        }
        if (index < (int)m_98.size()) {
            m_98[index] = d;
        }
    }
}

// The thin wrapper at 0x53dab0: it fills in the fourth value itself.
void GD53d5d0::SetD(unsigned int a, unsigned int b, unsigned int c, int index)
{
    SetC(a, b, c, 1, index);
}


// ---------------------------------------------------------------------------
// 0x0053bae0: the constructor of the 0x78-byte shader record. A string at 0xc
// (the compiler's default constructor writes 0xf, 0 and the empty inline
// buffer), five plain members after it, a flag at 0x54 and two pointer vectors
// at 0x58 and 0x68.
class GD53bae0
{
public:
    GD53bae0();

    void *m_0;
    void *m_4;
    void *m_8;
    std::string m_c;
    void *m_28;
    void *m_2c;
    void *m_30;
    void *m_34;
    void *m_38;
    char m_3c[0x18];
    bool m_54;
    char m_55[3];
    std::vector<void *> m_58;
    std::vector<void *> m_68;
};

GD53bae0::GD53bae0()
    : m_0(0), m_4(0), m_8(0), m_28(0), m_2c(0), m_30(0), m_34(0), m_38(0),
      m_54(false)
{
}


// ---------------------------------------------------------------------------
// 0x0053b330: the master's copy factory. It allocates the 0x34-byte shader
// (SHADER is 0x30 bytes here and the copy adds the flag at 0x30) and hands the
// master and the handle to SHADER's constructor at 0x004c5ba0.
class SHADER
{
public:
    SHADER(SHADERMASTER *master, const RESHANDLE &handle);
    virtual void Virtual();

    char m_4[0x30 - 4];
};

class GD53b330 : public SHADER
{
public:
    GD53b330(SHADERMASTER *master, const RESHANDLE &handle)
        : SHADER(master, handle)
    {
        m_30 = true;
    }

    bool m_30;
};

SHADER *GD53b2d0::Create(const RESHANDLE &handle)
{
    return new GD53b330(this, handle);
}


// 0x0053b390: the manager builds the 0x1b0-byte master. Its constructor is
// inline, so the allocation, the base constructor and the two empty 12-byte
// records at 0x154 and 0x160 all land in the factory.
RESOURCEMASTER *SHADERDX8SDMANAGER::CreateMaster(const char *name)
{
    return new GD53b2d0(this, name);
}


// ---------------------------------------------------------------------------
// 0x0053b520: the factory of the 0x238-byte APP-derived record. It only runs
// APP's constructor at 0x0048ba40 and takes the new vtable.
class APP
{
public:
    APP();
    virtual ~APP();

    char m_pad[0x238 - 4];
};

class GD53b520 : public APP
{
public:
    GD53b520()
    {
    }
};

class GD53b520FACTORY
{
public:
    APP *Create();
};

APP *GD53b520FACTORY::Create()
{
    return new GD53b520;
}


// ---------------------------------------------------------------------------
// 0x0053b5e0: a list's element-wise comparison. The two lists keep 0x40-byte
// records between +4 and +8 (a leading word sits at +0), and the comparison
// walks them with the compiler's inline memcmp. A null base counts as empty.
struct SHADERTYPE40
{
    char m[0x40];
};

class GD53b5e0
{
public:
    int size() const
    {
        return m_4 ? (int)(m_8 - m_4) >> 6 : 0;
    }

    bool operator==(const GD53b5e0 &other) const;

    int m_0;
    char *m_4;
    char *m_8;
};

bool GD53b5e0::operator==(const GD53b5e0 &other) const
{
    if (other.size() != size())
        return false;

    for (unsigned int i = 0; i < (unsigned int)size(); i++) {
        int diff = memcmp(m_4 + i * 0x40, other.m_4 + i * 0x40, 0x40);
        if (diff != 0)
            return false;
    }
    return true;
}


// ---------------------------------------------------------------------------
// 0x0053bb50: the copy constructor of the 0x40-byte shader parameter block
// (four vectors) and 0x0053b6d0, its destructor.
class GD53bb50
{
public:
    GD53bb50(const GD53bb50 &other);
    ~GD53bb50();

    std::vector<int> m_0;
    std::vector<V3> m_10;
    std::vector<V3> m_20;
    std::vector<float> m_30;
};

GD53bb50::GD53bb50(const GD53bb50 &other)
{
    for (unsigned int i = 0; i < other.m_0.size(); i++) {
        int value = other.m_0[i];
        m_0.push_back(value);
        m_10.push_back(other.m_10[i]);
        m_20.push_back(other.m_20[i]);
        float scalar = other.m_30[i];
        m_30.push_back(scalar);
    }
}

GD53bb50::~GD53bb50()
{
}


// ---------------------------------------------------------------------------
// 0x0053aa20: the copy's release, the RESOURCE half of the engine's
// Virtual20: stop the copy while it is in use, give it back to its master and
// mark it uninitialized. The vtable slots are the engine's; only Stop (0x5c)
// is called here.
class GD53aa20BASE
{
public:
    virtual void V00();
    virtual void V01();
    virtual void V02();
    virtual void V03();
    virtual void V04();
    virtual void V05();
    virtual void V06();
    virtual void V07();
    virtual void V08();
    virtual void V09();
    virtual void V10();
    virtual void V11();
    virtual void V12();
    virtual void V13();
    virtual void V14();
    virtual void V15();
    virtual void V16();
    virtual void V17();
    virtual void V18();
    virtual void V19();
    virtual void V20();
    virtual void V21();
    virtual void V22();
    virtual void Stop();

    void Release();

    // The RESOURCE fields the release touches.
    char m_pad[0x18];
    bool m_active;
    void *m_next;
    void *m_prev;
    RESOURCEMASTER *m_master;
    bool m_initialized;
};

void GD53aa20BASE::Release()
{
    if (m_initialized) {
        if (m_active) {
            Stop();
            m_active = false;
        }
        m_master->ReleaseCopy((RESOURCE *)this);
        m_initialized = false;
    }
}


// ---------------------------------------------------------------------------
// 0x0053d190: draws the shader's batch: the material of the object at +0x10
// when it is switched on, then the indexed primitive, counting its triangles.
extern int g_numTriangles;

class GD53d190MATERIAL
{
public:
    char m_0[0x101];
    bool m_101;
};

class GD53d190
{
public:
    void Draw(IDirect3DDevice8 *device);

    int m_0;
    int m_4;
    int m_8;
    int m_c;
    GD53d190MATERIAL *m_10;
    int m_14;
    int m_18;
    int m_1c;
    int m_20;
    int m_24;
};

void GD53d190::Draw(IDirect3DDevice8 *device)
{
    if (m_10 && m_10->m_101)
        device->SetMaterial((const D3DMATERIAL8 *)(m_10->m_0 + 4));
    device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, m_20, m_24 - m_20 + 1, m_4, m_14);
    g_numTriangles += m_14;
}


// ---------------------------------------------------------------------------
// 0x0053cae0: the render states a shader's copy sets before drawing: the
// texture transform flag of its texture stage, then the Z and blend states
// for the material it uses.
class SETTINGSTABLE
{
public:
    int GetInt(int index);
};

class GD53cae0MATERIAL
{
public:
    bool m_85(int flag);

    char m_0[0x100];
    bool m_100;
    bool m_101;
    char m_102[3];
    bool m_105;
    bool m_106;
    char m_107[4];
    bool m_10b;
};

class GD53cae0
{
public:
    void Apply(IDirect3DDevice8 *device);

    int m_0;
    int m_4;
    int m_8;
    SETTINGSTABLE *m_c;
    GD53cae0MATERIAL *m_10;
};

void GD53cae0::Apply(IDirect3DDevice8 *device)
{
    if (!m_10 || !m_10->m_101)
        return;

    if (m_10->m_85(0)) {
        int count = m_c->GetInt(0xe);
        int stage = 0;
        if (count > 0 && m_10->m_10b)
            stage = 1;
        device->SetTextureStageState(stage, D3DTSS_TEXTURETRANSFORMFLAGS, 0);
    }

    if (m_10->m_106) {
        device->SetRenderState(D3DRS_ZWRITEENABLE, 1);
        device->SetRenderState(D3DRS_ALPHABLENDENABLE, 0);
        device->SetRenderState(D3DRS_DESTBLEND, 1);
    } else if (m_10->m_105) {
        if (!m_10->m_100)
            device->SetRenderState(D3DRS_ZWRITEENABLE, 1);
        device->SetRenderState(D3DRS_ALPHABLENDENABLE, 0);
        device->SetRenderState(D3DRS_SRCBLEND, 2);
        device->SetRenderState(D3DRS_DESTBLEND, 1);
    }
}

