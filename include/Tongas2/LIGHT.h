// Lights: LIGHTMASTER holds a light type's colors, range and attenuation,
// and LIGHT is one light in the world. The manager itself comes from the
// renderer plugin; this unit only has the parts it shares. Placeholder
// names apart from the class names and properties.
#ifndef TONGAS2_LIGHT_H
#define TONGAS2_LIGHT_H

#include "Tongas2/RESOURCE.h"
#include "Tongas2/RESOURCES.h"
#include "Tongas2/SRESULT.h"

// A color that starts black.
struct LIGHTCOLOR
{
    LIGHTCOLOR() { r = g = b = 0.0f; }

    float r, g, b;
};

// Integer color components, as the renderer adds them up.
struct LIGHTRGB
{
    int r, g, b;
};

class LIGHTMANAGER : public RESOURCEMANAGER
{
public:
    virtual void Init();
    virtual void Update();
    // Defined by the renderer plugin: a vertex's color lit by the lights in
    // mask, from its own color and its normal, ambient the most each
    // component may reach.
    virtual void LightVertex(const LIGHTRGB &ambient, const V3I &color, const V3 &normal, int mask, LIGHTRGB &out) = 0;
    virtual void Virtual09() = 0;
    virtual void UpdateLights() = 0;

    // The sum of two colors, each component at most 255.
    void AddColors(const LIGHTRGB &a, const LIGHTRGB &b, int unused1, int unused2, LIGHTRGB &out);
    // The most lights in any of the groups in mask.
    int GetMaxCount(int mask);

    // How many lights each group has.
    int m_counts[RESOURCES_MAX_GROUPS];
};

extern RESOURCEMANAGER *g_pLIGHT;

class LIGHTMASTER : public RESOURCEMASTER
{
public:
    LIGHTMASTER(RESOURCEMANAGER *manager, const char *name);

    static void RegisterCommands();

    virtual int GetClassId();
    virtual int GetParentClassId();
    virtual const char *GetClassName();
    virtual SRESULT Execute(CMDENTRY *command, TOKENS **args);
    virtual RESOURCE *CreateCopy(const RESHANDLE &handle);

    virtual void SetColor(const LIGHTCOLOR &color);
    virtual void SetSpecular(const LIGHTCOLOR &color) { m_specular = color; }
    virtual void SetAmbient(const LIGHTCOLOR &color);
    virtual void SetType(const int &type) { m_type = type; }
    virtual void SetLightMask(const int &mask) { m_lightMask = mask; }
    virtual void SetRange(const float &range) { m_range = range; }
    virtual void SetAttenuation0(const float &value) { m_attenuation0 = value; }
    virtual void SetAttenuation1(const float &value) { m_attenuation1 = value; }
    virtual void SetAttenuation2(const float &value) { m_attenuation2 = value; }

    static int s_classId;
    static CMDENTRY s_commands[];
    static PROPENTRY s_properties[];

    LIGHTCOLOR m_color;
    LIGHTCOLOR m_specular;
    LIGHTCOLOR m_ambient;
    int m_type;
    int m_lightMask;
    float m_range;
    float m_attenuation0;
    float m_attenuation1;
    float m_attenuation2;
};

class LIGHT : public OBJECT
{
public:
    LIGHT(LIGHTMASTER *master, const RESHANDLE &handle);

    virtual void Init();
    virtual void Start() { AddCounts(); }
    virtual void Stop() { AddCounts(); }
    virtual bool IsLoaded() { return false; }
    virtual V3 GetScaleFactor() { return V3(0.0f, 0.0f, 0.0f); }

    virtual void SetColor(const LIGHTCOLOR &color);
    virtual const LIGHTCOLOR &GetColor() { return m_color; }
    virtual void SetSpecular(const LIGHTCOLOR &color);
    virtual const LIGHTCOLOR &GetSpecular() { return m_specular; }

    // Counts the light in each of its master's groups.
    __declspec(noinline) void AddCounts();

    LIGHTCOLOR m_color;
    LIGHTCOLOR m_baseColor;
    LIGHTCOLOR m_specular;
    LIGHTCOLOR m_baseSpecular;
};

#endif
