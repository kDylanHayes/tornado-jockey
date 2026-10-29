// Boned models: BONEDMODELMASTER, from BONEDMODEL.txt, names a model file
// under data/art/ with its render passes and whether its normals are
// normalized; BONEDMODELRESOURCE is the base of each placed copy, which
// plays the animation it is asked for once it starts. The copies' classes
// derive from it in other units.
// Placeholder names apart from the class names and properties.
#ifndef TONGAS2_BONEDMODEL_H
#define TONGAS2_BONEDMODEL_H

#include "Tongas2/CSTRING.h"
#include "Tongas2/RESOURCE.h"
#include "Tongas2/RESOURCES.h"
#include "Tongas2/SRESULT.h"

// A vector that starts at zero.
struct BONEDMODELV3 : V3
{
    BONEDMODELV3() : V3(0.0f, 0.0f, 0.0f) {}
};

// The model's bounds, once the model has set them.
struct BONEDMODELBOUNDS
{
    BONEDMODELBOUNDS() : set(false) {}

    BONEDMODELV3 min;
    BONEDMODELV3 max;
    bool set;
};

class BONEDMODELMASTER : public RESOURCEMASTER
{
public:
    BONEDMODELMASTER(RESOURCEMANAGER *manager, const char *name);
    virtual ~BONEDMODELMASTER() {}

    static void RegisterCommands();

    virtual int GetClassId();
    virtual int GetParentClassId();
    virtual const char *GetClassName();
    virtual SRESULT Execute(CMDENTRY *command, TOKENS **args)
    {
        return (this->*(SRESULT (BONEDMODELMASTER::*)(TOKENS **))command->func)(args);
    }
    // Works out the model's path under data/art/.
    virtual void Prepare();

    // Also names the master after the file.
    virtual void SetFileName(const CSTRING &file)
    {
        if (file.Length() > 0)
            SetName(file);
        m_file = file;
    }
    virtual void SetRenderPassesFlags(const int &flags) { m_renderPassesFlags = flags; }
    virtual void SetNormalizeNormals(const bool &normalize);

    static int s_classId;
    static CMDENTRY s_commands[];
    static PROPENTRY s_properties[];

    CSTRING m_file;
    int m_renderPassesFlags;
    bool m_normalizeNormals;
    CSTRING m_artPath;
    BONEDMODELBOUNDS m_bounds;
};

class BONEDMODELRESOURCE : public OBJECT
{
public:
    BONEDMODELRESOURCE(BONEDMODELMASTER *master, const RESHANDLE &handle);
    virtual ~BONEDMODELRESOURCE() {}

    // Resets the speed, then lets the model go.
    virtual void Virtual20()
    {
        m_speed = 1.0f;
        Virtual87();
        OBJECT::Virtual20();
    }
    // Follows the parent, then plays the animation asked for, if any.
    virtual void Start();
    virtual void Stop() { StopAnimation(); }
    virtual void Update() { OBJECT::Update(); }
    virtual bool IsLoaded() { return true; }
    // Takes the parent's speed.
    virtual void Virtual30()
    {
        if (m_parent.IsValid()) {
            WTACTOR *parent = g_pScene->GetActor(m_parent);
            if (parent)
                m_speed = parent->m_988;
        }
    }
    // Twice the larger extent of the bounds on each axis.
    virtual V3 GetScaleFactor();
    virtual void FollowParent(WTACTOR *parent);
    // The animation to play when the copy starts: n + 1 to play n once,
    // -(n + 1) to loop it.
    virtual void SetFrame(int frame) { m_animation = frame; }

    // Swaps material from for to on the model; whether it could.
    virtual bool ChangeMaterial(const char *from, const char *to);
    virtual void Virtual71() {}
    // The master's bounds, scaled.
    virtual void GetBounds(V3 *min, V3 *max);
    // Shows a frame of an animation.
    virtual void SetAnimationFrame(const char *animation, float frame) = 0;
    virtual void PlayAnimation(void *animation, int once, float speed) = 0;
    // Blends into an animation.
    virtual void BlendAnimation(const char *animation, float blend, bool loop, float speed) = 0;
    virtual void StopAnimation() {}
    virtual void PauseAnimation(bool pause) {}
    virtual void RestartAnimation() {}
    virtual int Virtual79(int a) { return -1; }
    virtual void *GetAnimation(int *index) { return 0; }
    virtual int Virtual81() { return 0; }
    virtual V3 Virtual82(int a) { return V3(0.0f, 0.0f, 0.0f); }
    virtual ANG Virtual83(int a) { return ANG(0.0f, 0.0f, 0.0f); }
    virtual int Virtual84(int a) { return -1; }
    virtual void *Virtual85(int a) { return 0; }
    virtual void Virtual86() {}
    virtual void Virtual87() {}

    void SetSpeed(const float &speed);

    bool m_400;
    int m_animation;
    float m_speed;
    int m_40c;
};

#endif
