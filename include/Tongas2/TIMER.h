// The frame timer at 0x6d09c0. Defined elsewhere; only what other units use
// of it.
#ifndef TONGAS2_TIMER_H
#define TONGAS2_TIMER_H

class TIMER
{
public:
    virtual ~TIMER();
    // Starts timing from now.
    virtual void Start();

    // Measures the frame.
    void Update(float dt);

    // The time since the game started, in seconds.
    float m_time;
    // The length of the last frame, in seconds.
    float m_dt;
    int m_c;
    // The frame time sound fades go by.
    float m_soundDt;
};
extern TIMER *g_pTimer;

// Makes the timer.
extern TIMER *(*g_pfnCreateTimer)();

#endif
