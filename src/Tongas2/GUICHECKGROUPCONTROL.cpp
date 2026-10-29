// GUICHECKGROUP, the GUI's check groups: a GUI group that keeps between its
// template's fewest and most of its check boxes checked. The original file
// name is not known; the masters are in GUICHECKGROUP.cpp.
// This file was built seeing SRESULT::FreeValue as one that can throw.
#define SRESULT_FREEVALUE_MAY_THROW

#include "Tongas2/GUICHECKGROUP.h"
#include "Tongas2/CMDMGR.h"
#include "Tongas2/GUICHECKBOX.h"
#include "Tongas2/LOG.h"

CMDENTRY GUICHECKGROUP::s_commands[] = {
    { 0, 0, 0, 0, 0 },
};

void GUICHECKGROUP::RegisterCommands()
{
    g_classParents[s_classId] = GUIGROUP::s_classId;
    g_classCommands[s_classId] = s_commands;
    HashCommands(s_commands);
    RegisterClassName(s_commands, "GUICHECKGROUP", "GUIGROUP");
}

GUICHECKGROUP::GUICHECKGROUP(const RESHANDLE &master, GUIGROUP *parent) : GUIGROUP(master, parent)
{
    m_groupMaster = 0;
    m_groupMaster = (GUICHECKGROUPMASTER *)g_pResources->GetMaster(master);
    if (!m_groupMaster && g_pLog)
        g_pLog->Error("ERROR CREATING GUICHECKGROUP CONTROL!!");
    for (int i = 0; i < m_controlCount; i++) {
        if (m_controls[i] && m_controls[i]->Virtual19() == 5)
            ((GUICHECKBOX *)m_controls[i])->m_group = this;
    }
    m_selected = 0;
    m_busy = false;
}

bool GUICHECKGROUP::CanCheck(GUICHECKBOX *box, bool checked)
{
    if (m_busy)
        return true;
    if (checked) {
        for (int i = 0; m_selected >= m_groupMaster->m_maxSelected && i < m_controlCount; i++) {
            if (m_controls[i] && m_controls[i]->Virtual19() == 5) {
                GUICHECKBOX *other = (GUICHECKBOX *)m_controls[i];
                if (!other->m_checked)
                    continue;
                m_busy = true;
                other->SetChecked(false);
                m_selected--;
                m_busy = false;
            }
        }
        m_selected++;
        return true;
    }
    if (m_selected <= m_groupMaster->m_minSelected)
        return false;
    m_selected--;
    return true;
}
