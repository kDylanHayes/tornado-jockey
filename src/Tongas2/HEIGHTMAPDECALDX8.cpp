#define HEIGHTMAP_BOXES
#define FRUSTUM_MATHS
#include "Tongas2/HEIGHTMAPDECALDX8.h"

#include <stdlib.h>

#include "Tongas2/CAMERA.h"
#include "Tongas2/LOG.h"
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

HEIGHTMAPDX8 *HEIGHTMAPDECALDX8RESOURCE::FindHeightmap()
{
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
