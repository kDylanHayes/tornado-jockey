// Lightning: TESLAMASTER, from TESLA.txt, gives a bolt type its two end
// points, how many times it splits the bolt in half, how often a second it
// strikes anew and how far each axis may jitter. A TESLA redraws its bolt
// between the ends at that rate, each split's midpoint pushed off at random
// by half as much as the last.
// Placeholder names apart from the class names and properties.
#ifndef TONGAS2_TESLA_H
#define TONGAS2_TESLA_H

#include "Tongas2/CURVE.h"

class TESLAMASTER : public CURVEMASTER
{
public:
    TESLAMASTER(RESOURCEMANAGER *manager, const char *name);
    virtual ~TESLAMASTER() {}

    static void RegisterCommands();

    virtual int GetClassId();
    virtual int GetParentClassId();
    virtual const char *GetClassName();
    virtual SRESULT Execute(CMDENTRY *command, TOKENS **args)
    {
        return (this->*(SRESULT (TESLAMASTER::*)(TOKENS **))command->func)(args);
    }
    virtual RESOURCE *CreateCopy(const RESHANDLE &handle);

    virtual void SetDepth(const int &depth) { m_depth = depth; }
    virtual void SetRate(const float &rate) { m_rate = rate; }
    virtual void SetRandX(const EMITRANGE &range) { m_randX = range; }
    virtual void SetRandY(const EMITRANGE &range) { m_randY = range; }
    virtual void SetRandZ(const EMITRANGE &range) { m_randZ = range; }
    virtual void SetPoint(const int &i, const V3 &point) { m_point[i] = point; }

    static int s_classId;
    static CMDENTRY s_commands[];
    static PROPENTRY s_properties[];

    int m_depth;
    float m_rate;
    EMITRANGE m_randX;
    EMITRANGE m_randY;
    EMITRANGE m_randZ;
    CURVEPOINT m_point[2];
};

class TESLA : public CURVE
{
public:
    TESLA(TESLAMASTER *master, const RESHANDLE &handle) : CURVE(master, handle) {}

    TESLAMASTER *Master() { return (TESLAMASTER *)m_master; }

    // Strikes when it is time, then places the mesh.
    virtual void Update();
    // Uses as many points as the splits call for at the detail level.
    virtual void PlacePoints();

    // Draws a new bolt between the ends when the last one's time is up.
    void Strike();

    float m_timer;
    int m_2474;
};

#endif
