#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "Tongas2/APP.h"
#include "Tongas2/APPOPTIONS.h"
#include "Tongas2/APPWINDOW.h"
#include "Tongas2/CAMERA.h"
#include "Tongas2/CMDMGR.h"
#include "Tongas2/DECAL.h"
#include "Tongas2/GUI.h"
#include "Tongas2/GUICHECKBOX.h"
#include "Tongas2/GUIGROUP.h"
#include "Tongas2/LOG.h"
#include "Tongas2/RESOURCES.h"
#include "Tongas2/SCRIPTROOT.h"
#include "Tongas2/SOUND.h"
#include "Tongas2/TIMER.h"

#define VK_RETURN 0x0D
#define VK_SPACE 0x20
#define VK_LEFT 0x25
#define VK_UP 0x26
#define VK_RIGHT 0x27
#define VK_DOWN 0x28

#define CMD_FUNC(CLASS, method) (CMDFUNC)(SRESULT (CLASS::*)(TOKENS **))(SRESULT (CLASS::*)(CMDCALL *))&CLASS::method

// Defined elsewhere.
extern RESOURCEMANAGER *g_pGUIGROUP;

GUI *CreateGui();

GUI *(*g_pfnCreateGui)() = CreateGui;

CMDENTRY GUI::s_commands[] = {
    { "blackzsort", 0, 1, 1, CMD_FUNC(GUI, BlackZSort) },
    { "blackopacity", 0, 1, 1, CMD_FUNC(GUI, BlackOpacity) },
    { "blackfade", 0, 3, 3, CMD_FUNC(GUI, BlackFade) },
    { 0, 0, 0, 0, 0 },
};

PROPENTRY GUI::s_properties[] = {
    { "Screen", 0, 9, 0, PROP_FIELD(GUI, m_screenName), false, 1, PROP_SETTER(GUI, SetScreen) },
    { "Fps", 0, 3, 0, PROP_FIELD(GUI, m_fps), false, 1, PROP_SETTER(GUI, SetFps) },
    { "LoadingProgress", 0, 3, 0, PROP_FIELD(GUI, m_loadingProgress), false, 1, PROP_SETTER(GUI, SetLoadingProgress) },
    { "VersionString", 0, 6, 0, PROP_FIELD(GUI, m_versionString), false, 1, PROP_SETTER(GUI, SetVersionString) },
    { 0, 0, 15, 0, 0, false },
};

void GUI::RegisterCommands()
{
    g_classParents[s_classId] = g_CMDBASEClassId;
    g_classCommands[s_classId] = s_commands;
    HashCommands(s_commands);
    RegisterClassName(s_commands, "GUI", "CMDBASE");
}

void GUI::SetBlackZSort(int zsort)
{
    DECAL *black = (DECAL *)g_pResources->GetResource(m_black);
    if (black) {
        black->SetLayer(zsort);
        black->SetScreenSize(2.0f, 1.0f);
        return;
    }
    if (g_pLog)
        g_pLog->Error("GUI::SetBlackOpacity>ERROR: no decal to set zsort");
}

void GUI::SetBlackOpacity(float opacity)
{
    DECAL *black = (DECAL *)g_pResources->GetResource(m_black);
    if (black) {
        black->SetAlpha(opacity);
        black->SetScreenSize(2.0f, 1.0f);
        return;
    }
    if (g_pLog)
        g_pLog->Error("GUI::SetBlackOpacity>ERROR: no decal to set opacity");
}

void GUI::FadeBlack(float from, float to, float time)
{
    SetBlackOpacity(from);
    m_fadeTarget = to;
    m_fadeTime = time;
}

GUIGROUP *GUI::AddGroup(const char *name)
{
    if (m_screen)
        return m_screen->AddGroup(name);
    return 0;
}

void GUI::Pause(bool toggle)
{
    if (g_pGame && g_pGame->IsPlaying()) {
        if (g_pTimer->m_paused) {
            if (toggle) {
                CMDCALL call;
                ((SCRIPTROOT *)g_pScriptRoot)->Unpause(&call);
            }
        } else {
            g_pTimer->SetPaused(true);
            g_pGame->OnPaused();
        }
    }
}

CMDBASE *GUI::GetObject(const RESHANDLE &handle)
{
    int index = handle.index;
    if (index >= 0 && index < GUI_MAX_CONTROLS - 1) {
        GUICONTROL *control = m_controls[index];
        if (control && control->m_handle == handle)
            return control;
    }
    return 0;
}

void GUI::AddControl(GUICONTROL *control)
{
    if (m_count < GUI_MAX_CONTROLS) {
        RESHANDLE handle;
        handle.kind = 2;
        handle.index = m_numFree > 0 ? m_freeList[--m_numFree] : m_nextFree++;
        handle.serial = m_serial;
        m_count++;
        m_serial++;
        m_controls[handle.index] = control;
        control->m_handle = handle;
    }
}

void GUI::RemoveControl(GUICONTROL *control)
{
    if (control->m_handle.IsValid()) {
        m_freeList[m_numFree++] = control->m_handle.index;
        m_controls[control->m_handle.index] = 0;
        control->m_handle.Invalidate();
        m_count--;
    }
}

void GUI::UpdateLayout()
{
    if (m_screen)
        m_screen->UpdateLayout();
}

void *GUI::Virtual09(const char *name)
{
    if (m_screen)
        return m_screen->Find(name);
    if (g_pLog)
        g_pLog->Error("FINDING... '%s' in the gui and not found...", name);
    return 0;
}

bool GUI::IsSelectable(GUICONTROL *control, const V2 &mouse)
{
    if (control->GetClassId() != GUIGROUP::s_classId && control->GetClassId() != GUICHECKBOX::s_classId)
        return false;
    return control->IsVisible() && control->IsEnabled() && control->IsActive()
        && ((GUIGROUP *)control)->m_master->m_selectableWithKeyboard
        && ((GUIGROUP *)control)->IsMouseOver(mouse.x, mouse.y);
}

bool GUI::IsSelectableElsewhere(GUICONTROL *control, const V2 &mouse)
{
    int top = -1;
    if (m_screen)
        top = m_screen->GetMouseZSort(-1);
    if (control->GetClassId() != GUIGROUP::s_classId && control->GetClassId() != GUICHECKBOX::s_classId)
        return false;
    return control->m_zsort > top && control->IsVisible() && control->IsEnabled() && control->IsActive()
        && ((GUIGROUP *)control)->m_master->m_selectableWithKeyboard
        && !((GUIGROUP *)control)->IsMouseOver(mouse.x, mouse.y);
}

bool GUI::MoveMouseTo(GUICONTROL *control)
{
    g_pDisplay->MoveCursor((int)(g_pDisplay->GetWidth() * control->m_position.x),
        (int)(g_pDisplay->GetHeight() * control->m_position.y));
    return true;
}

bool GUI::ClickSelected()
{
    V2 mouse(g_pInput->m_mouse.x, g_pInput->m_mouse.y);
    for (int i = 0; i < m_nextFree && i < GUI_MAX_CONTROLS; i++) {
        if (m_controls[i] && IsSelectable(m_controls[i], mouse)) {
            m_controls[i]->Click();
            return true;
        }
    }
    return false;
}

int GUI::GetTopZSort()
{
    if (m_screen)
        return m_screen->GetTopZSort();
    return -1;
}

void GUI::PlayClick()
{
    m_playClick = true;
}

void GUI::PlayMouseOver()
{
    m_playMouseOver = true;
}

void GUI::LockInput()
{
    m_inputLock++;
}

void GUI::UnlockInput()
{
    m_inputLock--;
}

void GUI::ShowControl(const char *name, float time)
{
    if (m_screen) {
        GUICONTROL *control = g_pGui->m_screen->FindControl(name);
        if (control)
            control->FadeTo(1.0f, time);
        else if (g_pLog)
            g_pLog->Error("Can't find GUICONTROL %s", name);
    }
}

void GUI::HideControl(const char *name, float time)
{
    if (m_screen) {
        GUICONTROL *control = g_pGui->m_screen->FindControl(name);
        if (control)
            control->FadeTo(0.0f, time);
        else if (g_pLog)
            g_pLog->Error("Can't find GUICONTROL %s", name);
    }
}

void GUI::ReloadScreen()
{
    g_pGui->SetScreen(m_screenName);
}

void GUI::OnFullscreenChanged()
{
    if (g_pTimer->m_paused)
        Pause(true);
    Pause(false);
}

bool GUI::GetDistance(const int &direction, const V2 &from, GUICONTROL *control, float *distance)
{
    if (!control) {
        *distance = 1000.0f;
        return false;
    }
    V2 position = control->m_position;
    V2 offset = position - from;
    float side = 0.0f;
    V2 axis(0.0f, 0.0f);
    switch (direction) {
    case GUIDIR_UP:
        axis.x = 0.0f;
        axis.y = -1.0f;
        side = (float)fabs(offset.x);
        break;
    case GUIDIR_DOWN:
        axis.x = 0.0f;
        axis.y = 1.0f;
        side = (float)fabs(offset.x);
        break;
    case GUIDIR_LEFT:
        axis.y = 0.0f;
        axis.x = -1.0f;
        side = (float)fabs(offset.y);
        break;
    case GUIDIR_RIGHT:
        axis.y = 0.0f;
        axis.x = 1.0f;
        side = (float)fabs(offset.y);
        break;
    }
    V2 normal = offset.Normalized();
    float dot = normal.y * axis.y + normal.x * axis.x;
    if (dot > 0.866f) {
        *distance = offset.Length();
        return true;
    }
    if (dot < -0.98f) {
        *distance = side - offset.Length();
        return true;
    }
    *distance = offset.Length();
    return false;
}

GUICONTROL *GUI::FindNext(const int &direction)
{
    GUICONTROL *ahead = 0;
    float aheadDistance = 10000000.0f;
    GUICONTROL *behind = 0;
    float behindDistance = 0.0f;
    GUICONTROL *other = 0;
    float otherDistance = 10000000.0f;
    V2 mouse(g_pInput->m_mouse.x, g_pInput->m_mouse.y);
    for (int i = 0; i < m_nextFree && i < GUI_MAX_CONTROLS; i++) {
        if (m_controls[i] && IsSelectableElsewhere(m_controls[i], mouse)) {
            float distance;
            if (GetDistance(direction, mouse, m_controls[i], &distance)) {
                if (distance >= 0.0f) {
                    if (distance < aheadDistance) {
                        aheadDistance = distance;
                        ahead = m_controls[i];
                    }
                } else if (distance < behindDistance) {
                    behindDistance = distance;
                    behind = m_controls[i];
                }
            } else if (distance < otherDistance) {
                otherDistance = distance;
                other = m_controls[i];
            }
        }
    }
    if (ahead)
        return ahead;
    if (behind)
        return behind;
    return other;
}

void GUI::SetSound(const char *name)
{
    if (name) {
        m_sound.Set(name);
        m_hasSound = true;
    } else
        m_hasSound = false;
}

SRESULT GUI::BlackZSort(CMDCALL *call)
{
    SetBlackZSort(atoi(call->tokens->GetToken(1)));
    return SRESULT(true);
}

SRESULT GUI::BlackOpacity(CMDCALL *call)
{
    SetBlackOpacity((float)atof(call->tokens->GetToken(1)));
    return SRESULT(true);
}

SRESULT GUI::BlackFade(CMDCALL *call)
{
    FadeBlack((float)atof(call->tokens->GetToken(1)), (float)atof(call->tokens->GetToken(2)),
        (float)atof(call->tokens->GetToken(3)));
    return SRESULT(true);
}

bool GUI::MoveSelection(const int &direction)
{
    GUICONTROL *next = FindNext(direction);
    if (next)
        return MoveMouseTo(next);
    return false;
}

bool GUI::OnKeyDown(unsigned int key)
{
    if (m_inputLock > 0)
        return false;
    switch (key) {
    case VK_LEFT:
        return MoveSelection(GUIDIR_LEFT);
    case VK_RIGHT:
        return MoveSelection(GUIDIR_RIGHT);
    case VK_UP:
        return MoveSelection(GUIDIR_UP);
    case VK_DOWN:
        return MoveSelection(GUIDIR_DOWN);
    case VK_RETURN:
    case VK_SPACE:
        return ClickSelected();
    }
    return false;
}

GUIGROUP *GUI::AddGroup(const char *name, const char *groupName, const V2 &position, const V2 &size,
    const float &opacity, const int &zsort)
{
    FIXEDSTRING<0x200> templateName;
    SafeSprintf(templateName, "GGRP:%s", name);
    GUIGROUP *group = g_pGui->AddGroup(templateName);
    if (!group) {
        if (g_pLog)
            g_pLog->Error("Error - GUI::AddGroup could not find guigroup template:%s", (char *)templateName);
        return 0;
    }
    group->SetZSort(zsort);
    group->SetName(CSTRING(groupName));
    group->SetSize(size);
    group->SetOpacity(opacity);
    group->SetPosition(position);
    return group;
}

void GUI::DeleteClosedScreens()
{
    for (std::set<GUIGROUP *>::iterator it = m_closedScreens.begin(); it != m_closedScreens.end(); ++it) {
        if (*it)
            delete *it;
    }
    m_closedScreens.clear();
}

void GUI::Update()
{
    DECAL *black = (DECAL *)g_pResources->GetResource(m_black);
    if (m_fadeTime > 0.0f && black) {
        float opacity = black->GetAlpha();
        float next = (m_fadeTarget - opacity) / m_fadeTime * g_pTimer->m_dt + opacity;
        m_fadeTime -= g_pTimer->m_dt;
        if (m_fadeTime <= 0.0f)
            SetBlackOpacity(m_fadeTarget);
        else
            SetBlackOpacity(next);
    }
    SetFps(g_pDisplay->GetFps());
    if (m_screen) {
        m_screen->Update();
        if (m_screen->m_closed)
            CloseScreen();
    }
    if (m_playClick) {
        RESOURCE *sound;
        if (m_hasSound)
            sound = g_pSOUND->GetResource(m_sound);
        else
            sound = g_pSOUND->GetResource("menu_click");
        if (sound) {
            ((OBJECT *)sound)->SetPosition(g_pCamera->GetWorldPosition());
            sound->SetActive(true);
        }
        m_playClick = false;
    }
    if (m_playMouseOver) {
        RESOURCE *sound;
        if (m_hasSound)
            sound = g_pSOUND->GetResource(m_sound);
        else
            sound = g_pSOUND->GetResource("menu_mouseover");
        if (sound) {
            ((OBJECT *)sound)->SetPosition(g_pCamera->GetWorldPosition());
            sound->SetActive(true);
        }
        m_playMouseOver = false;
    }
    m_hasSound = false;
    if (g_pDisplay->NeedsAspectFix() != m_aspectFix) {
        ReloadScreen();
        m_aspectFix = g_pDisplay->NeedsAspectFix();
    }
    if (g_pApp->m_pOptions->m_fullscreen != m_fullscreen) {
        OnFullscreenChanged();
        m_fullscreen = g_pApp->m_pOptions->m_fullscreen;
    }
    DeleteClosedScreens();
    OnUpdate();
}

void GUI::OpenScreen(RESHANDLE master)
{
    CloseScreen();
    ((CAMERA *)g_pCamera)->StopJitter();
    m_openPriority = g_pInput->m_topPriority + 1;
    m_screen = new GUIGROUP(master, 0);
    m_screen->SetPosition(V2(0.5f, 0.5f));
    m_screen->SetSize(V2(1.0f, 1.0f));
    m_screen->SetOpacity(1.0f);
    m_screen->SetZSort(GetScreenZSort());
    m_screen->Virtual25(true);
    m_screen->Virtual26(true);
    OnScreenOpened();
}

void GUI::SetScreen(const FIXEDSTRING<0x200> &name)
{
    if (!g_pResources->GetResource(m_black)) {
        DECAL *black = (DECAL *)g_pResources->GetResource("DECL:black_screen");
        if (black) {
            black->SetLayer(0x7e);
            black->SetSize(V2(2.0f, 1.0f));
            black->m_screenPosition.x = 0.5f;
            black->m_screenPosition.y = 0.5f;
            black->m_screenSpace = true;
            black->SetAlpha(0.0f);
            black->SetActive(true);
            m_black = black->m_handle;
        } else if (g_pLog)
            g_pLog->Error("Could not create black decal");
    }
    CloseScreen();
    m_screenName = name;
    RESHANDLE handle;
    if (g_pDisplay->NeedsAspectFix()) {
        FIXEDSTRING<0x200> wide = name;
        wide += "_WideScreen";
        handle = g_pGUIGROUP->GetHandle(wide);
    }
    if (!handle.IsValid())
        handle = g_pGUIGROUP->GetHandle(m_screenName);
    if (handle.IsValid()) {
        OpenScreen(handle);
        return;
    }
    if ((int)strlen(m_screenName) > 0 && g_pLog)
        g_pLog->Error("Set screen could not find group template: %s", (char *)m_screenName);
}

GUI::~GUI()
{
    SetScreen("");
    RESOURCE *black = g_pResources->GetResource(m_black);
    if (black)
        black->Virtual20();
    m_black.Invalidate();
    DeleteClosedScreens();
}

GUI::GUI()
{
    m_screen = 0;
    m_fadeTime = 0;
    m_screenName.Set("");
    m_fps = 0;
    m_loadingProgress = 0;
    m_versionString = L"";
    for (int i = 0; i < GUI_MAX_CONTROLS; i++) {
        m_controls[i] = 0;
        m_freeList[i] = 0;
    }
    m_serial = 0;
    m_count = 0;
    m_numFree = 0;
    m_nextFree = 0;
    m_playMouseOver = false;
    m_playClick = false;
    m_hasSound = false;
    SetName("gui");
    SetPriority(-1);
    m_inputLock = 0;
    m_aspectFix = g_pDisplay->NeedsAspectFix();
    m_fullscreen = g_pApp->m_pOptions->m_fullscreen;
}

GUI *CreateGui()
{
    return new GUI;
}
