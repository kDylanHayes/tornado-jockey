// Decals: a textured quad placed in the world (or on the screen), drawn
// with a copy of a mesh made for it. DECALMASTER holds what the decal
// type's file sets (the shader, the animation and how it turns), DECAL is
// one decal. Placeholder names apart from the class names and properties.
#ifndef TONGAS2_DECAL_H
#define TONGAS2_DECAL_H

#include "Tongas2/CAMERA.h"
#include "Tongas2/RESOURCE.h"
#include "Tongas2/RESOURCES.h"
#include "Tongas2/SHADER.h"
#include "Tongas2/TIMER.h"

// A corner of a decal's quad.
struct DECALVERTEX
{
    DECALVERTEX() : position(0.0f, 0.0f, 0.0f), u(0.0f), v(0.0f) {}

    V3 position;
    int color;
    float u, v;
};

// A colour, black until set.
struct DECALCOLOR : V3
{
    DECALCOLOR() { x = y = z = 0.0f; }
};

struct DECALUV
{
    DECALUV() : u(0.0f), v(0.0f) {}

    float u, v;
};

// Defined elsewhere: the copy of a mesh a decal is drawn with.
class MESH : public RESOURCE
{
public:
#ifdef MESH_DEFINITIONS
    // MESH.cpp alone was built seeing this.
    MESH(class MESHMASTER *master, const RESHANDLE &handle);
    virtual ~MESH() {}
    virtual bool IsLoaded() { return true; }
#endif
    virtual void SetColor(const V3 &color);
    virtual void SetPosition(const V3 &position);
    virtual void SetScale(const V3 &scale);
    virtual void SetAngles(const ANG &angles);
    virtual void SetLayer(int layer);
    virtual void SetVertices(DECALVERTEX *vertices, int numVertices, const short *indices, int numTriangles);

    int m_flags;
};

// Defined elsewhere: the manager g_pMESH points to.
class MESHMANAGER : public RESOURCEMANAGER
{
public:
    // A new mesh for the shader master.
    virtual RESHANDLE CreateMesh(RESHANDLE shader, int layer, const char *suffix);
};

class DECALMASTER : public RESOURCEMASTER
{
public:
    DECALMASTER(RESOURCEMANAGER *manager, const char *name);
    virtual ~DECALMASTER();

    static void RegisterCommands();

    virtual int GetClassId();
    virtual int GetParentClassId();
    virtual const char *GetClassName();
    virtual SRESULT Execute(CMDENTRY *command, TOKENS **args);
    virtual RESOURCE *CreateCopy(const RESHANDLE &handle);
    // Holds on to the shader while there are decals of the type.
    virtual void UpdateCopies();

    virtual void Virtual34() {}
    virtual void SetShader(const CSTRING &name);
    virtual void SetAlwaysLookCamera(const bool &value) { m_alwaysLookCamera = value; }
    virtual void SetNeverRotate(const bool &value) { m_neverRotate = value; }
    virtual void SetSimulate3D(const bool &value) { m_simulate3D = value; }
    virtual void SetDecalOrderForced(const int &value) { m_decalOrderForced = value; }
    virtual void SetAnimLen(const float &value) { m_animLen = value; }
    virtual void SetStartFrame(const int &value) { m_startFrame = value; }
    virtual void SetLastFrame(const int &value) { m_lastFrame = value; }
    virtual void SetLoop(const bool &value) { m_loop = value; }
    virtual void SetFwBk(const bool &value) { m_fwBk = value; }
    virtual void SetRandomStartFrame(const bool &value) { m_randomStartFrame = value; }
    virtual void SetLocalRotation(const ANG &value) { m_localRotation = value; }
    virtual void SetRotationSpeed(const V3 &value) { m_rotationSpeed = value; }

    void LoadShader();
    void ReleaseShader();

    static int s_classId;
    static CMDENTRY s_commands[];
    static PROPENTRY s_properties[];

    int m_numUsers;
    bool m_shaderLoaded;
    CSTRING m_shader;
    bool m_alwaysLookCamera;
    bool m_neverRotate;
    bool m_simulate3D;
    int m_decalOrderForced;
    float m_animLen;
    int m_startFrame;
    int m_lastFrame;
    bool m_loop;
    bool m_fwBk;
    bool m_randomStartFrame;
    ANG m_localRotation;
    V3 m_rotationSpeed;
    RESHANDLE m_shaderHandle;
};

class DECAL : public OBJECT
{
public:
    DECAL(DECALMASTER *master, RESHANDLE handle);
    virtual ~DECAL();

    virtual void Init();
    // Starts the animation and makes the mesh.
    virtual void Start();
    // Lets the mesh go.
    virtual void Stop();
    virtual void Update();
    virtual bool IsLoaded();
    virtual void UpdateAngles();
    virtual V3 GetScaleFactor() { return m_scaleFactor; }
    virtual V3 GetSize() { return m_scaleFactor; }
    virtual void SetPosition(V3 position);
    virtual void SetLayer(int layer);
    virtual void SetFrame(int frame) { m_frame = frame; }
    virtual void SetSize(V2 size) { SetScale(size.x, size.y, 1.0f); }
    virtual void SetScreenSize(float width, float height) { SetScale(width, height, 1.0f); }
    virtual V2 GetScreenSize() { return V2(m_scale.x, m_scale.y); }

    void CreateMesh();
    void CreateMeshCopy();
    void SetColor(const V3 &color);
    RESHANDLE GetShaderHandle();
    void SetUV(float u0, float v0, float u1, float v1);
    float GetRoll() { return m_angles.GetEuler().x; }

    DECALMASTER *Master() { return (DECALMASTER *)m_master; }

    // The corners of the quad, as x, y and z.
    static float s_corners[4][3];
    static short s_indices[6];

    DECALVERTEX m_vertices[4];
    bool m_meshReady;
    bool m_screenSpace;
    bool m_lastScreenSpace;
    V2 m_screenPosition;
    int m_frame;
    int m_lastFrame;
    bool m_scrolling;
    V2 m_scrollSpeed;
    V2 m_480;
    DECALUV m_uv[2];
    int m_frameOffset;
    bool m_animating;
    float m_animTime;
    bool m_backwards;
    float m_animLen;
    DECALCOLOR m_color;
    bool m_hasColor;
    ANG m_localRotation;
    RESHANDLE m_meshCopy;
    RESHANDLE m_mesh;
};

// Defined elsewhere.
extern RESOURCEMANAGER *g_pMESH;
extern RESOURCEMANAGER *g_pSHADER;

#endif
