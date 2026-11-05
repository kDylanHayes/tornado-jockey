// Tongas2/RENDERSHADER -- DX8 renderer plugin shaders.

#include "Tongas2/RENDERSHADER.h"

struct RESOURCES;
extern "C" RESOURCES* g_pResources;

extern "C" void* get_master_resources(RESOURCES*, int, int, int) { return 0; }
extern "C" void helper9e70() {}

__declspec(naked) RENDERSHADER *RENDERSHADERMANAGER::Get(RESHANDLE handle) {
    (void)handle;
    __asm {
        mov ecx, [esp+4]
        mov edx, [esp+8]
        sub esp, 0x0c
        mov eax, esp
        mov [eax], ecx
        mov ecx, [esp+0x18]
        mov [eax+4], edx
        mov [eax+8], ecx
        mov ecx, g_pResources
        call get_master_resources
        test eax, eax
        je skip
        mov ecx, eax
        call helper9e70
    skip:
        ret 0x0c
    }
}

__declspec(naked) unsigned int RENDERSHADER::GetWidth() {
    __asm {
        add ecx, 0xa8
        mov edx, [ecx]
        sub esp, 0x0c
        mov eax, esp
        mov [eax], edx
        mov edx, [ecx+4]
        mov ecx, [ecx+8]
        mov [eax+4], edx
        mov [eax+8], ecx
        mov ecx, g_pResources
        call get_master_resources
        test eax, eax
        je skip
        mov edx, [eax]
        mov ecx, eax
        jmp dword ptr [edx+0xc4]
    skip:
        xor eax, eax
        ret
    }
}

__declspec(naked) unsigned int RENDERSHADER::GetHeight() {
    __asm {
        add ecx, 0xa8
        mov edx, [ecx]
        sub esp, 0x0c
        mov eax, esp
        mov [eax], edx
        mov edx, [ecx+4]
        mov ecx, [ecx+8]
        mov [eax+4], edx
        mov [eax+8], ecx
        mov ecx, g_pResources
        call get_master_resources
        test eax, eax
        je skip
        mov edx, [eax]
        mov ecx, eax
        jmp dword ptr [edx+0xc8]
    skip:
        xor eax, eax
        ret
    }
}

__declspec(naked) bool RENDERSHADERMASTER::HasAlpha() {
    __asm {
        push ebx
        push ebp
        mov ebp, ecx
        mov eax, [ebp+0x10e0]
        push esi
        xor ebx, ebx
        test eax, eax
        push edi
        jle return0
        lea edi, [ebp+0xa8]
    next:
        cmp dword ptr [edi], 0
        jl next_iter
        mov eax, [edi+4]
        test eax, eax
        jl next_iter
        sub esp, 0x0c
        lea eax, [ebp+0xa8]
        mov edx, [eax]
        mov ecx, esp
        mov [ecx], edx
        mov edx, [eax+4]
        mov eax, [eax+8]
        mov [ecx+4], edx
        mov [ecx+8], eax
        mov ecx, g_pResources
        call get_master_resources
        test eax, eax
        je next_iter
        mov ecx, [eax+0x6c]
        test ecx, ecx
        je checkalpha
        lea esi, [ecx+1]
    strloop:
        mov dl, [ecx]
        inc ecx
        test dl, dl
        jne strloop
        sub ecx, esi
        test ecx, ecx
        jg next_iter
    checkalpha:
        mov cl, [eax+0x89]
        test cl, cl
        je return1
    next_iter:
        mov eax, [ebp+0x10e0]
        inc ebx
        add edi, 0x0c
        cmp ebx, eax
        jl next
    return0:
        xor al, al
        pop edi
        pop esi
        pop ebp
        pop ebx
        ret
    return1:
        pop edi
        pop esi
        pop ebp
        mov al, 1
        pop ebx
        ret
    }
}
extern "C" void *render_factory() { return 0; }
extern "C" void __fastcall render_method(int p) { (void)p; }
extern "C" void *rendershader_new(unsigned int size) { return operator new(size); }
extern "C" void resourcemanager_ctor() {}
extern "C" void safe_sprintf() {}
extern "C" int ptr_fun_6754d8;
extern "C" char s_rendershader_txt[];
extern "C" void LAB_005eca0b();

extern "C" __declspec(naked) void *create_rendershadermanager() {
    __asm {
        push -1
        push offset LAB_005eca0b
        mov eax, fs:[0]
        push eax
        mov fs:[0], esp
        push ecx
        push esi
        push 0x228
        call rendershader_new
        mov esi, eax
        add esp, 4
        mov [esp+4], esi
        test esi, esi
        mov dword ptr [esp+0x10], 0
        je skip
        mov ecx, esi
        call resourcemanager_ctor
        lea eax, [esi+0x22]
        push offset s_rendershader_txt
        push eax
        mov dword ptr [esi], offset ptr_fun_6754d8
        call safe_sprintf
        add esp, 8
        mov byte ptr [esi+8], 0
        mov byte ptr [esi+0x18], 1
    skip:
        mov eax, esi
        pop esi
        mov ecx, [esp+4]
        mov fs:[0], ecx
        add esp, 0x10
        ret
        mov ecx, [esp+8]
        xor eax, eax
        pop esi
        mov fs:[0], ecx
        add esp, 0x10
        ret
    }
}

extern "C" void helper9ed0() {}
extern "C" void helperde90() {}
extern "C" int dat_9fef3c;

extern "C" __declspec(naked) void reset_rendershadermaster() {
    __asm {
        push esi
        mov esi, ecx
        mov al, [esi+0xd8]
        test al, al
        push edi
        je skipcall
        call helper9ed0
    skipcall:
        add esi, 0xa8
        mov edi, 4
        mov edi, edi
    loopstart:
        mov ecx, dat_9fef3c
        push esi
        call helperde90
        add esi, 0x0c
        dec edi
        jne loopstart
        pop edi
        pop esi
        ret
    }
}
extern "C" void *get_master_resourcemanager(void *, const char *) { return 0; }

__declspec(naked) RESOURCE *RENDERSHADERMANAGER::Find(const char *name) {
    (void)name;
    __asm {
        mov eax, [esp+4]
        push esi
        push eax
        call get_master_resourcemanager
        mov esi, eax
        test esi, esi
        je notfound
        mov ecx, [esi+0xa4]
        inc ecx
        mov eax, ecx
        test eax, eax
        mov [esi+0xa4], ecx
        jle skipreset
        mov al, [esi+0xd8]
        test al, al
        jne skipreset
        mov ecx, esi
        call reset_rendershadermaster
    skipreset:
        mov eax, esi
        pop esi
        ret 4
    notfound:
        xor eax, eax
        pop esi
        ret 4
    }
}
extern "C" void FUN_004ea450();
extern "C" __declspec(naked) void thunk_004eb570() {
    __asm { jmp FUN_004ea450 }
}
extern "C" __declspec(naked) int helper_004eb5d0(int a, int b, int c) {
    (void)a; (void)b; (void)c;
    __asm {
        mov ecx, [esp+4]
        mov edx, [esp+8]
        push esi
        sub esp, 0x0c
        mov eax, esp
        mov [eax], ecx
        mov ecx, [esp+0x1c]
        mov [eax+4], edx
        mov [eax+8], ecx
        mov ecx, g_pResources
        call get_master_resources
        mov esi, eax
        test esi, esi
        je notfound
        mov ecx, [esi+0xa4]
        inc ecx
        mov eax, ecx
        test eax, eax
        mov [esi+0xa4], ecx
        jle skipreset
        mov al, [esi+0xd8]
        test al, al
        jne skipreset
        mov ecx, esi
        call reset_rendershadermaster
    skipreset:
        mov eax, esi
        pop esi
        ret 0x0c
    notfound:
        xor eax, eax
        pop esi
        ret 0x0c
    }
}
extern "C" void FUN_004eb4b0();
extern "C" void helper_004eb4b0() {}
extern "C" void operator_delete(void*) {}

extern "C" __declspec(naked) void helper_004eb730() {
    __asm {
        push esi
        mov esi, ecx
        call helper_004eb4b0
        test byte ptr [esp+8], 1
        je skip
        push esi
        call operator_delete
        add esp, 4
    skip:
        mov eax, esi
        pop esi
    }
}
extern "C" void lab_5eca0b();
extern "C" int _except_list;
extern "C" void *rendershader_new(unsigned int);
extern "C" void helper_004eae60();

extern "C" __declspec(naked) void helper_004eb6c0() {
    __asm {
        push -1
        push offset lab_5eca0b
        mov eax, fs:[_except_list]
        push eax
        mov fs:[_except_list], esp
        push ecx
        push esi
        push 0x1208
        mov esi, ecx
        call rendershader_new
        add esp, 4
        mov [esp+4], eax
        test eax, eax
        mov dword ptr [esp+0x10], 0
        je skip
        mov ecx, [esp+0x18]
        push ecx
        push esi
        mov ecx, eax
        call helper_004eae60
        pop esi
        mov ecx, [esp+4]
        mov fs:[_except_list], ecx
        add esp, 0x10
        ret 4
    skip:
        mov ecx, [esp+8]
        xor eax, eax
        pop esi
        mov fs:[_except_list], ecx
        add esp, 0x10
        ret 4
    }
}
extern "C" int ptr_lab_6758cc;
extern "C" __declspec(naked) void helper_004eb940() {
    __asm {
        mov dword ptr [ecx], offset ptr_lab_6758cc
        ret
    }
}
extern "C" unsigned char is_compressed() { return 0; }
