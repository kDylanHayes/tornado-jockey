/* Minimal stand-in for the Platform SDK's windows.h: only what ODE's error.cpp uses. */
#ifndef _WINDOWS_
#define _WINDOWS_
#ifdef __cplusplus
extern "C" {
#endif
#define MB_OK 0x00000000L
#define MB_ICONSTOP 0x00000010L
#define MB_ICONWARNING 0x00000030L
__declspec(dllimport) int __stdcall MessageBoxA(void *hWnd, const char *lpText, const char *lpCaption, unsigned int uType);
#define MessageBox MessageBoxA
#ifdef __cplusplus
}
#endif
#endif
