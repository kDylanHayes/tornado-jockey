// Tongas2/MODEL -- the model resource code at 0x004fb460.
//
// Discovery scaffold: a constructor/destructor pair and the lookup, load and
// create helpers . The bodies are empty until the unit is matched; each
// function has its exact target size in config/symbols.txt so a later delink
// cannot cut the target short.
#include "Tongas2/MODEL.h"

MODEL::MODEL()
{
}

MODEL::~MODEL()
{
}

void *MODEL::Find(const char *name)
{
    (void)name;
    return 0;
}

void *MODEL::Load(const char *name)
{
    (void)name;
    return 0;
}

void *MODEL::Create(const char *name)
{
    (void)name;
    return 0;
}
