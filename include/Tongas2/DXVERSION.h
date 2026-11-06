// The DirectX version detector, 0x00538cf0.
//
// Discovery scaffold. The large function at 0x00538cf0 loads ddraw.dll,
// dinput.dll and dpnet.dll and works out the installed DirectX version;
// 0x00539660 formats the version string ("%d.%d%c" / "%d.%d"). 0x00539950
// starts the RENDERTEXTURE registration instead. The class/function names are
// descriptive: only the DLL names and format strings identify the object.
#ifndef TONGAS2_DXVERSION_H
#define TONGAS2_DXVERSION_H

int GetDirectXVersion();
const char *FormatVersion(int major, int minor, char suffix);

#endif
