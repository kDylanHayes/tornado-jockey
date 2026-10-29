#include <math.h>
#include <stdlib.h>

#include "Tongas2/DECAL.h"
#include "Tongas2/LOG.h"

float DECAL::s_corners[4][3] = {
    { 0.5f, 0.5f, 0.0f },
    { 0.5f, -0.5f, 0.0f },
    { -0.5f, 0.5f, 0.0f },
    { -0.5f, -0.5f, 0.0f },
};

short DECAL::s_indices[6] = { 0, 1, 2, 3, 2, 1 };

class DECALMANAGER : public RESOURCEMANAGER
{
public:
    DECALMANAGER()
    {
        SafeSprintf(m_fileName, "DECAL.txt");
        m_8 = false;
        m_18 = true;
    }

    virtual RESOURCEMASTER *CreateMaster(const char *name);
    virtual const char *GetTypeCode();
};

static RESOURCEMANAGER *CreateDECAL()
{
    return new DECALMANAGER;
}

CREATEMANAGER g_pfnCreateDECAL = CreateDECAL;

CMDENTRY DECALMASTER::s_commands[] = {
    { 0, 0, 0, 0, 0 },
};

PROPENTRY DECALMASTER::s_properties[] = {
    { "Shader", 0, 7, 0, PROP_FIELD(DECALMASTER, m_shader), false, 1, PROP_SETTER(DECALMASTER, SetShader) },
    { "AlwaysLookCamera", 0, 2, 0, PROP_FIELD(DECALMASTER, m_alwaysLookCamera), false, 1, PROP_SETTER(DECALMASTER, SetAlwaysLookCamera) },
    { "NeverRotate", 0, 2, 0, PROP_FIELD(DECALMASTER, m_neverRotate), false, 1, PROP_SETTER(DECALMASTER, SetNeverRotate) },
    { "Simulate3D", 0, 2, 0, PROP_FIELD(DECALMASTER, m_simulate3D), false, 1, PROP_SETTER(DECALMASTER, SetSimulate3D) },
    { "DecalOrderForced", 0, 0, 0, PROP_FIELD(DECALMASTER, m_decalOrderForced), false, 1, PROP_SETTER(DECALMASTER, SetDecalOrderForced) },
    { "AnimLen", 0, 3, 0, PROP_FIELD(DECALMASTER, m_animLen), false, 1, PROP_SETTER(DECALMASTER, SetAnimLen) },
    { "StartFrame", 0, 0, 0, PROP_FIELD(DECALMASTER, m_startFrame), false, 1, PROP_SETTER(DECALMASTER, SetStartFrame) },
    { "LastFrame", 0, 0, 0, PROP_FIELD(DECALMASTER, m_lastFrame), false, 1, PROP_SETTER(DECALMASTER, SetLastFrame) },
    { "Loop", 0, 2, 0, PROP_FIELD(DECALMASTER, m_loop), false, 1, PROP_SETTER(DECALMASTER, SetLoop) },
    { "FwBk", 0, 2, 0, PROP_FIELD(DECALMASTER, m_fwBk), false, 1, PROP_SETTER(DECALMASTER, SetFwBk) },
    { "RandomStartFrame", 0, 2, 0, PROP_FIELD(DECALMASTER, m_randomStartFrame), false, 1, PROP_SETTER(DECALMASTER, SetRandomStartFrame) },
    { "LocalRotation", 0, 12, 0, PROP_FIELD(DECALMASTER, m_localRotation), false, 1, PROP_SETTER(DECALMASTER, SetLocalRotation) },
    { "RotationSpeed", 0, 11, 0, PROP_FIELD(DECALMASTER, m_rotationSpeed), false, 1, PROP_SETTER(DECALMASTER, SetRotationSpeed) },
    { 0, 0, 15, 0, 0, false },
};

void DECAL::CreateMesh()
{
    DECALMASTER *master = Master();
    m_mesh = ((MESHMANAGER *)g_pMESH)->CreateMesh(master->m_shaderHandle, GetLayer(), "__d");
}

void DECAL::Init()
{
    OBJECT::Init();
    m_hasColor = false;
    m_localRotation = Master()->m_localRotation;
}

void DECAL::SetColor(const V3 &color)
{
    static_cast<V3 &>(m_color) = color;
    m_hasColor = true;
    MESH *mesh = (MESH *)g_pResources->GetResource(m_meshCopy);
    if (mesh)
        mesh->SetColor(color);
}

RESHANDLE DECAL::GetShaderHandle()
{
    return Master()->m_shaderHandle;
}

void DECAL::SetUV(float u0, float v0, float u1, float v1)
{
    m_uv[0].v = v0;
    m_uv[0].u = u0;
    m_uv[1].u = u1;
    m_uv[1].v = v1;
    m_vertices[0].u = u0;
    m_vertices[0].v = v0;
    m_vertices[1].v = v1;
    m_vertices[1].u = u0;
    m_vertices[2].u = u1;
    m_vertices[2].v = v0;
    m_vertices[3].u = u1;
    m_vertices[3].v = v1;
    MESH *mesh = (MESH *)g_pResources->GetResource(m_meshCopy);
    if (mesh)
        mesh->m_flags |= 4;
    m_lastFrame = m_frame;
}

void DECAL::UpdateAngles()
{
    OBJECT::UpdateAngles();
}

void DECALMASTER::RegisterCommands()
{
    g_classParents[s_classId] = RESOURCEMASTER::s_classId;
    g_classCommands[s_classId] = s_commands;
    HashCommands(s_commands);
    RegisterClassName(s_commands, "DECALMASTER", "RESOURCEMASTER");
}

void DECALMASTER::LoadShader()
{
    SHADERMASTER *shader = (SHADERMASTER *)g_pResources->GetMaster(m_shaderHandle);
    if (shader) {
        shader->AddUser();
        m_shaderLoaded = true;
    }
}

void DECALMASTER::ReleaseShader()
{
    SHADERMASTER *shader = (SHADERMASTER *)g_pResources->GetMaster(m_shaderHandle);
    if (shader) {
        shader->RemoveUser();
        m_shaderLoaded = false;
    }
}

const char *DECALMANAGER::GetTypeCode()
{
    return "DECL";
}

DECAL::~DECAL()
{
    if (m_master)
        Master()->m_numUsers--;
}

void DECAL::SetPosition(V3 position)
{
    m_position = position;
    m_screenSpace = false;
}

void DECAL::Stop()
{
    MESH *mesh = (MESH *)g_pResources->GetResource(m_meshCopy);
    if (mesh)
        mesh->Virtual20();
    m_meshCopy.Invalidate();
}

void DECAL::CreateMeshCopy()
{
    if (m_meshCopy.IsValid()) {
        MESH *mesh = (MESH *)g_pResources->GetResource(m_meshCopy);
        if (mesh)
            mesh->Virtual20();
        m_meshCopy.Invalidate();
    }
    MESH *mesh = (MESH *)g_pResources->GetResource(m_mesh);
    if (!mesh) {
        CreateMesh();
        mesh = (MESH *)g_pResources->GetResource(m_mesh);
    }
    if (mesh) {
        mesh->SetVertices(m_vertices, 4, s_indices, 2);
        if (m_hasColor)
            mesh->SetColor(m_color);
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
    } else if (g_pLog)
        g_pLog->Error("warning! could not create a mesh resource for curve %s", Master()->m_name.c_str());
}

bool DECAL::IsLoaded()
{
    if (!m_meshReady) {
        SHADER *shader = (SHADER *)g_pResources->GetResource(Master()->m_shaderHandle);
        if (!shader) {
            if (m_parent.IsValid())
                g_pScene->GetActor(m_parent);
            return true;
        }
        m_master->SetGroupMask(shader->m_master->m_groupMask);
        if (!shader->IsLoaded())
            return false;
        CreateMesh();
        return m_meshReady = true;
    }
    return true;
}

void DECAL::SetLayer(int layer)
{
    if (layer != m_19c) {
        m_19c = layer;
        if (m_active)
            DECAL::Stop();
        CreateMesh();
        if (m_active)
            CreateMeshCopy();
    }
}

DECALMASTER::~DECALMASTER()
{
    if (m_shaderLoaded)
        ReleaseShader();
}

int DECALMASTER::GetClassId()
{
    return s_classId;
}

int DECALMASTER::GetParentClassId()
{
    return g_classParents[s_classId];
}

const char *DECALMASTER::GetClassName()
{
    return g_classNames[s_classId];
}

void DECALMASTER::UpdateCopies()
{
    if (m_numUsers <= 0 && m_shaderLoaded)
        ReleaseShader();
    else if (m_numUsers > 0 && !m_shaderLoaded)
        LoadShader();
    RESOURCEMASTER::UpdateCopies();
}

void DECALMASTER::SetShader(const CSTRING &name)
{
    bool reload = false;
    if (name != m_shader && m_shaderLoaded) {
        ReleaseShader();
        reload = true;
    }
    m_shader = name;
    m_shaderHandle = g_pSHADER->GetHandle(m_shader.c_str());
    if (!m_shaderHandle.IsValid())
        m_shader = "";
    if (reload && m_shaderHandle.IsValid())
        LoadShader();
}

DECAL::DECAL(DECALMASTER *master, RESHANDLE handle)
    : OBJECT(master, handle), m_screenPosition(0.0f, 0.0f), m_scrollSpeed(0.0f, 0.0f), m_480(0.0f, 0.0f),
      m_localRotation(0.0f, 0.0f, 0.0f)
{
    m_meshReady = false;
    m_screenSpace = false;
    m_frame = 0;
    m_lastFrame = -1;
    m_scrolling = false;
    m_animLen = master->m_animLen;
    if (++master->m_numUsers > 0 && !master->m_shaderLoaded)
        master->LoadShader();
    m_vertices[0].position.x = s_corners[0][0];
    m_vertices[0].position.y = s_corners[0][1];
    m_vertices[0].position.z = s_corners[0][2];
    m_vertices[1].position.x = s_corners[1][0];
    m_vertices[1].position.y = s_corners[1][1];
    m_vertices[1].position.z = s_corners[1][2];
    m_vertices[2].position.x = s_corners[2][0];
    m_vertices[2].position.y = s_corners[2][1];
    m_vertices[2].position.z = s_corners[2][2];
    m_vertices[3].position.x = s_corners[3][0];
    m_vertices[3].position.y = s_corners[3][1];
    m_vertices[3].position.z = s_corners[3][2];
    m_hasColor = false;
}

void DECAL::Start()
{
    DECALMASTER *master = Master();
    if (m_animLen > 0.0f) {
        m_animating = true;
        m_animTime = 0.0f;
        if (!master->m_randomStartFrame) {
            m_frameOffset = master->m_startFrame;
            m_backwards = false;
        } else if (master->m_lastFrame - master->m_startFrame > 0) {
            m_frameOffset = (int)((float)rand() * (float)(master->m_lastFrame - master->m_startFrame + 1) * (1.0f / 32768.0f));
            m_backwards = false;
        } else {
            m_frameOffset = 0;
            m_backwards = false;
        }
    } else
        m_animating = false;
    m_134 = ANG(1.0f, 1.0f, 1.0f);
    m_lastFrame = -1;
    CreateMeshCopy();
}

void DECAL::Update()
{
    OBJECT::Update();
    if (!m_active)
        return;
    DECALMASTER *master = Master();
    if (m_animating) {
        int first = master->m_startFrame;
        int last = master->m_lastFrame;
        int frame = (int)((float)(last - first + 1) / m_animLen * m_animTime);
        if (!m_backwards)
            m_frame = first + frame;
        else
            m_frame = last - frame;
        if (m_frame < master->m_startFrame)
            m_frame = master->m_startFrame;
        if (m_frame > master->m_lastFrame)
            m_frame = master->m_lastFrame;
        if (m_frameOffset > 0) {
            m_frame += m_frameOffset;
            if (m_frame > master->m_lastFrame)
                m_frame = m_frame - master->m_lastFrame + master->m_startFrame - 1;
        }
        float time = g_pTimer->m_dt + m_animTime;
        m_animTime = time;
        if (time > m_animLen) {
            if (master->m_fwBk) {
                if (!m_backwards) {
                    m_backwards = true;
                    m_animTime = time - m_animLen;
                } else if (master->m_loop) {
                    m_backwards = false;
                    m_animTime = time - m_animLen;
                } else {
                    m_frame = master->m_startFrame;
                    m_animating = false;
                }
            } else if (master->m_loop)
                m_animTime = time - m_animLen;
            else {
                m_frame = master->m_lastFrame;
                m_animating = false;
            }
        }
    }
    if (m_screenSpace != m_lastScreenSpace) {
        m_lastScreenSpace = m_screenSpace;
        m_c0.x = m_c0.y = m_c0.z = 0.0f;
    }
    if (!master->m_rotationSpeed.IsZero()) {
        float dt = g_pTimer->m_dt;
        m_localRotation.Rotate(ANG(dt * master->m_rotationSpeed.x, dt * master->m_rotationSpeed.y, dt * master->m_rotationSpeed.z));
    }
    MESH *mesh = (MESH *)g_pResources->GetResource(m_meshCopy);
    if (!mesh)
        return;
    CAMERA *camera;
    if (m_screenSpace) {
        V2 size = ((CAMERA *)g_pCamera)->GetViewSize();
        float depth = 1.0f;
        mesh->SetPosition(((CAMERA *)g_pCamera)->ScreenToWorld(m_screenPosition, depth));
        V3 scale(size.x * m_scale.x, size.y * m_scale.y, 1.0f);
        mesh->SetScale(scale);
        if (master->m_alwaysLookCamera) {
            camera = (CAMERA *)g_pCamera;
            V3 direction = camera->m_direction * -1.0f;
            V3 position = camera->m_up;
            float roll = GetRoll();
            if (roll != 0.0f && !master->m_neverRotate) {
                MATRIX m;
                m.Identity();
                m.RotateAxis(roll, direction);
                position = position * m;
            }
            ANG angles(0.0f, 0.0f, 0.0f);
            angles.SetLookAt(position, direction);
            mesh->SetAngles(angles);
        } else {
            ANG angles = m_localRotation;
            angles.Rotate(m_angles);
            mesh->SetAngles(angles);
        }
    } else {
        if (m_position != m_lastPosition) {
            mesh->SetPosition(m_position);
            m_lastPosition = m_position;
        }
        m_c0 = m_position;
        if (m_scale != m_lastScale) {
            mesh->SetScale(m_scale);
            m_lastScale = m_scale;
        }
        if (master->m_alwaysLookCamera) {
            camera = (CAMERA *)g_pCamera;
            V3 direction = camera->m_direction * -1.0f;
            V3 position = camera->m_up;
            if (master->m_simulate3D) {
                V3 forward = m_angles.GetDirection();
                MATRIX m = g_pCamera->GetWorldAngles().GetMatrix();
                m.Invert();
                V3 v = forward * m;
                v.z = 0.0f;
                if (!v.IsZero()) {
                    v.Normalize();
                    MATRIX r;
                    r.Identity();
                    r.RotateAxis(-((float)atan2(v.y, v.x) * 57.29578f), direction);
                    position = position * r;
                    ANG angles(0.0f, 0.0f, 0.0f);
                    angles.SetLookAt(position, direction);
                    mesh->SetAngles(angles);
                }
            } else {
                float roll = GetRoll();
                if (roll != 0.0f && !master->m_neverRotate) {
                    MATRIX m;
                    m.Identity();
                    m.RotateAxis(roll, direction);
                    position = position * m;
                }
                ANG angles(0.0f, 0.0f, 0.0f);
                angles.SetLookAt(position, direction);
                mesh->SetAngles(angles);
            }
        } else if (m_angles != m_134 || !master->m_rotationSpeed.IsZero() || !m_localRotation.IsZero()) {
            ANG angles = m_angles;
            angles.Rotate(m_localRotation);
            mesh->SetAngles(angles);
            m_134 = m_angles;
        }
    }
    if (m_layer != m_lastLayer) {
        m_lastLayer = m_layer;
        mesh->SetLayer(m_layer);
    }
    if (master->m_decalOrderForced >= 0)
        m_frame = master->m_decalOrderForced;
    if (m_scrolling) {
        float du = m_scrollSpeed.x * g_pTimer->m_dt;
        float dv = m_scrollSpeed.y * g_pTimer->m_dt;
        SetUV(du + m_uv[0].u, dv + m_uv[0].v, du + m_uv[1].u, dv + m_uv[1].v);
        return;
    }
    if (m_frame != m_lastFrame) {
        SHADER *shader = (SHADER *)g_pResources->GetResource(master->m_shaderHandle);
        if (shader) {
            UVRECT rect = shader->GetFrame(m_frame);
            SetUV(rect.left, rect.top, rect.right, rect.bottom);
        }
    }
}

DECALMASTER::DECALMASTER(RESOURCEMANAGER *manager, const char *name)
    : RESOURCEMASTER(manager, name), m_localRotation(0.0f, 0.0f, 0.0f), m_rotationSpeed(0.0f, 0.0f, 0.0f)
{
    m_shader = "";
    m_decalOrderForced = -1;
    m_localRotation = ANG(0.0f, 0.0f, 0.0f);
    m_alwaysLookCamera = true;
    m_neverRotate = false;
    m_simulate3D = false;
    m_animLen = 0.0f;
    m_startFrame = 0;
    m_lastFrame = 0;
    m_loop = false;
    m_fwBk = false;
    m_randomStartFrame = false;
    m_rotationSpeed = V3(0.0f, 0.0f, 0.0f);
    m_numUsers = 0;
    m_shaderLoaded = false;
    SetShader(name);
}

RESOURCE *DECALMASTER::CreateCopy(const RESHANDLE &handle)
{
    return new DECAL(this, handle);
}

SRESULT DECALMASTER::Execute(CMDENTRY *command, TOKENS **args)
{
    return (this->*(SRESULT (DECALMASTER::*)(TOKENS **))command->func)(args);
}

RESOURCEMASTER *DECALMANAGER::CreateMaster(const char *name)
{
    return new DECALMASTER(this, name);
}
