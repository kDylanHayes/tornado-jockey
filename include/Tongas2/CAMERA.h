// The camera: an actor whose position and angles are the view's. CAMERALENS
// turns them into the view, projection and frustum, perspective or
// orthographic; the renderer replaces it with its own through
// g_pfnCreateCameraLens. Scripts can make the camera shake (jitter) and pan
// or turn it to a target over time. Placeholder names apart from the class
// names, properties and commands.
#ifndef TONGAS2_CAMERA_H
#define TONGAS2_CAMERA_H

#include "Tongas2/ACTOR.h"

#define DEG2RAD(degrees) (0.017453292f * (degrees))

// Defined elsewhere: the corners of a box, a unit cube until set.
struct BOXCORNERS
{
    BOXCORNERS();

    V3 corners[8];
};

// What the camera sees: its planes and corners.
struct FRUSTUM
{
    FRUSTUM();

    // Defined elsewhere: the frustum of an inverted view-projection matrix,
    // turned by rotation.
    void Build(const MATRIX &inverseViewProj, const MATRIX &rotation);

    PLANE m_planes[6];
    BOXCORNERS m_corners;
    V3 m_c0;
};

// The view through the camera. Each matrix is rebuilt when what it depends
// on changes.
class CAMERALENS
{
public:
    // Seen as not throwing: making one keeps no unwind state for it.
    CAMERALENS() throw();
    virtual ~CAMERALENS() {}

    virtual void SetPosition(const V3 &position)
    {
        if (Differ(m_position, position)) {
            m_viewDirty = true;
            m_billboardDirty = true;
            m_frustumDirty = true;
        }
        m_position = position;
    }
    virtual void SetOrientation(const V3 &direction, const V3 &up)
    {
        if (direction.x != m_direction.x || direction.y != m_direction.y || direction.z != m_direction.z
            || up.x != m_up.x || up.y != m_up.y || up.z != m_up.z) {
            m_viewDirty = true;
            m_billboardDirty = true;
        }
        m_direction = direction;
        m_up = up;
    }
    // The horizontal field of view, in degrees.
    virtual void SetFov(const float &fov)
    {
        if (fov != m_fov)
            m_projDirty = true;
        m_fovY = VerticalFov(fov);
        m_fov = fov;
    }
    virtual void SetReflectionFov(const float &fov)
    {
        m_reflectionFov = fov;
        m_reflectionFovY = VerticalFov(fov);
    }
    virtual void SetFar(const float &distance)
    {
        if (m_far != distance)
            m_projDirty = true;
        m_far = distance;
    }
    virtual void SetNear(const float &distance)
    {
        if (m_near != distance)
            m_projDirty = true;
        m_near = distance;
    }
    virtual void SetAspect(const float &aspect)
    {
        if (m_aspect != aspect)
            m_projDirty = true;
        m_aspect = aspect;
    }
    virtual void SetFlag4c(const bool &value) { m_4c = value; }
    virtual void SetOrtho(const bool &ortho)
    {
        if (m_ortho != ortho)
            m_projDirty = true;
        m_ortho = ortho;
    }
    virtual void SetOrthoWidth(const float &width)
    {
        m_orthoWidth = width;
        m_projDirty = true;
    }
    // Where a point lands on the screen; the renderer's lens knows.
    virtual V2 Project(const V3 &point) { return V2(0.0f, 0.0f); }
    virtual V3 Unproject(const V2 &point) { return V3(0.0f, 0.0f, 0.0f); }
    virtual void Virtual13() {}
    virtual void UpdateFrustum();
    virtual void UpdateBillboard();
    virtual void UpdateView();
    virtual void UpdateProjection();

    float GetFar();
    float GetNear();
    // Rebuilds what changed, or everything.
    void Update(bool force);
    FRUSTUM *GetFrustum();

    static bool Differ(const V3 &a, const V3 &b) { return b.x != a.x || b.y != a.y || b.z != a.z; }
    static float VerticalFov(float fov) { return 2.0f * (float)atan(tan(DEG2RAD(fov * 0.5f)) * 0.75f); }

    V3 m_position;
    V3 m_direction;
    V3 m_up;
    float m_fov;
    float m_fovY;
    float m_reflectionFov;
    float m_reflectionFovY;
    float m_far;
    float m_near;
    float m_aspect;
    bool m_ortho;
    float m_orthoWidth;
    bool m_4c;
    bool m_viewDirty;
    MATRIX m_view;
    bool m_projDirty;
    MATRIX m_projection;
    bool m_billboardDirty;
    // The view's rotation undone, for things that face the camera.
    MATRIX m_billboard;
    bool m_frustumDirty;
    FRUSTUM m_frustum;
    MATRIX m_inverseViewProj;
    bool m_228;
};

// Makes the lens a camera uses; the renderer points it at its own.
extern CAMERALENS *(*g_pfnCreateCameraLens)();

class CAMERA : public ACTOR
{
public:
    CAMERA();
    virtual ~CAMERA();

    static void RegisterCommands();

    virtual int GetClassId() { return s_classId; }
    virtual int GetParentClassId() { return g_classParents[s_classId]; }
    virtual const char *GetClassName() { return g_classNames[s_classId]; }
    virtual SRESULT Execute(CMDENTRY *command, TOKENS **args)
    {
        return (this->*(SRESULT (CAMERA::*)(TOKENS **))command->func)(args);
    }
    virtual int GetFactoryId() { return s_factoryId; }
    virtual int GetParentFactoryId() { return ACTOR::s_factoryId; }
    virtual int GetFactoryFlags() { return g_factoryFlags[s_factoryId]; }
    virtual void SetPosition(const V3 &position);
    virtual void SetAngles(const ANG &angles);
    virtual void Virtual101() { m_998 = true; }
    virtual void Update();
    virtual void Virtual103();

    virtual void SetFov(const float &fov);
    virtual void SetReflectionFov(const float &fov);
    // How far the camera sees.
    virtual void SetClipping(const float &distance) { m_lens->SetFar(distance); }
    virtual void SetPausedMovement(const bool &paused) { m_pausedMovement = paused; }
    virtual void SetUseOrthoProjection(const bool &ortho);
    virtual void SetOrthoBoxWidth(const float &width)
    {
        if (m_orthoBoxWidth != width) {
            m_orthoBoxWidth = width;
            m_lens->SetOrthoWidth(width);
        }
    }
    // Puts the camera back where it starts, once.
    virtual void Reset();
    // Where a point in the world lands on the screen, from 0 to 1.
    virtual void WorldToScreen(V3 point, V2 *screen);
    virtual void Virtual148() {}
    virtual SRESULT Jitter(CMDCALL *call);
    virtual SRESULT PanTo(CMDCALL *call);
    virtual SRESULT RotateTo(CMDCALL *call);

    // The size of the view at distance 1.
    V2 GetViewSize();
    // The point at depth in front of the camera, from a point on the screen.
    V3 ScreenToWorld(const V2 &point, const float &depth);
    // Stops any jitter, pan and turn and puts the camera at position.
    void ResetMotion(V3 position);
    void StopJitter();
    void StopPan()
    {
        m_panTime = 0.0f;
        m_panTarget.x = m_panTarget.y = m_panTarget.z = 0.0f;
    }
    void StopRotate()
    {
        m_rotateTime = 0.0f;
        m_rotateTarget.euler.x = m_rotateTarget.euler.y = m_rotateTarget.euler.z = 0.0f;
        m_rotateTarget.flags = ANG::MATRIX_DIRTY;
    }

    static int s_classId;
    static int s_factoryId;
    static CMDENTRY s_commands[];
    static PROPENTRY s_properties[];

    bool m_initialized;
    V3 m_cd0;
    bool m_cdc;
    V3 m_ce0;
    V3 m_cec;
    // The height of the view at distance 1.
    float m_viewHeight;
    float m_jitterTime;
    float m_jitterAmount;
    V3 m_jitter;
    float m_panTime;
    V3 m_panTarget;
    float m_rotateTime;
    ANG m_rotateTarget;
    // The field of view from before the projection turned orthographic.
    float m_savedFov;
    float m_fov;
    float m_reflectionFov;
    float m_clipping;
    bool m_pausedMovement;
    bool m_useOrthoProjection;
    float m_orthoBoxWidth;
    V3 m_up;
    V3 m_direction;
    CAMERALENS *m_lens;
};

// Makes the camera; the game can point it at its own.
extern CAMERA *(*g_pfnCreateCamera)();

#endif
