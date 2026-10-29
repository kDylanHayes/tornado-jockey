#include "Tongas2/KERNELCOMMANDS.h"
#include "Tongas2/ACTOR.h"
#include "Tongas2/APP.h"
#include "Tongas2/CAMERA.h"
#include "Tongas2/CMDMGR.h"
#include "Tongas2/DRM.h"
#include "Tongas2/GAME.h"
#include "Tongas2/GUI.h"
#include "Tongas2/LOG.h"
#include "Tongas2/RESOURCES.h"
#include "Tongas2/SCENE.h"
#include "Tongas2/TIMER.h"
#include "Tongas2/TOKENS.h"
#include "Tongas2/VALUEMANAGER.h"

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define CMD_FUNC(CLASS, method) (CMDFUNC)(SRESULT (CLASS::*)(TOKENS **))(SRESULT (CLASS::*)(CMDCALL *))&CLASS::method

static CMDBASE *CreateKernelCommands();

CMDBASE *(*g_pfnCreateScriptRoot)() = CreateKernelCommands;

CMDENTRY KERNELCOMMANDS::s_commands[] = {
    { "create", 0, 2, 2, CMD_FUNC(KERNELCOMMANDS, Create) },
    { "createAttached", 0, 2, 2, CMD_FUNC(KERNELCOMMANDS, CreateAttached) },
    { "spawnpos", 0, 1, 1, CMD_FUNC(KERNELCOMMANDS, SpawnPos) },
    { "delay", 0, 2, 2, CMD_FUNC(KERNELCOMMANDS, Delay) },
    { "delayFrames", 0, 2, 2, CMD_FUNC(KERNELCOMMANDS, DelayFrames) },
    { "randomDelay", 0, 3, 3, CMD_FUNC(KERNELCOMMANDS, RandomDelay) },
    { "if", 0, 2, 4096, CMD_FUNC(KERNELCOMMANDS, If) },
    { "while", 0, 2, 2, CMD_FUNC(KERNELCOMMANDS, While) },
    { "dumplayer", 0, 0, 1, CMD_FUNC(KERNELCOMMANDS, DumpLayer) },
    { "loadScene", 0, 1, 1, CMD_FUNC(KERNELCOMMANDS, LoadScene) },
    { "quit", 0, 0, 0, CMD_FUNC(KERNELCOMMANDS, Quit) },
    { "startGame", 0, 0, 1, CMD_FUNC(KERNELCOMMANDS, StartGame) },
    { "restartLevel", 0, 0, 0, CMD_FUNC(KERNELCOMMANDS, RestartLevel) },
    { "continueGame", 0, 0, 0, CMD_FUNC(KERNELCOMMANDS, ContinueGame) },
    { "quitGame", 0, 0, 0, CMD_FUNC(KERNELCOMMANDS, QuitGame) },
    { "msgSpawned", 0, 2, 2, CMD_FUNC(KERNELCOMMANDS, MsgSpawned) },
    { "orthoProject", 0, 1, 1, CMD_FUNC(KERNELCOMMANDS, OrthoProject) },
    { "createFX", 0, 3, 3, CMD_FUNC(KERNELCOMMANDS, CreateFX) },
    { "startLevel", 0, 0, 0, CMD_FUNC(KERNELCOMMANDS, StartLevel) },
    { "TimeFactor", 0, 1, 1, CMD_FUNC(KERNELCOMMANDS, TimeFactor) },
    { "RepeatSteps", 0, 1, 1, CMD_FUNC(KERNELCOMMANDS, RepeatSteps) },
    { "eq", 0, 2, 2, CMD_FUNC(KERNELCOMMANDS, Eq) },
    { "ne", 0, 2, 2, CMD_FUNC(KERNELCOMMANDS, Ne) },
    { "lt", 0, 2, 2, CMD_FUNC(KERNELCOMMANDS, Lt) },
    { "le", 0, 2, 2, CMD_FUNC(KERNELCOMMANDS, Le) },
    { "gt", 0, 2, 2, CMD_FUNC(KERNELCOMMANDS, Gt) },
    { "ge", 0, 2, 2, CMD_FUNC(KERNELCOMMANDS, Ge) },
    { "not", 0, 1, 1, CMD_FUNC(KERNELCOMMANDS, Not) },
    { "and", 0, 2, 4096, CMD_FUNC(KERNELCOMMANDS, And) },
    { "add", 0, 2, 4096, CMD_FUNC(KERNELCOMMANDS, Add) },
    { "mult", 0, 2, 4096, CMD_FUNC(KERNELCOMMANDS, Mult) },
    { "or", 0, 2, 4096, CMD_FUNC(KERNELCOMMANDS, Or) },
    { "Log", 0, 1, 1, CMD_FUNC(KERNELCOMMANDS, Log) },
    { "LogVal", 0, 1, 1, CMD_FUNC(KERNELCOMMANDS, LogVal) },
    { "InEditor", 0, 0, 0, CMD_FUNC(KERNELCOMMANDS, InEditor) },
    { "CleanDelayedIngameCommands", 0, 0, 0, CMD_FUNC(KERNELCOMMANDS, CleanDelayedIngameCommands) },
    { "GetFixedStrMem", 0, 0, 0, CMD_FUNC(KERNELCOMMANDS, GetFixedStrMem) },
    { 0, 0, 0, 0, 0 },
};

PROPENTRY KERNELCOMMANDS::s_properties[] = {
    { 0, 0, 15, 0, 0, false },
};

// Whether two vectors are the same.
bool operator==(const V2 &a, const V2 &b)
{
    return a.x == b.x && a.y == b.y;
}

// Defined in other units.
bool operator==(const V3 &a, const V3 &b);
bool operator==(const RGB &a, const RGB &b);

// Whether a's value is above b's; only characters, integers, strings and
// floats compare.
bool operator>(const SRESULT &a, const SRESULT &b)
{
    switch (a.GetType()) {
    case SRT_CHAR:
        if (b.GetType() == SRT_CHAR && a.GetChar(0) > b.GetChar(0))
            return true;
        break;
    case SRT_INT:
        if (b.GetType() == SRT_CHAR && a.GetInt(0) > b.GetChar(0))
            return true;
        if (b.GetType() == SRT_INT && a.GetInt(0) > b.GetInt(0))
            return true;
        if (b.GetType() == SRT_FLOAT && a.GetInt(0) > (int)b.GetFloat(0))
            return true;
        return false;
    case SRT_STRING:
        if (b.GetType() == SRT_STRING && _stricmp(a.GetString(""), b.GetString("")) > 0)
            return true;
        break;
    case SRT_UNICODESTRING:
        if (b.GetType() == SRT_UNICODESTRING && _wcsicmp(a.GetUnicodeString(L""), b.GetUnicodeString(L"")) > 0)
            return true;
        break;
    case SRT_FLOAT:
        if (b.GetType() == SRT_CHAR && a.GetFloat(0) > (float)b.GetChar(0))
            return true;
        if (b.GetType() == SRT_INT && a.GetFloat(0) > (float)b.GetInt(0))
            return true;
        if (b.GetType() == SRT_FLOAT && a.GetFloat(0) > b.GetFloat(0))
            return true;
        break;
    }
    return false;
}

// Whether a's value is below b's.
bool operator<(const SRESULT &a, const SRESULT &b)
{
    switch (a.GetType()) {
    case SRT_CHAR:
        if (b.GetType() == SRT_CHAR && a.GetChar(0) < b.GetChar(0))
            return true;
        break;
    case SRT_INT:
        if (b.GetType() == SRT_CHAR && a.GetInt(0) < b.GetChar(0))
            return true;
        if (b.GetType() == SRT_INT && a.GetInt(0) < b.GetInt(0))
            return true;
        if (b.GetType() == SRT_FLOAT && a.GetInt(0) < (int)b.GetFloat(0))
            return true;
        return false;
    case SRT_STRING:
        if (b.GetType() == SRT_STRING && _stricmp(a.GetString(""), b.GetString("")) < 0)
            return true;
        break;
    case SRT_UNICODESTRING:
        if (b.GetType() == SRT_UNICODESTRING && _wcsicmp(a.GetUnicodeString(L""), b.GetUnicodeString(L"")) < 0)
            return true;
        break;
    case SRT_FLOAT:
        if (b.GetType() == SRT_CHAR && a.GetFloat(0) < (float)b.GetChar(0))
            return true;
        if (b.GetType() == SRT_INT && a.GetFloat(0) < (float)b.GetInt(0))
            return true;
        if (b.GetType() == SRT_FLOAT && a.GetFloat(0) < b.GetFloat(0))
            return true;
        break;
    }
    return false;
}

void KERNELCOMMANDS::RegisterCommands()
{
    g_classParents[s_classId] = g_CMDBASEClassId;
    g_classCommands[s_classId] = s_commands;
    HashCommands(s_commands);
    RegisterClassName(s_commands, "KERNELCOMMANDS", "CMDBASE");
}

void *KERNELCOMMANDS::Virtual09(const char *name)
{
    if (!_stricmp(name, "scene"))
        return g_pScene;
    if (!_stricmp(name, "resources"))
        return g_pResources;
    if (!_stricmp(name, "gui"))
        return g_pGui;
    if (!_stricmp(name, "guiscreen") && g_pGui->m_screen)
        return g_pGui->m_screen;
    if (!_stricmp(name, "game"))
        return g_pGame;
    if (!_stricmp(name, "camera"))
        return g_pCamera;
    if (!_stricmp(name, "var"))
        return g_pValueManager;
    if (!_stricmp(name, "drm"))
        return g_pDrm;
    return 0;
}

const char *KERNELCOMMANDS::Virtual58()
{
    return g_basePath;
}

// Whether the euler angles are the same.
bool operator==(const ANG &a, const ANG &b)
{
    ANG x = a;
    const V3 &ex = x.GetEuler();
    ANG y = b;
    const V3 &ey = y.GetEuler();
    return ex.x == ey.x && ex.y == ey.y && ex.z == ey.z;
}

bool SRTEXTURE::operator==(const SRTEXTURE &other) const
{
    return !strcmp(name.m_buf, other.name.m_buf) && index == other.index;
}

// Whether the values are the same; values of different types are the same
// only where one is a character, integer or float and the other converts
// to it.
bool operator==(const SRESULT &a, const SRESULT &b)
{
    switch (a.GetType()) {
    case SRT_CHAR:
        if (b.GetType() == SRT_CHAR && a.GetChar(0) == b.GetChar(0))
            return true;
        break;
    case SRT_INT:
        if (b.GetType() == SRT_CHAR && a.GetInt(0) == b.GetChar(0))
            return true;
        if (b.GetType() == SRT_INT && a.GetInt(0) == b.GetInt(0))
            return true;
        if (b.GetType() == SRT_FLOAT && a.GetInt(0) == (int)b.GetFloat(0))
            return true;
        return false;
    case SRT_STRING:
        if (b.GetType() == SRT_STRING && !_stricmp(a.GetString(""), b.GetString("")))
            return true;
        break;
    case SRT_UNICODESTRING:
        if (b.GetType() == SRT_UNICODESTRING && !_wcsicmp(a.GetUnicodeString(L""), b.GetUnicodeString(L"")))
            return true;
        break;
    case SRT_FLOAT:
        if (b.GetType() == SRT_CHAR && a.GetFloat(0) == (float)b.GetChar(0))
            return true;
        if (b.GetType() == SRT_INT && a.GetFloat(0) == (float)b.GetInt(0))
            return true;
        if (b.GetType() == SRT_FLOAT && a.GetFloat(0) == b.GetFloat(0))
            return true;
        break;
    case SRT_BOOL:
        if (b.GetType() == SRT_BOOL && a.GetBool(false) == b.GetBool(false))
            return true;
        if (b.GetType() == SRT_CHAR) {
            char c = b.GetChar(0);
            if (a.GetBool(false) == ToBool(c))
                return true;
        }
        if (b.GetType() == SRT_INT) {
            int i = b.GetInt(0);
            if (a.GetBool(false) == ToBool(i))
                return true;
        }
        if (b.GetType() == SRT_FLOAT) {
            float f = b.GetFloat(0);
            if (a.GetBool(false) == ToBool(f))
                return true;
        }
        break;
    case SRT_V3:
        if (b.GetType() == SRT_V3 && a.GetV3(V3(0.0f, 0.0f, 0.0f)) == b.GetV3(V3(0.0f, 0.0f, 0.0f)))
            return true;
        break;
    case SRT_V2:
        if (b.GetType() == SRT_V2 && a.GetV2(V2(0.0f, 0.0f)) == b.GetV2(V2(0.0f, 0.0f)))
            return true;
        break;
    case SRT_ANG:
        if (b.GetType() == SRT_ANG && a.GetANG(ANG(0.0f, 0.0f, 0.0f)) == b.GetANG(ANG(0.0f, 0.0f, 0.0f)))
            return true;
        break;
    case SRT_RGB:
        if (b.GetType() == SRT_RGB && a.GetRGB(RGB()) == b.GetRGB(RGB()))
            return true;
        break;
    case SRT_TEXTURE:
        if (b.GetType() == SRT_TEXTURE && a.GetTexture(SRTEXTURE()) == b.GetTexture(SRTEXTURE()))
            return true;
        break;
    case SRT_NOTSET:
        return b.GetType() == SRT_NOTSET;
    }
    return false;
}

static CMDBASE *CreateKernelCommands()
{
    return new KERNELCOMMANDS;
}

SRESULT KERNELCOMMANDS::SpawnPos(CMDCALL *call)
{
    if (call->caller.kind == 1 && call->caller.IsValid()) {
        ACTOR *target = (ACTOR *)g_pScene->FindActor(call->tokens->GetToken(1));
        ACTOR *caller;
        if (target && (caller = (ACTOR *)g_pScene->GetActor(call->caller)) != 0) {
            ACTOR *copy = target->Virtual86(0);
            copy->SetPosition(caller->GetPosition());
            copy->Virtual101();
            copy->m_998 = true;
            return SRESULT(true);
        }
    }
    return SRESULT(false);
}

SRESULT KERNELCOMMANDS::DumpLayer(CMDCALL *call)
{
    int layer;
    if (call->tokens->m_count == 2)
        layer = atol(call->tokens->GetToken(1));
    else
        layer = -1;
    g_pScene->DumpActors(layer);
    return SRESULT(true);
}

SRESULT KERNELCOMMANDS::LoadScene(CMDCALL *call)
{
    g_pCmdMgr->LoadMap(call->tokens->GetToken(1));
    while (!g_pCmdMgr->LoadScript())
        Sleep(50);
    return SRESULT(true);
}

SRESULT KERNELCOMMANDS::Quit(CMDCALL *call)
{
    g_pApp->m_paused = true;
    return SRESULT(true);
}

SRESULT KERNELCOMMANDS::StartGame(CMDCALL *call)
{
    if (call->tokens->m_count > 1) {
        const char *map = call->tokens->GetToken(1);
        char *dest = g_pApp->m_map;
        if (map == NULL)
            *dest = 0;
        else {
            strncpy(dest, map, sizeof(g_pApp->m_map) - 1);
            dest[sizeof(g_pApp->m_map) - 1] = 0;
        }
    }
    g_pTimer->StartGame();
    return SRESULT(true);
}

SRESULT KERNELCOMMANDS::QuitGame(CMDCALL *call)
{
    g_pTimer->QuitGame();
    return SRESULT(true);
}

SRESULT KERNELCOMMANDS::OrthoProject(CMDCALL *call)
{
    bool ortho = atol(call->tokens->GetToken(1)) != 0;
    ((CAMERA *)g_pCamera)->SetUseOrthoProjection(ortho);
    return SRESULT(true);
}

SRESULT KERNELCOMMANDS::ContinueGame(CMDCALL *call)
{
    g_pTimer->SetPaused(false);
    if (g_pGame)
        g_pGame->Virtual35();
    return SRESULT(true);
}

SRESULT KERNELCOMMANDS::StartLevel(CMDCALL *call)
{
    if (g_pGame)
        g_pGame->Virtual31();
    return SRESULT(true);
}

SRESULT KERNELCOMMANDS::RestartLevel(CMDCALL *call)
{
    if (g_pGame)
        g_pGame->Resume();
    return SRESULT(true);
}

SRESULT KERNELCOMMANDS::InEditor(CMDCALL *call)
{
    return SRESULT(false);
}

SRESULT KERNELCOMMANDS::TimeFactor(CMDCALL *call)
{
    g_pTimer->SetSpeed((float)atof(call->tokens->GetToken(1)));
    return SRESULT(true);
}

SRESULT KERNELCOMMANDS::RepeatSteps(CMDCALL *call)
{
    g_pTimer->SetSteps(atol(call->tokens->GetToken(1)));
    return SRESULT(true);
}

SRESULT KERNELCOMMANDS::CleanDelayedIngameCommands(CMDCALL *call)
{
    g_pCmdMgr->ClearFlagged();
    return SRESULT(true);
}

SRESULT KERNELCOMMANDS::GetFixedStrMem(CMDCALL *call)
{
    int size = 0;
    return SRESULT(size);
}

SRESULT KERNELCOMMANDS::CreateFX(CMDCALL *call)
{
    V3 position(0.0f, 0.0f, 0.0f);
    sscanf(call->tokens->GetToken(2), "%f %f %f", &position.x, &position.y, &position.z);
    int parent = atol(call->tokens->GetToken(3));
    FIXEDSTRING<0x200> name(call->tokens->GetToken(1));
    g_pResources->CreateFromTemplate(name, position, parent);
    return SRESULT(true);
}

SRESULT KERNELCOMMANDS::Create(CMDCALL *call)
{
    ACTOR *actor = g_pScene->CreateActor(call->tokens->GetToken(1));
    if (!actor)
        return SRESULT(false);
    actor->Run(STRING(call->tokens->GetToken(2)));
    actor->Virtual101();
    actor->m_998 = true;
    if (!g_pGame)
        actor->Virtual89(true);
    return SRESULT(true);
}

SRESULT KERNELCOMMANDS::CreateAttached(CMDCALL *call)
{
    if (call->caller.kind == 1 && call->caller.IsValid()) {
        ACTOR *actor = g_pScene->CreateActor(call->tokens->GetToken(1));
        if (actor) {
            actor->Run(STRING(call->tokens->GetToken(2)));
            actor->Virtual101();
            actor->m_998 = true;
            actor->AttachTo(call->caller);
            return SRESULT(true);
        }
    }
    return SRESULT(false);
}

SRESULT KERNELCOMMANDS::DelayFrames(CMDCALL *call)
{
    TOKENS *tokens = new TOKENS;
    int frames = atol(call->tokens->GetToken(1));
    tokens->Parse(STRING(call->tokens->GetToken(2)), 0);
    if (tokens->m_count <= 0)
        return SRESULT(false);
    if (frames <= 0) {
        CMDCONTEXT context(tokens, call->caller);
        context.Run(this);
        delete tokens;
    } else {
        CMDCONTEXT *context = new CMDCONTEXT(tokens, call->caller);
        context->m_frames = frames;
        g_pCmdMgr->Queue(context);
    }
    return SRESULT(true);
}

SRESULT KERNELCOMMANDS::RandomDelay(CMDCALL *call)
{
    TOKENS *tokens = new TOKENS;
    float time = (float)atof(call->tokens->GetToken(1));
    float most = (float)atof(call->tokens->GetToken(2));
    if (time < most)
        time += (most - time) * (float)rand() * (1.0f / RAND_MAX);
    tokens->Parse(STRING(call->tokens->GetToken(3)), 0);
    if (tokens->m_count <= 0)
        return SRESULT(false);
    if (time <= 0.0f) {
        CMDCONTEXT context(tokens, call->caller);
        context.Run(this);
        delete tokens;
    } else {
        CMDCONTEXT *context = new CMDCONTEXT(tokens, call->caller);
        context->m_time = time + g_pTimer->m_time;
        g_pCmdMgr->Queue(context);
    }
    return SRESULT(true);
}

SRESULT KERNELCOMMANDS::If(CMDCALL *call)
{
    SRESULT result;
    if (call->tokens->m_count < 5) {
        result.Assign(g_pCmdMgr->Evaluate(STRING(call->tokens->GetToken(1)), call->caller));
        if (result.GetBool(false)) {
            if (!g_pCmdMgr->Run(STRING(call->tokens->GetToken(2)), call->caller)) {
                if (g_pLog)
                    g_pLog->Error("KERNELCOMMANDS::IfControl>Error: Unable to run \"%s\" command", call->tokens->GetToken(2));
                return SRESULT(false);
            }
        } else if (call->tokens->m_count == 4) {
            if (!g_pCmdMgr->Run(STRING(call->tokens->GetToken(3)), call->caller)) {
                if (g_pLog)
                    g_pLog->Error("KERNELCOMMANDS::IfControl>Error: Unable to run else \"%s\" command", call->tokens->GetToken(3));
                return SRESULT(false);
            }
        }
    } else {
        int count = call->tokens->m_count;
        for (int i = 1; i < count; i += 3) {
            result.Assign(g_pCmdMgr->Evaluate(STRING(call->tokens->GetToken(i)), call->caller));
            if (!result.IsSet()) {
                if (g_pLog)
                    g_pLog->Error("KERNELCOMMANDS::If> Error evaluating \"%s\" expression");
                return SRESULT(false);
            }
            if (result.GetBool(false)) {
                if (i + 1 >= count) {
                    if (g_pLog)
                        g_pLog->Error("KERNELCOMMANDS::IfControl>Error: Missing 'ScriptTrue' after 'conition'");
                    return SRESULT(false);
                }
                if (!g_pCmdMgr->Run(STRING(call->tokens->GetToken(i + 1)), call->caller)) {
                    if (g_pLog)
                        g_pLog->Error("KERNELCOMMANDS::IfControl>Error: Unable to run \"%s\" command", call->tokens->GetToken(i));
                    return SRESULT(false);
                }
                return SRESULT(true);
            }
            if (i + 2 < count) {
                const char *word = call->tokens->GetToken(i + 2);
                if (!_stricmp(word, "else")) {
                    if (i + 3 >= count) {
                        if (g_pLog)
                            g_pLog->Error("KERNELCOMMANDS::IfControl>Error: Missing 'Scriptfalse' after 'else' token");
                        return SRESULT(false);
                    }
                    i += 3;
                    if (!g_pCmdMgr->Run(STRING(call->tokens->GetToken(i)), call->caller)) {
                        if (g_pLog)
                            g_pLog->Error("KERNELCOMMANDS::IfControl>Error: Unable to run else \"%s\" command", call->tokens->GetToken(3));
                        return SRESULT(false);
                    }
                    if (i + 1 < count) {
                        if (g_pLog)
                            g_pLog->Error("KERNELCOMMANDS::IfControl>Error: There are commands after 'ScriptFalse'");
                        return SRESULT(false);
                    }
                } else if (_stricmp(word, "elseif")) {
                    if (g_pLog)
                        g_pLog->Error("KERNELCOMMANDS::IfControl>Error: Unexpected expression \"%s\", there should be an 'else' or an 'elseif' token.", word);
                    return SRESULT(false);
                }
            }
        }
    }
    return SRESULT(true);
}

SRESULT KERNELCOMMANDS::While(CMDCALL *call)
{
    TOKENS tokens;
    tokens.Parse(STRING(call->tokens->GetToken(2)), 0);
    while (g_pCmdMgr->Evaluate(call->tokens->GetToken(1), call->caller).GetBool(false)) {
        if (!g_pCmdMgr->Run(STRING(tokens.GetToken(2)), call->caller)) {
            if (g_pLog)
                g_pLog->Error("KERNELCOMMANDS::WhileControl>Error: Unable to run \"%s\" command", tokens.GetToken(2));
            return SRESULT(false);
        }
    }
    return SRESULT(true);
}

SRESULT KERNELCOMMANDS::MsgSpawned(CMDCALL *call)
{
    WTACTOR *spawner = g_pScene->FindActor(call->tokens->GetToken(1));
    STRING message(call->tokens->GetToken(2));
    if (!spawner)
        return SRESULT(false);
    g_pScene->MessageSpawned(((ACTOR *)spawner)->m_handle, message);
    return SRESULT(true);
}

SRESULT KERNELCOMMANDS::Eq(CMDCALL *call)
{
    SRESULT a = g_pCmdMgr->Evaluate(call->tokens->GetToken(1), call->caller);
    SRESULT b = g_pCmdMgr->Evaluate(call->tokens->GetToken(2), call->caller);
    return SRESULT(a == b);
}

SRESULT KERNELCOMMANDS::Ne(CMDCALL *call)
{
    SRESULT a = g_pCmdMgr->Evaluate(call->tokens->GetToken(1), call->caller);
    SRESULT b = g_pCmdMgr->Evaluate(call->tokens->GetToken(2), call->caller);
    return SRESULT(!(a == b));
}

SRESULT KERNELCOMMANDS::Lt(CMDCALL *call)
{
    SRESULT a = g_pCmdMgr->Evaluate(call->tokens->GetToken(1), call->caller);
    SRESULT b = g_pCmdMgr->Evaluate(call->tokens->GetToken(2), call->caller);
    return SRESULT(a < b);
}

SRESULT KERNELCOMMANDS::Le(CMDCALL *call)
{
    SRESULT a = g_pCmdMgr->Evaluate(call->tokens->GetToken(1), call->caller);
    SRESULT b = g_pCmdMgr->Evaluate(call->tokens->GetToken(2), call->caller);
    return SRESULT(!(a > b));
}

SRESULT KERNELCOMMANDS::Gt(CMDCALL *call)
{
    SRESULT a = g_pCmdMgr->Evaluate(call->tokens->GetToken(1), call->caller);
    SRESULT b = g_pCmdMgr->Evaluate(call->tokens->GetToken(2), call->caller);
    return SRESULT(a > b);
}

SRESULT KERNELCOMMANDS::Ge(CMDCALL *call)
{
    SRESULT a = g_pCmdMgr->Evaluate(call->tokens->GetToken(1), call->caller);
    SRESULT b = g_pCmdMgr->Evaluate(call->tokens->GetToken(2), call->caller);
    return SRESULT(!(a < b));
}

SRESULT KERNELCOMMANDS::And(CMDCALL *call)
{
    for (int i = 1; i < call->tokens->m_count; i++) {
        if (!g_pCmdMgr->Evaluate(call->tokens->GetToken(i), call->caller).GetBool(false))
            return SRESULT(false);
    }
    return SRESULT(true);
}

SRESULT KERNELCOMMANDS::Or(CMDCALL *call)
{
    for (int i = 1; i < call->tokens->m_count; i++) {
        bool value = g_pCmdMgr->Evaluate(call->tokens->GetToken(i), call->caller).GetBool(false);
        if (value)
            return SRESULT(true);
    }
    return SRESULT(false);
}

SRESULT KERNELCOMMANDS::Add(CMDCALL *call)
{
    float sum = 0.0f;
    for (int i = 1; i < call->tokens->m_count; i++)
        sum += g_pCmdMgr->Evaluate(call->tokens->GetToken(i), call->caller).GetFloat(0.0f);
    return SRESULT(sum);
}

SRESULT KERNELCOMMANDS::Mult(CMDCALL *call)
{
    float product = 1.0f;
    for (int i = 1; i < call->tokens->m_count; i++)
        product *= g_pCmdMgr->Evaluate(call->tokens->GetToken(i), call->caller).GetFloat(0.0f);
    return SRESULT(product);
}

SRESULT KERNELCOMMANDS::Not(CMDCALL *call)
{
    SRESULT value = g_pCmdMgr->Evaluate(call->tokens->GetToken(1), call->caller);
    return SRESULT(!value.GetBool(false));
}

SRESULT KERNELCOMMANDS::LogVal(CMDCALL *call)
{
    SRESULT value = g_pCmdMgr->Evaluate(call->tokens->GetToken(1), call->caller);
    switch (value.GetType()) {
    case SRT_CHAR:
        if (g_pLog)
            g_pLog->Value("%c", value.GetChar(0));
        return SRESULT(true);
    case SRT_INT:
        if (g_pLog)
            g_pLog->Value("%d", value.GetInt(0));
        return SRESULT(true);
    case SRT_STRING:
        if (g_pLog)
            g_pLog->Value("%s", value.GetString(""));
        return SRESULT(true);
    case SRT_UNICODESTRING:
        if (g_pLog)
            g_pLog->Value("%s", (char *)FIXEDWSTRING<0x200>(value.GetUnicodeString(L"")).ToAnsi());
        return SRESULT(true);
    case SRT_FLOAT:
        if (g_pLog)
            g_pLog->Value("%f", value.GetFloat(0.0f));
        return SRESULT(true);
    case SRT_BOOL:
        if (g_pLog)
            g_pLog->Value("%d", value.GetBool(false));
        return SRESULT(true);
    case SRT_V3:
        if (g_pLog)
            g_pLog->Value("%f %f %f", value.GetV3(V3(0.0f, 0.0f, 0.0f)).x, value.GetV3(V3(0.0f, 0.0f, 0.0f)).y, value.GetV3(V3(0.0f, 0.0f, 0.0f)).z);
        return SRESULT(true);
    case SRT_V2:
        if (g_pLog)
            g_pLog->Value("%f %f", value.GetV2(V2(0.0f, 0.0f)).x, value.GetV2(V2(0.0f, 0.0f)).y);
        return SRESULT(true);
    case SRT_ANG:
        if (g_pLog)
            g_pLog->Value("%f %f %f", value.GetANG(ANG(0.0f, 0.0f, 0.0f)).GetEulerX(), value.GetANG(ANG(0.0f, 0.0f, 0.0f)).GetEulerY(), value.GetANG(ANG(0.0f, 0.0f, 0.0f)).GetEulerZ());
        return SRESULT(true);
    case SRT_RGB:
        if (g_pLog)
            g_pLog->Value("%d %d %d", value.GetRGB(RGB()).r, value.GetRGB(RGB()).g, value.GetRGB(RGB()).b);
        return SRESULT(true);
    case SRT_TEXTURE:
        if (g_pLog)
            g_pLog->Value("%s %d", value.GetTexture(SRTEXTURE()).name.m_buf, value.GetTexture(SRTEXTURE()).index);
        return SRESULT(true);
    }
    return SRESULT(false);
}

SRESULT KERNELCOMMANDS::Delay(CMDCALL *call)
{
    TOKENS *tokens = new TOKENS;
    float time = (float)atof(call->tokens->GetToken(1));
    tokens->Parse(STRING(call->tokens->GetToken(2)), 0);
    if (tokens->m_count <= 0)
        return SRESULT(false);
    if (time <= 0.0f) {
        CMDCONTEXT context(tokens, call->caller);
        context.Run(this);
        delete tokens;
    } else {
        CMDCONTEXT *context = new CMDCONTEXT(tokens, call->caller);
        context->m_time = time + g_pTimer->m_time;
        g_pCmdMgr->Queue(context);
    }
    return SRESULT(true);
}
