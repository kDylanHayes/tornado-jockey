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

#include <string>
#include <vector>
#include <deque>

class GD510b10;
class GD510b10CONTAINER;
struct GD510b10TABLE;

// The loaded model the mesh draws; +0x38 is its skeleton.
struct SKINNEDMESHMODEL
{
    GD510b10CONTAINER *GetSkeleton() { return m_38; }

    char pad_0[0x38];
    GD510b10CONTAINER *m_38;
};

class SKINNEDMESH
{
public:
    int GetAnimationIndex(const std::string &name);
    const std::string &GetAnimationName(unsigned int index);
    int RemoveAnimations(const char *name);
    void PlayAnimation(int index, bool loop, float speed);
    void BlendAnimation(int index, bool loop, float blendTime, float speed);
    int LoadExtraAnimations(const char *name);
    void StopAnimations();

    char m_000[0xc];
    SKINNEDMESHMODEL *m_0c;
    char m_010[4];
    std::vector<GD510b10TABLE *> m_animations;
    std::deque<GD510b10 *> m_playing;
};

#endif
