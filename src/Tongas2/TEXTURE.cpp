// Tongas2/TEXTURE -- the texture object and CTexture helpers.
//
// Discovery scaffold: bodies are empty until the unit is matched; each
// function has its exact target size in config/symbols.txt so a later delink
// cannot cut the target short. TEXTUREDX8SDTEXTURE is declared in
// TEXTUREDX8SD.h; its two out-of-line methods are compiled in this object.
#include "Tongas2/TEXTURE.h"

#include "Tongas2/TEXTUREDX8SD.h"

void *TEXTUREDX8SDTEXTURE::GetSurface()
{
    return m_surface;
}

TEXTUREDX8SDTEXTURE::~TEXTUREDX8SDTEXTURE()
{
}

void CTEXTURE::ConvertToFormat()
{
}
