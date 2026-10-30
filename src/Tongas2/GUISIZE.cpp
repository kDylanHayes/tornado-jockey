// The behavior that pulses a GUI control's size. Placeholder file name.
#define V2_GUIBEHAVIOR
#include "Tongas2/GUIBEHAVIOR.h"

void GUISIZE::Start()
{
    GUIBEHAVIOR::Start();
    GUICONTROL *control = m_control;
    m_base = control->m_size;
    m_current = control->m_size;
    m_left = 0.0f;
    m_back = false;
}

void GUISIZE::Stop()
{
    m_started = false;
    m_control->SetSize(m_base);
}

void GUISIZE::Update()
{
    GUIBEHAVIOR::Update();
    if (m_control->m_size != m_current)
        Start();
    m_left -= g_pTimer->m_soundDt;
    if (m_left <= 0.0f) {
        m_back = !m_back;
        m_left += !m_back ? GUIRANDOM(m_outTime.x, m_outTime.y) : GUIRANDOM(m_backTime.x, m_backTime.y);
    }
    GUICONTROL *control = m_control;
    V2 size = control->m_size;
    if (m_back) {
        float dt = g_pTimer->m_soundDt;
        size += (m_base - size) * dt / m_left;
    } else {
        float dt = g_pTimer->m_soundDt;
        size += (m_target - size) * dt / m_left;
    }
    m_current = size;
    control->SetSize(size);
}

GUISIZE::GUISIZE(GUICONTROL *control) : GUIBEHAVIOR(control)
{
    m_left = 0.0f;
    m_backTime = V2(0.5f, 0.5f);
    m_outTime = V2(0.5f, 0.5f);
    m_target = V2(1.5f, 1.5f);
}

void GUISIZE::Init(TOKENS &params)
{
    m_backTime.x = (float)atof(params.GetToken(0));
    m_backTime.y = (float)atof(params.GetToken(1));
    m_outTime.x = (float)atof(params.GetToken(2));
    m_outTime.y = (float)atof(params.GetToken(3));
    m_target = V2((float)atof(params.GetToken(4)), (float)atof(params.GetToken(5)));
}
