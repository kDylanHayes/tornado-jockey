// The SKINNEDGEOMETRY object, 0x0050d910.
//
// Discovery scaffold. The file loader at 0x0050d910 reports the
// "SKINNEDGEOMETRY - file does not exist" and diffuse-colour errors; the
// small functions at 0x005103d0-0x0051065f are its vector copy helpers and
// accessors. 0x00510660 starts the separate CSkinnedMesh object. The class
// name comes from the loader string; member layouts are still unknown.
#ifndef TONGAS2_SKINNEDGEOMETRY_H
#define TONGAS2_SKINNEDGEOMETRY_H

#include <string>

class SKINNEDGEOMETRY
{
public:
    // ret 0x3c and the only call site (0x00500cfc) push 0x1c + 0x1c + 4, so both
    // strings are passed by value: 4 + sizeof(std::string) + sizeof(std::string).
    int Load(void *file, std::string path, std::string other);
    int GetVertexData();
    int GetVertexCount();

private:
    char m_000[0x10];
};

#endif
