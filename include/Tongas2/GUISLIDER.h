// GUI sliders: GUISLIDERMASTER, from GUISLIDER.txt, is a GUI group with a
// slider visual and a command run when the slider moves. Placeholder names
// apart from the class name and properties.
#ifndef TONGAS2_GUISLIDER_H
#define TONGAS2_GUISLIDER_H

#include "Tongas2/FIXEDSTRING.h"
#include "Tongas2/GUIGROUP.h"

class GUISLIDERMASTER : public GUIGROUPMASTER
{
public:
    GUISLIDERMASTER(RESOURCEMANAGER *manager, const char *name);
    virtual ~GUISLIDERMASTER() {}

    static void RegisterCommands();

    virtual int GetClassId();
    virtual int GetParentClassId();
    virtual const char *GetClassName();
    virtual SRESULT Execute(CMDENTRY *command, TOKENS **args)
    {
        return (this->*(SRESULT (GUISLIDERMASTER::*)(TOKENS **))command->func)(args);
    }

    virtual void SetSliderVisualName(const CSTRING &name) { m_sliderVisualName = name; }
    virtual void SetSliderVisualIdx(const int &idx) { m_sliderVisualIdx = idx; }
    virtual void SetOnChange(const CSTRING &command) { m_onChange = command; }

    static int s_classId;
    static CMDENTRY s_commands[];
    static PROPENTRY s_properties[];

    CSTRING m_sliderVisualName;
    int m_sliderVisualIdx;
    CSTRING m_onChange;
};

// The GUI slider objects, defined elsewhere; only what CMDMGR needs so far.
class GUIVISUAL;
class GUIVISUALMASTER;

// A slider: a GUI group with one of its visuals' progress bars as the
// slider, which dragging along sets the bar's variable from and runs the
// template's OnChange.
class GUISLIDER : public GUIGROUP
{
public:
    GUISLIDER(const RESHANDLE &master, GUIGROUP *parent);
    virtual ~GUISLIDER() {}

    static void RegisterCommands();

    virtual int GetClassId() { return s_classId; }
    virtual int GetParentClassId() { return g_classParents[s_classId]; }
    virtual const char *GetClassName() { return g_classNames[s_classId]; }
    virtual SRESULT Execute(CMDENTRY *command, TOKENS **args)
    {
        return (this->*(SRESULT (GUISLIDER::*)(TOKENS **))command->func)(args);
    }
    virtual int Virtual19() { return 7; }
    virtual void Click()
    {
        GUIGROUP::Click();
        if (IsEnabled() && IsVisible()) {
        }
    }

    virtual bool OnLeftDown(MOUSESTATE state)
    {
        if (!GUIGROUP::OnLeftDown(state))
            return false;
        if (m_pressed)
            SetValueAt(state.x, state.y);
        return true;
    }
    virtual bool OnLeftDrag(MOUSESTATE state)
    {
        if (!GUIGROUP::OnLeftDrag(state))
            return false;
        if (m_pressed)
            SetValueAt(state.x, state.y);
        return true;
    }

    // The visual the template names as the slider, or null if it isn't one.
    GUIVISUAL *FindVisual();
    // Sets the bar's variable from where the point is along the bar.
    void SetValueAt(float x, float y);

    static int s_classId;
    static CMDENTRY s_commands[];
    static PROPENTRY s_properties[];

    GUIVISUALMASTER *m_visualMaster;
    GUISLIDERMASTER *m_sliderMaster;
    // The script command that sets the bar's variable, without the value:
    // "object.set field".
    FIXEDSTRING<0x200> m_setCommand;
};

#endif
