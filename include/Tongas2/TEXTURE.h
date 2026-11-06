// The texture object, 0x00534650.
//
// Discovery scaffold. The tiny TEXTUREDX8SDTEXTURE accessor and destructor at
// 0x00534650/0x00534df0 live here, followed by the CTexture format, merge and
// creation helpers at 0x00534660, 0x00534a00 and 0x00534e70. The next object,
// TEXTURERESIZER, starts at 0x005358f0. The CTexture name comes from the
// error strings.
#ifndef TONGAS2_TEXTURE_H
#define TONGAS2_TEXTURE_H

class CTEXTURE
{
public:
    void ConvertToFormat();

private:
    char m_000[0x10];
};

#endif
