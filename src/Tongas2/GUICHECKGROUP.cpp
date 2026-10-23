// This file was built seeing SRESULT::FreeValue as one that can throw.
#define SRESULT_FREEVALUE_MAY_THROW

#include "Tongas2/GUICHECKGROUP.h"

class GUICHECKGROUPMANAGER : public RESOURCEMANAGER
{
public:
    GUICHECKGROUPMANAGER()
    {
        SafeSprintf(m_fileName, "GUICHECKGROUP.txt");
        m_8 = false;
        m_18 = true;
    }

    virtual RESOURCEMASTER *CreateMaster(const char *name) { return new GUICHECKGROUPMASTER(this, name); }
    virtual const char *GetTypeCode();
};

static RESOURCEMANAGER *CreateGUICHECKGROUP();

CREATEMANAGER g_pfnCreateGUICHECKGROUP = CreateGUICHECKGROUP;

CMDENTRY GUICHECKGROUPMASTER::s_commands[] = {
    { 0, 0, 0, 0, 0 },
};

PROPENTRY GUICHECKGROUPMASTER::s_properties[] = {
    { "MinSelected", 0, 0, 0, PROP_FIELD(GUICHECKGROUPMASTER, m_minSelected), false, 1, PROP_SETTER(GUICHECKGROUPMASTER, SetMinSelected) },
    { "MaxSelected", 0, 0, 0, PROP_FIELD(GUICHECKGROUPMASTER, m_maxSelected), false, 1, PROP_SETTER(GUICHECKGROUPMASTER, SetMaxSelected) },
    { 0, 0, 15, 0, 0, false },
};

void GUICHECKGROUPMASTER::RegisterCommands()
{
    g_classParents[s_classId] = GUIGROUPMASTER::s_classId;
    g_classCommands[s_classId] = s_commands;
    HashCommands(s_commands);
    RegisterClassName(s_commands, "GUICHECKGROUPMASTER", "GUIGROUPMASTER");
}

int GUICHECKGROUPMASTER::GetClassId()
{
    return s_classId;
}

int GUICHECKGROUPMASTER::GetParentClassId()
{
    return g_classParents[s_classId];
}

const char *GUICHECKGROUPMASTER::GetClassName()
{
    return g_classNames[s_classId];
}

const char *GUICHECKGROUPMANAGER::GetTypeCode()
{
    return "GCHG";
}

static RESOURCEMANAGER *CreateGUICHECKGROUP()
{
    return new GUICHECKGROUPMANAGER;
}
