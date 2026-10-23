// Effects: FXMASTER, from FX.txt, lists up to 32 resources an effect
// starts one after another, each after its wait and for its life, placed
// relative to the effect, attached to another of its resources' bones, or
// kept at a point on the screen. An FX is one playing effect.
// Placeholder names apart from the class names and properties.
#ifndef TONGAS2_FX_H
#define TONGAS2_FX_H

#include "Tongas2/CSTRING.h"
#include "Tongas2/MATHS.h"
#include "Tongas2/RESOURCE.h"
#include "Tongas2/RESOURCES.h"
#include "Tongas2/SRESULT.h"

#define FX_MAX 32

// A screen position that starts at the origin.
struct FXV2 : V2
{
    FXV2() : V2(0.0f, 0.0f) {}
    FXV2 &operator=(const V2 &v) { V2::operator=(v); return *this; }
};

// A vector that starts at zero.
struct FXV3 : V3
{
    FXV3() : V3(0.0f, 0.0f, 0.0f) {}
    FXV3 &operator=(const V3 &v) { V3::operator=(v); return *this; }
};

// Angles that start at zero.
struct FXANG : ANG
{
    FXANG() : ANG(0.0f, 0.0f, 0.0f) {}
    FXANG &operator=(const ANG &a) { ANG::operator=(a); return *this; }
};

class FXMASTER : public RESOURCEMASTER
{
public:
    FXMASTER(RESOURCEMANAGER *manager, const char *name);

    static void RegisterCommands();

    virtual int GetClassId();
    virtual int GetParentClassId();
    virtual const char *GetClassName();
    virtual SRESULT Execute(CMDENTRY *command, TOKENS **args)
    {
        return (this->*(SRESULT (FXMASTER::*)(TOKENS **))command->func)(args);
    }
    virtual RESOURCE *CreateCopy(const RESHANDLE &handle);

    virtual void SetRandomMode(const bool &random);
    virtual void SetLooping(const bool &looping);
    virtual void SetForceCompletePlay(const bool &force);
    virtual void SetWait(const int &i, const float &wait);
    virtual void SetPos(const int &i, const V3 &pos);
    virtual void SetAng(const int &i, const ANG &ang);
    virtual void SetScale(const int &i, const V3 &scale);
    virtual void SetOpacity(const int &i, const float &opacity);
    virtual void SetZSort(const int &i, const int &zsort);
    virtual void SetSpecific(const int &i, const int &specific);
    virtual void SetLife(const int &i, const float &life);
    virtual void SetBoneAttach(const int &i, const CSTRING &bone) { m_boneAttach[i] = bone; }
    virtual void SetCommand(const int &i, const CSTRING &command) { m_command[i] = command; }
    // Also counts the resources up to the last one named.
    virtual void SetResource(const int &i, const CSTRING &name)
    {
        m_resource[i] = name;
        m_handles[i].Invalidate();
        m_count = 0;
        for (int j = FX_MAX - 1; j >= 0; j--) {
            if (m_resource[j].Length() > 0) {
                m_count = j + 1;
                return;
            }
        }
    }
    virtual void SetScreenPos(const V2 &pos);
    virtual void SetForceScreenPos(const bool &force);
    virtual void SetDetachToScreen(const bool &detach);

    // In random mode, a resource not played lately.
    int PickRandom();

    static int s_classId;
    static CMDENTRY s_commands[];
    static PROPENTRY s_properties[];

    bool m_randomMode;
    bool m_looping;
    bool m_forceCompletePlay;
    float m_wait[FX_MAX];
    FXV3 m_pos[FX_MAX];
    FXANG m_ang[FX_MAX];
    FXV3 m_scale[FX_MAX];
    float m_opacity[FX_MAX];
    int m_zsort[FX_MAX];
    int m_specific[FX_MAX];
    float m_life[FX_MAX];
    CSTRING m_boneAttach[FX_MAX];
    CSTRING m_command[FX_MAX];
    CSTRING m_resource[FX_MAX];
    FXV2 m_screenPos;
    bool m_forceScreenPos;
    bool m_detachToScreen;
    int m_count;
    RESHANDLE m_handles[FX_MAX];
    bool m_used[FX_MAX];
    int m_last;
};

class FX : public OBJECT
{
public:
    FX(FXMASTER *master, const RESHANDLE &handle);
    virtual ~FX() { Reset(); }

    FXMASTER *Master() { return (FXMASTER *)m_master; }

    virtual void Start() { Reset(); }
    virtual void Stop()
    {
        if (m_finishing)
            Virtual20();
        Reset();
    }
    // Places the playing resources, starts the ones whose wait is over and
    // lets go of the ones whose life is.
    virtual void Update();
    virtual bool IsLoaded() { return true; }
    // Lets the effect play to its end first when the master says so.
    virtual void Virtual31();
    virtual V3 GetScaleFactor() { return m_scaleFactor; }

    // Lets go of the resources and starts over.
    void Reset();
    // Starts resource i.
    void Spawn(int i);

    RESHANDLE m_children[FX_MAX];
    bool m_spawned[FX_MAX];
    bool m_done[FX_MAX];
    float m_start[FX_MAX];
    float m_time;
    float m_duration;
    int m_chosen;
    bool m_finishing;
    FXV2 m_screenPos;
    bool m_forceScreenPos;
};

#endif
