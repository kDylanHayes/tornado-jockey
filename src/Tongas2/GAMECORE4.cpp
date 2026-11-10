#define OBJECT_GETPARENTACTOR
#include <string.h>

// Tongas2/GAMECORE4 -- the game-code block at 0x4410b0. The string, vector
// and tree members the original translation unit emitted out of line, and the
// small resource accessors that live in the same block.
#define GC4_OUTOFLINE
#define MATRIX_AXIS_METHODS
#define ANG_UPDATE_OUTOFLINE
#define V2_ROTATE_OUTOFLINE

#include <math.h>
#include <map>
#include <string>
#include <vector>

#include "Tongas2/V2.h"

#include "Tongas2/ACTOR.h"
#include "Tongas2/ACTORLINK.h"
#include "Tongas2/APP.h"
#include "Tongas2/APPWINDOW.h"
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
    virtual ~V44AC00();
};

V44AC00::~V44AC00()
{
}

class GD440960
{
public:
    virtual ~GD440960();
};

__declspec(noinline) GD440960::~GD440960()
{
}

class GD4413d0
{
public:
    virtual ~GD4413d0();
};

__declspec(noinline) GD4413d0::~GD4413d0()
{
}

class GD441630
{
public:
    virtual ~GD441630();
};

__declspec(noinline) GD441630::~GD441630()
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

__declspec(noinline) GD4433c0::~GD4433c0()
{
}

class GD4434d0
{
public:
    virtual ~GD4434d0();
};

__declspec(noinline) GD4434d0::~GD4434d0()
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

__declspec(noinline) GD444450::~GD444450()
{
}

class GD444730
{
public:
    virtual ~GD444730();
};

__declspec(noinline) GD444730::~GD444730()
{
}

class GD444e40
{
public:
    virtual ~GD444e40();
};

__declspec(noinline) GD444e40::~GD444e40()
{
}

class GD4451f0
{
public:
    virtual ~GD4451f0();
};

__declspec(noinline) GD4451f0::~GD4451f0()
{
}

class GD4477d0
{
public:
    virtual ~GD4477d0();
};

__declspec(noinline) GD4477d0::~GD4477d0()
{
}

class GD447e50
{
public:
    virtual ~GD447e50();
};

__declspec(noinline) GD447e50::~GD447e50()
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

class GD4484c0
{
public:
    virtual ~GD4484c0();
};

__declspec(noinline) GD4484c0::~GD4484c0()
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

class GD44a560
{
public:
    virtual ~GD44a560();
};

__declspec(noinline) GD44a560::~GD44a560()
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

class GD450e80
{
public:
    virtual ~GD450e80();
};

__declspec(noinline) GD450e80::~GD450e80()
{
}

class GD4512e0
{
public:
    virtual ~GD4512e0();
};

__declspec(noinline) GD4512e0::~GD4512e0()
{
}

class GD4516f0
{
public:
    virtual ~GD4516f0();
};

__declspec(noinline) GD4516f0::~GD4516f0()
{
}

class GD451880
{
public:
    virtual ~GD451880();
};

__declspec(noinline) GD451880::~GD451880()
{
}

class GD452a90
{
public:
    virtual ~GD452a90();
};

__declspec(noinline) GD452a90::~GD452a90()
{
}

class GD453360
{
public:
    virtual ~GD453360();
};

__declspec(noinline) GD453360::~GD453360()
{
}

class GD453c50
{
public:
    virtual ~GD453c50();
};

__declspec(noinline) GD453c50::~GD453c50()
{
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
    virtual ~APPOPTIONS();
    void Save();
};

APPOPTIONS::~APPOPTIONS()
{
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
    virtual SRESULT NextTipCommand(int value);
    virtual SRESULT PreviousTipCommand(int value);
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
