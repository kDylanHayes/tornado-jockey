// The frame timer at 0x6d09c0. Only what other units use of it.
#ifndef TONGAS2_TIMER_H
#define TONGAS2_TIMER_H

class TIMER
{
public:
    int m_0;
    int m_4;
    // The length of the last frame, in seconds.
    float m_dt;
};
extern TIMER *g_pTimer;

#endif
