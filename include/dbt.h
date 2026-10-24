// Minimal stand-in for the Platform SDK's dbt.h (see windows.h).
#ifndef TJ_DBT_H
#define TJ_DBT_H

#include <windows.h>

#define DBT_DEVICEARRIVAL 0x8000
#define DBT_DEVICEREMOVECOMPLETE 0x8004
#define DBT_DEVTYP_VOLUME 0x00000002
#define DBTF_MEDIA 0x0001

typedef struct _DEV_BROADCAST_HDR {
    DWORD dbch_size;
    DWORD dbch_devicetype;
    DWORD dbch_reserved;
} DEV_BROADCAST_HDR, *PDEV_BROADCAST_HDR;

typedef struct _DEV_BROADCAST_VOLUME {
    DWORD dbcv_size;
    DWORD dbcv_devicetype;
    DWORD dbcv_reserved;
    DWORD dbcv_unitmask;
    WORD dbcv_flags;
} DEV_BROADCAST_VOLUME, *PDEV_BROADCAST_VOLUME;

#endif
