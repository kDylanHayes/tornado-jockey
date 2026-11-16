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

// The four-byte element of the render object's value vectors.  The retail
// pushes go through std::vector<GD510630>, whose instantiations are the
// helpers at 0x537630..0x537e8f.
struct GD510630
{
    int m_0;
};

// The sixteen-byte element of the item's value vector.
struct GD510430
{
    int m_0, m_4, m_8, m_c;
};

// The value vector at +0x34 of the render object; its add is 0x537f70.
class GD5383VEC : public std::vector<GD510630>
{
public:
    ~GD5383VEC() {}
    void Add(const int &value);
};

void GD5383VEC::Add(const int &value)
{
    const GD510630 &v = *(const GD510630 *)&value;
    if (size() < capacity())
        _Mylast = _Ufill(_Mylast, 1, v);
    else
        insert(end(), v);
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

// 0x00538160 -- the leaf's mix of six material codes: each code the renderer
// accepts with the current settings becomes one value of the leaf's first
// list.
class GD5383DEV
{
public:
    void BuildMix(GD507220LEAF *leaf);

    void *m_0;
    char pad_4[0x1c];
    unsigned int m_20;
    unsigned int m_24;
    unsigned int m_28;
    bool m_2c;
};

// The renderer the device drives; only the slots the builders call matter.
class GD5383REND
{
public:
    virtual int V00();
    virtual int V01();
    virtual int V02();
    virtual int V03();
    virtual int V04();
    virtual int V05();
    virtual int V06();
    virtual int V07();
    virtual int V08();
    virtual int V09();
    virtual int V10(void *self, int a, int b, int c, int d, int e, int f);
    virtual int V11();
    virtual int V12(void *self, int a, int b, int c, int d, int e);
};

// The words of a leaf that the builders read; the vectors sit at +0x14.
struct GD507220LEAFHEAD
{
    int m_0;
    int m_4;
    int m_8;
    int m_c;
    unsigned char m_10;
};

void GD5383DEV::BuildMix(GD507220LEAF *leaf)
{
    GD507220LEAFHEAD *head = (GD507220LEAFHEAD *)leaf;
    int codes[6] = {0x50, 0x49, 0x4d, 0x4b, 0x4f, 0x47};
    for (unsigned int i = 0; i < 6; i++) {
        int code = codes[i];
        unsigned int need;
        switch (code - 0x47) {
        case 0:
            need = 0x20;
            break;
        case 2:
            need = 0x0f;
            break;
        case 4:
        case 6:
        case 8:
            need = 0x18;
            break;
        case 9:
            need = 0x10;
            break;
        default:
            need = 0;
            break;
        }
        if (need >= m_24) {
            switch (code - 0x47) {
            case 2:
                need = 1;
                break;
            case 4:
                need = 8;
                break;
            case 8:
                need = 4;
                break;
            default:
                need = 0;
                break;
            }
            if (need >= m_28) {
                GD5383REND *render = (GD5383REND *)m_0;
                if (render->V10(render, head->m_0, head->m_4, head->m_8, 2, 1, code) >= 0
                    && render->V12(render, head->m_0, head->m_4, head->m_8, head->m_c, code) >= 0)
                    ((GD5383VEC *)((char *)leaf + 0x14))->Add(code);
            }
        }
    }
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
    GD537230(RESOURCEMANAGER *manager, const char *name);
    virtual ~GD537230() throw();

    CSTRING m_60;
    bool m_64;
    bool m_65;
    char pad_66[2];
    int m_68;
    bool m_6c;
    bool m_6d;
    bool m_6e;
    bool m_6f;
    CSTRING m_70;
};

// 0x00537160 -- the master's constructor.
GD537230::GD537230(RESOURCEMANAGER *manager, const char *name)
    : RESOURCEMASTER(manager, name), m_60(), m_70()
{
    m_60 = "";
    m_65 = true;
    m_68 = 1;
    m_6d = true;
    m_6e = true;
    m_64 = false;
    m_6c = false;
    m_6f = false;
}

__declspec(noinline) GD537230::~GD537230() throw()
{
}

// 0x005372c0 -- the manager's factory for the master above.
class GD537230MANAGER : public RESOURCEMANAGER
{
public:
    virtual RESOURCEMASTER *CreateMaster(const char *name);
};

RESOURCEMASTER *GD537230MANAGER::CreateMaster(const char *name)
{
    return new GD537230(this, name);
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

// The item's sixteen-byte value list and the object's int list: the retail
// keeps their push_back bodies as out-of-line COMDATs (0x537fe0, 0x538050).
template void std::vector<GD510430>::push_back(const GD510430 &);
template void std::vector<int>::push_back(const int &);

// 0x00537f20 -- the render item's value descriptor: one code, an empty count
// triple and six words handed straight through.
class GD537f20
{
public:
    GD537f20(int a0, int a1, int a2, int a3, int a4, int a5, int a6);

    int m_0;
    int m_4;
    int m_8;
    int m_c;
    int m_10;
    int m_14;
    int m_18;
    int m_1c;
    int m_20;
    int m_24;
    int m_28;
    char m_2c;
};

GD537f20::GD537f20(int a0, int a1, int a2, int a3, int a4, int a5, int a6)
{
    m_0 = a0;
    m_8 = 0;
    m_c = 0;
    m_10 = 0;
    m_14 = a1;
    m_18 = a2;
    m_1c = a3;
    m_20 = a4;
    m_24 = a5;
    m_28 = a6;
    m_2c = 0;
}

// 0x00538cc0 -- orders two (high, low) word pairs.
extern "C" int FUN_00538cc0(unsigned int a, unsigned int b, unsigned int c,
                            unsigned int d)
{
    return b > d ? 1 : b < d ? -1 : a > c ? 1 : a < c ? -1 : 0;
}

// 0x00538bf0 -- the file-version query behind the master's version check.
extern "C" unsigned long __stdcall GetFileVersionInfoSizeA(const char *file,
                                                           unsigned long *handle);
extern "C" int __stdcall GetFileVersionInfoA(const char *file, unsigned long handle,
                                             unsigned long size, void *data);
extern "C" int __stdcall VerQueryValueA(const void *block, const char *sub,
                                        void **buffer, unsigned int *length);

extern "C" int FUN_00538bf0(const char *filename, int *out)
{
    if (!filename || !out)
        return 0x80070057;
    unsigned long length;
    unsigned long size = GetFileVersionInfoSizeA(filename, &length);
    if (size == 0)
        return 0x80004005;
    void *data = operator new[](size);
    if (!data)
        return 0x8007000e;
    if (GetFileVersionInfoA(filename, 0, size, data)) {
        void *value = 0;
        if (VerQueryValueA(data, "\\", &value, (unsigned int *)&length)) {
            if (value) {
                out[1] = *(int *)((char *)value + 8);
                out[0] = *(int *)((char *)value + 0xc);
                operator delete[](data);
                return 0;
            }
        }
    }
    operator delete[](data);
    return 0x80004005;
}
