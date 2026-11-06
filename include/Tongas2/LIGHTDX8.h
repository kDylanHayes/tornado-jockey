// The renderer plugin's lights: LIGHTDX8MASTER and LIGHTDX8RESOURCE turn the
// engine's LIGHT into the device's lights, and LIGHTDX8MANAGER, from
// LIGHT.txt, keeps them.
#ifndef TONGAS2_LIGHTDX8_H
#define TONGAS2_LIGHTDX8_H

#include <vector>

#include "Tongas2/LIGHT.h"
#include "Tongas2/MATHS.h"
#include "Tongas2/SAFEINDEX.h"

// A colour with an alpha channel, as the device and the lighting code use it.
struct LIGHTVALUE
{
    float r, g, b, a;
};

// The device's light description.
struct LIGHTD3D
{
    int type;
    LIGHTVALUE diffuse;
    LIGHTVALUE specular;
    LIGHTVALUE ambient;
    V3 position;
    V3 direction;
    float range;
    float falloff;
    float attenuation0;
    float attenuation1;
    float attenuation2;
    float theta;
    float phi;
};

// A master whose copies are the device's lights.
class LIGHTDX8MASTER : public LIGHTMASTER
{
public:
    LIGHTDX8MASTER(RESOURCEMANAGER *manager, const char *name) : LIGHTMASTER(manager, name) {}

    virtual RESOURCE *CreateCopy(const RESHANDLE &handle);
    virtual void SetType(const int &type);
};

// One light turned into the device's form.
class LIGHTDX8RESOURCE : public LIGHT
{
public:
    LIGHTDX8RESOURCE(LIGHTMASTER *master, const RESHANDLE &handle);

    virtual void Start();
    virtual void Stop();
    virtual void Update();

    // Where the light points, and the device light built from the master.
    V3 m_direction;
    LIGHTD3D m_light;
    // The manager's ambient colour saved while this light is an ambient one.
    int m_savedAmbient;
};

// The LIGHT.txt manager: the lights, grouped by the masks they belong to.
class LIGHTDX8MANAGER : public LIGHTMANAGER
{
public:
    LIGHTDX8MANAGER();
    virtual ~LIGHTDX8MANAGER();

    virtual RESOURCEMASTER *CreateMaster(const char *name);
    virtual const char *GetTypeCode();
    virtual void Update();
    virtual void LightVertex(const LIGHTRGB &ambient, const V3I &color, const V3 &normal, int mask, LIGHTRGB &out);
    virtual void Virtual09(int mask);
    virtual void UpdateLights() {}
    virtual void Invalidate(int mask);

    // Adds or removes a light's handle from the group it belongs to.
    void RemoveHandle(int type, const RESHANDLE &handle);

    std::vector<RESHANDLE> m_lights;
    std::vector<RESHANDLE> m_lights2;
    int m_color;
    int m_mask;
};

LIGHTVALUE UnpackColor(const int *color);

RESOURCEMANAGER *CreateLIGHTDX8();

#endif
