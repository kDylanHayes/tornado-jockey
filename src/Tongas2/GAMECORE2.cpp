// Tongas2/GAMECORE2 -- the game-code block at 0x4162f0. These are the ANG,
// V3, FIXEDSTRING and SPAWNER members the original translation unit emitted
// out of line.
#define ANG_EULER_OUTOFLINE
#define V3_ISZERO_OUTOFLINE
#define SPAWNER_SETTERS_OUTOFLINE

#include <new>

#include "Tongas2/FIXEDSTRING.h"
#include "Tongas2/MATHS.h"
#include "Tongas2/RENDERER.h"
#include "Tongas2/SPAWNER.h"
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
