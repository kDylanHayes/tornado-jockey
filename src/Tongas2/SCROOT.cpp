// Tongas2/SCROOT -- the SCRoot object and its loader.
//
// Discovery scaffold: bodies are empty until the unit is matched; each
// function has its exact target size in config/symbols.txt so a later delink
// cannot cut the target short.
#include "Tongas2/SCROOT.h"

SCROOT::SCROOT(int type, const void *data)
{
    (void)type;
    (void)data;
}

void SCROOT::Init()
{
}

void SCROOT::Load(void *data, int a, int b, int c)
{
    (void)data;
    (void)a;
    (void)b;
    (void)c;
}

SCROOT *CreateSCROOT()
{
    return 0;
}
