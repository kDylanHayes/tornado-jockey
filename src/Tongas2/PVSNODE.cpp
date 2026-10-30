// The scene's visibility nodes: the list of actors in one, and the set
// with one node only. Placeholder file name.
#include "Tongas2/CAMERA.h"
#include "Tongas2/PVS.h"
#include "Tongas2/SRESULT.h"

PVSNODE::PVSNODE()
{
    m_members = 0;
}

void PVSNODE::AddMember(ACTOR *actor)
{
    if (!m_members) {
        m_members = actor;
        actor->m_prev = 0;
        actor->m_next = 0;
        actor->m_node = this;
        return;
    }
    actor->m_next = m_members;
    actor->m_prev = 0;
    actor->m_node = this;
    m_members->m_prev = actor;
    m_members = actor;
}

void PVSNODE::RemoveMember(ACTOR *actor)
{
    if (actor->m_node == this) {
        if (actor == m_members)
            m_members = actor->m_next;
        else if (actor->m_prev)
            actor->m_prev->m_next = actor->m_next;
        if (actor->m_next)
            actor->m_next->m_prev = actor->m_prev;
        actor->m_next = 0;
        actor->m_prev = 0;
        actor->m_node = 0;
    }
}

// Takes actor and the actors after it out of their node.
static void __stdcall Unlink(ACTOR *actor)
{
    if (actor->m_next)
        Unlink(actor->m_next);
    actor->m_next = 0;
    actor->m_prev = 0;
    actor->m_node = 0;
}

void ONENODE::Update(FRUSTUM *frustum)
{
    for (ACTOR *actor = m_members; actor; actor = actor->m_next) {
        if (frustum->Classify(actor->GetBox()) != 2)
            actor->m_inView = true;
    }
}

void ONENODE::Dump(FRUSTUM *frustum)
{
    for (ACTOR *actor = m_members; actor; actor = actor->m_next)
        frustum->Classify(actor->GetBox());
}

void ONENODE::Clear()
{
    if (m_members)
        Unlink(m_members);
    m_members = 0;
}

PVSNODE::~PVSNODE()
{
    if (m_members)
        Unlink(m_members);
    m_members = 0;
}

ONENODE::ONENODE()
{
}
