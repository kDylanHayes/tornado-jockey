#include "Tongas2/VALUEMANAGER.h"
#include "Tongas2/ACTOR.h"
#include "Tongas2/LOG.h"
#include "Tongas2/PATH.h"
#include "Tongas2/CMDMGR.h"

#define CMD_FUNC(CLASS, method) (CMDFUNC)(SRESULT (CLASS::*)(TOKENS **))(SRESULT (CLASS::*)(CMDCALL *))&CLASS::method

static CMDBASE *CreateValueManager();

CMDBASE *(*g_pfnCreateValueManager)() = CreateValueManager;

SRESULT VALUEMANAGER::s_unset;

CMDENTRY VALUEMANAGER::s_commands[] = {
    { "add", 0, 1, 1, CMD_FUNC(VALUEMANAGER, Add) },
    { "set", 0, 2, 2, CMD_FUNC(VALUEMANAGER, Set) },
    { "seteval", 0, 2, 2, CMD_FUNC(VALUEMANAGER, Set) },
    { "delete", 0, 1, 1, CMD_FUNC(VALUEMANAGER, Delete) },
    { "exist", 0, 1, 1, CMD_FUNC(VALUEMANAGER, Exist) },
    { 0, 0, 0, 0, 0 },
};

void VALUEMANAGER::RegisterCommands()
{
    g_classParents[s_classId] = g_CMDBASEClassId;
    g_classCommands[s_classId] = s_commands;
    HashCommands(s_commands);
    RegisterClassName(s_commands, "VALUEMANAGER", "CMDBASE");
}

SRESULT VALUEMANAGER::GetProperty(PROPENTRY *property, int index)
{
    return *(SRESULT *)property;
}

bool VALUEMANAGER::Exists(const char *name)
{
    std::map<std::string, SRESULT *>::iterator it = m_values.find(name);
    return it != m_values.end();
}

PROPENTRY *VALUEMANAGER::FindProperty(const char *name, int classId)
{
    std::map<std::string, SRESULT *>::iterator it = m_values.find(name);
    if (it == m_values.end())
        return 0;
    return (PROPENTRY *)it->second;
}

SRESULT VALUEMANAGER::Exist(CMDCALL *call)
{
    return SRESULT(Exists(call->tokens->GetToken(1)));
}

VALUEMANAGER::~VALUEMANAGER()
{
    for (std::map<std::string, SRESULT *>::iterator it = m_values.begin(); it != m_values.end(); ++it) {
        if (it->second != &s_unset && it->second) {
            delete it->second;
            it->second = 0;
        }
    }
    m_values.clear();
}

int VALUEMANAGER::GetClassId()
{
    return s_classId;
}

int VALUEMANAGER::GetParentClassId()
{
    return g_classParents[s_classId];
}

const char *VALUEMANAGER::GetClassName()
{
    return g_classNames[s_classId];
}

SRESULT VALUEMANAGER::Execute(CMDENTRY *command, TOKENS **args)
{
    return (this->*(SRESULT (VALUEMANAGER::*)(TOKENS **))command->func)(args);
}

VALUEMANAGER::VALUEMANAGER()
{
    CSTRING name("var");
    if (m_name != name)
        m_name = name.c_str();
}

static CMDBASE *CreateValueManager()
{
    return new VALUEMANAGER;
}

SRESULT VALUEMANAGER::Add(CMDCALL *call)
{
    const char *name = call->tokens->GetToken(1);
    if (Exists(name)) {
        if (g_pLog)
            g_pLog->Error("could not add variable '%s', already exists!", name);
        return SRESULT(false);
    }
    m_values[name] = &s_unset;
    return SRESULT(true);
}

SRESULT VALUEMANAGER::Delete(CMDCALL *call)
{
    const char *name = call->tokens->GetToken(1);
    if (!Exists(name)) {
        if (g_pLog)
            g_pLog->Error("could not delete variable '%s', does not exist!", name);
        return SRESULT(false);
    }
    if (m_values[name] != &s_unset) {
        if (m_values[name]) {
            delete m_values[name];
            m_values[name] = 0;
        }
    }
    m_values.erase(name);
    return SRESULT(true);
}

SRESULT VALUEMANAGER::Set(CMDCALL *call)
{
    const char *name = call->tokens->GetToken(1);
    std::map<std::string, SRESULT *>::iterator it = m_values.find(name);
    if (it == m_values.end()) {
        if (g_pLog)
            g_pLog->Error("could not set variable '%s', does not exist!", name);
        return SRESULT(false);
    }
    SRESULT *value = new SRESULT;
    {
        STRING expression(call->tokens->GetToken(2));
        value->Assign(g_pCmdMgr->Evaluate(expression, call->caller));
    }
    if (m_values[name] != &s_unset) {
        if (m_values[name]) {
            delete m_values[name];
            m_values[name] = 0;
        }
    }
    m_values[name] = value;
    return SRESULT(*value);
}
