// The plugin's concrete Direct3D application object, 0x00508a70.
//
// Discovery scaffold. The constructor at 0x00508a70 chains to the D3DAPP
// base constructor, sets the plugin's primary and secondary vtables
// (0x00678e68 and 0x00678e18), and creates the hardware and settings objects.
// 0x005098a0 is its factory, 0x00509900 its destructor and 0x005099f0 the
// initialization that builds HARDWARE (0x00504f90) and SETTINGS (0x004fa5b0).
// 0x00509d30 is the static registration that installs the renderer plugin's
// resource-manager factories. The template instantiations that follow it run
// through 0x0050b3df.
#ifndef TONGAS2_DX8APP_H

#include <string>
#include "Tongas2/HARDWARE.h"
#include "Tongas2/SETTINGS.h"
#define TONGAS2_DX8APP_H

// The primary base (0x44 bytes) and the secondary base at +0x44. Their
// destructors are the functions at 0x4e62d0 and 0x507220.
class DX8APPBASE1
{
public:
    virtual ~DX8APPBASE1();

    char pad_04[0x40];
};

class GD507220
{
public:
    virtual ~GD507220();
};

class DX8APPSTRING
{
public:
    ~DX8APPSTRING();
};

class DX8APP : public DX8APPBASE1, public GD507220
{
public:
    DX8APP();
    virtual ~DX8APP();
    void Init();

private:
    char pad_48[0x2b8 - 0x48];
    DX8APPSTRING *m_2b8;
    HARDWARE *m_2bc;
    SETTINGS *m_2c0;
};

DX8APP *CreateDX8APP();
void RegisterDX8Plugin();

#endif
