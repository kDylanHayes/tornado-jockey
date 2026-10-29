// GUI check groups: GUICHECKGROUPMASTER, from GUICHECKGROUP.txt, is a GUI
// group of check boxes with how many may be checked at once. Placeholder
// names apart from the class name and properties.
#ifndef TONGAS2_GUICHECKGROUP_H
#define TONGAS2_GUICHECKGROUP_H

#include "Tongas2/GUIGROUP.h"

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
class GUICHECKGROUP : public GUIGROUP
{
public:
    // Defined in another unit.
    GUICHECKGROUP(const RESHANDLE &master, GUIGROUP *parent);

    static void RegisterCommands();

    static int s_classId;
    static PROPENTRY s_properties[];

    char m_140[0xc];
};

#endif
