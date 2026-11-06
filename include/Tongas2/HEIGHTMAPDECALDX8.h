// The renderer plugin's heightmap decals: HEIGHTMAPDECALDX8MASTER names a
// heightmap decal, drawn by stretching a heightmap onto a mesh, and
// HEIGHTMAPDECALDX8RESOURCE is one placed copy.
//
// Discovery scaffold. The class names come from the RegisterClassName
// strings at 0x004ebd90 and 0x004ebdd0. The base classes and member layouts
// are deliberately left out for now: keeping the scaffold free of the
// engine's inline virtuals means the object defines only the functions whose
// exact target sizes are recorded in config/symbols.txt, so a later delink
// cannot cut a target function short.
#ifndef TONGAS2_HEIGHTMAPDECALDX8_H
#define TONGAS2_HEIGHTMAPDECALDX8_H

#include "Tongas2/SAFEINDEX.h"

class RESOURCE;
class RESOURCEMANAGER;
class RESOURCEMASTER;

class HEIGHTMAPDECALDX8MASTER
{
public:
    HEIGHTMAPDECALDX8MASTER(RESOURCEMANAGER *manager, const char *name);
    ~HEIGHTMAPDECALDX8MASTER();

    static void RegisterCommands();

    void Prepare();
    void UpdateCopies();
    RESOURCE *CreateCopy(const RESHANDLE &handle);
    void SetUserFolder(const bool &user);
    void SetCorrectSizeInWidescreen(const bool &correct);
};

class HEIGHTMAPDECALDX8RESOURCE
{
public:
    HEIGHTMAPDECALDX8RESOURCE(HEIGHTMAPDECALDX8MASTER *master, const RESHANDLE &handle);

    static void RegisterCommands();
};

class HEIGHTMAPDECALDX8MANAGER
{
public:
    RESOURCEMASTER *CreateMaster(const char *name);
};

RESOURCEMANAGER *CreateHEIGHTMAPDECALDX8();

#endif
