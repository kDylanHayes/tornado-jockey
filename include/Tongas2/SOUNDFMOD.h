// The FMOD sound wrapper, 0x005478e0 onward.
//
// Discovery scaffold. The error-string function at 0x005478e0 maps FMOD
// results to the messages ("Error initializing output device", "Soundcard
// does not support the features..."), and 0x00547a50/0x00547d20/0x005480e0
// are the volume, master-load and sample-play paths. Ends before the
// ACTOR/ODEWORLD object at 0x00548420. The FMOD name comes from the strings.
#ifndef TONGAS2_SOUNDFMOD_H
#define TONGAS2_SOUNDFMOD_H

const char *FMODErrorString(int code);
void SetVolume2(const char *name, float volume);
int PlaySample(const char *name);

#endif
