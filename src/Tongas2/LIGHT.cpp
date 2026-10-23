#include "Tongas2/LIGHT.h"
#include "Tongas2/LOG.h"

static const char s_lightTypes[][30] = {
    "AMBIENT",
    "DIRECTIONAL",
    "POINT",
    "",
};

static RESOURCEMANAGER *CreateLIGHT();

CREATEMANAGER g_pfnCreateLIGHT = CreateLIGHT;

CMDENTRY LIGHTMASTER::s_commands[] = {
    { 0, 0, 0, 0, 0 },
};

PROPENTRY LIGHTMASTER::s_properties[] = {
    { "Color", 0, 13, 0, PROP_FIELD(LIGHTMASTER, m_color), false, 1, PROP_SETTER(LIGHTMASTER, SetColor) },
    { "Specular", 0, 13, 0, PROP_FIELD(LIGHTMASTER, m_specular), false, 1, PROP_SETTER(LIGHTMASTER, SetSpecular) },
    { "Ambient", 0, 13, 0, PROP_FIELD(LIGHTMASTER, m_ambient), false, 1, PROP_SETTER(LIGHTMASTER, SetAmbient) },
    { "Type", 0, 1, (int)s_lightTypes, PROP_FIELD(LIGHTMASTER, m_type), false, 1, PROP_SETTER(LIGHTMASTER, SetType) },
    { "LightMask", 0, 0, 0, PROP_FIELD(LIGHTMASTER, m_lightMask), false, 1, PROP_SETTER(LIGHTMASTER, SetLightMask) },
    { "Range", 0, 3, 0, PROP_FIELD(LIGHTMASTER, m_range), false, 1, PROP_SETTER(LIGHTMASTER, SetRange) },
    { "Attenuation0", 0, 3, 0, PROP_FIELD(LIGHTMASTER, m_attenuation0), false, 1, PROP_SETTER(LIGHTMASTER, SetAttenuation0) },
    { "Attenuation1", 0, 3, 0, PROP_FIELD(LIGHTMASTER, m_attenuation1), false, 1, PROP_SETTER(LIGHTMASTER, SetAttenuation1) },
    { "Attenuation2", 0, 3, 0, PROP_FIELD(LIGHTMASTER, m_attenuation2), false, 1, PROP_SETTER(LIGHTMASTER, SetAttenuation2) },
    { 0, 0, 15, 0, 0, false },
};

void LIGHT::SetColor(const LIGHTCOLOR &color)
{
    m_color = color;
}

void LIGHT::SetSpecular(const LIGHTCOLOR &color)
{
    m_specular = color;
}

static RESOURCEMANAGER *CreateLIGHT()
{
    if (g_pLog)
        g_pLog->Error("No default plugin for LIGHTMANAGER");
    return 0;
}

void LIGHTMASTER::RegisterCommands()
{
    g_classParents[s_classId] = RESOURCEMASTER::s_classId;
    g_classCommands[s_classId] = s_commands;
    HashCommands(s_commands);
    RegisterClassName(s_commands, "LIGHTMASTER", "RESOURCEMASTER");
}

void LIGHT::Init()
{
    OBJECT::Init();
    SetColor(((LIGHTMASTER *)m_master)->m_color);
    m_baseColor = m_color;
    SetSpecular(((LIGHTMASTER *)m_master)->m_specular);
    m_baseSpecular = m_specular;
}

LIGHTMASTER::LIGHTMASTER(RESOURCEMANAGER *manager, const char *name)
    : RESOURCEMASTER(manager, name)
{
    m_ambient.r = m_specular.r = m_color.r = 0.0f;
    m_ambient.g = m_specular.g = m_color.g = 0.0f;
    m_ambient.b = m_specular.b = m_color.b = 0.0f;
    m_type = 0;
    m_attenuation0 = 0.0f;
    m_attenuation1 = 0.0f;
    m_attenuation2 = 0.0f;
    m_lightMask = 1;
    m_range = 100.0f;
}

int LIGHTMASTER::GetClassId()
{
    return s_classId;
}

int LIGHTMASTER::GetParentClassId()
{
    return g_classParents[s_classId];
}

const char *LIGHTMASTER::GetClassName()
{
    return g_classNames[s_classId];
}

void LIGHTMASTER::SetColor(const LIGHTCOLOR &color)
{
    m_color = color;
}

void LIGHTMASTER::SetAmbient(const LIGHTCOLOR &color)
{
    m_ambient = color;
}

void LIGHTMANAGER::Init()
{
    RESOURCEMANAGER::Init();
    for (int i = 0; i < RESOURCES_MAX_GROUPS; i++)
        m_counts[i] = -1;
}

void LIGHTMANAGER::AddColors(const LIGHTRGB &a, const LIGHTRGB &b, int unused1, int unused2, LIGHTRGB &out)
{
    out.r = a.r + b.r;
    out.g = a.g + b.g;
    out.b = a.b + b.b;
    if (out.r > 255)
        out.r = 255;
    if (out.g > 255)
        out.g = 255;
    if (out.b > 255)
        out.b = 255;
}

int LIGHTMANAGER::GetMaxCount(int mask)
{
    int max = -1;
    int bit = 1;
    for (int i = 0; i < RESOURCES_MAX_GROUPS; i++, bit <<= 1) {
        if ((mask & bit) && m_counts[i] > max)
            max = m_counts[i];
    }
    return max;
}

void LIGHTMANAGER::Update()
{
    RESOURCEMANAGER::Update();
    UpdateLights();
}

void LIGHT::AddCounts()
{
    int mask = ((LIGHTMASTER *)m_master)->m_lightMask;
    LIGHTMANAGER *manager = (LIGHTMANAGER *)g_pLIGHT;
    int bit = 1;
    for (int i = 0; i < RESOURCES_MAX_GROUPS; i++, bit <<= 1) {
        if (mask & bit)
            manager->m_counts[i]++;
    }
}

LIGHT::LIGHT(LIGHTMASTER *master, const RESHANDLE &handle)
    : OBJECT(master, handle)
{
    m_color = master->m_color;
    m_baseColor = master->m_color;
    m_specular = master->m_specular;
    m_baseSpecular = master->m_specular;
}

RESOURCE *LIGHTMASTER::CreateCopy(const RESHANDLE &handle)
{
    return new LIGHT(this, handle);
}

SRESULT LIGHTMASTER::Execute(CMDENTRY *command, TOKENS **args)
{
    return (this->*(SRESULT (LIGHTMASTER::*)(TOKENS **))command->func)(args);
}
