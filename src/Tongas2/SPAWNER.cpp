#include "Tongas2/SPAWNER.h"
#include "Tongas2/LOG.h"
#include "Tongas2/PATH.h"
#include "Tongas2/CMDMGR.h"
#include "Tongas2/TIMER.h"

#define CMD_FUNC(CLASS, method) (CMDFUNC)(SRESULT (CLASS::*)(TOKENS **))(SRESULT (CLASS::*)(CMDCALL *))&CLASS::method

CMDENTRY SPAWNER::s_commands[] = {
    { "reset", 0, 0, 0, CMD_FUNC(SPAWNER, Reset) },
    { 0, 0, 0, 0, 0 },
};

PROPENTRY SPAWNER::s_properties[] = {
    { "WaitTime", 0, 10, 0, PROP_FIELD(SPAWNER, m_waitTime), false, 1, PROP_SETTER(SPAWNER, SetWaitTime) },
    { "MaxSpawnTime", 0, 3, 0, PROP_FIELD(SPAWNER, m_maxSpawnTime), false, 1, PROP_SETTER(SPAWNER, SetMaxSpawnTime) },
    { "MaxSpawns", 0, 0, 0, PROP_FIELD(SPAWNER, m_maxSpawns), false, 1, PROP_SETTER(SPAWNER, SetMaxSpawns) },
    { "TargetCommand", 0, 8, 0, PROP_FIELD(SPAWNER, m_targetCommand), false, 1, PROP_SETTER(SPAWNER, SetTargetCommand) },
    { "Enabled", 0, 2, 0, PROP_FIELD(SPAWNER, m_enabled), false, 1, PROP_SETTER(SPAWNER, SetEnabled) },
    { "SpawnAngleDelta", 0, 3, 0, PROP_FIELD(SPAWNER, m_angleDelta), false, 1, PROP_SETTER(SPAWNER, SetSpawnAngleDelta) },
    { "WaitOnFirst", 0, 2, 0, PROP_FIELD(SPAWNER, m_waitOnFirst), false, 1, PROP_SETTER(SPAWNER, SetWaitOnFirst) },
    { "UseSpawnerAngleScale", 0, 2, 0, PROP_FIELD(SPAWNER, m_useSpawnerAngleScale), false, 1, PROP_SETTER(SPAWNER, SetUseSpawnerAngleScale) },
    { "SpawnObjectName", 0, 9, 0, PROP_FIELD(SPAWNER, m_names), false, SPAWNER_MAX_TEMPLATES, PROP_SETTER(SPAWNER, SetSpawnObjectName) },
    { "UseRandomAngle", 0, 2, 0, PROP_FIELD(SPAWNER, m_useRandomAngle), false, 1, PROP_SETTER(SPAWNER, SetUseRandomAngle) },
    { "RandomAngleRange", 0, 10, 0, PROP_FIELD(SPAWNER, m_randomAngleRange), false, 1, PROP_SETTER(SPAWNER, SetRandomAngleRange) },
    { 0, 0, 15, 0, 0, false },
};

int SPAWNER::s_steps;

void SPAWNER::RegisterCommands()
{
    g_classParents[s_classId] = ACTOR::s_classId;
    g_classCommands[s_classId] = s_commands;
    HashCommands(s_commands);
    RegisterClassName(s_commands, "SPAWNER", "ACTOR");
}

void SPAWNER::Start()
{
    ACTOR::Start();
    if (m_waitOnFirst)
        PickDelay();
}

ACTOR *SPAWNER::Clone(ACTOR *tmpl, V3 position)
{
    ACTOR *copy = tmpl->Clone(0);
    if (copy) {
        if (copy->GetFactoryId() != tmpl->GetFactoryId()) {
            g_pScene->Remove(copy->m_handle);
        } else {
            copy->SetPosition(position);
            copy->Place();
            copy->SetIsTemplate(false);
            copy->SetActive(true);
            return copy;
        }
    }
    return 0;
}

void SPAWNER::ResetSpawning()
{
    m_time = 0.0f;
    m_delay = 0.0f;
    m_spawned = 0;
    ResetAngle();
    if (m_waitOnFirst)
        PickDelay();
}

void SPAWNER::FindTemplates()
{
    for (int i = 0; i < SPAWNER_MAX_TEMPLATES; i++) {
        ACTOR *actor = (ACTOR *)g_pScene->FindActor(m_names[i]);
        if (!actor) {
            m_numTemplates = i;
            return;
        }
        m_templates[i] = actor->m_handle;
    }
}

SPAWNER::~SPAWNER()
{
}

void SPAWNER::Update()
{
    ACTOR::Update();
    if (m_numTemplates < 0)
        FindTemplates();
    if (m_enabled && m_numTemplates > 0) {
        s_steps = 1;
        for (;;) {
            if (m_maxSpawns > 0 && m_spawned >= m_maxSpawns)
                break;
            if (m_maxSpawnTime > 0.0f) {
                m_time += g_pTimer->m_dt;
                if (m_time > m_maxSpawnTime)
                    break;
            }
            if (m_delay > 0.0f) {
                m_delay -= g_pTimer->m_dt;
                if (m_delay > 0.0f)
                    return;
            }
            Spawn();
            PickDelay();
            if (++s_steps > 100) {
                if (g_pLog)
                    g_pLog->Error("ALERT!!!! - MAX WHILE STEPS!!! - SPAWN()!!");
                return;
            }
        }
        m_enabled = false;
    }
}

void SPAWNER::OnScriptLoaded()
{
    ACTOR::OnScriptLoaded();
    FindTemplates();
    ACTOR *tmpl = (ACTOR *)g_pScene->GetActor(m_templates[0]);
    if (tmpl)
        m_size = tmpl->GetSize();
    else
        m_size.x = m_size.y = m_size.z = 0.0f;
}

SPAWNER::SPAWNER()
{
    m_waitTime = V2(2.0f, 3.0f);
    m_maxSpawnTime = 0.0f;
    m_maxSpawns = -1;
    m_targetCommand = "";
    m_enabled = true;
    m_angleDelta = 0.0f;
    m_waitOnFirst = false;
    m_useSpawnerAngleScale = false;
    for (g_i = 0; g_i < SPAWNER_MAX_TEMPLATES; g_i++)
        m_names[g_i].Set("");
    m_useRandomAngle = false;
    m_spawned = 0;
    m_time = 0.0f;
    m_delay = 0.0f;
    m_randomAngleRange = V2(0.0f, 180.0f);
    m_size.x = m_size.y = m_size.z = 0.0f;
    m_numTemplates = -1;
    m_angle = 0.0f;
}

ACTOR *SPAWNER::Spawn()
{
    int i = m_numTemplates > 1 && m_numTemplates - 1 > 0 ? rand() % m_numTemplates : 0;
    ACTOR *tmpl = (ACTOR *)g_pScene->GetActor(m_templates[i]);
    if (!tmpl) {
        if (g_pLog)
            g_pLog->Error("COuld not spawn from NULL template! %s", m_name.c_str());
        return 0;
    }
    ACTOR *actor = Clone(tmpl, GetPosition());
    if (!actor) {
        if (g_pLog)
            g_pLog->Error("SPAWNER::Spawn> %.2f %s error in spawn!", g_pTimer->m_time, m_name.c_str());
        return 0;
    }
    actor->m_spawner = m_handle;
    STRING name;
    name.Format("%s_spw_%02d", tmpl->m_name.c_str(), m_spawned);
    actor->SetName(CSTRING(name.c_str()));
    if (m_targetCommand.Length() > 1)
        g_pCmdMgr->Run(m_targetCommand, actor->m_handle);
    m_spawned++;
    if (m_useSpawnerAngleScale) {
        actor->SetAngles(GetAngles());
        actor->SetScale(GetScale());
    }
    if (m_angle != 0.0f) {
        MATRIX m;
        m.Identity();
        m.RotateY(m_angle);
        actor->m_angles.Rotate(m);
    }
    if (m_useRandomAngle) {
        float angle = m_randomAngleRange.y > m_randomAngleRange.x
            ? rand() * (m_randomAngleRange.y - m_randomAngleRange.x) * (1.0f / RAND_MAX) + m_randomAngleRange.x
            : m_randomAngleRange.x;
        MATRIX m;
        m.Identity();
        m.RotateY(angle);
        actor->m_vel = actor->m_vel * m;
    }
    m_angle += m_angleDelta;
    return actor;
}
