#include "Tongas2/POLYTRAIL.h"
#include "Tongas2/FIXEDSTRING.h"
#include "Tongas2/TIMER.h"

#include <math.h>

class POLYTRAILMANAGER : public RESOURCEMANAGER
{
public:
    POLYTRAILMANAGER()
    {
        SafeSprintf(m_fileName, "POLYTRAIL.txt");
        m_8 = false;
        m_18 = true;
    }

    virtual RESOURCEMASTER *CreateMaster(const char *name) { return new POLYTRAILMASTER(this, name); }
    virtual const char *GetTypeCode();
};

static RESOURCEMANAGER *CreatePOLYTRAIL();

CREATEMANAGER g_pfnCreatePOLYTRAIL = CreatePOLYTRAIL;

CMDENTRY POLYTRAILMASTER::s_commands[] = {
    { 0, 0, 0, 0, 0 },
};

PROPENTRY POLYTRAILMASTER::s_properties[] = {
    { "Life", 0, 3, 0, PROP_FIELD(POLYTRAILMASTER, m_life), false, 1, PROP_SETTER(POLYTRAILMASTER, SetLife) },
    { "Rate", 0, 3, 0, PROP_FIELD(POLYTRAILMASTER, m_rate), false, 1, PROP_SETTER(POLYTRAILMASTER, SetRate) },
    { "ExternalVel", 0, 11, 0, PROP_FIELD(POLYTRAILMASTER, m_externalVel), false, 1, PROP_SETTER(POLYTRAILMASTER, SetExternalVel) },
    { 0, 0, 15, 0, 0, false },
};

POLYTRAIL::POLYTRAIL(POLYTRAILMASTER *master, const RESHANDLE &handle)
    : CURVE(master, handle)
{
    m_reset = false;
    m_dying = false;
    m_velocity.x = m_velocity.y = m_velocity.z = 0.0f;
}

void POLYTRAIL::Stop()
{
    if (m_dying)
        Virtual20();
    CURVE::Stop();
}

void POLYTRAIL::PlacePoints()
{
    POLYTRAILMASTER *master = Master();
    float rate = master->m_rate;
    float perSecond = g_pResources->GetDetailFactor() * rate;
    int count = (int)ceil(perSecond * master->m_life) + 1;
    if (count > CURVE_MAX_POINTS)
        count = CURVE_MAX_POINTS;
    else if (count < 2)
        count = 2;
    m_interval = 1.0f / perSecond;
    if (SetNumPoints(count))
        m_t[0] = 1.0f;
}

void POLYTRAIL::Reset()
{
    m_dying = false;
    m_reset = true;
    for (int i = 0; i < m_numPoints; i++) {
        m_points[i].x = m_points[i].y = m_points[i].z = 0.0f;
        m_t[i] = 1.0f;
    }
}

void POLYTRAIL::Start()
{
    CURVE::Start();
    Reset();
}

void POLYTRAILMASTER::RegisterCommands()
{
    g_classParents[s_classId] = CURVEMASTER::s_classId;
    g_classCommands[s_classId] = s_commands;
    HashCommands(s_commands);
    RegisterClassName(s_commands, "POLYTRAILMASTER", "CURVEMASTER");
}

POLYTRAILMASTER::POLYTRAILMASTER(RESOURCEMANAGER *manager, const char *name)
    : CURVEMASTER(manager, name), m_externalVel(0.0f, 0.0f, 0.0f)
{
    m_life = 1.0f;
    m_rate = 1.0f;
    m_externalVel = V3(0.0f, 0.0f, 0.0f);
}

int POLYTRAILMASTER::GetClassId()
{
    return s_classId;
}

int POLYTRAILMASTER::GetParentClassId()
{
    return g_classParents[s_classId];
}

const char *POLYTRAILMASTER::GetClassName()
{
    return g_classNames[s_classId];
}

RESOURCE *POLYTRAILMASTER::CreateCopy(const RESHANDLE &handle)
{
    return new POLYTRAIL(this, handle);
}

const char *POLYTRAILMANAGER::GetTypeCode()
{
    return "PYTR";
}

static RESOURCEMANAGER *CreatePOLYTRAIL()
{
    return new POLYTRAILMANAGER;
}

void POLYTRAIL::Move()
{
    V3 moved = m_position - m_c0;
    POLYTRAILMASTER *master = Master();
    if (!m_reset && (m_d8.x != m_scale.x || m_d8.y != m_scale.y || m_d8.z != m_scale.z)) {
        V3 ratio(m_d8.x / m_scale.x, m_d8.y / m_scale.y, m_d8.z / m_scale.z);
        for (int i = 0; i < m_numPoints; i++)
            m_points[i] = ratio * m_points[i];
        moved.x /= m_scale.x;
        moved.y /= m_scale.y;
        moved.z /= m_scale.z;
    }
    if (m_dying) {
        float fade = g_pTimer->m_dt / master->m_life;
        for (int i = 0; i < m_numPoints; i++) {
            m_t[i] -= fade;
            if (m_t[i] < 0.0f)
                m_t[i] = 0.0f;
        }
        if (m_t[0] == 0.0f) {
            Virtual20();
            return;
        }
    } else {
        if (m_reset) {
            m_reset = false;
            moved.x = moved.y = moved.z = 0.0f;
            m_timer = 0.0f;
            m_count = 1;
        }
        m_timer -= g_pTimer->m_dt;
        while (m_timer <= 0.0f) {
            m_count++;
            m_timer += m_interval;
            if (m_count > m_numPoints)
                m_count = m_numPoints;
            for (int j = m_numPoints - 1; j > 1; j--) {
                m_points[j] = m_points[j - 1];
                m_t[j] = m_t[j - 1];
            }
            m_points[1].x = m_points[1].y = m_points[1].z = 0.0f;
            m_t[1] = 1.0f;
        }
        m_2480 = 0;
        float dt = g_pTimer->m_dt;
        V3 step = dt * m_angles.Transform(m_velocity + master->m_externalVel);
        float fade = g_pTimer->m_dt / master->m_life;
        for (int i = 1; i < m_numPoints; i++) {
            m_points[i] -= moved;
            m_points[i] += step;
            if (i < m_count) {
                m_t[i] -= fade;
                if (m_t[i] < 0.0f)
                    m_t[i] = 0.0f;
            } else {
                m_t[i] = m_t[i - 1];
            }
        }
    }
    UpdateMesh();
}

void POLYTRAIL::Update()
{
    OBJECT::Update();
    if (m_active) {
        PlacePoints();
        Move();
        if (m_initialized) {
            MESH *mesh = (MESH *)g_pResources->GetResource(m_meshCopy);
            if (mesh) {
                if (m_drawPosition != m_lastPosition) {
                    mesh->SetPosition(m_drawPosition);
                    m_lastPosition = m_drawPosition;
                }
                m_c0 = m_position;
                if (m_drawScale != m_lastScale) {
                    mesh->SetScale(m_drawScale);
                    m_lastScale = m_drawScale;
                }
                m_d8 = m_scale;
                if (m_lastLayer != m_layer) {
                    mesh->SetLayer(m_layer);
                    m_lastLayer = m_layer;
                }
            }
        }
    }
}
