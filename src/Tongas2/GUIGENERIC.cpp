// This file was built seeing SRESULT::FreeValue as one that can throw.
#define SRESULT_FREEVALUE_MAY_THROW

#include "Tongas2/GUIGENERIC.h"
#include "Tongas2/PATH.h"

class GUIGENERICMANAGER : public RESOURCEMANAGER
{
public:
    GUIGENERICMANAGER()
    {
        SafeSprintf(m_fileName, "GUIGENERIC.txt");
        m_8 = false;
        m_18 = true;
    }

    virtual RESOURCEMASTER *CreateMaster(const char *name) { return new GUIGENERICMASTER(this, name); }
    virtual const char *GetTypeCode();
};

static RESOURCEMANAGER *CreateGUIGENERIC();

CREATEMANAGER g_pfnCreateGUIGENERIC = CreateGUIGENERIC;

CMDENTRY GUIGENERICMASTER::s_commands[] = {
    { 0, 0, 0, 0, 0 },
};

PROPENTRY GUIGENERICMASTER::s_properties[] = {
    { "", 0, 15, 0, 0, false, GUIGENERIC_MAX, 0 },
    { "Resource", 0, 7, 0, PROP_FIELD(GUIGENERICMASTER, m_resources), false, GUIGENERIC_MAX, PROP_SETTER(GUIGENERICMASTER, SetResource) },
    { "BoneAttach", 0, 7, 0, PROP_FIELD(GUIGENERICMASTER, m_boneAttach), false, GUIGENERIC_MAX, PROP_SETTER(GUIGENERICMASTER, SetBoneAttach) },
    { "ScreenPos", 0, 10, 0, PROP_FIELD(GUIGENERICMASTER, m_screenPos), false, GUIGENERIC_MAX, PROP_SETTER(GUIGENERICMASTER, SetScreenPos) },
    { "ScreenPosZ", 0, 3, 0, PROP_FIELD(GUIGENERICMASTER, m_screenPosZ), false, GUIGENERIC_MAX, PROP_SETTER(GUIGENERICMASTER, SetScreenPosZ) },
    { "Ang", 0, 12, 0, PROP_FIELD(GUIGENERICMASTER, m_ang), false, GUIGENERIC_MAX, PROP_SETTER(GUIGENERICMASTER, SetAng) },
    { "AngVel", 0, 11, 0, PROP_FIELD(GUIGENERICMASTER, m_angVel), false, GUIGENERIC_MAX, PROP_SETTER(GUIGENERICMASTER, SetAngVel) },
    { "Scale", 0, 11, 0, PROP_FIELD(GUIGENERICMASTER, m_scale), false, GUIGENERIC_MAX, PROP_SETTER(GUIGENERICMASTER, SetScale) },
    { "Opacity", 0, 3, 0, PROP_FIELD(GUIGENERICMASTER, m_opacity), false, GUIGENERIC_MAX, PROP_SETTER(GUIGENERICMASTER, SetOpacity) },
    { "ZSort", 0, 0, 0, PROP_FIELD(GUIGENERICMASTER, m_zsort), false, GUIGENERIC_MAX, PROP_SETTER(GUIGENERICMASTER, SetZSort) },
    { "Specific", 0, 0, 0, PROP_FIELD(GUIGENERICMASTER, m_specific), false, GUIGENERIC_MAX, PROP_SETTER(GUIGENERICMASTER, SetSpecific) },
    { 0, 0, 15, 0, 0, false },
};

void GUIGENERICMASTER::RegisterCommands()
{
    g_classParents[s_classId] = RESOURCEMASTER::s_classId;
    g_classCommands[s_classId] = s_commands;
    HashCommands(s_commands);
    RegisterClassName(s_commands, "GUIGENERICMASTER", "RESOURCEMASTER");
}

const char *GUIGENERICMANAGER::GetTypeCode()
{
    return "GGEN";
}

static RESOURCEMANAGER *CreateGUIGENERIC()
{
    return new GUIGENERICMANAGER;
}

GUIGENERICMASTER::GUIGENERICMASTER(RESOURCEMANAGER *manager, const char *name)
    : RESOURCEMASTER(manager, name)
{
    for (g_i = 0; g_i < GUIGENERIC_MAX; g_i++)
        m_resources[g_i] = "";
    for (g_i = 0; g_i < GUIGENERIC_MAX; g_i++)
        m_boneAttach[g_i] = "";
    for (g_i = 0; g_i < GUIGENERIC_MAX; g_i++)
        m_screenPos[g_i] = V2(0.0f, 0.0f);
    for (g_i = 0; g_i < GUIGENERIC_MAX; g_i++)
        m_screenPosZ[g_i] = 1.0f;
    for (g_i = 0; g_i < GUIGENERIC_MAX; g_i++)
        m_ang[g_i] = ANG(0.0f, 0.0f, 0.0f);
    for (g_i = 0; g_i < GUIGENERIC_MAX; g_i++)
        m_angVel[g_i] = V3(0.0f, 0.0f, 0.0f);
    for (g_i = 0; g_i < GUIGENERIC_MAX; g_i++)
        m_scale[g_i] = V3(1.0f, 1.0f, 1.0f);
    for (g_i = 0; g_i < GUIGENERIC_MAX; g_i++)
        m_opacity[g_i] = 1.0f;
    for (g_i = 0; g_i < GUIGENERIC_MAX; g_i++)
        m_zsort[g_i] = 0;
    for (g_i = 0; g_i < GUIGENERIC_MAX; g_i++)
        m_specific[g_i] = 0;
    m_60 = 0;
}

int GUIGENERICMASTER::GetClassId()
{
    return s_classId;
}

int GUIGENERICMASTER::GetParentClassId()
{
    return g_classParents[s_classId];
}

const char *GUIGENERICMASTER::GetClassName()
{
    return g_classNames[s_classId];
}

void GUIGENERICMASTER::SetScreenPos(const int &i, const V2 &pos)
{
    m_screenPos[i] = pos;
}

void GUIGENERICMASTER::SetScreenPosZ(const int &i, const float &z)
{
    m_screenPosZ[i] = z;
}

void GUIGENERICMASTER::SetAng(const int &i, const ANG &ang)
{
    m_ang[i] = ang;
}

void GUIGENERICMASTER::SetAngVel(const int &i, const V3 &angVel)
{
    m_angVel[i] = angVel;
}

void GUIGENERICMASTER::SetScale(const int &i, const V3 &scale)
{
    m_scale[i] = scale;
}

void GUIGENERICMASTER::SetOpacity(const int &i, const float &opacity)
{
    m_opacity[i] = opacity;
}

void GUIGENERICMASTER::SetZSort(const int &i, const int &zsort)
{
    m_zsort[i] = zsort;
}

void GUIGENERICMASTER::SetSpecific(const int &i, const int &specific)
{
    m_specific[i] = specific;
}
