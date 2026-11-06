// The DX8 shadowed textures: TEXTUREDX8SDMASTER.
//
// Discovery scaffold. The class name and the methods come from the
// "TEXTUREDX8SDMASTER::BuildDiffuse..." error strings at 0x00504210; the
// manager's destructor and vtable are at 0x00504360. Base classes and member
// layouts are deliberately left out for now. Bodies are empty until the unit
// is matched; each function's exact target size is in config/symbols.txt so a
// later delink cannot cut the target short.
#ifndef TONGAS2_TEXTUREDX8SD_H
#define TONGAS2_TEXTUREDX8SD_H

class TEXTUREDX8SDMASTER
{
public:
    void BuildDiffuseColorTexture();
    ~TEXTUREDX8SDMASTER();
};

class TEXTUREDX8SDMANAGER
{
public:
    ~TEXTUREDX8SDMANAGER();
};

#endif
