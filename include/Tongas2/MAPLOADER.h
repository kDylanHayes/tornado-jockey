// A placeholder unit for the object at 0x004f8350: it walks a map's object
// list and handles the special names TARGET, CAMERA, KYBOX, WINGPATH and
// CAMERAPATH, then updates the result over time. The class name is not
// recovered yet, so MAPLOADER is descriptive; the two methods are named from
// what they do, not from strings.
#ifndef TONGAS2_MAPLOADER_H
#define TONGAS2_MAPLOADER_H

#include <string>

#include "Tongas2/MATHS.h"

class MAPLOADER
{
public:
    void LoadMap(const char *name);
    void Update(float dt);

private:
    // Offsets recovered from the two functions' decompiler output.
    int m_00;
    void *m_04;
    char m_08[4];
    float m_0c;
    bool m_10;
    char m_11[7];
    void *m_18;
    void *m_1c;
    void *m_20;
    void *m_24;
    char m_28[0x10];
    void *m_38;
    char m_3c[0x1c8];
    void *m_204;
    void *m_208;
    std::string m_20c;
    float m_228;
    float m_22c;
    float m_230;
    V3 m_234;
    void *m_240;
    void *m_244;
    float m_248;
    bool m_24c;
    char m_24d[0xb];
    bool m_258;
};

#endif
