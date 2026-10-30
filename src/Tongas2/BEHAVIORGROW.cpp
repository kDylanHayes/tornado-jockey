// INTERPOLATEDGROW's step: scales the actor towards its scale with an
// overshoot. Placeholder file name.
#include "Tongas2/BEHAVIORS.h"

// Defined in another unit: eases t, from 0 to 1, past the end and back.
float EaseOvershoot(float t);

void INTERPOLATEDGROWBEHAVIOR::Update()
{
    m_time -= g_pTimer->GetDt();
    float t = m_scale > 1.0f ? 1.0f - m_time / m_duration : m_time / m_duration;
    if (t < 0.0f)
        t = 0.0f;
    else if (t > 1.0f)
        t = 1.0f;
    float eased = EaseOvershoot(t);
    ACTOR *actor = GetActor();
    if (actor) {
        if (m_scale > 1.0f)
            actor->SetScale(m_original * ((m_scale - 1.0f) * eased + 1.0f));
        else
            actor->SetScale(m_original * ((1.0f - eased) * m_scale + eased));
    }
}
