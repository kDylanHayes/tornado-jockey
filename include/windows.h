// Minimal stand-in for the Platform SDK's windows.h, which the Visual C++
// Toolkit 2003 doesn't ship. Declarations match the SDK's; add more as units
// need them.
#ifndef TJ_WINDOWS_H
#define TJ_WINDOWS_H

#define WINAPI __stdcall
#define WINBASEAPI __declspec(dllimport)
#define NO_ERROR 0L

typedef int BOOL;
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef long LONG;
typedef unsigned long ULONG;
typedef unsigned int UINT;
typedef void *PVOID;
typedef void *LPVOID;
typedef void *HANDLE;
typedef HANDLE *PHANDLE;
typedef const char *LPCSTR;
typedef ULONG *PULONG;
typedef struct HINSTANCE__ *HMODULE;
typedef int (WINAPI *FARPROC)();

typedef union _LARGE_INTEGER {
    struct {
        DWORD LowPart;
        LONG HighPart;
    };
    __int64 QuadPart;
} LARGE_INTEGER;

#define PROCESS_HEAP_ENTRY_BUSY 0x0004

typedef struct _PROCESS_HEAP_ENTRY {
    PVOID lpData;
    DWORD cbData;
    BYTE cbOverhead;
    BYTE iRegionIndex;
    WORD wFlags;
    union {
        struct {
            HANDLE hMem;
            DWORD dwReserved[3];
        } Block;
        struct {
            DWORD dwCommittedSize;
            DWORD dwUnCommittedSize;
            LPVOID lpFirstBlock;
            LPVOID lpLastBlock;
        } Region;
    };
} PROCESS_HEAP_ENTRY, *LPPROCESS_HEAP_ENTRY;

extern "C" {
WINBASEAPI DWORD WINAPI GetProcessHeaps(DWORD NumberOfHeaps, PHANDLE ProcessHeaps);
WINBASEAPI BOOL WINAPI HeapWalk(HANDLE hHeap, LPPROCESS_HEAP_ENTRY lpEntry);
WINBASEAPI BOOL WINAPI SetEnvironmentVariableA(LPCSTR lpName, LPCSTR lpValue);
WINBASEAPI BOOL WINAPI DeleteFileA(LPCSTR lpFileName);
WINBASEAPI HMODULE WINAPI GetModuleHandleA(LPCSTR lpModuleName);
WINBASEAPI FARPROC WINAPI GetProcAddress(HMODULE hModule, LPCSTR lpProcName);
WINBASEAPI void WINAPI Sleep(DWORD dwMilliseconds);
}

#endif
