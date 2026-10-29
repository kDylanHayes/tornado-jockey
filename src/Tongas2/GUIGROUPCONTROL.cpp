// GUIGROUP, the GUI's screens and panels: the controls they are made of,
// placed, sized and faded with them, and their mouse area. The original
// file name is not known; the masters are in GUIGROUP.cpp.
#define LOG_VALUE

#include "Tongas2/GUIGROUP.h"
#include "Tongas2/APPWINDOW.h"
#include "Tongas2/CMDMGR.h"
#include "Tongas2/FIXEDSTRING.h"
#include "Tongas2/GUI.h"
#include "Tongas2/GUICHECKBOX.h"
#include "Tongas2/GUICHECKGROUP.h"
#include "Tongas2/GUIGENERIC.h"
#include "Tongas2/GUISLIDER.h"
#include "Tongas2/GUITEXT.h"
#include "Tongas2/GUITEXTEDIT.h"
#include "Tongas2/GUIVISUAL.h"
#include "Tongas2/LOG.h"
#include "Tongas2/TOKENS.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CMD_FUNC(CLASS, method) (CMDFUNC)(SRESULT (CLASS::*)(TOKENS **))(SRESULT (CLASS::*)(CMDCALL *))&CLASS::method

extern RESOURCEMANAGER *g_pGUIGROUP;

static GUICONTROL *CreateGUICONTROL(const RESHANDLE &master, GUIGROUP *parent)
{
    return new GUICONTROL(master, parent);
}

static GUICONTROL *CreateGUICHECKGROUP(const RESHANDLE &master, GUIGROUP *parent)
{
    return new GUICHECKGROUP(master, parent);
}

static GUICONTROL *CreateGUICHECKBOX(const RESHANDLE &master, GUIGROUP *parent)
{
    return new GUICHECKBOX(master, parent);
}

static GUICONTROL *CreateGUITEXT(const RESHANDLE &master, GUIGROUP *parent)
{
    return new GUITEXT(master, parent);
}

static GUICONTROL *CreateGUITEXTEDIT(const RESHANDLE &master, GUIGROUP *parent)
{
    return new GUITEXTEDIT(master, parent);
}

static GUICONTROL *CreateGUIVISUAL(const RESHANDLE &master, GUIGROUP *parent)
{
    return new GUIVISUAL(master, parent);
}

static GUICONTROL *CreateGUIGENERIC(const RESHANDLE &master, GUIGROUP *parent)
{
    return new GUIGENERIC(master, parent);
}

static GUICONTROL *CreateGUISLIDER(const RESHANDLE &master, GUIGROUP *parent)
{
    return new GUISLIDER(master, parent);
}

static GUICONTROL *CreateGUIGROUPCONTROL(const RESHANDLE &master, GUIGROUP *parent);

// The control each GUI resource type makes, in the order of the types,
// GUIGROUP last.
static GUICONTROL *(*s_createControl[])(const RESHANDLE &master, GUIGROUP *parent) = {
    CreateGUICONTROL,
    CreateGUIGENERIC,
    CreateGUITEXT,
    CreateGUITEXTEDIT,
    CreateGUIVISUAL,
    CreateGUICHECKBOX,
    CreateGUICHECKGROUP,
    CreateGUISLIDER,
    CreateGUIGROUPCONTROL,
};

CMDENTRY GUIGROUP::s_commands[] = {
    { "addControl", 0, 4, 4, CMD_FUNC(GUIGROUP, AddControlCommand) },
    { "replaceControl", 0, 2, 2, CMD_FUNC(GUIGROUP, ReplaceControl) },
    { "reloadControl", 0, 1, 1, CMD_FUNC(GUIGROUP, ReloadControl) },
    { "click", 0, 0, 0, CMD_FUNC(GUIGROUP, ClickCommand) },
    { "MouseOver", 0, 0, 0, CMD_FUNC(GUIGROUP, MouseOverCommand) },
    { "MouseOut", 0, 0, 0, CMD_FUNC(GUIGROUP, MouseOutCommand) },
    { "RunCmd", 0, 1, 1, CMD_FUNC(GUIGROUP, RunCmd) },
    { "EvalCmd", 0, 1, 1, CMD_FUNC(GUIGROUP, EvalCmd) },
    { "SetOnTopPriority", 0, 0, 0, CMD_FUNC(GUIGROUP, SetOnTopPriority) },
    { "DumpControls", 0, 0, 0, CMD_FUNC(GUIGROUP, DumpControlsCommand) },
    { "OverrideControl", 0, 2, 2, CMD_FUNC(GUIGROUP, OverrideControl) },
    { 0, 0, 0, 0, 0 },
};

PROPENTRY GUIGROUP::s_properties[] = {
    { 0, 0, 15, 0, 0, false },
};

void GUIGROUP::RegisterCommands()
{
    g_classParents[s_classId] = GUICONTROL::s_classId;
    g_classCommands[s_classId] = s_commands;
    HashCommands(s_commands);
    RegisterClassName(s_commands, "GUIGROUP", "GUICONTROL");
}

void GUIGROUP::CloseSlot(int slot)
{
    if (m_controls[slot])
        m_controls[slot]->Close();
}

GUICONTROL *GUIGROUP::AddControl(int slot, const RESHANDLE &master)
{
    if (!master.IsValid())
        return 0;
    if (slot < 0 || slot >= GUIGROUP_MAX_CONTROLS) {
        if (g_pLog)
            g_pLog->Error("GUIGROUP::AddControl - Invalid slot");
        return 0;
    }
    RESOURCEMASTER *resource = g_pResources->GetMaster(master);
    if (!resource) {
        if (g_pLog)
            g_pLog->Error("GUIGROUP::AddControl - Tried to create invalid control");
        return 0;
    }
    GUICONTROL *control = s_createControl[resource->m_manager->m_id - g_pGUIGROUP->m_id + 8](master, this);
    if (!control) {
        if (g_pLog)
            g_pLog->Error("GUIGROUP::AddControl - Tried to create invalid control!");
        return 0;
    }
    if (m_controls[slot])
        CloseSlot(slot);
    m_controls[slot] = control;
    if (slot >= m_controlCount)
        m_controlCount = slot + 1;
    return control;
}

void GUIGROUP::SetPosition(const V2 &position)
{
    m_position.x = position.x;
    m_position.y = position.y;
    for (int i = 0; i < m_controlCount; i++) {
        if (m_controls[i] && !m_controls[i]->m_closed) {
            if (m_controls[i]->m_slot >= 0)
                m_controls[i]->SetPosition(m_master->m_controlPos[m_controls[i]->m_slot] + position);
            else
                m_controls[i]->SetPosition(position);
        }
    }
}

void GUIGROUP::SetSize(const V2 &size)
{
    m_size.x = size.x;
    m_size.y = size.y;
    for (int i = 0; i < m_controlCount; i++) {
        if (m_controls[i] && !m_controls[i]->m_closed) {
            if (m_controls[i]->m_slot >= 0)
                m_controls[i]->SetSize(m_master->m_controlSize[m_controls[i]->m_slot] * size);
            else
                m_controls[i]->SetSize(size);
        }
    }
}

void GUIGROUP::SetZSort(const int &zsort)
{
    m_zsort = zsort;
    for (int i = 0; i < m_controlCount; i++) {
        if (m_controls[i] && !m_controls[i]->m_closed) {
            if (m_controls[i]->m_slot >= 0)
                m_controls[i]->SetZSort(m_master->m_controlZSort[m_controls[i]->m_slot] + zsort);
            else
                m_controls[i]->SetZSort(zsort);
        }
    }
}

void GUIGROUP::SetOpacity(const float &opacity)
{
    m_opacity = opacity;
    float shown = ((m_enabled + 1.0f) * 0.5f) * m_visible * opacity;
    for (int i = 0; i < m_controlCount; i++) {
        if (m_controls[i] && !m_controls[i]->m_closed) {
            if (m_controls[i]->m_slot >= 0)
                m_controls[i]->SetOpacity(shown * m_master->m_controlOpacity[m_controls[i]->m_slot]);
            else
                m_controls[i]->SetOpacity(shown);
        }
    }
}

void *GUIGROUP::Find(const char *name)
{
    for (int i = 0; i < m_controlCount; i++) {
        GUICONTROL *control = m_controls[i];
        if (control && !control->m_closed && !stricmp(control->m_name.c_str(), name))
            return m_controls[i];
    }
    return 0;
}

GUICONTROL *GUIGROUP::FindControl(const char *name)
{
    for (int i = 0; i < m_controlCount; i++) {
        GUICONTROL *control = m_controls[i];
        if (control && !control->m_closed) {
            if (!stricmp(control->m_name.c_str(), name))
                return m_controls[i];
            if (m_controls[i]->GetClassId() == s_classId) {
                GUICONTROL *found = ((GUIGROUP *)m_controls[i])->FindControl(name);
                if (found)
                    return found;
            }
        }
    }
    return 0;
}

int GUIGROUP::GetMouseZSort(int min)
{
    if (m_master->m_fullScreenInputBlocker && m_topZSort > min)
        min = m_topZSort;
    for (int i = 0; i < m_controlCount; i++) {
        GUICONTROL *control = m_controls[i];
        if (control && !control->m_closed && control->GetClassId() == s_classId
            && m_controls[i]->IsEnabled() && m_controls[i]->IsVisible()
            && ((GUIGROUP *)m_controls[i])->m_master->m_fullScreenInputBlocker) {
            int zsort = ((GUIGROUP *)m_controls[i])->GetMouseZSort(min);
            if (zsort > min)
                min = zsort;
        }
    }
    return min;
}

void GUIGROUP::GetMouseArea(float *left, float *top, float *right, float *bottom)
{
    V2 half = m_size * m_master->m_mouseAreaSize * 0.5f;
    V2 offset = m_master->m_mouseAreaOffset;
    *left = m_position.x - half.x + offset.x;
    *right = offset.x + m_position.x + half.x;
    *top = m_position.y - half.y + offset.y;
    *bottom = offset.y + m_position.y + half.y;
}

bool GUIGROUP::IsMouseOver(float x, float y)
{
    if (!IsVisible())
        return false;
    float left, right, top, bottom;
    GetMouseArea(&left, &top, &right, &bottom);
    return x >= left && x <= right && y >= top && y <= bottom;
}

bool GUIGROUP::OnLeftUp(MOUSESTATE state)
{
    if (IsEnabled() && IsVisible() && m_master->m_fullScreenInputBlocker)
        return true;
    if (IsMouseOver(state.x, state.y) && IsActive()) {
        if (m_pressed) {
            m_pressed = false;
            Click();
        }
        return true;
    }
    return false;
}

bool GUIGROUP::OnLeftDown(MOUSESTATE state)
{
    m_pressed = false;
    if (IsEnabled() && IsVisible() && m_master->m_fullScreenInputBlocker)
        return true;
    if (IsMouseOver(state.x, state.y) && IsActive()) {
        m_pressed = true;
        return true;
    }
    return false;
}

bool GUIGROUP::OnLeftDrag(MOUSESTATE state)
{
    if (IsEnabled() && IsVisible() && m_master->m_fullScreenInputBlocker)
        return true;
    if (IsMouseOver(state.x, state.y) && IsActive())
        return true;
    return false;
}

void GUIGROUP::Click()
{
    if (IsEnabled() && IsVisible()) {
        g_pGui->PlayClick();
        if (m_master && m_master->m_onMouseClickSound.Length() > 0)
            g_pGui->SetSound(m_master->m_onMouseClickSound.c_str());
        else
            g_pGui->SetSound(0);
        g_pCmdMgr->Run(m_master->m_onClick, m_handle);
    }
}

bool GUIGROUP::OnMouseMove(MOUSESTATE state)
{
    if (IsEnabled() && IsVisible() && m_master->m_fullScreenInputBlocker)
        return true;
    if ((!IsEnabled() && !m_master->m_mouseOverOutWhenDisable) || !IsVisible())
        return false;
    if (m_master->m_onMouseOver.Length() <= 0 && m_master->m_onMouseOut.Length() <= 0)
        return false;
    bool over = IsMouseOver(state.x, state.y);
    if (over) {
        if (!m_mouseOver) {
            g_pGui->PlayMouseOver();
            g_pCmdMgr->Run(m_master->m_onMouseOver, m_handle);
            if (m_master && m_master->m_onMouseOverSound.Length() > 0)
                g_pGui->SetSound(m_master->m_onMouseOverSound.c_str());
            else
                g_pGui->SetSound(0);
        }
    } else if (m_mouseOver) {
        g_pCmdMgr->Run(m_master->m_onMouseOut, m_handle);
    }
    m_mouseOver = over;
    return m_master->m_passMouseOverToFather ? false : over;
}

void GUIGROUP::UpdateLayout()
{
    for (int i = 0; i < m_controlCount; i++) {
        GUICONTROL *control = m_controls[i];
        if (control && !control->m_closed) {
            if (control->Virtual19() == 1)
                m_controls[i]->SetPosition(m_controls[i]->m_position);
            else if (m_controls[i]->Virtual19() == 8)
                ((GUIGROUP *)m_controls[i])->UpdateLayout();
        }
    }
}

bool GUIGROUP::IsActive()
{
    return m_master->m_onClick.Length() > 0;
}

void GUIGROUP::SetTopZSortAll(int zsort)
{
    SetTopZSort(zsort);
    for (int i = 0; i < m_controlCount; i++) {
        GUICONTROL *control = m_controls[i];
        if (control && !control->m_closed) {
            if (control->GetClassId() == s_classId)
                ((GUIGROUP *)m_controls[i])->SetTopZSortAll(zsort + 1);
            else
                m_controls[i]->SetTopZSort(zsort + 1);
        }
    }
}

void GUIGROUP::SetTopZSort(int zsort)
{
    m_topZSort = zsort;
    if (m_master->m_mouseAreaSize.x == 0.0f && m_master->m_mouseAreaSize.y == 0.0f
        && !m_master->m_fullScreenInputBlocker)
        Unregister();
    else
        MOUSELISTENER::SetPriority(zsort);
}

int GUIGROUP::GetTopZSort()
{
    int top = m_topZSort;
    for (int i = 0; i < m_controlCount; i++) {
        GUICONTROL *control = m_controls[i];
        if (control && !control->m_closed) {
            int zsort = control->GetTopZSort();
            if (zsort > top)
                top = zsort;
        }
    }
    return top;
}

void GUIGROUP::FindTopAt(V2 point, GUICONTROL **hit, int *zsort)
{
    if (m_topZSort >= *zsort && IsMouseOver(point.x, point.y)) {
        *zsort = m_topZSort;
        *hit = this;
    }
    for (int i = 0; i < m_controlCount; i++) {
        GUICONTROL *control = m_controls[i];
        if (control && !control->m_closed && control->IsVisible() && m_controls[i]->m_opacity > 0.0f)
            m_controls[i]->FindTopAt(point, hit, zsort);
    }
}

void GUIGROUP::Virtual34(V2 point, GUICONTROL **hit, int *zsort)
{
    for (int i = 0; i < m_controlCount; i++) {
        GUICONTROL *control = m_controls[i];
        if (control && !control->m_closed && control->IsVisible() && m_controls[i]->m_opacity > 0.0f)
            m_controls[i]->Virtual34(point, hit, zsort);
    }
}

GUICONTROL *GUIGROUP::AddControl(const RESHANDLE &master)
{
    if (!master.IsValid())
        return 0;
    for (int i = 0; i < m_controlCount; i++) {
        if (!m_controls[i])
            return AddControl(i, master);
    }
    if (m_controlCount == GUIGROUP_MAX_CONTROLS) {
        if (g_pLog)
            g_pLog->Error("GUIGROUP::AddControl - Max controls reached!");
        return 0;
    }
    return AddControl(m_controlCount, master);
}

SRESULT GUIGROUP::OverrideControl(CMDCALL *call)
{
    int slot = atoi(call->tokens->GetToken(1));
    if (slot >= 0 && slot < GUIGROUP_MAX_CONTROLS) {
        if (AddControl(slot, g_pResources->GetHandle(call->tokens->GetToken(2))))
            return SRESULT(true);
        return SRESULT(false);
    }
    if (g_pLog)
        g_pLog->Error("GUIGROUP::OverrideControl - Invalid Slot");
    return SRESULT(false);
}

SRESULT GUIGROUP::ReloadControl(CMDCALL *call)
{
    int i;
    for (i = 0; i < m_controlCount; i++) {
        GUICONTROL *control = m_controls[i];
        if (control && !control->m_closed && !strcmp(control->m_name.c_str(), call->tokens->GetToken(1)))
            break;
    }
    if (i == m_controlCount) {
        if (g_pLog)
            g_pLog->Error("Could not find control %s that should be replaced", call->tokens->GetToken(1));
        return SRESULT(false);
    }
    GUICONTROL *control = AddControl(m_controls[i]->m_master);
    if (!control) {
        if (g_pLog)
            g_pLog->Error("Could not add control to reload %s", call->tokens->GetToken(1));
        return SRESULT(false);
    }
    m_controls[i]->CopyProperties(control, GUICONTROL::s_classId);
    CloseSlot(i);
    return SRESULT(true);
}

SRESULT GUIGROUP::ClickCommand(CMDCALL *call)
{
    Click();
    return SRESULT(true);
}

SRESULT GUIGROUP::MouseOverCommand(CMDCALL *call)
{
    if (!m_mouseOver) {
        g_pGui->PlayMouseOver();
        g_pGui->SetSound(m_master && m_master->m_onMouseOverSound.Length() > 0 ? m_master->m_onMouseOverSound.c_str() : 0);
        g_pCmdMgr->Run(m_master->m_onMouseOver, m_handle);
        m_mouseOver = true;
    }
    return SRESULT(true);
}

SRESULT GUIGROUP::MouseOutCommand(CMDCALL *call)
{
    if (m_mouseOver) {
        g_pCmdMgr->Run(m_master->m_onMouseOut, m_handle);
        m_mouseOver = false;
    }
    return SRESULT(true);
}

SRESULT GUIGROUP::SetOnTopPriority(CMDCALL *call)
{
    SetTopZSortAll(g_pGui->GetTopZSort() + 1);
    return SRESULT(true);
}

void GUIGROUP::DumpControls(int depth)
{
    FIXEDSTRING<0x200> indent;
    for (int i = 0; i < depth; i++)
        indent += " ";
    if (g_pLog)
        g_pLog->Value("%s%s", (char *)indent, g_classNames[GetClassId()]);
    indent += " ";
    for (int j = 0; j < m_controlCount; j++) {
        GUICONTROL *control = m_controls[j];
        if (control && !control->m_closed) {
            if (control->GetClassId() == s_classId)
                ((GUIGROUP *)m_controls[j])->DumpControls(depth + 1);
            else if (g_pLog)
                g_pLog->Value("%s%s", (char *)indent, g_classNames[m_controls[j]->GetClassId()]);
        }
    }
}

GUIGROUP *GUIGROUP::AddGroup(const char *name)
{
    if (g_pDisplay->NeedsAspectFix()) {
        FIXEDSTRING<0x200> wideName(name);
        wideName += "_WideScreen";
        RESHANDLE handle = g_pResources->GetHandle(wideName);
        if (handle.IsValid())
            return (GUIGROUP *)AddControl(handle);
    }
    return (GUIGROUP *)AddControl(g_pResources->GetHandle(name));
}

SRESULT GUIGROUP::DumpControlsCommand(CMDCALL *call)
{
    DumpControls(0);
    return SRESULT(true);
}

SRESULT GUIGROUP::ReplaceControl(CMDCALL *call)
{
    int i;
    for (i = 0; i < m_controlCount; i++) {
        GUICONTROL *control = m_controls[i];
        if (control && !control->m_closed && !strcmp(control->m_name.c_str(), call->tokens->GetToken(1)))
            break;
    }
    if (i == m_controlCount) {
        if (g_pLog)
            g_pLog->Error("Could not find control %s that should be replaced", call->tokens->GetToken(1));
        return SRESULT(false);
    }
    GUICONTROL *control = AddGroup(call->tokens->GetToken(2));
    if (!control) {
        if (g_pLog)
            g_pLog->Error("Could not add control to replace %s %s", call->tokens->GetToken(1), call->tokens->GetToken(2));
        return SRESULT(false);
    }
    m_controls[i]->CopyProperties(control, GUICONTROL::s_classId);
    CloseSlot(i);
    return SRESULT(true);
}

SRESULT GUIGROUP::AddControlCommand(CMDCALL *call)
{
    GUICONTROL *control = AddGroup(call->tokens->GetToken(2));
    if (!control) {
        if (g_pLog)
            g_pLog->Error("Could not add control %s %s %s", call->tokens->GetToken(1), call->tokens->GetToken(2),
                          call->tokens->GetToken(3));
        return SRESULT(false);
    }
    control->SetName(CSTRING(call->tokens->GetToken(1)));
    GUIGROUPV2 position;
    sscanf(call->tokens->GetToken(3), "%f %f", &position.x, &position.y);
    control->SetSize(V2(1.0f, 1.0f));
    control->SetZSort(atoi(call->tokens->GetToken(4)) + m_zsort);
    control->SetOpacity(1.0f);
    control->SetPosition(V2(position.x + m_position.x, position.y + m_position.y));
    return SRESULT(true);
}

bool GUIGROUP::RunGroupCommand(int i)
{
    if (i >= 0 && i < GUIGROUP_MAX_COMMANDS && IsEnabled() && IsVisible() && m_master->m_groupCommands[i].Length() >= 1) {
        g_pCmdMgr->Run(STRING(m_master->m_groupCommands[i].c_str()), m_handle);
        return true;
    }
    return false;
}

SRESULT GUIGROUP::EvalGroupCommand(int i)
{
    if (i >= 0 && i < GUIGROUP_MAX_COMMANDS) {
        if (IsEnabled() && IsVisible() && m_master->m_groupCommands[i].Length() >= 1)
            return g_pCmdMgr->Evaluate(STRING(m_master->m_groupCommands[i].c_str()), m_handle);
        return SRESULT(false);
    }
    return SRESULT(false);
}

void GUIGROUP::DeleteRemoved()
{
    if (m_removed.size()) {
        for (std::vector<GUICONTROL *>::iterator it = m_removed.begin(); it != m_removed.end(); ++it) {
            if (*it)
                delete *it;
        }
        m_removed.clear();
    }
}

inline void GUIGROUP_Activate(GUIGROUP *group)
{
    if (!group->m_activated) {
        group->m_activated = true;
        if (group->m_master->m_onActivateCommand.Length())
            g_pCmdMgr->Run(group->m_master->m_onActivateCommand, group->m_handle);
    }
}

void GUIGROUP::Update()
{
    GUIGROUP_Activate(this);
    GUICONTROL::Update();
    for (int i = 0; i < m_controlCount; i++) {
        GUICONTROL *control = m_controls[i];
        if (control && !control->m_closed)
            control->Update();
    }
    DeleteRemoved();
    if (m_hasStepCommand)
        g_pCmdMgr->Run(STRING(m_master->m_onStepCommand.c_str()), m_handle);
}

SRESULT GUIGROUP::RunCmd(CMDCALL *call)
{
    return SRESULT(RunGroupCommand(atoi(call->tokens->GetToken(1))));
}

SRESULT GUIGROUP::EvalCmd(CMDCALL *call)
{
    return EvalGroupCommand(atoi(call->tokens->GetToken(1)));
}

GUIGROUP::~GUIGROUP()
{
    GUIGROUP_Activate(this);
    if (m_master->m_onDeactivateCommand.Length())
        g_pCmdMgr->Run(m_master->m_onDeactivateCommand, m_handle);
    for (int i = 0; i < m_controlCount; i++) {
        if (m_controls[i]) {
            delete m_controls[i];
            m_controls[i] = 0;
        }
    }
    DeleteRemoved();
}

GUIGROUP::GUIGROUP(const RESHANDLE &master, GUIGROUP *parent) : GUICONTROL(master, parent)
{
    m_master = 0;
    m_controlCount = 0;
    m_hasStepCommand = false;
    for (int i = 0; i < GUIGROUP_MAX_CONTROLS; i++)
        m_controls[i] = 0;
    m_master = (GUIGROUPMASTER *)g_pResources->GetMaster(master);
    if (!m_master && g_pLog)
        g_pLog->Error("Gui group has no master");
    SetTopZSort(parent ? parent->m_topZSort + 1 : g_pGui->m_openPriority);
    if (m_master->m_randomResource) {
        int count = 0;
        int i;
        for (i = 0; i < GUIGROUP_MAX_CONTROLS; i++) {
            if (m_master->m_controls[i].Length() > 0)
                count++;
        }
        int pick = count - 1 > 0 ? rand() % count : 0;
        int n = 0;
        for (i = 0; i < GUIGROUP_MAX_CONTROLS; i++) {
            if (m_master->m_controls[i].Length() > 0) {
                if (n == pick) {
                    GUICONTROL *control = AddGroup(m_master->m_controls[i].c_str());
                    if (control) {
                        control->SetEnabled(m_master->m_controlEnabled[i]);
                        control->SetVisible(m_master->m_controlVisible[i]);
                        control->SetName(m_master->m_controlNames[i]);
                        control->SetBehaviorParams(m_master->m_behaviorParams[i]);
                        control->SetBehavior(m_master->m_behaviors[i]);
                        control->m_slot = i;
                    }
                }
                n++;
            }
        }
    } else {
        for (int i = 0; i < GUIGROUP_MAX_CONTROLS; i++) {
            if (m_master->m_controls[i].Length() > 0) {
                GUICONTROL *control = AddGroup(m_master->m_controls[i].c_str());
                if (control) {
                    control->SetEnabled(m_master->m_controlEnabled[i]);
                    control->SetVisible(m_master->m_controlVisible[i]);
                    control->SetName(m_master->m_controlNames[i]);
                    control->SetBehaviorParams(m_master->m_behaviorParams[i]);
                    control->SetBehavior(m_master->m_behaviors[i]);
                    control->m_slot = i;
                } else if (g_pLog) {
                    g_pLog->Error("Control not found %s", m_master->m_controls[i].c_str());
                }
            }
        }
    }
    m_activated = false;
    m_mouseOver = false;
    m_pressed = false;
    if (m_master->m_setOnTopPriority)
        SetTopZSortAll(g_pGui->GetTopZSort() + 1);
    if (m_master && m_master->m_onStepCommand.Length() > 0)
        m_hasStepCommand = true;
    if (m_master)
        m_timeSpace = m_master->m_timeSpace;
}

static GUICONTROL *CreateGUIGROUPCONTROL(const RESHANDLE &master, GUIGROUP *parent)
{
    return new GUIGROUP(master, parent);
}

void GUIGROUP::RemoveControl(GUICONTROL *control)
{
    if (control) {
        for (int i = 0; i < GUIGROUP_MAX_CONTROLS; i++) {
            if (m_controls[i] == control) {
                m_controls[i] = 0;
                m_removed.push_back(control);
            }
        }
    }
}
