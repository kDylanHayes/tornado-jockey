// The SKINNEDGEOMETRY object, 0x0050d910.
//
// Discovery scaffold. The file loader at 0x0050d910 reports the
// "SKINNEDGEOMETRY - file does not exist" and diffuse-colour errors; the
// small functions at 0x005103d0-0x0051065f are its vector copy helpers and
// accessors. 0x00510660 starts the separate CSkinnedMesh object. The class
// name comes from the loader string; member layouts are still unknown.
#ifndef TONGAS2_SKINNEDGEOMETRY_H
#define TONGAS2_SKINNEDGEOMETRY_H

class SKINNEDGEOMETRY
{
public:
    int Load(void *file, int flags, void *owner);
    int GetVertexData();
    int GetVertexCount();

private:
    char m_000[0x10];
};

#endif
