// The SD renderer's render textures, 0x00539750 onward.
//
// Discovery scaffold. The manager factory at 0x00539950 creates
// "RENDERTEXTURE.txt"; 0x005398d0 creates the 0x90-byte object with vtable
// 0x0067a760. 0x00539a10 is the first resource method. This is a second
// RENDERTEXTURE object (the SD plugin's), so the unit keeps the DX8SD suffix.
#ifndef TONGAS2_RENDERTEXTUREDX8SD_H
#define TONGAS2_RENDERTEXTUREDX8SD_H

class RENDERTEXTUREDX8SDMASTER;

RENDERTEXTUREDX8SDMASTER *CreateRENDERTEXTUREDX8SDMaster();
void *CreateRENDERTEXTUREDX8SD();
int GetRENDERTEXTUREFormat();

#endif
