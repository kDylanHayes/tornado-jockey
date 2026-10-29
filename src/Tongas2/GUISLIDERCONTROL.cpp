// GUISLIDER, the GUI's sliders: a GUI group with one of its visuals'
// progress bars as the slider, which dragging along sets the bar's variable
// from. The original file name is not known; the masters are in
// GUISLIDER.cpp.
// This file was built seeing SRESULT::FreeValue as one that can throw.
#define SRESULT_FREEVALUE_MAY_THROW

#include "Tongas2/GUISLIDER.h"
#include "Tongas2/CMDMGR.h"
#include "Tongas2/CPRIMITIVE3.h"
#include "Tongas2/GUIVISUAL.h"
#include "Tongas2/LOG.h"

#include <string.h>

CMDENTRY GUISLIDER::s_commands[] = {
    { 0, 0, 0, 0, 0 },
};

void GUISLIDER::RegisterCommands()
{
    g_classParents[s_classId] = GUIGROUP::s_classId;
    g_classCommands[s_classId] = s_commands;
    HashCommands(s_commands);
    RegisterClassName(s_commands, "GUISLIDER", "GUIGROUP");
}

GUIVISUAL *GUISLIDER::FindVisual()
{
    GUICONTROL *control = (GUICONTROL *)Find(m_sliderMaster->m_sliderVisualName.c_str());
    if (control && control->Virtual19() == 4)
        return (GUIVISUAL *)control;
    if (g_pLog)
        g_pLog->Error("SLIDER must have defined an associated VISUAL. Use the control name defined in this group");
    return 0;
}

GUISLIDER::GUISLIDER(const RESHANDLE &master, GUIGROUP *parent) : GUIGROUP(master, parent)
{
    m_sliderMaster = 0;
    m_sliderMaster = (GUISLIDERMASTER *)g_pResources->GetMaster(master);
    if (!m_sliderMaster && g_pLog)
        g_pLog->Error("ERROR CREATING GUISLIDER CONTROL!!");
    m_visualMaster = 0;
    GUIVISUAL *visual = FindVisual();
    if (!visual)
        return;
    m_visualMaster = visual->m_visualMaster;
    int bar = m_sliderMaster->m_sliderVisualIdx;
    if (m_visualMaster->m_bindProgressBar[bar].Length() <= 0) {
        if (g_pLog)
            g_pLog->Error("SLIDER is associated with a VISUAL not binded to a variable! (maybe index %d not correct?)", bar);
        return;
    }
    m_setCommand.Assign(m_visualMaster->m_bindProgressBar[bar].c_str());
    char *field = strrchr(m_setCommand, '.');
    if (!field)
        field = m_setCommand;
    else
        field++;
    FIXEDSTRING<0x200> name(field);
    strcpy(field, "set ");
    m_setCommand += name;
}

void GUISLIDER::SetValueAt(float x, float y)
{
    int bar = m_sliderMaster->m_sliderVisualIdx;
    GUIVISUAL *visual = FindVisual();
    if (!visual)
        return;
    V2 start;
    start = m_visualMaster->m_pos[bar] + visual->m_position;
    V2 end = m_visualMaster->m_barBehavior[bar] ? start + m_visualMaster->m_barMoveMax[bar] : start + V2(0.01f, 0.01f);
    SEGMENT3 segment(V3(start.x, start.y, 0.0f), V3(end.x, end.y, 0.0f));
    float value = segment.Project(V3(x, y, 0.0f), 0);
    if (value < 0.0f)
        value = 0.0f;
    else if (value > 1.0f)
        value = 1.0f;
    value *= m_visualMaster->m_bindProgressBarMax[bar];
    if (m_visualMaster->m_barDiscrete[bar]) {
        float step = m_visualMaster->m_bindProgressBarMax[bar] / m_visualMaster->m_barDiscreteSteps[bar];
        value = (int)(value / step) * step;
    }
    FIXEDSTRING<0x200> text;
    SafeSprintf(text, " %f", value);
    STRING command(m_setCommand);
    command.Append(text);
    g_pCmdMgr->Run(command, m_handle);
    if (m_sliderMaster->m_onChange.Length() > 0) {
        STRING onChange(m_sliderMaster->m_onChange.c_str());
        g_pCmdMgr->Run(onChange, m_handle);
    }
}
