// Tongas2/SCENEMODELDX8SD -- the DX8 shadowed scene-model resources.
//
// Discovery scaffold: bodies are empty until the unit is matched.
#include <vector>
#include "Tongas2/SCENEMODELDX8SD.h"
#include "Tongas2/RESOURCE.h"


RESOURCEMANAGER *CreateSCENEMODELDX8SD()
{
    return new SCENEMODELDX8SDMASTER();
}

// The master's screen-decal side: the file-name setter at 0x537270. Only the
// name setter is in this block.
class SCREENDECALMASTER
{
public:
    virtual void V00(); virtual void V01(); virtual void V02();
    virtual void V03(); virtual void V04(); virtual void V05();
    // Slot 0x18: the cooked name.
    virtual void SetScreenName(const CSTRING &name);
    virtual void SetFileName(const CSTRING &name);

    char pad_04[0x60 - 4];
    CSTRING m_fileName;
};

void SCREENDECALMASTER::SetFileName(const CSTRING &name)
{
    if (name.Length() > 0)
        SetScreenName(name);
    m_fileName = name;
}

// The leaf objects the sub list holds; its destructor is at 0x537790 and
// only releases its four POD vectors.
class GD507220LEAF
{
public:
    ~GD507220LEAF() throw();

    char pad_0[0x14];
    std::vector<int> m_14;
    std::vector<int> m_24;
    std::vector<int> m_34;
    std::vector<int> m_44;
};

GD507220LEAF::~GD507220LEAF() throw()
{
}

// The objects the item's list holds; its destructor is at 0x537e90.
class GD507220SUB
{
public:
    ~GD507220SUB() throw();

    char pad_0[0xdc];
    std::vector<GD507220LEAF *> m_dc;
};

GD507220SUB::~GD507220SUB() throw()
{
    for (unsigned int i = 0; i < m_dc.size(); i++) {
        if (m_dc[i]) {
            delete m_dc[i];
            m_dc[i] = 0;
        }
    }
}

// One item of the object's list; its destructor is out of line at 0x5380a0.
class GD507220ITEM
{
public:
    ~GD507220ITEM() throw();

    char pad_0[0x430];
    std::vector<int> m_430;
    std::vector<GD507220SUB *> m_440;
};

GD507220ITEM::~GD507220ITEM() throw()
{
    for (unsigned int i = 0; i < m_440.size(); i++) {
        if (m_440[i]) {
            delete m_440[i];
            m_440[i] = 0;
        }
    }
}

// The object the display keeps at +0xb0: a list of items freed one by one.
class GD507220OBJECT
{
public:
    ~GD507220OBJECT() throw();

    int m_0;
    std::vector<GD507220ITEM *> m_4;
};

GD507220OBJECT::~GD507220OBJECT() throw()
{
    for (unsigned int i = 0; i < m_4.size(); i++) {
        if (m_4[i]) {
            delete m_4[i];
            m_4[i] = 0;
        }
    }
}

// A resource master with two allocated names.
class GD537230 : public RESOURCEMASTER
{
public:
    virtual ~GD537230() throw();

    CSTRING m_60;
    char pad_64[0xc];
    CSTRING m_70;
};

__declspec(noinline) GD537230::~GD537230() throw()
{
}

class GD536D90DISPLAY
{
public:
    void m_44(void *arg);
};

class APPWINDOW
{
public:
    char pad_2bc[0x2bc];
    GD536D90DISPLAY *m_2bc;
};

extern APPWINDOW *g_pDisplay;

// The second base; only its vptr, destructor and +0x2c handle matter here.
class HEIGHTMAPTEMP
{
public:
    virtual ~HEIGHTMAPTEMP();

    char pad_04[0x28];
    void *m_2c;
};

class GD536d90 : public OBJECT, public HEIGHTMAPTEMP
{
public:
    virtual ~GD536d90();
};

GD536d90::~GD536d90()
{
    GD536D90DISPLAY *display = g_pDisplay->m_2bc;
    display->m_44(m_2c);
}

extern "C" char DAT_0067a410;
void *GetD0067a410()
{
    return &DAT_0067a410;
}

struct SMDPTR
{
    void *Get0();
};

void *SMDPTR::Get0()
{
    return (char *)this - 0x23c;
}

extern int g_classParents[];
extern char g_classNames[][30];

class SCENEMODELDX8CID
{
public:
    virtual int GetClassId();
    virtual int GetParentClassId();
    virtual const char *GetClassName();
    void Set65(const bool &value);
    void Set6F(const bool &value);

    static int s_classId;
};

int SCENEMODELDX8CID::GetClassId()
{
    return s_classId;
}

int SCENEMODELDX8CID::GetParentClassId()
{
    return g_classParents[s_classId];
}

const char *SCENEMODELDX8CID::GetClassName()
{
    return g_classNames[s_classId];
}

void SCENEMODELDX8CID::Set65(const bool &value)
{
    *(bool *)((char *)this + 0x65) = value;
}

void SCENEMODELDX8CID::Set6F(const bool &value)
{
    *(bool *)((char *)this + 0x6f) = value;
}

CMDENTRY SCENEMODELDX8SDMASTER::s_commands[] = {
    { 0, 0, 0, 0, 0 },
};

void SCENEMODELDX8SDMASTER::RegisterCommands()
{
    g_classParents[SCENEMODELDX8CID::s_classId] = RESOURCEMASTER::s_classId;
    g_classCommands[SCENEMODELDX8CID::s_classId] = s_commands;
    HashCommands(s_commands);
    RegisterClassName(s_commands, "SCENEMODELDX8SDMASTER", "RESOURCEMASTER");
}

// The value vector at +0x34 of the render object; its add is 0x537f70.
// The value list at render+0x34: a 4-byte header word, then begin/end/cap.
// Add appends one value; the copy and the grow path are the retail helpers.
class GD5383VEC;

extern "C" void GD5383CopyN(int *dest, unsigned int count, const void *value,
                            GD5383VEC *self, const void *value2);

class GD5383VEC
{
public:
    void Add(const int &value);
    void InsertAt(int *position, int count, const int &value);

    int m_0;
    int *m_begin;
    int *m_end;
    int *m_cap;
};

void GD5383VEC::Add(const int &value)
{
    unsigned int count = !m_begin ? 0 : (unsigned int)(m_end - m_begin);
    unsigned int capacity = !m_begin ? 0 : (unsigned int)(m_cap - m_begin);
    if (count < capacity) {
        int *last = m_end;
        GD5383CopyN(last, 1, &value, this, &value);
        m_end = last + 1;
    } else {
        InsertAt(m_end, 1, value);
    }
}

// The render-state selector at 0x5383e0.
class GD5383e0
{
public:
    void Method(void *object, void *render);

    char pad_0[0x2c];
    bool m_2c;
};

void GD5383e0::Method(void *object, void *render)
{
    int flags = *(int *)((char *)object + 0x24);
    int value;
    if (flags & 0x10000) {
        if (flags & 0x100000) {
            value = 3;
            ((GD5383VEC *)((char *)render + 0x34))->Add(value);
        }
        value = 2;
        ((GD5383VEC *)((char *)render + 0x34))->Add(value);
        if (m_2c) {
            value = 1;
            ((GD5383VEC *)((char *)render + 0x34))->Add(value);
        }
    }
    value = 0;
    ((GD5383VEC *)((char *)render + 0x34))->Add(value);
}
