// The script commands that belong to no object: making actors and effects,
// running commands later or on a condition, starting and quitting the game,
// comparing and combining values, and logging. It is the script root
// (g_pScriptRoot), the object script commands run on first. Placeholder
// names apart from the class name and the commands.
#ifndef TONGAS2_KERNELCOMMANDS_H
#define TONGAS2_KERNELCOMMANDS_H

#include "Tongas2/CMDBASE.h"

extern char g_classNames[][30];

class KERNELCOMMANDS : public CMDBASE
{
public:
    KERNELCOMMANDS() {}
    virtual ~KERNELCOMMANDS() {}

    static void RegisterCommands();

    virtual int GetClassId() { return s_classId; }
    virtual int GetParentClassId() { return g_classParents[s_classId]; }
    virtual const char *GetClassName() { return g_classNames[s_classId]; }
    virtual SRESULT Execute(CMDENTRY *command, TOKENS **args)
    {
        return (this->*(SRESULT (KERNELCOMMANDS::*)(TOKENS **))command->func)(args);
    }
    virtual int Virtual08() { return 0; }
    // The object a script names: the scene, the resources, the GUI and its
    // screen, the game, the camera, the variables or the copy protection.
    virtual void *Virtual09(const char *name);
    // A disc or other medium was put in the drive with the letter, or taken
    // out.
    virtual void OnMediaArrived(char drive) {}
    virtual void OnMediaRemoved(char drive) {}
    // The create command: makes an actor of a type and runs a command on it.
    virtual SRESULT Create(CMDCALL *call);
    // The createAttached command: the same, attached to the caller.
    virtual SRESULT CreateAttached(CMDCALL *call);
    // The spawnpos command: spawns a copy of an actor where the caller is.
    virtual SRESULT SpawnPos(CMDCALL *call);
    // The delay, delayFrames and randomDelay commands: run a command after
    // a time, a number of frames or a random time between two.
    virtual SRESULT Delay(CMDCALL *call);
    virtual SRESULT DelayFrames(CMDCALL *call);
    virtual SRESULT RandomDelay(CMDCALL *call);
    // The if command: "if condition command [else command]", or a chain of
    // "condition command elseif ... else command".
    virtual SRESULT If(CMDCALL *call);
    // The while command: runs a command while a condition holds.
    virtual SRESULT While(CMDCALL *call);
    virtual SRESULT DumpLayer(CMDCALL *call);
    virtual SRESULT LoadScene(CMDCALL *call);
    virtual SRESULT Quit(CMDCALL *call);
    // Carries on from the pause.
    virtual SRESULT ContinueGame(CMDCALL *call);
    virtual SRESULT StartGame(CMDCALL *call);
    virtual SRESULT RestartLevel(CMDCALL *call);
    virtual SRESULT QuitGame(CMDCALL *call);
    virtual SRESULT MsgSpawned(CMDCALL *call);
    virtual SRESULT StartLevel(CMDCALL *call);
    virtual SRESULT OrthoProject(CMDCALL *call);
    virtual SRESULT CreateFX(CMDCALL *call);
    virtual SRESULT InEditor(CMDCALL *call);
    virtual SRESULT TimeFactor(CMDCALL *call);
    virtual SRESULT RepeatSteps(CMDCALL *call);
    // Compare the values of two expressions.
    virtual SRESULT Eq(CMDCALL *call);
    virtual SRESULT Ne(CMDCALL *call);
    virtual SRESULT Lt(CMDCALL *call);
    virtual SRESULT Le(CMDCALL *call);
    virtual SRESULT Gt(CMDCALL *call);
    virtual SRESULT Ge(CMDCALL *call);
    virtual SRESULT And(CMDCALL *call);
    virtual SRESULT Or(CMDCALL *call);
    virtual SRESULT Not(CMDCALL *call);
    virtual SRESULT Add(CMDCALL *call);
    virtual SRESULT Mult(CMDCALL *call);
    virtual SRESULT Log(CMDCALL *call) { return SRESULT(true); }
    virtual SRESULT LogVal(CMDCALL *call);
    virtual SRESULT CleanDelayedIngameCommands(CMDCALL *call);
    virtual SRESULT GetFixedStrMem(CMDCALL *call);
    virtual const char *Virtual58();

    static int s_classId;
    static CMDENTRY s_commands[];
    static PROPENTRY s_properties[];
};

// Makes the script root; the game points it at its own.
extern CMDBASE *(*g_pfnCreateScriptRoot)();

#endif
