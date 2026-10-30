// GROWRESLOOP's step: pulses one of the actor's resources' scale twice a
// period, while the resource is there. Placeholder file name.
#include "Tongas2/BEHAVIORS.h"

void GROWRESLOOPBEHAVIOR::Update()
{
    m_time += g_pTimer->GetDt();
    float angle = m_time / m_period * 3.1415927f;
    float s = sin(angle);
    ACTOR *actor = GetActor();
    if (actor) {
        float scale = fabs(s) * (m_scale - 1.0f) + 1.0f;
        actor->SetResourcesScale(m_resource, m_original * scale);
    }
}

bool GROWRESLOOPBEHAVIOR::IsRunning()
{
    ACTOR *actor = GetActor();
    if (actor && g_pResources->GetResource(actor->m_copies[m_resource]))
        return true;
    return false;
}
