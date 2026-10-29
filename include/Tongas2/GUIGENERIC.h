// Generic GUI element templates: GUIGENERICMASTER lists, from
// GUIGENERIC.txt, up to 32 resources a GUI element draws with their screen
// placement. Placeholder names apart from the class name and properties.
#ifndef TONGAS2_GUIGENERIC_H
#define TONGAS2_GUIGENERIC_H

#include "Tongas2/CSTRING.h"
#include "Tongas2/GUICONTROL.h"
#include "Tongas2/MATHS.h"
#include "Tongas2/RESOURCE.h"
#include "Tongas2/RESOURCES.h"
#include "Tongas2/SRESULT.h"

#define GUIGENERIC_MAX 32

// A screen position that starts at the origin.
struct GUIGENERICPOS2 : V2
{
    GUIGENERICPOS2() : V2(0.0f, 0.0f) {}
    GUIGENERICPOS2 &operator=(const V2 &v) { V2::operator=(v); return *this; }
};

// A vector that starts at zero.
struct GUIGENERICV3 : V3
{
    GUIGENERICV3() : V3(0.0f, 0.0f, 0.0f) {}
    GUIGENERICV3 &operator=(const V3 &v) { V3::operator=(v); return *this; }
};

// Angles that start at zero.
struct GUIGENERICANG : ANG
{
    GUIGENERICANG() : ANG(0.0f, 0.0f, 0.0f) {}
    GUIGENERICANG &operator=(const ANG &a) { ANG::operator=(a); return *this; }
};

class GUIGENERICMASTER : public RESOURCEMASTER
{
public:
    GUIGENERICMASTER(RESOURCEMANAGER *manager, const char *name);
    virtual ~GUIGENERICMASTER() {}

    static void RegisterCommands();

    virtual int GetClassId();
    virtual int GetParentClassId();
    virtual const char *GetClassName();
    virtual SRESULT Execute(CMDENTRY *command, TOKENS **args)
    {
        return (this->*(SRESULT (GUIGENERICMASTER::*)(TOKENS **))command->func)(args);
    }

    virtual void SetResource(const int &i, const CSTRING &name) { m_resources[i] = name; }
    virtual void SetBoneAttach(const int &i, const CSTRING &bone) { m_boneAttach[i] = bone; }
    virtual void SetScreenPos(const int &i, const V2 &pos);
    virtual void SetScreenPosZ(const int &i, const float &z);
    virtual void SetAng(const int &i, const ANG &ang);
    virtual void SetAngVel(const int &i, const V3 &angVel);
    virtual void SetScale(const int &i, const V3 &scale);
    virtual void SetOpacity(const int &i, const float &opacity);
    virtual void SetZSort(const int &i, const int &zsort);
    virtual void SetSpecific(const int &i, const int &specific);

    static int s_classId;
    static CMDENTRY s_commands[];
    static PROPENTRY s_properties[];

    int m_60;
    CSTRING m_resources[GUIGENERIC_MAX];
    CSTRING m_boneAttach[GUIGENERIC_MAX];
    GUIGENERICPOS2 m_screenPos[GUIGENERIC_MAX];
    float m_screenPosZ[GUIGENERIC_MAX];
    GUIGENERICANG m_ang[GUIGENERIC_MAX];
    GUIGENERICV3 m_angVel[GUIGENERIC_MAX];
    GUIGENERICV3 m_scale[GUIGENERIC_MAX];
    float m_opacity[GUIGENERIC_MAX];
    int m_zsort[GUIGENERIC_MAX];
    int m_specific[GUIGENERIC_MAX];
};

// A GUI element made from a GUIGENERICMASTER: up to 32 objects (models,
// decals) placed in the world in front of the camera at the template's
// screen positions and depths, scaled and turned with the control, and
// placed again whenever the camera moves.
class GUIGENERIC : public GUICONTROL
{
public:
    GUIGENERIC(const RESHANDLE &master, GUIGROUP *parent);
    virtual ~GUIGENERIC();

    static void RegisterCommands();

    virtual int GetClassId() { return s_classId; }
    virtual int GetParentClassId() { return g_classParents[s_classId]; }
    virtual const char *GetClassName() { return g_classNames[s_classId]; }
    virtual SRESULT Execute(CMDENTRY *command, TOKENS **args)
    {
        return (this->*(SRESULT (GUIGENERIC::*)(TOKENS **))command->func)(args);
    }
    virtual int Virtual19() { return 1; }
    virtual int Virtual20() { return 0; }
    virtual void SetPosition(const V2 &position);
    virtual void SetSize(const V2 &size);
    virtual void SetZSort(const int &zsort);
    virtual void SetOpacity(const float &opacity);
    virtual void Update();

    // The properties' setters: each object's angles, turning speed, scale,
    // screen position and depth.
    virtual void SetAng(const int &i, const ANG &ang)
    {
        m_ang[i] = ang;
        m_cameraDirection = m_cameraUp;
    }
    virtual void SetAngVel(const int &i, const V3 &angVel) { m_angVel[i] = angVel; }
    virtual void SetScale(const int &i, const V3 &scale)
    {
        m_scale[i] = scale;
        SetSize(m_size);
    }
    virtual void SetScreenPos(const int &i, const V2 &pos)
    {
        m_screenPos[i] = pos;
        m_cameraPos = V3(-1.0f, -1.0f, -1.0f);
    }
    virtual void SetScreenPosZ(const int &i, const float &z)
    {
        m_screenPosZ[i] = z;
        m_cameraPos = V3(-1.0f, -1.0f, -1.0f);
    }
    // The script commands.
    virtual SRESULT ActorShowPart(CMDCALL *call);
    virtual SRESULT PlayAnimation(CMDCALL *call);
    virtual SRESULT BlendAnimation(CMDCALL *call);

    // Turns the objects: those attached to a bone by their own angles, the
    // others by theirs and the camera's.
    void UpdateAngles();
    // Plays or blends into an animation on one of the objects.
    void PlayResourceAnimation(int i, const char *animation, bool loop, float speed);
    void BlendResourceAnimation(int i, const char *animation, float blend, bool loop, float speed);

    static int s_classId;
    static CMDENTRY s_commands[];
    static PROPENTRY s_properties[];

    GUIGENERICANG m_ang[GUIGENERIC_MAX];
    GUIGENERICV3 m_angVel[GUIGENERIC_MAX];
    GUIGENERICV3 m_scale[GUIGENERIC_MAX];
    GUIGENERICPOS2 m_screenPos[GUIGENERIC_MAX];
    float m_screenPosZ[GUIGENERIC_MAX];
    // Where the camera was, which way it looked and its field of view when
    // the objects were last placed.
    V3 m_cameraPos;
    V3 m_cameraDirection;
    V3 m_cameraUp;
    float m_cameraFov;
    GUIGENERICMASTER *m_genericMaster;
    RESHANDLE m_handles[GUIGENERIC_MAX];
    // The object each one is attached to a bone of, or -1.
    int m_attach[GUIGENERIC_MAX];
};

#endif
