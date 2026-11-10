// Tongas2/TEXTURE -- the texture object and CTexture helpers.
//
// Discovery scaffold: bodies are empty until the unit is matched; each
// function has its exact target size in config/symbols.txt so a later delink
// cannot cut the target short. TEXTUREDX8SDTEXTURE is declared in
// TEXTUREDX8SD.h; its two out-of-line methods are compiled in this object.
#include "Tongas2/TEXTURE.h"

#include "Tongas2/TEXTUREDX8SD.h"
#include "Tongas2/TEXTURERESIZER.h"

void *TEXTUREDX8SDTEXTURE::GetSurface()
{
    return m_surface;
}

TEXTUREDX8SDTEXTURE::~TEXTUREDX8SDTEXTURE()
{
    if (m_surface) {
        m_surface->Release();
        m_surface = 0;
    }
}

void CTEXTURE::ConvertToFormat()
{
}

// The resizer node's destructor (0x535800) and the resizer's deleting
// destructor (0x5358a0) are compiled in this object.
TEXTURERESIZERNODE::~TEXTURERESIZERNODE()
{
}

TEXTURERESIZER::~TEXTURERESIZER()
{
    TEXTURERESIZERNODE *node = m_4;
    while (node) {
        TEXTURERESIZERNODE *next = node->m_4;
        delete node;
        node = next;
    }
}

// Makes the compiler emit the resizer's scalar deleting destructor in this
// object, as the retail build does.
void DeleteTextureResizer(TEXTURERESIZER *resizer)
{
    delete resizer;
}

void TEXTURERESIZER::Remove(int key)
{
    if (!key)
        return;
    TEXTURERESIZERNODE *prev = 0;
    TEXTURERESIZERNODE *node = m_4;
    if (!node)
        return;
    do {
        if (node->m_0 == key) {
            if (prev)
                prev->m_4 = node->m_4;
            else
                m_4 = node->m_4;
            delete node;
            return;
        }
        prev = node;
        node = node->m_4;
    } while (node);
}
