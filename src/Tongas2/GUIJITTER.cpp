// The behavior that shakes a GUI control's position. Placeholder file name.
#define V2_GUIBEHAVIOR
#include "Tongas2/GUIBEHAVIOR.h"

void GUIJITTER::Start()
{
    GUIBEHAVIOR::Start();
    GUICONTROL *control = m_control;
    m_base = control->m_position;
    m_current = control->m_position;
    m_left = 0.0f;
    m_on = false;
}

void GUIJITTER::Stop()
{
    m_started = false;
    m_control->SetPosition(m_base);
}

void GUIJITTER::Update()
{
    GUIBEHAVIOR::Update();
    if (m_control->m_position != m_current)
        Start();
    m_left -= g_pTimer->m_soundDt;
    if (m_left <= 0.0f) {
        m_on = !m_on;
        m_left += !m_on ? GUIRANDOM(m_offTime.x, m_offTime.y) : GUIRANDOM(m_onTime.x, m_onTime.y);
    }
    V2 position = m_base;
    if (m_on) {
        position.x += GUIRANDOM(-m_amount.x, m_amount.x);
        position.y += GUIRANDOM(-m_amount.y, m_amount.y);
    }
    m_current = position;
    m_control->SetPosition(position);
}

void GUIJITTER::Init(TOKENS &params)
{
    m_onTime.x = (float)atof(params.GetToken(0));
    m_onTime.y = (float)atof(params.GetToken(1));
    m_offTime.x = (float)atof(params.GetToken(2));
    m_offTime.y = (float)atof(params.GetToken(3));
    m_amount = V2((float)atof(params.GetToken(4)), (float)atof(params.GetToken(5)));
}

GUIJITTER::GUIJITTER(GUICONTROL *control) : GUIBEHAVIOR(control)
{
    m_left = 0.0f;
    m_onTime = V2(0.5f, 0.5f);
    m_offTime = V2(0.5f, 0.5f);
    m_amount = V2(0.01f, 0.01f);
}
