// The application's window and renderer, which a plugin provides: the base
// keeps the screen size and how far its shape is from 4:3, and passes the
// window's keyboard, mouse and media change messages on to the GUI and the
// scripts. Placeholder names apart from the class name.
#ifndef TONGAS2_APPWINDOW_H
#define TONGAS2_APPWINDOW_H

struct HWND__;

class APPWINDOW
{
public:
    APPWINDOW();
    virtual ~APPWINDOW();
    // Opens the window and the renderer.
    virtual void Create() = 0;
    // Closes them.
    virtual void Virtual02() = 0;
    virtual void Virtual03() = 0;
    virtual void Virtual04() = 0;
    virtual void Virtual05() = 0;
    virtual void Virtual06() = 0;
    virtual void Virtual07() = 0;
    virtual void Virtual08() = 0;
    virtual void SetDetail(int level) = 0;
    virtual void Virtual10() {}
    virtual void Virtual11() {}
    // Seconds since the last call.
    virtual float GetElapsedTime();
    // Puts the mouse cursor at a point in the window, in pixels.
    virtual void MoveCursor(int x, int y) = 0;
    virtual void Virtual14() = 0;
    virtual bool GetWireframe() = 0;
    virtual void SetWireframe(bool on) = 0;
    virtual void SetFrameRate(int rate);
    // What the last frames drew.
    virtual float GetFps() = 0;
    virtual int GetRenderCount() = 0;
    virtual int GetPolyCount() = 0;
    virtual void SetFullscreen(bool on) = 0;
    virtual void Virtual22() = 0;
    virtual void Virtual23() = 0;
    // Switches to the resolution with the index; the index it switched to.
    virtual int SetResolution(int index) = 0;
    // The options' resolution, with the first four taken as the fifth.
    virtual int GetResolution();
    virtual void Virtual26() = 0;
    // The size of the screen in pixels.
    virtual int GetWidth() { return m_width; }
    virtual int GetHeight() { return m_height; }
    virtual void Virtual29() = 0;
    // Takes the screen's size and works out how far its shape is from 4:3.
    virtual void SetSize(int width, int height);
    // Whether the screen's shape calls for heights to be corrected, and the
    // correction.
    virtual bool NeedsAspectFix() { return m_aspectFix; }
    virtual void FixHeight(float *height);
    virtual void Virtual33(float *height);
    // The same for a position from 0 to 1, about the middle.
    virtual void FixY(float *y);
    virtual void UnfixY(float *y);
    virtual void Virtual36() { m_14 = 0; }
    virtual void Virtual37() = 0;
    virtual HWND__ *GetWindow() = 0;
    // Handles a window message; -1 for those it leaves to the default.
    virtual long WndProc(HWND__ *window, unsigned int message, unsigned int wParam, long lParam);

    int m_width;
    int m_height;
    int m_frameRate;
    bool m_10;
    int m_14;
    // The instance APP::Init was given.
    void *m_instance;
    bool m_aspectFix;
    // Height over width.
    float m_aspect;
    float m_baseAspect;
    // m_aspect over m_baseAspect.
    float m_aspectScale;
};

extern APPWINDOW *g_pDisplay;

// Makes the window; the renderer plugin points it at its own.
extern APPWINDOW *(*g_pfnCreateAppWindow)();

#endif
