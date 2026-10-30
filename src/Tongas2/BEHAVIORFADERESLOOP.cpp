// FADERESLOOP's step: pulses one of the actor's resources' opacity between
// its two values twice a period. Its IsRunning is GROWRESLOOP's, which the
// linker kept. Placeholder file name.
#include "Tongas2/BEHAVIORS.h"

void FADERESLOOPBEHAVIOR::Update()
{
    m_time += g_pTimer->GetDt();
    float angle = m_time / m_period * 3.1415927f;
    float s = sin(angle);
    ACTOR *actor = GetActor();
    if (actor) {
        float opacity = fabs(s) * (m_to - m_from) + m_from;
        actor->SetResourcesOpacity(m_resource, opacity);
    }
}
