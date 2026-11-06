// The TEXTURERESIZER object, 0x005358f0.
//
// Discovery scaffold. The updater at 0x005358f0 reports the
// "TEXTURERESIZER::UpdateTextures" errors; the other methods run through
// 0x00535fcf. 0x00535fd0 starts the SD renderer registration instead. The
// class name comes from the error strings.
#ifndef TONGAS2_TEXTURERESIZER_H
#define TONGAS2_TEXTURERESIZER_H

class TEXTURERESIZER
{
public:
    void UpdateTextures();
    void Create();
    void Destroy();

private:
    char m_000[0x10];
};

#endif
