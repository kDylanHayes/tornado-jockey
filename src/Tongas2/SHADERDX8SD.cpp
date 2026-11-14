// Tongas2/SHADERDX8SD -- the SD renderer's shaders.
//
// Discovery scaffold: bodies are empty until the unit is matched; each
// function has its exact target size in config/symbols.txt so a later delink
// cannot cut the target short. Apply's extent includes its switch tables.
#include "Tongas2/SHADERDX8SD.h"

#include "Tongas2/MATHS.h"
#include "Tongas2/SAFEINDEX.h"
#include <vector>
#include <memory>

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
class RESOURCEMASTER;
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

class SHADERMASTER
{
public:
    virtual ~SHADERMASTER();
};

class GD53aa10 : public RESOURCE
{
public:
    virtual ~GD53aa10() throw();
};

__declspec(noinline) GD53aa10::~GD53aa10() throw()
{
}

class GD53b2d0 : public SHADERMASTER
{
public:
    virtual ~GD53b2d0() throw();
};

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
