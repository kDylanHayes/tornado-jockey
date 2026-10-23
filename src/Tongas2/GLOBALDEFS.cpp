// This file was built seeing SRESULT::FreeValue as one that can throw.
#define SRESULT_FREEVALUE_MAY_THROW

#include "Tongas2/GLOBALDEFS.h"

class GLOBALDEFSMANAGER : public RESOURCEMANAGER
{
public:
    GLOBALDEFSMANAGER()
    {
        SafeSprintf(m_fileName, "GLOBALDEFS.txt");
        m_8 = false;
        m_18 = true;
    }

    virtual RESOURCEMASTER *CreateMaster(const char *name) { return new GLOBALDEFSMASTER(this, name); }
    virtual const char *GetTypeCode();
};

static RESOURCEMANAGER *CreateGLOBALDEFS();

CREATEMANAGER g_pfnCreateGLOBALDEFS = CreateGLOBALDEFS;

CMDENTRY GLOBALDEFSMASTER::s_commands[] = {
    { 0, 0, 0, 0, 0 },
};

PROPENTRY GLOBALDEFSMASTER::s_properties[] = {
    { "Value", 0, 8, 0, PROP_FIELD(GLOBALDEFSMASTER, m_value), false, 1, PROP_SETTER(GLOBALDEFSMASTER, SetValue) },
    { 0, 0, 15, 0, 0, false },
};

void GLOBALDEFSMASTER::RegisterCommands()
{
    g_classParents[s_classId] = RESOURCEMASTER::s_classId;
    g_classCommands[s_classId] = s_commands;
    HashCommands(s_commands);
    RegisterClassName(s_commands, "GLOBALDEFSMASTER", "RESOURCEMASTER");
}

const char *GLOBALDEFSMANAGER::GetTypeCode()
{
    return "DEFS";
}

static RESOURCEMANAGER *CreateGLOBALDEFS()
{
    return new GLOBALDEFSMANAGER;
}

GLOBALDEFSMASTER::GLOBALDEFSMASTER(RESOURCEMANAGER *manager, const char *name)
    : RESOURCEMASTER(manager, name)
{
    m_value = "";
}

int GLOBALDEFSMASTER::GetClassId()
{
    return s_classId;
}

int GLOBALDEFSMASTER::GetParentClassId()
{
    return g_classParents[s_classId];
}

const char *GLOBALDEFSMASTER::GetClassName()
{
    return g_classNames[s_classId];
}

SRESULT GLOBALDEFSMASTER::Execute(CMDENTRY *command, TOKENS **args)
{
    return (this->*(SRESULT (GLOBALDEFSMASTER::*)(TOKENS **))command->func)(args);
}
