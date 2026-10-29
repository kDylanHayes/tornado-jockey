#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>

#include "Tongas2/APP.h"
#include "Tongas2/APPOPTIONS.h"
#include "Tongas2/APPWINDOW.h"
#include "Tongas2/DRM.h"
#include "Tongas2/INPUT.h"
#include "Tongas2/LOG.h"
#include "Tongas2/RESOURCE.h"
#include "Tongas2/SCREENSHOT.h"
#include "Tongas2/SOUND.h"
#include "Tongas2/SYSMONITOR.h"
#include "Tongas2/TIMER.h"

#define SAFE_DELETE(p) \
    if (p) { \
        delete p; \
        p = 0; \
    }

APP *CreateApp();

APP *(*g_pfnCreateApp)() = CreateApp;

char g_basePath[MAX_PATH];
APP *g_pApp;

void APP::GetScreenSize()
{
    m_screenWidth = GetSystemMetrics(SM_CXSCREEN);
    m_screenHeight = GetSystemMetrics(SM_CYSCREEN);
}

void APP::Frame(float dt)
{
    if (!g_pTimer)
        return;
    if (m_paused)
        g_pDisplay->Virtual03();
    else {
        g_pDisplay->Virtual26();
        g_pDrm->Update();
        g_pTimer->Update(dt);
    }
}

void APP::Init(HINSTANCE instance)
{
    m_monitor->Update();
    GetModuleFileNameA(0, g_basePath, MAX_PATH);
    int length = strlen(g_basePath);
    if (!length)
        g_basePath[0] = 0;
    else {
        char *slash = strrchr(g_basePath, '\\');
        if (!slash)
            slash = strrchr(g_basePath, '/');
        if (slash)
            *slash = 0;
        strcat(g_basePath, "/");
    }
    SetCurrentDirectoryA(g_basePath);
    m_pOptions->Load();
    g_pDrm = g_pfnCreateDrm();
    g_pDrm->m_instance = instance;
    g_pDrm->Init();
    g_pDisplay = g_pfnCreateAppWindow();
    g_pDisplay->m_instance = instance;
    g_pDisplay->Create();
    g_pTimer = g_pfnCreateTimer();
    GetScreenSize();
    m_monitor->Update();
}

void APP::Shutdown()
{
    if (m_running) {
        m_running = false;
        m_pOptions->Save();
        m_monitor->Update();
        SAFE_DELETE(g_pTimer)
        m_monitor->Update();
    }
}

void APP::Start()
{
    m_paused = false;
    srand(time(0));
    m_running = true;
    g_pTimer->Start();
}

void APP::SetResolution(int index)
{
    m_pOptions->m_resolution = g_pDisplay->SetResolution(index);
    g_pInput->SetMouseCapture(g_pInput->m_mouseCapture);
}

void APP::SetFullscreen(bool on)
{
    m_pOptions->m_fullscreen = on;
    g_pDisplay->SetFullscreen(on);
    g_pInput->SetMouseCapture(g_pInput->m_mouseCapture);
}

void APP::SetSound(bool on)
{
    m_pOptions->m_sound = on;
    ((SOUNDMANAGER *)g_pSOUND)->EnableStreams(on);
}

void APP::SetMusic(bool on)
{
    m_pOptions->m_music = on;
    ((SOUNDMANAGER *)g_pSOUND)->EnableSounds(on);
}

void APP::SetDetail(int level)
{
    m_pOptions->m_detail = level;
    g_pDisplay->SetDetail(level);
}

void APP::Virtual04()
{
    g_pDisplay->Virtual03();
}

APP::APP()
{
    m_map[0] = 0;
    m_map[sizeof(m_map) - 1] = 0;
    g_pLog = g_pfnCreateLog();
    m_pOptions = g_pfnCreateAppOptions();
    g_pTimer = 0;
    g_pScene = 0;
    m_running = false;
    m_paused = false;
    m_2e = false;
    m_monitor = new SYSMONITOR;
    m_monitor->Update();
    m_level = -1;
    m_screenshot = new SCREENSHOT;
}

APP::~APP()
{
    SAFE_DELETE(m_screenshot)
    m_monitor->Update();
    SAFE_DELETE(m_pOptions)
    m_monitor->Update();
    SAFE_DELETE(g_pDrm)
    m_monitor->Update();
    SAFE_DELETE(g_pTimer)
    m_monitor->Update();
    SAFE_DELETE(g_pDisplay)
    m_monitor->Update();
    SAFE_DELETE(m_monitor)
    SAFE_DELETE(g_pLog)
}

bool APP::ParseArg(const char *arg)
{
    if (!_stricmp(arg, "-map")) {
        char *value = strtok(0, " =");
        if (value) {
            char *map = g_pApp->m_map;
            strncpy(map, value, sizeof(g_pApp->m_map) - 1);
            map[sizeof(g_pApp->m_map) - 1] = 0;
        }
        return true;
    }
    if (!_stricmp(arg, "-level")) {
        char *value = strtok(0, " =");
        if (value)
            g_pApp->m_level = atoi(value);
        return true;
    }
    return false;
}

APP *CreateApp()
{
    return new APP;
}
