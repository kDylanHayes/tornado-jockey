// Stand-ins for the renderer plugin's shaders, which live in a unit of their
// own: just what the units before it use.
#ifndef TONGAS2_RENDERSHADER_H
#define TONGAS2_RENDERSHADER_H

#include "Tongas2/RESOURCES.h"

// Defined in the plugin: the shader a renderer shader's handle names.
class RENDERSHADERMASTER
{
public:
    // Whether the shader's first layer has an alpha map.
    bool HasAlpha();

    char m_00[0x10e4];
    // Set while the heightmap lights its vertices itself.
    bool m_lightingOff;
};

// Defined in the plugin: a renderer shader.
class RENDERSHADER
{
public:
    // The size of its first texture.
    unsigned int GetWidth();
    unsigned int GetHeight();

    char m_00[0xa8];
    RESHANDLE m_texture;
};

// Defined in the plugin: the renderer shaders' manager.
class RENDERSHADERMANAGER : public RESOURCEMANAGER
{
public:
    // The shader with a name, or 0.
    RESOURCE *Find(const char *name);
    RENDERSHADER *Get(RESHANDLE handle);

    // The triangles drawn this frame.
    int m_numTriangles;
};

extern RENDERSHADERMANAGER *g_pRENDERSHADER;

#endif
