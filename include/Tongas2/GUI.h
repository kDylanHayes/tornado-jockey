// The GUI. Defined elsewhere; only what CMDMGR needs so far.
#ifndef TONGAS2_GUI_H
#define TONGAS2_GUI_H

#include "Tongas2/CMDBASE.h"

class GUI
{
public:
    static void RegisterCommands();

    static int s_classId;
    static PROPENTRY s_properties[];
};

#endif
