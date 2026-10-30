// The behaviors actors' addBehavior command adds by name (BEHAVIORS.h), the
// table of their names, and the actors' list of them. The original file name
// is not known.
#define ANG_INLINE_AXES

#include "Tongas2/BEHAVIORS.h"
#include "Tongas2/GUIBEHAVIORS.h"

#include <string.h>

void BEHAVIOR::SetAngles(const V3 &angles)
{
    ACTOR *actor = GetActor();
    if (actor)
        actor->SetAngles(ANG(angles.x, angles.y, angles.z));
}

TIMEDBEHAVIOR::TIMEDBEHAVIOR(const RESHANDLE &owner, CMDCALL *call) : BEHAVIOR(owner)
{
    m_time = m_duration = (float)atof(call->tokens->GetToken(2));
}

ACCELERATION::ACCELERATION(const float &from, const float &to, const float &time)
{
    if (time == 0.0f) {
        m_running = false;
        return;
    }
    m_distance = to - from;
    m_from = from;
    m_to = to;
    m_elapsed = 0.0;
    m_time = time;
    m_acceleration = m_distance / (time * time * 0.25);
    m_running = true;
    m_value = from;
    m_velocity = 0.0;
}

bool ACCELERATION::Update(float dt)
{
    if (!m_running)
        return false;
    while (dt > 0.0f) {
        double step;
        if (dt > 0.01) {
            step = 0.01;
            dt -= 0.01f;
        } else {
            step = dt;
            dt = 0.0f;
        }
        double last = m_elapsed;
        m_elapsed += step;
        if (m_elapsed > m_time)
            m_elapsed = m_time;
        if (m_elapsed >= m_time) {
            m_value = m_to;
            return false;
        }
        double half = m_time * 0.5;
        if (m_elapsed < half) {
            m_velocity = step * m_acceleration + m_velocity;
            m_value = m_velocity * step + m_value;
        } else if (last < half) {
            double before = half - last;
            double after = m_elapsed - half;
            double velocity = before * m_acceleration + m_velocity;
            m_value = velocity * before + m_value;
            m_velocity = velocity - after * m_acceleration;
            m_value = m_velocity * after + m_value;
        } else {
            m_velocity = m_velocity - step * m_acceleration;
            m_value = m_velocity * step + m_value;
        }
        if (m_from <= m_to) {
            if (m_value < m_from)
                m_value = m_from;
            if (m_value > m_to)
                m_value = m_to;
        } else {
            if (m_value < m_to)
                m_value = m_to;
            if (m_value > m_from)
                m_value = m_from;
        }
    }
    return true;
}

V3ACCELERATION::V3ACCELERATION(V3 from, V3 to, float time)
{
    m_progress = ACCELERATION(0.0f, 1.0f, time);
    m_to = to;
    m_from = from;
    m_current = from;
}

bool V3ACCELERATION::Update(float dt)
{
    bool running = m_progress.Update(dt);
    float t = (float)(m_progress.m_running ? m_progress.m_value : m_progress.m_to);
    m_current = (m_to - m_from) * t + m_from;
    return running;
}

GROWBEHAVIOR::GROWBEHAVIOR(const RESHANDLE &owner, CMDCALL *call) : TIMEDBEHAVIOR(owner, call)
{
    m_resource = atoi(call->tokens->GetToken(3));
    m_scale = (float)atof(call->tokens->GetToken(4));
    ACTOR *actor = GetActor();
    if (actor && m_resource >= 0)
        m_original = actor->m_resourcesScale[m_resource];
}

void GROWBEHAVIOR::Update()
{
    m_time -= g_pTimer->GetDt();
    float t = 1.0f - m_time / m_duration;
    if (t < 0.0f)
        t = 0.0f;
    else if (t > 1.0f)
        t = 1.0f;
    t = EaseInOut(t);
    ACTOR *actor = GetActor();
    if (actor) {
        float scale = (m_scale - 1.0f) * t + 1.0f;
        actor->SetResourcesScale(m_resource, m_original * scale);
    }
}

GROWBEHAVIOR::~GROWBEHAVIOR()
{
    ACTOR *actor = GetActor();
    if (actor)
        actor->SetResourcesScale(m_resource, m_original);
}

TRAVELBEHAVIOR::TRAVELBEHAVIOR(const RESHANDLE &owner, CMDCALL *call) : TIMEDBEHAVIOR(owner, call)
{
    V3 offset(0.0f, 0.0f, 0.0f);
    sscanf(call->tokens->GetToken(3), "%f, %f, %f", &offset.x, &offset.y, &offset.z);
    ACTOR *actor = GetActor();
    if (actor)
        actor->m_vel = offset / m_time;
}

TRAVELBEHAVIOR::~TRAVELBEHAVIOR()
{
    ACTOR *actor = GetActor();
    if (actor)
        actor->m_vel.x = actor->m_vel.y = actor->m_vel.z = 0.0f;
}

FALLBEHAVIOR::FALLBEHAVIOR(const RESHANDLE &owner, CMDCALL *call) : BEHAVIOR(owner)
{
    m_floor = (float)atof(call->tokens->GetToken(2));
    ACTOR *actor = GetActor();
    if (actor)
        actor->m_aceleration.y = -9.8f;
}

FALLBEHAVIOR::~FALLBEHAVIOR()
{
    ACTOR *actor = GetActor();
    if (actor)
        actor->m_removeRequested = true;
}

bool FALLBEHAVIOR::IsRunning()
{
    ACTOR *actor = GetActor();
    if (!actor)
        return false;
    return m_floor < actor->m_position.y;
}

FALLWFXBEHAVIOR::FALLWFXBEHAVIOR(const RESHANDLE &owner, CMDCALL *call) : FALLBEHAVIOR(owner, call)
{
    m_fxHeight = (float)atof(call->tokens->GetToken(3));
    m_fxY = (float)atof(call->tokens->GetToken(5));
    RESOURCEMASTER *master = g_pResources->FindMaster(call->tokens->GetToken(4));
    if (master)
        m_fx = master->m_handle;
}

void FALLWFXBEHAVIOR::Update()
{
    ACTOR *actor = GetActor();
    if (actor && actor->m_position.y <= m_fxHeight) {
        OBJECT *fx;
        if (!m_fxCopy.IsValid()) {
            fx = (OBJECT *)g_pResources->GetResource(m_fx);
            fx->SetLayer(actor->m_zsort + 5);
            fx->SetActive(true);
            m_fxCopy = fx->m_handle;
        } else {
            fx = (OBJECT *)g_pResources->GetResource(m_fxCopy);
        }
        if (fx)
            fx->SetPosition(V3(actor->m_position.x, m_fxY, actor->m_position.z));
    }
}

void ORBITBEHAVIOR::Place()
{
    ACTOR *actor = GetActor();
    if (!actor)
        return;
    float angle = (m_start - m_time) / m_period * 360.0f * 0.017453292f;
    float s = (float)sin(angle);
    float c = (float)cos(angle);
    actor->SetPosition((m_circle.m_u * c + m_circle.m_v * s) * m_radius + m_circle.m_center);
    actor->m_angles.PointAt(m_lookAt + m_circle.m_center - actor->GetPosition());
}

INTERPOLATEDGROWBEHAVIOR::INTERPOLATEDGROWBEHAVIOR(const RESHANDLE &owner, CMDCALL *call) : TIMEDBEHAVIOR(owner, call)
{
    m_scale = (float)atof(call->tokens->GetToken(3));
    ACTOR *actor = GetActor();
    if (actor)
        m_original = actor->GetScale();
}

INTERPOLATEDGROWBEHAVIOR::~INTERPOLATEDGROWBEHAVIOR()
{
    ACTOR *actor = GetActor();
    if (actor)
        actor->SetScale(V3(m_original.x * m_scale, m_original.y * m_scale, m_original.z * m_scale));
}

GROWLOOPBEHAVIOR::GROWLOOPBEHAVIOR(const RESHANDLE &owner, CMDCALL *call) : BEHAVIOR(owner)
{
    m_time = 0.0f;
    m_period = (float)atof(call->tokens->GetToken(2));
    m_scale = (float)atof(call->tokens->GetToken(3));
    ACTOR *actor = GetActor();
    if (actor)
        m_original = actor->m_scale;
}

GROWLOOPBEHAVIOR::~GROWLOOPBEHAVIOR()
{
    ACTOR *actor = GetActor();
    if (actor)
        actor->SetScale(m_original);
}

GROWRESLOOPBEHAVIOR::GROWRESLOOPBEHAVIOR(const RESHANDLE &owner, CMDCALL *call) : BEHAVIOR(owner)
{
    m_time = 0.0f;
    m_resource = atoi(call->tokens->GetToken(2));
    m_period = (float)atof(call->tokens->GetToken(3));
    m_scale = (float)atof(call->tokens->GetToken(4));
    ACTOR *actor = GetActor();
    if (actor)
        m_original = actor->m_resourcesScale[m_resource];
}

GROWRESLOOPBEHAVIOR::~GROWRESLOOPBEHAVIOR()
{
    ACTOR *actor = GetActor();
    if (actor)
        actor->SetResourcesScale(m_resource, m_original);
}

FADERESLOOPBEHAVIOR::FADERESLOOPBEHAVIOR(const RESHANDLE &owner, CMDCALL *call) : BEHAVIOR(owner)
{
    m_time = 0.0f;
    m_resource = atoi(call->tokens->GetToken(2));
    m_period = (float)atof(call->tokens->GetToken(3));
    m_from = (float)atof(call->tokens->GetToken(4));
    m_to = (float)atof(call->tokens->GetToken(5));
}

BLINKBEHAVIOR::BLINKBEHAVIOR(const RESHANDLE &owner, CMDCALL *call) : TIMEDBEHAVIOR(owner, call)
{
    m_period = (float)atof(call->tokens->GetToken(3));
    m_phase = m_period * 0.5f;
    float opacity = 0.5f;
    ACTOR *actor = GetActor();
    if (actor)
        actor->SetOpacity(opacity);
}

void BLINKBEHAVIOR::Update()
{
    m_time -= g_pTimer->GetDt();
    float opacity = m_phase / m_period;
    if (opacity < 0.0f)
        opacity = 0.0f;
    else if (opacity > 1.0f)
        opacity = 1.0f;
    ACTOR *actor = GetActor();
    if (actor)
        actor->SetOpacity(opacity);
    m_phase += g_pTimer->GetDt();
    if (m_phase >= m_period) {
        m_period *= 0.95f;
        m_phase = m_period * 0.5f;
    }
}

BLINKBEHAVIOR::~BLINKBEHAVIOR()
{
    float opacity = 1.0f;
    ACTOR *actor = GetActor();
    if (actor)
        actor->SetOpacity(opacity);
}

static BEHAVIOR *CreateGROW(const RESHANDLE &owner, CMDCALL *call)
{
    return new GROWBEHAVIOR(owner, call);
}

static BEHAVIOR *CreateTRAVEL(const RESHANDLE &owner, CMDCALL *call)
{
    return new TRAVELBEHAVIOR(owner, call);
}

SMOOTHROTATEBEHAVIOR::SMOOTHROTATEBEHAVIOR(const RESHANDLE &owner, CMDCALL *call) : TIMEDBEHAVIOR(owner, call)
{
    V3 from(0.0f, 0.0f, 0.0f);
    sscanf(call->tokens->GetToken(3), "%f, %f, %f", &from.x, &from.y, &from.z);
    V3 to(0.0f, 0.0f, 0.0f);
    sscanf(call->tokens->GetToken(4), "%f, %f, %f", &to.x, &to.y, &to.z);
    m_angles = V3ACCELERATION(from, to, m_time);
    SetAngles(from);
}

void SMOOTHROTATEBEHAVIOR::Update()
{
    if (m_angles.Update(g_pTimer->GetDt()))
        SetAngles(V3(m_angles.m_current));
}

SMOOTHROTATEBEHAVIOR::~SMOOTHROTATEBEHAVIOR()
{
    SetAngles(V3(m_angles.m_to));
}

FLOWRADIUSTOBEHAVIOR::FLOWRADIUSTOBEHAVIOR(const RESHANDLE &owner, CMDCALL *call) : TIMEDBEHAVIOR(owner, call)
{
    V3 from(0.0f, 0.0f, 0.0f);
    ACTOR *actor = GetActor();
    if (actor)
        from = V3(actor->m_flowRadius, 0.0f, 0.0f);
    V3 to(0.0f, 0.0f, 0.0f);
    sscanf(call->tokens->GetToken(3), "%f, %f, %f", &to.x, &to.y, &to.z);
    m_radius = V3ACCELERATION(from, to, m_time);
    actor = GetActor();
    if (actor)
        actor->SetFlowRadius(from.x);
}

void FLOWRADIUSTOBEHAVIOR::Update()
{
    if (m_radius.Update(g_pTimer->GetDt())) {
        V3 radius = m_radius.m_current;
        ACTOR *actor = GetActor();
        if (actor)
            actor->SetFlowRadius(radius.x);
    }
}

FLOWRADIUSTOBEHAVIOR::~FLOWRADIUSTOBEHAVIOR()
{
    V3 radius = m_radius.m_to;
    ACTOR *actor = GetActor();
    if (actor)
        actor->SetFlowRadius(radius.x);
}

static BEHAVIOR *CreateFALL(const RESHANDLE &owner, CMDCALL *call)
{
    return new FALLBEHAVIOR(owner, call);
}

static BEHAVIOR *CreateFALLWFX(const RESHANDLE &owner, CMDCALL *call)
{
    return new FALLWFXBEHAVIOR(owner, call);
}

void ORBITCIRCLE::SetAxis(const V3 &axis)
{
    if (fabs(axis.x) > fabs(axis.y)) {
        m_u.x = axis.z;
        m_u.y = 0.0f;
        m_u.z = -axis.x;
    } else {
        m_u.x = 0.0f;
        m_u.y = axis.z;
        m_u.z = -axis.y;
    }
    m_v.x = m_u.z * axis.y - axis.z * m_u.y;
    m_v.y = axis.z * m_u.x - m_u.z * axis.x;
    m_v.z = axis.x * m_u.y - axis.y * m_u.x;
    m_u.Normalize();
    m_v.Normalize();
}

FADERESOURCEBEHAVIOR::FADERESOURCEBEHAVIOR(const RESHANDLE &owner, CMDCALL *call) : TIMEDBEHAVIOR(owner, call)
{
    m_resource = atoi(call->tokens->GetToken(3));
    m_from = (float)atof(call->tokens->GetToken(4));
    m_to = (float)atof(call->tokens->GetToken(5));
    ACTOR *actor = GetActor();
    if (actor)
        actor->SetResourcesOpacity(m_resource, m_from);
}

void FADERESOURCEBEHAVIOR::Update()
{
    m_time -= g_pTimer->GetDt();
    float opacity = (m_to - m_from) * (1.0f - m_time / m_duration) + m_from;
    if (opacity < 0.0f)
        opacity = 0.0f;
    else if (opacity > 1.0f)
        opacity = 1.0f;
    ACTOR *actor = GetActor();
    if (actor)
        actor->SetResourcesOpacity(m_resource, opacity);
}

FADERESOURCEBEHAVIOR::~FADERESOURCEBEHAVIOR()
{
    ACTOR *actor = GetActor();
    if (actor)
        actor->SetResourcesOpacity(m_resource, m_to);
}

static BEHAVIOR *CreateINTERPOLATEDGROW(const RESHANDLE &owner, CMDCALL *call)
{
    return new INTERPOLATEDGROWBEHAVIOR(owner, call);
}

static BEHAVIOR *CreateGROWLOOP(const RESHANDLE &owner, CMDCALL *call)
{
    return new GROWLOOPBEHAVIOR(owner, call);
}

static BEHAVIOR *CreateGROWRESLOOP(const RESHANDLE &owner, CMDCALL *call)
{
    return new GROWRESLOOPBEHAVIOR(owner, call);
}

static BEHAVIOR *CreateFADERESLOOP(const RESHANDLE &owner, CMDCALL *call)
{
    return new FADERESLOOPBEHAVIOR(owner, call);
}

BLINKRESBEHAVIOR::BLINKRESBEHAVIOR(const RESHANDLE &owner, CMDCALL *call) : TIMEDBEHAVIOR(owner, call)
{
    m_resource = atoi(call->tokens->GetToken(3));
    m_period = (float)atof(call->tokens->GetToken(4));
    m_phase = m_period * 0.5f;
    float opacity = 0.5f;
    ACTOR *actor = GetActor();
    if (actor)
        actor->SetResourcesOpacity(m_resource, opacity);
}

void BLINKRESBEHAVIOR::Update()
{
    m_time -= g_pTimer->GetDt();
    float opacity = m_phase / m_period;
    if (opacity < 0.0f)
        opacity = 0.0f;
    else if (opacity > 1.0f)
        opacity = 1.0f;
    ACTOR *actor = GetActor();
    if (actor)
        actor->SetResourcesOpacity(m_resource, opacity);
    m_phase += g_pTimer->GetDt();
    if (m_phase >= m_period) {
        m_period *= 0.95f;
        m_phase = m_period * 0.5f;
    }
}

BLINKRESBEHAVIOR::~BLINKRESBEHAVIOR()
{
    float opacity = 1.0f;
    ACTOR *actor = GetActor();
    if (actor)
        actor->SetResourcesOpacity(m_resource, opacity);
}

static BEHAVIOR *CreateBLINK(const RESHANDLE &owner, CMDCALL *call)
{
    return new BLINKBEHAVIOR(owner, call);
}

static BEHAVIOR *CreateSMOOTHROTATE(const RESHANDLE &owner, CMDCALL *call)
{
    return new SMOOTHROTATEBEHAVIOR(owner, call);
}

static BEHAVIOR *CreateFLOWRADIUSTO(const RESHANDLE &owner, CMDCALL *call)
{
    return new FLOWRADIUSTOBEHAVIOR(owner, call);
}

ORBITBEHAVIOR::ORBITBEHAVIOR(const RESHANDLE &owner, CMDCALL *call) : TIMEDBEHAVIOR(owner, call)
{
    m_start = m_time;
    m_period = (float)atof(call->tokens->GetToken(3));
    m_radius = (float)atof(call->tokens->GetToken(4));
    sscanf(call->tokens->GetToken(5), "%f, %f, %f", &m_circle.m_center.x, &m_circle.m_center.y, &m_circle.m_center.z);
    V3 axis(0.0f, 0.0f, 0.0f);
    sscanf(call->tokens->GetToken(6), "%f, %f, %f", &axis.x, &axis.y, &axis.z);
    sscanf(call->tokens->GetToken(7), "%f, %f, %f", &m_lookAt.x, &m_lookAt.y, &m_lookAt.z);
    m_circle.SetAxis(axis);
    Place();
}

void TRAVELWANIMBEHAVIOR::Turn(const V3 &direction)
{
    ACTOR *actor = GetActor();
    if (actor) {
        if (actor->m_angles.GetDirection().AngleY(direction) < 0.0f) {
            actor->SetAngVel(V3(m_turnSpeed, 0.0f, 0.0f));
            actor->BlendResourceAnimation(m_resource, m_turnLeft.c_str(), 0.1f, true, 1.0f);
        } else {
            actor->SetAngVel(V3(-m_turnSpeed, 0.0f, 0.0f));
            actor->BlendResourceAnimation(m_resource, m_turnRight.c_str(), 0.1f, true, 1.0f);
        }
    }
}

static BEHAVIOR *CreateFADERESOURCE(const RESHANDLE &owner, CMDCALL *call)
{
    return new FADERESOURCEBEHAVIOR(owner, call);
}

void CHANGETEXTUREBEHAVIOR::Update()
{
    m_time -= g_pTimer->GetDt();
    float opacity = m_phase / m_period;
    if (opacity < 0.0f)
        opacity = 0.0f;
    else if (opacity > 1.0f)
        opacity = 1.0f;
    m_actor->SetOpacity(opacity);
    if (m_up) {
        m_phase += g_pTimer->GetDt();
        if (m_phase >= m_period)
            m_up = false;
    } else {
        m_phase -= g_pTimer->GetDt();
        if (m_phase <= 0.0f)
            m_up = true;
    }
}

CHANGETEXTUREBEHAVIOR::~CHANGETEXTUREBEHAVIOR()
{
    g_pScene->Remove(m_copy);
}

static BEHAVIOR *CreateBLINKRES(const RESHANDLE &owner, CMDCALL *call)
{
    return new BLINKRESBEHAVIOR(owner, call);
}

static BEHAVIOR *CreateORBIT(const RESHANDLE &owner, CMDCALL *call)
{
    return new ORBITBEHAVIOR(owner, call);
}

void TRAVELWANIMBEHAVIOR::Start()
{
    ACTOR *actor = GetActor();
    if (!actor) {
        m_done = true;
        return;
    }
    actor->SetBehavior(ACTORBEHAVIOR_LINEAL);
    actor->SetAceleration(V3(0.0f, 0.0f, 0.0f));
    actor->SetAngAceleration(V3(0.0f, 0.0f, 0.0f));
    actor->SetVel(V3(0.0f, 0.0f, 0.0f));
    actor->SetAngVel(V3(0.0f, 0.0f, 0.0f));
    m_state = TURNING;
    m_done = false;
    Turn(m_target - actor->GetPosition());
}

static BEHAVIOR *CreateCHANGETEXTURE(const RESHANDLE &owner, CMDCALL *call)
{
    return new CHANGETEXTUREBEHAVIOR(owner, call);
}

TRAVELWANIMBEHAVIOR::TRAVELWANIMBEHAVIOR(const RESHANDLE &owner, CMDCALL *call) : BEHAVIOR(owner)
{
    m_resource = atoi(call->tokens->GetToken(2));
    m_turnSpeed = (float)atof(call->tokens->GetToken(3));
    m_speed = (float)atof(call->tokens->GetToken(4));
    sscanf(call->tokens->GetToken(5), "%f, %f, %f", &m_target.x, &m_target.y, &m_target.z);
    sscanf(call->tokens->GetToken(6), "%f, %f, %f", &m_facing.x, &m_facing.y, &m_facing.z);
    m_turnLeft = call->tokens->GetToken(7);
    m_turnRight = call->tokens->GetToken(8);
    m_walk = call->tokens->GetToken(9);
    m_idle = call->tokens->GetToken(10);
    Start();
}

TRAVELWANIMBEHAVIOR::~TRAVELWANIMBEHAVIOR()
{
    Stop();
}

void TRAVELWANIMBEHAVIOR::Stop()
{
    ACTOR *actor = GetActor();
    if (actor) {
        actor->SetAceleration(V3(0.0f, 0.0f, 0.0f));
        actor->SetAngAceleration(V3(0.0f, 0.0f, 0.0f));
        actor->SetVel(V3(0.0f, 0.0f, 0.0f));
        actor->SetAngVel(V3(0.0f, 0.0f, 0.0f));
        actor->BlendResourceAnimation(m_resource, m_idle.c_str(), 0.1f, true, 1.0f);
        m_done = true;
    }
}

void TRAVELWANIMBEHAVIOR::Update()
{
    ACTOR *actor = GetActor();
    if (!actor) {
        m_done = true;
        return;
    }
    V3 toTarget = m_target - actor->GetPosition();
    switch (m_state) {
    case TURNING:
        if (IsFacing(toTarget))
            Walk();
        break;
    case WALKING:
        if (toTarget.Length() < 0.1f)
            Arrive();
        break;
    case FACING:
        if (IsFacing(m_facing))
            Stop();
        break;
    }
}

void TRAVELWANIMBEHAVIOR::Walk()
{
    ACTOR *actor = GetActor();
    if (actor) {
        actor->SetAngVel(V3(0.0f, 0.0f, 0.0f));
        m_state = WALKING;
        V3 direction = m_target - actor->GetPosition();
        direction.Normalize();
        actor->SetVel(direction * m_speed);
        actor->BlendResourceAnimation(m_resource, m_walk.c_str(), 0.1f, true, 1.0f);
    }
}

bool TRAVELWANIMBEHAVIOR::IsFacing(const V3 &direction)
{
    ACTOR *actor = GetActor();
    if (actor) {
        if (fabs(actor->m_angles.GetDirection().AngleY(direction)) < 5.0f)
            return true;
    }
    return false;
}

void TRAVELWANIMBEHAVIOR::Arrive()
{
    ACTOR *actor = GetActor();
    if (actor) {
        actor->SetVel(V3(0.0f, 0.0f, 0.0f));
        m_state = FACING;
        Turn(m_facing);
    }
}

void BEHAVIORLIST::Clear()
{
    if (m_list.size() == 0)
        return;
    for (int i = m_list.size() - 1; i >= 0; i--) {
        if (m_list[i]) {
            delete m_list[i];
            m_list[i] = 0;
        }
        if (!m_list.empty())
            m_list.pop_back();
    }
    m_list.clear();
}

void BEHAVIORLIST::Update()
{
    if (m_list.size() == 0)
        return;
    for (unsigned int i = 0; i < m_list.size(); i++) {
        BEHAVIOR *behavior = m_list[i];
        if (!behavior)
            continue;
        if (behavior->IsRunning()) {
            behavior->Update();
        } else {
            delete behavior;
            m_list[i] = m_list[m_list.size() - 1];
            if (!m_list.empty())
                m_list.pop_back();
            i--;
        }
    }
}

static BEHAVIOR *CreateTRAVELWANIM(const RESHANDLE &owner, CMDCALL *call)
{
    return new TRAVELWANIMBEHAVIOR(owner, call);
}

static BEHAVIORENTRY s_behaviors[] = {
    { "BLINK", 0, CreateBLINK },
    { "GROW", 0, CreateGROW },
    { "TRAVEL", 0, CreateTRAVEL },
    { "SMOOTHROTATE", 0, CreateSMOOTHROTATE },
    { "FLOWRADIUSTO", 0, CreateFLOWRADIUSTO },
    { "FALL", 0, CreateFALL },
    { "FALLWFX", 0, CreateFALLWFX },
    { "ORBIT", 0, CreateORBIT },
    { "TRAVELWANIM", 0, CreateTRAVELWANIM },
    { "FADERESOURCE", 0, CreateFADERESOURCE },
    { "INTERPOLATEDGROW", 0, CreateINTERPOLATEDGROW },
    { "CHANGETEXTURE", 0, CreateCHANGETEXTURE },
    { "GROWLOOP", 0, CreateGROWLOOP },
    { "FADERESOURCE", 0, CreateFADERESOURCE },
    { "FADERESLOOP", 0, CreateFADERESLOOP },
    { "GROWRESLOOP", 0, CreateGROWRESLOOP },
    { "BLINKRES", 0, CreateBLINKRES },
    { 0, 0, 0 },
};

GUIBEHAVIORS::GUIBEHAVIORS()
{
    for (BEHAVIORENTRY *entry = s_behaviors; entry->name; entry++)
        entry->hash = HashName(entry->name);
}

BEHAVIORENTRY *GUIBEHAVIORS::Find(const char *name)
{
    int hash = HashName(name);
    for (BEHAVIORENTRY *entry = s_behaviors; entry->name; entry++) {
        if (entry->hash == hash && !_stricmp(name, entry->name))
            return entry;
    }
    return 0;
}
