#include "Tongas2/MESHDX8SD.h"
#include "Tongas2/RENDERER.h"

struct MESHUNKNOWN
{
    virtual void __stdcall QueryInterface();
    virtual void __stdcall AddRef();
    virtual void __stdcall Release();
};

class CAMERAMATRIX
{
public:
    void *GetMatrix();
};

extern int g_numTriangles;

RESOURCEMANAGER *CreateMESHDX8SD()
{
    return new MESHDX8SDMANAGER;
}

RESOURCEMASTER *MESHDX8SDMANAGER::CreateMaster(const char *name)
{
    return new MESHDX8SDMASTER(this, name);
}

RESOURCE *MESHDX8SDMASTER::CreateCopy(const RESHANDLE &handle)
{
    return new MESHDX8SDRESOURCE(this, handle);
}

MESHDX8SDMASTER::MESHDX8SDMASTER(RESOURCEMANAGER *manager, const char *name) : MESHMASTER(manager, name)
{
    {
        HEIGHTMAPTEMP temp;
    }
    m_b9 = 0;
    m_c0 = 0;
    m_bc = 0;
    m_c4 = 0;
    m_c8 = 0;
    m_first = 0;
    m_last = 0;
    m_b4 = 0;
    m_ac = 0;
    m_b0 = 0;
    m_a4 = 0;
    m_a8 = 0;
    m_b8 = 1;
}

void MESHDX8SDMASTER::ReleaseVertexBuffer()
{
    if (m_c0) {
        ((MESHUNKNOWN *)m_c0)->Release();
        m_c0 = 0;
    }
}

void MESHDX8SDMASTER::LockVertexBuffer()
{
    m_b9 = 0;
    if (m_a4 > 0) {
        IDirect3DDevice8 *device = g_pRenderer->GetDevice();
        device->CreateVertexBuffer(m_a4 * 0x18, 0, 0x142, 1, (IDirect3DVertexBuffer8 **)&m_c0);
    }
}

void MESHDX8SDMASTER::UpdateCopies()
{
    RESOURCEMASTER::UpdateCopies();
    int count = m_numCopies / 10;
    if (count < 100)
        count = 100;
    if (m_numCopies - m_numUsed > count)
        TrimCopies();
    if (m_b8) {
        Stop();
        return;
    }
    Update(false);
}

void MESHDX8SDMASTER::Render()
{
    if (!m_bc || !m_c0 || !m_b9 || m_a4 < 1 || m_a8 < 1)
        return;

    g_pRenderer->SetTongaShader(m_shader);

    int matrix[16];
    int *source = (int *)((CAMERAMATRIX *)*(void **)((char *)g_pCamera + 0xda4))->GetMatrix();
    memcpy(matrix, source, sizeof(matrix));
    g_pRenderer->GetDevice()->SetTransform(D3DTS_VIEW, (const D3DMATRIX *)matrix);

    RESOURCE *resource = g_pResources->GetResource(m_shader);
    bool fog = false;
    if (*(char *)((char *)g_pRenderer + 0x303e) && resource && resource->m_master) {
        int shaderType = *(int *)((char *)resource->m_master + 0x70);
        if (shaderType == 2) {
            V3I color(0xff, 0xff, 0xff);
            g_pRenderer->SetFogColor(&color);
            fog = true;
        } else if (shaderType == 3 || shaderType == 4) {
            V3I color(0, 0, 0);
            g_pRenderer->SetFogColor(&color);
            fog = true;
        }
    }

    IDirect3DDevice8 *device = g_pRenderer->GetDevice();
    device->SetVertexShader(0x142);
    device = g_pRenderer->GetDevice();
    device->SetStreamSource(0, (IDirect3DVertexBuffer8 *)m_c0, 0x18);
    device = g_pRenderer->GetDevice();
    device->SetIndices((IDirect3DIndexBuffer8 *)m_bc, 0);
    device = g_pRenderer->GetDevice();
    device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, m_a4, 0, m_a8);
    g_numTriangles += m_a8;

    if (resource && resource->m_master && *(char *)((char *)resource->m_master + 0x79)) {
        if (!*(char *)((char *)g_pRenderer + 0x3016)) {
            device = g_pRenderer->GetDevice();
            device->SetRenderState(D3DRS_FILLMODE, 2);
        }
        device = g_pRenderer->GetDevice();
        device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, m_a4, 0, m_a8);
        if (!*(char *)((char *)g_pRenderer + 0x3016)) {
            device = g_pRenderer->GetDevice();
            device->SetRenderState(D3DRS_FILLMODE, 3);
        }
        g_numTriangles += m_a8;
    }

    if (fog)
        g_pRenderer->SetFogColor(0);
}

void MESHDX8SDMASTER::Stop()
{
    m_b8 = 0;
    if (m_bc) {
        ((IDirect3DIndexBuffer8 *)m_bc)->Release();
        m_bc = 0;
    }
    ReleaseVertexBuffer();
    HEIGHTMAPTEMP::SetEnabled(true);

    int indices = m_b0 * m_numCopies;
    int vertices = m_ac * m_numCopies;
    m_a8 = indices;
    m_a4 = vertices;
    if (indices > 0) {
        IDirect3DDevice8 *device = g_pRenderer->GetDevice();
        device->CreateIndexBuffer(indices * 6, 8, 0x65, 1, (IDirect3DIndexBuffer8 **)&m_bc);
        unsigned short *data = 0;
        if (((IDirect3DIndexBuffer8 *)m_bc)->Lock(0, 0, (BYTE **)&data, 0) != 0) {
            if (g_pLog)
                g_pLog->Error("Error - could not lock index buffer");
            return;
        }
        int base = 0;
        int out = 0;
        for (MESHDX8SDRESOURCE *copy = m_first; copy; copy = copy->m_next) {
            copy->m_a4 = base;
            int count = m_b0 * 3;
            for (int i = 0; i < count; i++) {
                data[out] = ((unsigned short *)m_b4)[i] + (unsigned short)base;
                out++;
            }
            base += m_ac;
        }
        ((IDirect3DIndexBuffer8 *)m_bc)->Unlock();
        LockVertexBuffer();
        Update(true);
    }
}

MESHDX8SDMASTER::~MESHDX8SDMASTER()
{
    HEIGHTMAPTEMP::SetEnabled(false);
    if (m_bc) {
        ((MESHUNKNOWN *)m_bc)->Release();
        m_bc = 0;
    }
    if (m_c0) {
        ((MESHUNKNOWN *)m_c0)->Release();
        m_c0 = 0;
    }
}

const char *MESHDX8SDMANAGER::GetTypeCode()
{
    return "MESH";
}

void MESHDX8SDRESOURCE::Reset()
{
    m_flags = 0xffff;
    m_34.x = 0.0f;
    m_34.y = 0.0f;
    m_34.z = 0.0f;
    m_40.x = 0.0f;
    m_40.y = 0.0f;
    m_40.z = 0.0f;
    m_50.x = 0.0f;
    m_50.y = 0.0f;
    m_50.z = 0.0f;
    m_9c = true;
    m_a0 = 0;
    m_a8 = false;
    m_4c = -1;
    m_ac = m_b0 = m_b4 = 0xff;
}

MESHDX8SDRESOURCE::MESHDX8SDRESOURCE(MESHMASTER *master, const RESHANDLE &handle)
    : MESH(master, handle)
{
    m_34.x = m_34.y = m_34.z = 0.0f;
    m_40.x = m_40.y = m_40.z = 0.0f;
    m_50.x = m_50.y = m_50.z = 0.0f;
    m_9c = true;
    m_ac = 0;
    m_b0 = 0;
    m_b4 = 0;

    MESHDX8SDMASTER *m = (MESHDX8SDMASTER *)master;
    if (!m->m_first)
        m->m_first = this;
    if (!m->m_last) {
        m->m_last = this;
        m_prev = 0;
    } else {
        m_prev = m->m_last;
        m->m_last->m_next = this;
        m->m_last = this;
    }
    m_next = 0;
    m->m_b8 = 1;
    Reset();
}

MESHDX8SDRESOURCE::~MESHDX8SDRESOURCE()
{
    MESHDX8SDMASTER *m = (MESHDX8SDMASTER *)m_master;
    if (m->m_last == this)
        m->m_last = m_prev;
    if (m->m_first == this)
        m->m_first = m_next;
    if (m_prev)
        m_prev->m_next = m_next;
    if (m_next)
        m_next->m_prev = m_prev;
    m->m_b8 = 1;
    m_a0 = 0;
    m_a4 = -1;
}

void MESHDX8SDSETTER::Enable()
{
    Empty();
    m_temp.SetEnabled(true);
}

void MESHDX8SDSETTER::Disable()
{
    Empty();
    m_temp.SetEnabled(false);
}
