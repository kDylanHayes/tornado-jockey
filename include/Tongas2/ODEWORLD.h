// The ODE world object, 0x00548420.
//
// Discovery scaffold. RegisterClassName at 0x00548420 names ODEWORLD with
// parent ACTOR; the object's methods run through 0x00548de0. 0x00548e40
// starts ODEGEOM. Bodies are empty until the unit is matched.
#ifndef TONGAS2_ODEWORLD_H
#define TONGAS2_ODEWORLD_H

class ODEWORLDMASTER
{
public:
    static void RegisterCommands();
};

#endif
