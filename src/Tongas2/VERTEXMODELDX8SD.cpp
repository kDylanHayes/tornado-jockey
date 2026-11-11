// Tongas2/VERTEXMODELDX8SD -- the DX8 shadowed vertex-model resources.
//
// Discovery scaffold: bodies are empty until the unit is matched.
#include "Tongas2/VERTEXMODELDX8SD.h"
#include "Tongas2/BONEDMODEL.h"


RESOURCEMANAGER *CreateVERTEXMODELDX8SD()
{
    return new VERTEXMODELDX8SDMASTER();
}

class GD535df0
{
public:
    virtual ~GD535df0();
};

__declspec(noinline) GD535df0::~GD535df0()
{
}

extern "C" char DAT_00679fdc;
void *GetD00679fdc()
{
    return &DAT_00679fdc;
}

extern int g_classParents[];
extern char g_classNames[][30];

class VERTEXMODELDX8CID
{
public:
    virtual int GetClassId();
    virtual int GetParentClassId();
    virtual const char *GetClassName();

    static int s_classId;
};

int VERTEXMODELDX8CID::GetClassId()
{
    return s_classId;
}

int VERTEXMODELDX8CID::GetParentClassId()
{
    return g_classParents[s_classId];
}

const char *VERTEXMODELDX8CID::GetClassName()
{
    return g_classNames[s_classId];
}

CMDENTRY VERTEXMODELDX8SDMASTER::s_commands[] = {
    { 0, 0, 0, 0, 0 },
};

void VERTEXMODELDX8SDMASTER::RegisterCommands()
{
    g_classParents[VERTEXMODELDX8CID::s_classId] = BONEDMODELMASTER::s_classId;
    g_classCommands[VERTEXMODELDX8CID::s_classId] = s_commands;
    HashCommands(s_commands);
    RegisterClassName(s_commands, "VERTEXMODELDX8SDMASTER", "BONEDMODELMASTER");
}


// The base the shadowed vertex-model resource is built on; its constructor
// lives in the resource's own unit.
class BONEDMODELDX8MASTER
{
public:
    BONEDMODELDX8MASTER(RESOURCEMANAGER *manager, const char *name);
    virtual ~BONEDMODELDX8MASTER();
    virtual void V00();
};

// The resource the factory makes (0x8c bytes).
class VERTEXMODELDX8SDRESOURCE : public BONEDMODELDX8MASTER
{
public:
    VERTEXMODELDX8SDRESOURCE(RESOURCEMANAGER *manager, const char *name)
        : BONEDMODELDX8MASTER(manager, name)
    {
    }

    char pad_88[0x8c - sizeof(BONEDMODELDX8MASTER)];
};

// The manager the factory belongs to.
class GD5362b0MANAGER
{
public:
    VERTEXMODELDX8SDRESOURCE *CreateVERTEXMODELDX8SD(const char *name);
};

VERTEXMODELDX8SDRESOURCE *GD5362b0MANAGER::CreateVERTEXMODELDX8SD(const char *name)
{
    return new VERTEXMODELDX8SDRESOURCE((RESOURCEMANAGER *)this, name);
}
