// The application: starts the engine's systems in order, runs a frame of
// them and shuts them down, and keeps the options and the command line's
// -map and -level. Placeholder names apart from the class name.
#ifndef TONGAS2_APP_H
#define TONGAS2_APP_H

#include "Tongas2/STRING.h"

struct HINSTANCE__;
class APPOPTIONS;
class SCREENSHOT;
class SYSMONITOR;

class APP
{
public:
    APP();

    // Reads the size of the screen.
    virtual void GetScreenSize();
    virtual int Virtual01() { return 0; }
    virtual ~APP();
    // Takes a command line argument; false if it isn't one of the app's.
    virtual bool ParseArg(const char *arg);
    virtual void Virtual04();

    // Runs a frame.
    void Frame(float dt);
    // Starts the systems.
    void Init(HINSTANCE__ *instance);
    // Saves the options and stops the timer.
    void Shutdown();
    // Starts running.
    void Start();
    void SetResolution(int index);
    void SetFullscreen(bool on);
    void SetSound(bool on);
    void SetMusic(bool on);
    void SetDetail(int level);

    int m_screenWidth;
    int m_screenHeight;
    SYSMONITOR *m_monitor;
    APPOPTIONS *m_pOptions;
    STRING m_14;
    bool m_running;
    // Only the display is updated.
    bool m_paused;
    bool m_2e;
    // The -map argument.
    char m_map[0x200];
    // The -level argument, or -1.
    int m_level;
    SCREENSHOT *m_screenshot;
};

extern APP *g_pApp;

// Makes the application.
extern APP *(*g_pfnCreateApp)();

// The folder the game runs from, ending in '/'.
extern char g_basePath[];

#endif
