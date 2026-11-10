// Tongas2/SIMPLEMODELDX8 -- the DX8 renderer plugin's simple models.
//
// SIMPLEMODELDX8RESOURCE builds a MESH for each SIMPLEMODEL: the custom
// vertex data the model's start call passes, or one of the named parts the
// master picks, then keeps the mesh in step with the object's placement.
#include "Tongas2/SIMPLEMODELDX8.h"

#include <string.h>

#include "Tongas2/FIXEDSTRING.h"

// The engine compares vectors and angles field by field; the header only
// declares the operators, so the definition is here.
inline bool operator!=(const V3 &a, const V3 &b)
{
    return a.x != b.x || a.y != b.y || a.z != b.z;
}

// The built-in meshes: their vertices and triangle indices.
extern "C" int DAT_006c2740;
extern "C" int DAT_006c2920;
extern "C" int DAT_006c2968;
extern "C" int DAT_006c2980;
extern "C" int DAT_006c2990;
extern "C" int DAT_006c2a08;
extern "C" int DAT_006c2b28;
extern "C" int DAT_006c3038;
extern "C" int DAT_006c3060;
extern "C" int DAT_006c32a0;
extern "C" int DAT_006c3378;
extern "C" int DAT_006c34e0;
extern "C" int DAT_006c37a8;
extern "C" int DAT_006c3b98;
extern "C" int DAT_006c3c58;
extern "C" int DAT_006c3cb8;
extern "C" int DAT_006c3d00;
extern "C" int DAT_006c3e50;
extern "C" int DAT_006c40c0;
extern "C" int DAT_006c4b10;
extern "C" int DAT_006c4bb8;
extern "C" int DAT_006c4eb8;
extern "C" int DAT_006c50f8;
extern "C" int DAT_006c53f8;

SIMPLEMODELDX8MASTER::SIMPLEMODELDX8MASTER(RESOURCEMANAGER *manager, const char *name)
    : SIMPLEMODELMASTER(manager, name)
{
    m_numResources = 0;
    m_shaderLoaded = false;
    FIXEDSTRING<0x200> shader("default");
    SetShader(shader);
}

SIMPLEMODELDX8MASTER::~SIMPLEMODELDX8MASTER()
{
    if (m_shaderLoaded) {
        SHADERMASTER *shader = (SHADERMASTER *)g_pResources->GetMaster(m_shader);
        if (shader) {
            shader->RemoveUser();
            m_shaderLoaded = false;
        }
    }
}

RESOURCE *SIMPLEMODELDX8MASTER::CreateCopy(const RESHANDLE &handle)
{
    return new SIMPLEMODELDX8RESOURCE(this, handle);
}

void SIMPLEMODELDX8MASTER::UpdateCopies()
{
    if (m_numResources <= 0) {
        if (m_shaderLoaded) {
            ReleaseShader();
            RESOURCEMASTER::UpdateCopies();
            return;
        }
        if (0 >= m_numResources)
            goto update;
    }
    if (!m_shaderLoaded)
        AcquireShader();
update:
    RESOURCEMASTER::UpdateCopies();
}

__declspec(noinline) void SIMPLEMODELDX8MASTER::AcquireShader()
{
    SHADERMASTER *shader = (SHADERMASTER *)g_pResources->GetMaster(m_shader);
    if (shader) {
        shader->AddUser();
        m_shaderLoaded = true;
    }
}

__declspec(noinline) void SIMPLEMODELDX8MASTER::ReleaseShader()
{
    SHADERMASTER *shader = (SHADERMASTER *)g_pResources->GetMaster(m_shader);
    if (shader) {
        shader->RemoveUser();
        m_shaderLoaded = false;
    }
}

void SIMPLEMODELDX8MASTER::SetShader(const char *name)
{
    bool changed = false;
    if (strcmp(name, m_shaderName) != 0 && m_shaderLoaded) {
        ReleaseShader();
        changed = true;
    }
    memcpy(m_shaderName.m_buf, name, 0x200);
    m_shader = g_pSHADER->GetHandle(m_shaderName);
    if (m_shader.serial < 0 || m_shader.index < 0) {
        strncpy(m_shaderName.m_buf, "", 0x1ff);
        m_shaderName.m_buf[0x1ff] = 0;
    }
    if (changed) {
        SHADERMASTER *shader = (SHADERMASTER *)g_pResources->GetMaster(m_shader);
        if (shader) {
            shader->AddUser();
            m_shaderLoaded = true;
        }
    }
}

SIMPLEMODELDX8RESOURCE::SIMPLEMODELDX8RESOURCE(SIMPLEMODELMASTER *master, const RESHANDLE &handle)
    : SIMPLEMODEL(master, handle)
{
    m_440 = false;
    m_441 = false;
    SIMPLEMODELDX8MASTER *dx8Master = (SIMPLEMODELDX8MASTER *)master;
    dx8Master->m_numResources++;
    if (dx8Master->m_numResources > 0 && !dx8Master->m_shaderLoaded)
        dx8Master->AcquireShader();
    m_customIndices = 0;
    m_customVertices = 0;
}

SIMPLEMODELDX8RESOURCE::~SIMPLEMODELDX8RESOURCE()
{
    if (m_customVertices) {
        delete[] m_customVertices;
        m_customVertices = 0;
    }
    if (m_customIndices != 0) {
        delete[] (char *)m_customIndices;
        m_customIndices = 0;
    }
    SIMPLEMODELDX8MASTER *dx8Master = (SIMPLEMODELDX8MASTER *)m_master;
    if (dx8Master)
        dx8Master->m_numResources--;
}

bool SIMPLEMODELDX8RESOURCE::SetCustomData(V3 *vertices, int numVertices, int numFaces)
{
    if (m_active)
        ReleaseMesh();
    if (m_customVertices) {
        delete[] m_customVertices;
        m_customVertices = 0;
    }
    if (m_customIndices) {
        delete[] (char *)m_customIndices;
        m_customIndices = 0;
    }
    if (numFaces <= 0 || numVertices <= 0) {
        if (g_pLog)
            g_pLog->Error("invalid custom data! faces %d vertex %d", numFaces, numVertices);
        return false;
    }
    m_numFaces = numFaces;
    m_numVertices = numVertices;
    m_customIndices = new char[numFaces * 6];
    m_customVertices = new DECALVERTEX[numVertices];
    float *src = (float *)vertices;
    int i = 0;
    if (numVertices >= 4) {
        for (; i <= numVertices - 4; i += 4) {
            m_customVertices[i].position.x = *src++;
            m_customVertices[i].position.y = *src++;
            m_customVertices[i].position.z = *src++;
            m_customVertices[i].color = -1;
            m_customVertices[i + 1].position.x = *src++;
            m_customVertices[i + 1].position.y = *src++;
            m_customVertices[i + 1].position.z = *src++;
            m_customVertices[i + 1].color = -1;
            m_customVertices[i + 2].position.x = *src++;
            m_customVertices[i + 2].position.y = *src++;
            m_customVertices[i + 2].position.z = *src++;
            m_customVertices[i + 2].color = -1;
            m_customVertices[i + 3].position.x = *src++;
            m_customVertices[i + 3].position.y = *src++;
            m_customVertices[i + 3].position.z = *src++;
            m_customVertices[i + 3].color = -1;
        }
    }
    for (; i < numVertices; i++) {
        m_customVertices[i].position.x = *src++;
        m_customVertices[i].position.y = *src++;
        m_customVertices[i].position.z = *src++;
        m_customVertices[i].color = -1;
    }
    return true;
}

void SIMPLEMODELDX8RESOURCE::ReleaseMesh()
{
    SIMPLEMODELMESH *mesh = (SIMPLEMODELMESH *)g_pResources->GetResource(m_mesh);
    if (mesh)
        mesh->Virtual20();
    m_mesh.Invalidate();
}

void SIMPLEMODELDX8RESOURCE::CreateMesh()
{
    FIXEDSTRING<0x200> name;
    SafeSprintf(name, "__spl_%s", ((SIMPLEMODELDX8MASTER *)m_master)->m_name.c_str());
    SIMPLEMODELMESHMANAGER *manager = (SIMPLEMODELMESHMANAGER *)g_pMESH;
    m_meshMaster = manager->CreateMesh(((SIMPLEMODELDX8MASTER *)m_master)->m_shader, m_19c, name);
}

void SIMPLEMODELDX8RESOURCE::AddMeshPart()
{
    SIMPLEMODELDX8MASTER *master = (SIMPLEMODELDX8MASTER *)m_master;
    if (!master) {
        if (g_pLog)
            g_pLog->Error("Error - SIMPLEMODELDX8RESOURCE::AddMeshPart> null master");
        return;
    }
    if (m_mesh.IsValid())
        ReleaseMesh();
    SIMPLEMODELMESH *mesh = (SIMPLEMODELMESH *)g_pResources->GetResource(m_meshMaster);
    if (!mesh) {
        CreateMesh();
        mesh = (SIMPLEMODELMESH *)g_pResources->GetResource(m_meshMaster);
    }
    if (mesh) {
    const char *part = master->m_name.c_str();
    if (strcmp(part, "box") == 0) {
        mesh->SetVertices((DECALVERTEX *)&DAT_006c3b98, 8, (const short *)&DAT_006c2920, 0xc);
    } else if (strcmp(part, "arrow") == 0) {
        mesh->SetVertices((DECALVERTEX *)&DAT_006c3d00, 0xe, (const short *)&DAT_006c2990, 0x14);
    } else if (strcmp(part, "plane") == 0) {
        mesh->SetVertices((DECALVERTEX *)&DAT_006c3c58, 4, (const short *)&DAT_006c2968, 4);
    } else if (strcmp(part, "tri") == 0) {
        mesh->SetVertices((DECALVERTEX *)&DAT_006c3cb8, 3, (const short *)&DAT_006c2980, 2);
    } else if (strcmp(part, "hexagon") == 0) {
        mesh->SetVertices((DECALVERTEX *)&DAT_006c4b10, 7, (const short *)&DAT_006c3038, 6);
    } else if (strcmp(part, "editorbox") == 0) {
        mesh->SetVertices((DECALVERTEX *)&DAT_006c4bb8, 0x20, (const short *)&DAT_006c3060, 0x60);
    } else if (strcmp(part, "cylinder") == 0) {
        mesh->SetVertices((DECALVERTEX *)&DAT_006c3e50, 0x1a, (const short *)&DAT_006c2a08, 0x30);
    } else if (strcmp(part, "cylinderZ") == 0) {
        mesh->SetVertices((DECALVERTEX *)&DAT_006c40c0, 0x6e, (const short *)&DAT_006c2b28, 0xd8);
    } else if (strcmp(part, "orientedExagon") == 0) {
        mesh->SetVertices((DECALVERTEX *)&DAT_006c4eb8, 0x18, (const short *)&DAT_006c32a0, 0x24);
    } else if (strcmp(part, "camera") == 0) {
        mesh->SetVertices((DECALVERTEX *)&DAT_006c53f8, 0x4a, (const short *)&DAT_006c34e0, 0x76);
    } else if (strcmp(part, "lookat") == 0) {
        mesh->SetVertices((DECALVERTEX *)&DAT_006c50f8, 0x20, (const short *)&DAT_006c3378, 0x3c);
    } else if (strcmp(part, "custom") == 0) {
        if (!m_customVertices || !m_customIndices) {
            if (g_pLog)
                g_pLog->Error("custom simple model without mesh data!!");
            return;
        }
        mesh->SetVertices(m_customVertices, m_numVertices, (const short *)m_customIndices,
                          m_numFaces);
    } else {
        mesh->SetVertices((DECALVERTEX *)&DAT_006c37a8, 0x2a, (const short *)&DAT_006c2740, 0x50);
    }
    mesh->SetActive(true);
    m_lastLayer = -1;
    m_lastPosition.Set(-1.0f, -1.0f, -1.0f);
    m_lastScale.Set(-1.0f, -1.0f, -1.0f);
    m_134 = ANG(-1.0f, -1.0f, -1.0f);
    m_mesh = mesh->m_handle;
    return;
    }
    if (g_pLog)
        g_pLog->Error("Error - could not create a mesh resource for simple model %s",
                      ((SIMPLEMODELDX8MASTER *)m_master)->m_name.c_str());
}

void SIMPLEMODELDX8RESOURCE::Start()
{
    SIMPLEMODELDX8RESOURCE::Stop();
    AddMeshPart();
}

void SIMPLEMODELDX8RESOURCE::Update()
{
    OBJECT::Update();
    SIMPLEMODELMESH *mesh = (SIMPLEMODELMESH *)g_pResources->GetResource(m_mesh);
    if (mesh) {
        if (m_441)
            mesh->SetColor(m_pivot);
        if (m_drawPosition != m_lastPosition) {
            mesh->SetPosition(m_drawPosition);
            m_lastPosition = m_drawPosition;
        }
        m_c0 = m_position;
        if (m_drawScale != m_lastScale) {
            mesh->SetScale(m_drawScale);
            m_lastScale = m_drawScale;
        }
        if (m_angles != m_134)
            mesh->SetAngles(m_angles);
        if (m_layer != m_lastLayer) {
            m_lastLayer = m_layer;
            mesh->SetLayer(m_layer);
        }
    }
}

bool SIMPLEMODELDX8RESOURCE::IsLoaded()
{
    if (!m_master) {
        if (g_pLog)
            g_pLog->Error("Error - SIMPLEMODELDX8RESOURCE::IsLoaded> null master");
        return false;
    }
    if (m_440)
        return true;
    RESOURCE *shader = g_pResources->GetResource(((SIMPLEMODELDX8MASTER *)m_master)->m_shader);
    if (shader) {
        ((SIMPLEMODELDX8MASTER *)m_master)->SetGroupMask(shader->m_master->m_groupMask);
        if (!shader->IsLoaded())
            return false;
    }
    CreateMesh();
    m_440 = true;
    return true;
}

void SIMPLEMODELDX8RESOURCE::SetLayer(int layer)
{
    if (layer != m_19c) {
        m_19c = layer;
        if (m_active)
            ReleaseMesh();
        CreateMesh();
        if (m_active)
            AddMeshPart();
    }
}

void SIMPLEMODELDX8RESOURCE::Virtual71(int vertices, int numVertices, int indices, int numFaces)
{
    if (SetCustomData((V3 *)vertices, numVertices, numFaces)) {
        int count = numFaces * 3;
        int i = 0;
        if (count > 0) {
            const short *src = (const short *)indices;
            do {
                ((short *)m_customIndices)[i] = *src;
                src += 2;
                i++;
            } while (i < count);
        }
        if (m_active)
            AddMeshPart();
    }
}

void SIMPLEMODELDX8RESOURCE::Virtual72(int vertices, int numVertices, int indices, int numFaces)
{
    if (SetCustomData((V3 *)vertices, numVertices, numFaces)) {
        memcpy(m_customIndices, (const void *)indices, numFaces * 3);
        if (m_active)
            AddMeshPart();
    }
}

RESOURCEMASTER *SIMPLEMODELDX8MANAGER::CreateMaster(const char *name)
{
    return new SIMPLEMODELDX8MASTER(this, name);
}
