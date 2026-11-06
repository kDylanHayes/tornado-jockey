// The CAnimatedMesh object, 0x00530d10.
//
// Discovery scaffold. The loader at 0x00530d10 reports
// "CAnimatedMesh::LoadAnimations - Try to load empty file for animation" and
// uses TEXTFILE; the other methods build the mesh from ANIMATEDGEOMETRY and
// SKINNEDANIMATION. 0x00532bb0 starts the separate SKINNEDANIMATION object.
// The class name comes from the strings.
#ifndef TONGAS2_CANIMATEDMESH_H
#define TONGAS2_CANIMATEDMESH_H

class CANIMATEDMESH
{
public:
    int LoadAnimations(const char *name);
    int Load(const char *name);

private:
    char m_000[0x10];
};

#endif
