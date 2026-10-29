// GUITEXTEDIT, the GUI's text boxes: GUI text the player types into, each
// value shown through its template's separators mask with a blinking
// caret, limited in length and to the allowed characters, with pasting and
// the template's Enter command. The original file name is not known; the
// masters are in GUITEXTEDIT.cpp.
#include <windows.h>

#include "Tongas2/GUITEXTEDIT.h"
#include "Tongas2/CMDMGR.h"
#include "Tongas2/CONSOLE.h"
#include "Tongas2/GUI.h"
#include "Tongas2/LOG.h"
#include "Tongas2/PATH.h"
#include "Tongas2/SAFEINDEX.h"
#include "Tongas2/STRING.h"
#include "Tongas2/TIMER.h"
#include "Tongas2/TOKENS.h"

#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#define CMD_FUNC(CLASS, method) (CMDFUNC)(SRESULT (CLASS::*)(TOKENS **))(SRESULT (CLASS::*)(CMDCALL *))&CLASS::method

CMDENTRY GUITEXTEDIT::s_commands[] = {
    { "DoneInput", 0, 0, 0, CMD_FUNC(GUITEXTEDIT, DoneInput) },
    { "Lenght", 0, 1, 1, CMD_FUNC(GUITEXTEDIT, Length) },
    { "IsEmpty", 0, 1, 1, CMD_FUNC(GUITEXTEDIT, IsEmpty) },
    { 0, 0, 0, 0, 0 },
};

PROPENTRY GUITEXTEDIT::s_properties[] = {
    { "Value", 0, 6, 0, PROP_FIELD(GUITEXTEDIT, m_values), false, GUITEXT_MAX, PROP_SETTER(GUITEXTEDIT, SetValue) },
    { 0, 0, 15, 0, 0, false },
};

void GUITEXTEDIT::RegisterCommands()
{
    g_classParents[s_classId] = GUITEXT::s_classId;
    g_classCommands[s_classId] = s_commands;
    HashCommands(s_commands);
    RegisterClassName(s_commands, "GUITEXTEDIT", "GUITEXT");
}

GUITEXTEDIT::~GUITEXTEDIT()
{
    if (g_pConsole)
        g_pConsole->m_22d8--;
    g_pGui->UnlockInput();
}

void GUITEXTEDIT::Update()
{
    GUITEXT::Update();
    if (m_active >= 0) {
        m_blinkTime -= g_pTimer->GetSoundDt();
        if (m_blinkTime <= 0.0f) {
            m_blinkTime += 0.25f;
            m_caretOn = !m_caretOn;
            SetText(m_active + 0, m_values[m_active]);
        }
    }
}

void GUITEXTEDIT::AddChar(wchar_t c)
{
    int i = m_active;
    if (i < 0)
        return;
    wchar_t *value = m_values[i].m_buf;
    int length = wcslen(value);
    if ((!m_editMaster->m_dontAllowStartSpaces[i] || c != L' ' || length != 0) && length < m_editMaster->m_maxLen[i]) {
        if (m_editMaster->m_limitChars[i].Length() > 0 && !wcschr(m_editMaster->m_limitChars[i].c_str(), c))
            return;
        value[length] = c;
        value[length + 1] = 0;
    }
}

void GUITEXTEDIT::Paste()
{
    if (!OpenClipboard(0))
        return;
    HANDLE data = GetClipboardData(CF_UNICODETEXT);
    if (!data) {
        data = GetClipboardData(CF_TEXT);
        if (!data) {
            CloseClipboard();
            return;
        }
    }
    wchar_t *text = (wchar_t *)GlobalLock(data);
    if (text) {
        for (unsigned int i = 0; i < wcslen(text); i++)
            AddChar(text[i]);
        GlobalUnlock(data);
    }
    CloseClipboard();
}

GUITEXTEDIT::GUITEXTEDIT(const RESHANDLE &master, GUIGROUP *parent) : GUITEXT(master, parent)
{
    m_editMaster = 0;
    m_bound = false;
    for (g_i = 0; g_i < GUITEXT_MAX; g_i++)
        m_values[g_i] = L"";
    m_editMaster = (GUITEXTEDITMASTER *)g_pResources->GetMaster(master);
    if (!m_editMaster && g_pLog)
        g_pLog->Error("ERROR CREATING GUITEXTEDIT CONTROL!!");
    INPUTLISTENER::SetPriority(-1);
    m_active = 0;
    m_blinkTime = 0.0f;
    m_caretOn = false;
    g_pConsole->m_22d8++;
    g_pGui->LockInput();
}

void GUITEXTEDIT::SetText(const int &i, const FIXEDWSTRING<0x200> &text)
{
    SetValue(i, text);
    int maskLength = m_editMaster->m_separators[i].Length();
    if (maskLength <= 0) {
        if (!m_caretOn && m_editMaster->m_caretBlink[i]) {
            GUITEXT::SetText(i, text);
            return;
        }
        wchar_t shown[0x200];
        if (&text)
            memcpy(shown, text.m_buf, sizeof(shown));
        else
            shown[0] = 0;
        wcscat(shown, m_editMaster->m_caretChar[i].c_str());
        GUITEXT::SetText(i, *(FIXEDWSTRING<0x200> *)shown);
        return;
    }
    int length = wcslen(text.m_buf);
    int j = 0;
    FIXEDWSTRING<0x200> shown;
    int k;
    for (k = 0; k < maskLength && j < length; k++) {
        wchar_t c = m_editMaster->m_separators[i].c_str()[k];
        if (c == '%')
            c = text.m_buf[j++];
        shown.m_buf[k] = c;
    }
    shown.m_buf[k] = 0;
    if (m_caretOn || !m_editMaster->m_caretBlink[i]) {
        const wchar_t *caret = m_editMaster->m_caretChar[i].c_str();
        int caretLength = wcslen(caret);
        if ((int)wcslen(shown.m_buf) + caretLength < 0x1ff)
            wcscpy(shown.m_buf + wcslen(shown.m_buf), caret);
    }
    GUITEXT::SetText(i, shown);
}

bool GUITEXTEDIT::OnKeyDown(unsigned int key)
{
    if (m_active < 0)
        return false;
    if (IsEnabled() && IsVisible()) {
        wcslen(m_values[m_active].m_buf);
        if (key == VK_RETURN) {
            CMDCALL call;
            DoneInput(&call);
            return true;
        }
        if (key == 'V' && g_pInput->IsKeyDown(VK_CONTROL)) {
            Paste();
            return true;
        }
        return true;
    }
    return false;
}

bool GUITEXTEDIT::OnChar(unsigned int c)
{
    if (m_active < 0)
        return false;
    if (IsEnabled() && IsVisible()) {
        int i = m_active;
        int length = wcslen(m_values[i].m_buf);
        wchar_t ch = (wchar_t)c;
        if (ch == 8) {
            if (length > 0)
                m_values[i].m_buf[length - 1] = 0;
            return true;
        }
        if (ch >= 0x20)
            AddChar(ch);
        return true;
    }
    return false;
}

SRESULT GUITEXTEDIT::Length(CMDCALL *call)
{
    return SRESULT((int)wcslen(m_values[atoi(call->tokens->GetToken(1))].m_buf));
}

SRESULT GUITEXTEDIT::IsEmpty(CMDCALL *call)
{
    return SRESULT(m_values[atoi(call->tokens->GetToken(1))].m_buf[0] == 0);
}

FIXEDSTRING<0x200> GUITEXTEDIT::GetBindVar(int i)
{
    if (m_bound) {
        FIXEDSTRING<0x200> name;
        SafeSprintf(name, "my.value %d", i);
        return name;
    }
    m_boundValues[i].Assign(SRESULT());
    m_bound = true;
    return GUITEXT::GetBindVar(i);
}

SRESULT GUITEXTEDIT::DoneInput(CMDCALL *call)
{
    {
        STRING command(m_editMaster->m_onEnterKey[m_active].c_str());
        g_pCmdMgr->Run(command, m_handle);
    }
    return SRESULT(true);
}
