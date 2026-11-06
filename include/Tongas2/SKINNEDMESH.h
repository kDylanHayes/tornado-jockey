// The CSkinnedMesh object, 0x00510660.
//
// Discovery scaffold. The string-named methods run from
// CSkinnedMesh::GetAnimationIndex at 0x00510660 through LoadExtraAnimations
// at 0x00515290; the rest of the block is the mesh/skeleton, camera and path
// helpers the class uses (the camera and path object methods MAPLOADER maps
// at 0x005177a0 and 0x00527ec0). 0x00528610 starts TEXTFILE. The class name
// comes from the strings.
#ifndef TONGAS2_SKINNEDMESH_H
#define TONGAS2_SKINNEDMESH_H

class SKINNEDMESH
{
public:
    int GetAnimationIndex(const char *name);
    const char *GetAnimationName(unsigned int index);
    int RemoveAnimations(const char *name);
    int PlayAnimation(int index);
    int BlendAnimation(int index);
    int LoadExtraAnimations(const char *name);

private:
    char m_000[0x18];
};

#endif
