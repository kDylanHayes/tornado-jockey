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
#define TONGAS2_DX8APP_H

class DX8APP
{
public:
    DX8APP();
    virtual ~DX8APP();
    void Init();

private:
    char m_000[0x2c4];
};

DX8APP *CreateDX8APP();
void RegisterDX8Plugin();

#endif
