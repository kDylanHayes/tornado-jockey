// The renderer plugin's simple models: SIMPLEMODELDX8RESOURCE draws a
// SIMPLEMODEL through the DX8 mesh code, built from named mesh parts, and
// SIMPLEMODELDX8MASTER makes its copies.
//
// Discovery scaffold. The class names come from the string at 0x004ef720 and
// the SIMPLEMODELMASTER constructor call at 0x004efe40. The base classes and
// member layouts are deliberately left out for now: keeping the scaffold
// free of the engine's inline virtuals means the object defines only the
// functions whose exact target sizes are recorded in config/symbols.txt, so
// a later delink cannot cut a target function short.
#ifndef TONGAS2_SIMPLEMODELDX8_H
#define TONGAS2_SIMPLEMODELDX8_H

#include "Tongas2/SAFEINDEX.h"

class RESOURCEMANAGER;
class RESOURCEMASTER;

class SIMPLEMODELDX8RESOURCE
{
public:
    void Start();
    void Update();

    // Adds the named part of the model to the mesh.
    void AddMeshPart(const char *part, int firstVertex, int numVertices, int firstIndex);
};

class SIMPLEMODELDX8MASTER
{
public:
    SIMPLEMODELDX8MASTER(RESOURCEMANAGER *manager, const char *name);
};

class SIMPLEMODELDX8MANAGER
{
public:
    RESOURCEMASTER *CreateMaster(const char *name);
};

#endif
