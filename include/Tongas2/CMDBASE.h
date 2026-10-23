// Base of the engine's scriptable objects: each class has an id, a parent
// class and a table of console commands. Placeholder names; only what other
// units need so far.
#ifndef TONGAS2_CMDBASE_H
#define TONGAS2_CMDBASE_H

#include "Tongas2/CSTRING.h"
#include "Tongas2/SAFEINDEX.h"
#include "Tongas2/SRESULT.h"
#include "Tongas2/TOKENS.h"

class CMDBASE;

// Declared while CMDBASE is incomplete, so these use the general (16- and
// 12-byte) member pointer representations.
typedef SRESULT (CMDBASE::*CMDFUNC)(TOKENS **args);
typedef void (CMDBASE::*PROPSETTER)(const float &value);
typedef float CMDBASE::*PROPFIELD;
typedef float (CMDBASE::*PROPGETTER)();

// One console command: its name, the hash filled in at registration, how
// many arguments it takes and the method that runs it.
struct CMDENTRY
{
    const char *name;
    int hash;
    int minArgs;
    int maxArgs;
    CMDFUNC func;
};

// A property scripts can read and set.
struct PROPENTRY
{
    const char *name;
    int hash;
    int type;
    int m_c;
    // The value is read from a field or through a getter. Being a union
    // also makes the compiler initialize everything from here on in code.
    union
    {
        PROPFIELD field;
        PROPGETTER getter;
    };
    bool m_20;
    int m_24;
    PROPSETTER setter;
};

// A property's field and setter whatever their own types.
#define PROP_FIELD(CLASS, member) (PROPFIELD)(float CLASS::*)&CLASS::member
#define PROP_SETTER(CLASS, method) (PROPSETTER)(void (CLASS::*)(const float &))&CLASS::method

// Per-class tables, indexed by class id.
extern int g_classParents[];
extern CMDENTRY *g_classCommands[];

// Hashes a command table's names.
void HashCommands(CMDENTRY *commands);
// Records a class and its parent by name; compiled out of the release build.
void RegisterClassName(CMDENTRY *commands, const char *name, const char *parent);

class CMDBASE
{
public:
    CMDBASE();

    virtual SRESULT Virtual00(TOKENS **args);
    virtual ~CMDBASE() {}
    virtual int GetClassId();
    virtual int GetParentClassId();
    virtual const char *GetClassName();
    // Runs a command from the class's table.
    virtual SRESULT Execute(CMDENTRY *command, TOKENS **args);
    virtual void SetName(const CSTRING &name) { m_name = name; }
    // Parses and runs a script command.
    virtual SRESULT Run(STRING command);
    virtual int Virtual08() = 0;
    virtual void *Virtual09(const char *name) = 0;
    virtual void Virtual10();
    virtual void Virtual11();
    virtual void Virtual12();
    virtual void Virtual13();
    virtual void Virtual14();
    virtual void Virtual15();
    virtual void Virtual16();
    virtual void Virtual17();
    virtual void Virtual18();

    // Defined elsewhere.
    void ApplyProperties();
    // Logs the properties and their values.
    void DumpProperties();

    int m_4;
    RESHANDLE m_handle;
    CSTRING m_path;
    CSTRING m_name;
};

extern int g_CMDBASEClassId;

#endif
