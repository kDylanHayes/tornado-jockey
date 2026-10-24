// The copy protection. Defined elsewhere; only what CMDMGR needs so far.
#ifndef TONGAS2_DRM_H
#define TONGAS2_DRM_H

#include "Tongas2/CMDBASE.h"

class DRM
{
public:
    static void RegisterCommands();

    static int s_classId;
    static PROPENTRY s_properties[];
};

#endif
