// Starting other programs: the default web browser on a page, or any file
// through the shell. Placeholder names; the unit's own file name is unknown.
#ifndef TONGAS2_SHELL_H
#define TONGAS2_SHELL_H

#include <windows.h>

// Reads the string name under HKEY_LOCAL_MACHINE\key into value, which must
// hold MAX_PATH characters. TRUE if the key and the value were both there.
BOOL GetRegistryString(LPCSTR key, LPCSTR name, char *value);
// Starts a program, with message boxes if it can't be started.
BOOL RunProcess(LPCSTR application, LPSTR commandLine);
// Opens url in the program .htm files open with.
void OpenURL(const char *url);
// Opens file through the shell. With wait TRUE it waits for the program to
// exit and returns 0; otherwise it returns the program's process.
HANDLE ShellOpen(LPCSTR file, LPCSTR parameters, int wait);

#endif
