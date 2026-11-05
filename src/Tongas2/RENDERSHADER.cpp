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
