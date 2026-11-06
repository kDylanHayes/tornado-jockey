// Tongas2/HARDWARE -- hardware detection and the D3D capability cache.
//
// Discovery scaffold: bodies are empty until the unit is matched; each
// function has its exact target size in config/symbols.txt so a later delink
// cannot cut the target short. 0x00504dd0 and 0x00504ff0 are followed by
// embedded switch data, so they keep Ghidra's function size.
#include "Tongas2/HARDWARE.h"

#include <windows.h>

int HardwareA()
{
    HKEY key;
    unsigned long value;
    unsigned long size;

    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE,
                      "Hardware\\Description\\System\\CentralProcessor\\0", 0,
                      KEY_QUERY_VALUE, &key) == 0) {
        RegQueryValueExA(key, "~MHz", 0, 0, (LPBYTE)&value, &size);
        return value;
    }
    RegCloseKey(key);
    return -1;
}

extern "C" char s_vram_error[] = "VRAM requested before D3D setup completed";
extern "C" char s_detection_error[] = "DETECTION ERROR";

__declspec(naked) int HardwareC()
{
    __asm {
        push esi
        mov esi, ecx
        mov al, byte ptr [esi + 0x50]
        test al, al
        jnz done
        push 10h
        push offset s_detection_error
        push offset s_vram_error
        push 0
        call dword ptr [MessageBoxA]
    done:
        mov eax, dword ptr [esi + 0x1c]
        pop esi
        ret
    }
}

int HardwareD()
{
    return 0;
}

HARDWARE::HARDWARE()
{
}

void HARDWARE::Detect()
{
}
