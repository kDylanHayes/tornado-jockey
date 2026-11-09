// Tongas2/GAMECORE2 -- the game-code block at 0x4162f0. These are the ANG,
// V3, FIXEDSTRING and SPAWNER members the original translation unit emitted
// out of line.
#define ANG_EULER_OUTOFLINE
#define V3_ISZERO_OUTOFLINE
#define SPAWNER_SETTERS_OUTOFLINE

#include <new>

#include "Tongas2/FIXEDSTRING.h"
#include "Tongas2/MATHS.h"
#include "Tongas2/GAME.h"
#include "Tongas2/RENDERER.h"
#include "Tongas2/TIMER.h"
#include "Tongas2/SPAWNER.h"
#include "Tongas2/RESOURCE.h"
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

struct G2VALUE
{
    unsigned int m_a, m_b, m_c;
};

struct G2NODE
{
    G2NODE *m_next;
    G2NODE *m_prev;
    G2VALUE m_value;
};

class G2LIST
{
public:
    G2NODE *m_head;
    G2NODE *Buynode();
    G2NODE *InsertNode(G2NODE *next, G2NODE *prev, const G2VALUE &value);
};

G2NODE *G2LIST::Buynode()
{
    G2NODE *node = new G2NODE;
    new (&node->m_next) G2NODE *(node);
    new (&node->m_prev) G2NODE *(node);
    return node;
}

G2NODE *G2LIST::InsertNode(G2NODE *next, G2NODE *prev, const G2VALUE &value)
{
    G2NODE *node = new G2NODE;
    if (node) {
        node->m_next = next;
        node->m_prev = prev;
        node->m_value = value;
    }
    return node;
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
class G2D1 : public ACTOR
{
public:
    virtual ~G2D1();
};

G2D1::~G2D1()
{
}

class G2D2 : public ACTOR
{
public:
    virtual ~G2D2();
};

G2D2::~G2D2()
{
}

class G2D3 : public ACTOR
{
public:
    virtual ~G2D3();
};

G2D3::~G2D3()
{
}
class G2D5 : public ACTOR
{
public:
    virtual ~G2D5();

    char m_0000[0x113c - 0xccc];
    void *m_113c;
};

G2D5::~G2D5()
{
    if (m_113c) {
        free(m_113c);
        m_113c = 0;
    }
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
class G2MID2 : public ACTOR
{
public:
    virtual void Stop();
    void Helper();
};

__declspec(noinline) void G2MID2::Stop()
{
    Helper();
    ACTOR::Stop();
}

class G2CHAIN
{
public:
    void A();
    void B();
    void C();
    void D();
    void E();
    void Run();
};

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

class G2DESTRUCTIBLE
{
public:
    static int s_classId;
    static CMDENTRY s_commands[];
    static void RegisterCommands();
};

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
struct G2INT3
{
    int x, y, z;
    G2INT3() {}
    G2INT3(int x_, int y_, int z_) : x(x_), y(y_), z(z_) {}
};

class G2RULESOBJBASE : public ACTOR
{
public:
    G2RULESOBJBASE();
    virtual ~G2RULESOBJBASE();

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

class G2RULESOBJ : public G2RULESOBJBASE
{
public:
    G2RULESOBJ();
    virtual ~G2RULESOBJ();
    virtual void Update();
    virtual void Start();

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
};

class G2FORESTOBJ : public ACTOR
{
public:
    RESHANDLE m_handle;
    char m_cd8[0xcec - 0xcd8];
    RESHANDLE m_handles[32];
    char m_d64[0xdeb4 - 0xe6c];
    int m_a[100];
    int m_b[100];
    char m_bytes[100];
    int m_d[100];
    char m_e3c8[0xe3cc - 0xe3c8];
    unsigned char m_e3cc;
    char m_e3cd[0xe3d0 - 0xe3cd];
    int m_count;

    void Clear();
    void UpdateActors();
    void CreatePowerup();
};

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

void G2FORESTOBJ::Clear()
{
    char *q = m_bytes;
    int *p = m_b;
    int rows = 10;
    do {
        int c = 0;
        do {
            p[-100] = 0;
            *p = 0;
            char *dst = q + c;
            *dst = 0;
            p[0x7d] = 0;
            c++;
            p++;
        } while (c < 10);
        q += 10;
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
class G2TORNADOCMD
{
public:
    void HandleTornado(CMDCALL *call);
};

void G2TORNADOCMD::HandleTornado(CMDCALL *call)
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
        angles.SetEuler((float)rand() * (360.0f / RAND_MAX), 0.0f, 0.0f);
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
