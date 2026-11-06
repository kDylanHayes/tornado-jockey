// The renderer plugin's lights: LIGHTDX8MASTER and LIGHTDX8RESOURCE turn the
// engine's LIGHT into the device's lights, and LIGHTDX8MANAGER, from
// LIGHT.txt, keeps them.
//
// Discovery scaffold. The class names come from the log string at
// 0x004f10b0, the string at 0x004f0f40 and the LIGHTMASTER constructor calls.
// The base classes and member layouts are deliberately left out for now:
// keeping the scaffold free of the engine's inline virtuals means the object
// defines only the functions whose exact target sizes are recorded in
// config/symbols.txt, so a later delink cannot cut a target function short.
#ifndef TONGAS2_LIGHTDX8_H
#define TONGAS2_LIGHTDX8_H

#include "Tongas2/MATHS.h"
#include "Tongas2/SAFEINDEX.h"

class LIGHTMASTER;
class RESOURCEMANAGER;

class LIGHTDX8RESOURCE
{
public:
    LIGHTDX8RESOURCE(LIGHTMASTER *master, const RESHANDLE &handle);

    void Start();
    // Rebuilds the light's geometry; force overrides the cached state.
    void Update(bool force);
    void Stop();
};

class LIGHTDX8MANAGER
{
public:
    LIGHTDX8MANAGER();
    ~LIGHTDX8MANAGER();
};

// The packed 0x00RRGGBB color, and the float vector it comes from.
int PackColor(const V3 &color);
void UnpackColor(V3 *out, const int *color);

RESOURCEMANAGER *CreateLIGHTDX8();

#endif
