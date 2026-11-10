// The ODE ragdoll resources, 0x0054d350.
//
// Discovery scaffold. RegisterClassName at 0x0054d350 names
// ODERAGDOLLMASTER with parent RESOURCEMASTER; the manager factory at
// 0x0054d550 creates "ODERAGDOLL.txt". 0x0054ded0 is
// ODERAGDOLLRESOURCE::_BuildDoll. 0x0054eec0 starts ODEACTORTEMPLATE.
#ifndef TONGAS2_ODERAGDOLL_H
#define TONGAS2_ODERAGDOLL_H

#include "Tongas2/CMDBASE.h"
#include "Tongas2/RESOURCES.h"

class ODERAGDOLLMASTER : public RESOURCEMANAGER
{
public:
    ODERAGDOLLMASTER()
    {
        SafeSprintf(m_fileName, "ODERAGDOLL.txt");
        m_8 = false;
        m_18 = true;
    }

    static void RegisterCommands();

    static CMDENTRY s_commands[];
};

class ODERAGDOLLRESOURCE
{
public:
    void _BuildDoll();
};

void *CreateODERAGDOLL();

#endif
