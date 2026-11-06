// Tongas2/BONEDMODELDX8SD -- the DX8 renderer plugin's skinned boned models.
#include "Tongas2/BONEDMODELDX8SD.h"
#include "Tongas2/LOG.h"
#include "Tongas2/TIMER.h"

void FUN_004eb960(int);

#define CMD_FUNC(CLASS, method) (CMDFUNC)(SRESULT (CLASS::*)(TOKENS **))(SRESULT (CLASS::*)(CMDCALL *))&CLASS::method

int BONEDMODELDX8SDMASTER::s_classId;
int BONEDMODELDX8SDRESOURCE::s_classId;

CMDENTRY BONEDMODELDX8SDMASTER::s_commands[] = {
    { 0, 0, 0, 0, 0 },
};

CMDENTRY BONEDMODELDX8SDRESOURCE::s_commands[] = {
    { "LoadExtraAnimation", 0, 1, 1, CMD_FUNC(BONEDMODELDX8SDRESOURCE, LoadExtraAnimation) },
    { 0, 0, 0, 0, 0 },
};

BONEDMODELDX8SDMASTER::BONEDMODELDX8SDMASTER(RESOURCEMANAGER *manager, const char *name)
    : BONEDMODELMASTER(manager, name)
{
    m_bounds.set = false;
}

int BONEDMODELDX8SDMASTER::GetClassId()
{
    return s_classId;
}

int BONEDMODELDX8SDMASTER::GetParentClassId()
{
    return g_classParents[s_classId];
}

const char *BONEDMODELDX8SDMASTER::GetClassName()
{
    return g_classNames[s_classId];
}

SRESULT BONEDMODELDX8SDMASTER::Execute(CMDENTRY *command, TOKENS **args)
{
    return (this->*(SRESULT (BONEDMODELDX8SDMASTER::*)(TOKENS **))command->func)(args);
}

void BONEDMODELDX8SDMASTER::RegisterCommands()
{
    g_classParents[s_classId] = BONEDMODELMASTER::s_classId;
    g_classCommands[s_classId] = s_commands;
    HashCommands(s_commands);
    RegisterClassName(s_commands, "BONEDMODELDX8SDMASTER", "BONEDMODELMASTER");
}

void BONEDMODELDX8SDRESOURCE::Start()
{
    BONEDMODELRESOURCE::Start();
    FUN_004eb960(1);
    if (m_master) {
        m_424 = true;
        m_420 = *(int *)((char *)m_master + 100);
    }
}

void BONEDMODELDX8SDRESOURCE::Stop()
{
    FUN_004eb960(0);
    BONEDMODELRESOURCE::Stop();
}

void BONEDMODELDX8SDRESOURCE::RegisterCommands()
{
    g_classParents[s_classId] = RESOURCE::s_classId;
    g_classCommands[s_classId] = s_commands;
    HashCommands(s_commands);
    RegisterClassName(s_commands, "BONEDMODELDX8SDRESOURCE", "RESOURCE");
}

RESOURCEMASTER *BONEDMODELMANAGER::CreateMaster(const char *name)
{
    return new BONEDMODELMASTER(this, name);
}

const char *BONEDMODELMANAGER::GetTypeCode()
{
    return "BMDL";
}

RESOURCEMASTER *BONEDMODELDX8SDMANAGER::CreateMaster(const char *name)
{
    return new BONEDMODELDX8SDMASTER(this, name);
}

const char *BONEDMODELDX8SDMANAGER::GetTypeCode()
{
    return "BMD8";
}

RESOURCEMANAGER *CreateBONEDMODELDX8SD()
{
    return new BONEDMODELDX8SDMANAGER;
}

SRESULT BONEDMODELDX8SDRESOURCE::LoadExtraAnimation(CMDCALL *call)
{
    return SRESULT(FUN_004f7040(call->tokens->GetToken(1)));
}
