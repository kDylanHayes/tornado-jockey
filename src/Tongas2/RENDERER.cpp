// The base of the renderer plugin: see RENDERER.h. Placeholder names apart
// from the class name.
// SHADER.h brings in FreeImage.h, which has to come before windows.h.
#include "Tongas2/SHADER.h"
#include "Tongas2/APPWINDOW.h"
#include "Tongas2/LOG.h"
#include "Tongas2/RENDERER.h"
#include "Tongas2/RENDERSHADER.h"
#include "Tongas2/RESOURCES.h"

// The capabilities GetCap reads, by index.
const char g_capNames[][30] = {
    "TextureWidth",
    "TextureHeight",
    "MaxLights",
    "MaxTextures",
    "MaxBlends",
    "CanNormalizeNormals",
    "",
};

// Defined in the plugin: the window the device draws in.
class DX8WINDOW : public APPWINDOW
{
public:
    int m_2c;
    // Triangles drawn last frame.
    int m_numTriangles;
    int m_34;
    V3I m_clearColor;

    V3I GetClearColor() { return m_clearColor; }
};

static inline D3DCOLOR ToColor(V3I color)
{
    return (color.x << 16) | (color.y << 8) | color.z;
}

// Defined in the plugin: the shader of a tonga, as the renderer sets it.
class TONGASHADER : public SHADER
{
public:
    // Sets the shader's states on the device.
    virtual void Apply();
};

// Defined in the plugin.
class DX8CAMERA
{
public:
    const D3DMATRIX *GetProjection();
};

// Defined in the plugin.
extern int g_numTriangles;
extern int g_9fef44;

static RENDERER *CreateRENDERER()
{
    if (g_pLog)
        g_pLog->Error("No default plugin for RENDERER");
    return 0;
}

RENDERER *(*g_pfnCreateRENDERER)() = CreateRENDERER;

RENDERER *g_pRenderer;
D3DXMATRIX *g_pIdentityMatrix;

bool RENDERER::Init()
{
    if (!SpecificInit()) {
        if (g_pLog)
            g_pLog->Error("RENDERER::Init - Could not do specific initialization");
        return false;
    }
    if (!CreateDevice()) {
        if (g_pLog)
            g_pLog->Error("RENDERER::Init - Could not create graphics device");
        return false;
    }
    if (!DetectCaps()) {
        if (g_pLog)
            g_pLog->Error("RENDERER::Init - Could not detect graphics capabilities");
        return false;
    }
    m_initialized = true;
    return true;
}

int RENDERER::GetMaxTextureWidth()
{
    return m_caps[0];
}

int RENDERER::GetMaxTextureHeight()
{
    return m_caps[1];
}

void RENDERER::SetMipmapBias(float bias)
{
    if (m_initialized) {
        float value = bias;
        IDirect3DDevice8 *device = GetDevice();
        device->SetTextureStageState(0, D3DTSS_MIPMAPLODBIAS, *(DWORD *)&value);
        device->SetTextureStageState(1, D3DTSS_MIPMAPLODBIAS, *(DWORD *)&value);
        device->SetTextureStageState(2, D3DTSS_MIPMAPLODBIAS, *(DWORD *)&value);
    }
}

void RENDERER::ResetStates()
{
    IDirect3DDevice8 *device = GetDevice();
    device->SetTransform(D3DTS_WORLD, g_pIdentityMatrix);
    D3DMATERIAL8 material;
    memset(&material, 0, sizeof(material));
    material.Emissive.r = material.Diffuse.r = material.Ambient.r = 1.0f;
    material.Emissive.g = material.Diffuse.g = material.Ambient.g = 1.0f;
    material.Emissive.b = material.Diffuse.b = material.Ambient.b = 1.0f;
    material.Emissive.a = material.Diffuse.a = material.Ambient.a = 1.0f;
    device->SetMaterial(&material);
    device->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
    device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
    device->SetRenderState(D3DRS_ZENABLE, TRUE);
    device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
    device->SetRenderState(D3DRS_STENCILENABLE, FALSE);
    device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
    device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);
    device->SetRenderState(D3DRS_LIGHTING, TRUE);
    m_lighting = true;
    for (int i = 0; i < 8; i++) {
        device->SetTextureStageState(i, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);
        device->SetTextureStageState(i, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
        device->SetTextureStageState(i, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
        device->SetTextureStageState(i, D3DTSS_MAXMIPLEVEL, 0);
        device->SetTextureStageState(i, D3DTSS_TEXTURETRANSFORMFLAGS, 0);
        device->SetTextureStageState(i, D3DTSS_TEXCOORDINDEX, 0);
        device->SetTexture(i, 0);
        device->SetTransform((D3DTRANSFORMSTATETYPE)(D3DTS_TEXTURE0 + i), g_pIdentityMatrix);
        device->SetTextureStageState(i, D3DTSS_COLORARG1, D3DTA_TEXTURE);
        device->SetTextureStageState(i, D3DTSS_COLORARG2, D3DTA_CURRENT);
        DWORD op = D3DTOP_MODULATE;
        if (i > 0)
            op = D3DTOP_DISABLE;
        device->SetTextureStageState(i, D3DTSS_COLOROP, op);
        if (i > 0) {
            device->SetTextureStageState(i, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
            device->SetTextureStageState(i, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
            device->SetTextureStageState(i, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
        } else {
            device->SetTextureStageState(i, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
            device->SetTextureStageState(i, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
            device->SetTextureStageState(i, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
        }
    }
}

int RENDERER::GetCullMode()
{
    return m_cullMode;
}

void RENDERER::Set3020(const bool &on)
{
    m_3020 = on;
}

D3DXPLANE *RENDERER::GetLightPlane(D3DXPLANE *out, int unused)
{
    V3 pos = m_lightPos;
    D3DXVECTOR3 position(pos.x, pos.y, pos.z);
    D3DXVECTOR3 direction(m_lightDir.x, m_lightDir.y, m_lightDir.z);
    return D3DXPlaneFromPointNormal(out, &position, &direction);
}

void RENDERER::SetFog(bool enable, V3I color, float start, float end)
{
    m_fogEnabled = enable;
    m_fogColor = D3DXCOLOR(color.x * (1.0f / 255.0f), color.y * (1.0f / 255.0f), color.z * (1.0f / 255.0f), 1.0f);
    m_fogStart = start;
    m_fogEnd = end;
}

void RENDERER::SetFogDirect(bool enable, V3I color, float start, float end)
{
    IDirect3DDevice8 *device = GetDevice();
    device->SetRenderState(D3DRS_FOGENABLE, enable);
    if (enable) {
        device->SetRenderState(D3DRS_FOGCOLOR, D3DXCOLOR(color.x * (1.0f / 255.0f), color.y * (1.0f / 255.0f), color.z * (1.0f / 255.0f), 1.0f));
        if ((m_d3dCaps.RasterCaps & D3DPRASTERCAPS_FOGTABLE) && (m_d3dCaps.RasterCaps & D3DPRASTERCAPS_ZFOG))
            device->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_LINEAR);
        else
            device->SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR);
        device->SetRenderState(D3DRS_FOGSTART, *(DWORD *)&start);
        device->SetRenderState(D3DRS_FOGEND, *(DWORD *)&end);
    }
}

void RENDERER::SetFogColor(const V3I *color)
{
    if (!color) {
        IDirect3DDevice8 *device = g_pRenderer->GetDevice();
        device->SetRenderState(D3DRS_FOGCOLOR, m_fogColor);
    } else {
        IDirect3DDevice8 *device = g_pRenderer->GetDevice();
        device->SetRenderState(D3DRS_FOGCOLOR, D3DXCOLOR(color->x * (1.0f / 255.0f), color->y * (1.0f / 255.0f), color->z * (1.0f / 255.0f), 1.0f));
    }
}

void RENDERER::ApplyFog()
{
    IDirect3DDevice8 *device = GetDevice();
    device->SetRenderState(D3DRS_FOGENABLE, m_fogEnabled);
    if (m_fogEnabled) {
        device->SetRenderState(D3DRS_FOGCOLOR, m_fogColor);
        if ((m_d3dCaps.RasterCaps & D3DPRASTERCAPS_FOGTABLE) && (m_d3dCaps.RasterCaps & D3DPRASTERCAPS_ZFOG))
            device->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_LINEAR);
        else
            device->SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR);
        device->SetRenderState(D3DRS_FOGSTART, *(DWORD *)&m_fogStart);
        device->SetRenderState(D3DRS_FOGEND, *(DWORD *)&m_fogEnd);
    }
}

bool RENDERER::DrawTriangles(int minIndex, int numVertices, int startTriangle, int numTriangles)
{
    if (numVertices > 0 && numTriangles > 0) {
        GetDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, minIndex, numVertices, startTriangle * 3, numTriangles);
        g_numTriangles += numTriangles;
        return true;
    }
    return false;
}

int RENDERER::SetTexCoordIndex(int stage, int index)
{
    int old = m_texCoordIndex[stage];
    m_texCoordIndex[stage] = index;
    GetDevice()->SetTextureStageState(stage, D3DTSS_TEXCOORDINDEX, index);
    return old;
}

int RENDERER::GetCap(int cap)
{
    if (cap >= 0 && cap < 6)
        return m_caps[cap];
    return 0;
}

void RENDERER::SetLighting(bool on)
{
    m_lighting = on;
    GetDevice()->SetRenderState(D3DRS_LIGHTING, on);
}

void RENDERER::SetNormalizeNormals(bool on)
{
    if (m_normalize != on) {
        if (m_canNormalize && on)
            GetDevice()->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
        else
            GetDevice()->SetRenderState(D3DRS_NORMALIZENORMALS, FALSE);
        m_normalize = on;
    }
}

void RENDERER::Destroy()
{
    if (!m_initialized)
        return;
    m_initialized = false;
    int i;
    VERTEXBUFFER **vb;
    // Only the first buffer's slot is ever looked at: the loop doesn't take
    // the next slot GetNext finds.
    for (vb = m_vertexBuffers.GetFirst(&i); i != -1; m_vertexBuffers.GetNext(&i)) {
        if (vb && *vb) {
            delete *vb;
            *vb = 0;
        }
    }
    INDEXBUFFER **ib;
    for (ib = m_indexBuffers.GetFirst(&i); i != -1; ib = m_indexBuffers.GetNext(&i)) {
        if (ib && *ib) {
            delete *ib;
            *ib = 0;
        }
    }
    m_vertexBuffers.Clear();
    m_indexBuffers.Clear();
    if (g_pIdentityMatrix) {
        delete g_pIdentityMatrix;
        g_pIdentityMatrix = 0;
    }
    ReleaseSpecific();
    ReleaseDevice();
}

bool RENDERER::DetectCaps()
{
    GetDevice()->GetDeviceCaps(&m_d3dCaps);
    m_caps[0] = m_d3dCaps.MaxTextureWidth;
    m_caps[1] = m_d3dCaps.MaxTextureHeight;
    m_caps[2] = m_d3dCaps.MaxActiveLights;
    if (m_d3dCaps.MaxActiveLights > 8)
        m_caps[2] = 8;
    m_caps[3] = m_d3dCaps.MaxSimultaneousTextures;
    m_caps[4] = m_d3dCaps.MaxTextureBlendStages;
    m_caps[5] = 1;
    if (!m_canNormalize)
        SetNormalizeNormals(false);
    return true;
}

int RENDERER::Insert(RENDERABLE *item, int sort)
{
    int slot = sort + 0xff;
    if ((slot < 0 || slot > 0x1ff) && g_pLog)
        g_pLog->Error("Error - Invalid z sorting:%d, forced to limit", sort);
    if (slot < 0)
        slot = 0;
    else if (slot > 0x1ff)
        slot = 0x1ff;
    switch (item->GetType()) {
    case 1:
        m_lists1[slot].Add(item);
        break;
    case 2:
        m_lists2[slot].Add(item);
        break;
    case 4:
        m_lists4[slot].Add(item);
        break;
    default:
        if (g_pLog)
            g_pLog->Error("Not supported shader type");
        break;
    }
    return slot - 0xff;
}

void RENDERER::Remove(RENDERABLE *item, int sort)
{
    int slot = sort + 0xff;
    if (slot >= 0 && slot <= 0x1ff) {
        switch (item->GetType()) {
        case 1:
            m_lists1[slot].Remove(item);
            break;
        case 2:
            m_lists2[slot].Remove(item);
            break;
        case 4:
            m_lists4[slot].Remove(item);
            break;
        default:
            if (g_pLog)
                g_pLog->Error("Not supported shader type");
            break;
        }
        return;
    }
    if (g_pLog)
        g_pLog->Error("Error - RENDERER::Release> Invalid z sorting:%d, forced to look all tables", sort);
    for (int i = 0; i < 0xff; i++) {
        switch (item->GetType()) {
        case 1:
            m_lists1[i].Remove(item);
            break;
        case 2:
            m_lists2[i].Remove(item);
            break;
        case 4:
            m_lists4[i].Remove(item);
            break;
        default:
            if (g_pLog)
                g_pLog->Error("Not supported shader type");
            break;
        }
    }
}

void RENDERER::SetMode(int mode)
{
    if (mode == m_mode)
        return;
    m_shader.Invalidate();
    m_3148.Invalidate();
    switch (mode) {
    case 1:
        ResetStates();
        if (m_cullMode == 0)
            GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
        else
            GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
        break;
    }
    m_mode = mode;
}

void RENDERER::SetTongaShader(RESHANDLE handle)
{
    TONGASHADER *shader = (TONGASHADER *)g_pResources->GetResource(handle);
    if (!shader) {
        if (g_pLog)
            g_pLog->Error("Error - RENDERER::SetTongaShader> shader null");
        return;
    }
    SetMode(2);
    if (!(m_shader == shader->m_master->m_handle)) {
        m_shaderChanges++;
        shader->Apply();
        m_shader = shader->m_master->m_handle;
    }
}

void RENDERER::SetLight(const V3 &direction, const V3 &position)
{
    m_lightPos = position;
    V3 dir = direction;
    dir.Normalize();
    m_lightDir = dir;
}

RENDERER::RENDERER()
    : m_lightDir(0.0f, 0.0f, 0.0f), m_lightPos(0.0f, 0.0f, 0.0f)
{
    m_mode = 0;
    m_initialized = false;
    m_3015 = false;
    m_wireframe = false;
    m_cullMode = 0;
    m_3020 = false;
    m_303c = false;
    m_enabled = true;
    m_baseIndex = 0;
    if (g_pIdentityMatrix && g_pLog)
        g_pLog->Error("RENDERER::REDERER> Global identity matrix already exists!");
    g_pIdentityMatrix = new D3DXMATRIX;
    D3DXMatrixIdentity(g_pIdentityMatrix);
    SetLight(V3(0.0f, -1.0f, 0.0f), V3(0.0f, 0.0f, 0.0f));
    SetFog(false, V3I(0, 0, 0), 100.0f, 500.0f);
    m_lighting = false;
    m_normalize = false;
    m_canNormalize = true;
    memset(m_caps, 0, sizeof(m_caps));
}

RENDERER::~RENDERER()
{
    if (m_initialized && g_pLog)
        g_pLog->Error("RENDERER DESTRUCTOR - Did not call to destroy previously!");
}

VERTEXBUFFER *RENDERER::CreateVertexBuffer(int size, RESHANDLE *handle)
{
    VERTEXBUFFER *buffer = new VERTEXBUFFER(size);
    *handle = m_vertexBuffers.Insert(buffer);
    buffer->m_refs++;
    return buffer;
}

void RENDERER::AddRefVertexBuffer(const RESHANDLE &handle)
{
    VERTEXBUFFER *buffer = *m_vertexBuffers.Get(handle);
    if (buffer)
        buffer->m_refs++;
}

VERTEXBUFFER *RENDERER::GetVertexBuffer(const RESHANDLE &handle)
{
    VERTEXBUFFER *buffer = *m_vertexBuffers.Get(handle);
    if (buffer && buffer->m_refs > 0)
        return buffer;
    return 0;
}

void RENDERER::ReleaseVertexBuffer(const RESHANDLE &handle)
{
    VERTEXBUFFER *buffer = *m_vertexBuffers.Get(handle);
    if (buffer)
        buffer->m_refs--;
}

INDEXBUFFER *RENDERER::CreateIndexBuffer(RESHANDLE *handle)
{
    INDEXBUFFER *buffer = new INDEXBUFFER;
    *handle = m_indexBuffers.Insert(buffer);
    buffer->m_refs++;
    return buffer;
}

void RENDERER::AddRefIndexBuffer(const RESHANDLE &handle)
{
    INDEXBUFFER *buffer = *m_indexBuffers.Get(handle);
    if (buffer)
        buffer->m_refs++;
}

INDEXBUFFER *RENDERER::GetIndexBuffer(const RESHANDLE &handle)
{
    INDEXBUFFER *buffer = *m_indexBuffers.Get(handle);
    if (buffer && buffer->m_refs > 0)
        return buffer;
    return 0;
}

void RENDERER::ReleaseIndexBuffer(const RESHANDLE &handle)
{
    INDEXBUFFER *buffer = *m_indexBuffers.Get(handle);
    if (buffer)
        buffer->m_refs--;
}

bool RENDERER::SetVertexBuffer(const RESHANDLE &handle)
{
    if (handle == m_vertexBuffer)
        return true;
    VERTEXBUFFER *buffer = *m_vertexBuffers.Get(handle);
    if (buffer) {
        IDirect3DDevice8 *device = GetDevice();
        buffer->Validate();
        device->SetVertexShader(buffer->m_fvf);
        device->SetStreamSource(0, buffer->m_buffer, buffer->m_stride);
        m_vertexBuffer = handle;
        return true;
    }
    return false;
}

bool RENDERER::SetIndexBuffer(const RESHANDLE &handle, int baseIndex)
{
    if (handle == m_indexBuffer && m_baseIndex == baseIndex)
        return true;
    INDEXBUFFER *buffer = *m_indexBuffers.Get(handle);
    if (buffer) {
        buffer->Validate();
        GetDevice()->SetIndices(buffer->m_buffer, baseIndex);
        m_indexBuffer = handle;
        m_baseIndex = baseIndex;
        return true;
    }
    return false;
}

void RENDERER::CollectBuffers()
{
    int i = 0;
    VERTEXBUFFER **vb;
    for (vb = m_vertexBuffers.GetFirst(&i); i != -1; vb = m_vertexBuffers.GetNext(&i)) {
        if (vb && *vb && (*vb)->m_refs <= 0) {
            m_vertexBuffers.FreePos(i);
            if (*vb) {
                delete *vb;
                *vb = 0;
            }
        }
    }
    INDEXBUFFER **ib;
    for (ib = m_indexBuffers.GetFirst(&i); i != -1; ib = m_indexBuffers.GetNext(&i)) {
        if (ib && *ib && (*ib)->m_refs <= 0) {
            m_indexBuffers.FreePos(i);
            if (*ib) {
                delete *ib;
                *ib = 0;
            }
        }
    }
}

void RENDERER::Render()
{
    g_numTriangles = 0;
    if (!m_enabled)
        return;
    if (!m_initialized)
        Init();
    if (IsDeviceLost()) {
        if (g_pLog)
            g_pLog->Error("RENDERER::Render - Don't render, device is lost");
        return;
    }
    float height = (float)g_pDisplay->GetHeight();
    float width = (float)g_pDisplay->GetWidth();
    if (width < 600.0f || height < 400.0f) {
        if (g_pLog)
            g_pLog->Error("RENDERER::Render - Don't render, device is too small: %f %f", width, height);
        Restore();
        return;
    }
    m_shader.Invalidate();
    m_shaderChanges = 0;
    g_pRENDERSHADER->m_numTriangles = 0;
    g_9fef44 = 0;
    IDirect3DDevice8 *device = GetDevice();
    DX8CAMERA *camera = *(DX8CAMERA **)((char *)g_pCamera + 0xda4);
    if (m_wireframe)
        device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
    else
        device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
    BeginFrame();
    m_cullMode = 0;
    device->BeginScene();
    device->Clear(0, 0, D3DCLEAR_TARGET, (((DX8WINDOW *)g_pDisplay)->GetClearColor().x << 16) | (((DX8WINDOW *)g_pDisplay)->GetClearColor().y << 8) | ((DX8WINDOW *)g_pDisplay)->GetClearColor().z, 1.0f, 0);
    if (HasStencil())
        device->Clear(0, 0, D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0xffffffff, 1.0f, 0);
    else
        device->Clear(0, 0, D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
    device->SetTransform(D3DTS_PROJECTION, camera->GetProjection());
    SetLighting(true);
    ApplyFog();
    m_3148.Invalidate();
    for (int i = 0; i < 0x200; i++) {
        RENDERABLE *item;
        for (item = m_lists1[i].m_head; item; item = LIST<RENDERABLE>::Next(item)) {
            if (item->m_flags & 1) {
                if (m_canNormalize && item->NeedsNormalize())
                    SetNormalizeNormals(true);
                else
                    SetNormalizeNormals(false);
                item->Render();
            }
        }
        for (item = m_lists2[i].m_head; item; item = LIST<RENDERABLE>::Next(item)) {
            if (item->m_flags & 1) {
                if (m_canNormalize && item->NeedsNormalize())
                    SetNormalizeNormals(true);
                else
                    SetNormalizeNormals(false);
                item->Render();
            }
        }
        for (int j = 0; j < 8; j++)
            m_texCoordIndex[j] = 0;
        m_vertexBuffer.Invalidate();
        m_indexBuffer.Invalidate();
        for (item = m_lists4[i].m_head; item; item = LIST<RENDERABLE>::Next(item)) {
            if (item->m_flags & 1) {
                if (m_canNormalize && item->NeedsNormalize())
                    SetNormalizeNormals(true);
                else
                    SetNormalizeNormals(false);
                item->Render();
            }
        }
    }
    SetLighting(false);
    device->EndScene();
    ((DX8WINDOW *)g_pDisplay)->m_numTriangles = g_pRENDERSHADER->m_numTriangles + m_shaderChanges;
    HRESULT result = device->Present(0, 0, 0, 0);
    if (result == D3DERR_DEVICELOST)
        Restore();
    else if (result && g_pLog)
        g_pLog->Error("Error - Renderer error when presenting device:%d", result);
    CollectBuffers();
}
