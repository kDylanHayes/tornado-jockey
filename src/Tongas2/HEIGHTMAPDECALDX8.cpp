#include "lib/freeimage/FreeImage.h"

#define HEIGHTMAP_BOXES
#define FRUSTUM_MATHS
#include "Tongas2/HEIGHTMAPDECALDX8.h"

#include <stdlib.h>

#include "Tongas2/CAMERA.h"
#include "Tongas2/LIGHT.h"
#include "Tongas2/LOG.h"
#include "Tongas2/SCENE.h"
#include "Tongas2/RENDERSHADER.h"

int HEIGHTMAPDECALDX8MASTER::s_classId;

CMDENTRY HEIGHTMAPDECALDX8MASTER::s_commands[] = {
    { 0, 0, 0, 0, 0 },
};

PROPENTRY HEIGHTMAPDECALDX8MASTER::s_properties[] = {
    { 0, 0, 15, 0, 0, false },
};

int HEIGHTMAPDECALDX8RESOURCE::s_classId;

CMDENTRY HEIGHTMAPDECALDX8RESOURCE::s_commands[] = {
    { "SetHmapFromActor", 0, 2, 3, (CMDFUNC)&HEIGHTMAPDECALDX8RESOURCE::SetHmapFromActor },
    { 0, 0, 0, 0, 0 },
};

void HEIGHTMAPDECALDX8MASTER::RegisterCommands()
{
    g_classParents[s_classId] = RESOURCEMASTER::s_classId;
    g_classCommands[s_classId] = s_commands;
    HashCommands(s_commands);
    RegisterClassName(s_commands, "HEIGHTMAPDECALDX8MASTER", "RESOURCEMASTER");
}

void HEIGHTMAPDECALDX8RESOURCE::RegisterCommands()
{
    g_classParents[s_classId] = RESOURCE::s_classId;
    g_classCommands[s_classId] = s_commands;
    HashCommands(s_commands);
    RegisterClassName(s_commands, "HEIGHTMAPDECALDX8RESOURCE", "RESOURCE");
}


// Grows the box so that it holds point, one axis at a time.
void BOXCORNERS::Merge(const V3 &point)
{
    if (corners[0].x > point.x)
        corners[0].x = point.x;
    if (corners[0].y > point.y)
        corners[0].y = point.y;
    if (corners[0].z < point.z)
        corners[0].z = point.z;
    if (corners[1].x < point.x)
        corners[1].x = point.x;
    if (corners[1].y > point.y)
        corners[1].y = point.y;
    if (corners[1].z < point.z)
        corners[1].z = point.z;
    if (corners[2].x > point.x)
        corners[2].x = point.x;
    if (corners[2].y > point.y)
        corners[2].y = point.y;
    if (corners[2].z > point.z)
        corners[2].z = point.z;
    if (corners[3].x < point.x)
        corners[3].x = point.x;
    if (corners[3].y > point.y)
        corners[3].y = point.y;
    if (corners[3].z > point.z)
        corners[3].z = point.z;
    if (corners[4].x > point.x)
        corners[4].x = point.x;
    if (corners[4].y < point.y)
        corners[4].y = point.y;
    if (corners[4].z < point.z)
        corners[4].z = point.z;
    if (corners[5].x < point.x)
        corners[5].x = point.x;
    if (corners[5].y < point.y)
        corners[5].y = point.y;
    if (corners[5].z < point.z)
        corners[5].z = point.z;
    if (corners[6].x > point.x)
        corners[6].x = point.x;
    if (corners[6].y < point.y)
        corners[6].y = point.y;
    if (corners[6].z > point.z)
        corners[6].z = point.z;
    if (corners[7].x < point.x)
        corners[7].x = point.x;
    if (corners[7].y < point.y)
        corners[7].y = point.y;
    if (corners[7].z > point.z)
        corners[7].z = point.z;
}

// The matrix of the rotation by degrees about the Z axis.
void MATRIX::SetRotationZ(float degrees)
{
    _13 = 0.0f;
    _14 = 0.0f;
    float rad = -degrees;
    rad *= 0.017453292f;
    float c = (float)cos(rad);
    _23 = 0.0f;
    _24 = 0.0f;
    _31 = 0.0f;
    _32 = 0.0f;
    _33 = 1.0f;
    _34 = 0.0f;
    _41 = 0.0f;
    _42 = 0.0f;
    _43 = 0.0f;
    _44 = 1.0f;
    float s = (float)sin(rad);
    _11 = c;
    _12 = -s;
    _21 = s;
    _22 = c;
}

HEIGHTMAPDECALDX8MASTER::HEIGHTMAPDECALDX8MASTER(RESOURCEMANAGER *manager, const char *name)
    : RESOURCEMASTER(manager, name), m_size(0.0f, 0.0f)
{
    m_shader = "";
    m_size = V2(4.0f, 4.0f);
    m_70 = 1;
    m_userFolder = true;
    m_correctSizeInWidescreen = false;
    m_60 = false;
}

HEIGHTMAPDECALDX8MASTER::~HEIGHTMAPDECALDX8MASTER()
{
}

int HEIGHTMAPDECALDX8MASTER::GetClassId()
{
    return s_classId;
}

int HEIGHTMAPDECALDX8MASTER::GetParentClassId()
{
    return g_classParents[s_classId];
}

const char *HEIGHTMAPDECALDX8MASTER::GetClassName()
{
    return g_classNames[s_classId];
}

SRESULT HEIGHTMAPDECALDX8MASTER::Execute(CMDENTRY *command, TOKENS **args)
{
    return (this->*(SRESULT (HEIGHTMAPDECALDX8MASTER::*)(TOKENS **))command->func)(args);
}

void HEIGHTMAPDECALDX8MASTER::Prepare()
{
    m_60 = !(!m_userFolder && g_pResources->IsHighDetail() && !m_correctSizeInWidescreen);
    RESOURCEMASTER *shader = ((RESOURCEMANAGER *)g_pRENDERSHADER)->GetMaster(m_shader.c_str());
    if (shader)
        ((RENDERSHADERMASTER *)shader)->m_lightingOff = !m_60;
}

void HEIGHTMAPDECALDX8MASTER::UpdateCopies()
{
    bool hardware = (!m_userFolder && g_pResources->IsHighDetail() && !m_correctSizeInWidescreen) ? false : true;
    if (hardware != m_60) {
        HEIGHTMAPDECALDX8MASTER::Prepare();
        FreeCopiesMeshes();
    }
    RESOURCEMASTER::UpdateCopies();
}

void HEIGHTMAPDECALDX8MASTER::SetShader(const CSTRING &name)
{
    m_shader = name;
    FreeCopiesMeshes();
}

void HEIGHTMAPDECALDX8MASTER::SetSize(const V2 &size)
{
    m_size = size;
    FreeCopiesMeshes();
}

RESOURCE *HEIGHTMAPDECALDX8MASTER::CreateCopy(const RESHANDLE &handle)
{
    return new HEIGHTMAPDECALDX8RESOURCE(this, handle);
}

RESOURCEMASTER *HEIGHTMAPDECALDX8MANAGER::CreateMaster(const char *name)
{
    return new HEIGHTMAPDECALDX8MASTER(this, name);
}

RESOURCEMANAGER *CreateHEIGHTMAPDECALDX8()
{
    return new HEIGHTMAPDECALDX8MANAGER;
}

void HEIGHTMAPDECALDX8MASTER::FreeCopiesMeshes()
{
    HEIGHTMAPDECALDX8RESOURCE *copy = (HEIGHTMAPDECALDX8RESOURCE *)m_used;
    while (copy) {
        HEIGHTMAPDECALDX8RESOURCE *next = (HEIGHTMAPDECALDX8RESOURCE *)copy->m_next;
        copy->FreeMeshes();
        copy = next;
    }
}

HEIGHTMAPDECALDX8RESOURCE::HEIGHTMAPDECALDX8RESOURCE(HEIGHTMAPDECALDX8MASTER *master,
                                                     const RESHANDLE &handle)
    : OBJECT(master, handle), m_heightmap(0, 0, 0)
{
    HEIGHTMAPTEMP();
    m_heightmap = RESHANDLE(0xff, 0xff, 0xff);
    m_488 = -1;
    m_498 = -1;
    m_4e4 = -1;
    m_vertices = 0;
    m_normals = 0;
    m_480 = false;
    m_mesh = 0;
    m_48c = false;
    m_49c = false;
    m_4e0 = false;
}

HEIGHTMAPDECALDX8RESOURCE::~HEIGHTMAPDECALDX8RESOURCE()
{
    FreeMeshes();
    if (m_mesh) {
        delete m_mesh;
        m_mesh = 0;
    }
}

int HEIGHTMAPDECALDX8RESOURCE::GetClassId()
{
    return s_classId;
}

int HEIGHTMAPDECALDX8RESOURCE::GetParentClassId()
{
    return g_classParents[s_classId];
}

const char *HEIGHTMAPDECALDX8RESOURCE::GetClassName()
{
    return g_classNames[s_classId];
}

void HEIGHTMAPDECALDX8RESOURCE::FreeMeshes()
{
    if (m_mesh) {
        delete m_mesh;
        m_mesh = 0;
    }
    if (m_vertices) {
        delete[] m_vertices;
        m_vertices = 0;
    }
    if (m_normals) {
        delete[] m_normals;
        m_normals = 0;
    }
    m_480 = false;
}

void HEIGHTMAPDECALDX8RESOURCE::Start()
{
    SetVisibility(1);
    FreeMeshes();
}

void HEIGHTMAPDECALDX8RESOURCE::Stop()
{
    FreeMeshes();
    SetVisibility(1);
}

void HEIGHTMAPDECALDX8RESOURCE::Update()
{
    OBJECT::Update();
    if (!m_active)
        return;
    CreateMesh();
    HEIGHTMAPDX8 *heightmap = FindHeightmap();
    if (heightmap) {
        if (m_488 != heightmap->m_710) {
            m_488 = heightmap->m_710;
            m_48c = false;
        }
        if (m_498 != heightmap->m_matrixSerial) {
            m_498 = heightmap->m_matrixSerial;
            m_49c = false;
        }
    }
    if (m_scale.x != m_d8.x || m_scale.y != m_d8.y || m_scale.z != m_d8.z) {
        m_d8 = m_scale;
        m_48c = false;
    }
    if (m_angles != m_134) {
        m_134 = m_angles;
        m_48c = false;
    }
    if (m_position.x != m_c0.x || m_position.y != m_c0.y || m_position.z != m_c0.z) {
        m_c0 = m_position;
        m_48c = false;
    }
    if (m_layer != m_lastLayer) {
        m_lastLayer = m_layer;
        m_4e1 = false;
        m_4e0 = false;
    }
    if (((HEIGHTMAPDECALDX8MASTER *)m_master)->m_userFolder) {
        int count = ((LIGHTMANAGER *)g_pLIGHT)->GetMaxCount(m_1ac);
        if (m_4e4 != count) {
            m_4e4 = count;
            m_4e1 = false;
            m_4e0 = false;
        }
    }
    UpdateVertices();
    if (!m_49c) {
        heightmap = FindHeightmap();
        if (heightmap) {
            m_matrix = heightmap->m_matrix;
            m_4e0 = false;
            m_49c = true;
        }
    }
    UpdateVisibility();
    Render();
}

__declspec(noinline) void HEIGHTMAPDECALDX8RESOURCE::CreateMesh()
{
    if (m_480)
        return;
    HEIGHTMAPDECALDX8MASTER *master = (HEIGHTMAPDECALDX8MASTER *)m_master;
    int count = m_400;
    if (count != 0) {
        m_404 = count;
    } else {
        m_404 = (int)master->m_size.x;
        count = (int)master->m_size.y;
    }
    m_408 = count;
    m_vertices = new PRIMV3[m_404 * count];
    m_normals = new PRIMV3[m_404 * m_408];
    m_40c = m_404 * m_408;
    m_410 = (m_408 * 2 - 2) * (m_404 - 1);
    m_mesh = new MESHDX8;
    bool hardware = !master->m_userFolder && g_pResources->IsHighDetail() &&
                    !master->m_correctSizeInWidescreen;
    if (hardware)
        m_mesh->Create(3, m_40c, m_410, 1);
    else
        m_mesh->Create(1, m_40c, m_410, 1);
    m_mesh->SetVisibility(m_19c);
    m_mesh->m_passes = master->m_70;
    m_mesh->SetLightMask(m_1ac);
    RESOURCE *shader = g_pRENDERSHADER->Find(master->m_shader.c_str());
    if (shader) {
        m_mesh->AddSubmesh(shader->m_handle, 0, m_40c, 0, m_410, 0, 0);
        g_pRENDERSHADER->Get(shader->m_handle);
    } else {
        if (g_pLog)
            g_pLog->Error("Shader not found!");
    }
    short *indices = (short *)m_mesh->LockIndices(0, m_410);
    if (!indices) {
        if (g_pLog)
            g_pLog->Error("Could not lock face buffer %s!", master->m_name.c_str());
    } else {
        short base = 0;
        for (int y = 0; y < m_408 - 1; y++) {
            for (int x = 0; x < m_404 - 1; x++) {
                *indices++ = base;
                *indices++ = base + 1;
                *indices++ = (short)m_408 + base;
                *indices++ = base + 1;
                *indices++ = (short)m_408 + base + 1;
                *indices++ = (short)m_408 + base;
                base++;
            }
            base++;
        }
        m_mesh->UnlockIndices();
        if (hardware) {
            char *vertices = (char *)m_mesh->LockVertices(0, m_40c);
            if (!vertices) {
                if (g_pLog)
                    g_pLog->Error("could not lock vertex buffer! %s", master->m_name.c_str());
                return;
            }
            for (int y = 0; y < m_408; y++) {
                for (int x = 0; x < m_404; x++) {
                    *(int *)(vertices + 0x18) = -1;
                    *(float *)(vertices + 0x1c) = (float)x / (float)(m_404 - 1);
                    *(float *)(vertices + 0x20) = (float)y / (float)(m_408 - 1);
                    vertices += 0x24;
                }
            }
        } else {
            char *vertices = (char *)m_mesh->LockVertices(0, m_40c);
            if (!vertices) {
                if (g_pLog)
                    g_pLog->Error("could not lock vertex buffer! %s", master->m_name.c_str());
                return;
            }
            for (int y = 0; y < m_408; y++) {
                for (int x = 0; x < m_404; x++) {
                    *(int *)(vertices + 0xc) = -1;
                    *(float *)(vertices + 0x10) = (float)x / (float)(m_404 - 1);
                    *(float *)(vertices + 0x14) = (float)y / (float)(m_408 - 1);
                    vertices += 0x18;
                }
            }
        }
        m_mesh->UnlockVertices();
        m_48c = false;
        m_mesh->SetLit(true);
        m_480 = true;
    }
}

__declspec(noinline) void HEIGHTMAPDECALDX8RESOURCE::UpdateVertices()
{
    if (m_48c)
        return;
    m_4e0 = false;
    m_4e1 = false;
    HEIGHTMAPDX8 *heightmap = FindHeightmap();
    if (!heightmap)
        return;
    HEIGHTMAPDX8MASTER *master = heightmap->Master();

    V3 origin = heightmap->WorldToLocal(GetPosition());
    origin.z = 0.0f;
    float stepX = GetScale().x / (float)master->m_tilesX;
    float aspect = master->m_aspect * master->m_aspectRatioModifier;
    float stepY = GetScale().y * (float)master->m_tilesX / (float)master->m_tilesY;

    MATRIX rotation;
    rotation.SetRotationZ(GetAngles().GetEuler().x);
    V3 xAxis = V3(stepX, 0.0f, 0.0f) * rotation;
    V3 yAxis = V3(0.0f, stepY * aspect, 0.0f) * rotation;
    V3 base = origin - (xAxis + yAxis) * 0.5f;
    float zRange = master->m_maxHeight * 100.0f;
    float minZ = -zRange;
    float maxZ = zRange;

    for (int y = 0; y < m_408; y++) {
        float v = (m_408 > 1) ? (float)y / (float)(m_408 - 1) : 0.0f;
        for (int x = 0; x < m_404; x++) {
            float u = (m_404 > 1) ? (float)x / (float)(m_404 - 1) : 0.0f;
            V3 point = base + xAxis * u + yAxis * v;
            point.z += zRange;
            point = heightmap->GetGroundPoint(point);
            V3 normal = heightmap->GetNormal(point);
            int index = m_404 * y + x;
            m_vertices[index] = point;
            m_normals[index] = normal;
            if (point.z < minZ)
                minZ = point.z;
            if (point.z > maxZ)
                maxZ = point.z;
        }
    }
    m_box.Set(m_vertices[0], m_vertices[0]);
    m_box.Merge(m_vertices[m_404 - 1]);
    m_box.Merge(m_vertices[(m_408 - 1) * m_404]);
    m_box.Merge(m_vertices[m_404 * m_408 - 1]);
    m_box.ExtendHeight(minZ);
    m_box.ExtendHeight(maxZ);
    m_48c = true;
}

__declspec(noinline) void HEIGHTMAPDECALDX8RESOURCE::Render()
{
    if (m_4e0)
        return;
    HEIGHTMAPDX8 *heightmap = FindHeightmap();
    if (!heightmap)
        return;
    HEIGHTMAPDECALDX8MASTER *master = (HEIGHTMAPDECALDX8MASTER *)m_master;
    D3DXMATRIX matrix = m_matrix;
    m_mesh->SetMatrix(&matrix);
    MATRIX heightmapMatrix = heightmap->GetAngles().GetMatrix();
    if (!m_4e1) {
        char *out = (char *)m_mesh->LockVertices(0, m_40c);
        if (!out) {
            if (g_pLog)
                g_pLog->Error("could not lock vertex buffer! %s", master->m_name.c_str());
            return;
        }
        for (int y = 0; y < m_408; y++) {
            for (int x = 0; x < m_404; x++) {
                int index = m_404 * y + x;
                V3 &position = m_vertices[index];
                V3 normal = m_normals[index];
                if (!master->m_userFolder && g_pResources->IsHighDetail() &&
                    !master->m_correctSizeInWidescreen) {
                    *(unsigned int *)(out + 0x18) =
                        (((*(int *)&m_heightmap & 0xff) | m_layer << 8) << 8 |
                         (*(int *)((char *)&m_heightmap + 4) & 0xff)) << 8 |
                        (*(int *)((char *)&m_heightmap + 8) & 0xff);
                    *(V3 *)(out + 0xc) =
                        V3(normal.x * m_scale.x, normal.y * m_scale.y, normal.z * m_scale.z);
                    *(V3 *)out = position;
                    out += 0x24;
                } else {
                    V3 lightNormal(heightmapMatrix._11 * normal.x + heightmapMatrix._21 * normal.y +
                                       heightmapMatrix._31 * normal.z + heightmapMatrix._41,
                                   heightmapMatrix._12 * normal.x + heightmapMatrix._22 * normal.y +
                                       heightmapMatrix._32 * normal.z + heightmapMatrix._42,
                                   heightmapMatrix._13 * normal.x + heightmapMatrix._23 * normal.y +
                                       heightmapMatrix._33 * normal.z + heightmapMatrix._43);
                    LIGHTRGB lit;
                    if (!master->m_correctSizeInWidescreen) {
                        LIGHTRGB ambient = { 0xff, 0xff, 0xff };
                        ((LIGHTMANAGER *)g_pLIGHT)
                            ->LightVertex(ambient, *(V3I *)&m_heightmap, lightNormal, m_1ac, lit);
                    } else {
                        lit.r = 0xff;
                        lit.g = 0xff;
                        lit.b = 0xff;
                    }
                    *(unsigned int *)(out + 0xc) =
                        (((lit.r & 0xff) | m_layer << 8) << 8 | (lit.g & 0xff)) << 8 |
                        (lit.b & 0xff);
                    *(V3 *)out = position;
                    out += 0x18;
                }
            }
        }
        m_mesh->UnlockVertices();
    }
    m_4e1 = true;
    m_4e0 = true;
}

void HEIGHTMAPDECALDX8RESOURCE::SetLayer(int layer)
{
    m_19c = layer;
    if (m_mesh)
        m_mesh->SetVisibility(layer);
}

void HEIGHTMAPDECALDX8RESOURCE::SetFrame(int frame)
{
    m_400 = frame;
    FreeMeshes();
}

void HEIGHTMAPDECALDX8RESOURCE::Virtual30()
{
    m_488 = -1;
    m_498 = -1;
    m_heightmap = RESHANDLE(0xff, 0xff, 0xff);
    m_49c = false;
    OBJECT::Init();
}

SRESULT HEIGHTMAPDECALDX8RESOURCE::SetHmapFromActor(TOKENS **args)
{
    const char *name = args[0]->GetToken(1);
    const char *index = args[0]->GetToken(2);
    int resource = atoi(index);
    ACTOR *actor = (ACTOR *)g_pScene->FindActor(name);
    if (actor) {
        m_actor = actor->m_handle;
        m_500 = resource;
    }
    return SRESULT(true);
}

HEIGHTMAPDX8 *HEIGHTMAPDECALDX8RESOURCE::FindHeightmap()
{
    RESOURCE *resource = g_pResources->GetResource(m_hmap);
    if (resource)
        return (HEIGHTMAPDX8 *)resource;

    m_hmap.Invalidate();
    m_48c = false;
    m_49c = false;
    RESHANDLE &actorHandle = m_actor;
    if (actorHandle.serial >= 0 && actorHandle.index >= 0) {
        ACTOR *actor = (ACTOR *)g_pScene->GetActor(actorHandle);
        if (actor) {
            if (m_500 >= 0) {
                RESOURCE *copy = g_pResources->GetResource(actor->m_copies[m_500]);
                if (copy) {
                    int managerId = g_pHEIGHTMAPDX8->m_id;
                    if (copy->GetManagerId() == managerId)
                        return (HEIGHTMAPDX8 *)copy;
                }
                if (g_pLog)
                    g_pLog->Error("resource idx of actor specified for decal has no active heightmap!");
            } else {
                int managerId = g_pHEIGHTMAPDX8->m_id;
                RESOURCE *copy = actor->FindResourceCopy(managerId);
                if (copy)
                    return (HEIGHTMAPDX8 *)copy;
                if (g_pLog)
                    g_pLog->Error("actor specified for decal has no active heightmap!");
            }
        } else {
            m_actor.Invalidate();
        }
    }

    RESOURCE *editing = g_pResources->GetResource(((HEIGHTMAPDX8MANAGER *)g_pHEIGHTMAPDX8)->m_editing);
    if (editing) {
        m_hmap = editing->m_handle;
        return (HEIGHTMAPDX8 *)editing;
    }
    return 0;
}

void HEIGHTMAPDECALDX8RESOURCE::UpdateVisibility()
{
    FRUSTUM *frustum = ((CAMERA *)g_pCamera)->m_lens->GetFrustum();
    BOXCORNERS box = m_box;
    box.TransformAffine(m_matrix);
    BOXCORNERS transformed = box;
    bool lit = frustum->Classify(&transformed) != 2;
    if (m_mesh)
        m_mesh->SetLit(lit);
}
