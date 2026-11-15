// Tongas2/COLLISIONSCENEDX8SD -- the DX8 shadowed collision-scene resources.
//
// The class id trio (COLLISIONSCENEDX8CID), the manager factory, the master's
// two names and the placed copies' enable switch are all matched against
// retail; each new class at the end of the file names the retail address it
// reproduces.
#include "Tongas2/COLLISIONSCENEDX8SD.h"
#include "Tongas2/RESOURCE.h"
#include <string>

class TEXTFILE;


RESOURCEMANAGER *CreateCOLLISIONSCENEDX8SD()
{
    return new COLLISIONSCENEDX8SDMASTER();
}

// A resource master with two allocated names.
class GD5368a0 : public RESOURCEMASTER
{
public:
    // Retail 0x536800: the master takes a name from its manager.
    GD5368a0(RESOURCEMANAGER *manager, const char *name);
    virtual ~GD5368a0() throw();

    CSTRING m_60;
    CSTRING m_64;
};

GD5368a0::GD5368a0(RESOURCEMANAGER *manager, const char *name)
    : RESOURCEMASTER(manager, name)
{
    m_60 = "";
}

__declspec(noinline) GD5368a0::~GD5368a0() throw()
{
}

// The handle owner the display keeps at +0x2bc.
class GD536790DISPLAY
{
public:
    void m_45(void *arg);
    // Retail 0x500190: builds the token file for a name.
    TEXTFILE *m_65(std::string name, int a, int b);
};

class APPWINDOW
{
public:
    char pad_2bc[0x2bc];
    GD536790DISPLAY *m_2bc;
};

extern APPWINDOW *g_pDisplay;

class GD536790 : public OBJECT
{
public:
    // Retail 0x536aa0: one placed copy, made by its master from a handle.
    GD536790(RESOURCEMASTER *master, const RESHANDLE &handle);
    virtual ~GD536790();

    // Defined in other units; only the slots matter here.
    virtual bool IsLoaded();
    virtual V3 GetScaleFactor();

    void *m_400;
};

GD536790::~GD536790()
{
    GD536790DISPLAY *display = g_pDisplay->m_2bc;
    display->m_45(m_400);
}

extern "C" char DAT_0067a16c;
void *GetD0067a16c()
{
    return &DAT_0067a16c;
}

struct CSDGET
{
    int Get0(void);
};

int CSDGET::Get0(void)
{
    return *(int *)((char *)this - 0x264);
}

struct CSDSET
{
    void Set0(int value);
};

void CSDSET::Set0(int value)
{
    *(int *)((char *)this - 0x264) = value;
}

extern int g_classParents[];
extern char g_classNames[][30];

class COLLISIONSCENEDX8CID
{
public:
    virtual int GetClassId();
    virtual int GetParentClassId();
    virtual const char *GetClassName();

    static int s_classId;
};

int COLLISIONSCENEDX8CID::GetClassId()
{
    return s_classId;
}

int COLLISIONSCENEDX8CID::GetParentClassId()
{
    return g_classParents[s_classId];
}

const char *COLLISIONSCENEDX8CID::GetClassName()
{
    return g_classNames[s_classId];
}

CMDENTRY COLLISIONSCENEDX8SDMASTER::s_commands[] = {
    { 0, 0, 0, 0, 0 },
};

void COLLISIONSCENEDX8SDMASTER::RegisterCommands()
{
    g_classParents[COLLISIONSCENEDX8CID::s_classId] = RESOURCEMASTER::s_classId;
    g_classCommands[COLLISIONSCENEDX8CID::s_classId] = s_commands;
    HashCommands(s_commands);
    RegisterClassName(s_commands, "COLLISIONSCENEDX8SDMASTER", "RESOURCEMASTER");
}

// ---------------------------------------------------------------------------
// The token file a placed copy reads its placement from; only GetToken is used
// here (retail 0x528aa0).
class TEXTFILE
{
public:
    void GetToken(void *buf, int size);

    char m_pad[0x100];
};

// Retail 0x536aa0: the copy's constructor. It asks the display's context for
// the copy's token file, built from the master's second name, and reads the
// placement out of it.
GD536790::GD536790(RESOURCEMASTER *master, const RESHANDLE &handle)
    : OBJECT(master, handle)
{
    TEXTFILE *file = g_pDisplay->m_2bc->m_65(
        std::string(((GD5368a0 *)master)->m_64.c_str()), 1, 0);
    m_400 = file;
    file->GetToken((char *)file + 0x44, (int)((char *)file + 0x38));
}

// ---------------------------------------------------------------------------
// Retail 0x5368e0: the manager's CreateMaster, which builds the master above.
class GD5368e0 : public RESOURCEMANAGER
{
public:
    RESOURCEMASTER *CreateMaster(const char *name);
};

RESOURCEMASTER *GD5368e0::CreateMaster(const char *name)
{
    return new GD5368a0(this, name);
}

// ---------------------------------------------------------------------------
// Retail 0x536b70: the master's CreateCopy, which places the copy above.
class GD536b70 : public RESOURCEMASTER
{
public:
    virtual RESOURCE *CreateCopy(const RESHANDLE &handle);
};

RESOURCE *GD536b70::CreateCopy(const RESHANDLE &handle)
{
    return new GD536790(this, handle);
}

// ---------------------------------------------------------------------------
// Retail 0x536be0 and 0x536c30: switch the shadowed collision scene on and
// off. The scene keeps its resource pointer at +0x28, a heightmap temporary
// at +0x400 and the saved value the temporary replaces at +0x410.

// The object the scene looks at through +0x28: its flag and its value.
class GD536be0OBJ
{
public:
    char m_pad[0x68];
    int m_68;
    bool m_6c;
};

// The heightmap temporary at +0x400 of the scene; only its enable switch is
// reachable from this unit (the retail body is 0x4eb960).
class HEIGHTMAPTEMP
{
public:
    void SetEnabled(bool enabled);

    char m_pad[0x10];
};

// The renderer's 31d0 word (the retail body is 0x53a080).
class G2RTDSET
{
public:
    void Set31d0(int value);
};

class RENDERER;

extern RENDERER *g_pRenderer;

class GD536be0
{
public:
    void Enable();  // retail 0x536be0
    void Disable(); // retail 0x536c30

    char m_pad_00[0x28];
    GD536be0OBJ *m_28;
    char m_pad_2c[0x400 - 0x2c];
    HEIGHTMAPTEMP m_400;
    int m_410;
    bool m_414;
};

void GD536be0::Enable()
{
    if (m_28) {
        if (m_28->m_6c)
            ((G2RTDSET *)g_pRenderer)->Set31d0((int)this);
    }
    if (m_28) {
        m_410 = m_28->m_68;
        m_414 = false;
    }
    m_400.SetEnabled(true);
}

void GD536be0::Disable()
{
    m_400.SetEnabled(false);
    if (m_28) {
        if (m_28->m_6c)
            ((G2RTDSET *)g_pRenderer)->Set31d0(0);
    }
}

// ---------------------------------------------------------------------------
// Retail 0x536a20: the master's Execute override, which just runs the command
// entry's member function with the class's `this`. The entry is declared here
// so its function pointer is the 4-byte member pointer the retail table has.

// A class with no inheritance and no virtuals, so its member pointers are the
// single 4-byte form the retail command table stores.
struct GD536a20BASE
{
};

typedef SRESULT (GD536a20BASE::*GD536a20FUNC)(TOKENS **args);

struct GD536a20ENTRY
{
    const char *name;
    int hash;
    int minArgs;
    int maxArgs;
    GD536a20FUNC func;
};

class GD536a20 : public CMDBASE
{
public:
    SRESULT Execute(GD536a20ENTRY *command, TOKENS **args); // retail 0x536a20
};

SRESULT GD536a20::Execute(GD536a20ENTRY *command, TOKENS **args)
{
    return (((GD536a20BASE *)this)->*(command->func))(args);
}
