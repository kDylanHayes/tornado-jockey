// Actor resource templates: ACTORRESOURCESMASTER lists, from
// ACTORRESOURCES.txt, up to 16 resources an actor carries with their
// placement. Placeholder names apart from the class name and properties.
#ifndef TONGAS2_ACTORRESOURCES_H
#define TONGAS2_ACTORRESOURCES_H

#include "Tongas2/CSTRING.h"
#include "Tongas2/MATHS.h"
#include "Tongas2/RESOURCE.h"
#include "Tongas2/RESOURCES.h"
#include "Tongas2/SRESULT.h"

#define ACTORRESOURCES_MAX 16

// A position that starts at the origin.
struct ACTORRESOURCEPOS : V3
{
    ACTORRESOURCEPOS() : V3(0.0f, 0.0f, 0.0f) {}
    ACTORRESOURCEPOS &operator=(const V3 &v) { V3::operator=(v); return *this; }
};

// Angles that start at zero.
struct ACTORRESOURCEANG : ANG
{
    ACTORRESOURCEANG() : ANG(0.0f, 0.0f, 0.0f) {}
    ACTORRESOURCEANG &operator=(const ANG &a) { ANG::operator=(a); return *this; }
};

class ACTORRESOURCESMASTER : public RESOURCEMASTER
{
public:
    ACTORRESOURCESMASTER(RESOURCEMANAGER *manager, const char *name);

    static void RegisterCommands();

    virtual int GetClassId();
    virtual int GetParentClassId();
    virtual const char *GetClassName();
    virtual SRESULT Execute(CMDENTRY *command, TOKENS **args)
    {
        return (this->*(SRESULT (ACTORRESOURCESMASTER::*)(TOKENS **))command->func)(args);
    }

    virtual void SetResource(const int &i, const CSTRING &name) { m_resources[i] = name; }
    virtual void SetPos(const int &i, const V3 &pos);
    virtual void SetAng(const int &i, const ANG &ang);
    // Scales of 0 or less are logged and replaced by 1.
    virtual void SetScale(const int &i, const V3 &scale);
    virtual void SetOpacity(const int &i, const float &opacity);
    virtual void SetZSort(const int &i, const int &zsort);
    virtual void SetSpecific(const int &i, const int &specific);
    virtual void SetBoneAttach(const int &i, const CSTRING &bone) { m_boneAttach[i] = bone; }

    static int s_classId;
    static CMDENTRY s_commands[];
    static PROPENTRY s_properties[];

    CSTRING m_resources[ACTORRESOURCES_MAX];
    ACTORRESOURCEPOS m_pos[ACTORRESOURCES_MAX];
    ACTORRESOURCEANG m_ang[ACTORRESOURCES_MAX];
    ACTORRESOURCEPOS m_scale[ACTORRESOURCES_MAX];
    float m_opacity[ACTORRESOURCES_MAX];
    int m_zsort[ACTORRESOURCES_MAX];
    int m_specific[ACTORRESOURCES_MAX];
    CSTRING m_boneAttach[ACTORRESOURCES_MAX];
};

#endif
