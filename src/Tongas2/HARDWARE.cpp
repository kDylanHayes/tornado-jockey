// Tongas2/HARDWARE -- hardware detection and the D3D capability cache.
//
// The HARDWARE object (0x00504f90) is a std::string holding the detected
// adapter name followed by the capability cache the detector at 0x00504ff0
// fills: six values at 0x20, six byte-level defaults at 0x38 and the
// "detection finished" flags at 0x50. 0x00504dd0 reads the OS version and
// 0x00504f10/0x00504f50 are the guarded capability accessors.
#include "Tongas2/HARDWARE.h"

#include <windows.h>

#include <new>
#include <string>
#include <string.h>

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
extern "C" char s_capability_error[] =
    "D3D Capability Value requested before D3D setup completed";
extern "C" char s_capability_hold_error[] =
    "D3D Capability requested before D3D setup completed";

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

typedef struct _OSVERSIONINFOA
{
    DWORD dwOSVersionInfoSize;
    DWORD dwMajorVersion;
    DWORD dwMinorVersion;
    DWORD dwBuildNumber;
    DWORD dwPlatformId;
    char szCSDVersion[128];
} OSVERSIONINFOA;

extern "C" __declspec(dllimport) BOOL __stdcall
GetVersionExA(OSVERSIONINFOA *lpVersionInformation);

// 0x00504dd0 -- OS version class used by the capability detector.
int HardwareD()
{
    OSVERSIONINFOA os;

    memset(&os, 0, sizeof(os));
    os.dwOSVersionInfoSize = sizeof(os);
    if (!GetVersionExA(&os))
        return 8;

    switch (os.dwMajorVersion) {
    case 3:
        return 3;
    case 4:
        switch (os.dwMinorVersion) {
        case 0:
            return 0;
        case 10:
            return 1;
        case 51:
            return 4;
        case 90:
            return 2;
        }
        break;
    case 5:
        switch (os.dwMinorVersion) {
        case 0:
            return 5;
        case 1:
            return 6;
        case 2:
            return 7;
        }
        break;
    }
    return 8;
}

// 0x00504f10 and 0x00504f50 -- guarded accessors for the six cached
// D3D capability values. Both are HARDWARE members (this in ecx); they are
// written out of line here because HARDWARE.h still carries the opaque
// scaffold layout.
int HARDWARE::HardwareE(int index)
{
    bool ready = *(bool *)((char *)this + 0x50);

    if (!ready) {
        MessageBoxA(0, s_capability_error, s_detection_error, 0x10);
        return ((int *)((char *)this + 0x20))[index];
    }
    return ((int *)((char *)this + 0x20))[index];
}

bool HARDWARE::IsSupported(int index)
{
    bool ready = *(bool *)((char *)this + 0x50);

    if (!ready) {
        MessageBoxA(0, s_capability_hold_error, s_detection_error, 0x10);
        return ((bool *)((char *)this + 0x38))[index];
    }
    return ((bool *)((char *)this + 0x38))[index];
}

// Object layout, 0x54 bytes. The HARDWARE class itself is still the opaque
// scaffold in HARDWARE.h, so the constructor describes the members here.
namespace {

struct HWCORE
{
    std::string name;              // 0x00, "UNKNOWN" until Detect runs
    int m_1c;                      // 0x1c
    unsigned long m_values[6];     // 0x20
    unsigned long m_levels[6];     // 0x38, four one-byte levels per dword
    unsigned char m_50;            // 0x50
    unsigned char m_51;            // 0x51
    unsigned char m_52;            // 0x52
};

}

HARDWARE::HARDWARE()
{
    HWCORE *core = (HWCORE *)this;

    // name("UNKNOWN"): _Tidy() leaves the string empty, then it assigns.
    *(unsigned long *)((char *)this + 0x18) = 0xf;
    *(unsigned long *)((char *)this + 0x14) = 0;
    ((char *)this)[0x04] = 0;
    core->name.assign("UNKNOWN", 7);
    core->m_1c = 0;
    core->m_50 = 0;
    core->m_51 = 0;
    core->m_52 = 0;
    for (int i = 0; i < 6; ++i)
        core->m_levels[i] = 0x01010101;
    for (int i = 0; i < 6; ++i)
        core->m_values[i] = 0;
}

void HARDWARE::Detect()
{
}
