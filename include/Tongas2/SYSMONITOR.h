// Samples the process heap size and system CPU usage. Placeholder name; the
// original isn't known.
#ifndef TONGAS2_SYSMONITOR_H
#define TONGAS2_SYSMONITOR_H

#include <windows.h>

typedef LONG (WINAPI *PROCNTQSI)(UINT, PVOID, ULONG, PULONG);

class SYSMONITOR
{
public:
    SYSMONITOR();
    virtual ~SYSMONITOR() { DeleteFileA("heap.out"); }

    void UpdateHeapUsage();
    int GetCpuUsage();
    void Update();

    PROCNTQSI m_pfnNtQuerySystemInformation;
    DWORD m_numHeaps;
    HANDLE m_heaps[256];
    DWORD m_heapUsed;
    DWORD m_unknown410;
    DWORD m_sampleTime; // ms between CPU samples
};

#endif
