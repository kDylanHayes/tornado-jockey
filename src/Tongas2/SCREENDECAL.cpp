#include "Tongas2/SCREENDECAL.h"
#include "Tongas2/SAFEINDEX.h"
#include "Tongas2/SOUND.h"

static const char s_blends[][30] = {
    "BLEND",
    "REPLACE",
    "MULTIPLY",
    "ADD",
    "WEIGTEDADD",
    "",
};

class SCREENDECALMANAGER : public RESOURCEMANAGER
{
public:
    SCREENDECALMANAGER()
    {
        SafeSprintf(m_fileName, "SCREENDECAL.txt");
        m_8 = false;
        m_18 = true;
    }

    virtual RESOURCEMASTER *CreateMaster(const char *name) { return new SCREENDECALMASTER(this, name); }
    virtual const char *GetTypeCode();
};

void SCREENDECAL::SetPosition(V3 position)
{
    m_position = position;
    m_onScreen = false;
}

void SCREENDECAL::SetScreenPosition(float x, float y)
{
    m_screenPosition.x = x;
    m_screenPosition.y = y;
    m_onScreen = true;
}

V2 SCREENDECAL::GetScreenPosition()
{
    return m_screenPosition;
}

void SCREENDECAL::SetScreenSize(const V2 &size)
{
    m_screenSize = size;
}

static RESOURCEMANAGER *CreateSCREENDECAL();

CREATEMANAGER g_pfnCreateSCREENDECAL = CreateSCREENDECAL;

CMDENTRY SCREENDECALMASTER::s_commands[] = {
    { 0, 0, 0, 0, 0 },
};

PROPENTRY SCREENDECALMASTER::s_properties[] = {
    { "FileName", 0, 7, 0, PROP_FIELD(SCREENDECALMASTER, m_file), false, 1, PROP_SETTER(SCREENDECALMASTER, SetFileName) },
    { "ForceDuplicates", 0, 2, 0, PROP_FIELD(SCREENDECALMASTER, m_forceDuplicates), false, 1, PROP_SETTER(SCREENDECALMASTER, SetForceDuplicates) },
    { "ShifterSpeed", 0, 10, 0, PROP_FIELD(SCREENDECALMASTER, m_shifterSpeed), false, 1, PROP_SETTER(SCREENDECALMASTER, SetShifterSpeed) },
    { "Blend", 0, 1, (int)s_blends, PROP_FIELD(SCREENDECALMASTER, m_blend), false, 1, PROP_SETTER(SCREENDECALMASTER, SetBlend) },
    { "UserFolder", 0, 2, 0, PROP_FIELD(SCREENDECALMASTER, m_userFolder), false, 1, PROP_SETTER(SCREENDECALMASTER, SetUserFolder) },
    { "CorrectSizeInWidescreen", 0, 2, 0, PROP_FIELD(SCREENDECALMASTER, m_correctSizeInWidescreen), false, 1, PROP_SETTER(SCREENDECALMASTER, SetCorrectSizeInWidescreen) },
    { 0, 0, 15, 0, 0, false },
};

void SCREENDECALMASTER::RegisterCommands()
{
    g_classParents[s_classId] = RESOURCEMASTER::s_classId;
    g_classCommands[s_classId] = s_commands;
    HashCommands(s_commands);
    RegisterClassName(s_commands, "SCREENDECALMASTER", "RESOURCEMASTER");
}

const char *SCREENDECALMANAGER::GetTypeCode()
{
    return "SCDC";
}

static RESOURCEMANAGER *CreateSCREENDECAL()
{
    return new SCREENDECALMANAGER;
}

SCREENDECALMASTER::SCREENDECALMASTER(RESOURCEMANAGER *manager, const char *name)
    : RESOURCEMASTER(manager, name)
{
    m_file = "";
    m_forceDuplicates = false;
    m_shifterSpeed = SCREENDECALV2(0.0f, 0.0f);
    m_blend = 0;
    m_userFolder = false;
    m_correctSizeInWidescreen = true;
}

int SCREENDECALMASTER::GetClassId()
{
    return s_classId;
}

int SCREENDECALMASTER::GetParentClassId()
{
    return g_classParents[s_classId];
}

const char *SCREENDECALMASTER::GetClassName()
{
    return g_classNames[s_classId];
}

void SCREENDECALMASTER::SetShifterSpeed(const V2 &speed)
{
    m_shifterSpeed = speed;
}

SCREENDECAL::SCREENDECAL(SCREENDECALMASTER *master, const RESHANDLE &handle)
    : OBJECT(master, handle)
{
    m_400 = false;
    m_onScreen = true;
    m_screenPosition.x = 0.5f;
    m_screenPosition.y = 0.5f;
    m_screenSize.x = 1.0f;
    m_screenSize.y = 1.0f;
}

void SCREENDECALMASTER::Prepare()
{
    FIXEDSTRING<0x200> path;
    if (m_file.Length() <= 0)
        SafeSprintf(path, "%s%s", "data/art/", m_name.c_str());
    else
        SafeSprintf(path, "%s%s", "data/art/", m_file.c_str());
    ReplaceChar(path, '/', '\\');
    m_artPath = path;
}

RESOURCE *SCREENDECALMASTER::CreateCopy(const RESHANDLE &handle)
{
    return new SCREENDECAL(this, handle);
}
