// The TEXTURERESIZER object, 0x005358f0.
//
// Discovery scaffold. The updater at 0x005358f0 reports the
// "TEXTURERESIZER::UpdateTextures" errors; the other methods run through
// 0x00535fcf. 0x00535fd0 starts the SD renderer registration instead. The
// class name comes from the error strings.
#ifndef TONGAS2_TEXTURERESIZER_H
#define TONGAS2_TEXTURERESIZER_H

#include <string>

// One node of the resizer's list of per-size textures. The list link is at
// +4 and the two names at +8 and +0x24.
class TEXTURERESIZERNODE
{
public:
    TEXTURERESIZERNODE();
    ~TEXTURERESIZERNODE();

    int m_0;
    TEXTURERESIZERNODE *m_4;
    std::string m_8;
    std::string m_24;
};

class TEXTURERESIZER
{
public:
    TEXTURERESIZER();
    virtual ~TEXTURERESIZER();

    void UpdateTextures();
    // Unlinks and frees the node whose key is key.
    void Remove(int key);

    TEXTURERESIZERNODE *m_4;
};

#endif
