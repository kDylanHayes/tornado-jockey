// Meshes: the base of the renderer plugin's mesh types. MESHMASTER holds on
// to the shader a mesh is drawn with while it is in use, MESH is one copy,
// and MESHMANAGER::CreateMesh finds the master for a shader, layer and
// suffix. The plugin replaces the manager through g_pfnCreateMESH; without
// it there is none. Placeholder names apart from the class names.
#define MESH_DEFINITIONS
#include "Tongas2/DECAL.h"
#include "Tongas2/LOG.h"

class MESHMASTER : public RESOURCEMASTER
{
public:
    MESHMASTER(RESOURCEMANAGER *manager, const char *name);
    virtual ~MESHMASTER();

    virtual RESOURCE *CreateCopy(const RESHANDLE &handle);
    // A twentieth of the copies asked for at a time, at least one.
    virtual int GetCopiesPerBatch()
    {
        int count = m_numCopies / 20;
        if (count < 1)
            count = 1;
        return count;
    }
    virtual void Virtual34() {}
    virtual void SetLayer(int layer) {}

    // Takes the shader into use, and lets it go.
    void AcquireShader();
    void ReleaseShader();
    // Changes the shader, letting the old one go.
    void SetShader(const RESHANDLE &shader);

    bool m_acquired;
    RESHANDLE m_shader;
};

static RESOURCEMANAGER *CreateMESH()
{
    if (g_pLog)
        g_pLog->Error("No default plugin for MESHMANAGER");
    return 0;
}

CREATEMANAGER g_pfnCreateMESH = CreateMESH;

MESHMASTER::MESHMASTER(RESOURCEMANAGER *manager, const char *name)
    : RESOURCEMASTER(manager, name)
{
    m_acquired = false;
}

void MESHMASTER::AcquireShader()
{
    SHADERMASTER *shader = (SHADERMASTER *)g_pResources->GetMaster(m_shader);
    if (shader) {
        shader->AddUser();
        m_acquired = true;
    } else if (g_pLog) {
        g_pLog->Error("mesh %s COULD NOT acquire shader!", m_name.c_str());
    }
}

void MESHMASTER::ReleaseShader()
{
    SHADERMASTER *shader = (SHADERMASTER *)g_pResources->GetMaster(m_shader);
    if (shader) {
        shader->RemoveUser();
        m_acquired = false;
    } else if (g_pLog) {
        g_pLog->Error("mesh %s COULD NOT release shader!", m_name.c_str());
    }
}

MESH::MESH(MESHMASTER *master, const RESHANDLE &handle)
    : RESOURCE(master, handle)
{
    m_flags = 0;
}

void MESH::SetVertices(DECALVERTEX *vertices, int numVertices, const short *indices, int numTriangles)
{
}

MESHMASTER::~MESHMASTER()
{
    if (m_acquired)
        ReleaseShader();
}

void MESHMASTER::SetShader(const RESHANDLE &shader)
{
    if (shader == m_shader)
        return;
    if (m_acquired)
        ReleaseShader();
    m_shader = shader;
    AcquireShader();
}

RESOURCE *MESHMASTER::CreateCopy(const RESHANDLE &handle)
{
    return new MESH(this, handle);
}

RESHANDLE MESHMANAGER::CreateMesh(RESHANDLE shader, int layer, const char *suffix)
{
    SHADER *copy = (SHADER *)g_pResources->GetResource(shader);
    if (!copy) {
        if (g_pLog)
            g_pLog->Error("Error - no shader to build mesh!");
        return RESHANDLE();
    }
    FIXEDSTRING<0x200> name;
    SafeSprintf(name, "%s_%d_%s", copy->m_master->m_name.c_str(), layer, suffix);
    MESHMASTER *master = (MESHMASTER *)GetMaster(name);
    if (!master) {
        if (g_pLog)
            g_pLog->Error("no master for mesh %s", (const char *)name);
        return RESHANDLE();
    }
    master->SetGroupMask(copy->m_master->m_groupMask);
    master->SetShader(shader);
    master->SetLayer(layer);
    return master->m_handle;
}
