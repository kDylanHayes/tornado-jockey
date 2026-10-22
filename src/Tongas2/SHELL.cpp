#include <string.h>

#include "Tongas2/SHELL.h"

#define MAX_PATH 260

BOOL GetRegistryString(LPCSTR key, LPCSTR name, char *value)
{
    HKEY hKey;
    DWORD size = MAX_PATH - 1;
    DWORD type = REG_SZ;
    LONG opened = RegOpenKeyExA(HKEY_LOCAL_MACHINE, key, 0, KEY_QUERY_VALUE, &hKey);
    LONG queried = RegQueryValueExA(hKey, name, NULL, &type, (LPBYTE)value, &size);
    if (value[size] != 0)
        value[size] = 0;
    RegCloseKey(hKey);
    if (opened == ERROR_SUCCESS && queried == ERROR_SUCCESS)
        return TRUE;
    return FALSE;
}

BOOL RunProcess(LPCSTR application, LPSTR commandLine)
{
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    memset(&si, 0, sizeof(si));
    si.cb = sizeof(si);
    if (CreateProcessA(application, commandLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi) == TRUE) {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return TRUE;
    }
    GetLastError();
    MessageBoxA(NULL, "Process failed to create.", "Error", MB_OK);
    MessageBoxA(NULL, application, commandLine, MB_OK);
    return FALSE;
}

void OpenURL(const char *url)
{
    char key[] = "Software\\CLASSES\\.htm";
    char fileClass[MAX_PATH] = "";
    GetRegistryString(key, NULL, fileClass);
    char commandKey[MAX_PATH] = "";
    wsprintfA(commandKey, "Software\\CLASSES\\%s\\shell\\open\\command", fileClass);
    char command[MAX_PATH];
    GetRegistryString(commandKey, NULL, command);
    char *program = strtok(command, "\"");
    char commandLine[0x1000];
    wsprintfA(commandLine, "%s %s", program, url);
    RunProcess(NULL, commandLine);
}

HANDLE ShellOpen(LPCSTR file, LPCSTR parameters, int wait)
{
    SHELLEXECUTEINFOA sei;
    memset(&sei, 0, sizeof(sei));
    sei.cbSize = sizeof(sei);
    sei.fMask = SEE_MASK_NOCLOSEPROCESS;
    sei.nShow = SW_SHOWNORMAL;
    sei.lpFile = file;
    sei.lpParameters = parameters;
    if (!ShellExecuteExA(&sei))
        return NULL;
    if (wait == TRUE) {
        DWORD code;
        do {
            code = 0;
            if (!sei.hProcess || !GetExitCodeProcess(sei.hProcess, &code))
                return NULL;
        } while (code == STILL_ACTIVE);
        return NULL;
    }
    return sei.hProcess;
}
