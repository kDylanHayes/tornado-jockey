// This file was built seeing SRESULT::FreeValue as one that can throw.
#define SRESULT_FREEVALUE_MAY_THROW

#include "Tongas2/GUISLIDER.h"

class GUISLIDERMANAGER : public RESOURCEMANAGER
{
public:
    GUISLIDERMANAGER()
    {
        SafeSprintf(m_fileName, "GUISLIDER.txt");
        m_8 = false;
        m_18 = true;
    }

    virtual RESOURCEMASTER *CreateMaster(const char *name) { return new GUISLIDERMASTER(this, name); }
    virtual const char *GetTypeCode();
};

static RESOURCEMANAGER *CreateGUISLIDER();

CREATEMANAGER g_pfnCreateGUISLIDER = CreateGUISLIDER;

CMDENTRY GUISLIDERMASTER::s_commands[] = {
    { 0, 0, 0, 0, 0 },
};

PROPENTRY GUISLIDERMASTER::s_properties[] = {
    { "SliderVisualName", 0, 7, 0, PROP_FIELD(GUISLIDERMASTER, m_sliderVisualName), false, 1, PROP_SETTER(GUISLIDERMASTER, SetSliderVisualName) },
    { "SliderVisualIdx", 0, 0, 0, PROP_FIELD(GUISLIDERMASTER, m_sliderVisualIdx), false, 1, PROP_SETTER(GUISLIDERMASTER, SetSliderVisualIdx) },
    { "OnChange", 0, 7, 0, PROP_FIELD(GUISLIDERMASTER, m_onChange), false, 1, PROP_SETTER(GUISLIDERMASTER, SetOnChange) },
    { 0, 0, 15, 0, 0, false },
};

void GUISLIDERMASTER::RegisterCommands()
{
    g_classParents[s_classId] = GUIGROUPMASTER::s_classId;
    g_classCommands[s_classId] = s_commands;
    HashCommands(s_commands);
    RegisterClassName(s_commands, "GUISLIDERMASTER", "GUIGROUPMASTER");
}

const char *GUISLIDERMANAGER::GetTypeCode()
{
    return "GSLI";
}

static RESOURCEMANAGER *CreateGUISLIDER()
{
    return new GUISLIDERMANAGER;
}

GUISLIDERMASTER::GUISLIDERMASTER(RESOURCEMANAGER *manager, const char *name)
    : GUIGROUPMASTER(manager, name)
{
    m_sliderVisualName = "";
    m_sliderVisualIdx = 0;
    m_onChange = "";
}

int GUISLIDERMASTER::GetClassId()
{
    return s_classId;
}

int GUISLIDERMASTER::GetParentClassId()
{
    return g_classParents[s_classId];
}

const char *GUISLIDERMASTER::GetClassName()
{
    return g_classNames[s_classId];
}
