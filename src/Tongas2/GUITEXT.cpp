// This file was built seeing SRESULT::FreeValue as one that can throw.
#define SRESULT_FREEVALUE_MAY_THROW

#include "Tongas2/GUITEXT.h"
#include "Tongas2/PATH.h"

static const char s_alignments[][30] = {
    "LEFT",
    "CENTER",
    "RIGHT",
    "",
};

static const char s_alignmentsV[][30] = {
    "TOP",
    "CENTER",
    "BOTTOM",
    "",
};

class GUITEXTMANAGER : public RESOURCEMANAGER
{
public:
    GUITEXTMANAGER()
    {
        SafeSprintf(m_fileName, "GUITEXT.txt");
        m_8 = false;
        m_18 = true;
    }

    virtual RESOURCEMASTER *CreateMaster(const char *name) { return new GUITEXTMASTER(this, name); }
    virtual const char *GetTypeCode();
};

static RESOURCEMANAGER *CreateGUITEXT();

CREATEMANAGER g_pfnCreateGUITEXT = CreateGUITEXT;

CMDENTRY GUITEXTMASTER::s_commands[] = {
    { 0, 0, 0, 0, 0 },
};

PROPENTRY GUITEXTMASTER::s_properties[] = {
    { "Bullet", 0, 4, 0, PROP_FIELD(GUITEXTMASTER, m_bullet), false, 1, PROP_SETTER(GUITEXTMASTER, SetBullet) },
    { "ParagraphSpace", 0, 3, 0, PROP_FIELD(GUITEXTMASTER, m_paragraphSpace), false, 1, PROP_SETTER(GUITEXTMASTER, SetParagraphSpace) },
    { "PassMouseOverToFather", 0, 2, 0, PROP_FIELD(GUITEXTMASTER, m_passMouseOverToFather), false, 1, PROP_SETTER(GUITEXTMASTER, SetPassMouseOverToFather) },
    { "CheckMouseMove", 0, 2, 0, PROP_FIELD(GUITEXTMASTER, m_checkMouseMove), false, 1, PROP_SETTER(GUITEXTMASTER, SetCheckMouseMove) },
    { "CheckMouseClick", 0, 2, 0, PROP_FIELD(GUITEXTMASTER, m_checkMouseClick), false, 1, PROP_SETTER(GUITEXTMASTER, SetCheckMouseClick) },
    { "", 0, 15, 0, 0, false, GUITEXT_MAX, 0 },
    { "Text", 0, 4, 0, PROP_FIELD(GUITEXTMASTER, m_text), false, GUITEXT_MAX, PROP_SETTER(GUITEXTMASTER, SetText) },
    { "Font", 0, 7, 0, PROP_FIELD(GUITEXTMASTER, m_font), false, GUITEXT_MAX, PROP_SETTER(GUITEXTMASTER, SetFont) },
    { "Pos", 0, 10, 0, PROP_FIELD(GUITEXTMASTER, m_pos), false, GUITEXT_MAX, PROP_SETTER(GUITEXTMASTER, SetPos) },
    { "Size", 0, 10, 0, PROP_FIELD(GUITEXTMASTER, m_size), false, GUITEXT_MAX, PROP_SETTER(GUITEXTMASTER, SetSize) },
    { "Alignment", 0, 1, (int)s_alignments, PROP_FIELD(GUITEXTMASTER, m_alignment), false, GUITEXT_MAX, PROP_SETTER(GUITEXTMASTER, SetAlignment) },
    { "AlignmentV", 0, 1, (int)s_alignmentsV, PROP_FIELD(GUITEXTMASTER, m_alignmentV), false, GUITEXT_MAX, PROP_SETTER(GUITEXTMASTER, SetAlignmentV) },
    { "LineSpacing", 0, 3, 0, PROP_FIELD(GUITEXTMASTER, m_lineSpacing), false, GUITEXT_MAX, PROP_SETTER(GUITEXTMASTER, SetLineSpacing) },
    { "BindVar", 0, 7, 0, PROP_FIELD(GUITEXTMASTER, m_bindVar), false, GUITEXT_MAX, PROP_SETTER(GUITEXTMASTER, SetBindVar) },
    { "OnChange", 0, 7, 0, PROP_FIELD(GUITEXTMASTER, m_onChange), false, GUITEXT_MAX, PROP_SETTER(GUITEXTMASTER, SetOnChange) },
    { "HorizontalLimit", 0, 3, 0, PROP_FIELD(GUITEXTMASTER, m_horizontalLimit), false, GUITEXT_MAX, PROP_SETTER(GUITEXTMASTER, SetHorizontalLimit) },
    { "OnClick", 0, 7, 0, PROP_FIELD(GUITEXTMASTER, m_onClick), false, GUITEXT_MAX, PROP_SETTER(GUITEXTMASTER, SetOnClick) },
    { "OnMouseOver", 0, 7, 0, PROP_FIELD(GUITEXTMASTER, m_onMouseOver), false, GUITEXT_MAX, PROP_SETTER(GUITEXTMASTER, SetOnMouseOver) },
    { "OnMouseOut", 0, 7, 0, PROP_FIELD(GUITEXTMASTER, m_onMouseOut), false, GUITEXT_MAX, PROP_SETTER(GUITEXTMASTER, SetOnMouseOut) },
    { "Opacity", 0, 3, 0, PROP_FIELD(GUITEXTMASTER, m_opacity), false, GUITEXT_MAX, PROP_SETTER(GUITEXTMASTER, SetOpacity) },
    { "Color", 0, 13, 0, PROP_FIELD(GUITEXTMASTER, m_color), false, GUITEXT_MAX, PROP_SETTER(GUITEXTMASTER, SetColor) },
    { 0, 0, 15, 0, 0, false },
};

void GUITEXTMASTER::RegisterCommands()
{
    g_classParents[s_classId] = RESOURCEMASTER::s_classId;
    g_classCommands[s_classId] = s_commands;
    HashCommands(s_commands);
    RegisterClassName(s_commands, "GUITEXTMASTER", "RESOURCEMASTER");
}

const char *GUITEXTMANAGER::GetTypeCode()
{
    return "GTXT";
}

static RESOURCEMANAGER *CreateGUITEXT()
{
    return new GUITEXTMANAGER;
}

GUITEXTMASTER::GUITEXTMASTER(RESOURCEMANAGER *manager, const char *name)
    : RESOURCEMASTER(manager, name)
{
    m_bullet = L"";
    m_paragraphSpace = -1.0f;
    m_passMouseOverToFather = false;
    m_checkMouseMove = false;
    m_checkMouseClick = false;
    for (g_i = 0; g_i < GUITEXT_MAX; g_i++)
        m_text[g_i] = L"";
    for (g_i = 0; g_i < GUITEXT_MAX; g_i++)
        m_font[g_i] = "";
    for (g_i = 0; g_i < GUITEXT_MAX; g_i++)
        m_pos[g_i] = V2(0.0f, 0.0f);
    for (g_i = 0; g_i < GUITEXT_MAX; g_i++)
        m_size[g_i] = V2(0.05f, 0.05f);
    for (g_i = 0; g_i < GUITEXT_MAX; g_i++)
        m_alignment[g_i] = 1;
    for (g_i = 0; g_i < GUITEXT_MAX; g_i++)
        m_alignmentV[g_i] = 1;
    for (g_i = 0; g_i < GUITEXT_MAX; g_i++)
        m_lineSpacing[g_i] = 1.0f;
    for (g_i = 0; g_i < GUITEXT_MAX; g_i++)
        m_bindVar[g_i] = "";
    for (g_i = 0; g_i < GUITEXT_MAX; g_i++)
        m_onChange[g_i] = "";
    for (g_i = 0; g_i < GUITEXT_MAX; g_i++)
        m_horizontalLimit[g_i] = -1.0f;
    for (g_i = 0; g_i < GUITEXT_MAX; g_i++)
        m_onClick[g_i] = "";
    for (g_i = 0; g_i < GUITEXT_MAX; g_i++)
        m_onMouseOver[g_i] = "";
    for (g_i = 0; g_i < GUITEXT_MAX; g_i++)
        m_onMouseOut[g_i] = "";
    for (g_i = 0; g_i < GUITEXT_MAX; g_i++)
        m_opacity[g_i] = 1.0f;
    for (g_i = 0; g_i < GUITEXT_MAX; g_i++)
        m_color[g_i] = GUITEXTCOLOR(255, 255, 255);
    m_60 = 0;
}

int GUITEXTMASTER::GetClassId()
{
    return s_classId;
}

int GUITEXTMASTER::GetParentClassId()
{
    return g_classParents[s_classId];
}

const char *GUITEXTMASTER::GetClassName()
{
    return g_classNames[s_classId];
}

void GUITEXTMASTER::SetFont(const int &i, const CSTRING &font)
{
    m_font[i] = font;
}
