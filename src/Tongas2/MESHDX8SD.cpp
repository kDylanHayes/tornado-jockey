#include "Tongas2/MESHDX8SD.h"
#include "Tongas2/RENDERER.h"

struct MESHUNKNOWN
{
    virtual void __stdcall QueryInterface();
    virtual void __stdcall AddRef();
    virtual void __stdcall Release();
};

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
    m_first = 0;
    m_last = 0;
    m_a4 = 0;
    m_a8 = 0;
    m_ac = 0;
    m_b0 = 0;
    m_b4 = 0;
    m_b8 = 0;
    m_b9 = 0;
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

MESHDX8SDMASTER::~MESHDX8SDMASTER()
{
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
