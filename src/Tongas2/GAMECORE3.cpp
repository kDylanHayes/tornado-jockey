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
