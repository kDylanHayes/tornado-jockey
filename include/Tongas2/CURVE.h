// Curves: CURVEMASTER holds what a curve type's file sets (its width, its
// texture's shader, the colour and opacity at each end and whether it turns
// to face the camera), CURVE is one curve, a strip of quads through up to
// 129 points drawn with a copy of a mesh made for it. The curve types
// themselves (POLYTRAIL, TESLA) derive from these in other units.
// Placeholder names apart from the class names and properties.
#ifndef TONGAS2_CURVE_H
#define TONGAS2_CURVE_H

#include "Tongas2/DECAL.h"
#include "Tongas2/EMITTER.h"
#include "Tongas2/RESOURCE.h"
#include "Tongas2/RESOURCES.h"
#include "Tongas2/SRESULT.h"

#define CURVE_MAX_POINTS 129

class CURVEMASTER : public RESOURCEMASTER
{
public:
    CURVEMASTER(RESOURCEMANAGER *manager, const char *name);
    virtual ~CURVEMASTER();

    static void RegisterCommands();

    virtual int GetClassId();
    virtual int GetParentClassId();
    virtual const char *GetClassName();
    virtual SRESULT Execute(CMDENTRY *command, TOKENS **args)
    {
        return (this->*(SRESULT (CURVEMASTER::*)(TOKENS **))command->func)(args);
    }
    // Holds on to the shader while there are curves of the type.
    virtual void UpdateCopies();

    virtual void SetWidth(const float &width) { m_width = width; }
    // Also looks up the texture's shader.
    virtual void SetTexture(const SRTEXTURE &texture);
    virtual void SetColor(const int &i, const V3I &color) { m_color[i] = color; }
    virtual void SetAlpha(const int &i, const float &alpha) { m_alpha[i] = alpha; }
    virtual void SetLookCamera(const bool &look) { m_lookCamera = look; }

    void LoadShader();
    __declspec(noinline) void ReleaseShader();
    // Swaps the shader held, keeping it loaded if it was.
    void SetShaderHandle(const RESHANDLE &handle);

    static int s_classId;
    static CMDENTRY s_commands[];
    static PROPENTRY s_properties[];

    int m_numUsers;
    bool m_shaderLoaded;
    float m_width;
    SRTEXTURE m_texture;
    V3I m_color[2];
    float m_alpha[2];
    bool m_lookCamera;
    RESHANDLE m_shaderHandle;
};

// A point on a curve, at the origin until set.
struct CURVEPOINT : V3
{
    CURVEPOINT() : V3(0.0f, 0.0f, 0.0f) {}
};

// Texture coordinates that start as the whole texture.
struct CURVEUV : UVRECT
{
    CURVEUV() { left = top = 0.0f; right = bottom = 1.0f; }
    CURVEUV &operator=(const UVRECT &uv) { UVRECT::operator=(uv); return *this; }
};

class CURVE : public OBJECT
{
public:
    CURVE(CURVEMASTER *master, const RESHANDLE &handle);
    virtual ~CURVE()
    {
        if (m_master)
            Master()->m_numUsers--;
    }

    CURVEMASTER *Master() { return (CURVEMASTER *)m_master; }

    // Places the points, then puts the curve in its mesh.
    virtual void Start();
    // Takes the curve out of its mesh.
    virtual void Stop()
    {
        RESOURCE *mesh = g_pResources->GetResource(m_meshCopy);
        if (mesh)
            mesh->Virtual20();
        m_meshCopy.Invalidate();
    }
    // Waits for the shader, then takes the texture's part of it.
    virtual bool IsLoaded()
    {
        if (!m_loaded) {
            CURVEMASTER *master = Master();
            SHADER *shader = (SHADER *)g_pResources->GetResource(master->m_shaderHandle);
            if (shader) {
                if (!shader->IsLoaded())
                    return false;
                m_loaded = true;
                m_uv = shader->GetFrame(master->m_texture.index);
            }
        }
        return true;
    }
    virtual void SetLayer(int layer);
    // Where the curve type puts its points.
    virtual void PlacePoints() {}

    // Makes the mesh the curve is drawn with.
    void CreateMesh();
    // Works out the quads' corners and texture coordinates from the points.
    void UpdateMesh();
    // Puts the curve in its mesh.
    void AddMesh();
    // Uses count points, shading them from one end's colour to the other's.
    bool SetNumPoints(int count);

    static short s_indices[(CURVE_MAX_POINTS - 1) * 12];
    static bool s_indicesReady;

    bool m_loaded;
    CURVEPOINT m_points[CURVE_MAX_POINTS];
    float m_t[CURVE_MAX_POINTS];
    DECALVERTEX m_vertices[(CURVE_MAX_POINTS + 0) * 2];
    CURVEUV m_uv;
    int m_numPoints;
    RESHANDLE m_meshCopy;
    RESHANDLE m_mesh;
};

#endif
