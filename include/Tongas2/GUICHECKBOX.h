// GUI check boxes: GUICHECKBOXMASTER, from GUICHECKBOX.txt, is a GUI group
// with commands run when it is checked and unchecked. Placeholder names
// apart from the class name and properties.
#ifndef TONGAS2_GUICHECKBOX_H
#define TONGAS2_GUICHECKBOX_H

#include "Tongas2/GUIGROUP.h"

class GUICHECKBOXMASTER : public GUIGROUPMASTER
{
public:
    GUICHECKBOXMASTER(RESOURCEMANAGER *manager, const char *name);
    virtual ~GUICHECKBOXMASTER() {}

    static void RegisterCommands();

    virtual int GetClassId();
    virtual int GetParentClassId();
    virtual const char *GetClassName();
    virtual SRESULT Execute(CMDENTRY *command, TOKENS **args)
    {
        return (this->*(SRESULT (GUICHECKBOXMASTER::*)(TOKENS **))command->func)(args);
    }

    virtual void SetOnCheckOn(const STRING &command);
    virtual void SetOnCheckOff(const STRING &command);

    static int s_classId;
    static CMDENTRY s_commands[];
    static PROPENTRY s_properties[];

    STRING m_onCheckOn;
    STRING m_onCheckOff;
};

class GUICHECKGROUP;

// A check box: a GUI group that the template's OnCheckOn or OnCheckOff runs
// for when it is checked or unchecked, by clicking it or by its group.
class GUICHECKBOX : public GUIGROUP
{
public:
    GUICHECKBOX(const RESHANDLE &master, GUIGROUP *parent);
    virtual ~GUICHECKBOX() {}

    static void RegisterCommands();

    virtual int GetClassId() { return s_classId; }
    virtual int GetParentClassId() { return g_classParents[s_classId]; }
    virtual const char *GetClassName() { return g_classNames[s_classId]; }
    virtual SRESULT Execute(CMDENTRY *command, TOKENS **args)
    {
        return (this->*(SRESULT (GUICHECKBOX::*)(TOKENS **))command->func)(args);
    }
    virtual int Virtual19() { return 5; }
    // Clicking it turns it over.
    virtual void Click()
    {
        GUIGROUP::Click();
        if (IsEnabled() && IsVisible())
            SetChecked(!m_checked);
    }

    // Checks or unchecks it, if its group lets it, and runs the command.
    virtual void SetChecked(const bool &checked);

    static int s_classId;
    static CMDENTRY s_commands[];
    static PROPENTRY s_properties[];

    bool m_checked;
    // The check group it belongs to, if any.
    GUICHECKGROUP *m_group;
    GUICHECKBOXMASTER *m_checkMaster;
};

#endif
