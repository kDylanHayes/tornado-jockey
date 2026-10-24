// The base of the game at 0x6d09d4, which the game itself provides: the
// "Load" and "ExportStrings" commands, a map load that first closes the
// console, and carrying on from a pause. Placeholder names apart from the
// class name and the names its commands give.
#ifndef TONGAS2_GAME_H
#define TONGAS2_GAME_H

#include "Tongas2/CMDBASE.h"
#include "Tongas2/TOKENS.h"

extern char g_classNames[][30];

class GAME : public CMDBASE
{
public:
    GAME();
    virtual ~GAME();

    static void RegisterCommands();

    virtual int GetClassId() { return s_classId; }
    virtual int GetParentClassId() { return g_classParents[s_classId]; }
    virtual const char *GetClassName() { return g_classNames[s_classId]; }
    virtual SRESULT Execute(CMDENTRY *command, TOKENS **args)
    {
        return (this->*(SRESULT (GAME::*)(TOKENS **))command->func)(args);
    }
    // Defined elsewhere.
    virtual int Virtual08();
    virtual void *Virtual09(const char *name);
    // Runs once a frame.
    virtual void Update() = 0;
    // The Load command: closes the console and loads the map with the name.
    virtual SRESULT Load(CMDCALL *call);
    virtual void LoadMap(const char *name) = 0;
    virtual void Virtual22();
    virtual bool Virtual23() { return !Virtual26(); }
    // Whether a game is being played. Placeholder name.
    virtual bool IsPlaying() { return Virtual25(); }
    virtual bool Virtual25() = 0;
    virtual bool Virtual26() = 0;
    // Passes on to the copy protection.
    virtual void Virtual27();
    virtual void Virtual28();
    virtual void Virtual29() = 0;
    virtual void Virtual30() = 0;
    virtual void Virtual31() = 0;
    virtual void Virtual32() = 0;
    // Carries on from a pause.
    virtual void Resume();
    // The game was paused. Placeholder name.
    virtual void OnPaused() = 0;
    virtual void Virtual35() = 0;
    // Removes the actors.
    virtual void ClearActors();
    // The ExportStrings command. Defined elsewhere.
    virtual SRESULT ExportStrings(CMDCALL *call);

    static int s_classId;
    static CMDENTRY s_commands[];
    static PROPENTRY s_properties[];
};

extern GAME *g_pGame;

// Makes the game; the game points it at its own.
extern GAME *(*g_pfnCreateGame)();

#endif
