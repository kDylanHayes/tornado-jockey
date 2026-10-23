#include "Tongas2/CURVE.h"
#include "Tongas2/FIXEDSTRING.h"
#include "Tongas2/LOG.h"
#include "Tongas2/PATH.h"
#include "Tongas2/SAFEINDEX.h"

#include <math.h>

#define CURVE_ARGB(a, r, g, b) ((((a) & 0xff) << 24) | (((r) & 0xff) << 16) | (((g) & 0xff) << 8) | ((b) & 0xff))

short CURVE::s_indices[(CURVE_MAX_POINTS - 1) * 12];
bool CURVE::s_indicesReady;

CMDENTRY CURVEMASTER::s_commands[] = {
    { 0, 0, 0, 0, 0 },
};

PROPENTRY CURVEMASTER::s_properties[] = {
    { "Width", 0, 3, 0, PROP_FIELD(CURVEMASTER, m_width), false, 1, PROP_SETTER(CURVEMASTER, SetWidth) },
    { "Texture", 0, 14, 0, PROP_FIELD(CURVEMASTER, m_texture), false, 1, PROP_SETTER(CURVEMASTER, SetTexture) },
    { "Color", 0, 13, 0, PROP_FIELD(CURVEMASTER, m_color), false, 2, PROP_SETTER(CURVEMASTER, SetColor) },
    { "Alpha", 0, 3, 0, PROP_FIELD(CURVEMASTER, m_alpha), false, 2, PROP_SETTER(CURVEMASTER, SetAlpha) },
    { "LookCamera", 0, 2, 0, PROP_FIELD(CURVEMASTER, m_lookCamera), false, 1, PROP_SETTER(CURVEMASTER, SetLookCamera) },
    { 0, 0, 15, 0, 0, false },
};

void CURVEMASTER::RegisterCommands()
{
    g_classParents[s_classId] = RESOURCEMASTER::s_classId;
    g_classCommands[s_classId] = s_commands;
    HashCommands(s_commands);
    RegisterClassName(s_commands, "CURVEMASTER", "RESOURCEMASTER");
}

CURVEMASTER::~CURVEMASTER()
{
}

int CURVEMASTER::GetClassId()
{
    return s_classId;
}

int CURVEMASTER::GetParentClassId()
{
    return g_classParents[s_classId];
}

const char *CURVEMASTER::GetClassName()
{
    return g_classNames[s_classId];
}

void CURVEMASTER::LoadShader()
{
    SHADERMASTER *shader = (SHADERMASTER *)g_pResources->GetMaster(m_shaderHandle);
    if (shader) {
        shader->AddUser();
        m_shaderLoaded = true;
    }
}

void CURVEMASTER::ReleaseShader()
{
    SHADERMASTER *shader = (SHADERMASTER *)g_pResources->GetMaster(m_shaderHandle);
    if (shader) {
        shader->RemoveUser();
        m_shaderLoaded = false;
    }
}

void CURVEMASTER::UpdateCopies()
{
    if (m_numUsers <= 0 && m_shaderLoaded)
        ReleaseShader();
    else if (m_numUsers > 0 && !m_shaderLoaded)
        LoadShader();
    RESOURCEMASTER::UpdateCopies();
}

void CURVEMASTER::SetShaderHandle(const RESHANDLE &handle)
{
    RESHANDLE &current = m_shaderHandle;
    if (current.serial != handle.serial || current.index != handle.index || current.kind != handle.kind) {
        bool loaded = false;
        if (m_shaderLoaded)
            loaded = true;
        if (current.IsValid())
            ReleaseShader();
        current = handle;
        if (loaded && current.IsValid())
            LoadShader();
    }
}

void CURVEMASTER::SetTexture(const SRTEXTURE &texture)
{
    m_texture = texture;
    SetShaderHandle(g_pSHADER->GetHandle(texture.name));
}

CURVE::CURVE(CURVEMASTER *master, const RESHANDLE &handle)
    : OBJECT(master, handle)
{
    m_loaded = false;
    m_numPoints = 0;
    if (++master->m_numUsers > 0 && !master->m_shaderLoaded)
        master->LoadShader();
}

void CURVE::CreateMesh()
{
    FIXEDSTRING<0x200> name;
    CURVEMASTER *master = Master();
    SafeSprintf(name, "__c%d", m_numPoints);
    RESHANDLE shader = master->m_shaderHandle;
    m_mesh = ((MESHMANAGER *)g_pMESH)->CreateMesh(shader, GetLayer(), name);
}

// The translation plus the right axis of the angles' matrix.
static inline V3 CurveRightPoint(ANG &angles)
{
    if (angles.flags & ANG::MATRIX_DIRTY) {
        angles.matrix.FromEuler(angles.euler.z, angles.euler.y, angles.euler.x);
        angles.flags &= ~ANG::MATRIX_DIRTY;
    }
    return V3(angles.matrix._41 + angles.matrix._11, angles.matrix._42 + angles.matrix._12, angles.matrix._43 + angles.matrix._13);
}

// The side of the strip at a point.
static inline V3 CurveCross(const V3 &s, const V3 &b)
{
    return V3(s.z * b.y - s.y * b.z, s.x * b.z - s.z * b.x, s.y * b.x - s.x * b.y);
}

void CURVE::UpdateMesh()
{
    MESH *mesh = (MESH *)g_pResources->GetResource(m_meshCopy);
    if (!mesh) {
        if (g_pLog)
            g_pLog->Error("warning! no mesh updating curve %s!", m_master->m_name.c_str());
        return;
    }
    CURVEMASTER *master = Master();
    float halfWidth = master->m_width * 0.5f;
    int i;
    for (i = 0; i < m_numPoints; i++) {
        V3 side;
        if (master->m_lookCamera) {
            V3 prev = i == 0 ? m_points[0] : m_points[i - 1];
            V3 next;
            if (i == m_numPoints - 1)
                next = m_points[i];
            else
                next = m_points[i + 1];
            V3 toCamera = m_points[i] - WorldToLocal(g_pCamera->GetWorldPosition());
            side = CurveCross(next - prev, toCamera);
        } else {
            side = CurveRightPoint(m_angles);
        }
        float length = side.Length();
        V3 offset = (length != 0.0f ? side * (1.0f / length) : V3(0.0f, 0.0f, 0.0f)) * halfWidth;
        m_vertices[i * 2].position = m_points[i] - offset;
        m_vertices[i * 2 + 1].position = offset + m_points[i];
    }
    float range = m_uv.right - m_uv.left - 0.05f;
    for (i = 0; i < m_numPoints; i++) {
        float u = range * m_t[i] + m_uv.left + 0.025f;
        m_vertices[i * 2].u = u;
        m_vertices[i * 2].v = m_uv.top;
        m_vertices[i * 2 + 1].u = u;
        m_vertices[i * 2 + 1].v = m_uv.bottom;
    }
    mesh->m_flags |= 5;
}

void CURVE::AddMesh()
{
    if (m_meshCopy.IsValid()) {
        if (g_pLog)
            g_pLog->Error("warning! curve is already added to the mesh, removing first! %s", m_master->m_name.c_str());
        CURVE::Stop();
    }
    MESH *mesh = (MESH *)g_pResources->GetResource(m_mesh);
    if (!mesh) {
        CreateMesh();
        mesh = (MESH *)g_pResources->GetResource(m_mesh);
    }
    if (mesh) {
        mesh->SetVertices(m_vertices, m_numPoints * 2, s_indices, m_numPoints * 4 - 4);
        mesh->SetActive(true);
        m_lastLayer = -1;
        m_lastPosition.x = -1.0f;
        m_lastPosition.y = -1.0f;
        m_lastPosition.z = -1.0f;
        m_lastScale.x = -1.0f;
        m_lastScale.y = -1.0f;
        m_lastScale.z = -1.0f;
        m_134 = ANG(-1.0f, -1.0f, -1.0f);
        m_meshCopy = mesh->m_handle;
    } else if (g_pLog) {
        g_pLog->Error("Could not create a mesh resource for curve %s", m_master->m_name.c_str());
    }
}

CURVEMASTER::CURVEMASTER(RESOURCEMANAGER *manager, const char *name)
    : RESOURCEMASTER(manager, name)
{
    m_width = 1.0f;
    m_texture = SRTEXTURE();
    for (g_i = 0; g_i < 2; g_i++)
        m_color[g_i] = V3I(255, 255, 255);
    for (g_i = 0; g_i < 2; g_i++)
        m_alpha[g_i] = 1.0f;
    m_lookCamera = true;
    m_numUsers = 0;
    m_shaderLoaded = false;
}

bool CURVE::SetNumPoints(int count)
{
    if (m_numPoints == count)
        return false;
    CURVEMASTER *master = Master();
    if (!s_indicesReady) {
        s_indicesReady = true;
        for (int i = 0; i < CURVE_MAX_POINTS - 1; i++) {
            s_indices[i * 12 + 0] = i * 2;
            s_indices[i * 12 + 1] = i * 2 + 1;
            s_indices[i * 12 + 2] = i * 2 + 2;
            s_indices[i * 12 + 3] = i * 2 + 3;
            s_indices[i * 12 + 4] = i * 2 + 2;
            s_indices[i * 12 + 5] = i * 2 + 1;
            s_indices[i * 12 + 6] = i * 2 + 1;
            s_indices[i * 12 + 7] = i * 2 + 2;
            s_indices[i * 12 + 8] = i * 2 + 3;
            s_indices[i * 12 + 9] = i * 2 + 2;
            s_indices[i * 12 + 10] = i * 2 + 1;
            s_indices[i * 12 + 11] = i * 2;
        }
    }
    m_numPoints = count;
    if (m_active) {
        CURVE::Stop();
        AddMesh();
    }
    for (int i = 0; i < count; i++) {
        m_points[i].x = m_points[i].y = m_points[i].z = 0.0f;
        float t = (float)i / (float)(count - 1);
        float s = 1.0f - t;
        const V3I &c0 = master->m_color[0];
        const V3I &c1 = master->m_color[1];
        const float *alpha = master->m_alpha;
        int r = (int)(c1.x * t + c0.x * s);
        int g = (int)(c1.y * t + c0.y * s);
        int b = (int)(c1.z * t + c0.z * s);
        int a = (int)((t * alpha[1] + s * alpha[0]) * 255.0f);
        if (r < 0)
            r = 0;
        else if (r > 255)
            r = 255;
        if (g < 0)
            g = 0;
        else if (g > 255)
            g = 255;
        if (b < 0)
            b = 0;
        else if (b > 255)
            b = 255;
        if (a < 0)
            a = 0;
        else if (a > 255)
            a = 255;
        int color = CURVE_ARGB(a, r, g, b);
        m_vertices[i * 2].color = color;
        m_vertices[i * 2 + 1].color = color;
        m_t[i] = t;
    }
    return true;
}

void CURVE::SetLayer(int layer)
{
    if (layer != m_19c) {
        m_19c = layer;
        if (m_numPoints > 0) {
            if (m_active)
                CURVE::Stop();
            CreateMesh();
            if (m_active)
                AddMesh();
        }
    }
}

void CURVE::Start()
{
    PlacePoints();
    AddMesh();
}
