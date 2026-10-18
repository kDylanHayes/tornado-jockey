#include <string.h>

#include "Tongas2/LOG.h"
#include "Tongas2/SYSMONITOR.h"

// NtQuerySystemInformation classes and layouts, as in the well-known
// "CPU usage on NT" sample this is based on.
#define SystemBasicInformation 0
#define SystemPerformanceInformation 2
#define SystemTimeInformation 3

#define Li2Double(x) ((double)((x).HighPart) * 4.294967296E9 + (double)((x).LowPart))

typedef struct
{
    DWORD dwUnknown1;
    ULONG uKeMaximumIncrement;
    ULONG uPageSize;
    ULONG uMmNumberOfPhysicalPages;
    ULONG uMmLowestPhysicalPage;
    ULONG uMmHighestPhysicalPage;
    ULONG uAllocationGranularity;
    PVOID pLowestUserAddress;
    PVOID pMmHighestUserAddress;
    ULONG uKeActiveProcessors;
    BYTE bKeNumberProcessors;
    BYTE bUnknown2;
    WORD wUnknown3;
} SYSTEM_BASIC_INFORMATION;

typedef struct
{
    LARGE_INTEGER liIdleTime;
    DWORD dwSpare[76];
} SYSTEM_PERFORMANCE_INFORMATION;

typedef struct
{
    LARGE_INTEGER liKeBootTime;
    LARGE_INTEGER liKeSystemTime;
    LARGE_INTEGER liExpTimeZoneBias;
    ULONG uCurrentTimeZoneId;
    DWORD dwReserved;
} SYSTEM_TIME_INFORMATION;

void SYSMONITOR::UpdateHeapUsage()
{
    m_heapUsed = 0;
    memset(m_heaps, 0, sizeof(m_heaps));
    m_numHeaps = GetProcessHeaps(256, m_heaps);
    for (DWORD i = 0; i < m_numHeaps; i++) {
        PROCESS_HEAP_ENTRY entry;
        entry.lpData = NULL;
        while (HeapWalk(m_heaps[i], &entry)) {
            if (entry.wFlags & PROCESS_HEAP_ENTRY_BUSY)
                m_heapUsed += entry.cbData;
        }
    }
}

SYSMONITOR::SYSMONITOR()
{
    m_sampleTime = 1000;
    if (SetEnvironmentVariableA("__MSVCRT_HEAP_SELECT", "__GLOBAL_HEAP_SELECTED,1")) {
        m_heapUsed = 0;
        m_unknown410 = 0;
    }
}

int SYSMONITOR::GetCpuUsage()
{
    SYSTEM_PERFORMANCE_INFORMATION SysPerfInfo;
    SYSTEM_TIME_INFORMATION SysTimeInfo;
    SYSTEM_BASIC_INFORMATION SysBaseInfo;
    double dbIdleTime;
    double dbSystemTime;
    LONG status;
    static LARGE_INTEGER liOldIdleTime = {0, 0};
    static LARGE_INTEGER liOldSystemTime = {0, 0};

    m_pfnNtQuerySystemInformation =
        (PROCNTQSI)GetProcAddress(GetModuleHandleA("ntdll"), "NtQuerySystemInformation");
    if (!m_pfnNtQuerySystemInformation)
        return 0;

    status = m_pfnNtQuerySystemInformation(SystemBasicInformation, &SysBaseInfo, sizeof(SysBaseInfo), NULL);
    if (status != NO_ERROR)
        return 0;

    for (;;) {
        status = m_pfnNtQuerySystemInformation(SystemTimeInformation, &SysTimeInfo, sizeof(SysTimeInfo), 0);
        if (status != NO_ERROR)
            return 0;

        status = m_pfnNtQuerySystemInformation(SystemPerformanceInformation, &SysPerfInfo, sizeof(SysPerfInfo), NULL);
        if (status != NO_ERROR)
            return 0;

        if (liOldIdleTime.QuadPart != 0) {
            dbIdleTime = Li2Double(SysPerfInfo.liIdleTime) - Li2Double(liOldIdleTime);
            dbSystemTime = Li2Double(SysTimeInfo.liKeSystemTime) - Li2Double(liOldSystemTime);
            dbIdleTime = dbIdleTime / dbSystemTime;
            dbIdleTime = 100.0 - dbIdleTime * 100.0 / (double)SysBaseInfo.bKeNumberProcessors + 0.5;
            return (int)dbIdleTime;
        }

        liOldIdleTime = SysPerfInfo.liIdleTime;
        liOldSystemTime = SysTimeInfo.liKeSystemTime;
        Sleep(m_sampleTime);
    }
}

void SYSMONITOR::Update()
{
    if (g_pLog) {
        UpdateHeapUsage();
        GetCpuUsage();
    }
}
