// The FMOD sound wrapper, 0x005478e0 onward.
//
// Discovery scaffold. The error-string function at 0x005478e0 maps FMOD
// results to the messages ("Error initializing output device", "Soundcard
// does not support the features..."), and 0x00547a50/0x00547d20/0x005480e0
// are the volume, master-load and sample-play paths. Ends before the
// ACTOR/ODEWORLD object at 0x00548420. The FMOD name comes from the strings.
#ifndef TONGAS2_SOUNDFMOD_H
#define TONGAS2_SOUNDFMOD_H

#include "Tongas2/SOUND.h"

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

// The FMOD sound manager. It is the SOUNDMANAGER that makes the FMOD
// masters.
//
// Retail vtable 0x660768.
class GD548040 : public SOUNDMANAGER
{
public:
    virtual ~GD548040();
    virtual RESOURCEMASTER *CreateMaster(const char *name);
};


const char *FMODErrorString(int code);
void SetVolume2(const char *name, float volume);
int PlaySample(const char *name);

#endif
