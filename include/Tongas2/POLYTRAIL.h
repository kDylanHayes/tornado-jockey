// Trails: POLYTRAILMASTER, from POLYTRAIL.txt, gives a trail type how long
// its points live, how many it drops a second and a velocity they drift
// with. A POLYTRAIL drops a point behind the object as it moves, fades the
// older ones out and, when let go, fades out entirely before it goes.
// Placeholder names apart from the class names and properties.
#ifndef TONGAS2_POLYTRAIL_H
#define TONGAS2_POLYTRAIL_H

#include "Tongas2/CURVE.h"

class POLYTRAILMASTER : public CURVEMASTER
{
public:
    POLYTRAILMASTER(RESOURCEMANAGER *manager, const char *name);
    virtual ~POLYTRAILMASTER() {}

    static void RegisterCommands();

    virtual int GetClassId();
    virtual int GetParentClassId();
    virtual const char *GetClassName();
    virtual SRESULT Execute(CMDENTRY *command, TOKENS **args)
    {
        return (this->*(SRESULT (POLYTRAILMASTER::*)(TOKENS **))command->func)(args);
    }
    virtual RESOURCE *CreateCopy(const RESHANDLE &handle);

    virtual void SetLife(const float &life) { m_life = life; }
    virtual void SetRate(const float &rate) { m_rate = rate; }
    virtual void SetExternalVel(const V3 &velocity) { m_externalVel = velocity; }

    static int s_classId;
    static CMDENTRY s_commands[];
    static PROPENTRY s_properties[];

    float m_life;
    float m_rate;
    V3 m_externalVel;
};

class POLYTRAIL : public CURVE
{
public:
    POLYTRAIL(POLYTRAILMASTER *master, const RESHANDLE &handle);

    POLYTRAILMASTER *Master() { return (POLYTRAILMASTER *)m_master; }

    // Starts the trail over from the object.
    virtual void Start();
    // Lets go at once if the trail was fading out.
    virtual void Stop();
    // Drops and moves the points, then places the mesh.
    virtual void Update();
    // Fades the trail out before letting it go.
    virtual void Virtual31()
    {
        if (!m_active) {
            Virtual20();
            return;
        }
        m_dying = true;
    }
    // Uses as many points as the trail's rate and life call for.
    virtual void PlacePoints();

    // Puts every point back at the object.
    void Reset();
    // Drops a point when it is time, moves the points with the object and
    // the velocity, and fades them.
    void Move();

    bool m_reset;
    bool m_dying;
    float m_interval;
    float m_timer;
    int m_count;
    int m_2480;
    CURVEPOINT m_velocity;
};

#endif
