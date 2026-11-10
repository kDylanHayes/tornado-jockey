// Tongas2/TAIL2
#include <string.h>

#include "Tongas2/LOG.h"
// Tongas2/TAIL2 -- discovery scaffold for the remaining 0x54eec0-range code.

class KERNELCOMMANDS
{
public:
    virtual ~KERNELCOMMANDS();
};

class GD556050
{
public:
    virtual ~GD556050();
};

class GD5529f0 : public KERNELCOMMANDS
{
public:
    virtual ~GD5529f0() throw();
};

__declspec(noinline) GD5529f0::~GD5529f0() throw()
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

class GD553820 : public GD556050
{
public:
    virtual ~GD553820() throw();
};

__declspec(noinline) GD553820::~GD553820() throw()
{
}

class GD553990 : public GD556050
{
public:
    virtual ~GD553990() throw();
};

__declspec(noinline) GD553990::~GD553990() throw()
{
}

class GD553e60 : public GD556050
{
public:
    virtual ~GD553e60() throw();
};

__declspec(noinline) GD553e60::~GD553e60() throw()
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
    char *GetClearKey(char *out);
    char *GetClearLeaderBoardsKey(char *out);
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

char *DRMPALX::GetClearKey(char *out)
{
    if (g_pLog)
        g_pLog->Error("************************************************");
    if (g_pLog)
        g_pLog->Error("CRITIC ERROR: DRMPAL::GetClearKey method called");
    if (g_pLog)
        g_pLog->Error("You must overload this class with an apropiate GetClearKey method");
    if (g_pLog)
        g_pLog->Error("************************************************");

    char buffer[0x200];
    strncpy(buffer, "ERROR-ERROR-ERROR-ERROR", 0x1ff);
    buffer[0x1ff] = 0;
    memcpy(out, buffer, sizeof(buffer));
    return out;
}

char *DRMPALX::GetClearLeaderBoardsKey(char *out)
{
    if (g_pLog)
        g_pLog->Error("************************************************");
    if (g_pLog)
        g_pLog->Error("CRITIC ERROR: DRMPAL::GetClearLeaderBoardsKey method called");
    if (g_pLog)
        g_pLog->Error("You must overload this class with an apropiate GetClearLeaderBoardsKey method");
    if (g_pLog)
        g_pLog->Error("************************************************");

    char buffer[0x200];
    strncpy(buffer, "-ERROR--", 0x1ff);
    buffer[0x1ff] = 0;
    memcpy(out, buffer, sizeof(buffer));
    return out;
}
