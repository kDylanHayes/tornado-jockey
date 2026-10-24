// The GUI, at 0x6d2700. Defined elsewhere; only what other units need so far.
#ifndef TONGAS2_GUI_H
#define TONGAS2_GUI_H

#include "Tongas2/CMDBASE.h"

class GUI
{
public:
    static void RegisterCommands();

    // Lays the screens out for a wide screen or not.
    void SetWideScreen(bool on);

    static int s_classId;
    static PROPENTRY s_properties[];

    char m_0[0x14d5];
    bool m_wideScreen;
};

extern GUI *g_pGui;

#endif
