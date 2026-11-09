// Tongas2/GAMECORE3 -- the game-code block at 0x42c400. The OBJECT and
// RESOURCE accessors the original translation unit emitted out of line.
#define OBJECT_OUTOFLINE

#include "Tongas2/RESOURCE.h"

#include "Tongas2/CMDBASE.h"
#include "Tongas2/RESOURCES.h"
#include "Tongas2/SCENE.h"

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

void OBJECT::Init()
{
    m_lastPosition = V3(-100.0f, -100.0f, -100.0f);
    m_lastScale = V3(-100.0f, -100.0f, -100.0f);
    m_c0 = V3(-100.0f, -100.0f, -100.0f);
    m_d8 = V3(-100.0f, -100.0f, -100.0f);
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
    ANG ca = a;
    if (ca.flags & ANG::EULER_DIRTY)
        ca.UpdateEuler();
    ANG cb = b;
    if (cb.flags & ANG::EULER_DIRTY)
        cb.UpdateEuler();
    return ca.euler.x != cb.euler.x || ca.euler.y != cb.euler.y || ca.euler.z != cb.euler.z;
}
