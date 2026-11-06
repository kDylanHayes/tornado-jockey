// The DX8 texture core, 0x0053dfe0 onward.
//
// Discovery scaffold. This block carries the vector/deque helpers and the
// texture loader at 0x00546ea0 whose errors are "Texture:%s could not be
// created", "PNG FORMAT NOT SUPPORTED" and "Texture is now power of 2"; the
// 0x547880/0x5478c0 helpers finish it before the FMOD sound object at
// 0x005478e0. The CTexture name comes from the 0x00534660/0x00534a00 errors,
// so the unit is called TEXTUREDX8CORE to avoid the earlier TEXTURE unit.
#ifndef TONGAS2_TEXTUREDX8CORE_H
#define TONGAS2_TEXTUREDX8CORE_H

int CreateTexture();
int TextureFormatA();
int TextureFormatB();

#endif
