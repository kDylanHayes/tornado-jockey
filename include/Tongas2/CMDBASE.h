// Base of the engine's scriptable objects: each class has an id, a parent
// class, a table of console commands and a table of properties scripts can
// read and set. Placeholder names apart from the class name and the names
// its log messages give.
#ifndef TONGAS2_CMDBASE_H
#define TONGAS2_CMDBASE_H

#include "Tongas2/CSTRING.h"
#include "Tongas2/FIXEDSTRING.h"
#include "Tongas2/SAFEINDEX.h"
#include "Tongas2/SRESULT.h"
#include "Tongas2/TOKENS.h"

class CMDBASE;

// What a script command is run with: the tokens and who ran it.
struct CMDCALL
{
    TOKENS *tokens;
    RESHANDLE caller;
};

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

// The types of property values.
enum PROPTYPE
{
    PROP_INT,
    // An int naming one of the strings in names.
    PROP_ENUM,
    PROP_BOOL,
    PROP_FLOAT,
    PROP_CWSTRING,
    PROP_UNICODESTRING,
    PROP_WCHARS,
    PROP_CSTRING,
    PROP_STRING,
    PROP_CHARS,
    PROP_V2,
    PROP_V3,
    PROP_ANG,
    PROP_RGB,
    PROP_TEXTURE,
    // Ends a property table.
    PROP_END,
};

// A property scripts can read and set.
struct PROPENTRY
{
    const char *name;
    int hash;
    int type;
    // For PROP_ENUM: the names of the values, 30 characters each, ending
    // with an empty one.
    const char (*names)[30];
    // The value is read from a field or through a getter. Being a union
    // also makes the compiler initialize everything from here on in code.
    union
    {
        PROPFIELD field;
        PROPGETTER getter;
    };
    // The field points to an array on the heap, which ResizeArrays
    // reallocates.
    bool isArray;
    // The number of elements: properties can be arrays.
    int count;
    // Called instead of writing the field when set; arrays' setters also
    // take the index.
    PROPSETTER setter;
};

// A property's field and setter whatever their own types.
#define PROP_FIELD(CLASS, member) (PROPFIELD)(float CLASS::*)&CLASS::member
#define PROP_SETTER(CLASS, method) (PROPSETTER)(void (CLASS::*)(const float &))&CLASS::method

// Per-class tables, indexed by class id.
extern int g_classParents[];
extern CMDENTRY *g_classCommands[];
extern PROPENTRY *g_classProps[];

// The id of no class: CMDBASE's parent, and what FindProperty takes for
// the object's own class.
extern int g_NONEClassId;

// The hash of a command or property name, ignoring case.
int HashName(const char *name);
// Hashes a command table's names.
void HashCommands(CMDENTRY *commands);
// Hashes a property table's names.
void HashProperties(PROPENTRY *properties);
// The index of name in an enum property's names, or -1.
int FindEnum(const char (*names)[30], const char *name);
// Records a class and its parent by name; compiled out of the release build.
void RegisterClassName(CMDENTRY *commands, const char *name, const char *parent);

class CMDBASE
{
public:
    CMDBASE();

    static void RegisterCommands();

    // The dumpprops command.
    virtual SRESULT DumpProps(CMDCALL *call);
    virtual ~CMDBASE() {}
    virtual int GetClassId();
    virtual int GetParentClassId();
    virtual const char *GetClassName();
    // Runs a command from the class's table.
    virtual SRESULT Execute(CMDENTRY *command, TOKENS **args);
    virtual void SetName(const CSTRING &name) { m_name = name; }
    // Parses and runs a script command.
    virtual SRESULT Run(STRING command);
    // The object scripts reach as "parent" (a CMDBASE).
    virtual int Virtual08() = 0;
    // The object scripts reach by name from this one (a CMDBASE).
    virtual void *Virtual09(const char *name) = 0;
    // The value of a property, or of element index of an array one.
    virtual SRESULT GetProperty(PROPENTRY *property, int index);
    // A property as a number: vectors give their length.
    virtual float GetFloatAttribute(const char *name, int index);
    // A property as text.
    virtual void GetShortStringAttribute(FIXEDSTRING<0x200> &out, const char *name, int index);
    virtual void Virtual13();
    // Defined elsewhere; DumpProperties calls it first.
    virtual void Virtual14();
    // The seteval command: sets a property to the value of an expression.
    virtual SRESULT SetEval(CMDCALL *call);
    // The set command: sets properties from their values' text.
    virtual SRESULT SetAttribute(CMDCALL *call);
    // The setindirect command: sets the property an expression names.
    virtual SRESULT SetIndirect(CMDCALL *call);
    // The property with the name, searching from the class with the id
    // up through its parents (g_NONEClassId: the object's own class).
    virtual PROPENTRY *FindProperty(const char *name, int classId);

    // The object a path such as "parent.name" leads to from this one.
    CMDBASE *FindObject(const char *path, RESHANDLE caller);
    // Copies the properties of the class with the id and its parents to
    // target.
    void CopyProperties(CMDBASE *target, int classId);
    // Reallocates the array properties to m_arraySize elements, keeping
    // the first ones, where they hold more.
    void ResizeArrays();
    // Sets element index of a property to the value of an expression.
    bool SetAttributeEval(const char *name, int index, const char *expression, RESHANDLE caller);
    // A property's value as script text, followed by a space.
    void GetPropStringValue(PROPENTRY *property, int index, STRING &out);
    // The same, with the strings quoted and escaped for the parser.
    void GetPropStringValueEscaped(PROPENTRY *property, int index, STRING &out);
    // Writes out the properties and their values (the first element of
    // arrays).
    void DumpProperties();

    static CMDENTRY s_commands[];
    static PROPENTRY s_properties[];

    // The size ResizeArrays gives the array properties: the highest index
    // the set command has set, plus one.
    int m_arraySize;
    RESHANDLE m_handle;
    CSTRING m_path;
    CSTRING m_name;
};

extern int g_CMDBASEClassId;

// The nearest class two classes share, or g_NONEClassId; a class is its
// own.
int FindCommonClass(int classA, int classB);

// What a command gives back, the base of CMDCONTEXT. Placeholder names.
class CMDCONTEXTBASE
{
public:
    CMDCONTEXTBASE()
    {
        m_4 = 0;
        m_8 = 0;
    }
    virtual ~CMDCONTEXTBASE() {}

    int m_4;
    int m_8;
};

// A command being run: the call the command gets and what it gives back.
// Placeholder names.
class CMDCONTEXT : public CMDCONTEXTBASE
{
public:
    CMDCONTEXT(TOKENS *tokens, const RESHANDLE &caller);
    // Defined elsewhere.
    virtual ~CMDCONTEXT() {}

    CMDCALL m_call;
    int m_1c;
    bool m_20;
};

#endif
