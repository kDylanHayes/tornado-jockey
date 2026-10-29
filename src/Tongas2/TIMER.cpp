#include <string.h>

#include <windows.h>

#include "Tongas2/APP.h"
#include "Tongas2/APPOPTIONS.h"
#include "Tongas2/APPWINDOW.h"
#include "Tongas2/CAMERA.h"
#include "Tongas2/CMDMGR.h"
#include "Tongas2/CONSOLE.h"
#include "Tongas2/GUI.h"
#include "Tongas2/GUICONTROL.h"
#include "Tongas2/INPUT.h"
#include "Tongas2/LOG.h"
#include "Tongas2/RESOURCES.h"
#include "Tongas2/KERNELCOMMANDS.h"
#include "Tongas2/SYSMONITOR.h"
#include "Tongas2/TIMER.h"
#include "Tongas2/VALUEMANAGER.h"

#define SAFE_DELETE(p) \
    if (p) { \
        delete p; \
        p = 0; \
    }

TIMER *g_pTimer;
TIMERHOOKS g_startHooks;
TIMERHOOKS g_shutdownHooks;

TIMER::~TIMER()
{
    SAFE_DELETE(g_pGame)
    SAFE_DELETE(g_pConsole)
    SAFE_DELETE(g_pGui)
    SAFE_DELETE(g_pScene)
    SAFE_DELETE(g_pValueManager)
    SAFE_DELETE(g_pScriptRoot)
    SAFE_DELETE(g_pCmdMgr)
    SAFE_DELETE(g_pInput)
    SAFE_DELETE(g_pResources)
    SAFE_DELETE(g_pCamera)
    SAFE_DELETE(g_pGuiBehaviors)
    for (TIMERHOOK *hook = g_shutdownHooks.head; hook; hook = hook->next)
        hook->func();
}

void TIMER::Update(float dt)
{
    if (!m_started)
        return;
    HWND window = g_pDisplay->GetWindow();
    if (GetForegroundWindow() != window && g_pGame && g_pGame->Virtual25())
        g_pGui->Pause(false);
    if (m_maxDt > 0.0f && dt > m_maxDt)
        dt = m_maxDt;
    dt *= m_speed;
    m_soundDt = dt;
    int steps = m_steps;
    for (int i = 0; i < steps; i++) {
        m_soundTime += m_soundDt;
        if (m_paused)
            dt = 0.0f;
        else
            m_frames++;
        m_dt = dt * m_scale;
        m_time += m_dt;
        g_pInput->Update();
        g_pCmdMgr->Update();
        g_pConsole->Update();
        if (g_pGame)
            g_pGame->Update();
        ((ACTOR *)g_pCamera)->Step();
        g_pScene->Update();
        g_pGui->Update();
        g_pResources->Update();
        m_wasPaused = m_paused;
    }
}

void TIMER::SetPaused(bool paused)
{
    m_paused = paused;
}

void TIMER::StartGame()
{
    if (g_pGame) {
        if (g_pLog)
            g_pLog->Error("ERROR!!! trying to start a game when there is one active!!");
        return;
    }
    g_pApp->m_monitor->Update();
    g_pGame = g_pfnCreateGame();
    g_pApp->m_monitor->Update();
}

void TIMER::QuitGame()
{
    if (!g_pGame) {
        if (g_pLog)
            g_pLog->Error("ERROR!!! trying to quit from no game!!");
        return;
    }
    g_pApp->m_monitor->Update();
    SAFE_DELETE(g_pGame)
    g_pApp->m_monitor->Update();
    m_paused = false;
    g_pInput->SetMouseCapture(false);
    ((CAMERA *)g_pCamera)->ResetMotion(V3(0.0f, 0.0f, 0.0f));
}

void TIMER::SetSpeed(float speed)
{
    m_speed = speed;
}

void TIMER::SetSteps(int steps)
{
    m_steps = steps;
}

void TIMER::SetMaxDt(float dt)
{
    m_maxDt = dt;
}

TIMER *CreateTimer()
{
    return new TIMER;
}

TIMER *(*g_pfnCreateTimer)() = CreateTimer;

void TIMER::Start()
{
    for (TIMERHOOK *hook = g_startHooks.head; hook; hook = hook->next)
        hook->func();
    g_pGuiBehaviors = new GUIBEHAVIORS;
    g_pCamera = (WTACTOR *)g_pfnCreateCamera();
    g_pCamera->Virtual146();
    g_pResources = (RESOURCES *)RESOURCES::s_pfnCreate();
    RESOURCES *resources = g_pResources;
    resources->InitManagers();
    resources->ResetGroups();
    g_pInput = g_pfnCreateInput();
    g_pInput->Init();
    g_pCmdMgr = g_pfnCreateCmdMgr();
    g_pCmdMgr->RegisterClasses();
    g_pScriptRoot = g_pfnCreateScriptRoot();
    g_pValueManager = g_pfnCreateValueManager();
    g_pGui = g_pfnCreateGui();
    g_pScene = g_pfnCreateScene();
    g_pScene->Init();
    g_pConsole = g_pfnCreateConsole();
    g_pConsole->Enable();
    g_pApp->m_monitor->Update();
    while (!g_pResources->Load())
        ;
    g_pDisplay->Virtual11();
    g_pApp->m_pOptions->Apply();
    g_pResources->SetLevel(1);
    g_pApp->m_monitor->Update();
    if ((int)strlen(g_pApp->m_map) > 0 || g_pApp->m_level >= 0)
        StartGame();
    else
        g_pGui->SetScreen("Init");
    g_pApp->m_monitor->Update();
    m_started = true;
}
