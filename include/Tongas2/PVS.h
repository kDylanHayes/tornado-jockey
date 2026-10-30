// The potentially visible sets the scene can sort its actors into: an
// octree over a box, or one node holding every actor. Placeholder names
// apart from OCTREE and ONENODE, which are the names the scene's CreatePVS
// command takes.
#ifndef TONGAS2_PVS_H
#define TONGAS2_PVS_H

#include "Tongas2/ACTOR.h"

struct FRUSTUM;

// A node of the set: the actors in it, kept in a list through their
// m_node, m_next and m_prev.
class PVSNODE
{
public:
    PVSNODE();
    virtual ~PVSNODE();
    // Puts actor at the front of the node's list.
    virtual void AddMember(ACTOR *actor);
    // Takes actor out, if it is in this node.
    void RemoveMember(ACTOR *actor);

    ACTOR *m_members;
};

// A node of the octree over a box, which splits into eight the first time
// an actor fits inside one of its eighths, while it has depth left.
class OCTREENODE : public PVSNODE
{
public:
    OCTREENODE(const V3 &min, const V3 &max, int depth);
    virtual ~OCTREENODE();
    // Puts actor in the smallest node it fits inside.
    virtual void AddMember(ACTOR *actor);

    // Marks the actors the frustum sees as in view; with test false, the
    // node is known to be inside it.
    void Update(FRUSTUM *frustum, bool test);
    // Counts the actors in view, the actors tested and the nodes.
    void Dump(FRUSTUM *frustum, bool test, int *unused, int *visible, int *tested, int *nodes);

    BOXCORNERS m_box;
    V3 m_max;
    V3 m_min;
    int m_depth;
    bool m_split;
    OCTREENODE *m_children[8];
};

class OCTREE
{
public:
    // An octree over the box from min to max, depth levels deep.
    OCTREE(V3 min, V3 max, int depth);
    ~OCTREE();

    void AddMember(ACTOR *actor);
    // Works out which actors the camera's frustum can see.
    void Update(FRUSTUM *frustum);
    // Counts what the frustum sees, for the log that is no longer written.
    void Dump(FRUSTUM *frustum);
    // Takes every actor out.
    void Clear();

    int m_depth;
    V3 m_max;
    V3 m_min;
    OCTREENODE *m_root;
};

class ONENODE : public PVSNODE
{
public:
    ONENODE();

    void Update(FRUSTUM *frustum);
    void Dump(FRUSTUM *frustum);
    void Clear();
};

// Defined elsewhere: whether the box from innerMin to innerMax is inside
// the box from min to max.
bool BoxInside(const V3 &min, const V3 &max, const V3 &innerMin, const V3 &innerMax);

#endif
