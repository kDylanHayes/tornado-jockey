// The ODE geometry resources, 0x00548e40.
//
// Discovery scaffold. RegisterClassName at 0x00548e40 names ODEGEOMMASTER
// with parent RESOURCEMASTER; the manager factory at 0x00549270 creates
// "ODEGEOM.txt". 0x005493e0 is ODEGEOMRESOURCE::ShowHelpers and 0x00549960 is
// DYNAMICARRAY::Insert. 0x0054ab90 starts ODEACTOR.
#ifndef TONGAS2_ODEGEOM_H
#define TONGAS2_ODEGEOM_H

struct CMDENTRY;

class ODEGEOMMASTER
{
public:
    static void RegisterCommands();
    static CMDENTRY s_commands[];
};

class DYNAMICARRAY
{
public:
    void Insert(void *item);
};

void *CreateODEGEOM();

#endif
