// The DX8 shadowed model resources: MODELDX8SDMASTER, from
// MODELDX8SD.txt.
//
// Discovery scaffold. The class names come from the RegisterClassName string
// at 0x00502e30 and the manager string at 0x00502e80. Base classes and member
// layouts are deliberately left out for now. Bodies are empty until the unit
// is matched; each function's exact target size is in config/symbols.txt so a
// later delink cannot cut the target short.
#ifndef TONGAS2_MODELDX8SD_H
#define TONGAS2_MODELDX8SD_H

class RESOURCEMANAGER;

class MODELDX8SDMASTER
{
public:
    static void RegisterCommands();
};

class MODELDX8SDMANAGER
{
public:
    static const char *GetTypeCode();
};

RESOURCEMANAGER *CreateMODELDX8SD();

#endif
