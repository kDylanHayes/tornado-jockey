// The GUI, at 0x6d2700. Defined elsewhere; only what other units need so far.
#ifndef TONGAS2_GUI_H
#define TONGAS2_GUI_H

#include "Tongas2/CMDBASE.h"

// What happened to a key, for GUI::OnKey.
enum GUIKEYEVENT
{
    GUIKEY_DOWN = 1,
    GUIKEY_UP = 2,
    GUIKEY_CHAR = 3,
};

// What the mouse did, for GUI::OnMouse.
enum GUIMOUSEEVENT
{
    GUIMOUSE_MOVE = 2,
    GUIMOUSE_LDBLCLK = 5,
    GUIMOUSE_LDOWN = 6,
    GUIMOUSE_LUP = 7,
    GUIMOUSE_RDBLCLK = 8,
    GUIMOUSE_RDOWN = 9,
    GUIMOUSE_RUP = 10,
    GUIMOUSE_MDBLCLK = 11,
    GUIMOUSE_MDOWN = 12,
    GUIMOUSE_MUP = 13,
    GUIMOUSE_WHEELUP = 14,
    GUIMOUSE_WHEELDOWN = 15,
};

class GUI
{
public:
    static void RegisterCommands();

    // Passes the mouse to the window (through the window's slot 14) or not.
    // Placeholder name, inferred: while it isn't, a button let go outside
    // the window sends no message, so moving the mouse lets go of it.
    void SetMouseCapture(bool on);
    void OnMouse(int event, int x, int y);
    void OnKey(unsigned int key, int event);
    // Whether a mouse button is held down: 0 left, 1 right, 2 middle.
    bool IsButtonDown(int button) { return (m_buttons >> button & 1) != 0; }

    static int s_classId;
    static PROPENTRY s_properties[];

    char m_0[0x14a0];
    // The mouse buttons held down: 1 left, 2 right, 4 middle.
    unsigned int m_buttons;
    char m_14a4[0x31];
    bool m_mouseCapture;
};

extern GUI *g_pGui;

#endif
