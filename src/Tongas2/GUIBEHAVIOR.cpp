// The GUI behaviors' base, and the behavior that fades a control out and
// closes it. Placeholder file name.
#define TOKENS_INLINE_DTOR_CLEAR
#define STRINGMANAGER_FREE_NOTHROW
#define STRING_FREE_OUTOFLINE
#define V2_GUIBEHAVIOR
#include "Tongas2/GUIBEHAVIOR.h"

void GUIDIEFADE::Start()
{
    GUIBEHAVIOR::Start();
    m_opacity = m_control->m_opacity;
    m_left = m_time;
}

void GUIDIEFADE::Update()
{
    GUIBEHAVIOR::Update();
    if (m_left > 0.0f) {
        GUICONTROL *control = m_control;
        if (control->m_timeSpace == 1)
            m_left -= g_pTimer->m_soundDt;
        else
            m_left -= g_pTimer->m_dt;
        if (m_left <= 0.0f) {
            control->SetOpacity(0.0f);
            m_control->Close();
            m_left = 0.0f;
        } else if (m_left <= m_fadeTime) {
            control->SetOpacity(m_left / m_fadeTime);
        }
    }
}

void GUIDIEFADE::Init(TOKENS &params)
{
    m_time = (float)atof(params.GetToken(0));
    m_fadeTime = (float)atof(params.GetToken(1));
}

GUIDIEFADE::GUIDIEFADE(GUICONTROL *control) : GUIBEHAVIOR(control)
{
    m_left = 0.0f;
    m_time = 1.0f;
    m_fadeTime = 0.5f;
}

void GUIBEHAVIOR::SetParams(const char *params)
{
    TOKENS tokens;
    {
        STRING text(params);
        tokens.Parse(text, 0);
    }
    Init(tokens);
}

void GUIDIEFADE::Stop()
{
    m_started = false;
    m_control->SetOpacity(m_opacity);
}
