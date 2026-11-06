// A placeholder unit for the settings/config object at 0x004fa5b0.
//
// Discovery scaffold. The first function is the object's constructor (it
// checks the global settings enumeration and reads config.dat); the large
// function at 0x004fa830 parses config.dat, JOYAXISMAP and JOYBUTTONMAP.
// The class name is descriptive: no RegisterClassName string names it.
// Method names other than Load are descriptive too. Bodies are empty until
// the unit is matched; each function's exact target size is in
// config/symbols.txt so a later delink cannot cut the target short.
#ifndef TONGAS2_SETTINGS_H
#define TONGAS2_SETTINGS_H

class SETTINGS
{
public:
    SETTINGS();
    void PopFront();
    void ClearAll();
    void Destroy();
    void Load(const char *name);
};

#endif
