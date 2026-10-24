// This file was built seeing SRESULT::FreeValue as one that can throw.
#define SRESULT_FREEVALUE_MAY_THROW

#include "Tongas2/GUIVISUAL.h"
#include "Tongas2/PATH.h"

static const char s_barGrowDirections[][30] = {
    "DOWN",
    "UP",
    "LEFT",
    "RIGHT",
    "",
};

static const char s_barBehaviors[][30] = {
    "GROW",
    "MOVE",
    "",
};

class GUIVISUALMANAGER : public RESOURCEMANAGER
{
public:
    GUIVISUALMANAGER()
    {
        SafeSprintf(m_fileName, "GUIVISUAL.txt");
        m_8 = false;
        m_18 = true;
    }

    virtual RESOURCEMASTER *CreateMaster(const char *name) { return new GUIVISUALMASTER(this, name); }
    virtual const char *GetTypeCode();
};

static RESOURCEMANAGER *CreateGUIVISUAL();

CREATEMANAGER g_pfnCreateGUIVISUAL = CreateGUIVISUAL;

CMDENTRY GUIVISUALMASTER::s_commands[] = {
    { 0, 0, 0, 0, 0 },
};

PROPENTRY GUIVISUALMASTER::s_properties[] = {
    { "", 0, 15, 0, 0, false, GUIVISUAL_MAX, 0 },
    { "Texture", 0, 14, 0, PROP_FIELD(GUIVISUALMASTER, m_textures), false, GUIVISUAL_MAX, PROP_SETTER(GUIVISUALMASTER, SetTexture) },
    { "ScreenDecal", 0, 7, 0, PROP_FIELD(GUIVISUALMASTER, m_screenDecals), false, GUIVISUAL_MAX, PROP_SETTER(GUIVISUALMASTER, SetScreenDecal) },
    { "Pos", 0, 10, 0, PROP_FIELD(GUIVISUALMASTER, m_pos), false, GUIVISUAL_MAX, PROP_SETTER(GUIVISUALMASTER, SetPos) },
    { "Rotation", 0, 3, 0, PROP_FIELD(GUIVISUALMASTER, m_rotation), false, GUIVISUAL_MAX, PROP_SETTER(GUIVISUALMASTER, SetRotation) },
    { "Size", 0, 10, 0, PROP_FIELD(GUIVISUALMASTER, m_size), false, GUIVISUAL_MAX, PROP_SETTER(GUIVISUALMASTER, SetSize) },
    { "Opacity", 0, 3, 0, PROP_FIELD(GUIVISUALMASTER, m_opacity), false, GUIVISUAL_MAX, PROP_SETTER(GUIVISUALMASTER, SetOpacity) },
    { "ZSort", 0, 0, 0, PROP_FIELD(GUIVISUALMASTER, m_zsort), false, GUIVISUAL_MAX, PROP_SETTER(GUIVISUALMASTER, SetZSort) },
    { "BindProgressBar", 0, 7, 0, PROP_FIELD(GUIVISUALMASTER, m_bindProgressBar), false, GUIVISUAL_MAX, PROP_SETTER(GUIVISUALMASTER, SetBindProgressBar) },
    { "BindProgressBarMax", 0, 3, 0, PROP_FIELD(GUIVISUALMASTER, m_bindProgressBarMax), false, GUIVISUAL_MAX, PROP_SETTER(GUIVISUALMASTER, SetBindProgressBarMax) },
    { "BarStartOffset", 0, 3, 0, PROP_FIELD(GUIVISUALMASTER, m_barStartOffset), false, GUIVISUAL_MAX, PROP_SETTER(GUIVISUALMASTER, SetBarStartOffset) },
    { "BarEndOffset", 0, 3, 0, PROP_FIELD(GUIVISUALMASTER, m_barEndOffset), false, GUIVISUAL_MAX, PROP_SETTER(GUIVISUALMASTER, SetBarEndOffset) },
    { "BarGrowDirection", 0, 1, s_barGrowDirections, PROP_FIELD(GUIVISUALMASTER, m_barGrowDirection), false, GUIVISUAL_MAX, PROP_SETTER(GUIVISUALMASTER, SetBarGrowDirection) },
    { "BarDiscrete", 0, 2, 0, PROP_FIELD(GUIVISUALMASTER, m_barDiscrete), false, GUIVISUAL_MAX, PROP_SETTER(GUIVISUALMASTER, SetBarDiscrete) },
    { "BarDiscreteSteps", 0, 0, 0, PROP_FIELD(GUIVISUALMASTER, m_barDiscreteSteps), false, GUIVISUAL_MAX, PROP_SETTER(GUIVISUALMASTER, SetBarDiscreteSteps) },
    { "BarGrowVel", 0, 3, 0, PROP_FIELD(GUIVISUALMASTER, m_barGrowVel), false, GUIVISUAL_MAX, PROP_SETTER(GUIVISUALMASTER, SetBarGrowVel) },
    { "BarBehavior", 0, 1, s_barBehaviors, PROP_FIELD(GUIVISUALMASTER, m_barBehavior), false, GUIVISUAL_MAX, PROP_SETTER(GUIVISUALMASTER, SetBarBehavior) },
    { "BarMoveMax", 0, 10, 0, PROP_FIELD(GUIVISUALMASTER, m_barMoveMax), false, GUIVISUAL_MAX, PROP_SETTER(GUIVISUALMASTER, SetBarMoveMax) },
    { "ReposOnScale", 0, 2, 0, PROP_FIELD(GUIVISUALMASTER, m_reposOnScale), false, 1, PROP_SETTER(GUIVISUALMASTER, SetReposOnScale) },
    { 0, 0, 15, 0, 0, false },
};

void GUIVISUALMASTER::RegisterCommands()
{
    g_classParents[s_classId] = RESOURCEMASTER::s_classId;
    g_classCommands[s_classId] = s_commands;
    HashCommands(s_commands);
    RegisterClassName(s_commands, "GUIVISUALMASTER", "RESOURCEMASTER");
}

const char *GUIVISUALMANAGER::GetTypeCode()
{
    return "GVIS";
}

static RESOURCEMANAGER *CreateGUIVISUAL()
{
    return new GUIVISUALMANAGER;
}

int GUIVISUALMASTER::GetClassId()
{
    return s_classId;
}

int GUIVISUALMASTER::GetParentClassId()
{
    return g_classParents[s_classId];
}

const char *GUIVISUALMASTER::GetClassName()
{
    return g_classNames[s_classId];
}

void GUIVISUALMASTER::SetPos(const int &i, const V2 &pos)
{
    m_pos[i] = pos;
}

void GUIVISUALMASTER::SetRotation(const int &i, const float &rotation)
{
    m_rotation[i] = rotation;
}

void GUIVISUALMASTER::SetSize(const int &i, const V2 &size)
{
    m_size[i] = size;
}

void GUIVISUALMASTER::SetOpacity(const int &i, const float &opacity)
{
    m_opacity[i] = opacity;
}

void GUIVISUALMASTER::SetZSort(const int &i, const int &zsort)
{
    m_zsort[i] = zsort;
}

void GUIVISUALMASTER::SetBindProgressBarMax(const int &i, const float &max)
{
    m_bindProgressBarMax[i] = max;
}

void GUIVISUALMASTER::SetBarStartOffset(const int &i, const float &offset)
{
    m_barStartOffset[i] = offset;
}

void GUIVISUALMASTER::SetBarEndOffset(const int &i, const float &offset)
{
    m_barEndOffset[i] = offset;
}

void GUIVISUALMASTER::SetBarGrowDirection(const int &i, const int &direction)
{
    m_barGrowDirection[i] = direction;
}

void GUIVISUALMASTER::SetBarDiscrete(const int &i, const bool &discrete)
{
    m_barDiscrete[i] = discrete;
}

void GUIVISUALMASTER::SetBarDiscreteSteps(const int &i, const int &steps)
{
    m_barDiscreteSteps[i] = steps;
}

void GUIVISUALMASTER::SetBarGrowVel(const int &i, const float &vel)
{
    m_barGrowVel[i] = vel;
}

void GUIVISUALMASTER::SetBarBehavior(const int &i, const int &behavior)
{
    m_barBehavior[i] = behavior;
}

void GUIVISUALMASTER::SetBarMoveMax(const int &i, const V2 &max)
{
    m_barMoveMax[i] = max;
}

void GUIVISUALMASTER::SetReposOnScale(const bool &repos)
{
    m_reposOnScale = repos;
}

GUIVISUALMASTER::GUIVISUALMASTER(RESOURCEMANAGER *manager, const char *name)
    : RESOURCEMASTER(manager, name)
{
    for (g_i = 0; g_i < GUIVISUAL_MAX; g_i++)
        m_textures[g_i] = SRTEXTURE();
    for (g_i = 0; g_i < GUIVISUAL_MAX; g_i++)
        m_screenDecals[g_i] = "";
    for (g_i = 0; g_i < GUIVISUAL_MAX; g_i++)
        m_pos[g_i] = V2(0.0f, 0.0f);
    for (g_i = 0; g_i < GUIVISUAL_MAX; g_i++)
        m_rotation[g_i] = 0.0f;
    for (g_i = 0; g_i < GUIVISUAL_MAX; g_i++)
        m_size[g_i] = V2(0.1f, 0.1f);
    for (g_i = 0; g_i < GUIVISUAL_MAX; g_i++)
        m_opacity[g_i] = 1.0f;
    for (g_i = 0; g_i < GUIVISUAL_MAX; g_i++)
        m_zsort[g_i] = 0;
    for (g_i = 0; g_i < GUIVISUAL_MAX; g_i++)
        m_bindProgressBar[g_i] = "";
    for (g_i = 0; g_i < GUIVISUAL_MAX; g_i++)
        m_bindProgressBarMax[g_i] = 1000.0f;
    for (g_i = 0; g_i < GUIVISUAL_MAX; g_i++)
        m_barStartOffset[g_i] = 0.0f;
    for (g_i = 0; g_i < GUIVISUAL_MAX; g_i++)
        m_barEndOffset[g_i] = 0.0f;
    for (g_i = 0; g_i < GUIVISUAL_MAX; g_i++)
        m_barGrowDirection[g_i] = 3;
    for (g_i = 0; g_i < GUIVISUAL_MAX; g_i++)
        m_barDiscrete[g_i] = false;
    for (g_i = 0; g_i < GUIVISUAL_MAX; g_i++)
        m_barDiscreteSteps[g_i] = 1;
    for (g_i = 0; g_i < GUIVISUAL_MAX; g_i++)
        m_barGrowVel[g_i] = 0.0f;
    for (g_i = 0; g_i < GUIVISUAL_MAX; g_i++)
        m_barBehavior[g_i] = 0;
    for (g_i = 0; g_i < GUIVISUAL_MAX; g_i++)
        m_barMoveMax[g_i] = V2(0.25f, 0.25f);
    m_60 = 0;
    m_reposOnScale = true;
}
