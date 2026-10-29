// GUI groups: GUIGROUPMASTER, from GUIGROUP.txt, is a GUI screen or panel
// with its commands, mouse area and sounds and up to 32 controls placed in
// it. GUISLIDER, GUICHECKBOX and GUICHECKGROUP build on it. Placeholder
// names apart from the class name and properties.
#ifndef TONGAS2_GUIGROUP_H
#define TONGAS2_GUIGROUP_H

#include "Tongas2/CSTRING.h"
#include "Tongas2/GUICONTROL.h"
#include "Tongas2/INPUT.h"
#include "Tongas2/RESOURCE.h"
#include "Tongas2/RESOURCES.h"
#include "Tongas2/SRESULT.h"
#include "Tongas2/STRING.h"

#include <vector>

#define GUIGROUP_MAX_CONTROLS 32
#define GUIGROUP_MAX_COMMANDS 4

// A 2D position or size that starts at zero.
struct GUIGROUPV2 : V2
{
    GUIGROUPV2() : V2(0.0f, 0.0f) {}
    GUIGROUPV2(float x_, float y_) : V2(x_, y_) {}
    GUIGROUPV2 &operator=(const V2 &v) { V2::operator=(v); return *this; }
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
    virtual void SetOnDeactivateCommand(const STRING &command) { m_onDeactivateCommand = command; }
    virtual void SetOnStepCommand(const STRING &command) { m_onStepCommand = command; }
    virtual void SetSetOnTopPriority(const bool &onTop) { m_setOnTopPriority = onTop; }
    virtual void SetFullScreenInputBlocker(const bool &blocker) { m_fullScreenInputBlocker = blocker; }
    virtual void SetOnClick(const STRING &command) { m_onClick = command; }
    virtual void SetOnMouseOver(const STRING &command) { m_onMouseOver = command; }
    virtual void SetOnMouseOut(const STRING &command) { m_onMouseOut = command; }
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

// A GUI screen or panel made from a GUIGROUPMASTER, with up to 32 controls
// in it, which it places, sizes and fades with itself. Its mouse area runs
// the template's click, mouse over and mouse out commands.
class GUIGROUP : public GUICONTROL, public MOUSELISTENER
{
public:
    GUIGROUP(const RESHANDLE &master, GUIGROUP *parent);
    virtual ~GUIGROUP();

    static void RegisterCommands();

    virtual int GetClassId() { return s_classId; }
    virtual int GetParentClassId() { return g_classParents[s_classId]; }
    virtual const char *GetClassName() { return g_classNames[s_classId]; }
    virtual SRESULT Execute(CMDENTRY *command, TOKENS **args)
    {
        return (this->*(SRESULT (GUIGROUP::*)(TOKENS **))command->func)(args);
    }
    virtual void *Virtual09(const char *name) { return Find(name); }
    virtual int Virtual19() { return 8; }
    virtual void SetPosition(const V2 &position);
    virtual void SetSize(const V2 &size);
    virtual void SetZSort(const int &zsort);
    virtual void SetOpacity(const float &opacity);
    virtual void Update();
    virtual void SetTopZSort(int zsort);
    // The highest top zsort of it and its controls.
    virtual int GetTopZSort();
    virtual void FindTopAt(V2 point, GUICONTROL **hit, int *zsort);
    virtual void Virtual34(V2 point, GUICONTROL **hit, int *zsort);

    // The control was clicked: runs the template's OnClick.
    virtual void Click();
    // The script commands.
    virtual SRESULT ClickCommand(CMDCALL *call);
    virtual SRESULT MouseOverCommand(CMDCALL *call);
    virtual SRESULT MouseOutCommand(CMDCALL *call);
    virtual SRESULT AddControlCommand(CMDCALL *call);
    virtual SRESULT ReplaceControl(CMDCALL *call);
    virtual SRESULT ReloadControl(CMDCALL *call);
    virtual SRESULT RunCmd(CMDCALL *call);
    virtual SRESULT EvalCmd(CMDCALL *call);
    virtual SRESULT SetOnTopPriority(CMDCALL *call);
    virtual SRESULT DumpControlsCommand(CMDCALL *call);
    virtual SRESULT OverrideControl(CMDCALL *call);
    // The mouse area's edges on the screen.
    virtual void GetMouseArea(float *left, float *right, float *top, float *bottom);
    // Whether the control can take the mouse now.
    virtual bool IsActive();

    virtual bool OnMouseMove(MOUSESTATE state);
    virtual bool OnLeftDown(MOUSESTATE state);
    virtual bool OnLeftUp(MOUSESTATE state);
    virtual bool OnLeftDrag(MOUSESTATE state);

    // Closes the control in the slot, if there is one.
    void CloseSlot(int slot);
    // Makes a control from the template and puts it in the slot.
    GUICONTROL *AddControl(int slot, const RESHANDLE &master);
    // Makes a control from the template and puts it in the first free slot.
    GUICONTROL *AddControl(const RESHANDLE &master);
    // Makes a group from the template with the name ("GGRP:name") and puts
    // it in this one; the wide-screen one ("name_WideScreen") when the
    // screen calls for it and there is one.
    GUIGROUP *AddGroup(const char *name);
    // The object a script names, among the controls.
    void *Find(const char *name);
    // The control with the name, here or in the groups in this one.
    GUICONTROL *FindControl(const char *name);
    // The highest zsort of the groups that block the input, over min.
    int GetMouseZSort(int min);
    // Whether the point is in the mouse area.
    bool IsMouseOver(float x, float y);
    // Places the controls again.
    void UpdateLayout();
    // Sets the top zsort of it and, one higher, of everything in it.
    void SetTopZSortAll(int zsort);
    // Logs the controls, indented by depth.
    void DumpControls(int depth);
    // Runs or evaluates one of the template's group commands.
    bool RunGroupCommand(int i);
    SRESULT EvalGroupCommand(int i);
    // Deletes the controls taken out since the last update.
    void DeleteRemoved();
    // Takes a control out of the group; it is deleted on the next update.
    void RemoveControl(GUICONTROL *control);

    static int s_classId;
    static CMDENTRY s_commands[];
    static PROPENTRY s_properties[];

    GUIGROUPMASTER *m_master;
    bool m_mouseOver;
    // The left button went down on it.
    bool m_pressed;
    int m_controlCount;
    GUICONTROL *m_controls[GUIGROUP_MAX_CONTROLS];
    std::vector<GUICONTROL *> m_removed;
    // The template has an OnStepCommand.
    bool m_hasStepCommand;
    // The OnActivateCommand has run.
    bool m_activated;
};

#endif
