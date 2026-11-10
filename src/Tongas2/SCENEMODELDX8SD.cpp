// Tongas2/SCENEMODELDX8SD -- the DX8 shadowed scene-model resources.
//
// Discovery scaffold: bodies are empty until the unit is matched.
#include "Tongas2/SCENEMODELDX8SD.h"

void SCENEMODELDX8SDMASTER::RegisterCommands()
{
}

RESOURCEMANAGER *CreateSCENEMODELDX8SD()
{
    return 0;
}

class GD537230
{
public:
    virtual ~GD537230();
};

__declspec(noinline) GD537230::~GD537230()
{
}

class GD536d90
{
public:
    virtual ~GD536d90();
};

__declspec(noinline) GD536d90::~GD536d90()
{
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
