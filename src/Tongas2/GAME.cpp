#include "Tongas2/CONSOLE.h"
#include "Tongas2/DRM.h"
#include "Tongas2/GAME.h"
#include "Tongas2/LOG.h"
#include "Tongas2/RESOURCE.h"
#include "Tongas2/TIMER.h"

#define CMD_FUNC(CLASS, method) (CMDFUNC)(SRESULT (CLASS::*)(TOKENS **))(SRESULT (CLASS::*)(CMDCALL *))&CLASS::method

GAME *CreateGame()
{
    if (g_pLog)
        g_pLog->Error("No default plugin for GAME");
    return 0;
}

GAME *(*g_pfnCreateGame)() = CreateGame;

GAME *g_pGame;

CMDENTRY GAME::s_commands[] = {
    { "Load", 0, 1, 1, CMD_FUNC(GAME, Load) },
    { "ExportStrings", 0, 0, 0, CMD_FUNC(GAME, ExportStrings) },
    { 0, 0, 0, 0, 0 },
};

PROPENTRY GAME::s_properties[] = {
    { 0, 0, 15, 0, 0, false },
};

void GAME::RegisterCommands()
{
    g_classParents[s_classId] = g_CMDBASEClassId;
    g_classCommands[s_classId] = s_commands;
    HashCommands(s_commands);
    RegisterClassName(s_commands, "GAME", "CMDBASE");
}

void GAME::Virtual27()
{
    g_pDrm->Virtual22();
}

void GAME::ClearActors()
{
    g_pActors->RemoveAll();
}

void GAME::Resume()
{
    g_pTimer->SetPaused(false);
    ClearActors();
    Virtual28();
}

GAME::GAME()
{
    SetName("game");
}

GAME::~GAME()
{
    g_pActors->RemoveAll();
}

SRESULT GAME::Load(CMDCALL *call)
{
    g_pConsole->Show(false);
    LoadMap(call->tokens->GetToken(1));
    return SRESULT(true);
}
