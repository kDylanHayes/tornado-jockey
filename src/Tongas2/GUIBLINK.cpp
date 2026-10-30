// The behavior that blinks a GUI control. Placeholder file name.
#define V2_GUIBEHAVIOR
#include "Tongas2/GUIBEHAVIOR.h"

void GUIBLINK::Stop()
{
    m_started = false;
    m_control->SetOpacity(m_opacity);
}

void GUIBLINK::Start()
{
    GUIBEHAVIOR::Start();
    float opacity = m_control->m_opacity;
    m_opacity = opacity;
    m_left = 0.0f;
    m_current = opacity;
    m_on = false;
}

void GUIBLINK::Update()
{
    GUIBEHAVIOR::Update();
    if (m_control->m_opacity != m_current)
        Start();
    m_left -= g_pTimer->m_soundDt;
    if (m_left <= 0.0f) {
        m_on = !m_on;
        if (!m_on) {
            m_left = GUIRANDOM(m_offTime.x, m_offTime.y);
            m_current = m_target;
            m_control->SetOpacity(m_target);
        } else {
            m_left = GUIRANDOM(m_onTime.x, m_onTime.y);
            m_current = m_opacity;
            m_control->SetOpacity(m_opacity);
        }
    }
}

void GUIBLINK::Init(TOKENS &params)
{
    m_onTime.x = (float)atof(params.GetToken(0));
    m_onTime.y = (float)atof(params.GetToken(1));
    m_offTime.x = (float)atof(params.GetToken(2));
    m_offTime.y = (float)atof(params.GetToken(3));
    m_target = (float)atof(params.GetToken(4));
}

GUIBLINK::GUIBLINK(GUICONTROL *control) : GUIBEHAVIOR(control)
{
    m_onTime = V2(0.5f, 0.5f);
    m_offTime = V2(0.5f, 0.5f);
    m_left = 0.0f;
    m_target = 0.0f;
}
