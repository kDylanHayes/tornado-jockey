#include "Tongas2/TESLA.h"
#include "Tongas2/FIXEDSTRING.h"
#include "Tongas2/PATH.h"
#include "Tongas2/TIMER.h"

#include <math.h>

class TESLAMANAGER : public RESOURCEMANAGER
{
public:
    TESLAMANAGER()
    {
        SafeSprintf(m_fileName, "TESLA.txt");
        m_8 = false;
        m_18 = true;
    }

    virtual RESOURCEMASTER *CreateMaster(const char *name) { return new TESLAMASTER(this, name); }
    virtual const char *GetTypeCode();
};

static RESOURCEMANAGER *CreateTESLA();

CREATEMANAGER g_pfnCreateTESLA = CreateTESLA;

CMDENTRY TESLAMASTER::s_commands[] = {
    { 0, 0, 0, 0, 0 },
};

PROPENTRY TESLAMASTER::s_properties[] = {
    { "Depth", 0, 0, 0, PROP_FIELD(TESLAMASTER, m_depth), false, 1, PROP_SETTER(TESLAMASTER, SetDepth) },
    { "Rate", 0, 3, 0, PROP_FIELD(TESLAMASTER, m_rate), false, 1, PROP_SETTER(TESLAMASTER, SetRate) },
    { "RandX", 0, 10, 0, PROP_FIELD(TESLAMASTER, m_randX), false, 1, PROP_SETTER(TESLAMASTER, SetRandX) },
    { "RandY", 0, 10, 0, PROP_FIELD(TESLAMASTER, m_randY), false, 1, PROP_SETTER(TESLAMASTER, SetRandY) },
    { "RandZ", 0, 10, 0, PROP_FIELD(TESLAMASTER, m_randZ), false, 1, PROP_SETTER(TESLAMASTER, SetRandZ) },
    { "Point", 0, 11, 0, PROP_FIELD(TESLAMASTER, m_point), false, 2, PROP_SETTER(TESLAMASTER, SetPoint) },
    { 0, 0, 15, 0, 0, false },
};

void TESLA::Strike()
{
    TESLAMASTER *master = Master();
    m_timer -= g_pTimer->m_dt;
    if (m_timer > 0.0f)
        return;
    int count = 1;
    float scale = 1.0f;
    m_timer = 1.0f / master->m_rate;
    m_points[0] = master->m_point[0];
    m_points[m_numPoints - 1] = master->m_point[1];
    int last = m_numPoints - 1;
    int half = last / 2;
    for (int level = 0; level < master->m_depth; level++) {
        int span = last / count;
        int a = 0;
        int b = span;
        int m = half;
        for (int i = 0; i < count; i++) {
            float x = master->m_randX.Random() * scale;
            float y = master->m_randY.Random() * scale;
            float z = master->m_randZ.Random();
            m_points[m].x = (m_points[b].x + m_points[a].x) * 0.5f + x;
            m_points[m].y = (m_points[b].y + m_points[a].y) * 0.5f + y;
            m_points[m].z = (m_points[b].z + m_points[a].z) * 0.5f + z * scale;
            a += span;
            b += span;
            m += half * 2;
        }
        scale *= 0.5f;
        half /= 2;
        count *= 2;
    }
    UpdateMesh();
}

void TESLAMASTER::RegisterCommands()
{
    g_classParents[s_classId] = CURVEMASTER::s_classId;
    g_classCommands[s_classId] = s_commands;
    HashCommands(s_commands);
    RegisterClassName(s_commands, "TESLAMASTER", "CURVEMASTER");
}

TESLAMASTER::TESLAMASTER(RESOURCEMANAGER *manager, const char *name)
    : CURVEMASTER(manager, name)
{
    m_randX = EMITRANGE(-1.0f, 1.0f);
    m_randY = EMITRANGE(-1.0f, 1.0f);
    m_randZ = EMITRANGE(-1.0f, 1.0f);
    m_depth = 4;
    m_rate = 10.0f;
    for (g_i = 0; g_i < 2; g_i++)
        m_point[g_i] = V3(0.0f, 0.0f, 0.0f);
}

int TESLAMASTER::GetClassId()
{
    return s_classId;
}

int TESLAMASTER::GetParentClassId()
{
    return g_classParents[s_classId];
}

const char *TESLAMASTER::GetClassName()
{
    return g_classNames[s_classId];
}

RESOURCE *TESLAMASTER::CreateCopy(const RESHANDLE &handle)
{
    return new TESLA(this, handle);
}

const char *TESLAMANAGER::GetTypeCode()
{
    return "TESL";
}

static RESOURCEMANAGER *CreateTESLA()
{
    return new TESLAMANAGER;
}

void TESLA::PlacePoints()
{
    int depth = Master()->m_depth;
    int count = _Pow_int(2, depth - g_pResources->GetDetailBias()) + 1;
    if (count > CURVE_MAX_POINTS)
        count = CURVE_MAX_POINTS;
    else if (count < 3)
        count = 3;
    if (SetNumPoints(count))
        m_timer = 0.0f;
}

void TESLA::Update()
{
    OBJECT::Update();
    if (m_active) {
        PlacePoints();
        Strike();
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
            if (m_134 != m_angles) {
                mesh->SetAngles(m_angles);
                m_134 = m_angles;
            }
        }
    }
}
