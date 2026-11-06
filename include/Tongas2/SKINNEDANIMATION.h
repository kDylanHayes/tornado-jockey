// The SKINNEDANIMATION object, 0x00532bb0.
//
// Discovery scaffold. The loader at 0x00532bb0 reports the
// "SKINNEDANIMATION - file does not exist" and "EMPTY" errors; the
// accessors from 0x00533e40 read the bone and frame data. 0x00534650 starts
// the texture object. The class name comes from the loader string.
#ifndef TONGAS2_SKINNEDANIMATION_H
#define TONGAS2_SKINNEDANIMATION_H

class SKINNEDANIMATION
{
public:
    int Load(const char *name);
    int GetBoneCount();
    int GetFrameCount();

private:
    char m_000[0x10];
};

#endif
