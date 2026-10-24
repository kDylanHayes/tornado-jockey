// Sounds: SOUNDMASTER holds what a sound type's file sets (the wave file,
// volume, rate, how it fades with distance and when it dies), SOUND is one
// playing copy and SOUNDMANAGER keeps the sounds of each exclusive group
// from playing together. Placeholder names apart from the class names and
// properties.
#ifndef TONGAS2_SOUND_H
#define TONGAS2_SOUND_H

#include <map>

#include "Tongas2/RESOURCE.h"
#include "Tongas2/RESOURCES.h"
#include "Tongas2/TIMER.h"

// Defined elsewhere: the folder the game was started from.
extern char g_basePath[];

// Defined elsewhere: replaces each from in s with to.
void ReplaceChar(char *s, char from, char to);

class SOUNDMASTER : public RESOURCEMASTER
{
public:
    SOUNDMASTER(RESOURCEMANAGER *manager, const char *name);
    virtual ~SOUNDMASTER() {}

    static void RegisterCommands();

    virtual int GetClassId();
    virtual int GetParentClassId();
    virtual const char *GetClassName();
    virtual SRESULT Execute(CMDENTRY *command, TOKENS **args);
    virtual RESOURCE *CreateCopy(const RESHANDLE &handle);

    // Sets the file, and the path under data/sound/ it is loaded from.
    virtual void SetFileName(const CSTRING &name);
    virtual void SetLoop(const bool &value) { m_loop = value; }
    virtual void SetVolume(const int &value) { m_volume = value; }
    virtual void SetRate(const float &value) { m_rate = value; }
    virtual void SetStream(const bool &value) { m_stream = value; }
    virtual void SetPlayerDistVolume(const bool &value) { m_playerDistVolume = value; }
    virtual void SetUseProjectedDistance(const bool &value) { m_useProjectedDistance = value; }
    virtual void SetMinPlayerDist(const float &value) { m_minPlayerDist = value; }
    virtual void SetMaxPlayerDist(const float &value) { m_maxPlayerDist = value; }
    virtual void SetPriority(const int &value) { m_priority = value; }
    virtual void SetSingleInstance(const bool &value) { m_singleInstance = value; }
    virtual void SetExclusiveGroup(const int &value) { m_exclusiveGroup = value; }
    virtual void SetStreamMpegAccurate(const bool &value) { m_streamMpegAccurate = value; }
    virtual void SetDieOnDetach(const bool &value) { m_dieOnDetach = value; }
    virtual void SetDieFadeTime(const float &value) { m_dieFadeTime = value; }
    virtual void SetFadeOnPause(const bool &value) { m_fadeOnPause = value; }
    virtual void SetPauseFadeTime(const float &value) { m_pauseFadeTime = value; }
    virtual void SetCanPauseStream(const bool &value) { m_canPauseStream = value; }

    static int s_classId;
    static CMDENTRY s_commands[];
    static PROPENTRY s_properties[];

    CSTRING m_soundFile;
    bool m_loop;
    int m_volume;
    float m_rate;
    bool m_stream;
    bool m_playerDistVolume;
    bool m_useProjectedDistance;
    float m_minPlayerDist;
    float m_maxPlayerDist;
    int m_priority;
    bool m_singleInstance;
    int m_exclusiveGroup;
    bool m_streamMpegAccurate;
    bool m_dieOnDetach;
    float m_dieFadeTime;
    bool m_fadeOnPause;
    float m_pauseFadeTime;
    bool m_canPauseStream;
    // Where the file is loaded from.
    CSTRING m_path;
};

class SOUND : public OBJECT
{
public:
    SOUND(SOUNDMASTER *master, const RESHANDLE &handle);
    virtual ~SOUND();

    virtual void Virtual20();
    // Takes the place of the sound playing in its exclusive group.
    virtual void Start();
    // Stops at once if it is dying.
    virtual void Stop();
    // Fades with the distance to the listener, or while dying.
    virtual void Update();
    virtual bool IsLoaded() { return true; }
    virtual void Virtual30();
    // Starts dying (if it loops or dies on detach) once let go.
    virtual void Virtual31();
    virtual V3 GetScaleFactor() { return V3(0.0f, 0.0f, 0.0f); }
    virtual void FollowParent(WTACTOR *parent);
    virtual void SetFrame(int frame) { m_410 = frame; }
    // Where the sound is heard from.
    virtual V3 GetListenerPosition();

    SOUNDMASTER *Master() { return (SOUNDMASTER *)m_master; }

    bool m_400;
    // The volume, from 0 to 1, after the distance fade.
    float m_distVolume;
    float m_volumeScale;
    float m_40c;
    int m_410;
    bool m_enabled;
    bool m_415;
    bool m_dying;
    // How much of the fade out is left.
    float m_dieTime;
    float m_41c;
    float m_420;
};

class SOUNDMANAGER : public RESOURCEMANAGER
{
public:
    SOUNDMANAGER()
    {
        SafeSprintf(m_fileName, "SOUND.txt");
        m_8 = false;
        m_18 = true;
    }
    virtual ~SOUNDMANAGER();

    virtual RESOURCEMASTER *CreateMaster(const char *name) { return new SOUNDMASTER(this, name); }
    virtual const char *GetTypeCode() { return "SNFX"; }
    virtual bool IsMyType(const char *name) { return strstr(name, ".wav") != 0; }
    virtual void Init();

    // Turns the sounds (or the streams) on or off.
    void EnableSounds(bool enable);
    void EnableStreams(bool enable);
    // Stops the sound playing in sound's exclusive group, unless it is the
    // same sound and that may only play once.
    void PlayExclusive(SOUND *sound);

    bool m_streamsEnabled;
    bool m_soundsEnabled;
    // The sound playing in each exclusive group.
    std::map<int, RESHANDLE> m_exclusive;
};

// Defined elsewhere.
extern RESOURCEMANAGER *g_pSOUND;

#endif
