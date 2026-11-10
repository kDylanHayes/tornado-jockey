// Tongas2/SHADERDX8SD -- the SD renderer's shaders.
//
// Discovery scaffold: bodies are empty until the unit is matched; each
// function has its exact target size in config/symbols.txt so a later delink
// cannot cut the target short. Apply's extent includes its switch tables.
#include "Tongas2/SHADERDX8SD.h"

#include "Tongas2/SAFEINDEX.h"

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
