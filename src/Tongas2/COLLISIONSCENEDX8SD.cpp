// Tongas2/COLLISIONSCENEDX8SD -- the DX8 shadowed collision-scene resources.
//
// Discovery scaffold: bodies are empty until the unit is matched.
#include "Tongas2/COLLISIONSCENEDX8SD.h"
#include "Tongas2/RESOURCE.h"

void COLLISIONSCENEDX8SDMASTER::RegisterCommands()
{
}

RESOURCEMANAGER *CreateCOLLISIONSCENEDX8SD()
{
    return 0;
}

// A resource master with two allocated names.
class GD5368a0 : public RESOURCEMASTER
{
public:
    virtual ~GD5368a0() throw();

    CSTRING m_60;
    CSTRING m_64;
};

__declspec(noinline) GD5368a0::~GD5368a0() throw()
{
}

class GD536790
{
public:
    virtual ~GD536790();
};

__declspec(noinline) GD536790::~GD536790()
{
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
