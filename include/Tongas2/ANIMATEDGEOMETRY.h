// The ANIMATEDGEOMETRY object, 0x0052d400.
//
// Discovery scaffold. The loader at 0x0052d400 reports the
// "ANIMATEDGEOMETRY - file does not exist" error; the accessors from
// 0x0052f8c0 read the bone, frame and vertex data. 0x00530d10 starts the
// separate CAnimatedMesh object. The class name comes from the loader string.
#ifndef TONGAS2_ANIMATEDGEOMETRY_H
#define TONGAS2_ANIMATEDGEOMETRY_H

class ANIMATEDGEOMETRY
{
public:
    int Load(void *file, int flags, void *owner);
    int GetBoneCount();
    int GetFrameCount();
    int GetVertexCount();

private:
    char m_000[0x10];
};

#endif
