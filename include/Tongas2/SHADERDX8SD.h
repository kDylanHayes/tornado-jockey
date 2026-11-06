// The SD renderer's shaders, 0x0053a980 onward.
//
// Discovery scaffold. The manager factory at 0x0053a980 creates
// "SHADER.txt"; 0x0053aa50 is the 0x848-byte SHADERDX8SDRESOURCE::Apply
// reported by its error strings. Ends before the texture core at 0x0053dfe0.
#ifndef TONGAS2_SHADERDX8SD_H
#define TONGAS2_SHADERDX8SD_H

class SHADERDX8SDRESOURCE
{
public:
    void Apply();
};

void *CreateSHADERDX8SD();

#endif
