// Global definitions: GLOBALDEFSMASTER holds one named value from
// GLOBALDEFS.txt. Placeholder names apart from the class name and property.
#ifndef TONGAS2_GLOBALDEFS_H
#define TONGAS2_GLOBALDEFS_H

#include "Tongas2/RESOURCE.h"
#include "Tongas2/RESOURCES.h"
#include "Tongas2/STRING.h"

class GLOBALDEFSMASTER : public RESOURCEMASTER
{
public:
    GLOBALDEFSMASTER(RESOURCEMANAGER *manager, const char *name);

    static void RegisterCommands();

    virtual int GetClassId();
    virtual int GetParentClassId();
    virtual const char *GetClassName();
    virtual SRESULT Execute(CMDENTRY *command, TOKENS **args);

    virtual void SetValue(const STRING &value) { m_value = value; }

    static int s_classId;
    static CMDENTRY s_commands[];
    static PROPENTRY s_properties[];

    STRING m_value;
};

#endif
