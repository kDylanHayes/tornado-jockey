// The GUI controls. Defined elsewhere; only what CMDMGR needs so far.
#ifndef TONGAS2_GUICONTROL_H
#define TONGAS2_GUICONTROL_H

#include "Tongas2/CMDBASE.h"

class GUICONTROL
{
public:
    static void RegisterCommands();

    static int s_classId;
    static PROPENTRY s_properties[];
};

#endif
