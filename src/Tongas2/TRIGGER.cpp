#include "Tongas2/TRIGGER.h"
#include "Tongas2/CPRIMITIVE3.h"
#include "Tongas2/PATH.h"
#include "Tongas2/TIMER.h"

#include <stdlib.h>
#include <string.h>

#define CMD_FUNC(CLASS, method) (CMDFUNC)(SRESULT (CLASS::*)(TOKENS **))(SRESULT (CLASS::*)(CMDCALL *))&CLASS::method

CMDENTRY TRIGGER::s_commands[] = {
    { "touch", 0, 0, 0, CMD_FUNC(TRIGGER, Touch) },
    { "reset", 0, 0, 0, CMD_FUNC(TRIGGER, Reset) },
    { 0, 0, 0, 0, 0 },
};

PROPENTRY TRIGGER::s_properties[] = {
    { "CommandCounter", 0, 0, 0, PROP_FIELD(TRIGGER, m_commandCounter), false, 1, PROP_SETTER(TRIGGER, SetCommandCounter) },
    { "Enabled", 0, 2, 0, PROP_FIELD(TRIGGER, m_enabled), false, 1, PROP_SETTER(TRIGGER, SetEnabled) },
    { "MaxCommands", 0, 0, 0, PROP_FIELD(TRIGGER, m_maxCommands), false, 1, PROP_SETTER(TRIGGER, SetMaxCommands) },
    { "WaitTime", 0, 10, 0, PROP_FIELD(TRIGGER, m_waitTime), false, 1, PROP_SETTER(TRIGGER, SetWaitTime) },
    { "OncePerContact", 0, 2, 0, PROP_FIELD(TRIGGER, m_oncePerContact), false, 1, PROP_SETTER(TRIGGER, SetOncePerContact) },
    { "AlwaysTouched", 0, 2, 0, PROP_FIELD(TRIGGER, m_alwaysTouched), false, 1, PROP_SETTER(TRIGGER, SetAlwaysTouched) },
    { "SendToToucher", 0, 2, 0, PROP_FIELD(TRIGGER, m_sendToToucher), false, 1, PROP_SETTER(TRIGGER, SetSendToToucher) },
    { "Command", 0, 8, 0, PROP_FIELD(TRIGGER, m_commands), false, TRIGGER_MAX_COMMANDS, PROP_SETTER(TRIGGER, SetCommand) },
    { 0, 0, 15, 0, 0, false },
};

void TRIGGER::RegisterCommands()
{
    g_classParents[s_classId] = ACTOR::s_classId;
    g_classCommands[s_classId] = s_commands;
    HashCommands(s_commands);
    RegisterClassName(s_commands, "TRIGGER", "ACTOR");
}

void TRIGGER::SetPosition(const V3 &position)
{
    ACTOR::SetPosition(V3(position));
}

void TRIGGER::SetScale(const V3 &scale)
{
    ACTOR::SetScale(V3(scale));
}

void TRIGGER::RunCommand()
{
    int i = m_commandsRun;
    if (i > m_lastCommand)
        i = m_lastCommand;
    if (i >= 0) {
        STRING &command = m_commands[i];
        if (strlen(command.c_str()) != 0) {
            if (m_sendToToucher)
                g_pConsole->Run(command, m_toucher);
            else
                g_pConsole->Run(command, RESHANDLE());
        }
    }
    m_commandsRun++;
}

void TRIGGER::Touch(const RESHANDLE &toucher)
{
    if (m_maxCommands < 0 || m_commandsRun < m_maxCommands) {
        m_toucher = toucher;
        m_wait = m_waitTime.y > m_waitTime.x
            ? rand() * (m_waitTime.y - m_waitTime.x) * (1.0f / RAND_MAX) + m_waitTime.x
            : m_waitTime.x;
        m_touches++;
        if (m_commandCounter <= 1 || m_touches % m_commandCounter == 0)
            RunCommand();
    }
}

void TRIGGER::OnContact(CONTACT *contact)
{
    if (contact && contact->other && m_enabled && !(m_wait > 0.0f)) {
        if (m_oncePerContact && m_contact >= 0 && ::operator==(contact->other->GetParent(), m_toucher)) {
            m_contact = 1;
            return;
        }
        m_contact = 1;
        Touch(contact->other->GetParent());
    }
}

void TRIGGER::Update()
{
    ACTOR::Update();
    if (m_wait > 0.0f)
        m_wait -= g_pTimer->m_dt;
    m_contact--;
    if (m_wait <= 0.0f && m_alwaysTouched && m_enabled)
        Touch(RESHANDLE());
}

TRIGGER::~TRIGGER()
{
}

TRIGGER::TRIGGER()
{
    m_commandCounter = 1;
    m_enabled = true;
    m_maxCommands = 1;
    m_waitTime = TRIGGERWAIT();
    m_oncePerContact = false;
    m_alwaysTouched = false;
    m_sendToToucher = false;
    for (g_i = 0; g_i < TRIGGER_MAX_COMMANDS; g_i++)
        m_commands[g_i] = "";
    m_lastCommand = 0;
    ResetCounters();
}

void TRIGGER::SetCommand(const int &i, const STRING &command)
{
    m_commands[i] = command.c_str();
    int last;
    for (last = TRIGGER_MAX_COMMANDS - 1; last >= 0; last--) {
        if (m_commands[last].Length() > 1)
            break;
    }
    m_lastCommand = last;
}
