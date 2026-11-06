// Tongas2/HEIGHTMAPDECALDX8 -- the DX8 renderer plugin's heightmap decals.
//
// Discovery scaffold: the class registrations, manager factory and the
// constructor/destructor pair whose names the strings and the decompiler
// give. The bodies are empty until the unit is matched; every function here
// has its exact target size in config/symbols.txt so a later delink cannot
// cut the target short.
#include "Tongas2/HEIGHTMAPDECALDX8.h"

void HEIGHTMAPDECALDX8MASTER::RegisterCommands()
{
}

void HEIGHTMAPDECALDX8RESOURCE::RegisterCommands()
{
}

RESOURCEMANAGER *CreateHEIGHTMAPDECALDX8()
{
    return 0;
}

HEIGHTMAPDECALDX8MASTER::HEIGHTMAPDECALDX8MASTER(RESOURCEMANAGER *manager, const char *name)
{
    (void)manager;
    (void)name;
}

HEIGHTMAPDECALDX8MASTER::~HEIGHTMAPDECALDX8MASTER()
{
}

HEIGHTMAPDECALDX8RESOURCE::HEIGHTMAPDECALDX8RESOURCE(HEIGHTMAPDECALDX8MASTER *master,
                                                     const RESHANDLE &handle)
{
    (void)master;
    (void)handle;
}

RESOURCE *HEIGHTMAPDECALDX8MASTER::CreateCopy(const RESHANDLE &handle)
{
    (void)handle;
    return 0;
}

RESOURCEMASTER *HEIGHTMAPDECALDX8MANAGER::CreateMaster(const char *name)
{
    (void)name;
    return 0;
}

void HEIGHTMAPDECALDX8MASTER::Prepare()
{
}

void HEIGHTMAPDECALDX8MASTER::UpdateCopies()
{
}

void HEIGHTMAPDECALDX8MASTER::SetUserFolder(const bool &user)
{
    (void)user;
}

void HEIGHTMAPDECALDX8MASTER::SetCorrectSizeInWidescreen(const bool &correct)
{
    (void)correct;
}
