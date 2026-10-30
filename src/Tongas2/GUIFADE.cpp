// The behavior that fades a GUI control in and out. Placeholder file name.
#define V2_GUIBEHAVIOR
#include "Tongas2/GUIBEHAVIOR.h"

void GUIFADE::Update()
{
    GUIBEHAVIOR::Update();
    if (m_control->m_opacity != m_current)
        Start();
    m_left -= g_pTimer->m_soundDt;
    if (m_left <= 0.0f) {
        m_back = !m_back;
        m_left += !m_back ? GUIRANDOM(m_outTime.x, m_outTime.y) : GUIRANDOM(m_backTime.x, m_backTime.y);
    }
    GUICONTROL *control = m_control;
    float next = control->m_opacity;
    if (m_back)
        next += (m_opacity - next) / m_left * g_pTimer->m_soundDt;
    else
        next += (m_target - next) / m_left * g_pTimer->m_soundDt;
    if (next < 0.0f)
        next = 0.0f;
    else if (next > 1.0f)
        next = 1.0f;
    m_current = next;
    control->SetOpacity(next);
}

GUIFADE::GUIFADE(GUICONTROL *control) : GUIBEHAVIOR(control)
{
    m_backTime = V2(0.5f, 0.5f);
    m_outTime = V2(0.5f, 0.5f);
    m_left = 0.0f;
    m_target = 0.0f;
}

void GUIFADE::Start()
{
    GUIBEHAVIOR::Start();
    float opacity = m_control->m_opacity;
    m_opacity = opacity;
    m_left = 0.0f;
    m_current = opacity;
    m_back = false;
}

void GUIFADE::Stop()
{
    m_started = false;
    m_control->SetOpacity(m_opacity);
}

void GUIFADE::Init(TOKENS &params)
{
    m_backTime.x = (float)atof(params.GetToken(0));
    m_backTime.y = (float)atof(params.GetToken(1));
    m_outTime.x = (float)atof(params.GetToken(2));
    m_outTime.y = (float)atof(params.GetToken(3));
    m_target = (float)atof(params.GetToken(4));
}
