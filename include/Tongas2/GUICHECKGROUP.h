// GUI check groups: GUICHECKGROUPMASTER, from GUICHECKGROUP.txt, is a GUI
// group of check boxes with how many may be checked at once. Placeholder
// names apart from the class name and properties.
#ifndef TONGAS2_GUICHECKGROUP_H
#define TONGAS2_GUICHECKGROUP_H

#include "Tongas2/GUIGROUP.h"

class GUICHECKBOX;

class GUICHECKGROUPMASTER : public GUIGROUPMASTER
{
public:
    GUICHECKGROUPMASTER(RESOURCEMANAGER *manager, const char *name)
        : GUIGROUPMASTER(manager, name)
    {
        m_minSelected = 1;
        m_maxSelected = 1;
    }
    virtual ~GUICHECKGROUPMASTER() {}

    static void RegisterCommands();

    virtual int GetClassId();
    virtual int GetParentClassId();
    virtual const char *GetClassName();
    virtual SRESULT Execute(CMDENTRY *command, TOKENS **args)
    {
        return (this->*(SRESULT (GUICHECKGROUPMASTER::*)(TOKENS **))command->func)(args);
    }

    virtual void SetMinSelected(const int &count) { m_minSelected = count; }
    virtual void SetMaxSelected(const int &count) { m_maxSelected = count; }

    static int s_classId;
    static CMDENTRY s_commands[];
    static PROPENTRY s_properties[];

    int m_minSelected;
    int m_maxSelected;
};

// The GUI check group objects, defined elsewhere; only what CMDMGR needs so far.
// A check group: a GUI group whose check boxes it keeps between its
// template's fewest and most checked.
class GUICHECKGROUP : public GUIGROUP
{
public:
    GUICHECKGROUP(const RESHANDLE &master, GUIGROUP *parent);
    virtual ~GUICHECKGROUP() {}

    static void RegisterCommands();

    virtual int GetClassId() { return s_classId; }
    virtual int GetParentClassId() { return g_classParents[s_classId]; }
    virtual const char *GetClassName() { return g_classNames[s_classId]; }
    virtual SRESULT Execute(CMDENTRY *command, TOKENS **args)
    {
        return (this->*(SRESULT (GUICHECKGROUP::*)(TOKENS **))command->func)(args);
    }
    virtual int Virtual19() { return 6; }

    // Whether the box may be checked or unchecked: checking one past the
    // most allowed unchecks others, and unchecking stops at the fewest.
    bool CanCheck(GUICHECKBOX *box, bool checked);

    static int s_classId;
    static CMDENTRY s_commands[];
    static PROPENTRY s_properties[];

    GUICHECKGROUPMASTER *m_groupMaster;
    // How many of its boxes are checked.
    int m_selected;
    // It is unchecking boxes itself.
    bool m_busy;
};

#endif
