// A placeholder unit for the object at 0x004f9280: its first function builds
// the intro screen around data/art/models/interface/wtintro/intrologo.png and
// calls the MAPLOADER code. The class name is not recovered yet, so INTRO is
// descriptive; the methods after the first one are still unnamed.
#ifndef TONGAS2_INTRO_H
#define TONGAS2_INTRO_H

class INTRO
{
public:
    void Load(const char *name, const char *path);
};

#endif
