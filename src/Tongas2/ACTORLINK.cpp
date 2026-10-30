// The queue of behaviors actors and GUI controls run through their
// ACTORLINK interface, and the string hash the engine's indexes use.
// Placeholder file name.
#include "Tongas2/ACTORLINK.h"
#include "Tongas2/LOG.h"

#include <stdio.h>
#include <string.h>

int HashString(const char *str, int size)
{
    unsigned int hash = 0;
    int count = 0;
    if (!str)
        return 0;
    for (char c = *str; c; c = *++str) {
        if (c < 0x60)
            c += 0x20;
        hash = (hash << 4) + c;
        unsigned int high = hash & 0xf0000000;
        if (high)
            hash ^= high >> 24;
        hash &= ~high;
        if (++count > 0x200)
            break;
    }
    return hash % size;
}

// A 2D size that starts at zero.
struct LINKV2 : V2
{
    LINKV2() : V2(0.0f, 0.0f) {}
    LINKV2 &operator=(const V2 &v) { V2::operator=(v); return *this; }
};

// A value moved from one end to the other at a steady rate over a time.
template <class T>
struct INTERP
{
    INTERP() : m_elapsed(0.0f), m_running(false) {}
    INTERP(const T &from, const T &to, float time);

    // Steps it; false once it has arrived.
    bool Update(float dt);
    T GetValue() const
    {
        if (m_running)
            return m_value;
        return m_to;
    }

    T m_from;
    T m_to;
    float m_time;
    float m_elapsed;
    T m_value;
    bool m_running;
};

template <class T>
INTERP<T>::INTERP(const T &from, const T &to, float time)
{
    if (time > 0.0f) {
        m_time = time;
        m_elapsed = 0.0f;
        m_running = true;
        m_value = from;
        m_from = from;
        m_to = to;
    } else {
        m_running = false;
    }
}

template <class T>
bool INTERP<T>::Update(float dt)
{
    if (!m_running)
        return false;
    m_elapsed += dt;
    if (m_elapsed > m_time)
        m_elapsed = m_time;
    if (m_elapsed >= m_time) {
        m_running = false;
        m_value = m_to;
        return false;
    }
    m_value = (m_to - m_from) * (m_elapsed / m_time) + m_from;
    return true;
}

// B2Die: closes the control, or kills the actor, after a time.
class B2DIE : public BEHAVIOR2
{
public:
    virtual bool Init(const char *params);
    virtual bool Update(float dt);

    float m_delay;
};

bool B2DIE::Init(const char *params)
{
    if (!params)
        return false;
    sscanf(params, "%f", &m_delay);
    return true;
}

bool B2DIE::Update(float dt)
{
    m_delay -= dt;
    if (m_delay < 0.0f) {
        m_owner->Link13();
        return false;
    }
    return true;
}


// B2FadeTo: fades from one opacity to another over a time.
class B2FADETO : public BEHAVIOR2
{
public:
    virtual bool Init(const char *params);
    virtual bool Update(float dt);

    INTERP<float> m_fade;
};

// B2Scale2To: resizes from one size to another over a time.
class B2SCALE2TO : public BEHAVIOR2
{
public:
    B2SCALE2TO();
    virtual bool Init(const char *params);
    virtual bool Update(float dt);

    INTERP<LINKV2> m_size;
};

// B2FolllowMaxPath and B2LookAtPath: defined elsewhere.
class B2FOLLOWMAXPATH : public BEHAVIOR2
{
public:
    virtual bool Init(const char *params);
    virtual bool Update(float dt);

    char m_10[0x14];
};

class B2LOOKATPATH : public BEHAVIOR2
{
public:
    virtual bool Init(const char *params);
    virtual bool Update(float dt);

    char m_10[0x14];
};

void ACTORLINK::UpdateBehaviors(float dt)
{
    BEHAVIOR2 *behavior = m_behaviors.m_head;
    while (behavior) {
        if (!behavior->Update(dt)) {
            BEHAVIOR2 *next = static_cast<BEHAVIOR2 *>(behavior->next);
            m_behaviors.Remove(behavior);
            delete behavior;
            behavior = next;
        } else {
            behavior = LIST<BEHAVIOR2>::Next(behavior);
        }
    }
}

B2SCALE2TO::B2SCALE2TO()
{
}

bool B2FADETO::Init(const char *params)
{
    if (!params)
        return false;
    float from, to, time;
    sscanf(params, "%f,%f,%f", &from, &to, &time);
    m_fade = INTERP<float>(from, to, time);
    return true;
}

bool B2FADETO::Update(float dt)
{
    if (m_fade.Update(dt)) {
        m_owner->Link12(m_fade.GetValue());
        return true;
    }
    m_owner->Link12(m_fade.GetValue());
    return false;
}

bool B2SCALE2TO::Init(const char *params)
{
    if (!params)
        return false;
    LINKV2 from;
    LINKV2 to;
    float time;
    sscanf(params, "(%f,%f),(%f,%f),%f", &from.x, &from.y, &to.x, &to.y, &time);
    m_size = INTERP<LINKV2>(from, to, time);
    return true;
}

bool B2SCALE2TO::Update(float dt)
{
    if (m_size.Update(dt)) {
        m_owner->Link04(m_size.GetValue());
        return true;
    }
    return false;
}

ACTORLINK::ACTORLINK()
{
}

ACTORLINK::~ACTORLINK()
{
    m_behaviors.Clear();
}

void ACTORLINK::QueueBehavior(const char *name, const char *params)
{
    if (!name) {
        if (g_pLog)
            g_pLog->Error("Try to queue behavior2 with null name");
        return;
    }
    BEHAVIOR2 *behavior = 0;
    if (!stricmp(name, "B2FadeTo"))
        behavior = new B2FADETO;
    else if (!stricmp(name, "B2Scale2To"))
        behavior = new B2SCALE2TO;
    else if (!stricmp(name, "B2Die"))
        behavior = new B2DIE;
    else if (!stricmp(name, "B2FolllowMaxPath"))
        behavior = new B2FOLLOWMAXPATH;
    else if (!stricmp(name, "B2LookAtPath"))
        behavior = new B2LOOKATPATH;
    else if (g_pLog)
        g_pLog->Error("Behavior:%s not reconized", name);
    if (behavior) {
        behavior->m_owner = this;
        if (!behavior->Init(params)) {
            if (g_pLog)
                g_pLog->Error("Error - QueueBehavior could not set behavior:%s", name);
            delete behavior;
            return;
        }
        m_behaviors.Add(behavior);
        return;
    }
    if (g_pLog)
        g_pLog->Error("Error - QueueBehavior could not create behavior:%s", name);
}
