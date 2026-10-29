// GUICHECKBOX, the GUI's check boxes: a GUI group that runs the template's
// OnCheckOn or OnCheckOff when it is checked or unchecked. The original file
// name is not known; the masters are in GUICHECKBOX.cpp.
// This file was built seeing SRESULT::FreeValue as one that can throw.
#define SRESULT_FREEVALUE_MAY_THROW

#include "Tongas2/GUICHECKBOX.h"
#include "Tongas2/CMDMGR.h"
#include "Tongas2/GUICHECKGROUP.h"
#include "Tongas2/LOG.h"

CMDENTRY GUICHECKBOX::s_commands[] = {
    { 0, 0, 0, 0, 0 },
};

void GUICHECKBOX::RegisterCommands()
{
    g_classParents[s_classId] = GUIGROUP::s_classId;
    g_classCommands[s_classId] = s_commands;
    HashCommands(s_commands);
    RegisterClassName(s_commands, "GUICHECKBOX", "GUIGROUP");
}

GUICHECKBOX::GUICHECKBOX(const RESHANDLE &master, GUIGROUP *parent) : GUIGROUP(master, parent)
{
    m_checkMaster = 0;
    m_group = 0;
    m_checked = false;
    m_checkMaster = (GUICHECKBOXMASTER *)g_pResources->GetMaster(master);
    if (!m_checkMaster && g_pLog)
        g_pLog->Error("ERROR CREATING GUICHECKBOX CONTROL!!");
}

void GUICHECKBOX::SetChecked(const bool &checked)
{
    if (m_group && !m_group->CanCheck(this, checked))
        return;
    m_checked = checked;
    if (m_checked) {
        STRING command(m_checkMaster->m_onCheckOn.c_str());
        g_pCmdMgr->Run(command, m_handle);
    } else {
        STRING command(m_checkMaster->m_onCheckOff.c_str());
        g_pCmdMgr->Run(command, m_handle);
    }
}
