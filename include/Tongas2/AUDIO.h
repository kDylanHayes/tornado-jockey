// The sound system. Defined elsewhere; only what other units need so far.
#ifndef TONGAS2_AUDIO_H
#define TONGAS2_AUDIO_H

#include "Tongas2/SAFEINDEX.h"

class CMDBASE;

class AUDIO
{
public:
    // The sound object with a handle.
    CMDBASE *GetObject(const RESHANDLE &handle);
    void UpdateListener();
    // Pauses the game's sounds, or carries on with them. Placeholder name.
    void Pause(bool on);
};

extern AUDIO *g_pAudio;

#endif
