// The ODE actor object, 0x0054ab90.
//
// Discovery scaffold. RegisterClassName at 0x0054ab90 names ODEACTOR with
// parent ACTOR; the object's methods run through 0x0054d340. 0x0054d350
// starts ODERAGDOLL. Bodies are empty until the unit is matched.
#ifndef TONGAS2_ODEACTOR_H
#define TONGAS2_ODEACTOR_H

#include "Tongas2/CMDBASE.h"

class ODEACTORMASTER
{
public:
    static void RegisterCommands();

    static CMDENTRY s_commands[];
};

class ODEACTOR
{
public:
    void SetFlag(const char *flag);
};

#endif
