// The frame timer at 0x6d09c0. Only what other units use of it.
#ifndef TONGAS2_TIMER_H
#define TONGAS2_TIMER_H

class TIMER
{
public:
    int m_0;
    // The time since the game started, in seconds.
    float m_time;
    // The length of the last frame, in seconds.
    float m_dt;
    int m_c;
    // The frame time sound fades go by.
    float m_soundDt;
};
extern TIMER *g_pTimer;

#endif
