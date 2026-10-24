// The input system, at 0x6d2700: the mouse and keyboard as the window passes
// them on, and the listeners it hands keys to. Defined elsewhere; only what
// other units need so far. Placeholder names apart from the class name.
#ifndef TONGAS2_INPUT_H
#define TONGAS2_INPUT_H

#include "Tongas2/SRESULT.h"

// What happened to a key, for INPUT::OnKey.
enum INPUTKEYEVENT
{
    INPUTKEY_DOWN = 1,
    INPUTKEY_UP = 2,
    INPUTKEY_CHAR = 3,
};

// What the mouse did, for INPUT::OnMouse.
enum INPUTMOUSEEVENT
{
    INPUTMOUSE_MOVE = 2,
    INPUTMOUSE_LDBLCLK = 5,
    INPUTMOUSE_LDOWN = 6,
    INPUTMOUSE_LUP = 7,
    INPUTMOUSE_RDBLCLK = 8,
    INPUTMOUSE_RDOWN = 9,
    INPUTMOUSE_RUP = 10,
    INPUTMOUSE_MDBLCLK = 11,
    INPUTMOUSE_MDOWN = 12,
    INPUTMOUSE_MUP = 13,
    INPUTMOUSE_WHEELUP = 14,
    INPUTMOUSE_WHEELDOWN = 15,
};

// Something the input system passes keys to, highest priority first, until
// one takes them.
class INPUTLISTENER
{
public:
    // Adds itself to the input system's listeners.
    INPUTLISTENER();
    virtual ~INPUTLISTENER();
    // Each true if it took the key.
    virtual bool OnKeyDown(unsigned int key) { return false; }
    virtual bool OnKeyUp(unsigned int key) { return false; }
    virtual bool OnChar(unsigned int c) { return false; }

    // Moves itself among the input system's listeners.
    void SetPriority(int priority);
};

class INPUT
{
public:
    // Passes the mouse to the window (through the window's slot 14) or not.
    // Inferred: while it isn't, a button let go outside the window sends no
    // message, so moving the mouse lets go of it.
    void SetMouseCapture(bool on);
    void OnMouse(int event, int x, int y);
    void OnKey(unsigned int key, int event);
    // Whether a mouse button is held down: 0 left, 1 right, 2 middle.
    bool IsButtonDown(int button) { return (m_buttons >> button & 1) != 0; }

    char m_0[0x1498];
    // Counts the clicks, as far as the GUI can tell.
    int m_clicks;
    char m_149c[4];
    // The mouse buttons held down: 1 left, 2 right, 4 middle.
    unsigned int m_buttons;
    // Where the mouse is, from 0 to 1 across the screen.
    V2 m_mouse;
    char m_14ac[0x29];
    bool m_mouseCapture;
};

extern INPUT *g_pInput;

#endif
