#include "Tongas2/GUICONTROL.h"
#include "Tongas2/GUI.h"
#include "Tongas2/GUIGROUP.h"
#include "Tongas2/TIMER.h"
#include "Tongas2/TOKENS.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CMD_FUNC(CLASS, method) (CMDFUNC)(SRESULT (CLASS::*)(TOKENS **))(SRESULT (CLASS::*)(CMDCALL *))&CLASS::method

static const char s_behaviors[][30] = {
    "NONE",
    "JITTER",
    "FADE",
    "SIZE",
    "BLINK",
    "DIEFADE",
    "",
};

static const char s_timeSpaces[][30] = {
    "Game",
    "Real",
    "",
};

CMDENTRY GUICONTROL::s_commands[] = {
    { "move", 0, 2, 2, CMD_FUNC(GUICONTROL, Move) },
    { "moverel", 0, 2, 2, CMD_FUNC(GUICONTROL, MoveRel) },
    { "moveAndDie", 0, 2, 2, CMD_FUNC(GUICONTROL, MoveAndDie) },
    { "fade", 0, 2, 2, CMD_FUNC(GUICONTROL, Fade) },
    { "gparent", 0, 1, 1, CMD_FUNC(GUICONTROL, GParent) },
    { "Close", 0, 0, 0, CMD_FUNC(GUICONTROL, CloseCommand) },
    { "QueueBh2", 0, 2, 2, CMD_FUNC(GUICONTROL, QueueBehaviorCommand) },
    { 0, 0, 0, 0, 0 },
};

PROPENTRY GUICONTROL::s_properties[] = {
    { "TimeSpace", 0, 1, s_timeSpaces, PROP_FIELD(GUICONTROL, m_timeSpace), false, 1, PROP_SETTER(GUICONTROL, SetTimeSpace) },
    { "ScreenPos", 0, 10, 0, PROP_FIELD(GUICONTROL, m_position), false, 1, PROP_SETTER(GUICONTROL, SetPosition) },
    { "Size", 0, 10, 0, PROP_FIELD(GUICONTROL, m_size), false, 1, PROP_SETTER(GUICONTROL, SetSize) },
    { "ZSort", 0, 0, 0, PROP_FIELD(GUICONTROL, m_zsort), false, 1, PROP_SETTER(GUICONTROL, SetZSort) },
    { "Visible", 0, 2, 0, PROP_FIELD(GUICONTROL, m_visible), false, 1, PROP_SETTER(GUICONTROL, SetVisible) },
    { "Enabled", 0, 2, 0, PROP_FIELD(GUICONTROL, m_enabled), false, 1, PROP_SETTER(GUICONTROL, SetEnabled) },
    { "Opacity", 0, 3, 0, PROP_FIELD(GUICONTROL, m_opacity), false, 1, PROP_SETTER(GUICONTROL, SetOpacity) },
    { "Behavior", 0, 1, s_behaviors, PROP_FIELD(GUICONTROL, m_behavior), false, 1, PROP_SETTER(GUICONTROL, SetBehavior) },
    { "BehaviorParams", 0, 7, 0, PROP_FIELD(GUICONTROL, m_behaviorParams), false, 1, PROP_SETTER(GUICONTROL, SetBehaviorParams) },
    { 0, 0, 15, 0, 0, false },
};

void GUICONTROL::RegisterCommands()
{
    g_classParents[s_classId] = g_CMDBASEClassId;
    g_classCommands[s_classId] = s_commands;
    HashCommands(s_commands);
    RegisterClassName(s_commands, "GUICONTROL", "CMDBASE");
}

void GUICONTROL::Close()
{
    if (!m_closed) {
        m_closed = true;
        if (m_parent)
            m_parent->RemoveControl(this);
    }
}

void GUICONTROL::Update()
{
    float dt;
    if (m_timeSpace)
        dt = g_pTimer->m_soundDt;
    else
        dt = g_pTimer->m_dt;
    if (m_fadeTime > 0.0f) {
        m_fadeTime -= dt;
        if (m_fadeTime <= 0.0f) {
            if (m_dieWhenDone)
                Close();
            SetOpacity(m_fadeTarget);
        } else {
            m_fadeOpacity += (m_fadeTarget - m_fadeOpacity) / m_fadeTime * dt;
            if (m_fadeOpacity < 0.0f)
                m_fadeOpacity = 0.0f;
            if (m_fadeOpacity > 1.0f)
                m_fadeOpacity = 1.0f;
            SetOpacity(m_fadeOpacity);
        }
    }
    if (m_moveTime > 0.0f) {
        m_moveTime -= dt;
        if (m_moveTime <= 0.0f) {
            SetPosition(m_moveTarget);
            if (m_dieWhenDone)
                Close();
        } else {
            m_movePos += (m_moveTarget - m_movePos) * dt / m_moveTime;
            SetPosition(m_movePos);
        }
    }
    if (m_behaviorObject)
        m_behaviorObject->Update();
    UpdateBehaviors(dt);
}

void GUICONTROL::SetEnabled(const bool &enabled)
{
    m_enabled = enabled;
    SetOpacity(m_opacity);
}

void GUICONTROL::SetVisible(const bool &visible)
{
    m_visible = visible;
    SetOpacity(m_opacity);
}

void GUICONTROL::FadeTo(float opacity, float time)
{
    m_fadeTarget = opacity;
    m_fadeTime = time;
    m_fadeOpacity = m_opacity;
}

void GUICONTROL::MoveTo(const V2 &target, float time, bool die)
{
    m_moveTarget = target;
    m_moveTime = time;
    m_dieWhenDone = die;
    m_movePos = m_position;
}

bool GUICONTROL::IsEnabled()
{
    for (GUICONTROL *control = this; control->m_enabled; control = (GUICONTROL *)control->m_parent) {
        if (!control->m_parent)
            return true;
    }
    return false;
}

bool GUICONTROL::IsVisible()
{
    for (GUICONTROL *control = this; control->m_visible; control = (GUICONTROL *)control->m_parent) {
        if (!control->m_parent)
            return true;
    }
    return false;
}

GUICONTROL::GUICONTROL(const RESHANDLE &master, GUIGROUP *parent)
{
    V2 v(0.0f, 0.0f);
    m_position = v;
    v = V2(1.0f, 1.0f);
    m_timeSpace = 1;
    m_size = v;
    m_zsort = 0;
    m_visible = true;
    m_enabled = true;
    m_opacity = 1.0f;
    m_behavior = 0;
    m_behaviorParams = "";
    m_parent = parent;
    m_slot = -1;
    m_closed = false;
    m_behaviorObject = 0;
    m_master = master;
    m_topZSort = -1;
    m_moveTime = 0.0f;
    m_dieWhenDone = false;
    m_fadeTime = 0.0f;
    g_pGui->AddControl(this);
}

int GUICONTROL::Virtual08()
{
    if (m_parent)
        return (int)m_parent;
    return (int)g_pGui;
}

GUICONTROL::~GUICONTROL()
{
    g_pGui->RemoveControl(this);
    if (m_behaviorObject) {
        delete m_behaviorObject;
        m_behaviorObject = 0;
    }
}

SRESULT GUICONTROL::Move(CMDCALL *call)
{
    sscanf(call->tokens->GetToken(1), "%f %f", &m_moveTarget.x, &m_moveTarget.y);
    m_moveTime = (float)atof(call->tokens->GetToken(2));
    m_dieWhenDone = false;
    m_movePos = m_position;
    return SRESULT(true);
}

SRESULT GUICONTROL::MoveRel(CMDCALL *call)
{
    sscanf(call->tokens->GetToken(1), "%f %f", &m_moveTarget.x, &m_moveTarget.y);
    m_moveTarget += m_position;
    m_moveTime = (float)atof(call->tokens->GetToken(2));
    m_dieWhenDone = false;
    m_movePos = m_position;
    return SRESULT(true);
}

SRESULT GUICONTROL::MoveAndDie(CMDCALL *call)
{
    sscanf(call->tokens->GetToken(1), "%f %f", &m_moveTarget.x, &m_moveTarget.y);
    m_moveTime = (float)atof(call->tokens->GetToken(2));
    m_dieWhenDone = true;
    m_movePos = m_position;
    return SRESULT(true);
}

SRESULT GUICONTROL::Fade(CMDCALL *call)
{
    m_fadeTarget = (float)atof(call->tokens->GetToken(1));
    m_fadeTime = (float)atof(call->tokens->GetToken(2));
    m_fadeOpacity = m_opacity;
    return SRESULT(true);
}

SRESULT GUICONTROL::CloseCommand(CMDCALL *call)
{
    Close();
    return SRESULT(true);
}

SRESULT GUICONTROL::QueueBehaviorCommand(CMDCALL *call)
{
    QueueBehavior(call->tokens->GetToken(1), call->tokens->GetToken(2));
    return SRESULT(true);
}

void GUICONTROL::SetBehavior(const int &behavior)
{
    if (behavior == m_behavior)
        return;
    if (m_behaviorObject) {
        delete m_behaviorObject;
        m_behaviorObject = 0;
    }
    m_behavior = behavior;
    switch (m_behavior) {
    case GUIBEHAVIOR_BLINK:
        m_behaviorObject = new GUIBLINK(this);
        break;
    case GUIBEHAVIOR_JITTER:
        m_behaviorObject = new GUIJITTER(this);
        break;
    case GUIBEHAVIOR_FADE:
        m_behaviorObject = new GUIFADE(this);
        break;
    case GUIBEHAVIOR_SIZE:
        m_behaviorObject = new GUISIZE(this);
        break;
    case GUIBEHAVIOR_DIEFADE:
        m_behaviorObject = new GUIDIEFADE(this);
        break;
    }
    if (m_behaviorObject) {
        char params[0x200];
        strncpy(params, m_behaviorParams.c_str(), 0x1ff);
        params[0x1ff] = 0;
        m_behaviorObject->SetParams(params);
    }
}

void GUICONTROL::SetBehaviorParams(const CSTRING &params)
{
    if (_stricmp(params.c_str(), m_behaviorParams.c_str())) {
        m_behaviorParams = params;
        if (m_behaviorObject) {
            char buffer[0x200];
            strncpy(buffer, m_behaviorParams.c_str(), 0x1ff);
            buffer[0x1ff] = 0;
            m_behaviorObject->SetParams(buffer);
        }
    }
}

SRESULT GUICONTROL::GParent(CMDCALL *call)
{
    if (m_parent)
        m_parent->Run(STRING(call->tokens->GetToken(1)));
    return SRESULT(true);
}
