#include <stdlib.h>
#include <string.h>

#include "lib/freeimage/FreeImage.h"
#include "Tongas2/DECAL.h"
#include "Tongas2/EMITTER.h"
#include "Tongas2/FILELOADER.h"
#include "Tongas2/LOG.h"
#include "Tongas2/PATH.h"

// The names of the emit directions and emitted behaviours, as the
// EmitDirection and EmittedBehavior properties show them.
// A random value from min to max, and a random int from a to b.
#define RANDOM(min, max) \
    ((max) > (min) ? ((max) - (min)) * (float)rand() * (1.0f / RAND_MAX) + (min) : (min))
#define RANDOMINT(a, b)                                                            \
    ((b) > (a) ? (a) - (int)((float)rand() * (float)((b) - (a) + 1) * (-1.0f / 32768.0f)) \
               : (b) - (int)((float)rand() * (float)((a) - (b) + 1) * (-1.0f / 32768.0f)))

static const char s_directionNames[][30] = {
    "FW",
    "UP",
    "",
};

static const char s_behaviorNames[7][30] = {
    "Force",
    "PathFlow",
    "DragForce",
    "MagneticField",
    "",
};

int PARTICLE::PickPixel(EMITTERMASTER *master, EMITTER *emitter)
{
    int left = emitter->m_numPixels - emitter->m_numEmitted % emitter->m_numPixels;
    int n;
    if (left > 1)
        n = 1 - (int)((float)rand() * (float)left * (-1.0f / 32768.0f));
    else
        n = 1;
    bool pass = ((emitter->m_numEmitted / emitter->m_numPixels) & 1) != 0;
    int i = -1;
    int found = 0;
    do {
        if (emitter->m_pixelUsed[++i] == pass)
            found++;
    } while (found != n);
    return i;
}

void EMITTER::Init()
{
    OBJECT::Init();
    EMITTERMASTER *master = Master();
    Reset();
    ClearPixelsUsed();
    if (m_numPixels != master->m_numPixels) {
        if (m_pixelUsed) {
            delete[] m_pixelUsed;
            m_pixelUsed = 0;
        }
        m_pixelUsed = new bool[master->m_numPixels];
        for (int i = 0; i < master->m_numPixels; i++)
            m_pixelUsed[i] = false;
        m_numPixels = master->m_numPixels;
    }
}

void EMITTER::StartEmitting()
{
    EMITTERMASTER *master = Master();
    m_emitting = true;
    m_emitTime = master->m_timeToLive.Random();
    m_nextEmit = 0.0f;
    ClearPixelsUsed();
    m_numEmitted = 0;
}

void EMITTER::StopEmitting()
{
    m_emitting = false;
    m_rateScale = 1.0f;
}

void EMITTER::Start()
{
    EMITTERMASTER *master = Master();
    if (master)
        m_color = master->m_multiplyColor;
    StartEmitting();
    m_428 = false;
}

void EMITTER::Virtual31()
{
    if (!m_active) {
        Virtual20();
        return;
    }
    if (Master()->m_killResourceOnDetach)
        Virtual20();
    StopEmitting();
}

void EMITTER::NextEmitTime()
{
    EMITTERMASTER *master = Master();
    float rate = master->m_rate.Random() * m_rateScale;
    float perSecond = g_pResources->GetDetailScale() * rate;
    if (perSecond < 0.0001f)
        perSecond = 0.0001f;
    m_nextEmit += 1.0f / perSecond;
}

void EMITTERMASTER::RegisterCommands()
{
    g_classParents[s_classId] = RESOURCEMASTER::s_classId;
    g_classCommands[s_classId] = s_commands;
    HashCommands(s_commands);
    RegisterClassName(s_commands, "EMITTERMASTER", "RESOURCEMASTER");
}

void EMITTERMASTER::SetPathAdjustSize(const bool &value)
{
    m_pathAdjustSize = value;
}

class EMITTERMANAGER : public RESOURCEMANAGER
{
public:
    EMITTERMANAGER()
    {
        SafeSprintf(m_fileName, "EMITTER.txt");
        m_8 = false;
        m_18 = true;
    }

    virtual RESOURCEMASTER *CreateMaster(const char *name);
    virtual const char *GetTypeCode();
};

const char *EMITTERMANAGER::GetTypeCode()
{
    return "EMIT";
}

static RESOURCEMANAGER *CreateEMITTER()
{
    return new EMITTERMANAGER;
}

CREATEMANAGER g_pfnCreateEMITTER = CreateEMITTER;

CMDENTRY EMITTERMASTER::s_commands[] = {
    { 0, 0, 0, 0, 0 },
};

PROPENTRY EMITTERMASTER::s_properties[] = {
    { "Rate", 0, 10, 0, PROP_FIELD(EMITTERMASTER, m_rate), false, 1, PROP_SETTER(EMITTERMASTER, SetRate) },
    { "Force", 0, 11, 0, PROP_FIELD(EMITTERMASTER, m_force), false, 1, PROP_SETTER(EMITTERMASTER, SetForce) },
    { "EmitterSize", 0, 11, 0, PROP_FIELD(EMITTERMASTER, m_emitterSize), false, 1, PROP_SETTER(EMITTERMASTER, SetEmitterSize) },
    { "EmiterOffset", 0, 3, 0, PROP_FIELD(EMITTERMASTER, m_emiterOffset), false, 1, PROP_SETTER(EMITTERMASTER, SetEmiterOffset) },
    { "TimeToLive", 0, 10, 0, PROP_FIELD(EMITTERMASTER, m_timeToLive), false, 1, PROP_SETTER(EMITTERMASTER, SetTimeToLive) },
    { "AngRange", 0, 10, 0, PROP_FIELD(EMITTERMASTER, m_angRange), false, 1, PROP_SETTER(EMITTERMASTER, SetAngRange) },
    { "AngSpd", 0, 12, 0, PROP_FIELD(EMITTERMASTER, m_angSpd), false, 1, PROP_SETTER(EMITTERMASTER, SetAngSpd) },
    { "Life", 0, 10, 0, PROP_FIELD(EMITTERMASTER, m_life), false, 1, PROP_SETTER(EMITTERMASTER, SetLife) },
    { "SpeedRange", 0, 10, 0, PROP_FIELD(EMITTERMASTER, m_speedRange), false, 1, PROP_SETTER(EMITTERMASTER, SetSpeedRange) },
    { "AccelRange", 0, 10, 0, PROP_FIELD(EMITTERMASTER, m_accelRange), false, 1, PROP_SETTER(EMITTERMASTER, SetAccelRange) },
    { "AccelTime", 0, 3, 0, PROP_FIELD(EMITTERMASTER, m_accelTime), false, 1, PROP_SETTER(EMITTERMASTER, SetAccelTime) },
    { "RotateToMove", 0, 2, 0, PROP_FIELD(EMITTERMASTER, m_rotateToMove), false, 1, PROP_SETTER(EMITTERMASTER, SetRotateToMove) },
    { "Resource", 0, 7, 0, PROP_FIELD(EMITTERMASTER, m_resource), false, 1, PROP_SETTER(EMITTERMASTER, SetResource) },
    { "ResourcesSpecific", 0, 0, 0, PROP_FIELD(EMITTERMASTER, m_resourcesSpecific), false, 1, PROP_SETTER(EMITTERMASTER, SetResourcesSpecific) },
    { "EmitAttachedResources", 0, 2, 0, PROP_FIELD(EMITTERMASTER, m_emitAttachedResources), false, 1, PROP_SETTER(EMITTERMASTER, SetEmitAttachedResources) },
    { "KillResourceOnDetach", 0, 2, 0, PROP_FIELD(EMITTERMASTER, m_killResourceOnDetach), false, 1, PROP_SETTER(EMITTERMASTER, SetKillResourceOnDetach) },
    { "MultiplyColor", 0, 13, 0, PROP_FIELD(EMITTERMASTER, m_multiplyColor), false, 1, PROP_SETTER(EMITTERMASTER, SetMultiplyColor) },
    { "OneBigEmission", 0, 0, 0, PROP_FIELD(EMITTERMASTER, m_oneBigEmission), false, 1, PROP_SETTER(EMITTERMASTER, SetOneBigEmission) },
    { "ChangeColorInTime", 0, 2, 0, PROP_FIELD(EMITTERMASTER, m_changeColorInTime), false, 1, PROP_SETTER(EMITTERMASTER, SetChangeColorInTime) },
    { "MaxGlobalParticles", 0, 0, 0, PROP_FIELD(EMITTERMASTER, m_maxGlobalParticles), false, 1, PROP_SETTER(EMITTERMASTER, SetMaxGlobalParticles) },
    { "EmitDirection", 0, 1, s_directionNames, PROP_FIELD(EMITTERMASTER, m_emitDirection), false, 1, PROP_SETTER(EMITTERMASTER, SetEmitDirection) },
    { "InitialEmitTime", 0, 3, 0, PROP_FIELD(EMITTERMASTER, m_initialEmitTime), false, 1, PROP_SETTER(EMITTERMASTER, SetInitialEmitTime) },
    { "EmittedBehavior", 0, 1, s_behaviorNames, PROP_FIELD(EMITTERMASTER, m_emittedBehavior), false, 1, PROP_SETTER(EMITTERMASTER, SetEmittedBehavior) },
    { "BirthSize", 0, 10, 0, PROP_FIELD(EMITTERMASTER, m_birthSize), false, 1, PROP_SETTER(EMITTERMASTER, SetBirthSize) },
    { "BirthAlpha", 0, 10, 0, PROP_FIELD(EMITTERMASTER, m_birthAlpha), false, 1, PROP_SETTER(EMITTERMASTER, SetBirthAlpha) },
    { "BirthYaw", 0, 10, 0, PROP_FIELD(EMITTERMASTER, m_birthYaw), false, 1, PROP_SETTER(EMITTERMASTER, SetBirthYaw) },
    { "BirthPitch", 0, 10, 0, PROP_FIELD(EMITTERMASTER, m_birthPitch), false, 1, PROP_SETTER(EMITTERMASTER, SetBirthPitch) },
    { "BirthRoll", 0, 10, 0, PROP_FIELD(EMITTERMASTER, m_birthRoll), false, 1, PROP_SETTER(EMITTERMASTER, SetBirthRoll) },
    { "BirthColor1", 0, 13, 0, PROP_FIELD(EMITTERMASTER, m_birthColor1), false, 1, PROP_SETTER(EMITTERMASTER, SetBirthColor1) },
    { "BirthColor2", 0, 13, 0, PROP_FIELD(EMITTERMASTER, m_birthColor2), false, 1, PROP_SETTER(EMITTERMASTER, SetBirthColor2) },
    { "UseMidlife", 0, 2, 0, PROP_FIELD(EMITTERMASTER, m_useMidlife), false, 1, PROP_SETTER(EMITTERMASTER, SetUseMidlife) },
    { "MidLife", 0, 10, 0, PROP_FIELD(EMITTERMASTER, m_midLife), false, 1, PROP_SETTER(EMITTERMASTER, SetMidLife) },
    { "MidSize", 0, 10, 0, PROP_FIELD(EMITTERMASTER, m_midSize), false, 1, PROP_SETTER(EMITTERMASTER, SetMidSize) },
    { "MidAlpha", 0, 10, 0, PROP_FIELD(EMITTERMASTER, m_midAlpha), false, 1, PROP_SETTER(EMITTERMASTER, SetMidAlpha) },
    { "MidDuration", 0, 10, 0, PROP_FIELD(EMITTERMASTER, m_midDuration), false, 1, PROP_SETTER(EMITTERMASTER, SetMidDuration) },
    { "MidColor1", 0, 13, 0, PROP_FIELD(EMITTERMASTER, m_midColor1), false, 1, PROP_SETTER(EMITTERMASTER, SetMidColor1) },
    { "MidColor2", 0, 13, 0, PROP_FIELD(EMITTERMASTER, m_midColor2), false, 1, PROP_SETTER(EMITTERMASTER, SetMidColor2) },
    { "DeathSize", 0, 10, 0, PROP_FIELD(EMITTERMASTER, m_deathSize), false, 1, PROP_SETTER(EMITTERMASTER, SetDeathSize) },
    { "DeathAlpha", 0, 10, 0, PROP_FIELD(EMITTERMASTER, m_deathAlpha), false, 1, PROP_SETTER(EMITTERMASTER, SetDeathAlpha) },
    { "DeathColor1", 0, 13, 0, PROP_FIELD(EMITTERMASTER, m_deathColor1), false, 1, PROP_SETTER(EMITTERMASTER, SetDeathColor1) },
    { "DeathColor2", 0, 13, 0, PROP_FIELD(EMITTERMASTER, m_deathColor2), false, 1, PROP_SETTER(EMITTERMASTER, SetDeathColor2) },
    { "PathName", 0, 7, 0, PROP_FIELD(EMITTERMASTER, m_pathName), false, 1, PROP_SETTER(EMITTERMASTER, SetPathName) },
    { "PathRelativeRadius", 0, 10, 0, PROP_FIELD(EMITTERMASTER, m_pathRelativeRadius), false, 1, PROP_SETTER(EMITTERMASTER, SetPathRelativeRadius) },
    { "PathAdjustSize", 0, 2, 0, PROP_FIELD(EMITTERMASTER, m_pathAdjustSize), false, 1, PROP_SETTER(EMITTERMASTER, SetPathAdjustSize) },
    { "PathOpacityRange", 0, 11, 0, PROP_FIELD(EMITTERMASTER, m_pathOpacityRange), false, 1, PROP_SETTER(EMITTERMASTER, SetPathOpacityRange) },
    { "PathOpacity", 0, 2, 0, PROP_FIELD(EMITTERMASTER, m_pathOpacity), false, 1, PROP_SETTER(EMITTERMASTER, SetPathOpacity) },
    { "PathOffset", 0, 10, 0, PROP_FIELD(EMITTERMASTER, m_pathOffset), false, 1, PROP_SETTER(EMITTERMASTER, SetPathOffset) },
    { "PathScaleFactor", 0, 3, 0, PROP_FIELD(EMITTERMASTER, m_pathScaleFactor), false, 1, PROP_SETTER(EMITTERMASTER, SetPathScaleFactor) },
    { "PathRetimeFactor", 0, 3, 0, PROP_FIELD(EMITTERMASTER, m_pathRetimeFactor), false, 1, PROP_SETTER(EMITTERMASTER, SetPathRetimeFactor) },
    { "ExtForce", 0, 3, 0, PROP_FIELD(EMITTERMASTER, m_extForce), false, 1, PROP_SETTER(EMITTERMASTER, SetExtForce) },
    { "ExtDistanceMax", 0, 3, 0, PROP_FIELD(EMITTERMASTER, m_extDistanceMax), false, 1, PROP_SETTER(EMITTERMASTER, SetExtDistanceMax) },
    { "ExtDistanceMin", 0, 3, 0, PROP_FIELD(EMITTERMASTER, m_extDistanceMin), false, 1, PROP_SETTER(EMITTERMASTER, SetExtDistanceMin) },
    { "AccelerationAir", 0, 3, 0, PROP_FIELD(EMITTERMASTER, m_accelerationAir), false, 1, PROP_SETTER(EMITTERMASTER, SetAccelerationAir) },
    { "ExtForceActorName", 0, 9, 0, PROP_FIELD(EMITTERMASTER, m_extForceActorName), false, 1, PROP_SETTER(EMITTERMASTER, SetExtForceActorName) },
    { "PictureFileName", 0, 7, 0, PROP_FIELD(EMITTERMASTER, m_pictureFileName), false, 1, PROP_SETTER(EMITTERMASTER, SetPictureFileName) },
    { "UsePictureColor", 0, 2, 0, PROP_FIELD(EMITTERMASTER, m_usePictureColor), false, 1, PROP_SETTER(EMITTERMASTER, SetUsePictureColor) },
    { 0, 0, 15, 0, 0, false },
};

PARTICLE::~PARTICLE()
{
    OBJECT *resource = (OBJECT *)g_pResources->GetResource(m_resource);
    if (resource)
        resource->Detach();
    m_resource.Invalidate();
}

void EMITTER::KillParticles()
{
    PARTICLE *particle = m_particles;
    while (particle) {
        PARTICLE *next = particle->m_next;
        if (particle->m_prev)
            particle->m_prev->m_next = next;
        else
            m_particles = next;
        if (particle->m_next)
            particle->m_next->m_prev = particle->m_prev;
        delete particle;
        m_numParticles--;
        Master()->m_numParticles--;
        particle = next;
    }
}

EMITTERMASTER::~EMITTERMASTER()
{
    if (m_pixels) {
        delete[] m_pixels;
        m_pixels = 0;
    }
}

int EMITTERMASTER::GetClassId()
{
    return s_classId;
}

int EMITTERMASTER::GetParentClassId()
{
    return g_classParents[s_classId];
}

const char *EMITTERMASTER::GetClassName()
{
    return g_classNames[s_classId];
}

void EMITTERMASTER::SetRate(const EMITRANGE &value)
{
    m_rate = value;
}

void EMITTERMASTER::SetForce(const V3 &value)
{
    m_force = value;
}

void EMITTERMASTER::SetEmitterSize(const V3 &value)
{
    m_emitterSize = value;
}

void EMITTERMASTER::SetEmiterOffset(const float &value)
{
    m_emiterOffset = value;
}

void EMITTERMASTER::SetTimeToLive(const EMITRANGE &value)
{
    m_timeToLive = value;
}

void EMITTERMASTER::SetAngRange(const EMITRANGE &value)
{
    m_angRange = value;
}

void EMITTERMASTER::SetAngSpd(const ANG &value)
{
    m_angSpd = value;
}

void EMITTERMASTER::SetLife(const EMITRANGE &value)
{
    m_life = value;
}

void EMITTERMASTER::SetSpeedRange(const EMITRANGE &value)
{
    m_speedRange = value;
}

void EMITTERMASTER::SetAccelRange(const EMITRANGE &value)
{
    m_accelRange = value;
}

void EMITTERMASTER::SetAccelTime(const float &value)
{
    m_accelTime = value;
}

void EMITTERMASTER::SetRotateToMove(const bool &value)
{
    m_rotateToMove = value;
}

void EMITTERMASTER::SetResourcesSpecific(const int &value)
{
    m_resourcesSpecific = value;
}

void EMITTERMASTER::SetEmitAttachedResources(const bool &value)
{
    m_emitAttachedResources = value;
}

void EMITTERMASTER::SetKillResourceOnDetach(const bool &value)
{
    m_killResourceOnDetach = value;
}

void EMITTERMASTER::SetMultiplyColor(const V3I &value)
{
    m_multiplyColor = value;
}

void EMITTERMASTER::SetOneBigEmission(const int &value)
{
    m_oneBigEmission = value;
}

void EMITTERMASTER::SetChangeColorInTime(const bool &value)
{
    m_changeColorInTime = value;
}

void EMITTERMASTER::SetMaxGlobalParticles(const int &value)
{
    m_maxGlobalParticles = value;
}

void EMITTERMASTER::SetEmitDirection(const int &value)
{
    m_emitDirection = value;
}

void EMITTERMASTER::SetInitialEmitTime(const float &value)
{
    m_initialEmitTime = value;
}

void EMITTERMASTER::SetEmittedBehavior(const int &value)
{
    m_emittedBehavior = value;
}

void EMITTERMASTER::SetBirthSize(const EMITRANGE &value)
{
    m_birthSize = value;
}

void EMITTERMASTER::SetBirthAlpha(const EMITRANGE &value)
{
    m_birthAlpha = value;
}

void EMITTERMASTER::SetBirthYaw(const EMITRANGE &value)
{
    m_birthYaw = value;
}

void EMITTERMASTER::SetBirthPitch(const EMITRANGE &value)
{
    m_birthPitch = value;
}

void EMITTERMASTER::SetBirthRoll(const EMITRANGE &value)
{
    m_birthRoll = value;
}

void EMITTERMASTER::SetBirthColor1(const V3I &value)
{
    m_birthColor1 = value;
}

void EMITTERMASTER::SetBirthColor2(const V3I &value)
{
    m_birthColor2 = value;
}

void EMITTERMASTER::SetUseMidlife(const bool &value)
{
    m_useMidlife = value;
}

void EMITTERMASTER::SetMidLife(const EMITRANGE &value)
{
    m_midLife = value;
}

void EMITTERMASTER::SetMidSize(const EMITRANGE &value)
{
    m_midSize = value;
}

void EMITTERMASTER::SetMidAlpha(const EMITRANGE &value)
{
    m_midAlpha = value;
}

void EMITTERMASTER::SetMidDuration(const EMITRANGE &value)
{
    m_midDuration = value;
}

void EMITTERMASTER::SetMidColor1(const V3I &value)
{
    m_midColor1 = value;
}

void EMITTERMASTER::SetMidColor2(const V3I &value)
{
    m_midColor2 = value;
}

void EMITTERMASTER::SetDeathSize(const EMITRANGE &value)
{
    m_deathSize = value;
}

void EMITTERMASTER::SetDeathAlpha(const EMITRANGE &value)
{
    m_deathAlpha = value;
}

void EMITTERMASTER::SetDeathColor1(const V3I &value)
{
    m_deathColor1 = value;
}

void EMITTERMASTER::SetDeathColor2(const V3I &value)
{
    m_deathColor2 = value;
}

void EMITTERMASTER::SetPathRelativeRadius(const EMITRANGE &value)
{
    m_pathRelativeRadius = value;
}

void EMITTERMASTER::SetPathOpacityRange(const V3 &value)
{
    m_pathOpacityRange = value;
}

void EMITTERMASTER::SetPathOpacity(const bool &value)
{
    m_pathOpacity = value;
}

void EMITTERMASTER::SetPathOffset(const EMITRANGE &value)
{
    m_pathOffset = value;
}

void EMITTERMASTER::SetPathScaleFactor(const float &value)
{
    m_pathScaleFactor = value;
}

void EMITTERMASTER::SetPathRetimeFactor(const float &value)
{
    m_pathRetimeFactor = value;
}

void EMITTERMASTER::SetExtForce(const float &value)
{
    m_extForce = value;
}

void EMITTERMASTER::SetExtDistanceMax(const float &value)
{
    m_extDistanceMax = value;
}

void EMITTERMASTER::SetExtDistanceMin(const float &value)
{
    m_extDistanceMin = value;
}

void EMITTERMASTER::SetAccelerationAir(const float &value)
{
    m_accelerationAir = value;
}

void EMITTERMASTER::SetUsePictureColor(const bool &value)
{
    m_usePictureColor = value;
}

void EMITTERMASTER::SetExtForceActorName(const FIXEDSTRING<0x200> &value)
{
    m_extForceActorName = value;
}

void EMITTERMASTER::SetResource(const CSTRING &value)
{
    m_resource = value;
    m_resourceHandle = g_pResources->GetHandle(value.c_str());
}

void EMITTERMASTER::SetPathName(const CSTRING &value)
{
    m_pathName = value;
}

void PARTICLE::Update(const float &dt)
{
    EMITTERMASTER *master = m_emitter->Master();
    float retime = master->m_pathRetimeFactor;
    OBJECT *resource = (OBJECT *)g_pResources->GetResource(m_resource);
    if (!resource) {
        // Gone: let the emitter kill the particle.
        m_age = m_life + 1.0f;
        return;
    }
    float radius = 0.0f;

    if (master->m_emittedBehavior == EMITTERMASTER::BEHAVIOR_DRAGFORCE ||
        master->m_emittedBehavior == EMITTERMASTER::BEHAVIOR_MAGNETICFIELD) {
        // The pull of the force actor, and how far away it is.
        V3 force(0.0f, 0.0f, 0.0f);
        float dist;
        if (master->m_emittedBehavior == EMITTERMASTER::BEHAVIOR_DRAGFORCE) {
            WTACTOR *actor = g_pScene->FindActor(master->m_extForceActorName);
            if (actor) {
                float frameDt = g_pTimer->m_dt;
                force = (actor->GetWorldPosition() - actor->m_lastPosition) / frameDt;
                if (master->m_accelerationAir != 0.0f) {
                    float t = g_pTimer->m_dt;
                    m_velocity += t * (master->m_accelerationAir * m_velocity);
                }
                V3 actorPos = actor->GetWorldPosition();
                V3 d = resource->GetPosition() - actorPos;
                dist = d.Length();
                V3 n = force.Normalized();
                float dot = d.x * n.x + d.y * n.y + d.z * n.z;
                V3 proj = dot * force.Normalized();
                V3 perp = d - proj;
                V3 side;
                if (dot < 0.0f)
                    side = perp * -1.0f - proj;
                else
                    side = perp + proj;
                force = (side.Normalized() + force.Normalized()) * 0.5f * force.Length();
            }
        } else if (master->m_emittedBehavior == EMITTERMASTER::BEHAVIOR_MAGNETICFIELD) {
            WTACTOR *actor = g_pScene->FindActor(master->m_extForceActorName);
            if (actor)
                force = actor->GetWorldPosition() - resource->GetPosition();
            dist = force.Length();
        }
        if (m_attached) {
            WTACTOR *parent = g_pScene->GetActor(m_emitter->m_parent);
            if (parent)
                force = parent->WorldToLocal(force);
        }
        if (master->m_emittedBehavior == EMITTERMASTER::BEHAVIOR_DRAGFORCE) {
            if (dist > master->m_extDistanceMax) {
                force = V3(0.0f, 0.0f, 0.0f);
            } else if (dist < master->m_extDistanceMin) {
                force = force * master->m_extForce;
            } else {
                float range = master->m_extDistanceMax - master->m_extDistanceMin;
                force = force * (master->m_extDistanceMax - dist) / range * master->m_extForce;
            }
            m_velocity += force * dt;
            float maxSpeed = force.Length();
            if (m_velocity.Length() > maxSpeed)
                m_velocity = maxSpeed * m_velocity.Normalized();
        } else if (master->m_emittedBehavior == EMITTERMASTER::BEHAVIOR_MAGNETICFIELD) {
            if (dist > master->m_extDistanceMax) {
                force = V3(0.0f, 0.0f, 0.0f);
            } else if (dist < master->m_extDistanceMin) {
                force = force.Normalized() * master->m_extForce;
            } else {
                float range = master->m_extDistanceMax - master->m_extDistanceMin;
                float k = master->m_extDistanceMax - dist;
                force = k * force.Normalized() / range * master->m_extForce;
            }
            m_velocity += force * dt;
        }
    }

    if (master->m_emittedBehavior == EMITTERMASTER::BEHAVIOR_FORCE ||
        master->m_emittedBehavior == EMITTERMASTER::BEHAVIOR_DRAGFORCE ||
        master->m_emittedBehavior == EMITTERMASTER::BEHAVIOR_MAGNETICFIELD) {
        V3 step = m_velocity * dt;
        float ex, ey, ez;
        if (!m_attached) {
            V3 p = step * m_emitter->GetScale();
            resource->SetPosition(p + resource->GetPosition());
            resource->GetAngles().GetEuler(&ex, &ey, &ez);
        } else {
            m_offset += step * m_emitter->GetSize();
            resource->SetOffsetV(m_emitter->GetOffset() + m_offset);
            resource->GetRelAngles().GetEuler(&ex, &ey, &ez);
        }
        ex += dt * master->m_angSpd.GetEuler().x;
        ey += master->m_angSpd.GetEuler().y * dt;
        ez += master->m_angSpd.GetEuler().z * dt;
        if (!m_attached)
            resource->SetRotation(ex, ey, ez);
        else
            resource->SetRelRotation(ex, ey, ez);
        m_velocity += master->m_force * dt;
    } else if (master->m_emittedBehavior == EMITTERMASTER::BEHAVIOR_PATHFLOW) {
        if (!master->m_path) {
            master->m_path = (PATHMASTER *)g_pPATH->GetMaster(master->m_pathName.c_str());
            if (!master->m_path) {
                if (g_pLog)
                    g_pLog->Error("%s error loading path %s!", master->m_name.c_str(), master->m_pathName.c_str());
                return;
            }
        }
        V3 position(0.0f, 0.0f, 0.0f);
        V3 direction(0.0f, 0.0f, 0.0f);
        V3 up(0.0f, 0.0f, 0.0f);
        float roll = 0.0f;
        master->m_path->GetPoint(m_age / m_life + m_pathOffset, &position, &direction, &up, &radius, &roll);
        up.Normalize();
        direction.Normalize();
        float dot = up.x * direction.x + up.y * direction.y + up.z * direction.z;
        up = up - direction * dot;
        up.Normalize();
        float angle = (roll + m_pathAngle) * 0.017453292f;
        V3 out = up.Rotate(direction, angle);
        V3 p = out * (radius * m_pathRadius) + position;
        float ex, ey, ez;
        if (!m_attached) {
            resource->SetPosition(p);
            resource->GetAngles().GetEuler(&ex, &ey, &ez);
        } else {
            resource->SetOffsetV(p);
            resource->GetRelAngles().GetEuler(&ex, &ey, &ez);
        }
        ex += master->m_angSpd.GetEulerX() * dt * retime;
        ey += master->m_angSpd.GetEulerY() * dt * retime;
        ez += master->m_angSpd.GetEulerZ() * dt * retime;
        if (!m_attached)
            resource->SetRotation(ex, ey, ez);
        else
            resource->SetRelRotation(ex, ey, ez);
    }

    // Speeding up or slowing down for the first accelTime seconds.
    if (m_accel != 0.0f && master->m_accelTime > 0.0f && m_age <= master->m_accelTime) {
        float speed = m_velocity.Length() + m_distance * m_accel * dt;
        if (speed <= 0.0f) {
            speed = 0.0f;
            m_accel = 0.0f;
        }
        m_velocity = speed * m_velocity.Normalized();
    }

    // Size, alpha and colour, from birth through midlife to death.
    V3I color;
    float size, alpha;
    if (master->m_useMidlife) {
        if (m_age < m_midLife) {
            float t = m_age / m_midLife;
            float u = 1.0f - t;
            size = u * m_size + t * m_midSize;
            alpha = u * m_alpha + t * m_midAlpha;
            color = u * m_color + t * m_midColor;
        } else if (m_20 && m_midDuration <= 0.0f) {
            float t = (m_age - m_midLife) / (m_life - m_midLife);
            float u = 1.0f - t;
            size = u * m_midSize + t * m_deathSize;
            alpha = u * m_midAlpha + t * m_deathAlpha;
            color = u * m_midColor + t * m_deathColor;
        } else {
            size = m_midSize;
            alpha = m_midAlpha;
            m_20 = true;
            m_midDuration -= retime * dt;
            color = m_midColor;
            m_midLife = m_age;
        }
    } else {
        float t = m_age / m_life;
        float u = 1.0f - t;
        size = u * m_size + t * m_deathSize;
        alpha = u * m_alpha + t * m_deathAlpha;
        color = u * m_color + t * m_deathColor;
    }
    if (master->m_changeColorInTime && (!master->m_pictureLoaded || !master->m_usePictureColor)) {
        if (g_pDECAL->m_id == resource->GetManagerId())
            ((DECAL *)resource)->SetColor((const V3 &)color);
    }
    if (master->m_emittedBehavior == EMITTERMASTER::BEHAVIOR_PATHFLOW) {
        size *= master->m_pathScaleFactor;
        if (master->m_pathAdjustSize)
            size *= radius;
        if (master->m_pathOpacity) {
            float t = m_age / m_life;
            if (t < master->m_pathOpacityRange.x)
                alpha = 0.0f;
            if (master->m_pathOpacityRange.x <= t && t < master->m_pathOpacityRange.y)
                alpha = (t - master->m_pathOpacityRange.x) / (master->m_pathOpacityRange.y - master->m_pathOpacityRange.x) * alpha;
            if (master->m_pathOpacityRange.z <= t && t < 1.0f)
                alpha = (t - master->m_pathOpacityRange.z) / (1.0f - master->m_pathOpacityRange.z) * alpha;
        }
    }
    if (!m_attached) {
        resource->SetScaleV(size * m_emitter->GetScale());
        resource->SetAlpha(m_emitter->GetAlpha() * alpha);
    } else {
        resource->SetScaleFactorV(size * m_emitter->GetSize());
        resource->SetAlphaFactor(m_emitter->GetAlphaFactor() * alpha);
    }
    if (master->m_rotateToMove && !m_velocity.IsZero()) {
        ANG angles(0.0f, 0.0f, 0.0f);
        if (!m_attached) {
            angles = resource->GetAngles();
            angles.PointAt(m_velocity.Normalized());
            resource->SetAngles(angles);
        } else {
            angles = resource->GetRelAngles();
            angles.PointAt(m_velocity.Normalized());
            resource->SetRelAngles(angles);
        }
    }
    if (master->m_emittedBehavior != EMITTERMASTER::BEHAVIOR_PATHFLOW)
        m_age += dt;
    else
        m_age += retime * dt;
}

EMITTER::EMITTER(EMITTERMASTER *master, const RESHANDLE &handle)
    : OBJECT(master, handle)
{
    Reset();
    m_pixelUsed = 0;
    m_numPixels = master->m_numPixels;
    if (m_numPixels > 0) {
        m_pixelUsed = new bool[m_numPixels];
        for (int i = 0; i < m_numPixels; i++)
            m_pixelUsed[i] = false;
    }
}

void EMITTER::Virtual30()
{
    m_rateScale = 1.0f;
}

EMITTER::~EMITTER()
{
    KillParticles();
    if (m_pixelUsed)
        delete[] m_pixelUsed;
}

void EMITTER::Stop()
{
    if (!m_emitting)
        Virtual20();
    StopEmitting();
    KillParticles();
}

void EMITTER::Virtual20()
{
    KillParticles();
    OBJECT::Virtual20();
}

EMITTERMASTER::EMITTERMASTER(RESOURCEMANAGER *manager, const char *name)
    : RESOURCEMASTER(manager, name), m_angSpd(0.0f, 0.0f, 0.0f)
{
    m_rate = EMITRANGE(1.0f, 1.0f);
    m_force = V3(0.0f, 0.0f, 0.0f);
    m_emitterSize = V3(0.0f, 0.0f, 0.0f);
    m_timeToLive = EMITRANGE(0.0f, 0.0f);
    m_angRange = EMITRANGE(0.0f, 45.0f);
    m_angSpd = ANG(0.0f, 0.0f, 0.0f);
    m_life = EMITRANGE(1.0f, 5.0f);
    m_speedRange = EMITRANGE(1.0f, 2.0f);
    m_emiterOffset = 0.0f;
    m_accelRange = EMITRANGE(0.0f, 0.0f);
    m_accelTime = 0.0f;
    m_rotateToMove = false;
    m_resource = "";
    m_multiplyColor = V3I(255, 255, 255);
    m_birthSize = EMITRANGE(0.1f, 0.2f);
    m_birthAlpha = EMITRANGE(1.0f, 1.0f);
    m_birthYaw = EMITRANGE(0.0f, 0.0f);
    m_birthPitch = EMITRANGE(0.0f, 0.0f);
    m_birthRoll = EMITRANGE(0.0f, 0.0f);
    m_birthColor1 = V3I(0, 0, 0);
    m_birthColor2 = V3I(0, 0, 0);
    m_midLife = EMITRANGE(1.0f, 5.0f);
    m_midSize = EMITRANGE(0.1f, 0.2f);
    m_midAlpha = EMITRANGE(0.9f, 1.0f);
    m_midDuration = EMITRANGE(0.0f, 0.0f);
    m_resourcesSpecific = 0;
    m_emitAttachedResources = false;
    m_killResourceOnDetach = false;
    m_oneBigEmission = 0;
    m_changeColorInTime = false;
    m_maxGlobalParticles = 0;
    m_emitDirection = DIRECTION_UP;
    m_initialEmitTime = 0.0f;
    m_emittedBehavior = BEHAVIOR_FORCE;
    m_useMidlife = false;
    m_midColor1 = V3I(0, 0, 0);
    m_midColor2 = V3I(0, 0, 0);
    m_deathSize = EMITRANGE(0.1f, 0.2f);
    m_deathAlpha = EMITRANGE(0.0f, 0.0f);
    m_deathColor1 = V3I(0, 0, 0);
    m_deathColor2 = V3I(0, 0, 0);
    m_pathName = "";
    m_pathRelativeRadius = EMITRANGE(0.0f, 1.0f);
    m_pathAdjustSize = false;
    m_pathOpacityRange = V3(0.0f, 0.3f, 0.6f);
    m_pathOpacity = false;
    m_pathOffset = EMITRANGE(0.0f, 0.0f);
    m_pathScaleFactor = 1.0f;
    m_pathRetimeFactor = 1.0f;
    m_extForce = 1.0f;
    m_extDistanceMax = 50.0f;
    m_extDistanceMin = 0.0f;
    m_accelerationAir = 0.0f;
    m_extForceActorName.Set("");
    m_pictureFileName = "";
    m_usePictureColor = false;
    m_path = 0;
    m_pixels = 0;
    m_pictureLoaded = false;
    m_pictureWidth = 0;
    m_pictureHeight = 0;
    m_numPixels = 0;
    m_numParticles = 0;
}

FIBITMAP *EMITTERMASTER::LoadImage(const char *name)
{
    if (!name)
        return 0;
    int length = strlen(name);
    if (!length)
        return 0;
    FIBITMAP *bitmap = 0;
    FIXEDSTRING<0x200> path;
    SafeSprintf(path, "%s%s%s", g_basePath, "data/art/", name);
    _strlwr(path);
    path.Replace('/', '\\');
    FILELOADER *file = g_pfnCreateFileLoader();
    if (!file->Open(path)) {
        if (g_pLog)
            g_pLog->Error("Could not open file %s", (char *)path);
        delete file;
        return 0;
    }
    FIMEMORY *memory = FreeImage_OpenMemory((BYTE *)file->GetData(), file->GetSize());
    FREE_IMAGE_FORMAT format = FreeImage_GetFileTypeFromMemory(memory, 0);
    if (format == 1 || format == 0)
        bitmap = FreeImage_LoadFromMemory(format, memory, 2);
    else if (g_pLog)
        g_pLog->Error("Unsupported emitter image format, must be JPG or PNG! %s", (char *)path);
    FreeImage_CloseMemory(memory);
    file->Close();
    delete file;
    if (!bitmap)
        return 0;
    if (FreeImage_GetBPP(bitmap) != 24 && FreeImage_GetBPP(bitmap) != 32) {
        FIBITMAP *converted = FreeImage_ConvertTo24Bits(bitmap);
        FreeImage_Unload(bitmap);
        bitmap = converted;
    }
    return bitmap;
}

void EMITTERMASTER::LoadPicture()
{
    if (m_pixels) {
        delete[] m_pixels;
        m_pixels = 0;
    }
    m_pictureLoaded = false;
    m_pictureWidth = 0;
    m_pictureHeight = 0;
    m_numPixels = 0;
    if (!strlen(m_pictureFileName.c_str()))
        return;
    FIBITMAP *bitmap = LoadImage(m_pictureFileName.c_str());
    if (!bitmap) {
        if (g_pLog)
            g_pLog->Error("Could not load image: %s", m_pictureFileName.c_str());
        return;
    }
    m_pictureWidth = FreeImage_GetWidth(bitmap);
    m_pictureHeight = FreeImage_GetHeight(bitmap);
    if (m_pictureWidth > 256 || m_pictureHeight > 256) {
        if (g_pLog)
            g_pLog->Error("Image: %s (%d,%d) bigger than %d pixels", m_pictureFileName.c_str(), m_pictureWidth, m_pictureHeight, 256);
        FreeImage_Unload(bitmap);
        return;
    }
    EMITPIXEL *pixels = new EMITPIXEL[m_pictureWidth * m_pictureHeight];
    for (int y = 0; y < m_pictureHeight; y++) {
        for (int x = 0; x < m_pictureWidth; x++) {
            RGBQUAD color;
            FreeImage_GetPixelColor(bitmap, x, m_pictureHeight - y - 1, &color);
            if (color.rgbRed || color.rgbGreen || color.rgbBlue) {
                EMITPIXEL &pixel = pixels[m_numPixels];
                pixel.u = (float)x / (float)(m_pictureWidth - 1);
                pixel.v = (float)y / (float)(m_pictureHeight - 1);
                pixels[m_numPixels].color.x = color.rgbRed;
                pixels[m_numPixels].color.y = color.rgbGreen;
                pixels[m_numPixels].color.z = color.rgbBlue;
                m_numPixels++;
            }
        }
    }
    FreeImage_Unload(bitmap);
    if (m_numPixels > 0) {
        m_pixels = new EMITPIXEL[m_numPixels];
        memcpy(m_pixels, pixels, m_numPixels * sizeof(EMITPIXEL));
        m_pictureLoaded = true;
        if (m_emitterSize.x == 0.0f && m_emitterSize.y == 0.0f && m_emitterSize.z == 0.0f) {
            m_emitterSize.x = 1.0f;
            m_emitterSize.y = 1.0f;
            m_emitterSize.z = 1.0f;
        }
    }
    if (pixels)
        delete[] pixels;
}

RESOURCE *EMITTERMASTER::CreateCopy(const RESHANDLE &handle)
{
    return new EMITTER(this, handle);
}

SRESULT EMITTERMASTER::Execute(CMDENTRY *command, TOKENS **args)
{
    return (this->*(SRESULT (EMITTERMASTER::*)(TOKENS **))command->func)(args);
}

RESOURCEMASTER *EMITTERMANAGER::CreateMaster(const char *name)
{
    return new EMITTERMASTER(this, name);
}

PARTICLE::PARTICLE(EMITTER *emitter)
    : m_emitter(emitter), m_next(0), m_prev(0)
{
    EMITTERMASTER *master = emitter->Master();
    if (!master->m_resourceHandle.IsValid())
        master->SetResource(master->m_resource);
    OBJECT *resource = (OBJECT *)g_pResources->GetResource(master->m_resourceHandle);
    if (!resource) {
        if (g_pLog)
            g_pLog->Error("Trying to emit an invalid resource, master: '%s'", master->m_name.c_str());
        return;
    }
    resource->SetFrame(master->m_resourcesSpecific);
    if (master->m_emitAttachedResources && emitter->m_parent.IsValid()) {
        resource->SetParent(emitter->m_parent);
        if (emitter->m_attached.IsValid()) {
            WTACTOR *actor = (WTACTOR *)g_pResources->GetResource(emitter->m_attached);
            if (actor)
                resource->AttachTo(actor, emitter->m_tagName);
        }
        m_attached = true;
    } else {
        m_attached = false;
    }
    m_resource = resource->m_handle;

    // Where in the emitter the particle starts.
    V3 pos(0.0f, 0.0f, 0.0f);
    int pixel;
    if (!master->m_pictureLoaded) {
        pos.x = RANDOM(master->m_emitterSize.x * -0.5f, master->m_emitterSize.x * 0.5f);
        pos.y = RANDOM(master->m_emitterSize.y * -0.5f, master->m_emitterSize.y * 0.5f);
        pos.z = RANDOM(master->m_emitterSize.z * -0.5f, master->m_emitterSize.z * 0.5f);
    } else {
        pixel = PickPixel(master, emitter);
        emitter->m_pixelUsed[pixel] = !emitter->m_pixelUsed[pixel];
        if (master->m_emitDirection == EMITTERMASTER::DIRECTION_FW) {
            pos.z = 0.0f;
            pos.x = -((master->m_pixels[pixel].u - 0.5f) * master->m_emitterSize.x);
            pos.y = -((master->m_pixels[pixel].v - 0.5f) * master->m_emitterSize.y);
        } else if (master->m_emitDirection == EMITTERMASTER::DIRECTION_UP) {
            pos.x = (master->m_pixels[pixel].u - 0.5f) * master->m_emitterSize.x;
            pos.y = 0.0f;
            pos.z = -((master->m_pixels[pixel].v - 0.5f) * master->m_emitterSize.z);
        }
    }
    if (!m_attached) {
        V3 p = pos * emitter->GetScale();
        V3 at = emitter->GetPosition();
        pos = at + emitter->GetAngles().Transform(p);
        resource->SetPosition(pos);
        resource->m_fadeDistance = emitter->m_fadeDistance;
        resource->SetLayer(emitter->GetLayer());
    } else {
        pos = pos * emitter->GetSize();
        m_offset = emitter->GetRelAngles().Transform(pos);
        resource->SetOffsetV(m_offset + m_emitter->GetOffset());
        resource->m_fadeDistance = emitter->m_fadeDistance;
        resource->SetRelLayer(emitter->GetRelLayer());
    }

    m_size = master->m_birthSize.Random();
    m_alpha = master->m_birthAlpha.Random();
    m_color.x = RANDOMINT(master->m_birthColor1.x, master->m_birthColor2.x);
    m_color.y = RANDOMINT(master->m_birthColor1.y, master->m_birthColor2.y);
    m_color.z = RANDOMINT(master->m_birthColor1.z, master->m_birthColor2.z);
    m_deathSize = master->m_deathSize.Random();
    m_deathAlpha = master->m_deathAlpha.Random();
    m_deathColor.x = RANDOMINT(master->m_deathColor1.x, master->m_deathColor2.x);
    m_deathColor.y = RANDOMINT(master->m_deathColor1.y, master->m_deathColor2.y);
    m_deathColor.z = RANDOMINT(master->m_deathColor1.z, master->m_deathColor2.z);
    m_life = master->m_life.Random();
    if (!m_attached) {
        resource->SetScaleV(emitter->GetScale() * m_size);
        resource->SetAlpha(emitter->GetAlpha() * m_alpha);
    } else {
        resource->SetScaleFactorV(emitter->GetSize() * m_size);
        resource->SetAlphaFactor(emitter->GetAlphaFactor() * m_alpha);
    }
    if (master->m_useMidlife) {
        m_midSize = master->m_midSize.Random();
        m_midAlpha = master->m_midAlpha.Random();
        m_midLife = master->m_midLife.Random();
        m_midDuration = master->m_midDuration.Random();
        m_midColor.x = RANDOMINT(master->m_midColor1.x, master->m_midColor2.x);
        m_midColor.y = RANDOMINT(master->m_midColor1.y, master->m_midColor2.y);
        m_midColor.z = RANDOMINT(master->m_midColor1.z, master->m_midColor2.z);
    }
    m_age = 0.0f;
    m_accel = master->m_accelRange.Random();
    m_distance = 1.0f;

    // Which way the particle heads.
    ANG angles(0.0f, 0.0f, 0.0f);
    float speed = master->m_speedRange.Random();
    switch (master->m_emitDirection) {
    case EMITTERMASTER::DIRECTION_FW: {
        float angle = master->m_angRange.Random();
        angles = ANG(0.0f, angle, (float)rand() * (360.0f / RAND_MAX) - 180.0f);
        if (!master->m_pictureLoaded) {
            m_velocity = angles.Transform(V3(0.0f, 0.0f, speed));
        } else if (!m_attached) {
            m_velocity = (pos - emitter->GetPosition()) * speed;
            m_distance = (pos - emitter->GetPosition()).Length();
        } else {
            m_velocity = (pos - emitter->GetOffset()) * speed;
            m_distance = (pos - emitter->GetOffset()).Length();
        }
        break;
    }
    case EMITTERMASTER::DIRECTION_UP: {
        float angle = master->m_angRange.Random();
        angles = ANG((float)rand() * (360.0f / RAND_MAX) - 180.0f, 0.0f, angle);
        if (!master->m_pictureLoaded) {
            m_velocity = angles.Transform(V3(0.0f, speed, 0.0f));
        } else if (!m_attached) {
            m_velocity = (pos - emitter->GetPosition()) * speed;
            m_distance = (pos - emitter->GetPosition()).Length();
        } else {
            m_velocity = (pos - emitter->GetOffset()) * speed;
            m_distance = (pos - emitter->GetOffset()).Length();
        }
        break;
    }
    }

    if (master->m_emiterOffset != 0.0f) {
        V3 offset = angles.Transform(V3(0.0f, master->m_emiterOffset, 0.0f));
        if (!m_attached)
            resource->SetPosition(emitter->GetAngles().Transform(offset) + resource->m_position);
        else
            resource->SetOffsetV(emitter->GetRelAngles().Transform(offset) + resource->m_position);
    }
    if (!master->m_pictureLoaded) {
        if (!m_attached)
            m_velocity = emitter->GetAngles().Transform(m_velocity);
        else
            m_velocity = emitter->GetRelAngles().Transform(m_velocity);
    }
    if (master->m_rotateToMove) {
        ANG facing(0.0f, 0.0f, 0.0f);
        if (!m_attached)
            resource->GetAngles();
        else
            resource->GetRelAngles();
        facing.PointAt(m_velocity.Normalized());
        resource->SetAngles(facing);
    } else {
        resource->SetRotation(master->m_birthYaw.Random(), master->m_birthPitch.Random(), master->m_birthRoll.Random());
    }
    m_20 = false;
    m_pathRadius = master->m_pathRelativeRadius.Random();
    m_pathAngle = (float)rand() * (360.0f / RAND_MAX);
    m_pathOffset = master->m_pathOffset.Random();
    resource->SetActive(true);
    if (g_pBONEDMODEL->m_id == resource->GetManagerId()) {
        ((BONEDMODEL *)resource)->Virtual78();
    } else if (g_pDECAL->m_id == resource->GetManagerId()) {
        if (master->m_pictureLoaded && master->m_usePictureColor)
            ((DECAL *)resource)->SetColor((const V3 &)master->m_pixels[pixel].color);
        else if (master->m_changeColorInTime)
            ((DECAL *)resource)->SetColor((const V3 &)m_color);
        else {
            V3I color = emitter->m_color;
            ((DECAL *)resource)->SetColor((const V3 &)color);
        }
    }
    if (master->m_emittedBehavior == EMITTERMASTER::BEHAVIOR_PATHFLOW) {
        float dt = 0.0f;
        Update(dt);
    }
}

void EMITTER::Emit()
{
    EMITTERMASTER *master = Master();
    if (master->m_maxGlobalParticles <= 0 || master->m_numParticles < master->m_maxGlobalParticles) {
        master->m_numParticles++;
        PARTICLE *particle = new PARTICLE(this);
        particle->m_next = m_particles;
        if (m_particles)
            m_particles->m_prev = particle;
        m_particles = particle;
        m_numParticles++;
        m_numEmitted++;
    }
}

void EMITTERMASTER::SetPictureFileName(const CSTRING &value)
{
    m_pictureFileName = value;
    LoadPicture();
}

void EMITTER::Update()
{
    OBJECT::Update();
    if (!m_active)
        return;
    EMITTERMASTER *master = Master();
    float initialTime = 0.0f;
    if (!m_428 && master->m_initialEmitTime > 0.0f) {
        m_429 = true;
        m_42c = 0.1f;
        initialTime = master->m_initialEmitTime;
    }
    for (;;) {
        if (m_emitting && master->m_oneBigEmission > 0) {
            for (int i = 0; i < master->m_oneBigEmission; i++)
                Emit();
            StopEmitting();
        }
        float dt;
        if (m_429)
            dt = m_42c;
        else
            dt = g_pTimer->m_dt;
        if (dt <= 0.0f)
            return;
        if (m_emitting) {
            m_nextEmit -= dt;
            if (m_nextEmit <= 0.0f) {
                do {
                    Emit();
                    NextEmitTime();
                } while (m_nextEmit <= 0.0f);
            }
        } else if (!m_numParticles) {
            Virtual20();
            return;
        }
        PARTICLE *particle = m_particles;
        while (particle) {
            PARTICLE *next = particle->m_next;
            particle->Update(dt);
            if (particle->m_age > particle->m_life) {
                if (particle->m_prev)
                    particle->m_prev->m_next = particle->m_next;
                else
                    m_particles = particle->m_next;
                if (particle->m_next)
                    particle->m_next->m_prev = particle->m_prev;
                delete particle;
                m_numParticles--;
                Master()->m_numParticles--;
            }
            particle = next;
        }
        if (!m_429 || initialTime <= 0.0f)
            break;
        m_428 = true;
        initialTime -= 0.1f;
        m_42c = 0.1f;
    }
    m_429 = false;
    if (m_emitTime > 0.0f) {
        m_emitTime -= g_pTimer->m_dt;
        if (m_emitTime <= 0.0f)
            StopEmitting();
    }
}
