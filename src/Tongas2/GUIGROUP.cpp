// This file was built seeing SRESULT::FreeValue as one that can throw.
#define SRESULT_FREEVALUE_MAY_THROW

#include "Tongas2/GUIGROUP.h"
#include "Tongas2/PATH.h"

static const char s_behaviors[][30] = {
    "NONE",
    "JITTER",
    "FADE",
    "SIZE",
    "BLINK",
    "DIEFADE",
    "",
};

static const char s_timeSpaces[][30] = {
    "Game",
    "Real",
    "",
};

class GUIGROUPMANAGER : public RESOURCEMANAGER
{
public:
    GUIGROUPMANAGER()
    {
        SafeSprintf(m_fileName, "GUIGROUP.txt");
        m_8 = false;
        m_18 = true;
    }

    virtual RESOURCEMASTER *CreateMaster(const char *name) { return new GUIGROUPMASTER(this, name); }
    virtual const char *GetTypeCode();
};

static RESOURCEMANAGER *CreateGUIGROUP();

CREATEMANAGER g_pfnCreateGUIGROUP = CreateGUIGROUP;

CMDENTRY GUIGROUPMASTER::s_commands[] = {
    { 0, 0, 0, 0, 0 },
};

PROPENTRY GUIGROUPMASTER::s_properties[] = {
    { "TimeSpace", 0, 1, (int)s_timeSpaces, PROP_FIELD(GUIGROUPMASTER, m_timeSpace), false, 1, PROP_SETTER(GUIGROUPMASTER, SetTimeSpace) },
    { "OnActivateCommand", 0, 8, 0, PROP_FIELD(GUIGROUPMASTER, m_onActivateCommand), false, 1, PROP_SETTER(GUIGROUPMASTER, SetOnActivateCommand) },
    { "OnDeactivateCommand", 0, 8, 0, PROP_FIELD(GUIGROUPMASTER, m_onDeactivateCommand), false, 1, PROP_SETTER(GUIGROUPMASTER, SetOnDeactivateCommand) },
    { "OnStepCommand", 0, 8, 0, PROP_FIELD(GUIGROUPMASTER, m_onStepCommand), false, 1, PROP_SETTER(GUIGROUPMASTER, SetOnStepCommand) },
    { "SetOnTopPriority", 0, 2, 0, PROP_FIELD(GUIGROUPMASTER, m_setOnTopPriority), false, 1, PROP_SETTER(GUIGROUPMASTER, SetSetOnTopPriority) },
    { "FullScreenInputBlocker", 0, 2, 0, PROP_FIELD(GUIGROUPMASTER, m_fullScreenInputBlocker), false, 1, PROP_SETTER(GUIGROUPMASTER, SetFullScreenInputBlocker) },
    { "OnClick", 0, 8, 0, PROP_FIELD(GUIGROUPMASTER, m_onClick), false, 1, PROP_SETTER(GUIGROUPMASTER, SetOnClick) },
    { "OnMouseOver", 0, 8, 0, PROP_FIELD(GUIGROUPMASTER, m_onMouseOver), false, 1, PROP_SETTER(GUIGROUPMASTER, SetOnMouseOver) },
    { "OnMouseOut", 0, 8, 0, PROP_FIELD(GUIGROUPMASTER, m_onMouseOut), false, 1, PROP_SETTER(GUIGROUPMASTER, SetOnMouseOut) },
    { "MouseAreaOffset", 0, 10, 0, PROP_FIELD(GUIGROUPMASTER, m_mouseAreaOffset), false, 1, PROP_SETTER(GUIGROUPMASTER, SetMouseAreaOffset) },
    { "MouseAreaSize", 0, 10, 0, PROP_FIELD(GUIGROUPMASTER, m_mouseAreaSize), false, 1, PROP_SETTER(GUIGROUPMASTER, SetMouseAreaSize) },
    { "MouseOverOutWhenDisable", 0, 2, 0, PROP_FIELD(GUIGROUPMASTER, m_mouseOverOutWhenDisable), false, 1, PROP_SETTER(GUIGROUPMASTER, SetMouseOverOutWhenDisable) },
    { "PassMouseOverToFather", 0, 2, 0, PROP_FIELD(GUIGROUPMASTER, m_passMouseOverToFather), false, 1, PROP_SETTER(GUIGROUPMASTER, SetPassMouseOverToFather) },
    { "SelectableWithKeyboard", 0, 2, 0, PROP_FIELD(GUIGROUPMASTER, m_selectableWithKeyboard), false, 1, PROP_SETTER(GUIGROUPMASTER, SetSelectableWithKeyboard) },
    { "OnMouseOverSound", 0, 7, 0, PROP_FIELD(GUIGROUPMASTER, m_onMouseOverSound), false, 1, PROP_SETTER(GUIGROUPMASTER, SetOnMouseOverSound) },
    { "OnMouseClickSound", 0, 7, 0, PROP_FIELD(GUIGROUPMASTER, m_onMouseClickSound), false, 1, PROP_SETTER(GUIGROUPMASTER, SetOnMouseClickSound) },
    { "RandomResource", 0, 2, 0, PROP_FIELD(GUIGROUPMASTER, m_randomResource), false, 1, PROP_SETTER(GUIGROUPMASTER, SetRandomResource) },
    { "", 0, 15, 0, 0, false, GUIGROUP_MAX_CONTROLS, 0 },
    { "Control", 0, 7, 0, PROP_FIELD(GUIGROUPMASTER, m_controls), false, GUIGROUP_MAX_CONTROLS, PROP_SETTER(GUIGROUPMASTER, SetControl) },
    { "ControlPos", 0, 10, 0, PROP_FIELD(GUIGROUPMASTER, m_controlPos), false, GUIGROUP_MAX_CONTROLS, PROP_SETTER(GUIGROUPMASTER, SetControlPos) },
    { "ControlSize", 0, 10, 0, PROP_FIELD(GUIGROUPMASTER, m_controlSize), false, GUIGROUP_MAX_CONTROLS, PROP_SETTER(GUIGROUPMASTER, SetControlSize) },
    { "ControlName", 0, 7, 0, PROP_FIELD(GUIGROUPMASTER, m_controlNames), false, GUIGROUP_MAX_CONTROLS, PROP_SETTER(GUIGROUPMASTER, SetControlName) },
    { "ControlZSort", 0, 0, 0, PROP_FIELD(GUIGROUPMASTER, m_controlZSort), false, GUIGROUP_MAX_CONTROLS, PROP_SETTER(GUIGROUPMASTER, SetControlZSort) },
    { "ControlOpacity", 0, 3, 0, PROP_FIELD(GUIGROUPMASTER, m_controlOpacity), false, GUIGROUP_MAX_CONTROLS, PROP_SETTER(GUIGROUPMASTER, SetControlOpacity) },
    { "ControlVisible", 0, 2, 0, PROP_FIELD(GUIGROUPMASTER, m_controlVisible), false, GUIGROUP_MAX_CONTROLS, PROP_SETTER(GUIGROUPMASTER, SetControlVisible) },
    { "ControlEnabled", 0, 2, 0, PROP_FIELD(GUIGROUPMASTER, m_controlEnabled), false, GUIGROUP_MAX_CONTROLS, PROP_SETTER(GUIGROUPMASTER, SetControlEnabled) },
    { "Behavior", 0, 1, (int)s_behaviors, PROP_FIELD(GUIGROUPMASTER, m_behaviors), false, GUIGROUP_MAX_CONTROLS, PROP_SETTER(GUIGROUPMASTER, SetBehavior) },
    { "BehaviorParams", 0, 7, 0, PROP_FIELD(GUIGROUPMASTER, m_behaviorParams), false, GUIGROUP_MAX_CONTROLS, PROP_SETTER(GUIGROUPMASTER, SetBehaviorParams) },
    { "GroupCommand", 0, 7, 0, PROP_FIELD(GUIGROUPMASTER, m_groupCommands), false, GUIGROUP_MAX_COMMANDS, PROP_SETTER(GUIGROUPMASTER, SetGroupCommand) },
    { 0, 0, 15, 0, 0, false },
};

void GUIGROUPMASTER::RegisterCommands()
{
    g_classParents[s_classId] = RESOURCEMASTER::s_classId;
    g_classCommands[s_classId] = s_commands;
    HashCommands(s_commands);
    RegisterClassName(s_commands, "GUIGROUPMASTER", "RESOURCEMASTER");
}

const char *GUIGROUPMANAGER::GetTypeCode()
{
    return "GGRP";
}

static RESOURCEMANAGER *CreateGUIGROUP()
{
    return new GUIGROUPMANAGER;
}

int GUIGROUPMASTER::GetClassId()
{
    return s_classId;
}

int GUIGROUPMASTER::GetParentClassId()
{
    return g_classParents[s_classId];
}

const char *GUIGROUPMASTER::GetClassName()
{
    return g_classNames[s_classId];
}

GUIGROUPMASTER::GUIGROUPMASTER(RESOURCEMANAGER *manager, const char *name)
    : RESOURCEMASTER(manager, name)
{
    m_timeSpace = 1;
    m_onActivateCommand = "";
    m_onDeactivateCommand = "";
    m_onStepCommand = "";
    m_setOnTopPriority = false;
    m_fullScreenInputBlocker = false;
    m_onClick = "";
    m_onMouseOver = "";
    m_onMouseOut = "";
    m_mouseAreaOffset = V2(0.0f, 0.0f);
    m_mouseAreaSize = V2(0.0f, 0.0f);
    m_mouseOverOutWhenDisable = false;
    m_passMouseOverToFather = false;
    m_selectableWithKeyboard = false;
    m_onMouseOverSound = "";
    m_onMouseClickSound = "";
    m_randomResource = false;
    for (g_i = 0; g_i < GUIGROUP_MAX_CONTROLS; g_i++)
        m_controls[g_i] = "";
    for (g_i = 0; g_i < GUIGROUP_MAX_CONTROLS; g_i++)
        m_controlPos[g_i] = V2(0.0f, 0.0f);
    for (g_i = 0; g_i < GUIGROUP_MAX_CONTROLS; g_i++)
        m_controlSize[g_i] = V2(1.0f, 1.0f);
    for (g_i = 0; g_i < GUIGROUP_MAX_CONTROLS; g_i++)
        m_controlNames[g_i] = "";
    for (g_i = 0; g_i < GUIGROUP_MAX_CONTROLS; g_i++)
        m_controlZSort[g_i] = 0;
    for (g_i = 0; g_i < GUIGROUP_MAX_CONTROLS; g_i++)
        m_controlOpacity[g_i] = 1.0f;
    for (g_i = 0; g_i < GUIGROUP_MAX_CONTROLS; g_i++)
        m_controlVisible[g_i] = true;
    for (g_i = 0; g_i < GUIGROUP_MAX_CONTROLS; g_i++)
        m_controlEnabled[g_i] = true;
    for (g_i = 0; g_i < GUIGROUP_MAX_CONTROLS; g_i++)
        m_behaviors[g_i] = 0;
    for (g_i = 0; g_i < GUIGROUP_MAX_CONTROLS; g_i++)
        m_behaviorParams[g_i] = "";
    for (g_i = 0; g_i < GUIGROUP_MAX_COMMANDS; g_i++)
        m_groupCommands[g_i] = "";
    m_60 = 0;
}
