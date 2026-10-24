// The slots the script root (g_pScriptRoot) has after CMDBASE's. Defined
// elsewhere; placeholder names.
#ifndef TONGAS2_SCRIPTROOT_H
#define TONGAS2_SCRIPTROOT_H

#include "Tongas2/CMDBASE.h"

class SCRIPTROOT : public CMDBASE
{
public:
    // A disc or other medium was put in the drive with the letter, or taken
    // out.
    virtual void OnMediaArrived(char drive);
    virtual void OnMediaRemoved(char drive);
    virtual void Virtual21();
    virtual void Virtual22();
    virtual void Virtual23();
    virtual void Virtual24();
    virtual void Virtual25();
    virtual void Virtual26();
    virtual void Virtual27();
    virtual void Virtual28();
    virtual void Virtual29();
    virtual void Virtual30();
    virtual void Virtual31();
    // A script command the GUI runs to carry on from the pause.
    virtual SRESULT Unpause(CMDCALL *call);
};

// Makes the script root; the game points it at its own.
extern CMDBASE *(*g_pfnCreateScriptRoot)();

#endif
