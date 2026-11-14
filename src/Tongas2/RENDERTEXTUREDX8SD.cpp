// Tongas2/RENDERTEXTUREDX8SD -- the SD renderer's render textures.
//
// Discovery scaffold: bodies are empty until the unit is matched; each
// function has its exact target size in config/symbols.txt so a later delink
// cannot cut the target short.
#include "Tongas2/RENDERTEXTUREDX8SD.h"
#include "Tongas2/APPWINDOW.h"
#include "Tongas2/RESOURCES.h"
#include "Tongas2/RENDERTEXTURE.h"
#include "Tongas2/TEXTUREDX8SD.h"

int SafeSprintf(char *buf, const char *format, ...);

// The plain render-texture manager and its SD subclass; the factory builds
// the latter (0x539950).
class RENDERTEXTUREMANAGER : public RESOURCEMANAGER
{
public:
    RENDERTEXTUREMANAGER();
};

RENDERTEXTUREMANAGER::RENDERTEXTUREMANAGER()
{
    SafeSprintf(m_fileName, "RENDERTEXTURE.txt");
    m_8 = false;
    m_18 = true;
}

// The SD renderer's render-texture master. It adds one word after the
// RENDERTEXTUREMASTER layout and its own vtable (0x67a760).
class RENDERTEXTUREDX8SDMASTER : public RENDERTEXTUREMASTER
{
public:
    RENDERTEXTUREDX8SDMASTER(RESOURCEMANAGER *manager, const char *name)
        : RENDERTEXTUREMASTER(manager, name)
    {
        m_8c = 0;
    }

    virtual ~RENDERTEXTUREDX8SDMASTER();

    // Retail 0x5399e0: drops the scratch texture the master owns and
    // reports that it did.
    virtual bool ReleaseScratchTexture();

    // The scratch texture, at +0x8c; the master is 0x90 bytes long.
    TEXTUREDX8SDTEXTURE *m_8c;
};

class RENDERTEXTUREDX8SDMANAGER : public RENDERTEXTUREMANAGER
{
public:
    RENDERTEXTUREDX8SDMANAGER()
    {
    }

    virtual RESOURCEMASTER *CreateMaster(const char *name);
};

RESOURCEMASTER *RENDERTEXTUREDX8SDMANAGER::CreateMaster(const char *name)
{
    return new RENDERTEXTUREDX8SDMASTER(this, name);
}

void *CreateRENDERTEXTUREDX8SD()
{
    return new RENDERTEXTUREDX8SDMANAGER();
}

// The engine's renderer base; only its destructor matters here.
class RENDERER
{
public:
    virtual ~RENDERER();
};

class GD539e70 : public RENDERER
{
public:
    virtual ~GD539e70() throw();
};

__declspec(noinline) GD539e70::~GD539e70() throw()
{
}

struct G2RTDSET
{
    void Set31d0(int value);
};

void G2RTDSET::Set31d0(int value)
{
    *(int *)((char *)this + 0x31d0) = value;
}

class RTKbc
{
public:
    virtual void d0(); virtual void d1(); virtual void d2(); virtual void d3(); virtual void d4(); virtual void d5(); virtual void d6(); virtual void d7();
    virtual void d8(); virtual void d9(); virtual void d10(); virtual void d11(); virtual void d12(); virtual void d13(); virtual void d14(); virtual void d15();
    virtual void d16(); virtual void d17(); virtual void d18(); virtual void d19(); virtual void d20(); virtual void d21(); virtual void d22(); virtual void d23();
    virtual void d24(); virtual void d25(); virtual void d26(); virtual void d27(); virtual void d28(); virtual void d29(); virtual void d30(); virtual void d31();
    virtual void d32(); virtual void d33(); virtual void d34(); virtual void d35(); virtual void d36(); virtual void d37(); virtual void d38(); virtual void d39();
    virtual void d40(); virtual void d41(); virtual void d42(); virtual void d43(); virtual void d44(); virtual void d45(); virtual void d46();
    virtual void T();
    void Thunk();
};

void RTKbc::Thunk()
{
    T();
}

class RTKc0
{
public:
    virtual void d0(); virtual void d1(); virtual void d2(); virtual void d3(); virtual void d4(); virtual void d5(); virtual void d6(); virtual void d7();
    virtual void d8(); virtual void d9(); virtual void d10(); virtual void d11(); virtual void d12(); virtual void d13(); virtual void d14(); virtual void d15();
    virtual void d16(); virtual void d17(); virtual void d18(); virtual void d19(); virtual void d20(); virtual void d21(); virtual void d22(); virtual void d23();
    virtual void d24(); virtual void d25(); virtual void d26(); virtual void d27(); virtual void d28(); virtual void d29(); virtual void d30(); virtual void d31();
    virtual void d32(); virtual void d33(); virtual void d34(); virtual void d35(); virtual void d36(); virtual void d37(); virtual void d38(); virtual void d39();
    virtual void d40(); virtual void d41(); virtual void d42(); virtual void d43(); virtual void d44(); virtual void d45(); virtual void d46(); virtual void d47();
    virtual void T();
    void Thunk();
};

void RTKc0::Thunk()
{
    T();
}

extern "C" char DAT_0067abb8;
void *GetD0067abb8()
{
    return &DAT_0067abb8;
}

class SETTINGSTABLE
{
public:
    int GetInt(int index);
};

bool DisplaySetting10()
{
    SETTINGSTABLE *table = *(SETTINGSTABLE **)((char *)g_pDisplay + 0x2c0);
    return table->GetInt(10) != 0;
}

void *DisplayFieldEC()
{
    return *(void **)((char *)g_pDisplay + 0xec);
}

// Retail 0x5399e0: RENDERTEXTUREDX8SDMASTER::ReleaseScratchTexture. The
// master owns one scratch texture at +0x8c; it is destroyed and the word
// cleared, and the call always succeeds.
bool RENDERTEXTUREDX8SDMASTER::ReleaseScratchTexture()
{
    if (m_8c)
    {
        delete m_8c;
        m_8c = 0;
    }
    return true;
}

// Retail 0x539a10: RENDERTEXTUREDX8SDMASTER::~RENDERTEXTUREDX8SDMASTER.
// The retail symbol for it is the stale "?GetRENDERTEXTUREFormat@@YAHXZ";
// the scalar deleting destructor that follows it is retail 0x539a80.
RENDERTEXTUREDX8SDMASTER::~RENDERTEXTUREDX8SDMASTER()
{
    if (m_8c)
    {
        delete m_8c;
        m_8c = 0;
    }
}

