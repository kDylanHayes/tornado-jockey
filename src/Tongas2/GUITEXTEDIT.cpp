// This file was built seeing SRESULT::FreeValue as one that can throw.
#define SRESULT_FREEVALUE_MAY_THROW

#include "Tongas2/GUITEXTEDIT.h"
#include "Tongas2/PATH.h"

class GUITEXTEDITMANAGER : public RESOURCEMANAGER
{
public:
    GUITEXTEDITMANAGER()
    {
        SafeSprintf(m_fileName, "GUITEXTEDIT.txt");
        m_8 = false;
        m_18 = true;
    }

    virtual RESOURCEMASTER *CreateMaster(const char *name) { return new GUITEXTEDITMASTER(this, name); }
    virtual const char *GetTypeCode();
};

static RESOURCEMANAGER *CreateGUITEXTEDIT();

CREATEMANAGER g_pfnCreateGUITEXTEDIT = CreateGUITEXTEDIT;

CMDENTRY GUITEXTEDITMASTER::s_commands[] = {
    { 0, 0, 0, 0, 0 },
};

PROPENTRY GUITEXTEDITMASTER::s_properties[] = {
    { "CaretChar", 0, 4, 0, PROP_FIELD(GUITEXTEDITMASTER, m_caretChar), false, GUITEXT_MAX, PROP_SETTER(GUITEXTEDITMASTER, SetCaretChar) },
    { "CaretBlink", 0, 2, 0, PROP_FIELD(GUITEXTEDITMASTER, m_caretBlink), false, GUITEXT_MAX, PROP_SETTER(GUITEXTEDITMASTER, SetCaretBlink) },
    { "Separators", 0, 4, 0, PROP_FIELD(GUITEXTEDITMASTER, m_separators), false, GUITEXT_MAX, PROP_SETTER(GUITEXTEDITMASTER, SetSeparators) },
    { "MaxLen", 0, 0, 0, PROP_FIELD(GUITEXTEDITMASTER, m_maxLen), false, GUITEXT_MAX, PROP_SETTER(GUITEXTEDITMASTER, SetMaxLen) },
    { "LimitChars", 0, 4, 0, PROP_FIELD(GUITEXTEDITMASTER, m_limitChars), false, GUITEXT_MAX, PROP_SETTER(GUITEXTEDITMASTER, SetLimitChars) },
    { "OnEnterKey", 0, 7, 0, PROP_FIELD(GUITEXTEDITMASTER, m_onEnterKey), false, GUITEXT_MAX, PROP_SETTER(GUITEXTEDITMASTER, SetOnEnterKey) },
    { "DontAllowStartSpaces", 0, 2, 0, PROP_FIELD(GUITEXTEDITMASTER, m_dontAllowStartSpaces), false, GUITEXT_MAX, PROP_SETTER(GUITEXTEDITMASTER, SetDontAllowStartSpaces) },
    { 0, 0, 15, 0, 0, false },
};

void GUITEXTEDITMASTER::RegisterCommands()
{
    g_classParents[s_classId] = GUITEXTMASTER::s_classId;
    g_classCommands[s_classId] = s_commands;
    HashCommands(s_commands);
    RegisterClassName(s_commands, "GUITEXTEDITMASTER", "GUITEXTMASTER");
}

const char *GUITEXTEDITMANAGER::GetTypeCode()
{
    return "GTXE";
}

static RESOURCEMANAGER *CreateGUITEXTEDIT()
{
    return new GUITEXTEDITMANAGER;
}

GUITEXTEDITMASTER::GUITEXTEDITMASTER(RESOURCEMANAGER *manager, const char *name)
    : GUITEXTMASTER(manager, name)
{
    for (g_i = 0; g_i < GUITEXT_MAX; g_i++)
        m_caretChar[g_i] = L"_";
    for (g_i = 0; g_i < GUITEXT_MAX; g_i++)
        m_caretBlink[g_i] = true;
    for (g_i = 0; g_i < GUITEXT_MAX; g_i++)
        m_separators[g_i] = L"";
    for (g_i = 0; g_i < GUITEXT_MAX; g_i++)
        m_maxLen[g_i] = 30;
    for (g_i = 0; g_i < GUITEXT_MAX; g_i++)
        m_limitChars[g_i] = L"";
    for (g_i = 0; g_i < GUITEXT_MAX; g_i++)
        m_onEnterKey[g_i] = 0;
    for (g_i = 0; g_i < GUITEXT_MAX; g_i++)
        m_dontAllowStartSpaces[g_i] = false;
}

int GUITEXTEDITMASTER::GetClassId()
{
    return s_classId;
}

int GUITEXTEDITMASTER::GetParentClassId()
{
    return g_classParents[s_classId];
}

const char *GUITEXTEDITMASTER::GetClassName()
{
    return g_classNames[s_classId];
}
