// Minimal stand-in for the Platform SDK's windows.h, which the Visual C++
// Toolkit 2003 doesn't ship. Declarations match the SDK's; add more as units
// need them.
#ifndef TJ_WINDOWS_H
#define TJ_WINDOWS_H

#define WINAPI __stdcall
#define WINBASEAPI __declspec(dllimport)
#define WINADVAPI __declspec(dllimport)
#define WINUSERAPI __declspec(dllimport)
#define SHSTDAPI_(type) extern "C" __declspec(dllimport) type __stdcall
#define WINAPIV __cdecl
#define TRUE 1
#define FALSE 0
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
typedef char *LPSTR;
typedef BYTE *LPBYTE;
typedef DWORD *LPDWORD;
typedef WORD *LPWORD;
typedef ULONG *PULONG;
typedef struct HINSTANCE__ *HMODULE;
typedef int (WINAPI *FARPROC)();
typedef struct HINSTANCE__ *HINSTANCE;
typedef struct HWND__ *HWND;
typedef struct HKEY__ *HKEY;
typedef HKEY *PHKEY;
typedef DWORD ACCESS_MASK;
typedef ACCESS_MASK REGSAM;

#define HKEY_LOCAL_MACHINE ((HKEY)(ULONG)0x80000002)
#define KEY_QUERY_VALUE 0x0001
#define REG_SZ 1
#define ERROR_SUCCESS 0L
#define STILL_ACTIVE 0x00000103L
#define MB_OK 0x00000000L
#define SW_SHOWNORMAL 1
#define SEE_MASK_NOCLOSEPROCESS 0x00000040

typedef struct _SECURITY_ATTRIBUTES {
    DWORD nLength;
    LPVOID lpSecurityDescriptor;
    BOOL bInheritHandle;
} SECURITY_ATTRIBUTES, *LPSECURITY_ATTRIBUTES;

typedef struct _STARTUPINFOA {
    DWORD cb;
    LPSTR lpReserved;
    LPSTR lpDesktop;
    LPSTR lpTitle;
    DWORD dwX;
    DWORD dwY;
    DWORD dwXSize;
    DWORD dwYSize;
    DWORD dwXCountChars;
    DWORD dwYCountChars;
    DWORD dwFillAttribute;
    DWORD dwFlags;
    WORD wShowWindow;
    WORD cbReserved2;
    LPBYTE lpReserved2;
    HANDLE hStdInput;
    HANDLE hStdOutput;
    HANDLE hStdError;
} STARTUPINFOA, *LPSTARTUPINFOA;

typedef struct _PROCESS_INFORMATION {
    HANDLE hProcess;
    HANDLE hThread;
    DWORD dwProcessId;
    DWORD dwThreadId;
} PROCESS_INFORMATION, *LPPROCESS_INFORMATION;

typedef struct _SHELLEXECUTEINFOA {
    DWORD cbSize;
    ULONG fMask;
    HWND hwnd;
    LPCSTR lpVerb;
    LPCSTR lpFile;
    LPCSTR lpParameters;
    LPCSTR lpDirectory;
    int nShow;
    HINSTANCE hInstApp;
    LPVOID lpIDList;
    LPCSTR lpClass;
    HKEY hkeyClass;
    DWORD dwHotKey;
    HANDLE hIcon;
    HANDLE hProcess;
} SHELLEXECUTEINFOA, *LPSHELLEXECUTEINFOA;

typedef union _LARGE_INTEGER {
    struct {
        DWORD LowPart;
        LONG HighPart;
    };
    __int64 QuadPart;
} LARGE_INTEGER;

typedef struct _COORD {
    short X;
    short Y;
} COORD;

typedef struct _SMALL_RECT {
    short Left;
    short Top;
    short Right;
    short Bottom;
} SMALL_RECT;

typedef struct _CONSOLE_SCREEN_BUFFER_INFO {
    COORD dwSize;
    COORD dwCursorPosition;
    WORD wAttributes;
    SMALL_RECT srWindow;
    COORD dwMaximumWindowSize;
} CONSOLE_SCREEN_BUFFER_INFO, *PCONSOLE_SCREEN_BUFFER_INFO;

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
WINBASEAPI BOOL WINAPI CloseHandle(HANDLE hObject);
WINBASEAPI DWORD WINAPI GetLastError(void);
WINBASEAPI BOOL WINAPI GetExitCodeProcess(HANDLE hProcess, LPDWORD lpExitCode);
WINBASEAPI BOOL WINAPI CreateProcessA(LPCSTR lpApplicationName, LPSTR lpCommandLine,
    LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes,
    BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, LPCSTR lpCurrentDirectory,
    LPSTARTUPINFOA lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation);
WINADVAPI LONG WINAPI RegOpenKeyExA(HKEY hKey, LPCSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult);
WINADVAPI LONG WINAPI RegQueryValueExA(HKEY hKey, LPCSTR lpValueName, LPDWORD lpReserved, LPDWORD lpType,
    LPBYTE lpData, LPDWORD lpcbData);
WINADVAPI LONG WINAPI RegCloseKey(HKEY hKey);
WINBASEAPI BOOL WINAPI GetConsoleScreenBufferInfo(HANDLE hConsoleOutput,
    PCONSOLE_SCREEN_BUFFER_INFO lpConsoleScreenBufferInfo);
WINBASEAPI BOOL WINAPI SetConsoleTextAttribute(HANDLE hConsoleOutput, WORD wAttributes);
WINUSERAPI int WINAPI MessageBoxA(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType);
WINUSERAPI int WINAPIV wsprintfA(LPSTR, LPCSTR, ...);
}

SHSTDAPI_(BOOL) ShellExecuteExA(LPSHELLEXECUTEINFOA lpExecInfo);

#endif
