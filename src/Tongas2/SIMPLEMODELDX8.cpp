// Tongas2/SIMPLEMODELDX8 -- the DX8 renderer plugin's simple models.
//
// Discovery scaffold: the names come from the AddMeshPart string and the
// BONEDMODELRESOURCE / SIMPLEMODELMASTER calls in the decompiler. The bodies
// are empty until the unit is matched; every function here has its exact
// target size in config/symbols.txt so a later delink cannot cut the target
// short.
#include "Tongas2/SIMPLEMODELDX8.h"

void SIMPLEMODELDX8RESOURCE::Start()
{
}

void SIMPLEMODELDX8RESOURCE::Update()
{
}

void SIMPLEMODELDX8RESOURCE::AddMeshPart(const char *part, int firstVertex, int numVertices,
                                         int firstIndex)
{
    (void)part;
    (void)firstVertex;
    (void)numVertices;
    (void)firstIndex;
}

SIMPLEMODELDX8MASTER::SIMPLEMODELDX8MASTER(RESOURCEMANAGER *manager, const char *name)
{
    (void)manager;
    (void)name;
}

RESOURCEMASTER *SIMPLEMODELDX8MANAGER::CreateMaster(const char *name)
{
    (void)name;
    return 0;
}
