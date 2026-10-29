// The input system, at 0x6d2700: the mouse and keyboard as the window passes
// them on, the joysticks the input plugin reads, and the listeners it hands
// them to, highest priority first. The plugin derives from INPUT. Placeholder
// names apart from the class names and the ActivateJoy message.
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

// The mouse as the listeners get it. Plain floats, not V2s: the original
// copies it inline, which V2's copy constructor would stop.
struct MOUSESTATE
{
    MOUSESTATE()
    {
        buttons = 0;
        x = y = 0;
        lastX = lastY = 0;
    }

    // The buttons held down: 1 left, 2 right, 4 middle.
    unsigned int buttons;
    // Where the mouse is, from 0 to 1 across the screen, and where it was.
    float x, y;
    float lastX, lastY;
};

// Something the input system passes the mouse to. Each handler returns
// whether it took the event; a listener with priority -1 hears everything
// and takes nothing.
class MOUSELISTENER
{
public:
    // Adds itself to the input system's listeners.
    MOUSELISTENER();
    virtual ~MOUSELISTENER();
    virtual bool OnMouseMove(MOUSESTATE state) { return false; }
    virtual bool OnLeftDown(MOUSESTATE state) { return false; }
    virtual bool OnLeftUp(MOUSESTATE state) { return false; }
    virtual bool OnLeftDoubleClick(MOUSESTATE state) { return false; }
    // Every tenth of a second while the button is held.
    virtual bool OnLeftHeld(MOUSESTATE state) { return false; }
    // The mouse moving once it has been held.
    virtual bool OnLeftDrag(MOUSESTATE state) { return false; }
    virtual bool OnRightDown(MOUSESTATE state) { return false; }
    virtual bool OnRightUp(MOUSESTATE state) { return false; }
    virtual bool OnRightDoubleClick(MOUSESTATE state) { return false; }
    virtual bool OnRightHeld(MOUSESTATE state) { return false; }
    virtual bool OnRightDrag(MOUSESTATE state) { return false; }
    virtual bool OnMiddleDown(MOUSESTATE state) { return false; }
    virtual bool OnMiddleUp(MOUSESTATE state) { return false; }
    virtual bool OnMiddleDoubleClick(MOUSESTATE state) { return false; }
    virtual bool OnMiddleHeld(MOUSESTATE state) { return false; }
    virtual bool OnMiddleDrag(MOUSESTATE state) { return false; }
    virtual bool OnWheelUp(MOUSESTATE state) { return false; }
    virtual bool OnWheelDown(MOUSESTATE state) { return false; }

    // Moves itself among the input system's listeners.
    void SetPriority(int priority);
    // Takes itself out of them.
    void Unregister();
};

// Something the input system passes keys to.
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

// A joystick's buttons and axes, which the input plugin fills in.
struct JOY
{
    JOY()
    {
        m_active = false;
        m_numButtons = 0x80;
        m_numAxes = 5;
        m_buttons = new int[0x80];
        m_wasDown = new bool[m_numButtons];
        m_axes = new int[m_numAxes];
    }
    ~JOY()
    {
        if (m_buttons) {
            delete[] m_buttons;
            m_buttons = 0;
        }
        if (m_wasDown) {
            delete[] m_wasDown;
            m_wasDown = 0;
        }
        if (m_axes) {
            delete[] m_axes;
            m_axes = 0;
        }
    }

    // Turns it on or off, letting go of everything.
    void Activate(bool on);

    bool m_active;
    unsigned int m_numButtons;
    int *m_buttons;
    // Which buttons were down the frame before.
    bool *m_wasDown;
    unsigned int m_numAxes;
    int *m_axes;
};

#define MAX_INPUT_LISTENERS 0x80

class INPUT
{
public:
    INPUT();
    virtual ~INPUT();
    // Opens the input devices.
    virtual bool Init() { return true; }
    // Repeats the held mouse buttons and reads the joysticks for the frame.
    virtual void Update();
    // Reads a joystick.
    virtual void UpdateJoy(int id) {}

    // Passes the mouse to the window (through the window's slot 14) or not.
    // Inferred: while it isn't, a button let go outside the window sends no
    // message, so moving the mouse lets go of it.
    void SetMouseCapture(bool on);
    void OnMouse(int event, int x, int y);
    void OnKey(unsigned int key, int event);
    bool IsKeyDown(int key);
    // Whether a mouse button is held down: 0 left, 1 right, 2 middle.
    bool IsButtonDown(int button) { return (m_mouse.buttons >> button & 1) != 0; }

    // Makes room for the joysticks.
    void SetNumJoys(int count);
    void ActivateJoy(int id, bool on);
    bool IsJoyActive(int id);
    // Whether a joystick's button went up this frame.
    bool IsJoyButtonReleased(int id, unsigned int button);

    // Adds a listener with a priority, or moves it to the priority.
    // Priority -2 shuts out every other listener while it is there.
    void AddMouseListener(MOUSELISTENER *listener, int priority);
    void RemoveMouseListener(MOUSELISTENER *listener);
    void AddKeyListener(INPUTLISTENER *listener, int priority);
    void RemoveKeyListener(INPUTLISTENER *listener);

    // Calls a handler on the listeners, from the highest priority down, until
    // one at a priority takes it. Listeners added this frame are left out.
    void DispatchMouse(int handler, MOUSESTATE state);
    void DispatchKey(int handler, unsigned int key);
    // Works out the highest priority and whether one shuts out the rest.
    void UpdateTopPriority();

    bool m_4;
    int m_8;
    // Which keys are down. Keys go up to 0xff, one past the end.
    bool m_keys[0xff];
    // Each listener's priority, when it was added and whether it has had
    // the event being dispatched.
    bool m_mouseSent[MAX_INPUT_LISTENERS];
    int m_mousePriorities[MAX_INPUT_LISTENERS];
    float m_mouseTimes[MAX_INPUT_LISTENERS];
    MOUSELISTENER *m_mouseListeners[MAX_INPUT_LISTENERS];
    bool m_keySent[MAX_INPUT_LISTENERS];
    int m_keyPriorities[MAX_INPUT_LISTENERS];
    float m_keyTimes[MAX_INPUT_LISTENERS];
    INPUTLISTENER *m_keyListeners[MAX_INPUT_LISTENERS];
    // Joystick listeners; nothing adds any.
    bool m_joySent[MAX_INPUT_LISTENERS];
    int m_joyPriorities[MAX_INPUT_LISTENERS];
    float m_joyTimes[MAX_INPUT_LISTENERS];
    void *m_joyListeners[MAX_INPUT_LISTENERS];
    int m_numMouseListeners;
    int m_numKeyListeners;
    int m_numJoyListeners;
    int m_topPriority;
    bool m_exclusive;
    // The move that centring the captured mouse makes.
    bool m_ignoreMove;
    MOUSESTATE m_mouse;
    char m_14b4[8];
    // Each button held, for how long since it last repeated, and whether it
    // has been held long enough to repeat.
    bool m_leftDown;
    float m_leftTime;
    bool m_leftHeld;
    bool m_rightDown;
    float m_rightTime;
    bool m_rightHeld;
    bool m_middleDown;
    float m_middleTime;
    bool m_middleHeld;
    bool m_mouseCapture;
    int m_14d8;
    int m_numJoys;
    JOY *m_joys;
};

extern INPUT *g_pInput;

// Makes the input system; the input plugin points it at its own.
extern INPUT *(*g_pfnCreateInput)();

#endif
