// Tongas2/GAMECORE2 -- the game-code block at 0x4162f0. These are the ANG,
// V3, FIXEDSTRING and SPAWNER members the original translation unit emitted
// out of line.
#define ANG_EULER_OUTOFLINE
#define V3_ISZERO_OUTOFLINE
#define SPAWNER_SETTERS_OUTOFLINE
#define V2_ROTATE_OUTOFLINE
#define SCENE_FINDACTORS
#define V3_DISTANCEXZ_OUTOFLINE
#define ANG_SETEULERX_OUTOFLINE
#define MATRIX_TRANSPOSE_OUTOFLINE


#include <new>
#include <stdlib.h>
#include <wchar.h>

#include "Tongas2/FIXEDSTRING.h"
#include "Tongas2/MATHS.h"
#include "Tongas2/GAME.h"
#include "Tongas2/RENDERER.h"
#include "Tongas2/TIMER.h"
#include "Tongas2/SPAWNER.h"
#include "Tongas2/RESOURCE.h"
#include "Tongas2/GUI.h"
#include "Tongas2/GUICONTROL.h"
#include "Tongas2/GUIGROUP.h"
#include "Tongas2/INPUT.h"
#include "Tongas2/RESOURCES.h"
#include "Tongas2/CAMERA.h"
#include "Tongas2/SCENE.h"
#include "Tongas2/SRESULT.h"

float ANG::GetEulerY()
{
    if (flags & EULER_DIRTY) {
        matrix.ToEuler(&euler.z, &euler.y, &euler.x);
        flags &= ~EULER_DIRTY;
    }
    return euler.y;
}

float ANG::GetEulerZ()
{
    if (flags & EULER_DIRTY) {
        matrix.ToEuler(&euler.z, &euler.y, &euler.x);
        flags &= ~EULER_DIRTY;
    }
    return euler.z;
}

void ANG::GetEuler(float *x, float *y, float *z)
{
    if (flags & EULER_DIRTY) {
        matrix.ToEuler(&euler.z, &euler.y, &euler.x);
        flags &= ~EULER_DIRTY;
    }
    *x = euler.x;
    *y = euler.y;
    *z = euler.z;
}

bool V3::IsZero() const
{
    return x == 0.0f && y == 0.0f && z == 0.0f;
}

bool operator==(const V3 &a, const V3 &b)
{
    return a.x == b.x && a.y == b.y && a.z == b.z;
}

V3 operator*(const V3 &v, const MATRIX &m)
{
    return V3(v.z * m._31 + v.x * m._11 + v.y * m._21 + m._41,
              v.z * m._32 + v.x * m._12 + v.y * m._22 + m._42,
              v.z * m._33 + v.x * m._13 + v.y * m._23 + m._43);
}

template <int N>
void FIXEDSTRING<N>::operator+=(const char *text)
{
    if (strlen(text) + strlen(m_buf) < N - 1)
        strcpy(m_buf + strlen(m_buf), text);
}

template void FIXEDSTRING<0x200>::operator+=(const char *);

void SPAWNER::SetMaxSpawnTime(const float &time)
{
    m_maxSpawnTime = time;
}

void SPAWNER::SetMaxSpawns(const int &max)
{
    m_maxSpawns = max;
}

// Anonymous game-code helpers recovered from the 0x4162f0 block. The
// offsets are what the target accesses; the names are placeholders.
template <int N, int SIZE>
struct G2PADC
{
    char pad[N];
    unsigned char data[SIZE];

    void Set(const unsigned char (&value)[SIZE])
    {
        memcpy(data, value, SIZE);
    }
};

template struct G2PADC<0xccc, 0x200>;
template void G2PADC<0xccc, 0x200>::Set(const unsigned char (&)[0x200]);
template struct G2PADC<0xd10, 0x400>;
template void G2PADC<0xd10, 0x400>::Set(const unsigned char (&)[0x400]);
template struct G2PADC<0xf8a, 0x400>;
template void G2PADC<0xf8a, 0x400>::Set(const unsigned char (&)[0x400]);
template struct G2PADC<0x138a, 0x400>;
template void G2PADC<0x138a, 0x400>::Set(const unsigned char (&)[0x400]);
template struct G2PADC<0x178a, 0x400>;
template void G2PADC<0x178a, 0x400>::Set(const unsigned char (&)[0x400]);
template struct G2PADC<0xdac, 0x200>;
template void G2PADC<0xdac, 0x200>::Set(const unsigned char (&)[0x200]);
template struct G2PADC<0xf48, 0x200>;
template void G2PADC<0xf48, 0x200>::Set(const unsigned char (&)[0x200]);

struct G2CSTRING
{
    char *m_str;
    int Length() const;
};

int G2CSTRING::Length() const
{
    if (!m_str)
        return 0;
    return (int)strlen(m_str);
}

struct G2V2F
{
    float x, y;
    G2V2F(const float &x_, const float &y_) : x(x_), y(y_) {}
};

class G2ACTOR : public ACTOR
{
public:
    G2ACTOR();
    virtual ~G2ACTOR();
    virtual void Virtual140();
    virtual void Update();

    V3 m_ccc;
    G2V2F m_cd8;
    G2V2F m_ce0;
    G2V2F m_ce8;
    V3 m_cf0;
    bool m_cfc;
    bool m_cfd;
    char m_cfe[2];
    V3I m_d00;
};

G2ACTOR::G2ACTOR()
    : m_cd8(0.0f, 0.0f),
      m_ce0(0.0f, 0.0f),
      m_ce8(0.0f, 0.0f),
      m_d00(-1, -1, 0)
{
    m_d00 = V3I(-1, -1, 0);
    m_cd8 = G2V2F(1.0f, 3.0f);
    m_ce0 = G2V2F(1.0f, 1.0f);
    m_ce8 = G2V2F(2.0f, 1.0f);
    m_ccc = V3(0.0f, 0.0f, 0.0f);
    m_cf0 = V3(0.0f, 0.0f, 0.0f);
    m_cfc = false;
    m_cfd = false;
}

G2ACTOR::~G2ACTOR()
{
}

void G2ACTOR::Update()
{
    Virtual140();
    ACTOR::Update();
}
struct G2V2
{
    float x, y;
};

class G2FLAGS
{
public:
    char m_0000[0x28];
    float m_28;
    char m_2c[0x30 - 0x2c];
    float m_30;
    char m_34[0x998 - 0x34];
    unsigned char m_998;
    char m_999[0xed8 - 0x999];
    G2V2 m_ed8;

    void SetPosition2D();
};

void G2FLAGS::SetPosition2D()
{
    m_998 = 1;
    G2V2 value;
    value.x = m_28;
    value.y = m_30;
    m_ed8 = value;
}

class G2PAIR
{
public:
    char m_0000[0x112b];
    unsigned char m_112b;
    char m_112c[0x1388 - 0x112c];
    int m_1388;
    int m_138c;

    void Set(int first, int second);
};

void G2PAIR::Set(int first, int second)
{
    if (!m_112b) {
        m_1388 = first;
        m_138c = second;
    }
}
bool ACTOR::Virtual122(CMDCALL *call)
{
    (void)call;
    return true;
}
struct G2ADDER
{
    char m_0000[0xf34];
    float m_f34;
    float m_f38;

    void Add(float first, float second);
};

void G2ADDER::Add(float first, float second)
{
    m_f34 += first;
    m_f38 += second;
}

struct G2FOG
{
    char m_0000[0x114d];
    bool m_114d;

    void UpdateFog(CMDCALL *call);
};

void G2FOG::UpdateFog(CMDCALL *call)
{
    (void)call;
    if (m_114d)
        g_pRenderer->ApplyFog();
}

class G2MID : public ACTOR
{
public:
    virtual void Start();
};

__declspec(noinline) void G2MID::Start()
{
    ACTOR::Start();
}

class G2START : public G2MID
{
public:
    virtual void Start();

    char m_0000[0x1150 - 0xccc];
    int m_1150;
};

void G2START::Start()
{
    G2MID::Start();
    m_1150 = 0;
}
class BONEDMODELDX8PARTICLE
{
public:
    void Destroy();
};

class G2D1 : public ACTOR
{
public:
    virtual ~G2D1();

    void ClearParticle();

    V3 m_ccc;
    V3 m_cd8;
    BONEDMODELDX8PARTICLE *m_ce4;
};

G2D1::~G2D1()
{
}

void G2D1::ClearParticle()
{
    BONEDMODELDX8PARTICLE *particle = m_ce4;
    if (particle) {
        particle->Destroy();
        free(particle);
        m_ce4 = 0;
    }
    g_pRenderer->m_enabled = true;
}

class G2D2 : public ACTOR
{
public:
    virtual ~G2D2();
};

G2D2::~G2D2()
{
}

struct ZEROV2 : V2
{
    ZEROV2() : V2(0.0f, 0.0f) {}
    ZEROV2 &operator=(const V2 &v)
    {
        V2::operator=(v);
        return *this;
    }
};

template <int N>
struct WIDESTRING
{
    WIDESTRING()
    {
        m_buf[0] = 0;
        m_buf[N - 1] = 0;
    }
    void Set(const wchar_t *text)
    {
        wcsncpy(m_buf, text, N - 1);
        m_buf[N - 1] = 0;
    }
    wchar_t m_buf[N];
};

extern int g_destructibleFactoryId;

class G2DESTRUCTIBLE
{
public:
    static int s_classId;
    static CMDENTRY s_commands[];
    static void RegisterCommands();
};

class G2D5 : public ACTOR
{
public:
    G2D5();
    virtual ~G2D5();

    virtual int GetClassId() { return G2DESTRUCTIBLE::s_classId; }
    virtual int GetParentClassId() { return g_classParents[G2DESTRUCTIBLE::s_classId]; }
    virtual const char *GetClassName() { return g_classNames[G2DESTRUCTIBLE::s_classId]; }
    virtual int GetFactoryId() { return g_destructibleFactoryId; }
    virtual int GetFactoryFlags() { return g_factoryFlags[g_destructibleFactoryId]; }

    void SetTargetPoint(V3 value);
    void SetSize(const V2 &value);
    void SetFlag0(const bool &value);
    void SetRate(const int &value);
    void SetFlag1(const bool &value);
    void SetFlag2(const bool &value);
    void SetFlag3(const bool &value);
    void SetFlag4(const bool &value);
    void SetFlag5(const bool &value);
    void SetSpeed(const float &value);
    void SetAcceleration(const float &value);
    void SetFlag6(const bool &value);
    void SetFlag7(const bool &value);
    void SetFriction(const float &value);
    void SetFlag8(const bool &value);
    void SetFlag9(const bool &value);
    void SetColor(const V3I &value);
    void SetOpacity(const float &value);

    int m_ccc;
    bool m_cd0;
    bool m_cd1;
    ZEROV3 m_cd4;
    int m_ce0;
    int m_ce4;
    bool m_ce8;
    int m_cec;
    bool m_cf0;
    bool m_cf1;
    bool m_cf2;
    ZEROV2 m_cf4;
    bool m_cfc;
    int m_d00;
    bool m_d04;
    int m_d08;
    int m_d0c;
    WIDESTRING<0x200> m_d10;
    ZEROV2 m_1110;
    int m_1118;
    int m_111c;
    bool m_1120;
    int m_1124;
    bool m_1128;
    bool m_1129;
    bool m_112a;
    bool m_112b;
    bool m_112c;
    float m_1130;
    int m_1134;
    bool m_1138;
    CSTRING m_113c;
    int m_1140;
    bool m_1144;
    float m_1148;
    bool m_114c;
    bool m_114d;
    V3I m_1150;
    float m_115c;
    FIXEDSTRING<0x200> m_1160;
    bool m_1360;
    RESHANDLE m_1364;
    bool m_1370;
    bool m_1371;
    int m_1374;
    RESHANDLE m_1378;
    int m_1384;
    int m_1388;
    int m_138c;
    RESHANDLE m_1390;
    bool m_139c;
};

G2D5::G2D5()
{
    m_ccc = 0;
    m_cd0 = false;
    m_cd1 = false;
    m_cd4 = ZEROV3();
    m_ce0 = 0;
    m_ce4 = 0;
    m_ce8 = false;
    m_cec = 0;
    m_cf0 = true;
    m_cf1 = false;
    m_cf2 = false;
    m_cf4 = ZEROV2();
    m_cfc = false;
    m_d00 = 0;
    m_d04 = false;
    m_d08 = 0;
    m_d0c = 0;
    m_1370 = false;
    m_1374 = 0;
    m_1384 = -1;
    m_1388 = 0;
    m_138c = 0;
    m_1371 = false;
    m_d10.Set(L"");
    m_1110 = V2(0.0f, 6.0f);
    m_1118 = 0;
    m_111c = 0;
    m_1120 = false;
    m_1124 = 0;
    m_1128 = false;
    m_1129 = false;
    m_112a = false;
    m_112b = false;
    m_112c = true;
    m_1130 = 1.0f;
    m_1134 = 0;
    m_1138 = false;
    m_113c = "";
    m_1140 = -1;
    m_1144 = false;
    m_1148 = 1.5f;
    m_114c = false;
    m_114d = false;
    m_1150 = V3I(255, 64, 64);
    m_115c = 1.0f;
    m_1160.Set("");
    m_1360 = false;
    m_139c = false;
}

G2D5::~G2D5()
{
}

void G2D5::SetTargetPoint(V3 value)
{
    *(V3 *)((char *)this + 0x1364) = value;
}

void G2D5::SetSize(const V2 &value)
{
    *(V2 *)((char *)this + 0x1110) = value;
}

void G2D5::SetFlag0(const bool &value)
{
    *(bool *)((char *)this + 0x1120) = value;
}

void G2D5::SetRate(const int &value)
{
    *(int *)((char *)this + 0x1124) = value;
}

void G2D5::SetFlag1(const bool &value)
{
    *(bool *)((char *)this + 0x1128) = value;
}

void G2D5::SetFlag2(const bool &value)
{
    *(bool *)((char *)this + 0x1129) = value;
}

void G2D5::SetFlag3(const bool &value)
{
    *(bool *)((char *)this + 0x112a) = value;
}

void G2D5::SetFlag4(const bool &value)
{
    *(bool *)((char *)this + 0x112b) = value;
}

void G2D5::SetFlag5(const bool &value)
{
    *(bool *)((char *)this + 0x112c) = value;
}

void G2D5::SetSpeed(const float &value)
{
    *(float *)((char *)this + 0x1130) = value;
}

void G2D5::SetAcceleration(const float &value)
{
    *(float *)((char *)this + 0x1134) = value;
}

void G2D5::SetFlag6(const bool &value)
{
    *(bool *)((char *)this + 0x1138) = value;
}

void G2D5::SetFlag7(const bool &value)
{
    *(bool *)((char *)this + 0x1144) = value;
}

void G2D5::SetFriction(const float &value)
{
    *(float *)((char *)this + 0x1148) = value;
}

void G2D5::SetFlag8(const bool &value)
{
    *(bool *)((char *)this + 0x114c) = value;
}

void G2D5::SetFlag9(const bool &value)
{
    *(bool *)((char *)this + 0x114d) = value;
}

void G2D5::SetColor(const V3I &value)
{
    *(V3I *)((char *)this + 0x1150) = value;
}

void G2D5::SetOpacity(const float &value)
{
    *(float *)((char *)this + 0x115c) = value;
}

struct G2TIMESET
{
    char m_0000[0xd84];
    bool m_d84;
    char m_d85[0xfbc - 0xd85];
    float m_fbc;

    void ResetSpeed();
};

void G2TIMESET::ResetSpeed()
{
    if (m_d84)
        g_pTimer->SetSpeed(1.0f);
    m_fbc = 1.0f;
}
extern RESOURCEMANAGER *g_pActorResourceManager;
extern RESOURCEMANAGER *g_pActorResourceManager2;
extern RESOURCEMANAGER *g_pActorResourceManager3;

// The mesh registry the actor submits one of its meshes to. Only the methods
// this unit calls are declared here.
class G2MIDITEM;

class G2MIDMESH
{
public:
    G2MIDITEM *GetItem();
};

class G2MIDRES
{
public:
    G2MIDMESH *GetMIDMesh(int index);
};

class G2MIDLIST
{
public:
    bool IsReady();
    void Add(G2MIDITEM *item, const char *name);
};

extern G2MIDLIST *g_pMIDList;

class G2MID2 : public ACTOR
{
public:
    virtual void Stop();
    void Helper();
    void FindResource();
    void SetResourcesActive(bool active);
    void AddActiveMesh();

    char m_0000[0x113c - 0xccc];
    CSTRING m_113c;
    char m_1140[0x1371 - 0x1140];
    bool m_1371;
    char m_1372[0x1384 - 0x1372];
    // The handle slot the active resource was found in, or -1.
    int m_1384;
    char m_1388[0x1390 - 0x1388];
    RESHANDLE m_handle;
    bool m_139c;
};

__declspec(noinline) void G2MID2::Stop()
{
    Helper();
    ACTOR::Stop();
}

struct G2INT3
{
    int x, y, z;
    G2INT3() {}
    G2INT3(int x_, int y_, int z_) : x(x_), y(y_), z(z_) {}
};

struct G2CONTACTARG;

class G2RULESOBJBASE : public ACTOR
{
public:
    G2RULESOBJBASE();
    virtual ~G2RULESOBJBASE();

    // The target bookkeeping virtuals the game objects add after ACTOR's.
    virtual void SetNameBlock(const char *data) {}
    virtual char *GetNameBlock() { return m_name.m_buf; }
    virtual void HandleTornado(CMDCALL *call);
    virtual void IncMainTarget(ACTOR *actor) {}
    virtual void DecMainTarget(ACTOR *actor) {}
    virtual void IncSecondaryTarget(ACTOR *actor) {}
    virtual void DecSecondaryTarget(ACTOR *actor) {}
    virtual void AddTarget(ACTOR *actor) {}
    virtual void ResetTarget(ACTOR *actor) {}
    virtual void SetContact(G2CONTACTARG *arg) {}
    virtual void OnTargetDestroyed(G2CONTACTARG *arg) {}
    virtual void AddEntry(CMDBASE *object) {}

    FIXEDSTRING<0x200> m_name;
    G2INT3 m_entries[10];
};

G2RULESOBJBASE::G2RULESOBJBASE()
{
    G2INT3 *p = m_entries;
    for (int i = 10; i != 0; i--) {
        p->x = -1;
        p->y = -1;
        p->z = 0;
        p++;
    }
    m_name.Set("HudNormal");
}

__declspec(noinline) G2RULESOBJBASE::~G2RULESOBJBASE()
{
    if (g_pGame)
        g_pGame->ResetRules();
}

class G2GAMEVT
{
public:
    virtual void V00();
    virtual void V01();
    virtual void V02();
    virtual void V03();
    virtual void V04();
    virtual void V05();
    virtual void V06();
    virtual void V07();
    virtual void V08();
    virtual void V09();
    virtual void V10();
    virtual void V11();
    virtual void V12();
    virtual void V13();
    virtual void V14();
    virtual void V15();
    virtual void V16();
    virtual void V17();
    virtual void V18();
    virtual void V19();
    virtual void V20();
    virtual void V21();
    virtual void V22();
    virtual void V23();
    virtual void V24();
    virtual bool V25();
    virtual void V26();
    virtual void V27();
    virtual void V28();
    virtual void V29();
    virtual void V30();
    virtual void V31();
    virtual void V32();
    virtual void V33();
    virtual void V34();
    virtual void V35();
    virtual void V36();
    virtual void V37();
    virtual void V38();
    virtual void V39();
    virtual void V40();
    virtual void V41();
    virtual void V42();
    virtual void V43();
    virtual void V44();
    virtual void V45();
    virtual void V46();
    virtual void V47();
    virtual void V48();
    virtual void V49();
    virtual void V50();
    virtual void V51();
    virtual void V52();
    virtual void V53();
    virtual void V54();
    virtual void V55();
    virtual void V56();
    virtual void V57();
    virtual void V58();
    virtual void V59();
    virtual void V60();
    virtual void V61();
    virtual void V62();
    virtual void V63();
    virtual void V64();
};

struct G2CONTACTARG
{
    char m_0000[8];
    RESHANDLE m_handle;
    char m_0001[0xdd1 - 8 - sizeof(RESHANDLE)];
    bool m_flag;
    bool m_dd2;
};

class G2CHAIN : public G2RULESOBJBASE, public INPUTLISTENER
{
public:
    G2CHAIN();
    virtual ~G2CHAIN();

    void A();
    void B();
    void C();
    void D();
    void E();
    void Run();
    virtual void SetContact(G2CONTACTARG *obj);
    void RemoveContact(const RESHANDLE &handle);
    virtual void OnTargetDestroyed(G2CONTACTARG *obj);
    virtual void AddTarget(ACTOR *actor);
    virtual void IncMainTarget(ACTOR *actor);
    virtual void DecMainTarget(ACTOR *actor);
    virtual void IncSecondaryTarget(ACTOR *actor);
    virtual void DecSecondaryTarget(ACTOR *actor);
    virtual void ResetTarget(ACTOR *actor);
    virtual void Stop();

    float m_f48;
    int m_f4c;
    ZEROV2 m_f50;
    float m_f58;
    int m_f5c;
    ZEROV2 m_f60;
    ZEROV2 m_f68;
    ZEROV2 m_f70;
    ZEROV2 m_f78;
    int m_f80;
    int m_f84;
    bool m_f88;
    char m_f89[0xf8a - 0xf89];
    WIDESTRING<0x200> m_w1;
    WIDESTRING<0x200> m_w2;
    WIDESTRING<0x200> m_w3;
    char m_1b8a[0x1b8c - 0x1b8a];
    int m_1b8c;
    int m_1b90;
    int m_1b94;
    int m_1b98;
    int m_1b9c;
    int m_1ba0;
    // How long until the next supercell is picked.
    float m_f1ba4;
    int m_1ba8;
    // How long until the next target is picked.
    float m_f1bac;
    // How many targets have been picked.
    int m_1bb0;
    int m_1bb4;
    int m_1bb8;
    int m_1bbc;
    int m_1bc0;
    int m_1bc4;
    int m_1bc8;
    std::list<RESHANDLE> m_contacts;
    int m_1bd8;
    int m_1bdc;
    std::list<RESHANDLE> m_contacts2;
    int m_1bec;
    int m_1bf0;
    RESHANDLE m_1bf4;
    RESHANDLE m_1c00;
    int m_1c0c;
};

void G2CHAIN::RemoveContact(const RESHANDLE &handle)
{
    m_contacts2.remove(handle);
}

void G2CHAIN::SetContact(G2CONTACTARG *obj)
{
    if (obj->m_flag) {
        m_contacts.push_back(obj->m_handle);
        m_1bd8++;
        m_1b8c++;
        m_1b90++;
    } else {
        m_contacts2.push_back(obj->m_handle);
        m_1bec++;
    }
}

G2CHAIN::G2CHAIN()
{
    m_f4c = 3;
    m_f50 = V2(2.0f, 2.0f);
    m_f60 = V2(30.0f, 60.0f);
    m_f48 = 0.005f;
    m_f58 = 120.0f;
    m_f5c = 0;
    m_f68 = V2(30.0f, 60.0f);
    m_f70 = V2(60.0f, 120.0f);
    m_f78 = V2(60.0f, 120.0f);
    m_f80 = 1000000;
    m_f84 = 100000;
    m_f88 = true;
    m_w1.Set(L"0");
    m_w2.Set(L"0");
    m_w3.Set(L"0");
    m_1b8c = 0;
    m_1b90 = 0;
    m_1b94 = 0;
    m_1b98 = 0;
    m_1b9c = 0;
    m_1ba0 = 0;
    m_f1ba4 = 0;
    m_1ba8 = 0;
    m_f1bac = 0.0f;
    m_1bb0 = 0;
    m_1bb4 = 0;
    m_1bb8 = 0;
    m_1bbc = 0;
    m_1bc0 = 0;
    m_1bc4 = 0;
    m_1bc8 = 0;
    m_1bd8 = 0;
    m_1bdc = 0;
    m_1bec = 0;
    m_1bf0 = 0;
    m_1c0c = 0;
    SetPriority(5);
}

G2CHAIN::~G2CHAIN()
{
}

__declspec(noinline) void G2CHAIN::A()
{
    ACTOR::Update();
}

void G2CHAIN::AddTarget(ACTOR *actor)
{
    (void)actor;
    m_1bb0++;
}

void G2CHAIN::IncMainTarget(ACTOR *actor)
{
    (void)actor;
    m_1ba0++;
}

void G2CHAIN::DecMainTarget(ACTOR *actor)
{
    (void)actor;
    m_1ba0--;
}

void G2CHAIN::IncSecondaryTarget(ACTOR *actor)
{
    (void)actor;
    m_1ba8++;
}

void G2CHAIN::DecSecondaryTarget(ACTOR *actor)
{
    (void)actor;
    m_1ba8--;
    if (m_f60.y > m_f60.x)
        m_f1ba4 = m_f60.x + (m_f60.y - m_f60.x) * rand() * (1.0f / 32768.0f);
    else
        m_f1ba4 = m_f60.x;
}

void G2CHAIN::ResetTarget(ACTOR *actor)
{
    (void)actor;
    m_1bb0--;
    if (m_f68.y > m_f68.x)
        m_f1bac = m_f68.x + (m_f68.y - m_f68.x) * rand() * (1.0f / 32768.0f);
    else
        m_f1bac = m_f68.x;
}

void G2CHAIN::Stop()
{
    ACTOR::Stop();
}


void G2CHAIN::E()
{
    if (!((G2GAMEVT *)g_pGame)->V25())
        return;
    ACTOR *tornado = g_pGame->GetTornado();
    if (!tornado)
        return;
    if (*(float *)((char *)tornado + 0x2a38) > 0.0f)
        return;
    if (m_1bdc < m_1bd8)
        ((G2GAMEVT *)g_pGame)->V64();
    else
        g_pGame->ShowStageResult();
}

void G2CHAIN::Run()
{
    A();
    B();
    C();
    D();
    E();
}
class G2WIND
{
public:
    static int s_classId;
    static CMDENTRY s_commands[];

    static void RegisterCommands();
};

void G2WIND::RegisterCommands()
{
    g_classParents[s_classId] = ACTOR::s_classId;
    g_classCommands[s_classId] = s_commands;
    HashCommands(s_commands);
    RegisterClassName(s_commands, "WIND", "ACTOR");
}
class G2ODEACTOR
{
public:
    static int s_classId;
};

class G2WTINTRO
{
public:
    static int s_classId;
    static CMDENTRY s_commands[];
    static void RegisterCommands();
};

void G2WTINTRO::RegisterCommands()
{
    g_classParents[s_classId] = ACTOR::s_classId;
    g_classCommands[s_classId] = s_commands;
    HashCommands(s_commands);
    RegisterClassName(s_commands, "WTINTRO", "ACTOR");
}

class G2BUILDINGPIECE
{
public:
    static int s_classId;
    static CMDENTRY s_commands[];
    static void RegisterCommands();
};

void G2BUILDINGPIECE::RegisterCommands()
{
    g_classParents[s_classId] = ACTOR::s_classId;
    g_classCommands[s_classId] = s_commands;
    HashCommands(s_commands);
    RegisterClassName(s_commands, "BUILDINGPIECE", "ACTOR");
}

void G2DESTRUCTIBLE::RegisterCommands()
{
    g_classParents[s_classId] = ACTOR::s_classId;
    g_classCommands[s_classId] = s_commands;
    HashCommands(s_commands);
    RegisterClassName(s_commands, "DESTRUCTIBLE", "ACTOR");
}

class G2DESTRUCTIBLEPIECE
{
public:
    static int s_classId;
    static CMDENTRY s_commands[];
    static void RegisterCommands();
};

void G2DESTRUCTIBLEPIECE::RegisterCommands()
{
    g_classParents[s_classId] = G2ODEACTOR::s_classId;
    g_classCommands[s_classId] = s_commands;
    HashCommands(s_commands);
    RegisterClassName(s_commands, "DESTRUCTIBLEPIECE", "ODEACTOR");
}

class G2RULES
{
public:
    static int s_classId;
    static CMDENTRY s_commands[];
    static void RegisterCommands();
};

void G2RULES::RegisterCommands()
{
    g_classParents[s_classId] = ACTOR::s_classId;
    g_classCommands[s_classId] = s_commands;
    HashCommands(s_commands);
    RegisterClassName(s_commands, "RULES", "ACTOR");
}

class G2RULESBONUSCOLLECT
{
public:
    static int s_classId;
    static CMDENTRY s_commands[];
    static void RegisterCommands();
};

void G2RULESBONUSCOLLECT::RegisterCommands()
{
    g_classParents[s_classId] = G2RULES::s_classId;
    g_classCommands[s_classId] = s_commands;
    HashCommands(s_commands);
    RegisterClassName(s_commands, "RULESBONUSCOLLECT", "RULES");
}

class G2RULESSTORY
{
public:
    static int s_classId;
    static CMDENTRY s_commands[];
    static void RegisterCommands();
};

void G2RULESSTORY::RegisterCommands()
{
    g_classParents[s_classId] = G2RULES::s_classId;
    g_classCommands[s_classId] = s_commands;
    HashCommands(s_commands);
    RegisterClassName(s_commands, "RULESSTORY", "RULES");
}

class G2FOREST
{
public:
    static int s_classId;
    static CMDENTRY s_commands[];
    static void RegisterCommands();
};

void G2FOREST::RegisterCommands()
{
    g_classParents[s_classId] = ACTOR::s_classId;
    g_classCommands[s_classId] = s_commands;
    HashCommands(s_commands);
    RegisterClassName(s_commands, "FOREST", "ACTOR");
}
class G2RULESOBJ : public G2RULESOBJBASE
{
public:
    G2RULESOBJ();
    virtual ~G2RULESOBJ();
    virtual void Update();
    virtual void Start();
    virtual void HandleTornado(CMDCALL *call);

    int m_f44;
    FIXEDSTRING<0x200> m_f48;
    int m_1148;
    float m_114c;
    float m_1150;
    int m_1154;
};

G2RULESOBJ::G2RULESOBJ()
{
    m_1150 = 0.0f;
    m_1154 = 0;
    m_f44 = 10;
    m_f48.Set("");
    m_1148 = 0;
    m_114c = 1.0f;
}

G2RULESOBJ::~G2RULESOBJ()
{
}

void G2RULESOBJ::HandleTornado(CMDCALL *call)
{
    G2RULESOBJBASE::HandleTornado(call);
    m_1148++;
    m_1154--;
}
// A supercell: the actor the chain gives a spawn time and starts. Only the
// virtuals the chain calls are declared here.
class G2SUPERCELL : public ACTOR
{
public:
    static int s_factoryId;

    virtual void Virtual143();
    virtual void Virtual144();
    virtual void Virtual145();
    virtual void SetSpawnTime(const float &value);

    char m_0000[0xce8 - 0xccc];
    float m_ce8;
    float m_cec;
    float m_cf0;
    float m_cf4;
};

// A powerup: a trigger that spawns its objects when touched. Only the
// virtuals the forest calls are declared here.
class G2POWERUP : public ACTOR
{
public:
    static int s_factoryId;

    virtual void Virtual143();
    virtual void Virtual144();
    virtual void Virtual145();
    virtual void SetMaxCommands(const int &max);
    virtual void SetSpawnAngleDelta(const float &delta);
    virtual void SetWaitOnFirst(const bool &wait);

    char m_0000[0xcd0 - 0xccc];
    float m_cd0;
};

class G2FORESTOBJ;

class G2RULEHOOK : public ACTOR
{
public:
    virtual void Virtual143();
    virtual void Virtual144();
    virtual void Virtual145();
    virtual void Virtual146();
    virtual void Virtual147();
    virtual void Virtual148();
    virtual void Virtual149();
    virtual void Virtual150();
    virtual void AddForest(G2FORESTOBJ *forest);
};

class G2FORESTOBJ : public ACTOR
{
public:
    G2FORESTOBJ();

    static int s_factoryId;
    virtual ~G2FORESTOBJ();

    // The spawner cell the forest made a powerup in.
    RESHANDLE m_handle;
    bool m_cd8;
    char m_cd9[0xcdc - 0xcd9];
    int m_cdc;
    int m_ce0;
    int m_ce4;
    int m_ce8;
    // The trees: their handles, names and the cells they are in.
    RESHANDLE m_handles[100];
    FIXEDSTRING<0x200> m_names[100];
    int m_index1[100];
    int m_index2[100];
    char m_bytes3[100];
    int m_index4[100];
    int m_deb0;
    float m_scores[100];
    float m_weights[100];
    char m_occupied[100];
    int m_counts[100];
    int m_e3c8;
    bool m_e3cc;
    bool m_e3cd;
    char m_e3ce[0xe3d0 - 0xe3ce];
    int m_count;
    FIXEDSTRING<0x200> m_name;
    bool m_e5d4;
    bool m_e5d5;
    char m_e5d6[0xe5d8 - 0xe5d6];
    float m_e5d8;
    int m_e5dc;
    int m_e5e0;
    bool m_e5e4;
    char m_e5e5[0xe5e8 - 0xe5e5];
    int m_e5e8;
    bool m_e5ec;
    char m_e5ed[0xe5f0 - 0xe5ed];
    float m_e5f0;
    int m_e5f4;
    bool m_e5f8;
    char m_e5f9[0xe5fc - 0xe5f9];
    float m_e5fc;
    RESHANDLE m_e600;
    RESHANDLE m_e60c;

    void Clear();
    void UpdateActors();
    void CreatePowerup();
    void CheckTrees();
    void LoadTrees();
    void CreateMaxPowerup();
    void DumpGrid();
    void LoadModels(const char *name);
    V2 WorldToCell(V3 point);
};

V2 G2FORESTOBJ::WorldToCell(V3 point)
{
    float radius = 0.0f;
    ACTOR *tornado = g_pGame->GetTornado();
    if (tornado)
        radius = *(float *)((char *)tornado + 0x25e0);

    V2 offset = V2(m_position.x, m_position.z) -
                V2(m_occludingBox.x, m_occludingBox.z).Rotate(m_angles.GetEulerX() * 0.017453292f) * 0.5f;
    V2 local = V2(point.x, point.z) - offset;
    const V2 &cell = local.Rotate(-m_angles.GetEulerX() * 0.017453292f);
    float cx = cell.x;
    float cy = cell.y;
    if (cx < -radius || cy < -radius ||
        cx > radius + m_occludingBox.x ||
        cy > radius + m_occludingBox.z)
        return V2(-1.0f, -1.0f);
    int row = (int)floor((float)m_e5dc / m_occludingBox.x * cx);
    int col = (int)floor((float)m_e5e0 / m_occludingBox.z * cy);
    if (row < 0)
        row = 0;
    if (m_e5dc < row)
        row = m_e5dc;
    if (col < 0)
        col = 0;
    if (m_e5e0 < col)
        col = m_e5e0;
    return V2((float)row, (float)col);
}

void G2FORESTOBJ::DumpGrid()
{
    for (int row = 0; row < m_e5dc; row++) {
        char text[0x200];
        text[0x1ff] = 0;
        text[0] = 0;
        for (int col = 0; col < m_e5e0; col++)
            SafeSprintf(text, "%s %.2f", text, m_scores[row * 10 + col]);
    }
}

void G2FORESTOBJ::CreateMaxPowerup()
{
    m_cd8 = true;
    int row = 0;
    int col = 0;
    for (int r = 0; r < m_e5dc; r++) {
        for (int c = 0; c < m_e5e0; c++) {
            if (m_scores[r * 10 + c] > m_scores[row * 10 + col]) {
                row = r;
                col = c;
            }
        }
    }
    m_weights[row * 10 + col] = 100.0f;
    CreatePowerup();
}

void G2FORESTOBJ::LoadTrees()
{
    int *p = m_counts;
    int rows = 10;
    do {
        int c = 10;
        do {
            p[-0xe1] = 0;
            *p = 0;
            p++;
        } while (--c);
    } while (--rows);
    for (int i = 0; i < m_ce8; i++) {
        char *name = (char *)this + 0x119c + i * 0x200;
        if (!g_pScene->FindActor(name)) {
            if (g_pLog)
                g_pLog->Error("Can't get tree %s!", (char *)this + 0x119c + i * 0x200);
            return;
        }
        int idx = m_index1[i];
        int col = idx % m_e5e0;
        int row = (idx - col) / m_e5e0;
        int cell = row * 10 + col;
        float value = m_scores[cell] + 10.0f;
        m_counts[cell] = m_counts[cell] + 1;
        m_scores[cell] = value;
    }
}

void G2FORESTOBJ::CheckTrees()
{
    m_cd8 = false;
    for (int r = 0; r < m_e5dc; r++) {
        for (int c = 0; c < m_e5e0; c++) {
            if (m_occupied[r * 10 + c] != 0)
                m_cd8 = true;
        }
    }
    if (!m_cd8) {
        G2RULEHOOK *rules = (G2RULEHOOK *)g_pGame->GetRules();
        if (rules)
            rules->AddForest(this);
        G2POWERUP *actor = (G2POWERUP *)g_pScene->GetActor(m_handle);
        if (actor && actor->GetFactoryId() == G2POWERUP::s_factoryId) {
            actor->SetWaitOnFirst(false);
            actor->m_cd0 = 0.1f;
        }
    }
}

void G2FORESTOBJ::CreatePowerup()
{
    G2POWERUP *actor = (G2POWERUP *)g_pScene->CreateActor(G2POWERUP::s_factoryId);
    if (actor) {
        actor->SetPosition(GetPosition());
        actor->SetSpawnAngleDelta(120.0f);
        actor->SetMaxCommands(2);
        actor->SetOccludingBox(V3(40.0f, 40.0f, 40.0f));
        actor->SetWaitOnFirst(false);
        actor->SetActive(true);
        m_handle = actor->m_handle;
        m_e3cc = false;
    }
}

void G2FORESTOBJ::UpdateActors()
{
    for (int i = 0; i < m_count; i++) {
        WTACTOR *actor = g_pScene->GetActor(m_handles[i]);
        if (actor) {
            V3 position = *(V3 *)((char *)actor + 0x28);
            WTACTOR *scenario = g_pGame->GetScenario();
            *(float *)((char *)actor + 0x2c) = scenario->GetTerrainPosition(position).y;
        }
    }
}

G2FORESTOBJ::G2FORESTOBJ()
{
    m_cd8 = false;
    m_cdc = 0;
    m_ce0 = 0;
    m_ce4 = 0;
    m_ce8 = 0;
    for (int i = 0; i < 100; i++) {
        m_handles[i] = RESHANDLE(-1, -1, 0);
        m_names[i].Set("");
        m_index1[i] = 0;
        m_index2[i] = 0;
        m_bytes3[i] = 0;
        m_index4[i] = 0;
    }
    m_deb0 = 0;
    Clear();
    m_e600 = RESHANDLE(-1, -1, 0);
    m_e3c8 = -1;
    m_e3cc = false;
    m_e3cd = false;
    m_count = 0;
    m_name.Set("");
    m_e5dc = 5;
    m_e5e0 = 5;
    m_e5d4 = false;
    m_e5d5 = false;
    m_e5e4 = false;
    m_e5e8 = 0;
    m_e5ec = false;
    m_e5f4 = 0;
    m_e5f8 = false;
    m_e5d8 = 0.05f;
    m_e5f0 = 45.0f;
    m_e5fc = 1.5f;
}

G2FORESTOBJ::~G2FORESTOBJ()
{
}

void G2FORESTOBJ::Clear()
{
    char *q = m_occupied;
    int *p = (int *)m_weights;
    int rows = 10;
    int r = 0;
    do {
        int c = 0;
        do {
            p[-100] = 0;
            *p = 0;
            q[r * 10 + c] = 0;
            p[0x7d] = 0;
            c++;
            p++;
        } while (c < 10);
        r++;
    } while (--rows);
}
struct G2STREQ
{
    char m_buf[0x40];

    char Equals(const char *other) const;
};

char G2STREQ::Equals(const char *other) const
{
    return strcmp(m_buf, other) == 0;
}
__declspec(noinline) void G2RULESOBJBASE::HandleTornado(CMDCALL *call)
{
    (void)call;
    ACTOR *tornado = g_pGame->GetTornado();
    if (tornado)
        return tornado->OnTornadoCreated(call);
}
__declspec(noinline) void ANG::SetEuler(const float &x, const float &y, const float &z)
{
    if (euler.x != x) {
        euler.x = x;
        flags = MATRIX_DIRTY;
    }
    if (euler.y != y) {
        euler.y = y;
        flags = MATRIX_DIRTY;
    }
    if (euler.z != z) {
        euler.z = z;
        flags = MATRIX_DIRTY;
    }
}

void G2RULESOBJ::Update()
{
    ((G2CHAIN *)this)->A();
    m_1150 -= g_pTimer->m_dt;
    if (m_1150 <= 0.0f && m_1154 < m_f44) {
        m_1150 = m_114c;
        ACTOR *tornado = g_pGame->GetTornado();
        if (!tornado)
            return;
        m_1154++;
        V3 position = tornado->GetPosition();
        position.x += (float)rand() * (200.0f / RAND_MAX) - 100.0f;
        position.y += (float)rand() * (200.0f / RAND_MAX) - 100.0f;
        position.z += (float)rand() * (200.0f / RAND_MAX) - 100.0f;
        ANG angles(0.0f, 0.0f, 0.0f);
        angles.SetEuler(rand() * (360.0f / RAND_MAX), 0.0f, 0.0f);
        ACTOR *actor = g_pGame->CreateStormChaser(m_f48.m_buf, position, angles,
                                                  *(float *)((char *)tornado + 0x984));
        actor->SetActivation(1);
        actor->SetDeactivation(1);
    }
}
void G2RULESOBJ::Start()
{
    SetActivation(1);
    SetDeactivation(1);
    SetName(CSTRING("Rules"));
    if (g_pGame)
        g_pGame->SetRules(this);
    *(bool *)((char *)this + 0x998) = true;
}

extern RESOURCEMANAGER *g_pTreeModels;

void G2FORESTOBJ::LoadModels(const char *name)
{
    if (*name != '\0') {
        LoadTrees();
        DumpGrid();
        int *index = m_index4;
        char *tree = (char *)this + 0x119c;
        for (int i = 100; i != 0; i--) {
            RESOURCEMASTER *master = g_pTreeModels->GetMaster(m_name.m_buf);
            WTACTOR *actor = g_pScene->FindActor(tree);
            if (actor) {
                const char *src = ((const char **)((char *)master + 0xac))[*index];
                if (!src)
                    src = "";
                CSTRING resource(src);
                ((ACTOR *)actor)->SetActorResourcesTemplate(resource);
            }
            index++;
            tree += 0x200;
        }
        CreateMaxPowerup();
    }
}

void G2MID2::Helper()
{
    RESOURCE *first = g_pResources->GetResource(m_handle);
    if (first)
        first->Virtual20();
    m_handle.Invalidate();
    RESOURCE *second = g_pResources->GetResource(((RESHANDLE *)((char *)this + 0xb58))[m_1384]);
    if (second)
        second->SetActive(true);
}

void G2CHAIN::OnTargetDestroyed(G2CONTACTARG *obj)
{
    if (obj->m_flag) {
        m_contacts.remove(obj->m_handle);
        if (!obj->m_dd2)
            return;
        int count = ++m_1bdc;
        int left = m_1bd8 - count;
        m_1b8c = left;
        const char *title;
        if (left > 0) {
            title = "GGRP:TargetDestroyed";
        } else {
            title = "GGRP:TargetDestroyedLast";
            ACTOR *tornado = g_pGame->GetTornado();
            if (tornado)
                m_1bc0 = (int)(m_f80 * *(float *)((char *)tornado + 0x2a38) * 0.01f);
            g_pGui->HideControl("targets_hud", 1.0f);
        }
        GUICONTROL *control = (GUICONTROL *)g_pGui->GetObject(m_1bf4);
        if (control)
            control->Close();
        GUIGROUP *group = g_pGui->AddGroup(title, "targetdestroyed", V2(0.5f, 0.5f), V2(1.0f, 1.0f), 1.0f, 0x50);
        if (group)
            m_1bf4 = group->m_handle;
    } else {
        m_contacts2.remove(obj->m_handle);
        if (!obj->m_dd2)
            return;
        int count = ++m_1bf0;
        int left = m_1bec - count;
        m_1b94 = left;
        const char *title;
        if (left > 0) {
            title = "GGRP:TargetSecDestroyed";
        } else {
            title = "GGRP:TargetSecDestroyedLast";
        }
        GUICONTROL *control = (GUICONTROL *)g_pGui->GetObject(m_1c00);
        if (control)
            control->Close();
        GUIGROUP *group = g_pGui->AddGroup(title, "targetsecdestroyed", V2(0.5f, 0.5f), V2(1.0f, 1.0f), 1.0f, 0x50);
        if (group)
            m_1c00 = group->m_handle;
    }
}

void G2CHAIN::D()
{
    ACTOR *tornado = g_pGame->GetTornado();
    if (!tornado)
        return;
    if (m_1bb0 > 1)
        return;
    m_f1bac -= g_pTimer->m_dt;
    if (m_f1bac > 0.0f)
        return;

    ACTOR *actors[64];
    int count = g_pScene->FindActors(G2FORESTOBJ::s_factoryId, actors, 64, false);
    if (!count)
        return;

    float best = 1000.0f;
    G2FORESTOBJ *bestActor = 0;
    for (int i = 0; i < count; i++) {
        G2FORESTOBJ *forest = (G2FORESTOBJ *)actors[i];
        if (!forest)
            continue;
        if (!forest->m_e3cc)
            continue;
        const V3 &tornadoPos = tornado->GetPosition();
        const V3 &forestPos = forest->GetPosition();
        V2 delta(tornadoPos.x - forestPos.x, tornadoPos.z - forestPos.z);
        if (delta.Length() < best) {
            V2 screen(0.0f, 0.0f);
            ((CAMERA *)g_pCamera)->WorldToScreen(forest->GetPosition(), &screen);
            if (screen.x >= 0.0f && screen.x <= 1.0f &&
                screen.y >= 0.0f && screen.y <= 1.0f) {
                bestActor = forest;
                best = forest->GetPosition().DistanceXZ(tornado->GetPosition());
            }
        }
    }
    if (!bestActor)
        return;
    bestActor->CreateMaxPowerup();
    float wait;
    if (m_f68.y > m_f68.x)
        wait = m_f68.x + (m_f68.y - m_f68.x) * rand() * (1.0f / 32768.0f);
    else
        wait = m_f68.x;
    m_f1bac = wait;
    AddTarget(bestActor);
}

void G2CHAIN::C()
{
    ACTOR *tornado = g_pGame->GetTornado();
    if (!tornado)
        return;
    if (m_1ba8 > 0)
        return;
    m_f1ba4 -= g_pTimer->m_dt;
    if (m_f1ba4 > 0.0f)
        return;

    ACTOR *actors[64];
    int count = g_pScene->FindActors(G2SUPERCELL::s_factoryId, actors, 64, false);
    if (!count) {
        if (g_pLog)
            g_pLog->Error("Missing SUPERCELLS!!");
        m_f1ba4 = 1000.0f;
        return;
    }

    int i;
    float best = 1000.0f;
    G2SUPERCELL *bestActor = 0;
    for (i = 0; i < count; i++) {
        G2SUPERCELL *supercell = (G2SUPERCELL *)actors[i];
        if (!supercell)
            continue;
        V3 tornadoPos = tornado->GetPosition();
        V3 supercellPos = supercell->GetPosition();
        V2 delta(tornadoPos.x - supercellPos.x, tornadoPos.z - supercellPos.z);
        if (delta.Length() < best) {
            V2 screen(0.0f, 0.0f);
            ((CAMERA *)g_pCamera)->WorldToScreen(supercell->GetPosition(), &screen);
            if (screen.x >= 0.0f && screen.x <= 1.0f &&
                screen.y >= 0.0f && screen.y <= 1.0f) {
                bestActor = supercell;
                best = supercell->GetPosition().DistanceXZ(tornado->GetPosition());
            }
        }
    }
    if (!bestActor)
        return;
    float spawn;
    if (bestActor->m_cf0 > bestActor->m_cec)
        spawn = bestActor->m_cec + (bestActor->m_cf0 - bestActor->m_cec) * rand() * (1.0f / 32768.0f);
    else
        spawn = bestActor->m_cec;
    bestActor->SetSpawnTime(spawn);
    bestActor->SetActive(true);
    float wait;
    if (m_f60.y > m_f60.x)
        wait = m_f60.x + (m_f60.y - m_f60.x) * rand() * (1.0f / 32768.0f);
    else
        wait = m_f60.x;
    m_f1ba4 = wait + bestActor->m_cf4;
    IncSecondaryTarget(bestActor);
}

void MATRIX::RotateY(float degrees)
{
    float negated = -degrees;
    float radians = negated * 0.017453292f;
    float c = (float)cos(radians);
    float s = (float)sin(radians);
    _11 = 1.0f;
    _12 = 0.0f;
    _13 = 0.0f;
    _14 = 0.0f;
    _21 = 0.0f;
    _22 = c;
    _23 = -s;
    _24 = 0.0f;
    _31 = 0.0f;
    _32 = s;
    _33 = c;
    _34 = 0.0f;
    _41 = 0.0f;
    _42 = 0.0f;
    _43 = 0.0f;
    _44 = 1.0f;
}

void ANG::SetEulerX(const float &x)
{
    matrix.ToEuler(&euler.z, &euler.y, &euler.x);
    flags &= ~EULER_DIRTY;
    if (x != euler.x) {
        euler.x = x;
        flags = MATRIX_DIRTY;
    }
}

void MATRIX::Transpose(MATRIX *out)
{
    out->_11 = _11;
    out->_21 = _12;
    out->_31 = _13;
    out->_12 = _21;
    out->_22 = _22;
    out->_32 = _23;
    out->_13 = _31;
    out->_23 = _32;
    out->_33 = _33;
    out->_14 = _41;
    out->_24 = _42;
    out->_34 = _43;
}

void G2MID2::FindResource()
{
    m_1384 = -1;
    int i = 0;
    RESHANDLE *handles = (RESHANDLE *)((char *)this + 0xb58);
    while (i < 16) {
        RESOURCE *resource = g_pResources->GetResource(*handles);
        if (resource) {
            int managerId = g_pActorResourceManager->m_id;
            if (resource->GetManagerId() == managerId) {
                m_1384 = i;
                return;
            }
        }
        i++;
        handles++;
    }
}

void G2MID2::SetResourcesActive(bool active)
{
    if (!*(bool *)((char *)this + 0x9a0)) {
        m_1371 = false;
        return;
    }
    if (m_1371 == active)
        return;
    int managerId;
    RESHANDLE *handles = (RESHANDLE *)((char *)this + 0xb58);
    for (int i = 16; i != 0; i--) {
        RESOURCE *resource = g_pResources->GetResource(*handles);
        if (resource) {
            managerId = g_pActorResourceManager2->m_id;
            if (resource->GetManagerId() == managerId)
                resource->SetActive(active);
        }
        handles++;
    }
    m_1371 = active;
}

void G2MID2::AddActiveMesh()
{
    if (!g_pMIDList || !g_pMIDList->IsReady())
        return;
    if (m_139c)
        return;
    RESHANDLE handle = ((RESHANDLE *)((char *)this + 0xb58))[0];
    m_139c = true;
    RESOURCE *resource = g_pResources->GetResource(handle);
    if (!resource)
        return;
    int managerId = g_pActorResourceManager3->m_id;
    if (resource->GetManagerId() != managerId)
        return;
    G2MIDMESH *mesh = ((G2MIDRES *)resource)->GetMIDMesh(1);
    if (!mesh)
        return;
    G2MIDITEM *item = mesh->GetItem();
    if (!item)
        return;
    g_pMIDList->Add(item, m_113c.c_str());
}
