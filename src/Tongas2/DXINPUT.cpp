// Tongas2/DXINPUT -- the renderer plugin's DirectInput device layer.
//
// Discovery scaffold: the methods whose names the error strings give. The
// bodies are empty until the unit is matched; every function here has its
// exact target size in config/symbols.txt so a later delink cannot cut the
// target short.
#include "Tongas2/DXINPUT.h"

INPUTDEVICE::~INPUTDEVICE()
{
}

void INPUTDEVICE::Init(HWND hwnd, HINSTANCE instance)
{
    (void)hwnd;
    (void)instance;
}

void INPUTDEVICE::CreateKeyboard()
{
}

void INPUTDEVICE::CreateGamepad()
{
}

void INPUTDEVICE::CreateMouse()
{
}

DXINPUT::~DXINPUT()
{
}

void DXINPUT::StepMouse()
{
}
