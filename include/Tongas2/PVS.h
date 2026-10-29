// The potentially visible sets the scene can sort its actors into: an
// octree over a box, or one node holding every actor. Their code is in
// another unit. Placeholder names apart from the class names, which are
// the names the scene's CreatePVS command takes.
#ifndef TONGAS2_PVS_H
#define TONGAS2_PVS_H

#include "Tongas2/MATHS.h"

class ACTOR;
struct FRUSTUM;

// Defined elsewhere.
class OCTREE
{
public:
    // An octree over the box from min to max, depth levels deep.
    OCTREE(V3 min, V3 max, int depth);
    ~OCTREE();

    void AddMember(ACTOR *actor);
    // Works out which actors the camera's frustum can see.
    void Update(FRUSTUM *frustum);
    // Writes the tree out to the log.
    void Dump(FRUSTUM *frustum);
    // Takes every actor out.
    void Clear();

    char m_0[0x20];
};

// Defined elsewhere.
class ONENODE
{
public:
    ONENODE();
    virtual ~ONENODE();
    virtual void AddMember(ACTOR *actor);

    void Update(FRUSTUM *frustum);
    void Dump(FRUSTUM *frustum);
    void Clear();

    int m_4;
};

#endif
