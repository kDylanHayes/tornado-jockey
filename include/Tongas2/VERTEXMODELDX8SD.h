// The DX8 shadowed vertex-model resources: VERTEXMODELDX8SDMASTER, from
// VERTEXMODELDX8SD.txt.
//
// Discovery scaffold. The class name comes from the RegisterClassName string
// at 0x00535fd0 and the manager string at 0x00536030. Bodies are empty until
// the unit is matched; each function's exact target size is in
// config/symbols.txt so a later delink cannot cut the target short.
#ifndef TONGAS2_VERTEXMODELDX8SD_H
#define TONGAS2_VERTEXMODELDX8SD_H

class RESOURCEMANAGER;

class VERTEXMODELDX8SDMASTER
{
public:
    static void RegisterCommands();
};

RESOURCEMANAGER *CreateVERTEXMODELDX8SD();

#endif
