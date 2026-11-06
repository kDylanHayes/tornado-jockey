// The Direct3D 8 application framework, 0x00506190.
//
// Discovery scaffold. This is the base application class of the DX8 sample
// framework the plugin is built on: the window and device setup, the
// fullscreen and resolution switches, the render loop and the standard
// "Could not initialize Direct3D" error boxes. The class name is descriptive;
// the standard error strings name the methods. Base classes and member layout
// are deliberately left out so the scaffold does not pull the engine's inline
// virtuals into the object.
#ifndef TONGAS2_D3DAPP_H
#define TONGAS2_D3DAPP_H

struct HINSTANCE__;

class D3DAPP
{
public:
    D3DAPP();
    int Run();
    int Create(HINSTANCE__ *instance);
    int Initialize3DEnvironment();
    void Cleanup3DEnvironment();

private:
    char m_000[0x220];
};

#endif
