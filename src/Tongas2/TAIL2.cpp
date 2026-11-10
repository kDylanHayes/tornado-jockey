// Tongas2/TAIL2
#include "Tongas2/LOG.h"
// Tongas2/TAIL2 -- discovery scaffold for the remaining 0x54eec0-range code.

class GD5529f0
{
public:
    virtual ~GD5529f0();
};

__declspec(noinline) GD5529f0::~GD5529f0()
{
}

class GD558000
{
public:
    virtual ~GD558000();
};

__declspec(noinline) GD558000::~GD558000()
{
}

class GD553820
{
public:
    virtual ~GD553820();
};

__declspec(noinline) GD553820::~GD553820()
{
}

class GD553990
{
public:
    virtual ~GD553990();
};

__declspec(noinline) GD553990::~GD553990()
{
}

class GD553e60
{
public:
    virtual ~GD553e60();
};

__declspec(noinline) GD553e60::~GD553e60()
{
}

extern "C" void (__stdcall *g_fn656140)(void *);

struct G2TL2CALL
{
    void Call();
};

void G2TL2CALL::Call()
{
    g_fn656140(*(void **)((char *)this + 4));
}

struct DRMPALX
{
    void ViewGlobalScoresError();
    void PostGlobalScoresError();
};

void DRMPALX::ViewGlobalScoresError()
{
    if (g_pLog)
        g_pLog->Error("************************************************");
    if (g_pLog)
        g_pLog->Error("CRITIC ERROR: DRMPAL::ViewGlobalScores method called");
    if (g_pLog)
        g_pLog->Error("You must overload this class with an apropiate information to call ViewToGlobalScores");
    if (g_pLog)
        g_pLog->Error("************************************************");
}

void DRMPALX::PostGlobalScoresError()
{
    if (g_pLog)
        g_pLog->Error("************************************************");
    if (g_pLog)
        g_pLog->Error("CRITIC ERROR: DRMPAL::PostGlobalScores method called");
    if (g_pLog)
        g_pLog->Error("You must overload this class with an apropiate information to call SendToGlobalScores");
    if (g_pLog)
        g_pLog->Error("************************************************");
}
