#include <windows.h>
#include <dbt.h>

#include "Tongas2/APP.h"
#include "Tongas2/APPOPTIONS.h"
#include "Tongas2/APPWINDOW.h"
#include "Tongas2/CMDMGR.h"
#include "Tongas2/INPUT.h"
#include "Tongas2/LOG.h"
#include "Tongas2/SCRIPTROOT.h"

APPWINDOW *CreateAppWindow();

APPWINDOW *(*g_pfnCreateAppWindow)() = CreateAppWindow;

// The renderer plugin replaces it.
APPWINDOW *CreateAppWindow()
{
    if (g_pLog)
        g_pLog->Error("No default plugin for APPWINDOW");
    return 0;
}

APPWINDOW::APPWINDOW()
{
    m_width = 0;
    m_height = 0;
    m_10 = false;
    m_14 = 0;
    m_instance = 0;
    m_aspectFix = false;
    m_frameRate = 60;
    m_aspect = 0.75f;
    m_baseAspect = 0.75f;
    m_aspectScale = 1.0f;
}

float APPWINDOW::GetElapsedTime()
{
    return 0.0f;
}

APPWINDOW::~APPWINDOW()
{
}

void APPWINDOW::SetFrameRate(int rate)
{
    m_frameRate = rate;
}

// The letter of the first drive in a device change's unit mask, as in the
// Platform SDK's sample.
char FirstDriveFromMask(ULONG unitmask)
{
    char i;
    for (i = 0; i < 26; ++i) {
        if (unitmask & 0x1)
            break;
        unitmask = unitmask >> 1;
    }
    return i + 'A';
}

long APPWINDOW::WndProc(HWND__ *window, unsigned int message, unsigned int wParam, long lParam)
{
    switch (message) {
    case WM_CLOSE:
        if (!m_10) {
            g_pApp->Virtual04();
            return 0;
        }
        if (g_pApp->m_pOptions->m_fullscreen) {
            SetFullscreen(false);
            Virtual22();
        }
        Virtual02();
        return -1;
    case WM_CHAR:
        if (!g_pInput)
            break;
        g_pInput->OnKey(wParam, INPUTKEY_CHAR);
        return -1;
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
        if (!g_pInput)
            break;
        g_pInput->OnKey(wParam, INPUTKEY_DOWN);
        return -1;
    case WM_MOUSEMOVE:
        if (!g_pInput)
            break;
        if (!g_pInput->m_mouseCapture) {
            if (!(wParam & MK_LBUTTON) && g_pInput->IsButtonDown(0))
                g_pInput->OnMouse(INPUTMOUSE_LUP, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            if (!(wParam & MK_MBUTTON) && g_pInput->IsButtonDown(2))
                g_pInput->OnMouse(INPUTMOUSE_MUP, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            if (!(wParam & MK_RBUTTON) && g_pInput->IsButtonDown(1))
                g_pInput->OnMouse(INPUTMOUSE_RUP, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        }
        g_pInput->OnMouse(INPUTMOUSE_MOVE, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        return 0;
    case WM_LBUTTONDOWN:
        if (!g_pInput)
            break;
        g_pInput->OnMouse(INPUTMOUSE_LDOWN, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        return -1;
    case WM_LBUTTONUP:
        if (!g_pInput)
            break;
        g_pInput->OnMouse(INPUTMOUSE_LUP, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        return -1;
    case WM_KEYUP:
    case WM_SYSKEYUP:
        if (!g_pInput)
            break;
        g_pInput->OnKey(wParam, INPUTKEY_UP);
        return -1;
    case WM_LBUTTONDBLCLK:
        if (!g_pInput)
            break;
        g_pInput->OnMouse(INPUTMOUSE_LDBLCLK, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        return -1;
    case WM_MBUTTONDBLCLK:
        if (!g_pInput)
            break;
        g_pInput->OnMouse(INPUTMOUSE_MDBLCLK, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        return -1;
    case WM_MBUTTONDOWN:
        if (!g_pInput)
            break;
        g_pInput->OnMouse(INPUTMOUSE_MDOWN, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        return -1;
    case WM_MBUTTONUP:
        if (!g_pInput)
            break;
        g_pInput->OnMouse(INPUTMOUSE_MUP, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        return -1;
    case WM_RBUTTONDBLCLK:
        if (!g_pInput)
            break;
        g_pInput->OnMouse(INPUTMOUSE_RDBLCLK, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        return -1;
    case WM_RBUTTONDOWN:
        if (!g_pInput)
            break;
        g_pInput->OnMouse(INPUTMOUSE_RDOWN, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        return -1;
    case WM_RBUTTONUP:
        if (!g_pInput)
            break;
        g_pInput->OnMouse(INPUTMOUSE_RUP, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        return -1;
    case WM_MOUSEWHEEL: {
        if (!g_pInput)
            break;
        POINT point;
        point.x = GET_X_LPARAM(lParam);
        point.y = GET_Y_LPARAM(lParam);
        ScreenToClient(GetWindow(), &point);
        if (GET_WHEEL_DELTA_WPARAM(wParam) > 0)
            g_pInput->OnMouse(INPUTMOUSE_WHEELUP, point.x, point.y);
        else
            g_pInput->OnMouse(INPUTMOUSE_WHEELDOWN, point.x, point.y);
        return -1;
    }
    case WM_DEVICECHANGE: {
        PDEV_BROADCAST_VOLUME volume = (PDEV_BROADCAST_VOLUME)lParam;
        switch (wParam) {
        case DBT_DEVICEARRIVAL:
            if (volume->dbcv_devicetype == DBT_DEVTYP_VOLUME && (volume->dbcv_flags & DBTF_MEDIA) && g_pScriptRoot)
                ((SCRIPTROOT *)g_pScriptRoot)->OnMediaArrived(FirstDriveFromMask(volume->dbcv_unitmask));
            break;
        case DBT_DEVICEREMOVECOMPLETE:
            if (volume->dbcv_devicetype == DBT_DEVTYP_VOLUME && (volume->dbcv_flags & DBTF_MEDIA) && g_pScriptRoot) {
                ((SCRIPTROOT *)g_pScriptRoot)->OnMediaRemoved(FirstDriveFromMask(volume->dbcv_unitmask));
                return -1;
            }
            break;
        }
        break;
    }
    }
    return -1;
}

void APPWINDOW::SetSize(int width, int height)
{
    float w = (float)width;
    m_aspect = (float)height / w;
    if (m_aspect != m_baseAspect)
        m_aspectFix = true;
    else
        m_aspectFix = false;
    m_aspectScale = m_aspect / m_baseAspect;
}

void APPWINDOW::FixHeight(float *height)
{
    *height *= m_aspectScale;
}

void APPWINDOW::Virtual33(float *height)
{
    *height /= m_aspectScale;
}

void APPWINDOW::FixY(float *y)
{
    *y = (*y - 0.5f) * m_aspectScale + 0.5f;
}

void APPWINDOW::UnfixY(float *y)
{
    *y = (*y - 0.5f) / m_aspectScale + 0.5f;
}

int APPWINDOW::GetResolution()
{
    int index = g_pApp->m_pOptions->m_resolution;
    if (index == 2 || index == 3 || index == 0 || index == 1)
        index = 4;
    return index;
}
