// Tongas2/SOUNDFMOD -- the FMOD sound wrapper.
//
// Discovery scaffold: bodies are empty until the unit is matched; each
// function has its exact target size in config/symbols.txt so a later delink
// cannot cut the target short.
#include "Tongas2/SOUNDFMOD.h"
#include "Tongas2/SOUND.h"

const char *FMODErrorString(int code)
{
    (void)code;
    return 0;
}

void SetVolume2(const char *name, float volume)
{
    (void)name;
    (void)volume;
}

int PlaySample(const char *name)
{
    (void)name;
    return 0;
}

class GD5479c0 : public SOUND
{
public:
    virtual ~GD5479c0() throw();
};

__declspec(noinline) GD5479c0::~GD5479c0() throw()
{
}

extern void V5483b0();
extern void (*g_fmodFactory)();

struct G2FMODSET
{
    void Set();
};

void G2FMODSET::Set()
{
    g_fmodFactory = V5483b0;
}

extern "C" int FSOUND_Close(void);

class GD548040 : public SOUNDMANAGER
{
public:
    virtual ~GD548040();
};

__declspec(noinline) GD548040::~GD548040()
{
    FSOUND_Close();
}
