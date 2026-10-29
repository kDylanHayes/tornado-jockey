// The engine's kernel at 0x6d09c0: it makes the engine's systems, steps
// them each frame, a number of times and with the frame time scaled and
// capped, starts and quits the game, and deletes the systems again.
// Placeholder names; the class name is from its global's first use.
#ifndef TONGAS2_TIMER_H
#define TONGAS2_TIMER_H

// A list of functions the game adds to, run when the engine starts or
// shuts down.
struct TIMERHOOK
{
    TIMERHOOK()
    {
        next = 0;
        prev = 0;
        func = 0;
    }

    TIMERHOOK *prev;
    TIMERHOOK *next;
    void (*func)();
};

struct TIMERHOOKS
{
    ~TIMERHOOKS()
    {
        TIMERHOOK *next = head;
        while (next) {
            TIMERHOOK *hook = next;
            next = next->next;
            delete hook;
        }
    }

    TIMERHOOK *head;
    TIMERHOOK *tail;
};

extern TIMERHOOKS g_startHooks;
extern TIMERHOOKS g_shutdownHooks;

class TIMER
{
public:
    TIMER()
    {
        m_paused = false;
        m_soundDt = 0.0f;
        m_soundTime = 0.0f;
        m_20 = false;
        m_time = 0.0f;
        m_dt = 0.0f;
        m_scale = 1.0f;
        m_frames = 0;
        m_steps = 1;
        m_speed = 1.0f;
        m_maxDt = 0.05f;
        m_started = false;
    }
    // Deletes the systems.
    virtual ~TIMER();
    // Makes the systems and opens the first screen, or starts the game.
    virtual void Start();

    // Steps the systems for the frame.
    void Update(float dt);
    // Stops the game's time, or lets it run.
    void SetPaused(bool paused);
    // Makes the game, and deletes it again.
    void StartGame();
    void QuitGame();
    void SetSpeed(float speed);
    void SetSteps(int steps);
    void SetMaxDt(float dt);

    // The time since the game started, in seconds.
    float m_time;
    // The length of the last frame, in seconds.
    float m_dt;
    float m_scale;
    // The frame time sound fades go by.
    float m_soundDt;
    float m_soundTime;
    // The longest a frame counts for, if above 0.
    float m_maxDt;
    int m_frames;
    bool m_20;
    char m_21[7];
    bool m_paused;
    bool m_wasPaused;
    bool m_started;
    // How fast the game runs and how many steps a frame takes.
    float m_speed;
    int m_steps;
};
extern TIMER *g_pTimer;

// Makes the timer.
extern TIMER *(*g_pfnCreateTimer)();

#endif
