#include <string.h>

// Tongas2/MESHDX8 -- DX8 renderer plugin: vertex/index buffers, DX8 camera and
// the MESHDX8 mesh implementation.
//
// First carve of the 0x004e5d50-0x004ea0ff cluster out of the
// unassigned/004e5d50 placeholder. These are the class declarations and the
// decorated signatures of the functions this translation unit defines, so the
// unit delinks and objdiff can pair it with the original. The bodies are the
// initial scaffold only: matching them byte-for-byte is the follow-up work.
// The remaining ~81 FUN_004e.... functions in the range are still unnamed and
// will be added here as they are identified.

// The engine's opaque types; kept as incomplete struct tags so the decorated
// names below stay exactly the ones config/symbols.txt records.
struct RESHANDLE;
struct _D3DMATRIX;
struct D3DXMATRIX;

class INDEXBUFFER
{
public:
    INDEXBUFFER();
    void Validate();
};

class VERTEXBUFFER
{
public:
    VERTEXBUFFER(int size);
    void Validate();
};

class DX8CAMERA
{
public:
    const _D3DMATRIX *GetProjection();
};

class MESHDX8
{
    // Stand-in layout: enough padding to put the fields the code touches at
    // their original offsets. Filled in as each function is matched.
    char m_pad[0xC4];
    int m_lightMask;

public:
    MESHDX8();

    void SetLightMask(int mask);
    void SetLit(bool lit);
    void SetMatrix(const D3DXMATRIX *matrix);

    void *LockVertices(int first, int count);
    void UnlockVertices();
    void *LockIndices(int first, int count);
    void UnlockIndices();

    void Create(int numVertices, int numIndices, int numSubmeshes, int flags);
    void ShowSubmesh(int index, bool show);
    void RemoveSubmesh(int index);
    int AddSubmesh(const RESHANDLE &shader, int a, int b, int c, int d, int e, int f);
    void SetSubmeshShader(int index, const RESHANDLE &shader);
};

extern "C" int idx_vtable;

INDEXBUFFER::INDEXBUFFER() {
    int *p = (int *)this;
    p[1] = 0;
    *(char *)(p + 2) = 0;
    p[3] = 0;
    p[4] = 0;
    p[5] = 0;
    p[6] = 0;
    *(char *)((char *)p + 0x1d) = 0;
    p[0] = (int)&idx_vtable;
    p[8] = 0;
    p[9] = 0;
}
void INDEXBUFFER::Validate() {}

VERTEXBUFFER::VERTEXBUFFER(int size) { (void)size; }
void VERTEXBUFFER::Validate() {}

class CAMERALENS { public: void Update(bool); };

const _D3DMATRIX *DX8CAMERA::GetProjection() {
    if (*(unsigned char *)((char *)this + 0x90)) {
        ((CAMERALENS *)this)->Update(false);
    }
    return (const _D3DMATRIX *)((char *)this + 0x26c);
}

MESHDX8::MESHDX8() {}
void MESHDX8::SetLightMask(int mask) { m_lightMask = mask; }
void MESHDX8::SetLit(bool lit) { (void)lit; }
void MESHDX8::SetMatrix(const D3DXMATRIX *matrix) {
    memcpy((char *)this + 0x80, matrix, 64);
}

void *MESHDX8::LockVertices(int first, int count) { (void)first; (void)count; return 0; }
void MESHDX8::UnlockVertices() {}
void *MESHDX8::LockIndices(int first, int count) { (void)first; (void)count; return 0; }
void MESHDX8::UnlockIndices() {}

void MESHDX8::Create(int numVertices, int numIndices, int numSubmeshes, int flags)
{
    (void)numVertices; (void)numIndices; (void)numSubmeshes; (void)flags;
}
void MESHDX8::ShowSubmesh(int index, bool show) { (void)index; (void)show; }
void MESHDX8::RemoveSubmesh(int index) { (void)index; }
int MESHDX8::AddSubmesh(const RESHANDLE &, int, int, int, int, int, int)
{
    return 0;
}
void MESHDX8::SetSubmeshShader(int, const RESHANDLE &) {}
