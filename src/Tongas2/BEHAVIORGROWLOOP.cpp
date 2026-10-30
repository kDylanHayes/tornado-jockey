// GROWLOOP's step: pulses the actor's scale once a period. Placeholder file
// name.
#include "Tongas2/BEHAVIORS.h"

void GROWLOOPBEHAVIOR::Update()
{
    m_time += g_pTimer->GetDt();
    float angle = m_time / m_period * 6.2831855f;
    ACTOR *actor = GetActor();
    if (actor) {
        float scale = sin(angle) * (m_scale - 1.0f) + 1.0f;
        actor->SetScale(m_original * scale);
    }
}
