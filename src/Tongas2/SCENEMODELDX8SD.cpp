// Tongas2/SCENEMODELDX8SD -- the DX8 shadowed scene-model resources.
//
// Discovery scaffold: bodies are empty until the unit is matched.
#include "Tongas2/SCENEMODELDX8SD.h"
#include "Tongas2/RESOURCE.h"

void SCENEMODELDX8SDMASTER::RegisterCommands()
{
}

RESOURCEMANAGER *CreateSCENEMODELDX8SD()
{
    return 0;
}

// A resource master with two allocated names.
class GD537230 : public RESOURCEMASTER
{
public:
    virtual ~GD537230() throw();

    CSTRING m_60;
    char pad_64[0xc];
    CSTRING m_70;
};

__declspec(noinline) GD537230::~GD537230() throw()
{
}

class GD536D90DISPLAY
{
public:
    void m_44(void *arg);
};

class APPWINDOW
{
public:
    char pad_2bc[0x2bc];
    GD536D90DISPLAY *m_2bc;
};

extern APPWINDOW *g_pDisplay;

// The second base; only its vptr, destructor and +0x2c handle matter here.
class HEIGHTMAPTEMP
{
public:
    virtual ~HEIGHTMAPTEMP();

    char pad_04[0x28];
    void *m_2c;
};

class GD536d90 : public OBJECT, public HEIGHTMAPTEMP
{
public:
    virtual ~GD536d90();
};

GD536d90::~GD536d90()
{
    GD536D90DISPLAY *display = g_pDisplay->m_2bc;
    display->m_44(m_2c);
}

extern "C" char DAT_0067a410;
void *GetD0067a410()
{
    return &DAT_0067a410;
}

struct SMDPTR
{
    void *Get0();
};

void *SMDPTR::Get0()
{
    return (char *)this - 0x23c;
}

extern int g_classParents[];
extern char g_classNames[][30];

class SCENEMODELDX8CID
{
public:
    virtual int GetClassId();
    virtual int GetParentClassId();
    virtual const char *GetClassName();
    void Set65(const bool &value);
    void Set6F(const bool &value);

    static int s_classId;
};

int SCENEMODELDX8CID::GetClassId()
{
    return s_classId;
}

int SCENEMODELDX8CID::GetParentClassId()
{
    return g_classParents[s_classId];
}

const char *SCENEMODELDX8CID::GetClassName()
{
    return g_classNames[s_classId];
}

void SCENEMODELDX8CID::Set65(const bool &value)
{
    *(bool *)((char *)this + 0x65) = value;
}

void SCENEMODELDX8CID::Set6F(const bool &value)
{
    *(bool *)((char *)this + 0x6f) = value;
}
