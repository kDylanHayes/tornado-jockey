// The renderer plugin's DirectInput layer: INPUTDEVICE wraps one
// IDirectInput8 device and DXINPUT is the input system's plugin copy.
//
// Discovery scaffold. The class names come from the error strings at
// 0x004ee0b0-0x004ee780. The base classes and member layouts are
// deliberately left out for now: keeping the scaffold free of the engine's
// inline virtuals means the object defines only the functions whose exact
// target sizes are recorded in config/symbols.txt, so a later delink cannot
// cut a target function short.
#ifndef TONGAS2_DXINPUT_H
#define TONGAS2_DXINPUT_H

#include <windows.h>

class INPUTDEVICE
{
public:
    ~INPUTDEVICE();

    // Opens DirectInput 8 and the devices.
    void Init(HWND hwnd, HINSTANCE instance);
    void CreateKeyboard();
    void CreateGamepad();
    void CreateMouse();
};

class DXINPUT
{
public:
    ~DXINPUT();

    // Reads the mouse and feeds the listeners.
    void StepMouse();
};

#endif
