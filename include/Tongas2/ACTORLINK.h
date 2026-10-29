// The second interface actors and GUI controls have, at 0x1c in both: their
// placement on the screen and in the world, their opacity, and the queue of
// behaviors they run. Defined elsewhere; the overrides take this pointing
// at it. Placeholder names.
#ifndef TONGAS2_ACTORLINK_H
#define TONGAS2_ACTORLINK_H

#include "Tongas2/MATHS.h"
#include "Tongas2/SRESULT.h"

class ACTORLINK
{
public:
    ACTORLINK();
    virtual ~ACTORLINK();
    // The placement on the screen, from 0 to 1, and the size.
    virtual V2 Link01() = 0;
    virtual void Link02(const V2 &position) = 0;
    virtual V2 Link03() = 0;
    virtual void Link04(const V2 &size) = 0;
    // The placement in the world, and the scale.
    virtual V3 Link05() = 0;
    virtual void Link06(const V3 &position) = 0;
    virtual void Link07(V3 *direction, V3 *up) = 0;
    // Turns to look along direction from position.
    virtual void Link08(const V3 &position, const V3 &direction) = 0;
    virtual V3 Link09() = 0;
    virtual void Link10(const V3 &scale) = 0;
    virtual float Link11() = 0;
    virtual void Link12(float opacity) = 0;
    // Closes it, or kills the actor.
    virtual void Link13() = 0;
    virtual float Link14();

    // Adds a behavior by name with its parameters to the queue.
    void QueueBehavior(const char *name, const char *params);
    // Steps the behaviors.
    void UpdateBehaviors(float dt);

    int m_4;
    int m_8;
};

#endif
