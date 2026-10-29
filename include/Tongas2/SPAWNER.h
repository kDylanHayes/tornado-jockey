// Spawners: actors that make copies of up to 8 template actors, picked at
// random, at their own position. A spawner spawns after a random delay
// between WaitTime's two values (also before the first with WaitOnFirst),
// up to MaxSpawns copies or for MaxSpawnTime seconds, turning each copy
// SpawnAngleDelta degrees further than the last, or by a random angle in
// RandomAngleRange. Each copy is named after its template and gets
// TargetCommand run on it. Placeholder names apart from the class names,
// properties and commands.
#ifndef TONGAS2_SPAWNER_H
#define TONGAS2_SPAWNER_H

#include "Tongas2/ACTOR.h"
#include "Tongas2/FIXEDSTRING.h"
#include "Tongas2/STRING.h"

#include <stdlib.h>

#define SPAWNER_MAX_TEMPLATES 8

// A range of values, none until set.
struct SPAWNERRANGE : V2
{
    SPAWNERRANGE() : V2(0.0f, 0.0f) {}
    SPAWNERRANGE &operator=(const V2 &other)
    {
        V2::operator=(other);
        return *this;
    }
};

// A size, none until set.
struct SPAWNERSIZE : V3
{
    SPAWNERSIZE() : V3(0.0f, 0.0f, 0.0f) {}
    SPAWNERSIZE &operator=(const V3 &other)
    {
        V3::operator=(other);
        return *this;
    }
};

class SPAWNER : public ACTOR
{
public:
    SPAWNER();
    virtual ~SPAWNER();

    static void RegisterCommands();

    virtual int GetClassId() { return s_classId; }
    virtual int GetParentClassId() { return g_classParents[s_classId]; }
    virtual const char *GetClassName() { return g_classNames[s_classId]; }
    virtual SRESULT Execute(CMDENTRY *command, TOKENS **args)
    {
        return (this->*(SRESULT (SPAWNER::*)(TOKENS **))command->func)(args);
    }
    virtual int GetFactoryId() { return s_factoryId; }
    virtual int GetParentFactoryId() { return ACTOR::s_factoryId; }
    virtual int GetFactoryFlags() { return g_factoryFlags[s_factoryId]; }
    virtual void Place()
    {
        m_placed = true;
        ResetAngle();
    }
    virtual void Update();
    virtual void Start();
    virtual void OnScriptLoaded();
    virtual V3 GetSize() { return m_size; }

    virtual void SetWaitTime(const V2 &time) { m_waitTime = time; }
    virtual void SetMaxSpawnTime(const float &time) { m_maxSpawnTime = time; }
    virtual void SetMaxSpawns(const int &max) { m_maxSpawns = max; }
    virtual void SetTargetCommand(const STRING &command) { m_targetCommand = command; }
    virtual void SetEnabled(const bool &enabled)
    {
        if (enabled) {
            m_enabled = true;
            m_delay = 0.0f;
        } else {
            m_enabled = false;
        }
    }
    virtual void SetSpawnAngleDelta(const float &delta) { m_angleDelta = delta; }
    virtual void SetWaitOnFirst(const bool &wait) { m_waitOnFirst = wait; }
    virtual void SetUseSpawnerAngleScale(const bool &use) { m_useSpawnerAngleScale = use; }
    virtual void SetSpawnObjectName(const int &i, const FIXEDSTRING<0x200> &name)
    {
        m_numTemplates = -1;
        m_names[i] = name;
    }
    virtual void SetUseRandomAngle(const bool &use) { m_useRandomAngle = use; }
    virtual void SetRandomAngleRange(const V2 &range) { m_randomAngleRange = range; }
    // Spawns a copy of a template; 0 if it could not.
    virtual ACTOR *Spawn();
    virtual SRESULT Reset(CMDCALL *call)
    {
        ResetSpawning();
        m_enabled = true;
        return SRESULT(true);
    }

    // A copy of tmpl at position, or 0.
    static ACTOR *Clone(ACTOR *tmpl, V3 position);

    // The angle the first copy is turned by, so the copies fan out evenly.
    void ResetAngle() { m_angle = (1 - m_maxSpawns) * m_angleDelta * 0.5f; }
    void ResetSpawning();
    // Picks the wait before the next spawn.
    __forceinline void PickDelay()
    {
        if (m_waitTime.y > 0.0f) {
            if (m_waitTime.y - m_waitTime.x > 0.0f)
                m_delay = rand() % (int)((m_waitTime.y - m_waitTime.x) * 10.0f) * 0.1f + m_waitTime.x;
            else
                m_delay = m_waitTime.x;
        }
    }
    // Finds the templates by name.
    void FindTemplates();

    static int s_classId;
    static int s_factoryId;
    static CMDENTRY s_commands[];
    static PROPENTRY s_properties[];
    // Spawns this frame, to catch a runaway loop.
    static int s_steps;

    SPAWNERRANGE m_waitTime;
    float m_maxSpawnTime;
    int m_maxSpawns;
    STRING m_targetCommand;
    bool m_enabled;
    float m_angleDelta;
    bool m_waitOnFirst;
    bool m_useSpawnerAngleScale;
    FIXEDSTRING<0x200> m_names[SPAWNER_MAX_TEMPLATES];
    bool m_useRandomAngle;
    SPAWNERRANGE m_randomAngleRange;
    int m_spawned;
    // Time spent spawning.
    float m_time;
    // Time left before the next spawn.
    float m_delay;
    SPAWNERSIZE m_size;
    // The angle the next copy is turned by.
    float m_angle;
    // How many templates were found, -1 until looked for.
    int m_numTemplates;
    RESHANDLE m_templates[SPAWNER_MAX_TEMPLATES];
};

#endif
