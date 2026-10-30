// The base of the renderer plugin: the Direct3D 8 device's default states,
// fog, lighting, the vertex and index buffer tables, and the lists of things
// to draw, sorted by depth and by kind. The plugin derives its renderer from
// it and makes it through g_pfnCreateRENDERER. Placeholder names apart from
// the class name.
#ifndef TONGAS2_RENDERER_H
#define TONGAS2_RENDERER_H

#include <d3dx8.h>

#include "Tongas2/LIST.h"
#include "Tongas2/MATHS.h"
#include "Tongas2/SAFETABLE.h"

// Defined in the plugin: something drawn, linked into one of the renderer's
// lists while it is shown.
class RENDERABLE : public LISTNODE
{
public:
    virtual ~RENDERABLE();
    virtual void Render();
    virtual void SetSort(int sort);
    virtual int GetSort();
    virtual void Virtual04();
    virtual void Virtual05();
    // Which list it goes in: 1, 2 or 4.
    virtual int GetType();
    // Whether it needs its normals normalized.
    virtual bool NeedsNormalize();

    bool m_inserted;
    // Drawn while bit 0 is set.
    int m_flags;
};

// Defined in the plugin: a Direct3D vertex buffer and the shader for its
// vertices.
class VERTEXBUFFER
{
public:
    VERTEXBUFFER(int size);
    virtual void Virtual00();
    virtual void Virtual01();
    virtual void Virtual02();
    virtual void Virtual03();
    virtual ~VERTEXBUFFER();
    // Makes sure the buffer is there to draw from.
    void Validate();

    int m_refs;
    char m_08[0x18];
    IDirect3DVertexBuffer8 *m_buffer;
    char m_24[0xc];
    UINT m_stride;
    char m_34[0x18];
    DWORD m_fvf;
};

// Defined in the plugin: a Direct3D index buffer.
class INDEXBUFFER
{
public:
    INDEXBUFFER();
    virtual void Virtual00();
    virtual void Virtual01();
    virtual void Virtual02();
    virtual void Virtual03();
    virtual ~INDEXBUFFER();
    void Validate();

    int m_refs;
    char m_08[0x18];
    IDirect3DIndexBuffer8 *m_buffer;
    char m_24[0x8];
};

class RENDERER
{
public:
    RENDERER();

    virtual void Virtual00() {}
    virtual bool SpecificInit() { return true; }
    virtual void ReleaseDevice() {}
    virtual bool CreateDevice() { return true; }
    virtual void ReleaseSpecific() {}
    // Reads the device's capabilities.
    virtual bool DetectCaps();
    virtual ~RENDERER();
    virtual IDirect3DDevice8 *GetDevice() = 0;
    virtual bool IsDeviceLost() = 0;
    // Gets the device back after it was lost.
    virtual void Restore() = 0;
    virtual void Virtual10() = 0;
    virtual bool HasStencil() = 0;
    virtual void BeginFrame() {}

    bool Init();
    // Lets the device and the buffers go.
    void Destroy();
    int GetMaxTextureWidth();
    int GetMaxTextureHeight();
    void SetMipmapBias(float bias);
    // The device's states as the engine starts each mode from.
    void ResetStates();
    int GetCullMode();
    void Set3020(const bool &on);
    D3DXPLANE *GetLightPlane(D3DXPLANE *out, int unused);
    void SetFog(bool enable, V3I color, float start, float end);
    // Sets the fog on the device without keeping it.
    void SetFogDirect(bool enable, V3I color, float start, float end);
    // The fog color on the device: the kept one, or color.
    void SetFogColor(const V3I *color);
    // Sets the kept fog on the device.
    void ApplyFog();
    bool DrawTriangles(int minIndex, int numVertices, int startTriangle, int numTriangles);
    // The texture coordinates the stage uses; the ones it used.
    int SetTexCoordIndex(int stage, int index);
    // One of the capabilities g_capNames names, or 0.
    int GetCap(int cap);
    void SetLighting(bool on);
    void SetNormalizeNormals(bool on);
    // Puts the item in the list for its kind at the depth sort (-255..256);
    // the depth it went in at.
    int Insert(RENDERABLE *item, int sort);
    void Remove(RENDERABLE *item, int sort);
    void SetMode(int mode);
    void SetTongaShader(RESHANDLE shader);
    void SetLight(const V3 &direction, const V3 &position);
    VERTEXBUFFER *CreateVertexBuffer(int size, RESHANDLE *handle);
    void AddRefVertexBuffer(const RESHANDLE &handle);
    // The buffer, while it is in use.
    VERTEXBUFFER *GetVertexBuffer(const RESHANDLE &handle);
    void ReleaseVertexBuffer(const RESHANDLE &handle);
    INDEXBUFFER *CreateIndexBuffer(RESHANDLE *handle);
    void AddRefIndexBuffer(const RESHANDLE &handle);
    INDEXBUFFER *GetIndexBuffer(const RESHANDLE &handle);
    void ReleaseIndexBuffer(const RESHANDLE &handle);
    bool SetVertexBuffer(const RESHANDLE &handle);
    bool SetIndexBuffer(const RESHANDLE &handle, int baseIndex);
    // Deletes the buffers no longer in use.
    void CollectBuffers();
    void Render();

    LIST<RENDERABLE> m_lists2[0x200];
    LIST<RENDERABLE> m_lists1[0x200];
    LIST<RENDERABLE> m_lists4[0x200];
    int m_mode;
    RESHANDLE m_shader;
    bool m_initialized;
    bool m_3015;
    bool m_wireframe;
    int m_cullMode;
    int m_shaderChanges;
    bool m_3020;
    V3 m_lightDir;
    V3 m_lightPos;
    bool m_303c;
    bool m_enabled;
    bool m_fogEnabled;
    D3DXCOLOR m_fogColor;
    float m_fogStart;
    float m_fogEnd;
    bool m_lighting;
    bool m_canNormalize;
    bool m_normalize;
    int m_caps[6];
    D3DCAPS8 m_d3dCaps;
    RESHANDLE m_3148;
    RESHANDLE m_vertexBuffer;
    RESHANDLE m_indexBuffer;
    int m_texCoordIndex[8];
    int m_baseIndex;
    SAFETABLE<VERTEXBUFFER, 5000, 6> m_vertexBuffers;
    SAFETABLE<INDEXBUFFER, 5000, 7> m_indexBuffers;
};

// The names of the capabilities GetCap reads, by index, ending with "".
extern const char g_capNames[][30];

extern RENDERER *g_pRenderer;
extern D3DXMATRIX *g_pIdentityMatrix;
extern RENDERER *(*g_pfnCreateRENDERER)();

#endif
