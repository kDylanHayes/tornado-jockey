// Script variables: VALUEMANAGER keeps named values that scripts add, set,
// delete and test for with its commands. It stands in for its variables'
// properties too, so scripts read a variable as a property of it. A variable
// added but not yet set holds s_unset. Placeholder names apart from the
// class name and commands.
#ifndef TONGAS2_VALUEMANAGER_H
#define TONGAS2_VALUEMANAGER_H

#include <map>
#include <string>

#include "Tongas2/CMDBASE.h"

struct CMDCALL;

class VALUEMANAGER : public CMDBASE
{
public:
    VALUEMANAGER();
    virtual ~VALUEMANAGER();

    static void RegisterCommands();

    virtual int GetClassId();
    virtual int GetParentClassId();
    virtual const char *GetClassName();
    virtual SRESULT Execute(CMDENTRY *command, TOKENS **args);
    // Defined elsewhere.
    virtual int Virtual08();
    virtual void *Virtual09(const char *name);
    // property is one of the variables, as FindProperty gives them.
    virtual SRESULT GetProperty(PROPENTRY *property, int index);
    virtual PROPENTRY *FindProperty(const char *name, int classId);

    virtual SRESULT Add(CMDCALL *call);
    virtual SRESULT Delete(CMDCALL *call);
    // Sets a variable to the value of a script expression.
    virtual SRESULT Set(CMDCALL *call);
    virtual SRESULT Exist(CMDCALL *call);

    bool Exists(const char *name);

    static int s_classId;
    static CMDENTRY s_commands[];
    static PROPENTRY s_properties[];
    static SRESULT s_unset;

    std::map<std::string, SRESULT *> m_values;
};

// Makes the value manager.
extern CMDBASE *(*g_pfnCreateValueManager)();

// The value manager.
extern CMDBASE *g_pValueManager;

#endif
