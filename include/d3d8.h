// Minimal stand-in for the DirectX 8 SDK's d3d8.h, which the Visual C++
// Toolkit 2003 doesn't ship: the types, constants and interfaces the engine
// uses, laid out as the SDK's. Add more as units need them.
#ifndef TJ_D3D8_H
#define TJ_D3D8_H

#include <windows.h>

typedef long HRESULT;
typedef DWORD D3DCOLOR;
struct _GUID;
typedef const _GUID &REFIID;

#define D3D_OK 0
#define D3DERR_DEVICELOST ((HRESULT)0x88760868L)

typedef struct _D3DCOLORVALUE {
    float r;
    float g;
    float b;
    float a;
} D3DCOLORVALUE;

typedef struct _D3DVECTOR {
    float x;
    float y;
    float z;
} D3DVECTOR;

typedef struct _D3DMATRIX {
    union {
        struct {
            float _11, _12, _13, _14;
            float _21, _22, _23, _24;
            float _31, _32, _33, _34;
            float _41, _42, _43, _44;
        };
        float m[4][4];
    };
} D3DMATRIX;

typedef struct _D3DMATERIAL8 {
    D3DCOLORVALUE Diffuse;
    D3DCOLORVALUE Ambient;
    D3DCOLORVALUE Specular;
    D3DCOLORVALUE Emissive;
    float Power;
} D3DMATERIAL8;

typedef struct _D3DRECT {
    LONG x1;
    LONG y1;
    LONG x2;
    LONG y2;
} D3DRECT;

typedef enum _D3DDEVTYPE {
    D3DDEVTYPE_HAL = 1,
    D3DDEVTYPE_REF = 2,
    D3DDEVTYPE_SW = 3,
    D3DDEVTYPE_FORCE_DWORD = 0x7fffffff
} D3DDEVTYPE;

typedef struct _D3DCAPS8 {
    D3DDEVTYPE DeviceType;
    UINT AdapterOrdinal;
    DWORD Caps;
    DWORD Caps2;
    DWORD Caps3;
    DWORD PresentationIntervals;
    DWORD CursorCaps;
    DWORD DevCaps;
    DWORD PrimitiveMiscCaps;
    DWORD RasterCaps;
    DWORD ZCmpCaps;
    DWORD SrcBlendCaps;
    DWORD DestBlendCaps;
    DWORD AlphaCmpCaps;
    DWORD ShadeCaps;
    DWORD TextureCaps;
    DWORD TextureFilterCaps;
    DWORD CubeTextureFilterCaps;
    DWORD VolumeTextureFilterCaps;
    DWORD TextureAddressCaps;
    DWORD VolumeTextureAddressCaps;
    DWORD LineCaps;
    DWORD MaxTextureWidth, MaxTextureHeight;
    DWORD MaxVolumeExtent;
    DWORD MaxTextureRepeat;
    DWORD MaxTextureAspectRatio;
    DWORD MaxAnisotropy;
    float MaxVertexW;
    float GuardBandLeft;
    float GuardBandTop;
    float GuardBandRight;
    float GuardBandBottom;
    float ExtentsAdjust;
    DWORD StencilCaps;
    DWORD FVFCaps;
    DWORD TextureOpCaps;
    DWORD MaxTextureBlendStages;
    DWORD MaxSimultaneousTextures;
    DWORD VertexProcessingCaps;
    DWORD MaxActiveLights;
    DWORD MaxUserClipPlanes;
    DWORD MaxVertexBlendMatrices;
    DWORD MaxVertexBlendMatrixIndex;
    float MaxPointSize;
    DWORD MaxPrimitiveCount;
    DWORD MaxVertexIndex;
    DWORD MaxStreams;
    DWORD MaxStreamStride;
    DWORD VertexShaderVersion;
    DWORD MaxVertexShaderConst;
    DWORD PixelShaderVersion;
    float MaxPixelShaderValue;
} D3DCAPS8;

#define D3DPRASTERCAPS_FOGTABLE 0x00000100L
#define D3DPRASTERCAPS_WFOG 0x00100000L
#define D3DPRASTERCAPS_ZFOG 0x00200000L

#define D3DCLEAR_TARGET 0x00000001L
#define D3DCLEAR_ZBUFFER 0x00000002L
#define D3DCLEAR_STENCIL 0x00000004L

typedef enum _D3DTRANSFORMSTATETYPE {
    D3DTS_VIEW = 2,
    D3DTS_PROJECTION = 3,
    D3DTS_TEXTURE0 = 16,
    D3DTS_FORCE_DWORD = 0x7fffffff
} D3DTRANSFORMSTATETYPE;
#define D3DTS_WORLDMATRIX(index) (D3DTRANSFORMSTATETYPE)(index + 256)
#define D3DTS_WORLD D3DTS_WORLDMATRIX(0)

typedef enum _D3DRENDERSTATETYPE {
    D3DRS_ZENABLE = 7,
    D3DRS_FILLMODE = 8,
    D3DRS_SHADEMODE = 9,
    D3DRS_ZWRITEENABLE = 14,
    D3DRS_ALPHATESTENABLE = 15,
    D3DRS_SRCBLEND = 19,
    D3DRS_DESTBLEND = 20,
    D3DRS_CULLMODE = 22,
    D3DRS_ALPHABLENDENABLE = 27,
    D3DRS_FOGENABLE = 28,
    D3DRS_FOGCOLOR = 34,
    D3DRS_FOGTABLEMODE = 35,
    D3DRS_FOGSTART = 36,
    D3DRS_FOGEND = 37,
    D3DRS_STENCILENABLE = 52,
    D3DRS_LIGHTING = 137,
    D3DRS_FOGVERTEXMODE = 140,
    D3DRS_NORMALIZENORMALS = 143,
    D3DRS_FORCE_DWORD = 0x7fffffff
} D3DRENDERSTATETYPE;

typedef enum _D3DTEXTURESTAGESTATETYPE {
    D3DTSS_COLOROP = 1,
    D3DTSS_COLORARG1 = 2,
    D3DTSS_COLORARG2 = 3,
    D3DTSS_ALPHAOP = 4,
    D3DTSS_ALPHAARG1 = 5,
    D3DTSS_ALPHAARG2 = 6,
    D3DTSS_TEXCOORDINDEX = 11,
    D3DTSS_MAGFILTER = 16,
    D3DTSS_MINFILTER = 17,
    D3DTSS_MIPFILTER = 18,
    D3DTSS_MIPMAPLODBIAS = 19,
    D3DTSS_MAXMIPLEVEL = 20,
    D3DTSS_TEXTURETRANSFORMFLAGS = 24,
    D3DTSS_FORCE_DWORD = 0x7fffffff
} D3DTEXTURESTAGESTATETYPE;

typedef enum _D3DPRIMITIVETYPE {
    D3DPT_TRIANGLELIST = 4,
    D3DPT_FORCE_DWORD = 0x7fffffff
} D3DPRIMITIVETYPE;

#define D3DFILL_WIREFRAME 2
#define D3DFILL_SOLID 3
#define D3DSHADE_GOURAUD 2
#define D3DCULL_CW 2
#define D3DCULL_CCW 3
#define D3DBLEND_ZERO 1
#define D3DBLEND_ONE 2
#define D3DFOG_LINEAR 3
#define D3DTA_DIFFUSE 0x00000000
#define D3DTA_CURRENT 0x00000001
#define D3DTA_TEXTURE 0x00000002
#define D3DTEXF_POINT 1
#define D3DTEXF_LINEAR 2

typedef enum _D3DTEXTUREOP {
    D3DTOP_DISABLE = 1,
    D3DTOP_SELECTARG1 = 2,
    D3DTOP_MODULATE = 4,
    D3DTOP_FORCE_DWORD = 0x7fffffff
} D3DTEXTUREOP;

struct IDirect3DBaseTexture8;
struct IDirect3DVertexBuffer8;
struct IDirect3DIndexBuffer8;
struct IDirect3DSurface8;

// The device's methods in the SDK's order; the ones the engine doesn't call
// keep their slot with their arguments left out.
struct IDirect3DDevice8
{
    virtual HRESULT __stdcall QueryInterface(REFIID riid, void **ppvObj) = 0;
    virtual ULONG __stdcall AddRef() = 0;
    virtual ULONG __stdcall Release() = 0;
    virtual HRESULT __stdcall TestCooperativeLevel() = 0;
    virtual UINT __stdcall GetAvailableTextureMem() = 0;
    virtual HRESULT __stdcall ResourceManagerDiscardBytes(DWORD Bytes) = 0;
    virtual HRESULT __stdcall GetDirect3D(void **ppD3D8) = 0;
    virtual HRESULT __stdcall GetDeviceCaps(D3DCAPS8 *pCaps) = 0;
    virtual HRESULT __stdcall GetDisplayMode(void *pMode) = 0;
    virtual HRESULT __stdcall GetCreationParameters(void *pParameters) = 0;
    virtual HRESULT __stdcall SetCursorProperties(UINT XHotSpot, UINT YHotSpot, IDirect3DSurface8 *pCursorBitmap) = 0;
    virtual void __stdcall SetCursorPosition(int X, int Y, DWORD Flags) = 0;
    virtual BOOL __stdcall ShowCursor(BOOL bShow) = 0;
    virtual HRESULT __stdcall CreateAdditionalSwapChain(void *pPresentationParameters, void **pSwapChain) = 0;
    virtual HRESULT __stdcall Reset(void *pPresentationParameters) = 0;
    virtual HRESULT __stdcall Present(const RECT *pSourceRect, const RECT *pDestRect, HWND hDestWindowOverride, const void *pDirtyRegion) = 0;
    virtual HRESULT __stdcall GetBackBuffer(UINT BackBuffer, int Type, IDirect3DSurface8 **ppBackBuffer) = 0;
    virtual HRESULT __stdcall GetRasterStatus(void *pRasterStatus) = 0;
    virtual void __stdcall SetGammaRamp(DWORD Flags, const void *pRamp) = 0;
    virtual void __stdcall GetGammaRamp(void *pRamp) = 0;
    virtual HRESULT __stdcall CreateTexture() = 0;
    virtual HRESULT __stdcall CreateVolumeTexture() = 0;
    virtual HRESULT __stdcall CreateCubeTexture() = 0;
    virtual HRESULT __stdcall CreateVertexBuffer() = 0;
    virtual HRESULT __stdcall CreateIndexBuffer() = 0;
    virtual HRESULT __stdcall CreateRenderTarget() = 0;
    virtual HRESULT __stdcall CreateDepthStencilSurface() = 0;
    virtual HRESULT __stdcall CreateImageSurface() = 0;
    virtual HRESULT __stdcall CopyRects() = 0;
    virtual HRESULT __stdcall UpdateTexture() = 0;
    virtual HRESULT __stdcall GetFrontBuffer() = 0;
    virtual HRESULT __stdcall SetRenderTarget() = 0;
    virtual HRESULT __stdcall GetRenderTarget() = 0;
    virtual HRESULT __stdcall GetDepthStencilSurface(IDirect3DSurface8 **ppZStencilSurface) = 0;
    virtual HRESULT __stdcall BeginScene() = 0;
    virtual HRESULT __stdcall EndScene() = 0;
    virtual HRESULT __stdcall Clear(DWORD Count, const D3DRECT *pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil) = 0;
    virtual HRESULT __stdcall SetTransform(D3DTRANSFORMSTATETYPE State, const D3DMATRIX *pMatrix) = 0;
    virtual HRESULT __stdcall GetTransform() = 0;
    virtual HRESULT __stdcall MultiplyTransform() = 0;
    virtual HRESULT __stdcall SetViewport() = 0;
    virtual HRESULT __stdcall GetViewport() = 0;
    virtual HRESULT __stdcall SetMaterial(const D3DMATERIAL8 *pMaterial) = 0;
    virtual HRESULT __stdcall GetMaterial() = 0;
    virtual HRESULT __stdcall SetLight() = 0;
    virtual HRESULT __stdcall GetLight() = 0;
    virtual HRESULT __stdcall LightEnable() = 0;
    virtual HRESULT __stdcall GetLightEnable() = 0;
    virtual HRESULT __stdcall SetClipPlane() = 0;
    virtual HRESULT __stdcall GetClipPlane() = 0;
    virtual HRESULT __stdcall SetRenderState(D3DRENDERSTATETYPE State, DWORD Value) = 0;
    virtual HRESULT __stdcall GetRenderState() = 0;
    virtual HRESULT __stdcall BeginStateBlock() = 0;
    virtual HRESULT __stdcall EndStateBlock() = 0;
    virtual HRESULT __stdcall ApplyStateBlock() = 0;
    virtual HRESULT __stdcall CaptureStateBlock() = 0;
    virtual HRESULT __stdcall DeleteStateBlock() = 0;
    virtual HRESULT __stdcall CreateStateBlock() = 0;
    virtual HRESULT __stdcall SetClipStatus() = 0;
    virtual HRESULT __stdcall GetClipStatus() = 0;
    virtual HRESULT __stdcall GetTexture() = 0;
    virtual HRESULT __stdcall SetTexture(DWORD Stage, IDirect3DBaseTexture8 *pTexture) = 0;
    virtual HRESULT __stdcall GetTextureStageState() = 0;
    virtual HRESULT __stdcall SetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value) = 0;
    virtual HRESULT __stdcall ValidateDevice() = 0;
    virtual HRESULT __stdcall GetInfo() = 0;
    virtual HRESULT __stdcall SetPaletteEntries() = 0;
    virtual HRESULT __stdcall GetPaletteEntries() = 0;
    virtual HRESULT __stdcall SetCurrentTexturePalette() = 0;
    virtual HRESULT __stdcall GetCurrentTexturePalette() = 0;
    virtual HRESULT __stdcall DrawPrimitive() = 0;
    virtual HRESULT __stdcall DrawIndexedPrimitive(D3DPRIMITIVETYPE Type, UINT MinIndex, UINT NumVertices, UINT StartIndex, UINT PrimitiveCount) = 0;
    virtual HRESULT __stdcall DrawPrimitiveUP() = 0;
    virtual HRESULT __stdcall DrawIndexedPrimitiveUP() = 0;
    virtual HRESULT __stdcall ProcessVertices() = 0;
    virtual HRESULT __stdcall CreateVertexShader() = 0;
    virtual HRESULT __stdcall SetVertexShader(DWORD Handle) = 0;
    virtual HRESULT __stdcall GetVertexShader() = 0;
    virtual HRESULT __stdcall DeleteVertexShader() = 0;
    virtual HRESULT __stdcall SetVertexShaderConstant() = 0;
    virtual HRESULT __stdcall GetVertexShaderConstant() = 0;
    virtual HRESULT __stdcall GetVertexShaderDeclaration() = 0;
    virtual HRESULT __stdcall GetVertexShaderFunction() = 0;
    virtual HRESULT __stdcall SetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer8 *pStreamData, UINT Stride) = 0;
    virtual HRESULT __stdcall GetStreamSource() = 0;
    virtual HRESULT __stdcall SetIndices(IDirect3DIndexBuffer8 *pIndexData, UINT BaseVertexIndex) = 0;
    virtual HRESULT __stdcall GetIndices() = 0;
    virtual HRESULT __stdcall CreatePixelShader() = 0;
    virtual HRESULT __stdcall SetPixelShader() = 0;
    virtual HRESULT __stdcall GetPixelShader() = 0;
    virtual HRESULT __stdcall DeletePixelShader() = 0;
    virtual HRESULT __stdcall SetPixelShaderConstant() = 0;
    virtual HRESULT __stdcall GetPixelShaderConstant() = 0;
    virtual HRESULT __stdcall GetPixelShaderFunction() = 0;
    virtual HRESULT __stdcall DrawRectPatch() = 0;
    virtual HRESULT __stdcall DrawTriPatch() = 0;
    virtual HRESULT __stdcall DeletePatch() = 0;
};

#endif
