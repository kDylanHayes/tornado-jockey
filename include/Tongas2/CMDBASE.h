// Base of the engine's scriptable objects: each class has an id, a parent
// class and a table of console commands. Placeholder names; only what other
// units need so far.
#ifndef TONGAS2_CMDBASE_H
#define TONGAS2_CMDBASE_H

#include "Tongas2/CSTRING.h"
#include "Tongas2/SRESULT.h"
#include "Tongas2/TOKENS.h"

class CMDBASE;

// Declared while CMDBASE is incomplete, so these use the general (16- and
// 12-byte) member pointer representations.
typedef SRESULT (CMDBASE::*CMDFUNC)(TOKENS **args);
typedef void (CMDBASE::*PROPSETTER)(const float &value);
typedef float CMDBASE::*PROPFIELD;

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
    PROPFIELD field;
    int m_1c;
    bool m_20;
    int m_24;
    PROPSETTER setter;
};

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
    virtual void Virtual05();
    virtual void Virtual06();
    virtual void Virtual07();
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

    void SetName(const CSTRING &name) { m_name = name; }

    int m_4;
    int m_8;
    int m_c;
    int m_10;
    CSTRING m_14;
    CSTRING m_name;
};

extern int g_CMDBASEClassId;

#endif
