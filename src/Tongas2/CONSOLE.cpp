#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "Tongas2/APP.h"
#include "Tongas2/APPOPTIONS.h"
#include "Tongas2/APPWINDOW.h"
#include "Tongas2/AUDIO.h"
#include "Tongas2/CMDMGR.h"
#include "Tongas2/CONSOLE.h"
#include "Tongas2/FIXEDWSTRING.h"
#include "Tongas2/LOG.h"
#include "Tongas2/RESOURCES.h"
#include "Tongas2/SCREENSHOT.h"
#include "Tongas2/TEXT.h"
#include "Tongas2/SYSMONITOR.h"

#define VK_BACK 0x08
#define VK_RETURN 0x0D
#define VK_LEFT 0x25
#define VK_UP 0x26
#define VK_RIGHT 0x27
#define VK_DOWN 0x28
#define VK_F8 0x77
#define VK_F11 0x7A

CONSOLE *CreateConsole();

CONSOLE *(*g_pfnCreateConsole)() = CreateConsole;

CONSOLE *g_pConsole;
// The number of the next screenshot file to try.
static int s_screenshotIndex;

void CONSOLE::Enable()
{
    m_enabled = true;
    for (int i = 0; i < CONSOLE_LINES; i++)
        m_history[i][0] = 0;
}

void CONSOLE::Show(bool open)
{
    m_open = open;
    int i;
    for (i = 0; i < CONSOLE_LINES; i++) {
        RESOURCE *line = g_pResources->GetResource(m_lines[i]);
        if (line)
            line->Virtual20();
        m_lines[i].Invalidate();
    }
    RESOURCE *prompt = g_pResources->GetResource(m_prompt);
    if (prompt)
        prompt->Virtual20();
    m_prompt.Invalidate();
    RESOURCE *stats = g_pResources->GetResource(m_stats);
    if (stats)
        stats->Virtual20();
    m_stats.Invalidate();
    if (!m_open) {
        SetPriority(-1);
        return;
    }
    for (i = 0; i < CONSOLE_LINES; i++) {
        TEXT *text = (TEXT *)g_pResources->GetResource(GetFont());
        m_lines[i] = text->m_handle;
        text->m_alignH = 0;
        text->SetScreenPosition(0.0f, GetLineHeight() * (i + 1));
        text->m_onScreen = true;
        text->SetSize(GetScale());
        text->SetLayer(0x7e);
    }
    TEXT *text = (TEXT *)g_pResources->GetResource(GetFont());
    m_prompt = text->m_handle;
    text->m_alignH = 0;
    text->SetScreenPosition(0.0f, GetLineHeight() * (i + 1));
    text->m_onScreen = true;
    text->SetSize(GetScale());
    text->SetLayer(0x7e);
    text = (TEXT *)g_pResources->GetResource(GetFont());
    m_stats = text->m_handle;
    text->m_alignH = 0;
    text->SetScreenPosition(0.0f, GetLineHeight() * (i + 2));
    text->m_onScreen = true;
    text->SetSize(GetScale());
    text->SetLayer(0x7e);
    SetPriority(-2);
}

bool CONSOLE::OnChar(unsigned int c)
{
    if (m_22d8 > 0)
        return false;
    wchar_t ch = (wchar_t)c;
    if (m_open && ch >= 0x20 && ch <= 0x7f)
        SafeSprintf(m_input, "%s%c", (char *)m_input, ch);
    return true;
}

void CONSOLE::SelectHistory(int index)
{
    if (index < 0)
        index = CONSOLE_LINES - 1;
    else if (index >= CONSOLE_LINES)
        index = 0;
    if (index == m_next) {
        m_browse = -1;
        m_input[0] = 0;
        return;
    }
    m_browse = index;
    SafeSprintf(m_input, m_history[m_browse]);
}

CONSOLE::CONSOLE()
{
    m_first = 0;
    m_next = 0;
    m_d1 = false;
    m_open = false;
    m_22d8 = 0;
    m_enabled = false;
    m_browse = -1;
    SetPriority(-1);
}

bool CONSOLE::OnHotKey(const int &key)
{
    if (key == VK_F8) {
        SCREENCAPTURE capture;
        FIXEDSTRING<0x200> path;
        FILE *fp = 0;
        do {
            if (fp)
                fclose(fp);
            SafeSprintf(path, "scr%03d.png", s_screenshotIndex);
            s_screenshotIndex++;
            fp = fopen(path, "r");
        } while (fp);
        capture.Save(path, g_pDisplay->GetWindow(), 1, 0, V2(0.0f, 0.0f), V2(0.0f, 0.0f), V2(0.0f, 0.0f));
        SafeSprintf(path, "scr%03d.jpg", s_screenshotIndex - 1);
        capture.Save(path, g_pDisplay->GetWindow(), 0, 0x100, V2(0.0f, 0.0f), V2(0.0f, 0.0f), V2(0.0f, 0.0f));
        return true;
    }
    if (key == 'F') {
        g_pApp->SetFullscreen(!g_pApp->m_pOptions->m_fullscreen);
        return true;
    }
    if (key == 'P') {
        g_pAudio->Pause(true);
        return true;
    }
    if (key == 'C') {
        g_pActors->ToggleHelpers();
        return true;
    }
    if (key == 'W') {
        g_pDisplay->SetWireframe(!g_pDisplay->GetWireframe());
        return true;
    }
    if (key == 'T') {
        g_pLog->Toggle();
        return true;
    }
    return false;
}

void CONSOLE::Update()
{
    if (!m_open)
        return;
    int line = 0;
    for (int i = m_first; i != m_next; i++) {
        if (i >= CONSOLE_LINES) {
            i = -1;
            continue;
        }
        m_history[i][0xfe] = 0;
        RESOURCE *text = g_pResources->GetResource(m_lines[line]);
        if (text) {
            FIXEDWSTRING<0x200> wide;
            wide.FromAnsi(m_history[i]);
            ((TEXT *)text)->SetText(wide.m_buf);
            text->SetActive(true);
        }
        line++;
    }
    for (; line < CONSOLE_LINES; line++) {
        RESOURCE *text = g_pResources->GetResource(m_lines[line]);
        if (text && text->m_active) {
            text->Stop();
            text->m_active = false;
        }
    }
    sprintf(m_text, "> %s", (char *)m_input);
    RESOURCE *prompt = g_pResources->GetResource(m_prompt);
    if (prompt) {
        FIXEDWSTRING<0x200> wide;
        wide.FromAnsi(m_text);
        ((TEXT *)prompt)->SetText(wide.m_buf);
        prompt->SetActive(true);
    }
    sprintf(m_text, "FPS:%.1f POLYS:%d RENDERCOUNT:%d CPU:%d%% MEM: %dK", g_pDisplay->GetFps(),
        g_pDisplay->GetPolyCount(), g_pDisplay->GetRenderCount(), g_pApp->m_monitor->GetCpuUsage(), 0);
    RESOURCE *stats = g_pResources->GetResource(m_stats);
    if (stats) {
        FIXEDWSTRING<0x200> wide;
        wide.FromAnsi(m_text);
        ((TEXT *)stats)->SetText(wide.m_buf);
        stats->SetActive(true);
    }
}

CONSOLE *CreateConsole()
{
    return new CONSOLE;
}

void CONSOLE::Run(const char *format, ...)
{
    if (!m_enabled)
        return;
    va_list args;
    va_start(args, format);
    vsprintf(m_text, format, args);
    int last = m_next - 1;
    if (last < 0)
        last = CONSOLE_LINES;
    if (!strcmp(m_history[last], m_input))
        m_next = last;
    else
        strcpy(m_history[m_next], m_input);
    {
        STRING command(m_history[m_next]);
        g_pCmdMgr->Run(command, RESHANDLE());
    }
    if (++m_next >= CONSOLE_LINES)
        m_next = 0;
    while (m_first == m_next) {
        if (++m_first >= CONSOLE_LINES)
            m_first = 0;
    }
}

bool CONSOLE::OnKeyDown(unsigned int vkey)
{
    if (m_22d8 > 0)
        return false;
    int key = vkey;
    if (key == VK_F11 && CanOpen()) {
        Show(!m_open);
        return true;
    }
    if (!m_open) {
        OnHotKey(key);
        return true;
    }
    switch (key) {
    case VK_RETURN:
        Run(m_input);
        m_input[0] = 0;
        m_browse = -1;
        break;
    case VK_LEFT:
    case VK_RIGHT:
        break;
    case VK_UP:
        if (m_browse == -1) {
            if (m_next != m_first)
                SelectHistory(m_next - 1);
        } else if (m_browse != m_first)
            SelectHistory(m_browse - 1);
        break;
    case VK_DOWN:
        if (m_browse != -1 && m_browse != m_next)
            SelectHistory(m_browse + 1);
        break;
    case VK_BACK:
        if ((int)strlen(m_input) > 0)
            m_input[strlen(m_input) - 1] = 0;
        break;
    default:
        return true;
    }
    return true;
}
