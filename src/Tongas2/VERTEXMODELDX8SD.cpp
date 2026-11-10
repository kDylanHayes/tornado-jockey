// Tongas2/VERTEXMODELDX8SD -- the DX8 shadowed vertex-model resources.
//
// Discovery scaffold: bodies are empty until the unit is matched.
#include "Tongas2/VERTEXMODELDX8SD.h"
#include "Tongas2/BONEDMODEL.h"


RESOURCEMANAGER *CreateVERTEXMODELDX8SD()
{
    return 0;
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
