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

// The GUI check box objects, defined elsewhere; only what CMDMGR needs so far.
class GUICHECKBOX : public GUIGROUP
{
public:
    // Defined in another unit.
    GUICHECKBOX(const RESHANDLE &master, GUIGROUP *parent);

    static void RegisterCommands();

    static int s_classId;
    static PROPENTRY s_properties[];

    char m_140[0xc];
};

#endif
