#include <string.h>

#include "Tongas2/SOUND.h"

static RESOURCEMANAGER *CreateSOUND();

CREATEMANAGER g_pfnCreateSOUND = CreateSOUND;

CMDENTRY SOUNDMASTER::s_commands[] = {
    { 0, 0, 0, 0, 0 },
};

PROPENTRY SOUNDMASTER::s_properties[] = {
    { "FileName", 0, 7, 0, PROP_FIELD(SOUNDMASTER, m_soundFile), false, 1, PROP_SETTER(SOUNDMASTER, SetFileName) },
    { "Loop", 0, 2, 0, PROP_FIELD(SOUNDMASTER, m_loop), false, 1, PROP_SETTER(SOUNDMASTER, SetLoop) },
    { "Volume", 0, 0, 0, PROP_FIELD(SOUNDMASTER, m_volume), false, 1, PROP_SETTER(SOUNDMASTER, SetVolume) },
    { "Rate", 0, 3, 0, PROP_FIELD(SOUNDMASTER, m_rate), false, 1, PROP_SETTER(SOUNDMASTER, SetRate) },
    { "Stream", 0, 2, 0, PROP_FIELD(SOUNDMASTER, m_stream), false, 1, PROP_SETTER(SOUNDMASTER, SetStream) },
    { "PlayerDistVolume", 0, 2, 0, PROP_FIELD(SOUNDMASTER, m_playerDistVolume), false, 1, PROP_SETTER(SOUNDMASTER, SetPlayerDistVolume) },
    { "UseProjectedDistance", 0, 2, 0, PROP_FIELD(SOUNDMASTER, m_useProjectedDistance), false, 1, PROP_SETTER(SOUNDMASTER, SetUseProjectedDistance) },
    { "MinPlayerDist", 0, 3, 0, PROP_FIELD(SOUNDMASTER, m_minPlayerDist), false, 1, PROP_SETTER(SOUNDMASTER, SetMinPlayerDist) },
    { "MaxPlayerDist", 0, 3, 0, PROP_FIELD(SOUNDMASTER, m_maxPlayerDist), false, 1, PROP_SETTER(SOUNDMASTER, SetMaxPlayerDist) },
    { "Priority", 0, 0, 0, PROP_FIELD(SOUNDMASTER, m_priority), false, 1, PROP_SETTER(SOUNDMASTER, SetPriority) },
    { "SingleInstance", 0, 2, 0, PROP_FIELD(SOUNDMASTER, m_singleInstance), false, 1, PROP_SETTER(SOUNDMASTER, SetSingleInstance) },
    { "ExclusiveGroup", 0, 0, 0, PROP_FIELD(SOUNDMASTER, m_exclusiveGroup), false, 1, PROP_SETTER(SOUNDMASTER, SetExclusiveGroup) },
    { "StreamMpegAccurate", 0, 2, 0, PROP_FIELD(SOUNDMASTER, m_streamMpegAccurate), false, 1, PROP_SETTER(SOUNDMASTER, SetStreamMpegAccurate) },
    { "DieOnDetach", 0, 2, 0, PROP_FIELD(SOUNDMASTER, m_dieOnDetach), false, 1, PROP_SETTER(SOUNDMASTER, SetDieOnDetach) },
    { "DieFadeTime", 0, 3, 0, PROP_FIELD(SOUNDMASTER, m_dieFadeTime), false, 1, PROP_SETTER(SOUNDMASTER, SetDieFadeTime) },
    { "FadeOnPause", 0, 2, 0, PROP_FIELD(SOUNDMASTER, m_fadeOnPause), false, 1, PROP_SETTER(SOUNDMASTER, SetFadeOnPause) },
    { "PauseFadeTime", 0, 3, 0, PROP_FIELD(SOUNDMASTER, m_pauseFadeTime), false, 1, PROP_SETTER(SOUNDMASTER, SetPauseFadeTime) },
    { "CanPauseStream", 0, 2, 0, PROP_FIELD(SOUNDMASTER, m_canPauseStream), false, 1, PROP_SETTER(SOUNDMASTER, SetCanPauseStream) },
    { 0, 0, 15, 0, 0, false },
};

void SOUND::Virtual30()
{
    m_dying = false;
    m_dieTime = 0.0f;
}

void SOUND::Stop()
{
    if (m_dying)
        Virtual20();
}

void SOUND::Virtual31()
{
    if (!m_active) {
        Virtual20();
        return;
    }
    SOUNDMASTER *master = Master();
    if (master->m_loop || master->m_dieOnDetach) {
        if (m_dying) {
            if (m_dieTime <= 0.0f)
                Virtual20();
        } else {
            m_dieTime = master->m_dieFadeTime;
            m_dying = true;
        }
    }
}

V3 SOUND::GetListenerPosition()
{
    return g_pCamera->GetWorldPosition();
}

void SOUND::FollowParent(WTACTOR *parent)
{
    OBJECT::FollowParent(parent);
    if (parent && parent->m_16c)
        m_400 = true;
}

void SOUNDMASTER::RegisterCommands()
{
    g_classParents[s_classId] = RESOURCEMASTER::s_classId;
    g_classCommands[s_classId] = s_commands;
    HashCommands(s_commands);
    RegisterClassName(s_commands, "SOUNDMASTER", "RESOURCEMASTER");
}

void SOUNDMANAGER::Init()
{
    RESOURCEMANAGER::Init();
}

void SOUNDMANAGER::EnableSounds(bool enable)
{
    SOUNDMASTER *master = (SOUNDMASTER *)m_first;
    while (master) {
        SOUNDMASTER *next = (SOUNDMASTER *)master->m_next;
        if (!master->m_stream) {
            for (RESOURCE *copy = master->m_used; copy; copy = copy->m_next)
                ((SOUND *)copy)->m_enabled = enable;
        }
        master = next;
    }
    m_soundsEnabled = enable;
}

void SOUNDMANAGER::EnableStreams(bool enable)
{
    SOUNDMASTER *master = (SOUNDMASTER *)m_first;
    while (master) {
        SOUNDMASTER *next = (SOUNDMASTER *)master->m_next;
        if (master->m_stream) {
            for (RESOURCE *copy = master->m_used; copy; copy = copy->m_next)
                ((SOUND *)copy)->m_enabled = enable;
        }
        master = next;
    }
    m_streamsEnabled = enable;
}

SOUND::~SOUND()
{
}

void SOUND::Virtual20()
{
    m_distVolume = 1.0f;
    m_volumeScale = 1.0f;
    m_40c = -1.0f;
    m_dying = false;
    m_dieTime = 0.0f;
    m_41c = -1.0f;
    m_420 = 1.0f;
    m_410 = 0;
    OBJECT::Virtual20();
}

SOUNDMASTER::SOUNDMASTER(RESOURCEMANAGER *manager, const char *name)
    : RESOURCEMASTER(manager, name)
{
    m_soundFile = "";
    m_loop = false;
    m_stream = false;
    m_playerDistVolume = false;
    m_useProjectedDistance = false;
    m_minPlayerDist = 0.0f;
    m_maxPlayerDist = 0.0f;
    m_singleInstance = false;
    m_streamMpegAccurate = false;
    m_dieOnDetach = false;
    m_dieFadeTime = 0.0f;
    m_fadeOnPause = false;
    m_canPauseStream = false;
    m_volume = 127;
    m_rate = 1.0f;
    m_priority = 128;
    m_exclusiveGroup = -1;
    m_pauseFadeTime = 0.3f;
}

int SOUNDMASTER::GetClassId()
{
    return s_classId;
}

int SOUNDMASTER::GetParentClassId()
{
    return g_classParents[s_classId];
}

const char *SOUNDMASTER::GetClassName()
{
    return g_classNames[s_classId];
}

SOUND::SOUND(SOUNDMASTER *master, const RESHANDLE &handle)
    : OBJECT(master, handle), m_400(false), m_distVolume(1.0f), m_volumeScale(1.0f), m_40c(-1.0f), m_410(0), m_enabled(true),
      m_415(false), m_dying(false), m_dieTime(0.0f), m_41c(-1.0f), m_420(1.0f)
{
}

void SOUND::Update()
{
    OBJECT::Update();
    if (!m_active)
        return;
    SOUNDMASTER *master = Master();
    float scale;
    if (m_dying) {
        m_dieTime -= g_pTimer->m_soundDt;
        if (m_dieTime <= 0.0f) {
            Virtual31();
            return;
        }
        scale = m_dieTime / master->m_dieFadeTime;
    } else {
        m_distVolume = 1.0f;
        if (master->m_playerDistVolume) {
            V3 listener = GetListenerPosition();
            float distance;
            if (master->m_useProjectedDistance) {
                PLANE plane(GetAngles().GetDirection(), GetPosition());
                distance = plane.Distance(listener, 0, 0);
            } else
                distance = GetPosition().Distance(listener);
            if (distance >= master->m_maxPlayerDist)
                m_distVolume = 0.0f;
            else if (distance <= master->m_minPlayerDist)
                m_distVolume = 1.0f;
            else
                m_distVolume = (master->m_maxPlayerDist - distance) / (master->m_maxPlayerDist - master->m_minPlayerDist);
        }
        scale = m_volumeScale;
    }
    m_distVolume = scale * m_distVolume;
}

void SOUNDMASTER::SetFileName(const CSTRING &name)
{
    m_soundFile = name;
    FIXEDSTRING<0x200> path;
    SafeSprintf(path, "%s%s%s", g_basePath, "data/sound/", m_soundFile.c_str());
    ReplaceChar(path, '\\', '/');
    m_path = path;
}

RESOURCE *SOUNDMASTER::CreateCopy(const RESHANDLE &handle)
{
    return new SOUND(this, handle);
}

SRESULT SOUNDMASTER::Execute(CMDENTRY *command, TOKENS **args)
{
    return (this->*(SRESULT (SOUNDMASTER::*)(TOKENS **))command->func)(args);
}

SOUNDMANAGER::~SOUNDMANAGER()
{
}

void SOUNDMANAGER::PlayExclusive(SOUND *sound)
{
    SOUNDMASTER *master = sound->Master();
    int group = master->m_exclusiveGroup;
    std::map<int, RESHANDLE>::iterator it = m_exclusive.find(group);
    if (it != m_exclusive.end()) {
        if (master->m_singleInstance) {
            RESOURCE *playing = g_pResources->GetResource(it->second);
            if (playing && playing->m_master == master)
                return;
        }
        RESOURCE *playing = g_pResources->GetResource(it->second);
        if (playing)
            playing->Virtual20();
        it->second.Invalidate();
    }
    m_exclusive[group] = sound->m_handle;
}

static RESOURCEMANAGER *CreateSOUND()
{
    return new SOUNDMANAGER;
}

void SOUND::Start()
{
    if (Master()->m_exclusiveGroup >= 0)
        ((SOUNDMANAGER *)g_pSOUND)->PlayExclusive(this);
}
