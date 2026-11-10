// Tongas2/GAMECORE3 -- the game-code block at 0x42c400. The OBJECT and
// RESOURCE accessors the original translation unit emitted out of line.
#define OBJECT_OUTOFLINE
#define CSTRING_DTOR_OUTOFLINE
#define RM_SETTERS_OUTOFLINE
#define TRIGGER_SETTERS_OUTOFLINE
#define GC3_OUTOFLINE
#define ANG_CTOR_INIT
#define MATRIX_AXIS_METHODS
#define ANG_UPDATE_OUTOFLINE

#include <deque>

#include "Tongas2/BONEDMODEL.h"
#include "Tongas2/RESOURCE.h"

#include "Tongas2/CMDBASE.h"
#include "Tongas2/CWSTRING.h"
#include "Tongas2/RESOURCES.h"
#include "Tongas2/SCENE.h"
#include "Tongas2/TRIGGER.h"

template class std::deque<unsigned short>;

int RESOURCE::GetClassId()
{
    return s_classId;
}

int RESOURCE::GetParentClassId()
{
    return g_classParents[s_classId];
}

const char *RESOURCE::GetClassName()
{
    return g_classNames[s_classId];
}

int RESOURCEMASTER::Virtual08()
{
    return (int)(size_t)g_pResources;
}

void OBJECT::SetFlag54(const bool &value)
{
    m_54 = value;
}

void OBJECT::SetOffsetV(V3 offset)
{
    m_offset = offset;
}

void OBJECT::SetOffset(float x, float y, float z)
{
    SetOffsetV(V3(x, y, z));
}

V3 OBJECT::GetOffset()
{
    return m_offset;
}

void OBJECT::SetScaleFactorV(V3 scale)
{
    m_scaleFactor = scale;
}

void OBJECT::SetScaleFactor(float x, float y, float z)
{
    SetScaleFactorV(V3(x, y, z));
}

void OBJECT::SetRelAngles(ANG angles)
{
    m_relAngles = angles;
}

void OBJECT::SetRelRotation(float x, float y, float z)
{
    SetRelAngles(ANG(x, y, z));
}

ANG OBJECT::GetRelAngles()
{
    return m_relAngles;
}

void OBJECT::SetRelLayer(int layer)
{
    m_ac = layer;
}

int OBJECT::GetRelLayer()
{
    return m_ac;
}

void OBJECT::SetAlphaFactor(float alpha)
{
    m_alphaFactor = alpha;
}

float OBJECT::GetAlphaFactor()
{
    return m_alphaFactor;
}

void OBJECT::SetPosition(V3 position)
{
    m_position = position;
}

void OBJECT::SetPositionXYZ(float x, float y, float z)
{
    SetPosition(V3(x, y, z));
}

V3 OBJECT::GetPosition()
{
    return m_position;
}

void OBJECT::SetScaleV(V3 scale)
{
    m_scale = scale;
}

void OBJECT::SetScale(float x, float y, float z)
{
    SetScaleV(V3(x, y, z));
}

V3 OBJECT::GetScale()
{
    return m_scale;
}

void OBJECT::SetRotation(float x, float y, float z)
{
    SetAngles(ANG(x, y, z));
}

ANG OBJECT::GetAngles()
{
    return m_angles;
}

void OBJECT::SetLayer(int layer)
{
    m_19c = layer;
}

int OBJECT::GetLayer()
{
    return m_19c;
}

void OBJECT::SetAlpha(float alpha)
{
    m_layer = (int)(alpha * 255.0f);
}

float OBJECT::GetAlpha()
{
    return m_layer * (1.0f / 255.0f);
}

void OBJECT::SetVisibility(int value)
{
    m_1ac = value;
}

int OBJECT::GetVisibility()
{
    return m_1ac;
}

RESHANDLE OBJECT::GetParent()
{
    return m_parent;
}

void OBJECT::SetAngles(ANG angles)
{
    m_angles = angles;
    if (m_angles != m_134) {
        if (m_angles.flags & ANG::MATRIX_DIRTY)
            m_angles.UpdateMatrix();
        m_184 = m_angles.matrix.GetForward();
        m_190 = m_angles.GetUpPoint();
    }
}

void OBJECT::Init()
{
    {
        V3 minusHundred(-100.0f, -100.0f, -100.0f);
        m_lastPosition = minusHundred;
        m_lastScale = minusHundred;
    }
    {
        V3 minusHundred(-100.0f, -100.0f, -100.0f);
        m_c0 = minusHundred;
        m_d8 = minusHundred;
    }
    m_134 = ANG(-1.0f, -1.0f, -1.0f);
    m_lastLayer = -1;
    m_1a0 = -1;
    m_fadeDistance = -1.0f;
    m_initialized = true;
    m_54 = true;
    m_a8 = true;
    m_3fc = false;
    m_1ac = 1;
    if (m_tag) {
        delete m_tag;
        m_tag = 0;
    }
}

void OBJECT::Virtual20()
{
    if (m_1b4)
        return;
    m_1b4 = true;
    if (m_parent.IsValid())
        Detach();
    if (m_initialized) {
        if (m_active) {
            Stop();
            m_active = false;
        }
        m_master->ReleaseCopy(this);
        m_initialized = false;
    }
    m_1b4 = false;
    if (m_tag) {
        delete m_tag;
        m_tag = 0;
    }
}

bool operator!=(const ANG &a, const ANG &b)
{
    ANG ca = a;    if (ca.flags & ANG::EULER_DIRTY)
        ca.UpdateEuler();
    ANG cb = b;
    if (cb.flags & ANG::EULER_DIRTY)
        cb.UpdateEuler();
    return ca.euler.x != cb.euler.x || ca.euler.y != cb.euler.y || ca.euler.z != cb.euler.z;
}

CSTRING::~CSTRING()
{
    if (m_str) {
        free(m_str);
        m_str = 0;
    }
}

void RESOURCEMASTER::SetGroupMask(const int &mask)
{
    m_groupMask = mask;
}

const char *RESOURCEMASTER::Virtual14()
{
    return m_fileName.c_str();
}

void TRIGGER::SetMaxCommands(const int &max)
{
    m_maxCommands = max;
}

long SupportsICCProfiles()
{
    return 1;
}

void ANG::Rotate(const MATRIX &m)
{
    if (flags & MATRIX_DIRTY) {
        matrix.FromEuler(euler.z, euler.y, euler.x);
        flags &= ~MATRIX_DIRTY;
    }
    matrix = m * matrix;
    flags = EULER_DIRTY;
}

void ANG::Rotate(const ANG &other)
{
    if (flags & MATRIX_DIRTY) {
        matrix.FromEuler(euler.z, euler.y, euler.x);
        flags &= ~MATRIX_DIRTY;
    }
    ANG &o = const_cast<ANG &>(other);
    if (o.flags & MATRIX_DIRTY) {
        o.matrix.FromEuler(o.euler.z, o.euler.y, o.euler.x);
        o.flags &= ~MATRIX_DIRTY;
    }
    MATRIX m = o.matrix;
    matrix = m * matrix;
    flags = EULER_DIRTY;
}

V3 ANG::Transform(V3 point)
{
    if (flags & MATRIX_DIRTY) {
        matrix.FromEuler(euler.z, euler.y, euler.x);
        flags &= ~MATRIX_DIRTY;
    }
    V3 result;
    result.x = point.x * matrix._11 + point.z * matrix._31 + point.y * matrix._21 + matrix._41;
    result.y = point.x * matrix._12 + point.z * matrix._32 + point.y * matrix._22 + matrix._42;
    result.z = point.x * matrix._13 + point.z * matrix._33 + point.y * matrix._23 + matrix._43;
    return result;
}

V3 ANG::GetRightPoint()
{
    if (flags & MATRIX_DIRTY) {
        matrix.FromEuler(euler.z, euler.y, euler.x);
        flags &= ~MATRIX_DIRTY;
    }
    return V3(matrix._41 + matrix._11, matrix._42 + matrix._12, matrix._43 + matrix._13);
}

RESOURCE::~RESOURCE()
{
    if (m_initialized) {
        if (g_pLog)
            g_pLog->Error("ERROR!!! - deleting used copy of %s!!!", m_master->m_name.c_str());
        if (m_initialized) {
            if (m_active) {
                Stop();
                m_active = false;
            }
            m_master->ReleaseCopy(this);
            m_initialized = false;
        }
    }
}

OBJECT::~OBJECT()
{
    if (m_parent.IsValid()) {
        if (g_pLog)
            g_pLog->Error("ERROR!! deleting an attached resource!!! - must have been detached first!");
    }
    if (m_tag) {
        delete m_tag;
        m_tag = 0;
    }
}

int RESOURCE::Virtual08()
{
    OBJECT *self = (OBJECT *)this;
    WTACTOR *actor = g_pScene->GetActor(self->m_parent);
    if (!actor)
        return (int)(size_t)g_pResources;
    return (int)(size_t)actor;
}

void BONEDMODELRESOURCE::Stop()
{
    StopAnimation();
}

CWSTRING &CWSTRING::operator=(const CWSTRING &other)
{
    if (*this != other)
        *this = other.c_str();
    return *this;
}

OBJECT::OBJECT(RESOURCEMASTER *master, RESHANDLE handle)
    : RESOURCE(master, handle),
      m_offset(0.0f, 0.0f, 0.0f),
      m_scaleFactor(0.0f, 0.0f, 0.0f),
      m_position(0.0f, 0.0f, 0.0f),
      m_c0(0.0f, 0.0f, 0.0f),
      m_scale(0.0f, 0.0f, 0.0f),
      m_d8(0.0f, 0.0f, 0.0f),
      m_184(0.0f, 0.0f, 0.0f),
      m_190(0.0f, 0.0f, 0.0f),
      m_drawPosition(0.0f, 0.0f, 0.0f),
      m_drawScale(0.0f, 0.0f, 0.0f),
      m_lastPosition(0.0f, 0.0f, 0.0f),
      m_lastScale(0.0f, 0.0f, 0.0f),
      m_parent(),
      m_attached()
{
    m_scaleFactor.x = 1.0f;
    m_scaleFactor.y = 1.0f;
    m_scaleFactor.z = 1.0f;
    m_ac = 0;
    m_alphaFactor = 1.0f;
    m_scale.x = 1.0f;
    m_scale.y = 1.0f;
    m_scale.z = 1.0f;
    m_d8.x = 1.0f;
    m_d8.y = 1.0f;
    m_d8.z = 1.0f;
    m_lastScale.x = 1.0f;
    m_lastScale.y = 1.0f;
    m_lastScale.z = 1.0f;
    m_layer = 255;    m_lastLayer = 255;
    m_1a0 = -1;
    m_19c = 0;
    m_1b4 = false;
    m_tag = 0;
    m_3fc = false;
    m_fadeDistance = -1.0f;
    m_a8 = true;
    m_54 = true;
    m_1e8 = -1.0f;
    m_1ac = 1;
}

class T88
{
public:
    virtual void d0(); virtual void d1(); virtual void d2(); virtual void d3(); virtual void d4(); virtual void d5();
    virtual void d6(); virtual void d7(); virtual void d8(); virtual void d9(); virtual void d10(); virtual void d11();
    virtual void d12(); virtual void d13(); virtual void d14(); virtual void d15(); virtual void d16(); virtual void d17();
    virtual void d18(); virtual void d19(); virtual void d20(); virtual void d21(); virtual void d22(); virtual void d23();
    virtual void d24(); virtual void d25(); virtual void d26(); virtual void d27(); virtual void d28(); virtual void d29();
    virtual void d30(); virtual void d31(); virtual void d32(); virtual void d33();
    virtual void T();
    void Thunk();
};

void T88::Thunk()
{
    T();
}

class T8c
{
public:
    virtual void d0(); virtual void d1(); virtual void d2(); virtual void d3(); virtual void d4(); virtual void d5();
    virtual void d6(); virtual void d7(); virtual void d8(); virtual void d9(); virtual void d10(); virtual void d11();
    virtual void d12(); virtual void d13(); virtual void d14(); virtual void d15(); virtual void d16(); virtual void d17();
    virtual void d18(); virtual void d19(); virtual void d20(); virtual void d21(); virtual void d22(); virtual void d23();
    virtual void d24(); virtual void d25(); virtual void d26(); virtual void d27(); virtual void d28(); virtual void d29();
    virtual void d30(); virtual void d31(); virtual void d32(); virtual void d33(); virtual void d34();
    virtual void T();
    void Thunk();
};

void T8c::Thunk()
{
    T();
}

class T90
{
public:
    virtual void d0(); virtual void d1(); virtual void d2(); virtual void d3(); virtual void d4(); virtual void d5();
    virtual void d6(); virtual void d7(); virtual void d8(); virtual void d9(); virtual void d10(); virtual void d11();
    virtual void d12(); virtual void d13(); virtual void d14(); virtual void d15(); virtual void d16(); virtual void d17();
    virtual void d18(); virtual void d19(); virtual void d20(); virtual void d21(); virtual void d22(); virtual void d23();
    virtual void d24(); virtual void d25(); virtual void d26(); virtual void d27(); virtual void d28(); virtual void d29();
    virtual void d30(); virtual void d31(); virtual void d32(); virtual void d33(); virtual void d34(); virtual void d35();
    virtual void T();
    void Thunk();
};

void T90::Thunk()
{
    T();
}

class T94
{
public:
    virtual void d0(); virtual void d1(); virtual void d2(); virtual void d3(); virtual void d4(); virtual void d5();
    virtual void d6(); virtual void d7(); virtual void d8(); virtual void d9(); virtual void d10(); virtual void d11();
    virtual void d12(); virtual void d13(); virtual void d14(); virtual void d15(); virtual void d16(); virtual void d17();
    virtual void d18(); virtual void d19(); virtual void d20(); virtual void d21(); virtual void d22(); virtual void d23();
    virtual void d24(); virtual void d25(); virtual void d26(); virtual void d27(); virtual void d28(); virtual void d29();
    virtual void d30(); virtual void d31(); virtual void d32(); virtual void d33(); virtual void d34(); virtual void d35();
    virtual void d36();
    virtual void T();
    void Thunk();
};

void T94::Thunk()
{
    T();
}

// The forest object's ClearForest and PreviewForest properties. Their vtable
// slots follow the setters GAMECORE2's G2FORESTOBJ defines; the layout is the
// same class, only the members these two methods touch are repeated here.
class G2FORESTOBJ : public ACTOR
{
public:
    virtual ~G2FORESTOBJ();

    virtual SRESULT Virtual140(CMDCALL *call);
    virtual void SetDescription(const int &description);
    virtual void SetE3cc(const bool &value);
    virtual void SetE3cd(const bool &value);
    virtual void SetNumTrees(const int &count);
    virtual void SetForestTemplate(const FIXEDSTRING<0x200> &name);

    virtual void SetPreviewForest(const bool &preview);
    virtual void SetClearForest(const bool &clear);
    virtual SRESULT SomeCommand(int value);

    void CreateMaxPowerup();
    void CreateTree(int index);
    void RemoveTree(int index);
    void Clear();

    RESHANDLE m_handle;
    bool m_cd8;
    char m_cd9[0xcdc - 0xcd9];
    int m_cdc;
    int m_ce0;
    int m_ce4;
    int m_ce8;
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
};

// Lays out the preview trees the forest would place, once the standing ones
// are removed.
void G2FORESTOBJ::SetPreviewForest(const bool &preview)
{
    if (!preview)
        return;
    SetClearForest(true);
    for (int i = 0; i < m_count; i++)
        CreateTree(i);
    m_e5d4 = false;
    m_placed = true;
}

// Removes the trees standing in the grid and clears the grid itself.
void G2FORESTOBJ::SetClearForest(const bool &clear)
{
    if (!clear)
        return;
    for (int i = m_ce8 - 1; i >= 0; i--)
        RemoveTree(i);
    Clear();
    m_e5d5 = false;
}

// A minitornado's resource master: the actor resources template it hands the
// minitornado, then the four parameters the tornado copies for its movement.
class MINITORNADOMASTER : public RESOURCEMASTER
{
public:
    CSTRING m_template;
    float m_64;
    float m_68;
    float m_6c;
    float m_70;
};

extern RESOURCEMANAGER *g_pMinitornadoTemplates;

class GD42ddd0 : public ACTOR
{
public:
    virtual ~GD42ddd0();

    virtual void SetTemplate(const FIXEDSTRING<0x200> &name);
    virtual void SetEcc(const float &value);
    virtual void SetEd0(const float &value);
    virtual void SetEd4(const float &value);
    virtual void SetEd8(const float &value);

    FIXEDSTRING<0x200> m_name;
    float m_ecc;
    float m_ed0;
    float m_ed4;
    float m_ed8;
};

__declspec(noinline) GD42ddd0::~GD42ddd0()
{
}

// Takes the named minitornado template: its actor resources template and the
// four parameters that go with it.
void GD42ddd0::SetTemplate(const FIXEDSTRING<0x200> &name)
{
    m_name = name;
    MINITORNADOMASTER *master = (MINITORNADOMASTER *)g_pMinitornadoTemplates->GetMaster(name.m_buf);
    if (master) {
        SetActorResourcesTemplate(CSTRING(master->m_template.c_str()));
        SetEcc(master->m_64);
        SetEd0(master->m_68);
        SetEd4(master->m_6c);
        SetEd8(master->m_70);
    }
    else if (g_pLog) {
        g_pLog->Error("INVALID MINITORNADO TEMPLATE '%s'", name.m_buf);
    }
}

class GD42e6e0
{
public:
    virtual ~GD42e6e0();
};

__declspec(noinline) GD42e6e0::~GD42e6e0()
{
}

class GD42f370
{
public:
    virtual ~GD42f370();
};

__declspec(noinline) GD42f370::~GD42f370()
{
}

class GD432870
{
public:
    virtual ~GD432870();
};

__declspec(noinline) GD432870::~GD432870()
{
}

class GD4331a0
{
public:
    virtual ~GD4331a0();
};

__declspec(noinline) GD4331a0::~GD4331a0()
{
}

class GD438ff0
{
public:
    virtual ~GD438ff0();
};

__declspec(noinline) GD438ff0::~GD438ff0()
{
}

class GD43b550
{
public:
    virtual ~GD43b550();
};

__declspec(noinline) GD43b550::~GD43b550()
{
}

class GD43b850
{
public:
    virtual ~GD43b850();
};

__declspec(noinline) GD43b850::~GD43b850()
{
}

class GD43bb40
{
public:
    virtual ~GD43bb40();
};

__declspec(noinline) GD43bb40::~GD43bb40()
{
}

class GD43cc30
{
public:
    virtual ~GD43cc30();
};

__declspec(noinline) GD43cc30::~GD43cc30()
{
}

class GD43ea30
{
public:
    virtual ~GD43ea30();
};

__declspec(noinline) GD43ea30::~GD43ea30()
{
}

class GD43f2f0
{
public:
    virtual ~GD43f2f0();
};

__declspec(noinline) GD43f2f0::~GD43f2f0()
{
}

class GD43fad0
{
public:
    virtual ~GD43fad0();
};

__declspec(noinline) GD43fad0::~GD43fad0()
{
}

class GD43fa60
{
public:
    virtual ~GD43fa60();
};

__declspec(noinline) GD43fa60::~GD43fa60()
{
}

struct G2CORE3SCAN
{
    void Scan();
    int Get();
};

void G2CORE3SCAN::Scan()
{
    *(int *)((char *)this + 0xa9c) = 0;
    int i = 7;
    float *p = (float *)((char *)this + 0x84);
    while (i >= 0) {
        if (*p > 0.0f) {
            *(int *)((char *)this + 0xa9c) = i + 1;
            return;
        }
        i--;
        p--;
    }
}

int G2CORE3SCAN::Get()
{
    return *(int *)((char *)this + 0xa9c);
}

class G2GC3STOP : public ACTOR
{
public:
    void StopAndFlag();
};

void G2GC3STOP::StopAndFlag()
{
    ACTOR::Stop();
    *(bool *)((char *)this + 0xcbc) = true;
}

struct G2GC3FLAG
{
    void Set998();
};

void G2GC3FLAG::Set998()
{
    *(bool *)((char *)this + 0x998) = true;
}

class G3K31c
{
public:
    virtual void d0(); virtual void d1(); virtual void d2(); virtual void d3(); virtual void d4(); virtual void d5(); virtual void d6(); virtual void d7();
    virtual void d8(); virtual void d9(); virtual void d10(); virtual void d11(); virtual void d12(); virtual void d13(); virtual void d14(); virtual void d15();
    virtual void d16(); virtual void d17(); virtual void d18(); virtual void d19(); virtual void d20(); virtual void d21(); virtual void d22(); virtual void d23();
    virtual void d24(); virtual void d25(); virtual void d26(); virtual void d27(); virtual void d28(); virtual void d29(); virtual void d30(); virtual void d31();
    virtual void d32(); virtual void d33(); virtual void d34(); virtual void d35(); virtual void d36(); virtual void d37(); virtual void d38(); virtual void d39();
    virtual void d40(); virtual void d41(); virtual void d42(); virtual void d43(); virtual void d44(); virtual void d45(); virtual void d46(); virtual void d47();
    virtual void d48(); virtual void d49(); virtual void d50(); virtual void d51(); virtual void d52(); virtual void d53(); virtual void d54(); virtual void d55();
    virtual void d56(); virtual void d57(); virtual void d58(); virtual void d59(); virtual void d60(); virtual void d61(); virtual void d62(); virtual void d63();
    virtual void d64(); virtual void d65(); virtual void d66(); virtual void d67(); virtual void d68(); virtual void d69(); virtual void d70(); virtual void d71();
    virtual void d72(); virtual void d73(); virtual void d74(); virtual void d75(); virtual void d76(); virtual void d77(); virtual void d78(); virtual void d79();
    virtual void d80(); virtual void d81(); virtual void d82(); virtual void d83(); virtual void d84(); virtual void d85(); virtual void d86(); virtual void d87();
    virtual void d88(); virtual void d89(); virtual void d90(); virtual void d91(); virtual void d92(); virtual void d93(); virtual void d94(); virtual void d95();
    virtual void d96(); virtual void d97(); virtual void d98(); virtual void d99(); virtual void d100(); virtual void d101(); virtual void d102(); virtual void d103();
    virtual void d104(); virtual void d105(); virtual void d106(); virtual void d107(); virtual void d108(); virtual void d109(); virtual void d110(); virtual void d111();
    virtual void d112(); virtual void d113(); virtual void d114(); virtual void d115(); virtual void d116(); virtual void d117(); virtual void d118(); virtual void d119();
    virtual void d120(); virtual void d121(); virtual void d122(); virtual void d123(); virtual void d124(); virtual void d125(); virtual void d126(); virtual void d127();
    virtual void d128(); virtual void d129(); virtual void d130(); virtual void d131(); virtual void d132(); virtual void d133(); virtual void d134(); virtual void d135();
    virtual void d136(); virtual void d137(); virtual void d138(); virtual void d139(); virtual void d140(); virtual void d141(); virtual void d142(); virtual void d143();
    virtual void d144(); virtual void d145(); virtual void d146(); virtual void d147(); virtual void d148(); virtual void d149(); virtual void d150(); virtual void d151();
    virtual void d152(); virtual void d153(); virtual void d154(); virtual void d155(); virtual void d156(); virtual void d157(); virtual void d158(); virtual void d159();
    virtual void d160(); virtual void d161(); virtual void d162(); virtual void d163(); virtual void d164(); virtual void d165(); virtual void d166(); virtual void d167();
    virtual void d168(); virtual void d169(); virtual void d170(); virtual void d171(); virtual void d172(); virtual void d173(); virtual void d174(); virtual void d175();
    virtual void d176(); virtual void d177(); virtual void d178(); virtual void d179(); virtual void d180(); virtual void d181(); virtual void d182(); virtual void d183();
    virtual void d184(); virtual void d185(); virtual void d186(); virtual void d187(); virtual void d188(); virtual void d189(); virtual void d190(); virtual void d191();
    virtual void d192(); virtual void d193(); virtual void d194(); virtual void d195(); virtual void d196(); virtual void d197(); virtual void d198();
    virtual void T();
    void Thunk();
};

void G3K31c::Thunk()
{
    T();
}

class G3K320
{
public:
    virtual void d0(); virtual void d1(); virtual void d2(); virtual void d3(); virtual void d4(); virtual void d5(); virtual void d6(); virtual void d7();
    virtual void d8(); virtual void d9(); virtual void d10(); virtual void d11(); virtual void d12(); virtual void d13(); virtual void d14(); virtual void d15();
    virtual void d16(); virtual void d17(); virtual void d18(); virtual void d19(); virtual void d20(); virtual void d21(); virtual void d22(); virtual void d23();
    virtual void d24(); virtual void d25(); virtual void d26(); virtual void d27(); virtual void d28(); virtual void d29(); virtual void d30(); virtual void d31();
    virtual void d32(); virtual void d33(); virtual void d34(); virtual void d35(); virtual void d36(); virtual void d37(); virtual void d38(); virtual void d39();
    virtual void d40(); virtual void d41(); virtual void d42(); virtual void d43(); virtual void d44(); virtual void d45(); virtual void d46(); virtual void d47();
    virtual void d48(); virtual void d49(); virtual void d50(); virtual void d51(); virtual void d52(); virtual void d53(); virtual void d54(); virtual void d55();
    virtual void d56(); virtual void d57(); virtual void d58(); virtual void d59(); virtual void d60(); virtual void d61(); virtual void d62(); virtual void d63();
    virtual void d64(); virtual void d65(); virtual void d66(); virtual void d67(); virtual void d68(); virtual void d69(); virtual void d70(); virtual void d71();
    virtual void d72(); virtual void d73(); virtual void d74(); virtual void d75(); virtual void d76(); virtual void d77(); virtual void d78(); virtual void d79();
    virtual void d80(); virtual void d81(); virtual void d82(); virtual void d83(); virtual void d84(); virtual void d85(); virtual void d86(); virtual void d87();
    virtual void d88(); virtual void d89(); virtual void d90(); virtual void d91(); virtual void d92(); virtual void d93(); virtual void d94(); virtual void d95();
    virtual void d96(); virtual void d97(); virtual void d98(); virtual void d99(); virtual void d100(); virtual void d101(); virtual void d102(); virtual void d103();
    virtual void d104(); virtual void d105(); virtual void d106(); virtual void d107(); virtual void d108(); virtual void d109(); virtual void d110(); virtual void d111();
    virtual void d112(); virtual void d113(); virtual void d114(); virtual void d115(); virtual void d116(); virtual void d117(); virtual void d118(); virtual void d119();
    virtual void d120(); virtual void d121(); virtual void d122(); virtual void d123(); virtual void d124(); virtual void d125(); virtual void d126(); virtual void d127();
    virtual void d128(); virtual void d129(); virtual void d130(); virtual void d131(); virtual void d132(); virtual void d133(); virtual void d134(); virtual void d135();
    virtual void d136(); virtual void d137(); virtual void d138(); virtual void d139(); virtual void d140(); virtual void d141(); virtual void d142(); virtual void d143();
    virtual void d144(); virtual void d145(); virtual void d146(); virtual void d147(); virtual void d148(); virtual void d149(); virtual void d150(); virtual void d151();
    virtual void d152(); virtual void d153(); virtual void d154(); virtual void d155(); virtual void d156(); virtual void d157(); virtual void d158(); virtual void d159();
    virtual void d160(); virtual void d161(); virtual void d162(); virtual void d163(); virtual void d164(); virtual void d165(); virtual void d166(); virtual void d167();
    virtual void d168(); virtual void d169(); virtual void d170(); virtual void d171(); virtual void d172(); virtual void d173(); virtual void d174(); virtual void d175();
    virtual void d176(); virtual void d177(); virtual void d178(); virtual void d179(); virtual void d180(); virtual void d181(); virtual void d182(); virtual void d183();
    virtual void d184(); virtual void d185(); virtual void d186(); virtual void d187(); virtual void d188(); virtual void d189(); virtual void d190(); virtual void d191();
    virtual void d192(); virtual void d193(); virtual void d194(); virtual void d195(); virtual void d196(); virtual void d197(); virtual void d198(); virtual void d199();
    virtual void T();
    void Thunk();
};

void G3K320::Thunk()
{
    T();
}

class G3K324
{
public:
    virtual void d0(); virtual void d1(); virtual void d2(); virtual void d3(); virtual void d4(); virtual void d5(); virtual void d6(); virtual void d7();
    virtual void d8(); virtual void d9(); virtual void d10(); virtual void d11(); virtual void d12(); virtual void d13(); virtual void d14(); virtual void d15();
    virtual void d16(); virtual void d17(); virtual void d18(); virtual void d19(); virtual void d20(); virtual void d21(); virtual void d22(); virtual void d23();
    virtual void d24(); virtual void d25(); virtual void d26(); virtual void d27(); virtual void d28(); virtual void d29(); virtual void d30(); virtual void d31();
    virtual void d32(); virtual void d33(); virtual void d34(); virtual void d35(); virtual void d36(); virtual void d37(); virtual void d38(); virtual void d39();
    virtual void d40(); virtual void d41(); virtual void d42(); virtual void d43(); virtual void d44(); virtual void d45(); virtual void d46(); virtual void d47();
    virtual void d48(); virtual void d49(); virtual void d50(); virtual void d51(); virtual void d52(); virtual void d53(); virtual void d54(); virtual void d55();
    virtual void d56(); virtual void d57(); virtual void d58(); virtual void d59(); virtual void d60(); virtual void d61(); virtual void d62(); virtual void d63();
    virtual void d64(); virtual void d65(); virtual void d66(); virtual void d67(); virtual void d68(); virtual void d69(); virtual void d70(); virtual void d71();
    virtual void d72(); virtual void d73(); virtual void d74(); virtual void d75(); virtual void d76(); virtual void d77(); virtual void d78(); virtual void d79();
    virtual void d80(); virtual void d81(); virtual void d82(); virtual void d83(); virtual void d84(); virtual void d85(); virtual void d86(); virtual void d87();
    virtual void d88(); virtual void d89(); virtual void d90(); virtual void d91(); virtual void d92(); virtual void d93(); virtual void d94(); virtual void d95();
    virtual void d96(); virtual void d97(); virtual void d98(); virtual void d99(); virtual void d100(); virtual void d101(); virtual void d102(); virtual void d103();
    virtual void d104(); virtual void d105(); virtual void d106(); virtual void d107(); virtual void d108(); virtual void d109(); virtual void d110(); virtual void d111();
    virtual void d112(); virtual void d113(); virtual void d114(); virtual void d115(); virtual void d116(); virtual void d117(); virtual void d118(); virtual void d119();
    virtual void d120(); virtual void d121(); virtual void d122(); virtual void d123(); virtual void d124(); virtual void d125(); virtual void d126(); virtual void d127();
    virtual void d128(); virtual void d129(); virtual void d130(); virtual void d131(); virtual void d132(); virtual void d133(); virtual void d134(); virtual void d135();
    virtual void d136(); virtual void d137(); virtual void d138(); virtual void d139(); virtual void d140(); virtual void d141(); virtual void d142(); virtual void d143();
    virtual void d144(); virtual void d145(); virtual void d146(); virtual void d147(); virtual void d148(); virtual void d149(); virtual void d150(); virtual void d151();
    virtual void d152(); virtual void d153(); virtual void d154(); virtual void d155(); virtual void d156(); virtual void d157(); virtual void d158(); virtual void d159();
    virtual void d160(); virtual void d161(); virtual void d162(); virtual void d163(); virtual void d164(); virtual void d165(); virtual void d166(); virtual void d167();
    virtual void d168(); virtual void d169(); virtual void d170(); virtual void d171(); virtual void d172(); virtual void d173(); virtual void d174(); virtual void d175();
    virtual void d176(); virtual void d177(); virtual void d178(); virtual void d179(); virtual void d180(); virtual void d181(); virtual void d182(); virtual void d183();
    virtual void d184(); virtual void d185(); virtual void d186(); virtual void d187(); virtual void d188(); virtual void d189(); virtual void d190(); virtual void d191();
    virtual void d192(); virtual void d193(); virtual void d194(); virtual void d195(); virtual void d196(); virtual void d197(); virtual void d198(); virtual void d199();
    virtual void d200();
    virtual void T();
    void Thunk();
};

void G3K324::Thunk()
{
    T();
}

class G3K328
{
public:
    virtual void d0(); virtual void d1(); virtual void d2(); virtual void d3(); virtual void d4(); virtual void d5(); virtual void d6(); virtual void d7();
    virtual void d8(); virtual void d9(); virtual void d10(); virtual void d11(); virtual void d12(); virtual void d13(); virtual void d14(); virtual void d15();
    virtual void d16(); virtual void d17(); virtual void d18(); virtual void d19(); virtual void d20(); virtual void d21(); virtual void d22(); virtual void d23();
    virtual void d24(); virtual void d25(); virtual void d26(); virtual void d27(); virtual void d28(); virtual void d29(); virtual void d30(); virtual void d31();
    virtual void d32(); virtual void d33(); virtual void d34(); virtual void d35(); virtual void d36(); virtual void d37(); virtual void d38(); virtual void d39();
    virtual void d40(); virtual void d41(); virtual void d42(); virtual void d43(); virtual void d44(); virtual void d45(); virtual void d46(); virtual void d47();
    virtual void d48(); virtual void d49(); virtual void d50(); virtual void d51(); virtual void d52(); virtual void d53(); virtual void d54(); virtual void d55();
    virtual void d56(); virtual void d57(); virtual void d58(); virtual void d59(); virtual void d60(); virtual void d61(); virtual void d62(); virtual void d63();
    virtual void d64(); virtual void d65(); virtual void d66(); virtual void d67(); virtual void d68(); virtual void d69(); virtual void d70(); virtual void d71();
    virtual void d72(); virtual void d73(); virtual void d74(); virtual void d75(); virtual void d76(); virtual void d77(); virtual void d78(); virtual void d79();
    virtual void d80(); virtual void d81(); virtual void d82(); virtual void d83(); virtual void d84(); virtual void d85(); virtual void d86(); virtual void d87();
    virtual void d88(); virtual void d89(); virtual void d90(); virtual void d91(); virtual void d92(); virtual void d93(); virtual void d94(); virtual void d95();
    virtual void d96(); virtual void d97(); virtual void d98(); virtual void d99(); virtual void d100(); virtual void d101(); virtual void d102(); virtual void d103();
    virtual void d104(); virtual void d105(); virtual void d106(); virtual void d107(); virtual void d108(); virtual void d109(); virtual void d110(); virtual void d111();
    virtual void d112(); virtual void d113(); virtual void d114(); virtual void d115(); virtual void d116(); virtual void d117(); virtual void d118(); virtual void d119();
    virtual void d120(); virtual void d121(); virtual void d122(); virtual void d123(); virtual void d124(); virtual void d125(); virtual void d126(); virtual void d127();
    virtual void d128(); virtual void d129(); virtual void d130(); virtual void d131(); virtual void d132(); virtual void d133(); virtual void d134(); virtual void d135();
    virtual void d136(); virtual void d137(); virtual void d138(); virtual void d139(); virtual void d140(); virtual void d141(); virtual void d142(); virtual void d143();
    virtual void d144(); virtual void d145(); virtual void d146(); virtual void d147(); virtual void d148(); virtual void d149(); virtual void d150(); virtual void d151();
    virtual void d152(); virtual void d153(); virtual void d154(); virtual void d155(); virtual void d156(); virtual void d157(); virtual void d158(); virtual void d159();
    virtual void d160(); virtual void d161(); virtual void d162(); virtual void d163(); virtual void d164(); virtual void d165(); virtual void d166(); virtual void d167();
    virtual void d168(); virtual void d169(); virtual void d170(); virtual void d171(); virtual void d172(); virtual void d173(); virtual void d174(); virtual void d175();
    virtual void d176(); virtual void d177(); virtual void d178(); virtual void d179(); virtual void d180(); virtual void d181(); virtual void d182(); virtual void d183();
    virtual void d184(); virtual void d185(); virtual void d186(); virtual void d187(); virtual void d188(); virtual void d189(); virtual void d190(); virtual void d191();
    virtual void d192(); virtual void d193(); virtual void d194(); virtual void d195(); virtual void d196(); virtual void d197(); virtual void d198(); virtual void d199();
    virtual void d200(); virtual void d201();
    virtual void T();
    void Thunk();
};

void G3K328::Thunk()
{
    T();
}

class G3K15c
{
public:
    virtual void d0(); virtual void d1(); virtual void d2(); virtual void d3(); virtual void d4(); virtual void d5(); virtual void d6(); virtual void d7();
    virtual void d8(); virtual void d9(); virtual void d10(); virtual void d11(); virtual void d12(); virtual void d13(); virtual void d14(); virtual void d15();
    virtual void d16(); virtual void d17(); virtual void d18(); virtual void d19(); virtual void d20(); virtual void d21(); virtual void d22(); virtual void d23();
    virtual void d24(); virtual void d25(); virtual void d26(); virtual void d27(); virtual void d28(); virtual void d29(); virtual void d30(); virtual void d31();
    virtual void d32(); virtual void d33(); virtual void d34(); virtual void d35(); virtual void d36(); virtual void d37(); virtual void d38(); virtual void d39();
    virtual void d40(); virtual void d41(); virtual void d42(); virtual void d43(); virtual void d44(); virtual void d45(); virtual void d46(); virtual void d47();
    virtual void d48(); virtual void d49(); virtual void d50(); virtual void d51(); virtual void d52(); virtual void d53(); virtual void d54(); virtual void d55();
    virtual void d56(); virtual void d57(); virtual void d58(); virtual void d59(); virtual void d60(); virtual void d61(); virtual void d62(); virtual void d63();
    virtual void d64(); virtual void d65(); virtual void d66(); virtual void d67(); virtual void d68(); virtual void d69(); virtual void d70(); virtual void d71();
    virtual void d72(); virtual void d73(); virtual void d74(); virtual void d75(); virtual void d76(); virtual void d77(); virtual void d78(); virtual void d79();
    virtual void d80(); virtual void d81(); virtual void d82(); virtual void d83(); virtual void d84(); virtual void d85(); virtual void d86();
    virtual void T();
    void Thunk();
};

void G3K15c::Thunk()
{
    T();
}

class G3K13c
{
public:
    virtual void d0(); virtual void d1(); virtual void d2(); virtual void d3(); virtual void d4(); virtual void d5(); virtual void d6(); virtual void d7();
    virtual void d8(); virtual void d9(); virtual void d10(); virtual void d11(); virtual void d12(); virtual void d13(); virtual void d14(); virtual void d15();
    virtual void d16(); virtual void d17(); virtual void d18(); virtual void d19(); virtual void d20(); virtual void d21(); virtual void d22(); virtual void d23();
    virtual void d24(); virtual void d25(); virtual void d26(); virtual void d27(); virtual void d28(); virtual void d29(); virtual void d30(); virtual void d31();
    virtual void d32(); virtual void d33(); virtual void d34(); virtual void d35(); virtual void d36(); virtual void d37(); virtual void d38(); virtual void d39();
    virtual void d40(); virtual void d41(); virtual void d42(); virtual void d43(); virtual void d44(); virtual void d45(); virtual void d46(); virtual void d47();
    virtual void d48(); virtual void d49(); virtual void d50(); virtual void d51(); virtual void d52(); virtual void d53(); virtual void d54(); virtual void d55();
    virtual void d56(); virtual void d57(); virtual void d58(); virtual void d59(); virtual void d60(); virtual void d61(); virtual void d62(); virtual void d63();
    virtual void d64(); virtual void d65(); virtual void d66(); virtual void d67(); virtual void d68(); virtual void d69(); virtual void d70(); virtual void d71();
    virtual void d72(); virtual void d73(); virtual void d74(); virtual void d75(); virtual void d76(); virtual void d77(); virtual void d78();
    virtual void T();
    void Thunk();
};

void G3K13c::Thunk()
{
    T();
}

class G3K160
{
public:
    virtual void d0(); virtual void d1(); virtual void d2(); virtual void d3(); virtual void d4(); virtual void d5(); virtual void d6(); virtual void d7();
    virtual void d8(); virtual void d9(); virtual void d10(); virtual void d11(); virtual void d12(); virtual void d13(); virtual void d14(); virtual void d15();
    virtual void d16(); virtual void d17(); virtual void d18(); virtual void d19(); virtual void d20(); virtual void d21(); virtual void d22(); virtual void d23();
    virtual void d24(); virtual void d25(); virtual void d26(); virtual void d27(); virtual void d28(); virtual void d29(); virtual void d30(); virtual void d31();
    virtual void d32(); virtual void d33(); virtual void d34(); virtual void d35(); virtual void d36(); virtual void d37(); virtual void d38(); virtual void d39();
    virtual void d40(); virtual void d41(); virtual void d42(); virtual void d43(); virtual void d44(); virtual void d45(); virtual void d46(); virtual void d47();
    virtual void d48(); virtual void d49(); virtual void d50(); virtual void d51(); virtual void d52(); virtual void d53(); virtual void d54(); virtual void d55();
    virtual void d56(); virtual void d57(); virtual void d58(); virtual void d59(); virtual void d60(); virtual void d61(); virtual void d62(); virtual void d63();
    virtual void d64(); virtual void d65(); virtual void d66(); virtual void d67(); virtual void d68(); virtual void d69(); virtual void d70(); virtual void d71();
    virtual void d72(); virtual void d73(); virtual void d74(); virtual void d75(); virtual void d76(); virtual void d77(); virtual void d78(); virtual void d79();
    virtual void d80(); virtual void d81(); virtual void d82(); virtual void d83(); virtual void d84(); virtual void d85(); virtual void d86(); virtual void d87();
    virtual void T();
    void Thunk();
};

void G3K160::Thunk()
{
    T();
}

class G3K140
{
public:
    virtual void d0(); virtual void d1(); virtual void d2(); virtual void d3(); virtual void d4(); virtual void d5(); virtual void d6(); virtual void d7();
    virtual void d8(); virtual void d9(); virtual void d10(); virtual void d11(); virtual void d12(); virtual void d13(); virtual void d14(); virtual void d15();
    virtual void d16(); virtual void d17(); virtual void d18(); virtual void d19(); virtual void d20(); virtual void d21(); virtual void d22(); virtual void d23();
    virtual void d24(); virtual void d25(); virtual void d26(); virtual void d27(); virtual void d28(); virtual void d29(); virtual void d30(); virtual void d31();
    virtual void d32(); virtual void d33(); virtual void d34(); virtual void d35(); virtual void d36(); virtual void d37(); virtual void d38(); virtual void d39();
    virtual void d40(); virtual void d41(); virtual void d42(); virtual void d43(); virtual void d44(); virtual void d45(); virtual void d46(); virtual void d47();
    virtual void d48(); virtual void d49(); virtual void d50(); virtual void d51(); virtual void d52(); virtual void d53(); virtual void d54(); virtual void d55();
    virtual void d56(); virtual void d57(); virtual void d58(); virtual void d59(); virtual void d60(); virtual void d61(); virtual void d62(); virtual void d63();
    virtual void d64(); virtual void d65(); virtual void d66(); virtual void d67(); virtual void d68(); virtual void d69(); virtual void d70(); virtual void d71();
    virtual void d72(); virtual void d73(); virtual void d74(); virtual void d75(); virtual void d76(); virtual void d77(); virtual void d78(); virtual void d79();
    virtual void T();
    void Thunk();
};

void G3K140::Thunk()
{
    T();
}

class G3K164
{
public:
    virtual void d0(); virtual void d1(); virtual void d2(); virtual void d3(); virtual void d4(); virtual void d5(); virtual void d6(); virtual void d7();
    virtual void d8(); virtual void d9(); virtual void d10(); virtual void d11(); virtual void d12(); virtual void d13(); virtual void d14(); virtual void d15();
    virtual void d16(); virtual void d17(); virtual void d18(); virtual void d19(); virtual void d20(); virtual void d21(); virtual void d22(); virtual void d23();
    virtual void d24(); virtual void d25(); virtual void d26(); virtual void d27(); virtual void d28(); virtual void d29(); virtual void d30(); virtual void d31();
    virtual void d32(); virtual void d33(); virtual void d34(); virtual void d35(); virtual void d36(); virtual void d37(); virtual void d38(); virtual void d39();
    virtual void d40(); virtual void d41(); virtual void d42(); virtual void d43(); virtual void d44(); virtual void d45(); virtual void d46(); virtual void d47();
    virtual void d48(); virtual void d49(); virtual void d50(); virtual void d51(); virtual void d52(); virtual void d53(); virtual void d54(); virtual void d55();
    virtual void d56(); virtual void d57(); virtual void d58(); virtual void d59(); virtual void d60(); virtual void d61(); virtual void d62(); virtual void d63();
    virtual void d64(); virtual void d65(); virtual void d66(); virtual void d67(); virtual void d68(); virtual void d69(); virtual void d70(); virtual void d71();
    virtual void d72(); virtual void d73(); virtual void d74(); virtual void d75(); virtual void d76(); virtual void d77(); virtual void d78(); virtual void d79();
    virtual void d80(); virtual void d81(); virtual void d82(); virtual void d83(); virtual void d84(); virtual void d85(); virtual void d86(); virtual void d87();
    virtual void d88();
    virtual void T();
    void Thunk();
};

void G3K164::Thunk()
{
    T();
}

class G3K144
{
public:
    virtual void d0(); virtual void d1(); virtual void d2(); virtual void d3(); virtual void d4(); virtual void d5(); virtual void d6(); virtual void d7();
    virtual void d8(); virtual void d9(); virtual void d10(); virtual void d11(); virtual void d12(); virtual void d13(); virtual void d14(); virtual void d15();
    virtual void d16(); virtual void d17(); virtual void d18(); virtual void d19(); virtual void d20(); virtual void d21(); virtual void d22(); virtual void d23();
    virtual void d24(); virtual void d25(); virtual void d26(); virtual void d27(); virtual void d28(); virtual void d29(); virtual void d30(); virtual void d31();
    virtual void d32(); virtual void d33(); virtual void d34(); virtual void d35(); virtual void d36(); virtual void d37(); virtual void d38(); virtual void d39();
    virtual void d40(); virtual void d41(); virtual void d42(); virtual void d43(); virtual void d44(); virtual void d45(); virtual void d46(); virtual void d47();
    virtual void d48(); virtual void d49(); virtual void d50(); virtual void d51(); virtual void d52(); virtual void d53(); virtual void d54(); virtual void d55();
    virtual void d56(); virtual void d57(); virtual void d58(); virtual void d59(); virtual void d60(); virtual void d61(); virtual void d62(); virtual void d63();
    virtual void d64(); virtual void d65(); virtual void d66(); virtual void d67(); virtual void d68(); virtual void d69(); virtual void d70(); virtual void d71();
    virtual void d72(); virtual void d73(); virtual void d74(); virtual void d75(); virtual void d76(); virtual void d77(); virtual void d78(); virtual void d79();
    virtual void d80();
    virtual void T();
    void Thunk();
};

void G3K144::Thunk()
{
    T();
}

class G3K124
{
public:
    virtual void d0(); virtual void d1(); virtual void d2(); virtual void d3(); virtual void d4(); virtual void d5(); virtual void d6(); virtual void d7();
    virtual void d8(); virtual void d9(); virtual void d10(); virtual void d11(); virtual void d12(); virtual void d13(); virtual void d14(); virtual void d15();
    virtual void d16(); virtual void d17(); virtual void d18(); virtual void d19(); virtual void d20(); virtual void d21(); virtual void d22(); virtual void d23();
    virtual void d24(); virtual void d25(); virtual void d26(); virtual void d27(); virtual void d28(); virtual void d29(); virtual void d30(); virtual void d31();
    virtual void d32(); virtual void d33(); virtual void d34(); virtual void d35(); virtual void d36(); virtual void d37(); virtual void d38(); virtual void d39();
    virtual void d40(); virtual void d41(); virtual void d42(); virtual void d43(); virtual void d44(); virtual void d45(); virtual void d46(); virtual void d47();
    virtual void d48(); virtual void d49(); virtual void d50(); virtual void d51(); virtual void d52(); virtual void d53(); virtual void d54(); virtual void d55();
    virtual void d56(); virtual void d57(); virtual void d58(); virtual void d59(); virtual void d60(); virtual void d61(); virtual void d62(); virtual void d63();
    virtual void d64(); virtual void d65(); virtual void d66(); virtual void d67(); virtual void d68(); virtual void d69(); virtual void d70(); virtual void d71();
    virtual void d72();
    virtual void T();
    void Thunk();
};

void G3K124::Thunk()
{
    T();
}

class G3K148
{
public:
    virtual void d0(); virtual void d1(); virtual void d2(); virtual void d3(); virtual void d4(); virtual void d5(); virtual void d6(); virtual void d7();
    virtual void d8(); virtual void d9(); virtual void d10(); virtual void d11(); virtual void d12(); virtual void d13(); virtual void d14(); virtual void d15();
    virtual void d16(); virtual void d17(); virtual void d18(); virtual void d19(); virtual void d20(); virtual void d21(); virtual void d22(); virtual void d23();
    virtual void d24(); virtual void d25(); virtual void d26(); virtual void d27(); virtual void d28(); virtual void d29(); virtual void d30(); virtual void d31();
    virtual void d32(); virtual void d33(); virtual void d34(); virtual void d35(); virtual void d36(); virtual void d37(); virtual void d38(); virtual void d39();
    virtual void d40(); virtual void d41(); virtual void d42(); virtual void d43(); virtual void d44(); virtual void d45(); virtual void d46(); virtual void d47();
    virtual void d48(); virtual void d49(); virtual void d50(); virtual void d51(); virtual void d52(); virtual void d53(); virtual void d54(); virtual void d55();
    virtual void d56(); virtual void d57(); virtual void d58(); virtual void d59(); virtual void d60(); virtual void d61(); virtual void d62(); virtual void d63();
    virtual void d64(); virtual void d65(); virtual void d66(); virtual void d67(); virtual void d68(); virtual void d69(); virtual void d70(); virtual void d71();
    virtual void d72(); virtual void d73(); virtual void d74(); virtual void d75(); virtual void d76(); virtual void d77(); virtual void d78(); virtual void d79();
    virtual void d80(); virtual void d81();
    virtual void T();
    void Thunk();
};

void G3K148::Thunk()
{
    T();
}

class G3K128
{
public:
    virtual void d0(); virtual void d1(); virtual void d2(); virtual void d3(); virtual void d4(); virtual void d5(); virtual void d6(); virtual void d7();
    virtual void d8(); virtual void d9(); virtual void d10(); virtual void d11(); virtual void d12(); virtual void d13(); virtual void d14(); virtual void d15();
    virtual void d16(); virtual void d17(); virtual void d18(); virtual void d19(); virtual void d20(); virtual void d21(); virtual void d22(); virtual void d23();
    virtual void d24(); virtual void d25(); virtual void d26(); virtual void d27(); virtual void d28(); virtual void d29(); virtual void d30(); virtual void d31();
    virtual void d32(); virtual void d33(); virtual void d34(); virtual void d35(); virtual void d36(); virtual void d37(); virtual void d38(); virtual void d39();
    virtual void d40(); virtual void d41(); virtual void d42(); virtual void d43(); virtual void d44(); virtual void d45(); virtual void d46(); virtual void d47();
    virtual void d48(); virtual void d49(); virtual void d50(); virtual void d51(); virtual void d52(); virtual void d53(); virtual void d54(); virtual void d55();
    virtual void d56(); virtual void d57(); virtual void d58(); virtual void d59(); virtual void d60(); virtual void d61(); virtual void d62(); virtual void d63();
    virtual void d64(); virtual void d65(); virtual void d66(); virtual void d67(); virtual void d68(); virtual void d69(); virtual void d70(); virtual void d71();
    virtual void d72(); virtual void d73();
    virtual void T();
    void Thunk();
};

void G3K128::Thunk()
{
    T();
}

class G3K14c
{
public:
    virtual void d0(); virtual void d1(); virtual void d2(); virtual void d3(); virtual void d4(); virtual void d5(); virtual void d6(); virtual void d7();
    virtual void d8(); virtual void d9(); virtual void d10(); virtual void d11(); virtual void d12(); virtual void d13(); virtual void d14(); virtual void d15();
    virtual void d16(); virtual void d17(); virtual void d18(); virtual void d19(); virtual void d20(); virtual void d21(); virtual void d22(); virtual void d23();
    virtual void d24(); virtual void d25(); virtual void d26(); virtual void d27(); virtual void d28(); virtual void d29(); virtual void d30(); virtual void d31();
    virtual void d32(); virtual void d33(); virtual void d34(); virtual void d35(); virtual void d36(); virtual void d37(); virtual void d38(); virtual void d39();
    virtual void d40(); virtual void d41(); virtual void d42(); virtual void d43(); virtual void d44(); virtual void d45(); virtual void d46(); virtual void d47();
    virtual void d48(); virtual void d49(); virtual void d50(); virtual void d51(); virtual void d52(); virtual void d53(); virtual void d54(); virtual void d55();
    virtual void d56(); virtual void d57(); virtual void d58(); virtual void d59(); virtual void d60(); virtual void d61(); virtual void d62(); virtual void d63();
    virtual void d64(); virtual void d65(); virtual void d66(); virtual void d67(); virtual void d68(); virtual void d69(); virtual void d70(); virtual void d71();
    virtual void d72(); virtual void d73(); virtual void d74(); virtual void d75(); virtual void d76(); virtual void d77(); virtual void d78(); virtual void d79();
    virtual void d80(); virtual void d81(); virtual void d82();
    virtual void T();
    void Thunk();
};

void G3K14c::Thunk()
{
    T();
}

class G3K12c
{
public:
    virtual void d0(); virtual void d1(); virtual void d2(); virtual void d3(); virtual void d4(); virtual void d5(); virtual void d6(); virtual void d7();
    virtual void d8(); virtual void d9(); virtual void d10(); virtual void d11(); virtual void d12(); virtual void d13(); virtual void d14(); virtual void d15();
    virtual void d16(); virtual void d17(); virtual void d18(); virtual void d19(); virtual void d20(); virtual void d21(); virtual void d22(); virtual void d23();
    virtual void d24(); virtual void d25(); virtual void d26(); virtual void d27(); virtual void d28(); virtual void d29(); virtual void d30(); virtual void d31();
    virtual void d32(); virtual void d33(); virtual void d34(); virtual void d35(); virtual void d36(); virtual void d37(); virtual void d38(); virtual void d39();
    virtual void d40(); virtual void d41(); virtual void d42(); virtual void d43(); virtual void d44(); virtual void d45(); virtual void d46(); virtual void d47();
    virtual void d48(); virtual void d49(); virtual void d50(); virtual void d51(); virtual void d52(); virtual void d53(); virtual void d54(); virtual void d55();
    virtual void d56(); virtual void d57(); virtual void d58(); virtual void d59(); virtual void d60(); virtual void d61(); virtual void d62(); virtual void d63();
    virtual void d64(); virtual void d65(); virtual void d66(); virtual void d67(); virtual void d68(); virtual void d69(); virtual void d70(); virtual void d71();
    virtual void d72(); virtual void d73(); virtual void d74();
    virtual void T();
    void Thunk();
};

void G3K12c::Thunk()
{
    T();
}

class G3K150
{
public:
    virtual void d0(); virtual void d1(); virtual void d2(); virtual void d3(); virtual void d4(); virtual void d5(); virtual void d6(); virtual void d7();
    virtual void d8(); virtual void d9(); virtual void d10(); virtual void d11(); virtual void d12(); virtual void d13(); virtual void d14(); virtual void d15();
    virtual void d16(); virtual void d17(); virtual void d18(); virtual void d19(); virtual void d20(); virtual void d21(); virtual void d22(); virtual void d23();
    virtual void d24(); virtual void d25(); virtual void d26(); virtual void d27(); virtual void d28(); virtual void d29(); virtual void d30(); virtual void d31();
    virtual void d32(); virtual void d33(); virtual void d34(); virtual void d35(); virtual void d36(); virtual void d37(); virtual void d38(); virtual void d39();
    virtual void d40(); virtual void d41(); virtual void d42(); virtual void d43(); virtual void d44(); virtual void d45(); virtual void d46(); virtual void d47();
    virtual void d48(); virtual void d49(); virtual void d50(); virtual void d51(); virtual void d52(); virtual void d53(); virtual void d54(); virtual void d55();
    virtual void d56(); virtual void d57(); virtual void d58(); virtual void d59(); virtual void d60(); virtual void d61(); virtual void d62(); virtual void d63();
    virtual void d64(); virtual void d65(); virtual void d66(); virtual void d67(); virtual void d68(); virtual void d69(); virtual void d70(); virtual void d71();
    virtual void d72(); virtual void d73(); virtual void d74(); virtual void d75(); virtual void d76(); virtual void d77(); virtual void d78(); virtual void d79();
    virtual void d80(); virtual void d81(); virtual void d82(); virtual void d83();
    virtual void T();
    void Thunk();
};

void G3K150::Thunk()
{
    T();
}

class G3K154
{
public:
    virtual void d0(); virtual void d1(); virtual void d2(); virtual void d3(); virtual void d4(); virtual void d5(); virtual void d6(); virtual void d7();
    virtual void d8(); virtual void d9(); virtual void d10(); virtual void d11(); virtual void d12(); virtual void d13(); virtual void d14(); virtual void d15();
    virtual void d16(); virtual void d17(); virtual void d18(); virtual void d19(); virtual void d20(); virtual void d21(); virtual void d22(); virtual void d23();
    virtual void d24(); virtual void d25(); virtual void d26(); virtual void d27(); virtual void d28(); virtual void d29(); virtual void d30(); virtual void d31();
    virtual void d32(); virtual void d33(); virtual void d34(); virtual void d35(); virtual void d36(); virtual void d37(); virtual void d38(); virtual void d39();
    virtual void d40(); virtual void d41(); virtual void d42(); virtual void d43(); virtual void d44(); virtual void d45(); virtual void d46(); virtual void d47();
    virtual void d48(); virtual void d49(); virtual void d50(); virtual void d51(); virtual void d52(); virtual void d53(); virtual void d54(); virtual void d55();
    virtual void d56(); virtual void d57(); virtual void d58(); virtual void d59(); virtual void d60(); virtual void d61(); virtual void d62(); virtual void d63();
    virtual void d64(); virtual void d65(); virtual void d66(); virtual void d67(); virtual void d68(); virtual void d69(); virtual void d70(); virtual void d71();
    virtual void d72(); virtual void d73(); virtual void d74(); virtual void d75(); virtual void d76(); virtual void d77(); virtual void d78(); virtual void d79();
    virtual void d80(); virtual void d81(); virtual void d82(); virtual void d83(); virtual void d84();
    virtual void T();
    void Thunk();
};

void G3K154::Thunk()
{
    T();
}

class G3K134
{
public:
    virtual void d0(); virtual void d1(); virtual void d2(); virtual void d3(); virtual void d4(); virtual void d5(); virtual void d6(); virtual void d7();
    virtual void d8(); virtual void d9(); virtual void d10(); virtual void d11(); virtual void d12(); virtual void d13(); virtual void d14(); virtual void d15();
    virtual void d16(); virtual void d17(); virtual void d18(); virtual void d19(); virtual void d20(); virtual void d21(); virtual void d22(); virtual void d23();
    virtual void d24(); virtual void d25(); virtual void d26(); virtual void d27(); virtual void d28(); virtual void d29(); virtual void d30(); virtual void d31();
    virtual void d32(); virtual void d33(); virtual void d34(); virtual void d35(); virtual void d36(); virtual void d37(); virtual void d38(); virtual void d39();
    virtual void d40(); virtual void d41(); virtual void d42(); virtual void d43(); virtual void d44(); virtual void d45(); virtual void d46(); virtual void d47();
    virtual void d48(); virtual void d49(); virtual void d50(); virtual void d51(); virtual void d52(); virtual void d53(); virtual void d54(); virtual void d55();
    virtual void d56(); virtual void d57(); virtual void d58(); virtual void d59(); virtual void d60(); virtual void d61(); virtual void d62(); virtual void d63();
    virtual void d64(); virtual void d65(); virtual void d66(); virtual void d67(); virtual void d68(); virtual void d69(); virtual void d70(); virtual void d71();
    virtual void d72(); virtual void d73(); virtual void d74(); virtual void d75(); virtual void d76();
    virtual void T();
    void Thunk();
};

void G3K134::Thunk()
{
    T();
}

class G3K158
{
public:
    virtual void d0(); virtual void d1(); virtual void d2(); virtual void d3(); virtual void d4(); virtual void d5(); virtual void d6(); virtual void d7();
    virtual void d8(); virtual void d9(); virtual void d10(); virtual void d11(); virtual void d12(); virtual void d13(); virtual void d14(); virtual void d15();
    virtual void d16(); virtual void d17(); virtual void d18(); virtual void d19(); virtual void d20(); virtual void d21(); virtual void d22(); virtual void d23();
    virtual void d24(); virtual void d25(); virtual void d26(); virtual void d27(); virtual void d28(); virtual void d29(); virtual void d30(); virtual void d31();
    virtual void d32(); virtual void d33(); virtual void d34(); virtual void d35(); virtual void d36(); virtual void d37(); virtual void d38(); virtual void d39();
    virtual void d40(); virtual void d41(); virtual void d42(); virtual void d43(); virtual void d44(); virtual void d45(); virtual void d46(); virtual void d47();
    virtual void d48(); virtual void d49(); virtual void d50(); virtual void d51(); virtual void d52(); virtual void d53(); virtual void d54(); virtual void d55();
    virtual void d56(); virtual void d57(); virtual void d58(); virtual void d59(); virtual void d60(); virtual void d61(); virtual void d62(); virtual void d63();
    virtual void d64(); virtual void d65(); virtual void d66(); virtual void d67(); virtual void d68(); virtual void d69(); virtual void d70(); virtual void d71();
    virtual void d72(); virtual void d73(); virtual void d74(); virtual void d75(); virtual void d76(); virtual void d77(); virtual void d78(); virtual void d79();
    virtual void d80(); virtual void d81(); virtual void d82(); virtual void d83(); virtual void d84(); virtual void d85();
    virtual void T();
    void Thunk();
};

void G3K158::Thunk()
{
    T();
}

class G3K138
{
public:
    virtual void d0(); virtual void d1(); virtual void d2(); virtual void d3(); virtual void d4(); virtual void d5(); virtual void d6(); virtual void d7();
    virtual void d8(); virtual void d9(); virtual void d10(); virtual void d11(); virtual void d12(); virtual void d13(); virtual void d14(); virtual void d15();
    virtual void d16(); virtual void d17(); virtual void d18(); virtual void d19(); virtual void d20(); virtual void d21(); virtual void d22(); virtual void d23();
    virtual void d24(); virtual void d25(); virtual void d26(); virtual void d27(); virtual void d28(); virtual void d29(); virtual void d30(); virtual void d31();
    virtual void d32(); virtual void d33(); virtual void d34(); virtual void d35(); virtual void d36(); virtual void d37(); virtual void d38(); virtual void d39();
    virtual void d40(); virtual void d41(); virtual void d42(); virtual void d43(); virtual void d44(); virtual void d45(); virtual void d46(); virtual void d47();
    virtual void d48(); virtual void d49(); virtual void d50(); virtual void d51(); virtual void d52(); virtual void d53(); virtual void d54(); virtual void d55();
    virtual void d56(); virtual void d57(); virtual void d58(); virtual void d59(); virtual void d60(); virtual void d61(); virtual void d62(); virtual void d63();
    virtual void d64(); virtual void d65(); virtual void d66(); virtual void d67(); virtual void d68(); virtual void d69(); virtual void d70(); virtual void d71();
    virtual void d72(); virtual void d73(); virtual void d74(); virtual void d75(); virtual void d76(); virtual void d77();
    virtual void T();
    void Thunk();
};

void G3K138::Thunk()
{
    T();
}

extern "C" char DAT_0065e96c;
void *GetD0065e96c()
{
    return &DAT_0065e96c;
}

extern "C" char DAT_0065eb98;
void *GetD0065eb98()
{
    return &DAT_0065eb98;
}

extern "C" char DAT_0065ec70;
void *GetD0065ec70()
{
    return &DAT_0065ec70;
}

extern "C" char DAT_0065eeb4;
void *GetD0065eeb4()
{
    return &DAT_0065eeb4;
}

extern "C" char DAT_0065f37c;
void *GetD0065f37c()
{
    return &DAT_0065f37c;
}

extern "C" char DAT_0065f564;
void *GetD0065f564()
{
    return &DAT_0065f564;
}

extern "C" char DAT_0065f64c;
void *GetD0065f64c()
{
    return &DAT_0065f64c;
}

extern "C" char DAT_0065f86c;
void *GetD0065f86c()
{
    return &DAT_0065f86c;
}

struct G3SET
{
    void Set0();
    void Set1();
};

void G3SET::Set0()
{
    *(int *)((char *)this + 0xccc) = 0;
}

void G3SET::Set1()
{
    *(bool *)((char *)this + 0x2c) = true;
}

// The remaining setters this unit owns, in address order.
struct G2CORE3M1
{
    void SetED0(const int &value);
    void SetED4(const int &value);
    void SetED8(const int &value);
    void SetEDC(const int &value);
    void SetEE0(const int &value);
    void SetEEC(const int &value);
    void SetEF0(const int &value);
    void SetEF4(const int &value);
    void SetEF8(const int &value);
    void SetEFC(const int &value);
    void SetF00(const int &value);
    void SetF04(const int &value);
    void Set1108(const int &value);
    void Set110C(const int &value);
    void Set1118(const int &value);
    void Set111C(const int &value);
    void Set1128(const int &value);
    void Set112C(const int &value);
    void Set1130(const bool &value);
    void SetCE8(const int &value);
    void Set10CC(const int &value);
    void Set10D0(const int &value);
    void Set10D4(const int &value);
    void Set10E4(const int &value);
    void Set10EC(const int &value);
    void Set10F0(const int &value);
    void Set10F4(const int &value);
    void Set10F8(const int &value);
    void Set10FC(const int &value);
    void Set1100(const int &value);
    void Set15A8(const int &value);
    void Set15AC(const int &value);
    void Set15B0(const int &value);
    void Set15B4(const int &value);
    void Set15B8(const int &value);
    void Set15C4(const int &value);
    void Set15C8(const int &value);
    void Set15CC(const int &value);
    void Set15D0(const int &value);
    void Set15D4(const int &value);
    void Set15D8(const int &value);
    void Set15DC(const int &value);
    void Set17E0(const int &value);
    void Set17E4(const int &value);
    void Set17F0(const int &value);
    void Set1800(const int &value);
    void Set1804(const int &value);
    void SetCD0(const bool &value);
    void SetC0(const bool &value);
    void SetC1(const bool &value);
    void SetD0(const int &value);
    void SetD4(const bool &value);
    void SetE4(const int &value);
    void SetE8(const bool &value);
    void SetE9(const bool &value);
    void SetEA(const bool &value);
    void SetEB(const bool &value);
    void SetEC(const int &value);
    void SetF0(const bool &value);
    void SetF4(const int &value);
    void SetA94(const bool &value);
    void SetA95(const bool &value);
    void SetA96(const bool &value);
    void Set134(const bool &value);
    void Set460(const int &value);
    void Set464(const int &value);
    void Set468(const int &value);
};

void G2CORE3M1::SetED0(const int &value)
{
    *(int *)((char *)this + 0xed0) = value;
}

void G2CORE3M1::SetED4(const int &value)
{
    *(int *)((char *)this + 0xed4) = value;
}

void G2CORE3M1::SetED8(const int &value)
{
    *(int *)((char *)this + 0xed8) = value;
}

void G2CORE3M1::SetEDC(const int &value)
{
    *(int *)((char *)this + 0xedc) = value;
}

void G2CORE3M1::SetEE0(const int &value)
{
    *(int *)((char *)this + 0xee0) = value;
}

void G2CORE3M1::SetEEC(const int &value)
{
    *(int *)((char *)this + 0xeec) = value;
}

void G2CORE3M1::SetEF0(const int &value)
{
    *(int *)((char *)this + 0xef0) = value;
}

void G2CORE3M1::SetEF4(const int &value)
{
    *(int *)((char *)this + 0xef4) = value;
}

void G2CORE3M1::SetEF8(const int &value)
{
    *(int *)((char *)this + 0xef8) = value;
}

void G2CORE3M1::SetEFC(const int &value)
{
    *(int *)((char *)this + 0xefc) = value;
}

void G2CORE3M1::SetF00(const int &value)
{
    *(int *)((char *)this + 0xf00) = value;
}

void G2CORE3M1::SetF04(const int &value)
{
    *(int *)((char *)this + 0xf04) = value;
}

void G2CORE3M1::Set1108(const int &value)
{
    *(int *)((char *)this + 0x1108) = value;
}

void G2CORE3M1::Set110C(const int &value)
{
    *(int *)((char *)this + 0x110c) = value;
}

void G2CORE3M1::Set1118(const int &value)
{
    *(int *)((char *)this + 0x1118) = value;
}

void G2CORE3M1::Set111C(const int &value)
{
    *(int *)((char *)this + 0x111c) = value;
}

void G2CORE3M1::Set1128(const int &value)
{
    *(int *)((char *)this + 0x1128) = value;
}

void G2CORE3M1::Set112C(const int &value)
{
    *(int *)((char *)this + 0x112c) = value;
}

void G2CORE3M1::Set1130(const bool &value)
{
    *(bool *)((char *)this + 0x1130) = value;
}

void G2CORE3M1::SetCE8(const int &value)
{
    *(int *)((char *)this + 0xce8) = value;
}

void G2CORE3M1::Set10CC(const int &value)
{
    *(int *)((char *)this + 0x10cc) = value;
}

void G2CORE3M1::Set10D0(const int &value)
{
    *(int *)((char *)this + 0x10d0) = value;
}

void G2CORE3M1::Set10D4(const int &value)
{
    *(int *)((char *)this + 0x10d4) = value;
}

void G2CORE3M1::Set10E4(const int &value)
{
    *(int *)((char *)this + 0x10e4) = value;
}

void G2CORE3M1::Set10EC(const int &value)
{
    *(int *)((char *)this + 0x10ec) = value;
}

void G2CORE3M1::Set10F0(const int &value)
{
    *(int *)((char *)this + 0x10f0) = value;
}

void G2CORE3M1::Set10F4(const int &value)
{
    *(int *)((char *)this + 0x10f4) = value;
}

void G2CORE3M1::Set10F8(const int &value)
{
    *(int *)((char *)this + 0x10f8) = value;
}

void G2CORE3M1::Set10FC(const int &value)
{
    *(int *)((char *)this + 0x10fc) = value;
}

void G2CORE3M1::Set1100(const int &value)
{
    *(int *)((char *)this + 0x1100) = value;
}

void G2CORE3M1::Set15A8(const int &value)
{
    *(int *)((char *)this + 0x15a8) = value;
}

void G2CORE3M1::Set15AC(const int &value)
{
    *(int *)((char *)this + 0x15ac) = value;
}

void G2CORE3M1::Set15B0(const int &value)
{
    *(int *)((char *)this + 0x15b0) = value;
}

void G2CORE3M1::Set15B4(const int &value)
{
    *(int *)((char *)this + 0x15b4) = value;
}

void G2CORE3M1::Set15B8(const int &value)
{
    *(int *)((char *)this + 0x15b8) = value;
}

void G2CORE3M1::Set15C4(const int &value)
{
    *(int *)((char *)this + 0x15c4) = value;
}

void G2CORE3M1::Set15C8(const int &value)
{
    *(int *)((char *)this + 0x15c8) = value;
}

void G2CORE3M1::Set15CC(const int &value)
{
    *(int *)((char *)this + 0x15cc) = value;
}

void G2CORE3M1::Set15D0(const int &value)
{
    *(int *)((char *)this + 0x15d0) = value;
}

void G2CORE3M1::Set15D4(const int &value)
{
    *(int *)((char *)this + 0x15d4) = value;
}

void G2CORE3M1::Set15D8(const int &value)
{
    *(int *)((char *)this + 0x15d8) = value;
}

void G2CORE3M1::Set15DC(const int &value)
{
    *(int *)((char *)this + 0x15dc) = value;
}

void G2CORE3M1::Set17E0(const int &value)
{
    *(int *)((char *)this + 0x17e0) = value;
}

void G2CORE3M1::Set17E4(const int &value)
{
    *(int *)((char *)this + 0x17e4) = value;
}

void G2CORE3M1::Set17F0(const int &value)
{
    *(int *)((char *)this + 0x17f0) = value;
}

void G2CORE3M1::Set1800(const int &value)
{
    *(int *)((char *)this + 0x1800) = value;
}

void G2CORE3M1::Set1804(const int &value)
{
    *(int *)((char *)this + 0x1804) = value;
}

void G2CORE3M1::SetCD0(const bool &value)
{
    *(bool *)((char *)this + 0xcd0) = value;
}

void G2CORE3M1::SetC0(const bool &value)
{
    *(bool *)((char *)this + 0xc0) = value;
}

void G2CORE3M1::SetC1(const bool &value)
{
    *(bool *)((char *)this + 0xc1) = value;
}

void G2CORE3M1::SetD0(const int &value)
{
    *(int *)((char *)this + 0xd0) = value;
}

void G2CORE3M1::SetD4(const bool &value)
{
    *(bool *)((char *)this + 0xd4) = value;
}

void G2CORE3M1::SetE4(const int &value)
{
    *(int *)((char *)this + 0xe4) = value;
}

void G2CORE3M1::SetE8(const bool &value)
{
    *(bool *)((char *)this + 0xe8) = value;
}

void G2CORE3M1::SetE9(const bool &value)
{
    *(bool *)((char *)this + 0xe9) = value;
}

void G2CORE3M1::SetEA(const bool &value)
{
    *(bool *)((char *)this + 0xea) = value;
}

void G2CORE3M1::SetEB(const bool &value)
{
    *(bool *)((char *)this + 0xeb) = value;
}

void G2CORE3M1::SetEC(const int &value)
{
    *(int *)((char *)this + 0xec) = value;
}

void G2CORE3M1::SetF0(const bool &value)
{
    *(bool *)((char *)this + 0xf0) = value;
}

void G2CORE3M1::SetF4(const int &value)
{
    *(int *)((char *)this + 0xf4) = value;
}

void G2CORE3M1::SetA94(const bool &value)
{
    *(bool *)((char *)this + 0xa94) = value;
}

void G2CORE3M1::SetA95(const bool &value)
{
    *(bool *)((char *)this + 0xa95) = value;
}

void G2CORE3M1::SetA96(const bool &value)
{
    *(bool *)((char *)this + 0xa96) = value;
}

void G2CORE3M1::Set134(const bool &value)
{
    *(bool *)((char *)this + 0x134) = value;
}

void G2CORE3M1::Set460(const int &value)
{
    *(int *)((char *)this + 0x460) = value;
}

void G2CORE3M1::Set464(const int &value)
{
    *(int *)((char *)this + 0x464) = value;
}

void G2CORE3M1::Set468(const int &value)
{
    *(int *)((char *)this + 0x468) = value;
}

struct G2CORE3M2
{
    void SetD00(const V2 &value);
    void SetEE4(const V2 &value);
    void Set1120(const V2 &value);
    void SetCEC(const V2 &value);
    void Set15BC(const V2 &value);
    void Set17E8(const V2 &value);
    void SetD8(const V2 &value);
    void Set12C(const V2 &value);
};

void G2CORE3M2::SetD00(const V2 &value)
{
    *(V2 *)((char *)this + 0xd00) = value;
}

void G2CORE3M2::SetEE4(const V2 &value)
{
    *(V2 *)((char *)this + 0xee4) = value;
}

void G2CORE3M2::Set1120(const V2 &value)
{
    *(V2 *)((char *)this + 0x1120) = value;
}

void G2CORE3M2::SetCEC(const V2 &value)
{
    *(V2 *)((char *)this + 0xcec) = value;
}

void G2CORE3M2::Set15BC(const V2 &value)
{
    *(V2 *)((char *)this + 0x15bc) = value;
}

void G2CORE3M2::Set17E8(const V2 &value)
{
    *(V2 *)((char *)this + 0x17e8) = value;
}

void G2CORE3M2::SetD8(const V2 &value)
{
    *(V2 *)((char *)this + 0xd8) = value;
}

void G2CORE3M2::Set12C(const V2 &value)
{
    *(V2 *)((char *)this + 0x12c) = value;
}

SRESULT G2FORESTOBJ::SomeCommand(int value)
{
    int unused = 0;
    CreateMaxPowerup();
    return SRESULT(true);
}

struct G2CORE3M5
{
    void Set17F4(const V3 &value);
    void SetCD0(const V3 &value);
    void SetA08(const V3 &value);
    void SetA14(const V3 &value);
};

void G2CORE3M5::Set17F4(const V3 &value)
{
    *(V3 *)((char *)this + 0x17f4) = value;
}

void G2CORE3M5::SetCD0(const V3 &value)
{
    *(V3 *)((char *)this + 0xcd0) = value;
}

void G2CORE3M5::SetA08(const V3 &value)
{
    *(V3 *)((char *)this + 0xa08) = value;
}

void G2CORE3M5::SetA14(const V3 &value)
{
    *(V3 *)((char *)this + 0xa14) = value;
}
