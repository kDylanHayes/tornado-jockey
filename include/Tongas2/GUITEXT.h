// GUI text: GUITEXTMASTER, from GUITEXT.txt, lists up to 32 lines of text a
// GUI element shows, with their fonts, placement, alignment, colors and the
// script commands run when they're clicked or change. GUITEXTEDIT builds on
// it. Placeholder names apart from the class name and properties.
#ifndef TONGAS2_GUITEXT_H
#define TONGAS2_GUITEXT_H

#include "Tongas2/CSTRING.h"
#include "Tongas2/CWSTRING.h"
#include "Tongas2/RESOURCE.h"
#include "Tongas2/RESOURCES.h"
#include "Tongas2/SRESULT.h"

#define GUITEXT_MAX 32

// A 2D position or size that starts at zero.
struct GUITEXTV2 : V2
{
    GUITEXTV2() : V2(0.0f, 0.0f) {}
    GUITEXTV2 &operator=(const V2 &v) { V2::operator=(v); return *this; }
};

// A 0-255 color that starts black.
struct GUITEXTCOLOR
{
    GUITEXTCOLOR() { r = g = b = 0; }
    GUITEXTCOLOR(int r_, int g_, int b_) : r(r_), g(g_), b(b_) {}

    int r, g, b;
};

class GUITEXTMASTER : public RESOURCEMASTER
{
public:
    GUITEXTMASTER(RESOURCEMANAGER *manager, const char *name);
    virtual ~GUITEXTMASTER() {}

    static void RegisterCommands();

    virtual int GetClassId();
    virtual int GetParentClassId();
    virtual const char *GetClassName();
    virtual SRESULT Execute(CMDENTRY *command, TOKENS **args)
    {
        return (this->*(SRESULT (GUITEXTMASTER::*)(TOKENS **))command->func)(args);
    }

    virtual void SetBullet(const CWSTRING &bullet) { m_bullet = bullet; }
    virtual void SetParagraphSpace(const float &space) { m_paragraphSpace = space; }
    virtual void SetPassMouseOverToFather(const bool &pass) { m_passMouseOverToFather = pass; }
    virtual void SetCheckMouseMove(const bool &check) { m_checkMouseMove = check; }
    virtual void SetCheckMouseClick(const bool &check) { m_checkMouseClick = check; }
    virtual void SetText(const int &i, const CWSTRING &text) { m_text[i] = text; }
    virtual void SetFont(const int &i, const CSTRING &font);
    virtual void SetPos(const int &i, const V2 &pos) { m_pos[i] = pos; }
    virtual void SetSize(const int &i, const V2 &size) { m_size[i] = size; }
    virtual void SetAlignment(const int &i, const int &alignment) { m_alignment[i] = alignment; }
    virtual void SetAlignmentV(const int &i, const int &alignment) { m_alignmentV[i] = alignment; }
    virtual void SetLineSpacing(const int &i, const float &spacing) { m_lineSpacing[i] = spacing; }
    virtual void SetBindVar(const int &i, const CSTRING &var) { m_bindVar[i] = var; }
    virtual void SetOnChange(const int &i, const CSTRING &command) { m_onChange[i] = command; }
    virtual void SetHorizontalLimit(const int &i, const float &limit) { m_horizontalLimit[i] = limit; }
    virtual void SetOnClick(const int &i, const CSTRING &command) { m_onClick[i] = command; }
    virtual void SetOnMouseOver(const int &i, const CSTRING &command) { m_onMouseOver[i] = command; }
    virtual void SetOnMouseOut(const int &i, const CSTRING &command) { m_onMouseOut[i] = command; }
    virtual void SetOpacity(const int &i, const float &opacity) { m_opacity[i] = opacity; }
    virtual void SetColor(const int &i, const GUITEXTCOLOR &color) { m_color[i] = color; }

    static int s_classId;
    static CMDENTRY s_commands[];
    static PROPENTRY s_properties[];

    int m_60;
    CWSTRING m_bullet;
    float m_paragraphSpace;
    bool m_passMouseOverToFather;
    bool m_checkMouseMove;
    bool m_checkMouseClick;
    CWSTRING m_text[GUITEXT_MAX];
    CSTRING m_font[GUITEXT_MAX];
    GUITEXTV2 m_pos[GUITEXT_MAX];
    GUITEXTV2 m_size[GUITEXT_MAX];
    int m_alignment[GUITEXT_MAX];
    int m_alignmentV[GUITEXT_MAX];
    float m_lineSpacing[GUITEXT_MAX];
    CSTRING m_bindVar[GUITEXT_MAX];
    CSTRING m_onChange[GUITEXT_MAX];
    float m_horizontalLimit[GUITEXT_MAX];
    CSTRING m_onClick[GUITEXT_MAX];
    CSTRING m_onMouseOver[GUITEXT_MAX];
    CSTRING m_onMouseOut[GUITEXT_MAX];
    float m_opacity[GUITEXT_MAX];
    GUITEXTCOLOR m_color[GUITEXT_MAX];
};

#endif
