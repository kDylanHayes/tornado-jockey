// Tongas2/SHADERDX8SD -- the SD renderer's shaders.
//
// Discovery scaffold: bodies are empty until the unit is matched; each
// function has its exact target size in config/symbols.txt so a later delink
// cannot cut the target short. Apply's extent includes its switch tables.
#include "Tongas2/SHADERDX8SD.h"

#include "Tongas2/SAFEINDEX.h"
#include <vector>

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

    char pad_0[0x78];
    std::vector<SHADERITEM *> m_78;
};

bool SHADERLIST::Any()
{
    if (m_78.empty())
        return false;
    for (unsigned int i = 0; i < m_78.size(); i++) {
        SHADERITEM *item = m_78[i];
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
