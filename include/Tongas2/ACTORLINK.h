// The second interface actors and GUI controls have, at 0x1c in both: their
// placement on the screen and in the world, their opacity, and the queue of
// behaviors they run, which scripts add by name (B2FadeTo and the rest). The
// overrides take this pointing at it. Placeholder names apart from the
// behaviors' script names.
#ifndef TONGAS2_ACTORLINK_H
#define TONGAS2_ACTORLINK_H

#include "Tongas2/LIST.h"
#include "Tongas2/MATHS.h"
#include "Tongas2/V2.h"

class BEHAVIOR2;

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

    LIST<BEHAVIOR2> m_behaviors;
};

// A behavior on the queue: it reads its parameters from text and is
// stepped until Update says it is done, then deleted.
class BEHAVIOR2 : public LISTNODE
{
public:
    // False if the parameters can't be used.
    virtual bool Init(const char *params) = 0;
    virtual bool Update(float dt) = 0;

    ACTORLINK *m_owner;
};

class PATHMASTER;

// B2FolllowMaxPath: moves from point to point of a path, taking the same
// time over each segment. Defined in B2PATH.cpp.
class B2FOLLOWMAXPATH : public BEHAVIOR2
{
public:
    virtual bool Init(const char *params);
    virtual bool Update(float dt);

    // The time left on this segment, and over the whole path.
    float m_time;
    float m_duration;
    PATHMASTER *m_path;
    int m_numPoints;
    int m_index;
};

// B2LookAtPath: turns to look at the point of a path as far along it as
// the owner's progress.
class B2LOOKATPATH : public BEHAVIOR2
{
public:
    virtual bool Init(const char *params);
    virtual bool Update(float dt);

    char m_10[0x8];
    PATHMASTER *m_path;
    char m_1c[0x8];
};

#endif
