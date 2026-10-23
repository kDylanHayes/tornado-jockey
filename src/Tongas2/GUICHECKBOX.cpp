// This file was built seeing SRESULT::FreeValue as one that can throw.
#define SRESULT_FREEVALUE_MAY_THROW

#include "Tongas2/GUICHECKBOX.h"

class GUICHECKBOXMANAGER : public RESOURCEMANAGER
{
public:
    GUICHECKBOXMANAGER()
    {
        SafeSprintf(m_fileName, "GUICHECKBOX.txt");
        m_8 = false;
        m_18 = true;
    }

    virtual RESOURCEMASTER *CreateMaster(const char *name) { return new GUICHECKBOXMASTER(this, name); }
    virtual const char *GetTypeCode();
};

static RESOURCEMANAGER *CreateGUICHECKBOX();

CREATEMANAGER g_pfnCreateGUICHECKBOX = CreateGUICHECKBOX;

CMDENTRY GUICHECKBOXMASTER::s_commands[] = {
    { 0, 0, 0, 0, 0 },
};

PROPENTRY GUICHECKBOXMASTER::s_properties[] = {
    { "OnCheckOn", 0, 8, 0, PROP_FIELD(GUICHECKBOXMASTER, m_onCheckOn), false, 1, PROP_SETTER(GUICHECKBOXMASTER, SetOnCheckOn) },
    { "OnCheckOff", 0, 8, 0, PROP_FIELD(GUICHECKBOXMASTER, m_onCheckOff), false, 1, PROP_SETTER(GUICHECKBOXMASTER, SetOnCheckOff) },
    { 0, 0, 15, 0, 0, false },
};

void GUICHECKBOXMASTER::RegisterCommands()
{
    g_classParents[s_classId] = GUIGROUPMASTER::s_classId;
    g_classCommands[s_classId] = s_commands;
    HashCommands(s_commands);
    RegisterClassName(s_commands, "GUICHECKBOXMASTER", "GUIGROUPMASTER");
}

const char *GUICHECKBOXMANAGER::GetTypeCode()
{
    return "GCHK";
}

static RESOURCEMANAGER *CreateGUICHECKBOX()
{
    return new GUICHECKBOXMANAGER;
}

int GUICHECKBOXMASTER::GetClassId()
{
    return s_classId;
}

int GUICHECKBOXMASTER::GetParentClassId()
{
    return g_classParents[s_classId];
}

const char *GUICHECKBOXMASTER::GetClassName()
{
    return g_classNames[s_classId];
}

void GUICHECKBOXMASTER::SetOnCheckOn(const STRING &command)
{
    m_onCheckOn = command;
}

void GUICHECKBOXMASTER::SetOnCheckOff(const STRING &command)
{
    m_onCheckOff = command;
}

GUICHECKBOXMASTER::GUICHECKBOXMASTER(RESOURCEMANAGER *manager, const char *name)
    : GUIGROUPMASTER(manager, name)
{
    m_onCheckOn = "";
    m_onCheckOff = "";
}
