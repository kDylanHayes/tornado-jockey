// Tongas2/TEXTFILE -- the text tokenizer used by the skinned objects.
//
// Discovery scaffold: bodies are empty until the unit is matched; each
// function has its exact target size in config/symbols.txt so a later delink
// cannot cut the target short.
#include "Tongas2/TEXTFILE.h"

int TEXTFILE::Load(const char *name)
{
    (void)name;
    return 0;
}

int TEXTFILE::GetToken()
{
    return 0;
}

int TEXTFILE::GetLine()
{
    return 0;
}
