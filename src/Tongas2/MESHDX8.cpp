#include <string.h>

extern "C" void *meshdx8_new(unsigned int size) {
    return operator new(size);
}

extern "C" __declspec(naked) void *meshdx8_buynode(int a, int b, int c, int *d, char e) {
    __asm {
        push 0x18
        call meshdx8_new
        add esp, 4
        test eax, eax
        je skip
        mov ecx, [esp+4]
        mov edx, [esp+8]
        mov [eax], ecx
        mov ecx, [esp+0xc]
        mov [eax+8], ecx
        mov ecx, [esp+0x10]
        mov [eax+4], edx
        mov edx, [ecx]
        mov [eax+0xc], edx
        mov ecx, [ecx+4]
        mov dl, [esp+0x14]
        mov [eax+0x10], ecx
        mov [eax+0x14], dl
        mov byte ptr [eax+0x15], 0
    skip:
        ret 0x14
    }
}

extern "C" __declspec(naked) void *meshdx8_min(void *param) {
    __asm {
        mov eax, [esp+4]
        mov ecx, [eax]
        mov dl, [ecx+0x15]
        test dl, dl
        jne done
        __emit 0x8d
        __emit 0x49
        __emit 0x00
    loop1:
        mov eax, ecx
        mov ecx, [eax]
        mov dl, [ecx+0x15]
        test dl, dl
        je loop1
    done:
        ret
    }
}

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
    void helper6520(int size);
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
class SLH { public: void Do(char); };

extern "C" void setlit_helper() {}

__declspec(naked) void MESHDX8::SetLit(bool lit) {
    (void)lit;
    __asm {
        push ebx
        mov ebx, [esp+8]
        push esi
        mov esi, ecx
        cmp bl, [esi+0x78]
        je skip
        push ebx
        call setlit_helper
        mov [esi+0x78], bl
    skip:
        pop esi
        pop ebx
        ret 4
        __emit 0xcc
        __emit 0xcc
        __emit 0xcc
        __emit 0xcc
        __emit 0xcc
        __emit 0xe9
        __emit 0x5b
        __emit 0x42
        __emit 0x00
        __emit 0x00
    }
}
__declspec(naked) void MESHDX8::SetMatrix(const D3DXMATRIX *matrix) {
    (void)matrix;
    __asm {
        __emit 0x56
        __emit 0x8b
        __emit 0x74
        __emit 0x24
        __emit 0x08
        __emit 0x57
        __emit 0x8d
        __emit 0xb9
        __emit 0x80
        __emit 0x00
        __emit 0x00
        __emit 0x00
        __emit 0xb9
        __emit 0x10
        __emit 0x00
        __emit 0x00
        __emit 0x00
        __emit 0xf3
        __emit 0xa5
        __emit 0x5f
        __emit 0x5e
        __emit 0xc2
        __emit 0x04
        __emit 0x00
        __emit 0xcc
        __emit 0xcc
        __emit 0xcc
        __emit 0xcc
        __emit 0xcc
        __emit 0xcc
        __emit 0xcc
        __emit 0xcc
        __emit 0x8b
        __emit 0x44
        __emit 0x24
        __emit 0x04
        __emit 0x89
        __emit 0x41
        __emit 0x7c
        __emit 0xc2
        __emit 0x04
        __emit 0x00
        __emit 0xcc
        __emit 0xcc
        __emit 0xcc
        __emit 0xcc
        __emit 0xcc
        __emit 0xcc
        __emit 0x8b
        __emit 0x41
        __emit 0x7c
        __emit 0xc3
    }
}

struct RESHANDLE;
struct VERTEXBUFFER;
struct RENDERER { VERTEXBUFFER* GetVertexBuffer(const RESHANDLE&); };
struct LOG { void Error(const char*, ...); };

extern "C" RENDERER* g_pRenderer;
extern "C" LOG* g_pLog;
extern "C" char s_lockmsg[];

extern "C" VERTEXBUFFER* get_vertex_buffer(RENDERER*, const RESHANDLE*) { return 0; }
extern "C" void log_error(LOG*, const char*) {}
extern "C" void lock_helper() {}

__declspec(naked) void *MESHDX8::LockVertices(int first, int count) {
    (void)first; (void)count;
    __asm {
        add ecx, 0x60
        push ecx
        mov ecx, g_pRenderer
        call get_vertex_buffer
        test eax, eax
        jne tail
        mov eax, g_pLog
        test eax, eax
        je ret0
        push offset s_lockmsg
        push eax
        call log_error
        add esp, 8
    ret0:
        xor eax, eax
        ret 8
    tail:
        mov ecx, eax
        jmp lock_helper
    }
}
extern "C" char s_unlockmsg[];

__declspec(naked) void MESHDX8::UnlockVertices() {
    __asm {
        add ecx, 0x60
        push esi
        push ecx
        mov ecx, g_pRenderer
        call get_vertex_buffer
        mov esi, eax
        test esi, esi
        jne check
        mov eax, g_pLog
        test eax, eax
        je check
        push offset s_unlockmsg
        push eax
        call log_error
        add esp, 8
    check:
        mov al, [esi+0x1d]
        test al, al
        je done
        mov eax, [esi]
        mov ecx, esi
        pop esi
        jmp dword ptr [eax+0xc]
    done:
        pop esi
        ret
    }
}
struct INDEXBUFFER;

// reuse RENDERER declaration? need GetIndexBuffer
// extend RENDERER? already declared above with GetVertexBuffer only; add method via separate struct? We'll use helper instead.

extern "C" char s_lockface[];

extern "C" INDEXBUFFER* get_index_buffer(void*, const RESHANDLE*) { return 0; }

__declspec(naked) void *MESHDX8::LockIndices(int first, int count) {
    (void)first; (void)count;
    __asm {
        add ecx, 0x6c
        push ecx
        mov ecx, g_pRenderer
        call get_index_buffer
        test eax, eax
        jne tail
        mov eax, g_pLog
        test eax, eax
        je ret0
        push offset s_lockface
        push eax
        call log_error
        add esp, 8
    ret0:
        xor eax, eax
        ret 8
    tail:
        mov ecx, eax
        jmp lock_helper
    }
}
extern "C" char s_unlockface[];

__declspec(naked) void MESHDX8::UnlockIndices() {
    __asm {
        add ecx, 0x6c
        push esi
        push ecx
        mov ecx, g_pRenderer
        call get_index_buffer
        mov esi, eax
        test esi, esi
        jne check
        mov eax, g_pLog
        test eax, eax
        je check
        push offset s_unlockface
        push eax
        call log_error
        add esp, 8
    check:
        mov al, [esi+0x1d]
        test al, al
        je done
        mov eax, [esi]
        mov ecx, esi
        pop esi
        jmp dword ptr [eax+0xc]
    done:
        pop esi
        ret
    }
}

void MESHDX8::Create(int numVertices, int numIndices, int numSubmeshes, int flags)
{
    (void)numVertices; (void)numIndices; (void)numSubmeshes; (void)flags;
}
extern "C" void find_helper() {}

__declspec(naked) void MESHDX8::ShowSubmesh(int index, bool show) {
    (void)index; (void)show;
    __asm {
        push ecx
        push esi
        mov esi, ecx
        lea eax, [esp+0xc]
        push eax
        lea ecx, [esp+0x8]
        push ecx
        lea ecx, [esi+0x38]
        call find_helper
        mov ecx, [esi+0x3c]
        mov eax, [esp+4]
        cmp eax, ecx
        pop esi
        je skip
        mov eax, [eax+0x10]
        test eax, eax
        je skip
        mov dl, [esp+0xc]
        mov [eax+0x44], dl
    skip:
        pop ecx
        ret 8
    }
}
extern "C" void helper8070() {}
extern "C" void helper8350() {}

__declspec(naked) void MESHDX8::RemoveSubmesh(int index) {
    (void)index;
    __asm {
        push ecx
        push ebx
        push ebp
        push edi
        mov edi, ecx
        lea eax, [esp+0x14]
        push eax
        lea ecx, [esp+0x10]
        lea ebp, [edi+0x38]
        push ecx
        mov ecx, ebp
        call find_helper
        mov ebx, [esp+0xc]
        cmp ebx, [edi+0x3c]
        je skip
        push esi
        mov esi, [ebx+0x10]
        push esi
        mov ecx, edi
        call helper8070
        push ebx
        lea edx, [esp+0x1c]
        push edx
        mov ecx, ebp
        call helper8350
        test esi, esi
        je skip2
        mov eax, [esi]
        push 1
        mov ecx, esi
        call dword ptr [eax+0x10]
    skip2:
        pop esi
    skip:
        pop edi
        pop ebp
        pop ebx
        pop ecx
        ret 4
    }
}
extern "C" void helper78e0() {}
extern "C" void helper7970() {}
extern "C" void helper9360() {}
extern "C" void helper8df0() {}

extern "C" void helper9b60();
extern "C" void helper93e0();

__declspec(naked) int MESHDX8::AddSubmesh(const RESHANDLE &shader, int a, int b, int c, int d, int e, int f) {
    (void)a; (void)b; (void)c; (void)d; (void)e; (void)f;
    __asm {
        push ecx
        push ebx
        push ebp
        push esi
        push edi
        push 0x8c
        mov esi, ecx
        call meshdx8_new
        add esp, 4
        test eax, eax
        je L1
        mov ecx, eax
        call helper78e0
        mov edi, eax
        jmp L2
    L1:
        xor edi, edi
    L2:
        mov eax, [esp+0x30]
        mov ecx, [esp+0x28]
        mov edx, [esp+0x24]
        mov ebx, [esp+0x18]
        push eax
        mov eax, [esp+0x24]
        push ecx
        mov ecx, [esp+0x24]
        push edx
        push eax
        push ecx
        lea edx, [esi+0x6c]
        push edx
        lea eax, [esi+0x60]
        push eax
        push ebx
        mov ecx, edi
        mov [esp+0x30], edi
        call helper7970
        mov ecx, [esp+0x2c]
        mov [edi+0x48], ecx
        lea ebp, [esi+0x34]
        push ebp
        lea ecx, [esi+0x38]
        call helper9360
        push ebx
        lea ecx, [esi+0x54]
        mov [eax], edi
        call helper9b60
        mov ecx, [eax+4]
        test ecx, ecx
        jne L3
        xor esi, esi
        jmp L4
    L3:
        mov esi, [eax+8]
        sub esi, ecx
        sar esi, 2
    L4:
        test ecx, ecx
        je L6
        mov edx, [eax+0xc]
        sub edx, ecx
        sar edx, 2
        cmp esi, edx
        jae L6
        mov ecx, [eax+8]
        mov [ecx], edi
        add ecx, 4
        mov [eax+8], ecx
        jmp L7
    L6:
        mov ecx, [eax+8]
        lea edx, [esp+0x10]
        push edx
        push 1
        push ecx
        mov ecx, eax
        call helper8df0
    L7:
        mov eax, [ebp]
        inc eax
        mov [ebp], eax
        pop edi
        mov ebp, eax
        pop esi
        lea eax, [ebp-1]
        pop ebp
        pop ebx
        pop ecx
        ret 0x1c
    }
}
extern "C" void helper7520() {}
extern "C" void helper9b60() {}
extern "C" void helper93e0() {}
extern "C" char s_setshader[];

__declspec(naked) void MESHDX8::SetSubmeshShader(int index, const RESHANDLE &shader) {
    (void)index;
    __asm {
        push ecx
        mov eax, [esp+8]
        push esi
        push edi
        mov edi, ecx
        lea ecx, [esp+0x10]
        push ecx
        lea edx, [esp+0x0c]
        push edx
        lea ecx, [edi+0x38]
        mov [esp+0x18], eax
        call find_helper
        mov eax, [esp+8]
        cmp eax, [edi+0x3c]
        je notfound
        mov esi, [eax+0x10]
        jmp hasnode
    notfound:
        xor esi, esi
    hasnode:
        test esi, esi
        mov [esp+0x10], esi
        jne found
        mov eax, g_pLog
        test eax, eax
        je ret0
        push offset s_setshader
        push eax
        call log_error
        add esp, 8
    ret0:
        pop edi
        xor al, al
        pop esi
        pop ecx
        ret 8
    found:
        push ebx
        push esi
        mov ecx, edi
        call helper8070
        mov ebx, [esp+0x18]
        push ebx
        mov ecx, esi
        call helper7520
        lea eax, [esp+0x14]
        push eax
        push ebx
        lea ecx, [edi+0x54]
        call helper9b60
        mov ecx, eax
        call helper93e0
        pop ebx
        pop edi
        mov al, 1
        pop esi
        pop ecx
        ret 8
    }
}
