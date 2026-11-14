// Tongas2/SOUNDFMOD -- the FMOD sound wrapper.
//
// The unit holds the FMOD side of the sound system: the error strings, the
// FMOD sound master (a SOUNDMASTER that loads the file through FMOD), the
// FMOD sound (a SOUND that plays a channel) and the sound manager that
// hands them out.
#include "Tongas2/SOUNDFMOD.h"
#include "Tongas2/SOUND.h"

// The FMOD library (FMOD.DLL). Its functions are stdcall.
extern "C" int __stdcall FSOUND_GetError(void);
extern "C" int __stdcall FSOUND_StopSound(int handle);
extern "C" int __stdcall FSOUND_Stream_Stop(int handle);
extern "C" int __stdcall FSOUND_Close(void);

const char *FMODErrorString(int code)
{
    (void)code;
    switch (FSOUND_GetError())
    {
    case 0:
        return "No errors";
    case 1:
        return "Cannot call this command after FSOUND_Init. Call FSOUND_Close first.";
    case 2:
        return "This command failed because FSOUND_Init or FSOUND_SetOutput was not called";
    case 3:
        return "Error initializing output device.";
    case 4:
        return "Error initializing output device, but more specifically, the output device is already in use and cannot be reused.";
    case 5:
        return "Playing the sound failed.";
    case 6:
        return "Soundcard does not support the features needed for this soundsystem (16bit stereo output)";
    case 7:
        return "Error setting cooperative level for hardware.";
    case 8:
        return "Error creating hardware sound buffer.";
    case 9:
        return "File not found";
    case 10:
        return "Unknown file format";
    case 11:
        return "Error loading file";
    case 12:
        return "Not enough memory or resources";
    case 13:
        return "The version number of this file format is not supported";
    case 14:
        return "An invalid parameter was passed to this function";
    case 15:
        return "Tried to use an EAX command on a non EAX enabled channel or output.";
    case 16:
        return "Failed to allocate a new channel";
    case 17:
        return "Recording is not supported on this machine";
    case 18:
        return "Windows Media Player not installed so cannot play wma or use internet streaming.";
    case 19:
        return "An error occured trying to open the specified CD device";
    default:
        return "Unknown error";
    }
}

// The FMOD sound master. It is the SOUNDMASTER with the file the FMOD
// library reads and the handles of the sample and the stream it plays.
//
// Retail vtable 0x67b878; constructor at 0x005482f0, the copy maker at
// 0x00548340, the loader at 0x00547d20.
class GD5482f0 : public SOUNDMASTER
{
public:
    GD5482f0(RESOURCEMANAGER *manager, const char *name);
    virtual RESOURCE *CreateCopy(const RESHANDLE &handle);

    // Loads the file of the master into FMOD (retail 0x00547d20).
    __declspec(noinline) void LoadSound();

    // Whether the file is loaded.
    bool m_a0;
    // The file's contents, as FMOD gives them back.
    char *m_a4;
    // The sample handle, when the master plays a sample.
    int m_a8;
    // The stream handle, when the master plays a stream.
    int m_ac;
    // The channel the last sound took.
    int m_b0;
    // How many copies of the master are playing.
    int m_b4;
};

// The FMOD sound. It is the SOUND with the channel it plays on.
//
// Retail vtable 0x67b6e8; constructor at 0x00548060, Stop at 0x005479d0.
class GD5479c0 : public SOUND
{
public:
    GD5479c0(SOUNDMASTER *master, const RESHANDLE &handle);
    virtual ~GD5479c0() throw();

    virtual void Start();
    virtual void Stop();
    virtual void Update();

    // The channel the sound plays on.
    int m_424;
    // The channel the sound took before.
    int m_428;
};

// Retail 0x005482f0.
GD5482f0::GD5482f0(RESOURCEMANAGER *manager, const char *name) : SOUNDMASTER(manager, name)
{
    m_ac = 0;
    m_a8 = 0;
    m_a0 = false;
    m_b4 = 0;
    m_b0 = -1;
}

// Retail 0x00548340: a copy of the master is a sound.
RESOURCE *GD5482f0::CreateCopy(const RESHANDLE &handle)
{
    return new GD5479c0(this, handle);
}

// Retail 0x00548060.
GD5479c0::GD5479c0(SOUNDMASTER *master, const RESHANDLE &handle) : SOUND(master, handle)
{
    ((GD5482f0 *)m_master)->LoadSound();
}

__declspec(noinline) GD5479c0::~GD5479c0() throw()
{
}

// Retail 0x005479d0: stops the sound, and lets go of the channel.
void GD5479c0::Stop()
{
    SOUND::Stop();
    GD5482f0 *master = (GD5482f0 *)m_master;
    if (master->m_singleInstance)
    {
        --master->m_b4;
        if (master->m_b4 > 0)
        {
            m_424 = -1;
            return;
        }
    }
    if (master->m_stream)
    {
        FSOUND_Stream_Stop(master->m_ac);
        master->m_b0 = -1;
        m_424 = -1;
        return;
    }
    FSOUND_StopSound(m_424);
    master->m_b0 = -1;
    m_424 = -1;
}

// The FMOD sound manager. It is the SOUNDMANAGER that makes the FMOD
// masters.
//
// Retail vtable 0x660768.
class GD548040 : public SOUNDMANAGER
{
public:
    virtual ~GD548040();
};

__declspec(noinline) GD548040::~GD548040()
{
    FSOUND_Close();
}

// Makes the FMOD sound manager (retail 0x005483b0).
void V5483b0()
{
    new GD548040();
}

extern void (*g_fmodFactory)();

struct G2FMODSET
{
    void Set();
};

void G2FMODSET::Set()
{
    g_fmodFactory = V5483b0;
}

// The FMOD unit is the one that defines the no-op registration hook.
void RegisterClassName(CMDENTRY *commands, const char *name, const char *parent)
{
    (void)commands;
    (void)name;
    (void)parent;
}
