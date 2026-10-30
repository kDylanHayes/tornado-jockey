// The behaviors an actor's addBehavior command adds to it, made by name
// through GUIBEHAVIORS: each works on its actor through its handle and
// reads its parameters from the command's tokens, from the third on.
// Placeholder names apart from the script names.
#ifndef TONGAS2_BEHAVIORS_H
#define TONGAS2_BEHAVIORS_H

#include "Tongas2/ACTOR.h"
#include "Tongas2/CMDBASE.h"
#include "Tongas2/FIXEDSTRING.h"
#include "Tongas2/RESOURCES.h"
#include "Tongas2/SCENE.h"
#include "Tongas2/STRING.h"
#include "Tongas2/TIMER.h"
#include "Tongas2/TOKENS.h"

#include <stdio.h>
#include <stdlib.h>

// Defined in another unit: eases t, from 0 to 1, in and out.
float EaseInOut(float t);

class BEHAVIOR
{
public:
    BEHAVIOR(const RESHANDLE &owner) { m_owner = owner; }
    virtual ~BEHAVIOR() {}
    // Steps the behavior.
    virtual void Update() = 0;
    // Whether it still has work to do. A finished behavior is deleted.
    virtual bool IsRunning() { return true; }

    // Turns the actor.
    void SetAngles(const V3 &angles);

    ACTOR *GetActor() { return (ACTOR *)g_pScene->GetActor(m_owner); }

    RESHANDLE m_owner;
};

// A behavior that runs for its third token's seconds.
class TIMEDBEHAVIOR : public BEHAVIOR
{
public:
    TIMEDBEHAVIOR(const RESHANDLE &owner, CMDCALL *call);

    virtual void Update() { m_time -= g_pTimer->GetDt(); }
    virtual bool IsRunning() { return m_time > 0.0f; }

    // The time left and the whole time.
    float m_time;
    float m_duration;
};

// A value moved from one end to the other over a time, accelerating to
// halfway and slowing down after, in steps of a hundredth of a second.
struct ACCELERATION
{
    ACCELERATION() : m_from(0.0), m_to(0.0), m_running(false) {}
    ACCELERATION(const float &from, const float &to, const float &time);

    // Steps it; false once it has arrived.
    bool Update(float dt);

    double m_velocity;
    double m_distance;
    double m_time;
    double m_elapsed;
    double m_acceleration;
    double m_from;
    double m_to;
    double m_value;
    bool m_running;
};

// A point moved from one end to the other the same way.
struct V3ACCELERATION
{
    V3ACCELERATION() {}
    V3ACCELERATION(V3 from, V3 to, float time);

    // Steps it; false once it has arrived.
    bool Update(float dt);

    ACCELERATION m_unused;
    ZEROV3 m_from;
    ZEROV3 m_to;
    ZEROV3 m_current;
    ACCELERATION m_progress;
};

// GROW: scales one of the actor's resources to a scale and back, eased.
class GROWBEHAVIOR : public TIMEDBEHAVIOR
{
public:
    GROWBEHAVIOR(const RESHANDLE &owner, CMDCALL *call);
    virtual ~GROWBEHAVIOR();
    virtual void Update();

    float m_scale;
    ZEROV3 m_original;
    long m_resource;
};

// TRAVEL: moves the actor by an offset over the time.
class TRAVELBEHAVIOR : public TIMEDBEHAVIOR
{
public:
    TRAVELBEHAVIOR(const RESHANDLE &owner, CMDCALL *call);
    virtual ~TRAVELBEHAVIOR();
};

// FALL: falls until below a height, then removes the actor.
class FALLBEHAVIOR : public BEHAVIOR
{
public:
    FALLBEHAVIOR(const RESHANDLE &owner, CMDCALL *call);
    virtual ~FALLBEHAVIOR();
    virtual void Update() {}
    virtual bool IsRunning();

    float m_floor;
};

// FALLWFX: falls the same way, and starts an effect at a height on the way
// down that follows the actor across.
class FALLWFXBEHAVIOR : public FALLBEHAVIOR
{
public:
    FALLWFXBEHAVIOR(const RESHANDLE &owner, CMDCALL *call);
    virtual void Update();

    float m_fxHeight;
    float m_fxY;
    RESHANDLE m_fx;
    RESHANDLE m_fxCopy;
};

// INTERPOLATEDGROW: scales the actor to a scale over the time.
class INTERPOLATEDGROWBEHAVIOR : public TIMEDBEHAVIOR
{
public:
    INTERPOLATEDGROWBEHAVIOR(const RESHANDLE &owner, CMDCALL *call);
    virtual ~INTERPOLATEDGROWBEHAVIOR();
    // Defined in another unit.
    virtual void Update();

    float m_scale;
    ZEROV3 m_original;
};

// GROWLOOP: pulses the actor's scale for ever.
class GROWLOOPBEHAVIOR : public BEHAVIOR
{
public:
    GROWLOOPBEHAVIOR(const RESHANDLE &owner, CMDCALL *call);
    virtual ~GROWLOOPBEHAVIOR();
    // Defined in another unit.
    virtual void Update();

    float m_time;
    float m_scale;
    ZEROV3 m_original;
    float m_period;
};

// GROWRESLOOP: pulses one of the actor's resources' scale while the
// resource is there.
class GROWRESLOOPBEHAVIOR : public BEHAVIOR
{
public:
    GROWRESLOOPBEHAVIOR(const RESHANDLE &owner, CMDCALL *call);
    virtual ~GROWRESLOOPBEHAVIOR();
    // Defined in another unit.
    virtual void Update();
    virtual bool IsRunning();

    long m_resource;
    float m_time;
    float m_scale;
    ZEROV3 m_original;
    float m_period;
};

// FADERESLOOP: pulses one of the actor's resources' opacity between two
// values while the resource is there.
class FADERESLOOPBEHAVIOR : public BEHAVIOR
{
public:
    FADERESLOOPBEHAVIOR(const RESHANDLE &owner, CMDCALL *call);
    // Defined in another unit.
    virtual void Update();
    virtual bool IsRunning();

    long m_resource;
    float m_time;
    float m_from;
    float m_to;
    float m_period;
};

// BLINK: blinks the actor, faster and faster.
class BLINKBEHAVIOR : public TIMEDBEHAVIOR
{
public:
    BLINKBEHAVIOR(const RESHANDLE &owner, CMDCALL *call);
    virtual ~BLINKBEHAVIOR();
    virtual void Update();

    float m_period;
    float m_phase;
};

// SMOOTHROTATE: turns the actor from some angles to others over the time.
class SMOOTHROTATEBEHAVIOR : public TIMEDBEHAVIOR
{
public:
    SMOOTHROTATEBEHAVIOR(const RESHANDLE &owner, CMDCALL *call);
    virtual ~SMOOTHROTATEBEHAVIOR();
    virtual void Update();

    V3ACCELERATION m_angles;
};

// FLOWRADIUSTO: moves the actor's path flow radius to a value over the
// time.
class FLOWRADIUSTOBEHAVIOR : public TIMEDBEHAVIOR
{
public:
    FLOWRADIUSTOBEHAVIOR(const RESHANDLE &owner, CMDCALL *call);
    virtual ~FLOWRADIUSTOBEHAVIOR();
    virtual void Update();

    V3ACCELERATION m_radius;
};

// FADERESOURCE: fades one of the actor's resources from one opacity to
// another over the time.
class FADERESOURCEBEHAVIOR : public TIMEDBEHAVIOR
{
public:
    FADERESOURCEBEHAVIOR(const RESHANDLE &owner, CMDCALL *call);
    virtual ~FADERESOURCEBEHAVIOR();
    virtual void Update();

    float m_from;
    float m_to;
    long m_resource;
};

// BLINKRES: blinks one of the actor's resources, faster and faster.
class BLINKRESBEHAVIOR : public TIMEDBEHAVIOR
{
public:
    BLINKRESBEHAVIOR(const RESHANDLE &owner, CMDCALL *call);
    virtual ~BLINKRESBEHAVIOR();
    virtual void Update();

    long m_resource;
    float m_period;
    float m_phase;
};

// The circle ORBIT goes round: its center and two axes across it.
struct ORBITCIRCLE
{
    // Works out the axes across the circle from the axis through it.
    void SetAxis(const V3 &axis);

    V3 m_center;
    V3 m_u;
    V3 m_v;
};

// ORBIT: moves the actor round a circle, facing a point.
class ORBITBEHAVIOR : public TIMEDBEHAVIOR
{
public:
    ORBITBEHAVIOR(const RESHANDLE &owner, CMDCALL *call);
    virtual void Update()
    {
        TIMEDBEHAVIOR::Update();
        Place();
    }

    // Places and turns the actor for the time.
    void Place();

    float m_period;
    float m_start;
    float m_radius;
    ORBITCIRCLE m_circle;
    V3 m_lookAt;
};

// CHANGETEXTURE: never finished; it blinks an actor it never sets.
class CHANGETEXTUREBEHAVIOR : public TIMEDBEHAVIOR
{
public:
    CHANGETEXTUREBEHAVIOR(const RESHANDLE &owner, CMDCALL *call) : TIMEDBEHAVIOR(owner, call) {}
    virtual ~CHANGETEXTUREBEHAVIOR();
    virtual void Update();

    float m_period;
    float m_phase;
    bool m_up;
    ACTOR *m_actor;
    RESHANDLE m_copy;
    FIXEDSTRING<0x200> m_texture;
    FIXEDSTRING<0x200> m_original;
};

// TRAVELWANIM: turns the actor towards a point, walks it there and turns it
// to face another, playing an animation of one of its resources for each.
class TRAVELWANIMBEHAVIOR : public BEHAVIOR
{
public:
    enum
    {
        TURNING,
        WALKING,
        FACING
    };

    TRAVELWANIMBEHAVIOR(const RESHANDLE &owner, CMDCALL *call);
    virtual ~TRAVELWANIMBEHAVIOR();
    virtual void Update();
    virtual bool IsRunning() { return !m_done; }

    void Start();
    // Spins the actor towards direction.
    void Turn(const V3 &direction);
    // Whether the actor faces direction.
    bool IsFacing(const V3 &direction);
    void Walk();
    void Arrive();
    void Stop();

    int m_state;
    bool m_done;
    float m_turnSpeed;
    float m_speed;
    ZEROV3 m_target;
    ZEROV3 m_facing;
    int m_resource;
    STRING m_turnLeft;
    STRING m_turnRight;
    STRING m_walk;
    STRING m_idle;
};

#endif
