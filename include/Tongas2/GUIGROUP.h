// GUI groups: GUIGROUPMASTER, from GUIGROUP.txt, is a GUI screen or panel
// with its commands, mouse area and sounds and up to 32 controls placed in
// it. GUISLIDER, GUICHECKBOX and GUICHECKGROUP build on it. Placeholder
// names apart from the class name and properties.
#ifndef TONGAS2_GUIGROUP_H
#define TONGAS2_GUIGROUP_H

#include "Tongas2/CSTRING.h"
#include "Tongas2/RESOURCE.h"
#include "Tongas2/RESOURCES.h"
#include "Tongas2/SRESULT.h"
#include "Tongas2/STRING.h"

#define GUIGROUP_MAX_CONTROLS 32
#define GUIGROUP_MAX_COMMANDS 4

// A 2D position or size that starts at zero.
struct GUIGROUPV2 : V2
{
    GUIGROUPV2() : V2(0.0f, 0.0f) {}
    GUIGROUPV2(const V2 &v) : V2(v) {}
};

class GUIGROUPMASTER : public RESOURCEMASTER
{
public:
    GUIGROUPMASTER(RESOURCEMANAGER *manager, const char *name);
    virtual ~GUIGROUPMASTER() {}

    static void RegisterCommands();

    virtual int GetClassId();
    virtual int GetParentClassId();
    virtual const char *GetClassName();
    virtual SRESULT Execute(CMDENTRY *command, TOKENS **args)
    {
        return (this->*(SRESULT (GUIGROUPMASTER::*)(TOKENS **))command->func)(args);
    }

    virtual void SetTimeSpace(const int &timeSpace) { m_timeSpace = timeSpace; }
    virtual void SetOnActivateCommand(const STRING &command) { m_onActivateCommand = command; }
    virtual void SetOnDeactivateCommand(STRING &command)
    {
        m_onDeactivateCommand.Reserve(command.Length());
        m_onDeactivateCommand = command.c_str();
    }
    virtual void SetOnStepCommand(STRING &command)
    {
        m_onStepCommand.Reserve(command.Length());
        m_onStepCommand = command.c_str();
    }
    virtual void SetSetOnTopPriority(const bool &onTop) { m_setOnTopPriority = onTop; }
    virtual void SetFullScreenInputBlocker(const bool &blocker) { m_fullScreenInputBlocker = blocker; }
    virtual void SetOnClick(STRING &command)
    {
        m_onClick.Reserve(command.Length());
        m_onClick = command.c_str();
    }
    virtual void SetOnMouseOver(STRING &command)
    {
        m_onMouseOver.Reserve(command.Length());
        m_onMouseOver = command.c_str();
    }
    virtual void SetOnMouseOut(STRING &command)
    {
        m_onMouseOut.Reserve(command.Length());
        m_onMouseOut = command.c_str();
    }
    virtual void SetMouseAreaOffset(const V2 &offset) { m_mouseAreaOffset = offset; }
    virtual void SetMouseAreaSize(const V2 &size) { m_mouseAreaSize = size; }
    virtual void SetMouseOverOutWhenDisable(const bool &value) { m_mouseOverOutWhenDisable = value; }
    virtual void SetPassMouseOverToFather(const bool &value) { m_passMouseOverToFather = value; }
    virtual void SetSelectableWithKeyboard(const bool &value) { m_selectableWithKeyboard = value; }
    virtual void SetOnMouseOverSound(const CSTRING &sound) { m_onMouseOverSound = sound; }
    virtual void SetOnMouseClickSound(const CSTRING &sound) { m_onMouseClickSound = sound; }
    virtual void SetRandomResource(const bool &random) { m_randomResource = random; }
    virtual void SetControl(const int &i, const CSTRING &control) { m_controls[i] = control; }
    virtual void SetControlPos(const int &i, const V2 &pos) { m_controlPos[i] = pos; }
    virtual void SetControlSize(const int &i, const V2 &size) { m_controlSize[i] = size; }
    virtual void SetControlName(const int &i, const CSTRING &name) { m_controlNames[i] = name; }
    virtual void SetControlZSort(const int &i, const int &zsort) { m_controlZSort[i] = zsort; }
    virtual void SetControlOpacity(const int &i, const float &opacity) { m_controlOpacity[i] = opacity; }
    virtual void SetControlVisible(const int &i, const bool &visible) { m_controlVisible[i] = visible; }
    virtual void SetControlEnabled(const int &i, const bool &enabled) { m_controlEnabled[i] = enabled; }
    virtual void SetBehavior(const int &i, const int &behavior) { m_behaviors[i] = behavior; }
    virtual void SetBehaviorParams(const int &i, const CSTRING &params) { m_behaviorParams[i] = params; }
    virtual void SetGroupCommand(const int &i, const CSTRING &command) { m_groupCommands[i] = command; }

    static int s_classId;
    static CMDENTRY s_commands[];
    static PROPENTRY s_properties[];

    int m_60;
    int m_timeSpace;
    STRING m_onActivateCommand;
    STRING m_onDeactivateCommand;
    STRING m_onStepCommand;
    bool m_setOnTopPriority;
    bool m_fullScreenInputBlocker;
    STRING m_onClick;
    STRING m_onMouseOver;
    STRING m_onMouseOut;
    GUIGROUPV2 m_mouseAreaOffset;
    GUIGROUPV2 m_mouseAreaSize;
    bool m_mouseOverOutWhenDisable;
    bool m_passMouseOverToFather;
    bool m_selectableWithKeyboard;
    CSTRING m_onMouseOverSound;
    CSTRING m_onMouseClickSound;
    bool m_randomResource;
    CSTRING m_controls[GUIGROUP_MAX_CONTROLS];
    GUIGROUPV2 m_controlPos[GUIGROUP_MAX_CONTROLS];
    GUIGROUPV2 m_controlSize[GUIGROUP_MAX_CONTROLS];
    CSTRING m_controlNames[GUIGROUP_MAX_CONTROLS];
    int m_controlZSort[GUIGROUP_MAX_CONTROLS];
    float m_controlOpacity[GUIGROUP_MAX_CONTROLS];
    bool m_controlVisible[GUIGROUP_MAX_CONTROLS];
    bool m_controlEnabled[GUIGROUP_MAX_CONTROLS];
    int m_behaviors[GUIGROUP_MAX_CONTROLS];
    CSTRING m_behaviorParams[GUIGROUP_MAX_CONTROLS];
    CSTRING m_groupCommands[GUIGROUP_MAX_COMMANDS];
};

#endif
