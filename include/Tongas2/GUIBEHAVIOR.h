// The behaviors a GUI control can have, made when its Behavior is set:
// fading out and closing, pulsing its size, fading in and out, jittering
// its position and blinking. Each takes its parameters as a string of
// numbers. Placeholder names apart from the class names.
#ifndef TONGAS2_GUIBEHAVIOR_H
#define TONGAS2_GUIBEHAVIOR_H

// The behaviors' own files define V2_GUIBEHAVIOR: they were built seeing
// these 2D vector operators.
#ifdef V2_GUIBEHAVIOR
#include "Tongas2/V2.h"

inline V2 &operator+=(V2 &a, const V2 &b)
{
    a.x += b.x;
    a.y += b.y;
    return a;
}

inline bool operator!=(const V2 &a, const V2 &b)
{
    return a.x != b.x || a.y != b.y;
}

inline V2 operator*(const V2 &v, float f)
{
    return V2(v.x * f, v.y * f);
}

// v divided by f, or zero when f is.
inline V2 operator/(const V2 &v, const float &f)
{
    if (f == 0.0f)
        return V2(0.0f, 0.0f);
    float inv = 1.0f / f;
    return V2(v.x * inv, v.y * inv);
}
#endif

#include "Tongas2/GUICONTROL.h"
#include "Tongas2/TIMER.h"
#include "Tongas2/TOKENS.h"

#include <stdlib.h>

// A random value from min to max, or min when the range is empty.
#define GUIRANDOM(min, max) \
    ((max) > (min) ? ((max) - (min)) * (float)rand() * (1.0f / RAND_MAX) + (min) : (min))

// A 2D value that starts at zero.
struct BEHAVIORV2 : V2
{
    BEHAVIORV2() { x = 0.0f; y = 0.0f; }
    BEHAVIORV2 &operator=(const V2 &v) { V2::operator=(v); return *this; }
};

class GUIBEHAVIOR
{
public:
    GUIBEHAVIOR(GUICONTROL *control)
    {
        m_started = false;
        m_control = control;
    }
    virtual ~GUIBEHAVIOR() { m_started = false; }

    // Steps the behavior, starting it the first time.
    virtual void Update()
    {
        if (!m_started)
            Start();
    }
    // Reads the parameters.
    virtual void Init(TOKENS &params) = 0;
    // Splits the control's BehaviorParams and reads them.
    virtual void SetParams(const char *params);
    // Takes what it changes from the control, and puts it back.
    virtual void Start()
    {
        if (m_started)
            Stop();
        m_started = true;
    }
    virtual void Stop() { m_started = false; }

    GUICONTROL *m_control;
    bool m_started;
};

// Waits, then fades the control out over the last part of the time and
// closes it.
class GUIDIEFADE : public GUIBEHAVIOR
{
public:
    GUIDIEFADE(GUICONTROL *control);

    virtual void Update();
    virtual void Init(TOKENS &params);
    virtual void Start();
    virtual void Stop();

    float m_opacity;
    float m_time;
    float m_fadeTime;
    float m_left;
};

// Moves the control's size towards a target and back, waiting a random
// time each way.
class GUISIZE : public GUIBEHAVIOR
{
public:
    GUISIZE(GUICONTROL *control);

    virtual void Update();
    virtual void Init(TOKENS &params);
    virtual void Start();
    virtual void Stop();

    BEHAVIORV2 m_base;
    BEHAVIORV2 m_current;
    BEHAVIORV2 m_target;
    float m_left;
    // The ranges of the times back and out.
    BEHAVIORV2 m_backTime;
    BEHAVIORV2 m_outTime;
    bool m_back;
};

// Fades the control towards an opacity and back, waiting a random time
// each way.
class GUIFADE : public GUIBEHAVIOR
{
public:
    GUIFADE(GUICONTROL *control);

    virtual void Update();
    virtual void Init(TOKENS &params);
    virtual void Start();
    virtual void Stop();

    float m_opacity;
    float m_current;
    float m_target;
    float m_left;
    BEHAVIORV2 m_backTime;
    BEHAVIORV2 m_outTime;
    bool m_back;
};

// Shakes the control's position by random amounts for a random time, then
// leaves it still for another.
class GUIJITTER : public GUIBEHAVIOR
{
public:
    GUIJITTER(GUICONTROL *control);

    virtual void Update();
    virtual void Init(TOKENS &params);
    virtual void Start();
    virtual void Stop();

    BEHAVIORV2 m_base;
    BEHAVIORV2 m_current;
    BEHAVIORV2 m_amount;
    float m_left;
    BEHAVIORV2 m_onTime;
    BEHAVIORV2 m_offTime;
    bool m_on;
};

// Switches the control's opacity between its own and another, staying a
// random time on each.
class GUIBLINK : public GUIBEHAVIOR
{
public:
    GUIBLINK(GUICONTROL *control);

    virtual void Update();
    virtual void Init(TOKENS &params);
    virtual void Start();
    virtual void Stop();

    float m_opacity;
    float m_current;
    float m_target;
    float m_left;
    BEHAVIORV2 m_onTime;
    BEHAVIORV2 m_offTime;
    bool m_on;
};

#endif
