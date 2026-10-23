// This file was built seeing SRESULT::FreeValue as one that can throw.
#define SRESULT_FREEVALUE_MAY_THROW

#include "Tongas2/ACTORRESOURCES.h"
#include "Tongas2/LOG.h"
#include "Tongas2/PATH.h"

class ACTORRESOURCESMANAGER : public RESOURCEMANAGER
{
public:
    ACTORRESOURCESMANAGER()
    {
        SafeSprintf(m_fileName, "ACTORRESOURCES.txt");
        m_8 = false;
        m_18 = true;
    }

    virtual RESOURCEMASTER *CreateMaster(const char *name) { return new ACTORRESOURCESMASTER(this, name); }
    virtual const char *GetTypeCode();
};

static RESOURCEMANAGER *CreateACTORRESOURCES();

CREATEMANAGER g_pfnCreateACTORRESOURCES = CreateACTORRESOURCES;

CMDENTRY ACTORRESOURCESMASTER::s_commands[] = {
    { 0, 0, 0, 0, 0 },
};

PROPENTRY ACTORRESOURCESMASTER::s_properties[] = {
    { "Resources", 0, 7, 0, PROP_FIELD(ACTORRESOURCESMASTER, m_resources), false, ACTORRESOURCES_MAX, PROP_SETTER(ACTORRESOURCESMASTER, SetResource) },
    { "ResourcesPos", 0, 11, 0, PROP_FIELD(ACTORRESOURCESMASTER, m_pos), false, ACTORRESOURCES_MAX, PROP_SETTER(ACTORRESOURCESMASTER, SetPos) },
    { "ResourcesAng", 0, 12, 0, PROP_FIELD(ACTORRESOURCESMASTER, m_ang), false, ACTORRESOURCES_MAX, PROP_SETTER(ACTORRESOURCESMASTER, SetAng) },
    { "ResourcesScale", 0, 11, 0, PROP_FIELD(ACTORRESOURCESMASTER, m_scale), false, ACTORRESOURCES_MAX, PROP_SETTER(ACTORRESOURCESMASTER, SetScale) },
    { "ResourcesOpacity", 0, 3, 0, PROP_FIELD(ACTORRESOURCESMASTER, m_opacity), false, ACTORRESOURCES_MAX, PROP_SETTER(ACTORRESOURCESMASTER, SetOpacity) },
    { "ResourcesZSort", 0, 0, 0, PROP_FIELD(ACTORRESOURCESMASTER, m_zsort), false, ACTORRESOURCES_MAX, PROP_SETTER(ACTORRESOURCESMASTER, SetZSort) },
    { "ResourcesSpecific", 0, 0, 0, PROP_FIELD(ACTORRESOURCESMASTER, m_specific), false, ACTORRESOURCES_MAX, PROP_SETTER(ACTORRESOURCESMASTER, SetSpecific) },
    { "ResourcesBoneAttach", 0, 7, 0, PROP_FIELD(ACTORRESOURCESMASTER, m_boneAttach), false, ACTORRESOURCES_MAX, PROP_SETTER(ACTORRESOURCESMASTER, SetBoneAttach) },
    { 0, 0, 15, 0, 0, false },
};

void ACTORRESOURCESMASTER::RegisterCommands()
{
    g_classParents[s_classId] = RESOURCEMASTER::s_classId;
    g_classCommands[s_classId] = s_commands;
    HashCommands(s_commands);
    RegisterClassName(s_commands, "ACTORRESOURCESMASTER", "RESOURCEMASTER");
}

void ACTORRESOURCESMASTER::SetPos(const int &i, const V3 &pos)
{
    m_pos[i] = pos;
}

void ACTORRESOURCESMASTER::SetAng(const int &i, const ANG &ang)
{
    m_ang[i] = ang;
}

void ACTORRESOURCESMASTER::SetScale(const int &i, const V3 &scale)
{
    m_scale[i] = scale;
    if (scale.x <= 0.0f || scale.y <= 0.0f || scale.z <= 0.0f) {
        if (g_pLog)
            g_pLog->Error("setting a scale with value<=0 in template %s", m_path.m_str);
        if (scale.x <= 0.0f)
            m_scale[i].x = 1.0f;
        if (scale.y <= 0.0f)
            m_scale[i].y = 1.0f;
        if (scale.z <= 0.0f)
            m_scale[i].z = 1.0f;
    }
}

void ACTORRESOURCESMASTER::SetOpacity(const int &i, const float &opacity)
{
    m_opacity[i] = opacity;
}

void ACTORRESOURCESMASTER::SetZSort(const int &i, const int &zsort)
{
    m_zsort[i] = zsort;
}

void ACTORRESOURCESMASTER::SetSpecific(const int &i, const int &specific)
{
    m_specific[i] = specific;
}

const char *ACTORRESOURCESMANAGER::GetTypeCode()
{
    return "ACRS";
}

static RESOURCEMANAGER *CreateACTORRESOURCES()
{
    return new ACTORRESOURCESMANAGER;
}

ACTORRESOURCESMASTER::ACTORRESOURCESMASTER(RESOURCEMANAGER *manager, const char *name)
    : RESOURCEMASTER(manager, name)
{
    for (g_i = 0; g_i < ACTORRESOURCES_MAX; g_i++)
        m_resources[g_i] = "";
    for (g_i = 0; g_i < ACTORRESOURCES_MAX; g_i++)
        m_pos[g_i] = V3(0.0f, 0.0f, 0.0f);
    for (g_i = 0; g_i < ACTORRESOURCES_MAX; g_i++)
        m_ang[g_i] = ANG(0.0f, 0.0f, 0.0f);
    for (g_i = 0; g_i < ACTORRESOURCES_MAX; g_i++)
        m_scale[g_i] = V3(1.0f, 1.0f, 1.0f);
    for (g_i = 0; g_i < ACTORRESOURCES_MAX; g_i++)
        m_opacity[g_i] = 1.0f;
    for (g_i = 0; g_i < ACTORRESOURCES_MAX; g_i++)
        m_zsort[g_i] = 0;
    for (g_i = 0; g_i < ACTORRESOURCES_MAX; g_i++)
        m_specific[g_i] = 0;
    for (g_i = 0; g_i < ACTORRESOURCES_MAX; g_i++)
        m_boneAttach[g_i] = "";
}

int ACTORRESOURCESMASTER::GetClassId()
{
    return s_classId;
}

int ACTORRESOURCESMASTER::GetParentClassId()
{
    return g_classParents[s_classId];
}

const char *ACTORRESOURCESMASTER::GetClassName()
{
    return g_classNames[s_classId];
}
