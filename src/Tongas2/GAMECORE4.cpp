#define RESOURCES_NODE_CTOR_NEXT_FIRST
#include "lib/freeimage/FreeImage.h"
#include <windows.h>

#ifndef INFINITE
#define INFINITE 0xFFFFFFFF
#endif
#ifndef WM_QUIT
#define WM_QUIT 0x0012
#endif

extern "C" {
__declspec(dllimport) void __stdcall SysFreeString(unsigned short *bstr);
__declspec(dllimport) HANDLE __stdcall GetProcessHeap(void);
__declspec(dllimport) BOOL __stdcall HeapFree(HANDLE hHeap, DWORD dwFlags, void *lpMem);
__declspec(dllimport) BOOL __stdcall DestroyAcceleratorTable(HANDLE hAccel);
__declspec(dllimport) BOOL __stdcall IsWindow(HWND hWnd);
__declspec(dllimport) BOOL __stdcall DestroyWindow(HWND hWnd);
__declspec(dllimport) BOOL __stdcall PostThreadMessageA(DWORD idThread, UINT Msg,
                                                        WPARAM wParam, LPARAM lParam);
__declspec(dllimport) DWORD __stdcall WaitForSingleObject(HANDLE hHandle,
                                                          DWORD dwMilliseconds);
}
#define SOUNDMASTER_NOVTABLE_DTOR
#define OBJECT_GETPARENTACTOR
#define DRM_OUTOFLINE
#include <string.h>

// Tongas2/GAMECORE4 -- the game-code block at 0x4410b0. The string, vector
// and tree members the original translation unit emitted out of line, and the
// small resource accessors that live in the same block.
#define GC4_OUTOFLINE
#define MATRIX_AXIS_METHODS
#define ANG_UPDATE_OUTOFLINE
#define V2_ROTATE_OUTOFLINE
// The retail link kept the master's implicit destructor in this block.
#define CPRIMITIVE3MASTER_NOVTABLE
#include "Tongas2/CPRIMITIVE3.h"

// The tips' wide text conversion is defined with the other out-of-line core
// string operations; only this translation unit needs to see it declared.
#define TONGAS2_CORE_OUTOFLINE
#include "Tongas2/FIXEDWSTRING.h"
#undef TONGAS2_CORE_OUTOFLINE

#include <math.h>
#include <map>
#include <string>
#include <vector>

#include "Tongas2/V2.h"

#include "Tongas2/ACTOR.h"
#include "Tongas2/ACTORLINK.h"
#include "Tongas2/APP.h"
#include "Tongas2/APPWINDOW.h"
#include "Tongas2/CAMERA.h"
#include "Tongas2/DECAL.h"
#include "Tongas2/DRM.h"
#include "Tongas2/GUI.h"
#include "Tongas2/RESOURCES.h"
#include "Tongas2/SAFEINDEX.h"
#include "Tongas2/SCENE.h"
#include "Tongas2/SCREENDECAL.h"
#include "Tongas2/SOUND.h"
#include "Tongas2/STRING.h"
#include "Tongas2/UNICODESTRING.h"

__declspec(noinline) CPRIMITIVE3MASTER::~CPRIMITIVE3MASTER()
{
}

// The two resource Execute overrides in this block forward to the command
// table entry the caller named, exactly as RESOURCEMASTER::Execute does.
SRESULT RESOURCE::Execute(CMDENTRY *command, TOKENS **args)
{
    return (this->*(SRESULT (RESOURCE::*)(TOKENS **))command->func)(args);
}

SRESULT RESOURCES::Execute(CMDENTRY *command, TOKENS **args)
{
    return (this->*(SRESULT (RESOURCES::*)(TOKENS **))command->func)(args);
}

template class std::basic_string<char>;
template class std::vector<std::basic_string<char> >;
template class std::map<int, RESHANDLE>;
template struct std::less<std::basic_string<char> >;
template std::basic_string<char> std::operator+<char, std::char_traits<char>, std::allocator<char> >(
    const std::basic_string<char> &, const std::basic_string<char> &);
template std::basic_string<char> std::operator+<char, std::char_traits<char>, std::allocator<char> >(
    const std::basic_string<char> &, const char *);
template class CALLBACKLIST<RESOURCESCALLBACK>;

int RESOURCES::GetClassId()
{
    return s_classId;
}

int RESOURCES::GetParentClassId()
{
    return g_classParents[s_classId];
}

const char *RESOURCES::GetClassName()
{
    return g_classNames[s_classId];
}

void RESOURCES::SetSoundVolume(const float &volume)
{
    m_soundVolume = volume;
}

void RESOURCES::SetMusicVolume(const float &volume)
{
    m_musicVolume = volume;
}

float RESOURCES::GetMusicVolume()
{
    return m_musicVolume;
}

float RESOURCES::GetSoundVolume()
{
    return m_soundVolume;
}

void SCREENDECALMASTER::SetForceDuplicates(const bool &force)
{
    m_forceDuplicates = force;
}

void DECALMASTER::SetLoop(const bool &value)
{
    m_loop = value;
}

void DECALMASTER::SetAnimLen(const float &value)
{
    m_animLen = value;
}

void DECALMASTER::SetStartFrame(const int &value)
{
    m_startFrame = value;
}

void ACTOR::SetIsTemplate(const bool &value)
{
    m_isTemplate = value;
}

void ACTOR::SetColliderColorOverride(const bool &value)
{
    m_colliderColorOverride = value;
}

SOUNDMASTER::~SOUNDMASTER()
{
}

SOUNDMANAGER::SOUNDMANAGER()
{
    SafeSprintf(m_fileName, "SOUND.txt");
    m_8 = false;
    m_18 = true;
}

RESOURCEMASTER *SOUNDMANAGER::CreateMaster(const char *name)
{
    return new SOUNDMASTER(this, name);
}

const char *SOUNDMANAGER::GetTypeCode()
{
    return "SNFX";
}

bool SOUNDMANAGER::IsMyType(const char *name)
{
    return strstr(name, ".wav");
}

UNICODESTRING::UNICODESTRING()
{
    m_block = -1;
    m_pages = 0;
    m_buf = 0;
    m_length = 0;
    m_lengthValid = true;
}

UNICODESTRING::~UNICODESTRING() throw()
{
    if (m_pages > 0) {
        if (m_block >= 0)
            ((STRINGMANAGER *)&g_UnicodeStringManager)->Free(m_block, m_pages);
        m_block = -1;
        m_pages = 0;
        m_buf = 0;
        m_length = 0;
        m_lengthValid = true;
    }
}

void UNICODESTRING::Free()
{
    if (m_block >= 0)
        ((STRINGMANAGER *)&g_UnicodeStringManager)->Free(m_block, m_pages);
    m_block = -1;
    m_pages = 0;
    m_buf = 0;
    m_length = 0;
    m_lengthValid = true;
}

void *RESOURCE::Virtual09(const char *name)
{
    (void)name;
    return 0;
}

void *RESOURCES::Virtual09(const char *name)
{
    return FindMaster(name);
}

float ACTORLINK::Link14()
{
    return 1.0f;
}

void ANG::GetAxes(V3 *forward, V3 *up)
{    if (flags & MATRIX_DIRTY)
        UpdateMatrix();
    {
        V3 axis = matrix.GetForward();
        *forward = axis;
    }
    if (flags & MATRIX_DIRTY)
        UpdateMatrix();
    {
        V3 axis = matrix.GetUp();
        *up = axis;
    }
}

class V44AC00
{
public:
    virtual ~V44AC00() {}
};

class GD440960
{
public:
    virtual ~GD440960();
};

__declspec(noinline) GD440960::~GD440960()
{
}

class GD4413d0 : public RESOURCEMASTER
{
public:
    virtual ~GD4413d0() throw();
};

__declspec(noinline) GD4413d0::~GD4413d0() throw()
{
}

class GD441630 : public OBJECT
{
public:
    virtual ~GD441630() throw();
};

__declspec(noinline) GD441630::~GD441630() throw()
{
}

class GD442440
{
public:
    virtual ~GD442440();
};

__declspec(noinline) GD442440::~GD442440()
{
}

class GD43f600
{
public:
    virtual ~GD43f600();
};

__declspec(noinline) GD43f600::~GD43f600()
{
}

class GD4433c0
{
public:
    virtual ~GD4433c0();
};

__declspec(noinline) GD4433c0::~GD4433c0() throw()
{
}

// As at 0x43cc30: no vptr store; the master is a member and the four names
// sit at 0x60/0x98/0xac/0xb0.
class GD4433c0X
{
public:
    ~GD4433c0X();

    RESOURCEMASTER m_master;
    CSTRING m_60;
    char pad_98[0x98 - 0x64];
    CSTRING m_98;
    char pad_ac[0xac - 0x9c];
    CSTRING m_ac;
    CSTRING m_b0;
};

__declspec(noinline) GD4433c0X::~GD4433c0X()
{
}

class GD5479c0
{
public:
    virtual ~GD5479c0();
};

class GD4434d0 : public GD5479c0
{
public:
    virtual ~GD4434d0() throw();
};

__declspec(noinline) GD4434d0::~GD4434d0() throw()
{
}

class GD548040
{
public:
    virtual ~GD548040();
};

__declspec(noinline) GD548040::~GD548040()
{
}

class GD443f70
{
public:
    virtual ~GD443f70();
};

__declspec(noinline) GD443f70::~GD443f70()
{
}

class GD444450
{
public:
    virtual ~GD444450();
};

__declspec(noinline) GD444450::~GD444450() throw()
{
}

// As at 0x43cc30: the exe's destructor leaves the vptr alone, so the master
// is a member here and the three names sit at 0x60/0x64/0x9c.
class GD444450X
{
public:
    ~GD444450X();

    RESOURCEMASTER m_master;
    CSTRING m_60;
    CSTRING m_64;
    char pad_9c[0x9c - 0x68];
    CSTRING m_9c;
};

__declspec(noinline) GD444450X::~GD444450X()
{
}

class GD444730X
{
public:
    ~GD444730X();
    GD444730X *Destroy(bool flag);

    RESOURCEMASTER m_master;
    CSTRING m_60;
    CSTRING m_64;
};

__declspec(noinline) GD444730X::~GD444730X()
{
}

GD444730X *GD444730X::Destroy(bool flag)
{
    this->~GD444730X();
    if (flag)
        operator delete(this);
    return this;
}

class GD444e40
{
public:
    virtual ~GD444e40();
};

__declspec(noinline) GD444e40::~GD444e40() throw()
{
}

// As at 0x43cc30: no vptr store; seven names at 0x60..0x8c.
class GD444e40X
{
public:
    ~GD444e40X();

    RESOURCEMASTER m_master;
    CSTRING m_60;
    CSTRING m_64;
    char pad_6c[0x6c - 0x68];
    CSTRING m_6c;
    char pad_74[0x74 - 0x70];
    CSTRING m_74;
    char pad_7c[0x7c - 0x78];
    CSTRING m_7c;
    char pad_84[0x84 - 0x80];
    CSTRING m_84;
    char pad_8c[0x8c - 0x88];
    CSTRING m_8c;
};

__declspec(noinline) GD444e40X::~GD444e40X()
{
}

// The resolved object the table stores against a record; slot 2 releases it.
class G2MIDRESOLVED
{
public:
    virtual void __stdcall V0();
    virtual void __stdcall V1();
    virtual void __stdcall Release();
};

class G2MIDITEM
{
public:
    virtual void Release(int value);
    virtual void V01(); virtual void V02(); virtual void V03(); virtual void V04();
    virtual void V05(); virtual void V06(); virtual void V07(); virtual void V08();
    virtual void V09(); virtual void V10(); virtual void V11(); virtual void V12();
    virtual void V13(); virtual void V14();
    // Resolves the item into a storable object at slot 0x3c.
    virtual int __stdcall Resolve(int index, void **out);
};

struct G2MIDRECORD
{
    int m_0;
    int m_4;
    int m_8;
    int m_c;
};

// The four-word scratch record the table hands to its worker.
struct G2MIDPREPAREV4
{
    int m_0;
    int m_4;
    int m_8;
    int m_c;
};

// The interface the table keeps at +0x50.
class G2MIDPREPAREIFACE
{
public:
    virtual void __stdcall V0();
    virtual void __stdcall V1();
    virtual void __stdcall V2();
    virtual void __stdcall V3(int one, int minusOne, int zero1, int zero2,
                              int zero3, void *other, G2MIDPREPAREV4 *scratch,
                              int zero4, int zero5, int zero6);
};

// The table at +0x28 of the MID list.
class G2MIDTABLE
{
public:
    void Insert(G2MIDITEM *item, const std::string &name, int flag);
    // Resolves a name while the table is not built yet (0x44bd80), then
    // inserts the item straight away (0x44ad60).
    bool ResolveName(const std::string &name, G2MIDRECORD *found);
    void InsertDirect(G2MIDITEM *item, int flag, G2MIDRECORD *found);
    // Stores a record for a resolved object (0x44acf0).
    void Store(G2MIDRESOLVED *object, int flag, const G2MIDRECORD *record);
    // Rebuilds the record table (0x44acb0).
    void Prepare();

    char pad_00[8];
    void *m_8;
    void *m_c;
    char pad_10[0x44 - 0x10];
    int m_44;
    int m_48;
    char pad_4c[0x50 - 0x4c];
    void *m_50;
    void *m_54;
    char pad_58[0x74 - 0x58];
    std::vector<std::string> m_74;
    char pad_84[0x88 - 0x84];
    G2MIDRECORD *m_88;
    char pad_8c[0x98 - 0x8c];
    bool m_98;
    char pad_99[0xa0 - 0x99];
};

void G2MIDTABLE::Insert(G2MIDITEM *item, const std::string &name, int flag)
{
    if (m_98) {
        for (unsigned int i = 0; i < m_74.size(); i++) {
            if (m_74[i].compare(name) != 0)
                continue;
            G2MIDRECORD *base = &m_88[i];
            G2MIDRECORD record;
            record.m_0 = base->m_0;
            record.m_4 = base->m_4;
            record.m_8 = base->m_8;
            record.m_c = base->m_c;
            if (!record.m_0 && !record.m_4 && !record.m_8 && !record.m_c)
                continue;
            if (!m_50)
                continue;
            G2MIDRESOLVED *out = 0;
            if (item->Resolve(0, (void **)&out) != 0)
                continue;
            Store(out, flag, &record);
            out->Release();
        }
    } else {
        G2MIDRECORD found;
        if (ResolveName(name, &found))
            InsertDirect(item, flag, &found);
    }
}

void G2MIDTABLE::InsertDirect(G2MIDITEM *item, int flag, G2MIDRECORD *found)
{
    if (!m_50)
        return;
    G2MIDRESOLVED *out = 0;
    if (item->Resolve(0, (void **)&out) == 0) {
        Store(out, flag, found);
        out->Release();
    }
}

// The table's worker (0x454806); it takes the record and the table's mode.
extern "C" int __stdcall GD454806Store(void *object, int zero, int flag,
                                       void *device, int op, int stride,
                                       int zero2, const G2MIDRECORD *record,
                                       int two, int mask);

void G2MIDTABLE::Store(G2MIDRESOLVED *object, int flag, const G2MIDRECORD *record)
{
    if (!m_50)
        return;
    if (!m_98)
        Prepare();
    if (GD454806Store(object, 0, flag, m_c, 0x16, m_44 * 4, 0, record,
                      2, 0xffff00ff) < 0) {
        if (g_pLog)
            g_pLog->Error("not rendered surface in html rendering!");
    }
}

void G2MIDTABLE::Prepare()
{
    G2MIDPREPAREV4 temp = { 0, 0, m_44, m_48 };
    ((G2MIDPREPAREIFACE *)m_50)->V3(1, -1, 0, 0, 0, m_8, &temp, 0, 0, 0);
}

// The object G2MIDLIST holds at +4: a released item and, at +0x28, the
// table the items are inserted into.
class G2MIDLISTIMPL
{
public:
    virtual void Release(int value);

    __declspec(noinline) bool Insert(G2MIDITEM *item, const std::string &name, int flag);

    char pad_04[0x28 - 0x04];
    G2MIDTABLE m_28;
    char pad_c8[0x130 - (0x28 + 0xa0)];
    bool m_130;
    bool m_131;
};

bool G2MIDLISTIMPL::Insert(G2MIDITEM *item, const std::string &name, int flag)
{
    if (!m_131 || !item)
        return false;
    m_28.Insert(item, name, flag);
    return true;
}

class G2MIDLIST
{
public:
    ~G2MIDLIST();
    void Add(G2MIDITEM *item, const char *name);
    bool IsReady();

    G2MIDITEM *m_00;
    G2MIDLISTIMPL *m_04;
    char pad_08[0x20c - 0x08];
    std::string m_20c;
};

void G2MIDLIST::Add(G2MIDITEM *item, const char *name)
{
    if (m_04) {
        std::string key(name);
        m_04->Insert(item, key, 0);
    }
}

bool G2MIDLIST::IsReady()
{
    if (!m_04)
        return false;
    return m_04->m_130;
}

extern G2MIDLIST *g_pMIDList;

G2MIDLIST::~G2MIDLIST()
{
    if (m_04) {
        m_04->Release(1);
        m_04 = 0;
        if (m_00) {
            m_00->Release(1);
            m_00 = 0;
        }
    }
}

class GD4451f0 : public GUI
{
public:
    virtual ~GD4451f0();
};

GD4451f0::~GD4451f0()
{
    if (g_pMIDList) {
        delete g_pMIDList;
        g_pMIDList = 0;
    }
}


class GD447e50 : public APP
{
public:
    virtual ~GD447e50() throw();
};

__declspec(noinline) GD447e50::~GD447e50() throw()
{
}

class GD4482b0
{
public:
    virtual ~GD4482b0();
};

__declspec(noinline) GD4482b0::~GD4482b0()
{
}

class GD5529f0
{
public:
    virtual ~GD5529f0();
};

// Two strings live at 0x62c and 0x648; the rest is the base and padding.
class GD4484c0 : public GD5529f0
{
public:
    virtual ~GD4484c0();

    char pad_00[0x62c - sizeof(GD5529f0)];
    std::string m_62c;
    std::string m_648;
};

GD4484c0::~GD4484c0()
{
}

class GD449130
{
public:
    virtual ~GD449130();
};

__declspec(noinline) GD449130::~GD449130()
{
}


class GD44c1b0
{
public:
    virtual ~GD44c1b0();
};

__declspec(noinline) GD44c1b0::~GD44c1b0()
{
}


// The engine object the dtor talks to at 0x6cb318; only its third virtual
// matters here.
class GD4512e0GLOBAL
{
public:
    virtual void V0();
    virtual void V1();
    virtual void V2();
};

extern GD4512e0GLOBAL *g_pGD4512e0Global;

class GD450e80
{
public:
    virtual ~GD450e80();

    void Destroy();

    char pad_04[0x18 - 0x04];
    int m_18;
};

__declspec(noinline) GD450e80::~GD450e80()
{
    m_18 = 0xc0000001;
    g_pGD4512e0Global->V2();
    Destroy();
}

// The 0x44f840/0x44bb50 member at +8: Stop runs first, the destructor is the
// compiler's own call.
class GD4512e0RELEASE
{
public:
    virtual void __stdcall V0(const void *iid, void **out);
    virtual void __stdcall V1();
    virtual void __stdcall Release();
    virtual void __stdcall V3(int value);
    virtual void __stdcall V4();
    virtual void __stdcall V5();
    virtual void __stdcall V6(int value);
    virtual void __stdcall V7(int a, int b, int c);
    virtual void __stdcall V8();
    virtual void __stdcall V9();
    virtual void __stdcall V10();
    virtual void __stdcall V11();
    virtual void __stdcall V12();
    virtual void __stdcall V13();
    virtual void __stdcall V14();
    virtual void __stdcall V15();
    virtual void __stdcall V16();
    virtual void __stdcall V17();
    virtual void __stdcall V18();
    virtual void __stdcall V19();
    virtual void __stdcall V20(int value);
};

void __stdcall GD452a90SetMedia(void *device, const void *format, int value);

extern "C" const unsigned char g_GD4512e0StopIID[16] = {
    0xa3, 0x01, 0x48, 0xfc, 0xa9, 0x2b, 0xcf, 0x11,
    0xa2, 0x29, 0x00, 0xaa, 0x00, 0x3d, 0x73, 0x52
};

// A local interface pointer that releases itself when the block leaves.
class GD4512e0PTR
{
public:
    GD4512e0PTR() { m_p = 0; }
    ~GD4512e0PTR() { if (m_p) m_p->Release(); }

    GD4512e0RELEASE *m_p;
};

class GD4512e0MEMBER
{
public:
    ~GD4512e0MEMBER();
    void Stop();

    // Releases the heap block at 0x14 (0x44b610).
    void FreeHeap()
    {
        if (m_14)
            HeapFree(GetProcessHeap(), 0, m_14);
    }

    char pad_00[0x14];
    void *m_14;
    char pad_18[0x3c - 0x18];
    GD4512e0RELEASE *m_3c;
    char pad_40[0x54 - 0x40];
    GD4512e0RELEASE *m_54;
    char pad_58[0x60 - 0x58];
    GD4512e0RELEASE *m_60;
    GD4512e0RELEASE *m_64;
    GD4512e0RELEASE *m_68;
    GD4512e0RELEASE *m_6c;
    GD4512e0RELEASE *m_70;
    GD4512e0RELEASE *m_74;
    GD4512e0RELEASE *m_78;
    GD4512e0RELEASE *m_7c;
    char m_80[0x10];
    int m_90;
    int m_94;
    int m_98;
    int m_9c;
    char pad_a0[0xc4 - 0xa0];
    HANDLE m_c4;
    char pad_c8[0xd8 - 0xc8];
    GD4512e0RELEASE *m_d8;
    GD4512e0RELEASE *m_dc;
    char pad_e0[0xe8 - 0xe0];
    unsigned short *m_e8;
};

// Frees the member's heap block when the destructor leaves, on any path.
class GD4512e0GUARD
{
public:
    GD4512e0GUARD(GD4512e0MEMBER *owner) : m_owner(owner) {}
    ~GD4512e0GUARD() { m_owner->FreeHeap(); }

    GD4512e0MEMBER *m_owner;
};

__declspec(noinline) GD4512e0MEMBER::~GD4512e0MEMBER()
{
    GD4512e0GUARD guard(this);
    SysFreeString(m_e8);
    if (m_dc) m_dc->Release();
    if (m_d8) m_d8->Release();
    if (m_7c) m_7c->Release();
    if (m_78) m_78->Release();
    if (m_74) m_74->Release();
    if (m_70) m_70->Release();
    if (m_6c) m_6c->Release();
    if (m_68) m_68->Release();
    if (m_64) m_64->Release();
    if (m_60) m_60->Release();
    if (m_54) m_54->Release();
    if (m_3c) m_3c->Release();
}

__declspec(noinline) void GD4512e0MEMBER::Stop()
{
    if (m_98 & 0x40)
        return;
    m_98 |= 0x40;

    if (m_70)
        m_70->V7(1, 0, 0);

    if (m_94 != 0xcdcdcdcd) {
        GD4512e0RELEASE *device = m_60;
        GD452a90SetMedia(device, m_80, m_94);
        m_94 = 0xcdcdcdcd;
    }

    if (m_64) {
        m_64->V20(m_9c);
        m_64->V6(1);
        m_64->V3(0);
    }

    if (m_60) {
        GD4512e0PTR ptr;
        m_60->V0(g_GD4512e0StopIID, (void **)&ptr.m_p);
        if (ptr.m_p)
            ptr.m_p->V3(0);
    }

    GD4512e0RELEASE *release;
    if (m_70) {
        release = m_70;
        m_70 = 0;
        release->Release();
    }
    m_90 = 0;
    if (m_74) {
        release = m_74;
        m_74 = 0;
        release->Release();
    }
    if (m_64) {
        release = m_64;
        m_64 = 0;
        release->Release();
    }
    if (m_60) {
        release = m_60;
        m_60 = 0;
        release->Release();
    }
    if (m_6c) {
        release = m_6c;
        m_6c = 0;
        release->Release();
    }
    if (m_68) {
        release = m_68;
        m_68 = 0;
        release->Release();
    }
    m_98 &= ~0x5b;
    if (m_c4) {
        DestroyAcceleratorTable(m_c4);
        m_c4 = 0;
    }
}

class GD4512e0
{
public:
    virtual ~GD4512e0();

    int m_4;
    GD4512e0MEMBER m_8;
};

GD4512e0::~GD4512e0()
{
    m_4 = 0xc0000001;
    m_8.Stop();
    g_pGD4512e0Global->V2();
}

// The primary base (0..0x24) and the interface base at 0x24. Both have
// polymorphic vtables but trivial destructors, so the derived dtor only
// stores their vptrs and never calls a base dtor.
class GD4516f0PRIMARY
{
public:
    virtual void V0();
    virtual void V1();
    virtual void V2();

    void Destroy();

    HWND m_4;
    char pad_8[0x14 - 0x8];
    void *m_14;
    char pad_18[0x24 - 0x18];
};

class GD4516f0IFACE
{
public:
    virtual void W0();
    virtual void W1();
    virtual void W2();
};

class GD4516f0 : public GD4516f0PRIMARY, public GD4516f0IFACE
{
public:
    virtual ~GD4516f0();

    void Cleanup();

    int m_28;
    char pad_2c[0x44 - 0x2c];
    GD4512e0RELEASE *m_44;
};

__declspec(noinline) void GD4516f0::Cleanup()
{
    GD4512e0RELEASE *release = m_44;
    if (release) {
        m_44 = 0;
        release->Release();
    }
    if (m_4)
        DestroyWindow(m_4);
}

__declspec(noinline) GD4516f0::~GD4516f0()
{
    m_28 = 0xc0000001;
    Cleanup();
    g_pGD4512e0Global->V2();
    Destroy();
}

class GD451880 : public GD4516f0PRIMARY, public GD4516f0IFACE
{
public:
    virtual ~GD451880();

    int m_28;
    char pad_2c[0x48 - 0x2c];
};

__declspec(noinline) GD451880::~GD451880()
{
    m_28 = 0xc0000001;
    ((GD4516f0 *)this)->Cleanup();
    g_pGD4512e0Global->V2();
    Destroy();
}

// One of the COM-like objects released through slot +8.
class GD452a90COM
{
public:
    virtual void __stdcall V0();
    virtual void __stdcall V1();
    virtual void __stdcall Release();
};

// The object at +0xa4: a media sample with its format at +0x28.
struct GD452a90A4 : public GD452a90COM
{
    char pad_04[0x28 - 0x04];
    int m_28;
};

extern const char g_GD452a90Format[];

class GD452a90
{
public:
    virtual ~GD452a90();

    // Releases the handles in the member fields.
    void Close();
    void CloseDirect();

    char pad_04[0x08 - 0x04];
    HDC m_8;
    char pad_0c[0x3c - 0x0c];
    HGDIOBJ m_3c;
    HBITMAP m_40;
    char pad_44[0x4c - 0x44];
    HWND m_4c;
    GD452a90COM *m_50;
    GD452a90COM *m_54;
    std::string m_58;
    std::vector<std::string> m_74;
    std::vector<int> m_84;
    char pad_94[0x98 - 0x94];
    bool m_98;
    char pad_99[0x9c - 0x99];
    DWORD m_9c;
    HANDLE m_a0;
    GD452a90A4 *m_a4;
    V44AC00 m_a8;
};

__declspec(noinline) GD452a90::~GD452a90()
{
    Close();
}

void GD452a90::Close()
{
    if (m_98) {
        if (m_a0) {
            PostThreadMessageA(m_9c, WM_QUIT, 0, 0);
            WaitForSingleObject(m_a0, INFINITE);
            CloseHandle(m_a0);
            m_a0 = 0;
            m_9c = 0;
        }
    } else {
        CloseDirect();
    }
    if (IsWindow(m_4c)) {
        if (DestroyWindow(m_4c))
            m_4c = 0;
    }
}
void GD452a90::CloseDirect()
{
    GD452a90COM *first = m_50;
    if (first) {
        first->Release();
        m_50 = 0;
    }
    GD452a90A4 *sample = m_a4;
    if (sample) {
        GD452a90SetMedia(m_54, g_GD452a90Format, sample->m_28);
        sample->m_28 = 0xfefefefe;
        m_a4->Release();
        m_a4 = 0;
    }
    GD452a90COM *second = m_54;
    if (second) {
        second->Release();
        m_54 = 0;
    }
    if (m_40) {
        SelectObject(m_8, m_40);
        DeleteDC(m_8);
        m_8 = 0;
        m_40 = 0;
    }
    if (m_3c) {
        DeleteObject(m_3c);
        m_3c = 0;
    }
}

// The WildTangent info blob the two code-name accessors belong to. Its
// GetString lives in the 0x54f8c0 block; only the accessors are here.
class WTINFO
{
public:
    virtual ~WTINFO();

    std::string GetString(const std::string &key);
    std::string GetDPCodeName();
    std::string GetProductCodeName();
};

std::string WTINFO::GetDPCodeName()
{
    std::string key("DPCodeName", 10);
    return GetString(key);
}

std::string WTINFO::GetProductCodeName()
{
    std::string key("ProductCodeName", 15);
    return GetString(key);
}

// A COM-like value: Release is the third virtual.
class GD453360VALUE
{
public:
    virtual void __stdcall QueryInterface();
    virtual void __stdcall AddRef();
    virtual void __stdcall Release();
};

class GD453360
{
public:
    virtual ~GD453360();

    std::map<std::string, GD453360VALUE *> m_4;
};

__declspec(noinline) GD453360::~GD453360()
{
    for (std::map<std::string, GD453360VALUE *>::iterator it = m_4.begin(); it != m_4.end(); ++it) {
        if (it->second) {
            it->second->Release();
            it->second = 0;
        }
    }
    m_4.clear();
}

struct GD453c50VALUE
{
    char m_00[0x1c];
};

typedef std::vector<std::string> GD453c50OBJECTLIST;

class GD453c50OBJECT
{
public:
    ~GD453c50OBJECT();

    int m_0;
    GD453c50OBJECTLIST m_4;
};

GD453c50OBJECT::~GD453c50OBJECT()
{
    m_4.~GD453c50OBJECTLIST();
    m_0 = 0;
}

class GD453c50
{
public:
    virtual ~GD453c50();

    std::map<std::string, GD453c50VALUE> m_4;
    GD453c50OBJECT *m_10;
};

__declspec(noinline) GD453c50::~GD453c50()
{
    if (m_10) {
        delete m_10;
        m_10 = 0;
    }
    m_4.clear();
}

class GD44a560 : public GD453360, public GD453c50
{
public:
    virtual ~GD44a560();

    char pad_24[0x28 - 0x24];
    GD452a90 m_28;
    char pad_34[0x130 - (0x28 + sizeof(GD452a90))];
    bool m_130;
    bool m_131;
};

__declspec(noinline) GD44a560::~GD44a560()
{
    m_28.Close();
    m_130 = false;
    m_131 = false;
}

struct G2GC4GET
{
    char Get31();
    char Get30();
};

char G2GC4GET::Get31()
{
    char *p = *(char **)((char *)this + 0xa4);
    if (p)
        return p[0x31];
    return 0;
}

char G2GC4GET::Get30()
{
    char *p = *(char **)((char *)this + 0xa4);
    if (p)
        return p[0x30];
    return 0;
}

struct G2GC4SET
{
    void SetF0(int value);
};

void G2GC4SET::SetF0(int value)
{
    *(int *)((char *)this + 0xf0) = value;
}

extern "C" void (__stdcall *g_fn656294)(void *);
extern "C" void (__stdcall *g_fn656284)(void *);

struct G2GC4CALL
{
    void CallA();
    void CallB();
};

void G2GC4CALL::CallA()
{
    g_fn656294(*(void **)this);
}

void G2GC4CALL::CallB()
{
    g_fn656284(this);
}

extern "C" void (__stdcall *g_fn656120)(void *);

struct G2GC4INIT
{
    int Init();
};

int G2GC4INIT::Init()
{
    g_fn656120(*(void **)this);
    *(char *)((char *)this + 4) = 1;
    return 0;
}

typedef void (__stdcall *G2GC4RELEASEFN)(void *);

struct G2GC4REL
{
    void Release();
};

__declspec(noinline) void G2GC4REL::Release()
{
    char *p = *(char **)this;
    if (p)
        (*(G2GC4RELEASEFN *)((char *)*(void **)p + 8))(p);
}

struct G2GC4JMPREL
{
    void Release();
};

void G2GC4JMPREL::Release()
{
    ((G2GC4REL *)this)->Release();
}

class APPOPTIONS
{
public:
    virtual ~APPOPTIONS() {}
    void Save();
};

// The object the +0x? global points at; slot 0 takes the count.
class GD4477d0GLOBAL
{
public:
    virtual void Release(int value);
};

extern GD4477d0GLOBAL *g_pGD4477d0Global;

class GD4477d0 : public APPOPTIONS
{
public:
    virtual ~GD4477d0();
};

__declspec(noinline) GD4477d0::~GD4477d0()
{
    if (g_pGD4477d0Global) {
        g_pGD4477d0Global->Release(1);
        g_pGD4477d0Global = 0;
    }
}

struct G2GC4CPY
{
    void Set60(const void *src);
};

void G2GC4CPY::Set60(const void *src)
{
    char *dest = (char *)this + 0x60;
    memcpy(dest, src, 0x200);
    *(int *)(dest + 0x200) = *(int *)((char *)src + 0x200);
}

struct G2GC4GETC
{
    void *GetConst();
    void *GetGlobal();
};

extern "C" char DAT_0065fefc;

void *G2GC4GETC::GetConst()
{
    return &DAT_0065fefc;
}

extern void *g_6ad674;

void *G2GC4GETC::GetGlobal()
{
    return g_6ad674;
}

class G4K80
{
public:
    virtual void d0(); virtual void d1(); virtual void d2(); virtual void d3(); virtual void d4(); virtual void d5(); virtual void d6(); virtual void d7();
    virtual void d8(); virtual void d9(); virtual void d10(); virtual void d11(); virtual void d12(); virtual void d13(); virtual void d14(); virtual void d15();
    virtual void d16(); virtual void d17(); virtual void d18(); virtual void d19(); virtual void d20(); virtual void d21(); virtual void d22(); virtual void d23();
    virtual void d24(); virtual void d25(); virtual void d26(); virtual void d27(); virtual void d28(); virtual void d29(); virtual void d30(); virtual void d31();
    virtual void T();
    void Thunk();
};

void G4K80::Thunk()
{
    T();
}

class G4K84
{
public:
    virtual void d0(); virtual void d1(); virtual void d2(); virtual void d3(); virtual void d4(); virtual void d5(); virtual void d6(); virtual void d7();
    virtual void d8(); virtual void d9(); virtual void d10(); virtual void d11(); virtual void d12(); virtual void d13(); virtual void d14(); virtual void d15();
    virtual void d16(); virtual void d17(); virtual void d18(); virtual void d19(); virtual void d20(); virtual void d21(); virtual void d22(); virtual void d23();
    virtual void d24(); virtual void d25(); virtual void d26(); virtual void d27(); virtual void d28(); virtual void d29(); virtual void d30(); virtual void d31();
    virtual void d32();
    virtual void T();
    void Thunk();
};

void G4K84::Thunk()
{
    T();
}

extern "C" char DAT_0065fd58;
void *GetD0065fd58()
{
    return &DAT_0065fd58;
}

extern "C" char DAT_006601e4;
void *GetD006601e4()
{
    return &DAT_006601e4;
}

extern "C" char DAT_00660438;
void *GetD00660438()
{
    return &DAT_00660438;
}

extern "C" char DAT_006609a0;
void *GetD006609a0()
{
    return &DAT_006609a0;
}

extern "C" char DAT_00660ae0;
void *GetD00660ae0()
{
    return &DAT_00660ae0;
}

extern "C" char DAT_00660bc8;
void *GetD00660bc8()
{
    return &DAT_00660bc8;
}

extern "C" char DAT_00661ae4;
void *GetD00661ae4()
{
    return &DAT_00661ae4;
}

extern "C" char DAT_00661b24;
void *GetD00661b24()
{
    return &DAT_00661b24;
}

extern "C" void FUN_0044e2c0();
void *GetF0044e2c0()
{
    return (void *)&FUN_0044e2c0;
}

int GetK5a()
{
    return 0x5a;
}

int GetK3()
{
    return 0x3;
}

struct GC4GET
{
    char Get0(void);
    char Get1(void);
    int Get2(void);
    int Get3(void);
    int Get4(void);
};

char GC4GET::Get0(void)
{
    return *(char *)((char *)this + 0x71);
}

char GC4GET::Get1(void)
{
    return *(char *)((char *)this + 0x1c);
}

int GC4GET::Get2(void)
{
    return *(int *)((char *)this + 0x1ec);
}

int GC4GET::Get3(void)
{
    return *(int *)((char *)this + 0xe8);
}

int GC4GET::Get4(void)
{
    return *(int *)((char *)this + 0x50);
}

struct G4SET
{
    void Set0();
};

void G4SET::Set0()
{
    *(bool *)((char *)this + 0x242) = true;
}

struct GC4SET
{
    void Set0(int value);
};

void GC4SET::Set0(int value)
{
    *(int *)((char *)this + 0x410) = value;
}

// The progress bar's master names the two decals it draws with.
class PROGRESSBARMASTER : public RESOURCEMASTER
{
public:
    FIXEDSTRING<0x200> m_bgDecal;
    FIXEDSTRING<0x200> m_fgDecal;
};

// One progress bar: a background and a foreground decal.
class PROGRESSBAR : public OBJECT
{
public:
    void Start();
    void SetDecal(RESHANDLE &handle, const char *name, bool active);

    float m_400;
    float m_404;
    RESHANDLE m_bgDecal;
    RESHANDLE m_fgDecal;
    int m_420;
    float m_424;
    int m_428;
    float m_42c;
};

void PROGRESSBAR::Start()
{
    PROGRESSBARMASTER *master = (PROGRESSBARMASTER *)m_master;
    if (!master)
        return;
    m_bgDecal.Invalidate();
    if ((int)strlen(master->m_bgDecal) > 0)
        SetDecal(m_bgDecal, master->m_bgDecal, false);
    m_fgDecal.Invalidate();
    if ((int)strlen(master->m_fgDecal) > 0)
        SetDecal(m_fgDecal, master->m_fgDecal, false);
}

// What a shadow prop's file sets: the decal and the pivot it is placed by.
class PROPSHADOWMASTER : public RESOURCEMASTER
{
public:
    FIXEDSTRING<0x200> m_decalName;
    int m_260;
    V2 m_pivot;
};

// A decal placed under an actor to stand in for its shadow.
class PROPSHADOW : public OBJECT
{
public:
    void Stop();
    void Update();

    FIXEDSTRING<0x200> m_name;
    int m_600;
    RESHANDLE m_decal;
    RESHANDLE m_610;
    float m_angle;
};

void PROPSHADOW::Stop()
{
    RESOURCE *decal = g_pResources->GetResource(m_decal);
    if (decal)
        decal->Virtual20();
    m_decal.Invalidate();
}

static __forceinline float G2GC4ShadowAngle(WTACTOR *parent)
{
    V3 direction = parent->GetWorldAngles().GetDirection();
    return atan2f(direction.x, direction.z) * 57.29578f;
}

void PROPSHADOW::Update()
{
    OBJECT::Update();
    if (!m_active)
        return;
    DECAL *decal = (DECAL *)g_pResources->GetResource(m_decal);
    if (!decal)
        return;
    WTACTOR *parent = GetParentActor();
    if (!parent)
        return;
    decal->SetAlpha(GetAlpha());
    decal->SetLayer(GetLayer());
    decal->SetScaleV(GetScale());
    PROPSHADOWMASTER *master = (PROPSHADOWMASTER *)m_master;
    float angle = G2GC4ShadowAngle(parent) + m_angle;
    if (angle < 0.0f)
        angle += 360.0f;
    else if (angle > 360.0f)
        angle -= 360.0f;
    bool flip = (angle > 45.0f && angle < 135.0f) || (angle > 225.0f && angle < 315.0f);
    V2 offset;
    offset.x = (master->m_pivot.x - 0.5f) * GetScale().x;
    offset.y = (master->m_pivot.y - 0.5f) * GetScale().y;
    if (m_angle != 0.0f)
        offset = offset.Rotate(m_angle * 0.017453292f);
    V3 position = GetPosition();
    ANG angles(0.0f, 0.0f, 0.0f);
    if (!flip) {
        angles.SetEuler(180.0f, 90.0f, 0.0f);
        position.x -= offset.x;
        position.z += offset.y;
    } else {
        angles.SetEuler(-90.0f, 0.0f, 90.0f);
        position.z += offset.x;
        position.x -= offset.y;
    }
    if (m_angle != 0.0f) {
        MATRIX m;
        m.RotateY(m_angle);
        angles.SetMatrix(m * angles.GetMatrix());
    }
    decal->SetAngles(angles);
    decal->SetPosition(position);
}

// The core object's scalar fields that its properties write one at a time.
struct G2CORE4M1
{
    void Set9C(const int &value);
    void Set88(const bool &value);
    void Set89(const bool &value);
    void Set90(const int &value);
    void Set94(const int &value);
    void Set98(const int &value);
    void SetA0(const int &value);
    void SetA4(const int &value);
    void SetBC(const int &value);
    void SetB0(const int &value);
    void SetA8(const int &value);
    void SetAC(const int &value);
    void SetB8(const int &value);
    void SetC0(const int &value);
    void SetDC(const int &value);
    void SetE0(const int &value);
    void Set30C(const int &value);
    void Set1CCB4(const int &value);
    void Set1288C(const int &value);
    void Set12890(const int &value);
    void Set12894(const int &value);
    void Set12898(const int &value);
    void Set1289C(const int &value);
    void Set128A0(const bool &value);
    void Set128A4(const int &value);
    void Set128A8(const bool &value);
    void Set128A9(const bool &value);
    void Set128AC(const int &value);
    void Set128B0(const bool &value);
    void Set1DCB8(const int &value);
    void Set1DCBC(const int &value);
    void Set1DCC0(const int &value);
    void Set1DCC4(const int &value);
    void Set1DCC8(const int &value);
    void Set1DCCC(const int &value);
    void Set1DCD0(const int &value);
    void Set1DCD4(const int &value);
    void Set1DCD8(const int &value);
};

void G2CORE4M1::Set9C(const int &value)
{
    *(int *)((char *)this + 0x9c) = value;
}

void G2CORE4M1::Set88(const bool &value)
{
    *(bool *)((char *)this + 0x88) = value;
}

void G2CORE4M1::Set89(const bool &value)
{
    *(bool *)((char *)this + 0x89) = value;
}

void G2CORE4M1::Set90(const int &value)
{
    *(int *)((char *)this + 0x90) = value;
}

void G2CORE4M1::Set94(const int &value)
{
    *(int *)((char *)this + 0x94) = value;
}

void G2CORE4M1::Set98(const int &value)
{
    *(int *)((char *)this + 0x98) = value;
}

void G2CORE4M1::SetA0(const int &value)
{
    *(int *)((char *)this + 0xa0) = value;
}

void G2CORE4M1::SetA4(const int &value)
{
    *(int *)((char *)this + 0xa4) = value;
}

void G2CORE4M1::SetBC(const int &value)
{
    *(int *)((char *)this + 0xbc) = value;
}

void G2CORE4M1::SetB0(const int &value)
{
    *(int *)((char *)this + 0xb0) = value;
}

void G2CORE4M1::SetA8(const int &value)
{
    *(int *)((char *)this + 0xa8) = value;
}

void G2CORE4M1::SetAC(const int &value)
{
    *(int *)((char *)this + 0xac) = value;
}

void G2CORE4M1::SetB8(const int &value)
{
    *(int *)((char *)this + 0xb8) = value;
}

void G2CORE4M1::SetC0(const int &value)
{
    *(int *)((char *)this + 0xc0) = value;
}

void G2CORE4M1::SetDC(const int &value)
{
    *(int *)((char *)this + 0xdc) = value;
}

void G2CORE4M1::SetE0(const int &value)
{
    *(int *)((char *)this + 0xe0) = value;
}

void G2CORE4M1::Set30C(const int &value)
{
    *(int *)((char *)this + 0x30c) = value;
}

void G2CORE4M1::Set1CCB4(const int &value)
{
    *(int *)((char *)this + 0x1ccb4) = value;
}

void G2CORE4M1::Set1288C(const int &value)
{
    *(int *)((char *)this + 0x1288c) = value;
}

void G2CORE4M1::Set12890(const int &value)
{
    *(int *)((char *)this + 0x12890) = value;
}

void G2CORE4M1::Set12894(const int &value)
{
    *(int *)((char *)this + 0x12894) = value;
}

void G2CORE4M1::Set12898(const int &value)
{
    *(int *)((char *)this + 0x12898) = value;
}

void G2CORE4M1::Set1289C(const int &value)
{
    *(int *)((char *)this + 0x1289c) = value;
}

void G2CORE4M1::Set128A0(const bool &value)
{
    *(bool *)((char *)this + 0x128a0) = value;
}

void G2CORE4M1::Set128A4(const int &value)
{
    *(int *)((char *)this + 0x128a4) = value;
}

void G2CORE4M1::Set128A8(const bool &value)
{
    *(bool *)((char *)this + 0x128a8) = value;
}

void G2CORE4M1::Set128A9(const bool &value)
{
    *(bool *)((char *)this + 0x128a9) = value;
}

void G2CORE4M1::Set128AC(const int &value)
{
    *(int *)((char *)this + 0x128ac) = value;
}

void G2CORE4M1::Set128B0(const bool &value)
{
    *(bool *)((char *)this + 0x128b0) = value;
}

void G2CORE4M1::Set1DCB8(const int &value)
{
    *(int *)((char *)this + 0x1dcb8) = value;
}

void G2CORE4M1::Set1DCBC(const int &value)
{
    *(int *)((char *)this + 0x1dcbc) = value;
}

void G2CORE4M1::Set1DCC0(const int &value)
{
    *(int *)((char *)this + 0x1dcc0) = value;
}

void G2CORE4M1::Set1DCC4(const int &value)
{
    *(int *)((char *)this + 0x1dcc4) = value;
}

void G2CORE4M1::Set1DCC8(const int &value)
{
    *(int *)((char *)this + 0x1dcc8) = value;
}

void G2CORE4M1::Set1DCCC(const int &value)
{
    *(int *)((char *)this + 0x1dccc) = value;
}

void G2CORE4M1::Set1DCD0(const int &value)
{
    *(int *)((char *)this + 0x1dcd0) = value;
}

void G2CORE4M1::Set1DCD4(const int &value)
{
    *(int *)((char *)this + 0x1dcd4) = value;
}

void G2CORE4M1::Set1DCD8(const int &value)
{
    *(int *)((char *)this + 0x1dcd8) = value;
}

struct G2CORE4M2
{
    void Set264(const V2 &value);
    void Set280(const V2 &value);
    void Set498(const V2 &value);
    void Set4AC(const V2 &value);
    void SetA4(const V2 &value);
    void SetA8(const V2 &value);
    void Set94(const V2 &value);
    void Set9C(const V2 &value);
    void SetC4(const V2 &value);
    void Set310(const V2 &value);
};

void G2CORE4M2::Set264(const V2 &value)
{
    *(V2 *)((char *)this + 0x264) = value;
}

void G2CORE4M2::Set280(const V2 &value)
{
    *(V2 *)((char *)this + 0x280) = value;
}

void G2CORE4M2::Set498(const V2 &value)
{
    *(V2 *)((char *)this + 0x498) = value;
}

void G2CORE4M2::Set4AC(const V2 &value)
{
    *(V2 *)((char *)this + 0x4ac) = value;
}

void G2CORE4M2::SetA4(const V2 &value)
{
    *(V2 *)((char *)this + 0xa4) = value;
}

void G2CORE4M2::SetA8(const V2 &value)
{
    *(V2 *)((char *)this + 0xa8) = value;
}

void G2CORE4M2::Set94(const V2 &value)
{
    *(V2 *)((char *)this + 0x94) = value;
}

void G2CORE4M2::Set9C(const V2 &value)
{
    *(V2 *)((char *)this + 0x9c) = value;
}

void G2CORE4M2::SetC4(const V2 &value)
{
    *(V2 *)((char *)this + 0xc4) = value;
}

void G2CORE4M2::Set310(const V2 &value)
{
    *(V2 *)((char *)this + 0x310) = value;
}

struct G2CORE4M5
{
    void Set274(const V3 &value);
    void Set48C(const V3 &value);
    void Set4A0(const V3 &value);
    void SetB0(const V3 &value);
};

void G2CORE4M5::Set274(const V3 &value)
{
    *(V3 *)((char *)this + 0x274) = value;
}

void G2CORE4M5::Set48C(const V3 &value)
{
    *(V3 *)((char *)this + 0x48c) = value;
}

void G2CORE4M5::Set4A0(const V3 &value)
{
    *(V3 *)((char *)this + 0x4a0) = value;
}

void G2CORE4M5::SetB0(const V3 &value)
{
    *(V3 *)((char *)this + 0xb0) = value;
}

// Starts the program .htm files open with (SHELL.cpp).
void OpenURL(const char *url);

// The game's core object runs these from the options screens, the tips and
// the scene. Placeholder class names: each class carries only the virtuals
// its command calls through a pointer, so its vtable is just a slot count.
struct G2CORE4CM1
{
    virtual SRESULT Command(CMDCALL *call);
};

SRESULT G2CORE4CM1::Command(CMDCALL *call)
{
    int unused = 0;
    g_pApp->SetFullscreen(false);
    g_pDisplay->Virtual22();
    OpenURL(call->tokens->GetToken(1));
    return SRESULT(true);
}

struct G2CORE4CM2
{
    virtual SRESULT Command(CMDCALL *call);
};

SRESULT G2CORE4CM2::Command(CMDCALL *call)
{
    int unused = 0;
    g_pApp->SetFullscreen(false);
    g_pDisplay->Virtual22();
    FIXEDSTRING<0x200> url;
    SafeSprintf(url, "file://%s%s", g_basePath, call->tokens->GetToken(1));
    OpenURL(url);
    return SRESULT(true);
}

// The options the core object copies out of the saved ones.
struct G2CORE4CM3
{
    virtual SRESULT Command(int value);
};

SRESULT G2CORE4CM3::Command(int value)
{
    int unused = 0;
    *(int *)((char *)this + 0x1289c) = *(int *)((char *)g_pApp->m_pOptions + 0x5268);
    *(bool *)((char *)this + 0x128a0) = *(bool *)((char *)g_pApp->m_pOptions + 0x526c);
    *(int *)((char *)this + 0x128a4) = *(int *)((char *)g_pApp->m_pOptions + 0x5270);
    *(bool *)((char *)this + 0x128a8) = *(bool *)((char *)g_pApp->m_pOptions + 0x5274);
    return SRESULT(true);
}

// The setters that take one of those options and apply it.
struct G2CORE4APP
{
    void SetOption68(int value);
    void SetOption6C(bool value);
    void SetOption70(int value);
    void SetOption74(bool value);
};

struct G2CORE4CM4
{
    virtual SRESULT Command(int value);
};

SRESULT G2CORE4CM4::Command(int value)
{
    int unused = 0;
    ((G2CORE4APP *)g_pApp)->SetOption68(*(int *)((char *)this + 0x1289c));
    ((G2CORE4APP *)g_pApp)->SetOption6C(*(bool *)((char *)this + 0x128a0));
    ((G2CORE4APP *)g_pApp)->SetOption70(*(int *)((char *)this + 0x128a4));
    ((G2CORE4APP *)g_pApp)->SetOption74(*(bool *)((char *)this + 0x128a8));
    return SRESULT(true);
}

struct G2CORE4CM5
{
    virtual SRESULT Command(int value);
};

SRESULT G2CORE4CM5::Command(int value)
{
    int unused = 0;
    *(bool *)((char *)this + 0x128a9) = *(bool *)((char *)g_pApp->m_pOptions + 0x110);
    *(int *)((char *)this + 0x128ac) = *(int *)((char *)g_pApp->m_pOptions + 0x10c);
    *(bool *)((char *)this + 0x128b0) = *(bool *)((char *)g_pApp->m_pOptions + 0x5275);
    return SRESULT(true);
}

// The resource manager's side of those options.
struct G2CORE4RES
{
    void ApplyOptions(int &a, bool &b, int &c, bool &d);
};

struct G2CORE4CM6
{
    virtual SRESULT Command(int value);
};

SRESULT G2CORE4CM6::Command(int value)
{
    int unused = 0;
    ((G2CORE4RES *)g_pResources)->ApplyOptions(
        *(int *)((char *)this + 0x1289c),
        *(bool *)((char *)this + 0x128a0),
        *(int *)((char *)this + 0x128a4),
        *(bool *)((char *)this + 0x128a8));
    return SRESULT(true);
}

struct G2CORE4CM7
{
    virtual SRESULT Command(int value);
};

SRESULT G2CORE4CM7::Command(int value)
{
    int unused = 0;
    *(int *)((char *)this + 0x12890) = *(int *)((char *)g_pApp->m_pOptions + 0x5278);
    *(int *)((char *)this + 0x12898) = *(int *)((char *)g_pApp->m_pOptions + 0x527c);
    if (*(int *)((char *)this + 0x1288c) > *(int *)((char *)this + 0x12890))
        *(int *)((char *)this + 0x1288c) = *(int *)((char *)this + 0x12890);
    if (*(int *)((char *)this + 0x12894) > *(int *)((char *)this + 0x12898))
        *(int *)((char *)this + 0x12894) = *(int *)((char *)this + 0x12898);
    return SRESULT(true);
}

// The game's copy protection, which adds its own virtuals after the base's.
struct G2CORE4DRM
{
    virtual void W00(); virtual void W01(); virtual void W02(); virtual void W03(); virtual void W04(); virtual void W05(); virtual void W06(); virtual void W07();
    virtual void W08(); virtual void W09(); virtual void W10(); virtual void W11(); virtual void W12(); virtual void W13(); virtual void W14(); virtual void W15();
    virtual void W16(); virtual void W17(); virtual void W18(); virtual void W19(); virtual void W20(); virtual void W21(); virtual void W22(); virtual void W23();
    virtual void W24(); virtual void W25(); virtual void W26(); virtual void W27(); virtual void W28();
    virtual void Virtual29();
};

struct G2CORE4CM8
{
    virtual SRESULT Command(int value);
};

SRESULT G2CORE4CM8::Command(int value)
{
    int unused = 0;
    ((G2CORE4DRM *)g_pDrm)->Virtual29();
    return SRESULT(true);
}

struct G2CORE4CM10
{
    virtual SRESULT Command(int value);
};

SRESULT G2CORE4CM10::Command(int value)
{
    int unused = 0;
    g_pScene->Remove(*(RESHANDLE *)((char *)this + 0x1dcdc));
    return SRESULT(true);
}

struct G2CORE4CM11
{
    virtual SRESULT Command(int value);
};

SRESULT G2CORE4CM11::Command(int value)
{
    int unused = 0;
    WTACTOR *actor = g_pScene->GetActor(*(RESHANDLE *)((char *)this + 0x1dcdc));
    if (actor)
        ((ACTOR *)actor)->Step();
    return SRESULT(true);
}

struct G2CORE4CM12
{
    virtual SRESULT Command(int value);
};

SRESULT G2CORE4CM12::Command(int value)
{
    int unused = 0;
    bool ok = false;
    ok = g_pDrm->Virtual25();
    if (!ok)
        return SRESULT(false);
    return SRESULT(*(bool *)((char *)this + 0x1dcf0));
}

struct G2CORE4DRM2
{
    virtual void W00(); virtual void W01(); virtual void W02(); virtual void W03(); virtual void W04(); virtual void W05(); virtual void W06(); virtual void W07();
    virtual void W08(); virtual void W09(); virtual void W10(); virtual void W11(); virtual void W12(); virtual void W13(); virtual void W14(); virtual void W15();
    virtual void W16(); virtual void W17(); virtual void W18(); virtual void W19(); virtual void W20(); virtual void W21(); virtual void W22(); virtual void W23();
    virtual void W24(); virtual void W25(); virtual void W26(); virtual void W27(); virtual void W28(); virtual void W29();
    virtual void Virtual30();
};

struct G2CORE4CM13
{
    virtual SRESULT Command(int value);
};

SRESULT G2CORE4CM13::Command(int value)
{
    int unused = 0;
    g_pApp->SetFullscreen(false);
    g_pDisplay->Virtual22();
    ((G2CORE4DRM2 *)g_pDrm)->Virtual30();
    return SRESULT(true);
}

struct G2CORE4OPTS
{
    virtual void W0(); virtual void W1(); virtual void W2(); virtual void W3(); virtual void W4(); virtual void W5(); virtual void W6(); virtual void W7();
    virtual void W8();
    virtual void Virtual09();
};

struct G2CORE4CM14
{
    virtual SRESULT Command(int value);
};

SRESULT G2CORE4CM14::Command(int value)
{
    int unused = 0;
    ((G2CORE4OPTS *)g_pApp->m_pOptions)->Virtual09();
    g_pApp->m_pOptions->Save();
    return SRESULT(true);
}

// The tips: the core object steps through a scenario's tip names.
struct G2CORE4CM15
{
    virtual void W00(); virtual void W01(); virtual void W02(); virtual void W03(); virtual void W04(); virtual void W05(); virtual void W06(); virtual void W07();
    virtual void W08(); virtual void W09(); virtual void W10(); virtual void W11(); virtual void W12(); virtual void W13(); virtual void W14(); virtual void W15();
    virtual void W16(); virtual void W17(); virtual void W18(); virtual void W19(); virtual void W20(); virtual void W21(); virtual void W22(); virtual void W23();
    virtual void W24(); virtual void W25(); virtual void W26(); virtual void W27(); virtual void W28(); virtual void W29(); virtual void W30(); virtual void W31();
    virtual void W32(); virtual void W33(); virtual void W34(); virtual void W35(); virtual void W36(); virtual void W37(); virtual void W38(); virtual void W39();
    virtual void W40(); virtual void W41(); virtual void W42(); virtual void W43(); virtual void W44(); virtual void W45(); virtual void W46(); virtual void W47();
    virtual void W48();
    virtual void SetTipText(FIXEDWSTRING<0x200> &text);
    virtual void SetTipName(FIXEDWSTRING<0x200> &text);
    virtual void SetTipDescription(FIXEDWSTRING<0x200> &text);
    virtual SRESULT NextTipCommand(int value);
    virtual SRESULT PreviousTipCommand(int value);
    virtual SRESULT ShowTipCommand(int value);
    wchar_t *GetTipText(wchar_t *text, int index, int year);
    void NextTip();
};

SRESULT G2CORE4CM15::NextTipCommand(int value)
{
    int unused = 0;
    if (!g_pGame->GetScenario())
        return SRESULT(false);
    *(int *)((char *)this + 0x1dce8) = *(int *)((char *)this + 0x1dce8) + 1;
    if (*(int *)((char *)this + 0x1dce8) >= *(int *)((char *)this + 0x1dcec))
        *(int *)((char *)this + 0x1dce8) = 0;
    NextTip();
    return SRESULT(true);
}

SRESULT G2CORE4CM15::PreviousTipCommand(int value)
{
    int unused = 0;
    if (!g_pGame->GetScenario())
        return SRESULT(false);
    if (--*(int *)((char *)this + 0x1dce8) < 0)
        *(int *)((char *)this + 0x1dce8) = *(int *)((char *)this + 0x1dcec) - 1;
    NextTip();
    return SRESULT(true);
}

extern void *g_pSoundX;
extern void *g_pMusicX;

struct SOUNDX
{
    virtual void W00();
    virtual void W01();
    virtual void W02();
    virtual void W03();
    virtual void W04();
    virtual void W05();
    virtual void W06();
    virtual void W07();
    virtual void W08();
    virtual void W09();
    virtual void W10();
    virtual void W11();
    virtual void W12();
    virtual void W13();
    virtual void W14();
    virtual void W15();
    virtual void W16();
    virtual void W17();
    virtual void W18();
    virtual void W19();
    virtual void W20();
    virtual void W21();
    virtual void W22();
    virtual void W23();
    virtual void W24();
    virtual void W25();
    virtual void W26();
    virtual void W27();
    virtual void W28();
    virtual void W29();
    virtual void W30();
    virtual void W31();
    virtual void W32();
    virtual void W33();
    virtual void W34();
    virtual void W35();
    virtual void SetTime(float time);
};

struct MUSICX
{
    void Update();
};

__declspec(noinline) void G2CORE4APP::SetOption68(int value)
{
    if (*(int *)((char *)g_pApp->m_pOptions + 0x5268) != value) {
        *(int *)((char *)g_pApp->m_pOptions + 0x5268) = value;
        ((SOUNDX *)g_pSoundX)->SetTime(*(float *)((char *)g_pTimer + 0x14));
        if (g_pMusicX)
            ((MUSICX *)g_pMusicX)->Update();
    }
}

__declspec(noinline) void G2CORE4APP::SetOption74(bool value)
{
    *(bool *)((char *)g_pApp->m_pOptions + 0x5274) = value;
}

// The engine's saved setting table, and the hardware capability table it
// points at. Only the calls the core object makes are declared here.
class SETTINGSTABLE
{
public:
    void SetInt(int index, int value);
    int GetDetailLevel();

    char m_000[0xff0];
    void *m_ff0;
};

class HARDWARE
{
public:
    bool IsSupported(int feature);
};

class G2CORE4D3DAPP
{
public:
    void SetResolution(int index);
};

void G2CORE4RES::ApplyOptions(int &a, bool &b, int &c, bool &d)
{
    switch (((SETTINGSTABLE *)*(void **)((char *)g_pDisplay + 0x2c0))->GetDetailLevel()) {
    case 4:
        a = 2;
        b = true;
        c = 0;
        d = true;
        return;
    case 3:
        a = 2;
        b = false;
        c = 0;
        d = true;
        return;
    case 2:
        a = 1;
        b = false;
        c = 0;
        d = false;
        return;
    case 0:
    case 1:
        a = 0;
        b = false;
        c = 0;
        d = false;
        break;
    }
}

__declspec(noinline) void G2CORE4APP::SetOption6C(bool value)
{
    ((SETTINGSTABLE *)*(void **)((char *)g_pDisplay + 0x2c0))->SetInt(0xb, value);
    ((SETTINGSTABLE *)*(void **)((char *)g_pDisplay + 0x2c0))->SetInt(0x20, value);
    ((SETTINGSTABLE *)*(void **)((char *)g_pDisplay + 0x2c0))->SetInt(0xf, value);
    ((SETTINGSTABLE *)*(void **)((char *)g_pDisplay + 0x2c0))->SetInt(0x15, value);
    ((SETTINGSTABLE *)*(void **)((char *)g_pDisplay + 0x2c0))->SetInt(0xc, value);
    if (!((HARDWARE *)((SETTINGSTABLE *)*(void **)((char *)g_pDisplay + 0x2c0))->m_ff0)->IsSupported(0)) {
        ((SETTINGSTABLE *)*(void **)((char *)g_pDisplay + 0x2c0))->SetInt(0x20, 0);
        ((SETTINGSTABLE *)*(void **)((char *)g_pDisplay + 0x2c0))->SetInt(0x20, 0);
    }
    if (!((HARDWARE *)((SETTINGSTABLE *)*(void **)((char *)g_pDisplay + 0x2c0))->m_ff0)->IsSupported(0x13)) {
        ((SETTINGSTABLE *)*(void **)((char *)g_pDisplay + 0x2c0))->SetInt(0x15, 0);
        ((SETTINGSTABLE *)*(void **)((char *)g_pDisplay + 0x2c0))->SetInt(0xc, 0);
    }
    *(bool *)((char *)g_pApp->m_pOptions + 0x526c) = value;
}

__declspec(noinline) void G2CORE4APP::SetOption70(int value)
{
    switch (value) {
    case 0:
        ((SETTINGSTABLE *)*(void **)((char *)g_pDisplay + 0x2c0))->SetInt(9, 0);
        ((G2CORE4D3DAPP *)((char *)g_pDisplay + 0x44))->SetResolution(0);
        break;
    case 1:
        ((SETTINGSTABLE *)*(void **)((char *)g_pDisplay + 0x2c0))->SetInt(9, 1);
        ((G2CORE4D3DAPP *)((char *)g_pDisplay + 0x44))->SetResolution(2);
        break;
    case 2:
        ((SETTINGSTABLE *)*(void **)((char *)g_pDisplay + 0x2c0))->SetInt(9, 1);
        ((G2CORE4D3DAPP *)((char *)g_pDisplay + 0x44))->SetResolution(4);
        break;
    }
    *(int *)((char *)g_pApp->m_pOptions + 0x5270) = value;
}

// The managers and wide text the tips use.
extern RESOURCEMANAGER *g_pStormChaserResources;
extern RESOURCEMANAGER *g_pDestructibleResources;
extern RESOURCEMANAGER *g_pACTORRESOURCES;
extern wchar_t g_tipTextTable[][50];

static __forceinline char *G2CORE4TipName(WTACTOR *scenario, int index)
{
    char **names = (char **)((char *)scenario + 0x332c);
    char *&name = names[index];
    return name ? name : "";
}

static __forceinline char *G2CORE4TipString(char *text)
{
    return text ? text : "";
}

static __forceinline wchar_t *G2CORE4TipWide(wchar_t *text)
{
    return text ? text : L"";
}

wchar_t *G2CORE4CM15::GetTipText(wchar_t *text, int index, int year)
{
    wchar_t buffer[0x200];
    buffer[0] = 0;
    buffer[0x1ff] = 0;
    if (year == 0) {
        if (g_tipTextTable[index] == 0)
            buffer[0] = 0;
        else {
            wcsncpy(buffer, g_tipTextTable[index], 0x1ff);
            buffer[0x1ff] = 0;
        }
    } else {
        SafeSwprintf(buffer, L"%s - Year %d",
                     &g_tipTextTable[index][0], year);
    }
    memcpy(text, buffer, sizeof(buffer));
    return text;
}

__declspec(noinline) void G2CORE4CM15::NextTip()
{
    float scale;
    WTACTOR *scenario = g_pGame->GetScenario();
    if (!scenario)
        return;
    wchar_t textStorage[0x200];
    RESOURCEMASTER *resources;
    RESOURCEMASTER *storm = g_pStormChaserResources->GetMaster(
        G2CORE4TipName(scenario, *(int *)((char *)this + 0x1dce8)));
    if (!storm) {
        if (g_pLog)
            g_pLog->Error("start map - could not find storm chaser master %s",
                G2CORE4TipName(scenario, *(int *)((char *)this + 0x1dce8)));
        return;
    }
    RESOURCEMASTER *destructible = g_pDestructibleResources->GetMaster(
        G2CORE4TipString(*(char **)((char *)storm + 0x60)));
    if (!destructible) {
        if (g_pLog)
            g_pLog->Error("start map - could not find destructible master %s",
                G2CORE4TipString(*(char **)((char *)storm + 0x60)));
        return;
    }
    resources = g_pACTORRESOURCES->GetMaster(
        G2CORE4TipString(*(char **)((char *)destructible + 0xfc)));
    if (!resources) {
        if (g_pLog)
            g_pLog->Error("start map - could not find actor resources master %s",
                G2CORE4TipString(*(char **)((char *)destructible + 0xfc)));
        return;
    }
    FIXEDWSTRING<0x200> *text = (FIXEDWSTRING<0x200> *)textStorage;
    *text = (const char *)G2CORE4TipWide(*(wchar_t **)((char *)destructible + 0xb4));
    SetTipName(*text);
    *text = (const char *)G2CORE4TipWide(*(wchar_t **)((char *)storm + 0x64));
    SetTipDescription(*text);
    ((ACTOR *)g_pCamera)->SetAngles(ANG(0.0f, 0.0f, 0.0f));
    ACTOR *actor = (ACTOR *)g_pScene->GetActor(*(RESHANDLE *)((char *)this + 0x1dcdc));
    if (!actor) {
        actor = g_pScene->CreateActor(ACTOR::s_factoryId);
        if (!actor) {
            if (g_pLog)
                g_pLog->Error("start map - could not create actor!");
            return;
        }
        V3 position(0.0f, 0.0f, 0.0f);
        position = ((CAMERA *)g_pCamera)->ScreenToWorld(V2(0.68f, 0.65f), 2.0f);
        actor->SetPosition(position);
        actor->SetAngles(ANG(0.0f, -30.0f, 0.0f));
        actor->SetBehavior(0);
        actor->SetAngVel(V3(90.0f, 0.0f, 0.0f));
        actor->SetZSort(100);
        actor->SetActive(true);
        *(RESHANDLE *)((char *)this + 0x1dcdc) = actor->m_handle;
    }
    scale = *(float *)((char *)storm + 0x68) * 0.035f;
    actor->SetScale(V3(scale, scale, scale));
    actor->ApplyResourcesTemplate((ACTORRESOURCESMASTER *)resources);
}

SRESULT G2CORE4CM15::ShowTipCommand(int value)
{
    int unused = 0;
    wchar_t text[0x200];
    wchar_t *source = GetTipText(text, *(int *)((char *)g_pGame + 0x23c),
        *(unsigned char *)((char *)g_pGame + 0x230) ? 0 : *(int *)((char *)g_pGame + 0x240));
    FIXEDWSTRING<0x200> tip(source);
    SetTipText(tip);
    *(int *)((char *)this + 0x1dce8) = 0;
    NextTip();
    WTACTOR *scenario = g_pGame->GetScenario();
    if (!scenario)
        return SRESULT(false);
    *(int *)((char *)this + 0x1dcec) = 16;
    for (int i = 0; i < 16; i++) {
        char *name = *(char **)((char *)scenario + 0x332c + i * 4);
        if (!name || (int)strlen(name) <= 0) {
            *(int *)((char *)this + 0x1dcec) = i;
            break;
        }
    }
    return SRESULT(true);
}

struct G2CORE4SW
{
    CSTRING &Set60(const CSTRING &value);
    CSTRING &Set98(const CSTRING &value);
    CSTRING &SetAC(const CSTRING &value);
    CSTRING &SetB0(const CSTRING &value);
    CSTRING &Set9C(const CSTRING &value);
    CSTRING &Set6C(const CSTRING &value);
    CSTRING &Set74(const CSTRING &value);
    CSTRING &Set7C(const CSTRING &value);
    CSTRING &Set84(const CSTRING &value);
    CSTRING &Set8C(const CSTRING &value);
};

CSTRING &G2CORE4SW::Set60(const CSTRING &value)
{
    return *(CSTRING *)((char *)this + 0x60) = value;
}

CSTRING &G2CORE4SW::Set98(const CSTRING &value)
{
    return *(CSTRING *)((char *)this + 0x98) = value;
}

CSTRING &G2CORE4SW::SetAC(const CSTRING &value)
{
    return *(CSTRING *)((char *)this + 0xac) = value;
}

CSTRING &G2CORE4SW::SetB0(const CSTRING &value)
{
    return *(CSTRING *)((char *)this + 0xb0) = value;
}

CSTRING &G2CORE4SW::Set9C(const CSTRING &value)
{
    return *(CSTRING *)((char *)this + 0x9c) = value;
}

CSTRING &G2CORE4SW::Set6C(const CSTRING &value)
{
    return *(CSTRING *)((char *)this + 0x6c) = value;
}

CSTRING &G2CORE4SW::Set74(const CSTRING &value)
{
    return *(CSTRING *)((char *)this + 0x74) = value;
}

CSTRING &G2CORE4SW::Set7C(const CSTRING &value)
{
    return *(CSTRING *)((char *)this + 0x7c) = value;
}

CSTRING &G2CORE4SW::Set84(const CSTRING &value)
{
    return *(CSTRING *)((char *)this + 0x84) = value;
}

CSTRING &G2CORE4SW::Set8C(const CSTRING &value)
{
    return *(CSTRING *)((char *)this + 0x8c) = value;
}

int DRM::GetClassId()
{
    return s_classId;
}

int DRM::GetParentClassId()
{
    return g_classParents[s_classId];
}

const char *DRM::GetClassName()
{
    return g_classNames[s_classId];
}

class G2CORE4FID1
{
public:
    virtual int GetFactoryId();

    static int s_factoryId;
};

int G2CORE4FID1::GetFactoryId()
{
    return s_factoryId;
}

class G2CORE4FID2
{
public:
    virtual int GetFactoryId();

    static int s_factoryId;
};

int G2CORE4FID2::GetFactoryId()
{
    return s_factoryId;
}

class G2CORE4FID3
{
public:
    virtual int GetFactoryId();

    static int s_factoryId;
};

int G2CORE4FID3::GetFactoryId()
{
    return s_factoryId;
}

class G2CORE4FID4
{
public:
    virtual int GetFactoryId();

    static int s_factoryId;
};

int G2CORE4FID4::GetFactoryId()
{
    return s_factoryId;
}

class G2CORE4FID5
{
public:
    virtual int GetFactoryId();

    static int s_factoryId;
};

int G2CORE4FID5::GetFactoryId()
{
    return s_factoryId;
}

class G2CORE4FID6
{
public:
    virtual int GetFactoryId();

    static int s_factoryId;
};

int G2CORE4FID6::GetFactoryId()
{
    return s_factoryId;
}

class G2CORE4FID7
{
public:
    virtual int GetFactoryId();

    static int s_factoryId;
};

int G2CORE4FID7::GetFactoryId()
{
    return s_factoryId;
}

class G2CORE4FID8
{
public:
    virtual int GetFactoryId();

    static int s_factoryId;
};

int G2CORE4FID8::GetFactoryId()
{
    return s_factoryId;
}

// The SCRIPTS manager (0x00442df0).
class SCRIPTSMANAGER : public RESOURCEMANAGER
{
public:
    SCRIPTSMANAGER()
    {
        SafeSprintf(m_fileName, "SCRIPTS.txt");
        m_8 = false;
        m_18 = true;
    }
};

RESOURCEMANAGER *CreateSCRIPTS()
{
    return new SCRIPTSMANAGER();
}

// The SIDEKICK manager (0x00443030).
class SIDEKICKMANAGER : public RESOURCEMANAGER
{
public:
    SIDEKICKMANAGER()
    {
        SafeSprintf(m_fileName, "SIDEKICK.txt");
        m_8 = false;
        m_18 = true;
    }
};

RESOURCEMANAGER *CreateSIDEKICK()
{
    return new SIDEKICKMANAGER();
}

// The STORMCHASER manager (0x00444050).
class STORMCHASERMANAGER : public RESOURCEMANAGER
{
public:
    STORMCHASERMANAGER()
    {
        SafeSprintf(m_fileName, "STORMCHASER.txt");
        m_8 = false;
        m_18 = true;
    }
};

RESOURCEMANAGER *CreateSTORMCHASER()
{
    return new STORMCHASERMANAGER();
}

// The TOOLTIPS manager (0x00444540).
class TOOLTIPSMANAGER : public RESOURCEMANAGER
{
public:
    TOOLTIPSMANAGER()
    {
        SafeSprintf(m_fileName, "TOOLTIPS.txt");
        m_8 = false;
        m_18 = true;
    }
};

RESOURCEMANAGER *CreateTOOLTIPS()
{
    return new TOOLTIPSMANAGER();
}

// The TORNADO manager (0x00444810).
class TORNADOMANAGER : public RESOURCEMANAGER
{
public:
    TORNADOMANAGER()
    {
        SafeSprintf(m_fileName, "TORNADO.txt");
        m_8 = false;
        m_18 = true;
    }
};

RESOURCEMANAGER *CreateTORNADO()
{
    return new TORNADOMANAGER();
}

// The PROPSHADOW manager (0x00441440).
class PROPSHADOWMANAGER : public RESOURCEMANAGER
{
public:
    PROPSHADOWMANAGER()
    {
        SafeSprintf(m_fileName, "PROPSHADOW.txt");
        m_8 = false;
        m_18 = true;
    }
};

RESOURCEMANAGER *CreatePROPSHADOW()
{
    return new PROPSHADOWMANAGER();
}

// The RADAR manager (0x00441d60).
class RADARMANAGER : public RESOURCEMANAGER
{
public:
    RADARMANAGER()
    {
        SafeSprintf(m_fileName, "RADAR.txt");
        m_8 = false;
        m_18 = true;
    }
};

RESOURCEMANAGER *CreateRADAR()
{
    return new RADARMANAGER();
}
