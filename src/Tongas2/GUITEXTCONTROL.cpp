// GUITEXT, the GUI's text: up to 32 TEXT copies placed with the control,
// one under another or each at its own position, showing the template's
// text formatted with a bound variable's value, with click, mouse over and
// mouse out commands. The original file name is not known; the masters are
// in GUITEXT.cpp.
#include "Tongas2/GUITEXT.h"
#include "Tongas2/CMDMGR.h"
#include "Tongas2/GUI.h"
#include "Tongas2/GUIGROUP.h"
#include "Tongas2/LOG.h"
#include "Tongas2/PATH.h"
#include "Tongas2/SAFEINDEX.h"
#include "Tongas2/STRING.h"
#include "Tongas2/TEXT.h"
#include "Tongas2/TOKENS.h"

#include <stdlib.h>
#include <string.h>

#define CMD_FUNC(CLASS, method) (CMDFUNC)(SRESULT (CLASS::*)(TOKENS **))(SRESULT (CLASS::*)(CMDCALL *))&CLASS::method

// Defined in RESOURCES.cpp.
extern RESOURCEMANAGER *g_pTEXT;
// Defined in KERNELCOMMANDS.cpp.
bool operator==(const SRESULT &a, const SRESULT &b);

CMDENTRY GUITEXT::s_commands[] = {
    { "click", 0, 1, 1, CMD_FUNC(GUITEXT, ClickCommand) },
    { "MouseOver", 0, 1, 1, CMD_FUNC(GUITEXT, MouseOverCommand) },
    { "MouseOut", 0, 1, 1, CMD_FUNC(GUITEXT, MouseOutCommand) },
    { 0, 0, 0, 0, 0 },
};

PROPENTRY GUITEXT::s_properties[] = {
    { "Text", 0, 6, 0, PROP_FIELD(GUITEXT, m_texts), false, GUITEXT_MAX, PROP_SETTER(GUITEXT, SetText) },
    { "SlotOpacity", 0, 3, 0, PROP_FIELD(GUITEXT, m_slotOpacity), false, GUITEXT_MAX, PROP_SETTER(GUITEXT, SetSlotOpacity) },
    { 0, 0, 15, 0, 0, false },
};

void GUITEXT::RegisterCommands()
{
    g_classParents[s_classId] = GUICONTROL::s_classId;
    g_classCommands[s_classId] = s_commands;
    HashCommands(s_commands);
    RegisterClassName(s_commands, "GUITEXT", "GUICONTROL");
}

GUITEXT::~GUITEXT()
{
    for (int i = 0; i < GUITEXT_MAX; i++) {
        RESOURCE *copy = g_pResources->GetResource(m_copies[i]);
        if (copy)
            copy->Virtual20();
        m_copies[i].Invalidate();
    }
}

void GUITEXT::SetPosition(const V2 &position)
{
    m_position.x = position.x;
    m_position.y = position.y;
    if (m_paragraphSpace >= 0.0f) {
        V2 pos(position.x + m_textMaster->m_pos[0].x, m_textMaster->m_pos[0].y + position.y);
        for (int i = 0; i < GUITEXT_MAX; i++) {
            TEXT *copy = (TEXT *)g_pResources->GetResource(m_copies[i]);
            if (copy) {
                copy->SetScreenPosition(pos.x, pos.y);
                copy->m_onScreen = true;
                copy->Update();
                pos.y = copy->m_bounds.maxY;
                if (m_paragraphSpace >= 0.0f)
                    pos.y += m_size.y * m_paragraphSpace;
                m_min[i].x = copy->m_bounds.minX;
                m_min[i].y = copy->m_bounds.minY;
                m_max[i].x = copy->m_bounds.maxX;
                m_max[i].y = copy->m_bounds.maxY;
            } else {
                m_min[i].y = 0.0f;
                m_min[i].x = 0.0f;
                m_max[i].x = 0.0f;
                m_max[i].y = 0.0f;
            }
        }
        return;
    }
    for (int i = 0; i < GUITEXT_MAX; i++) {
        TEXT *copy = (TEXT *)g_pResources->GetResource(m_copies[i]);
        if (copy) {
            (V2 &)copy->m_screenPosition = m_textMaster->m_pos[i] + position;
            copy->m_onScreen = true;
            copy->Update();
            m_min[i].x = copy->m_bounds.minX;
            m_min[i].y = copy->m_bounds.minY;
            m_max[i].x = copy->m_bounds.maxX;
            m_max[i].y = copy->m_bounds.maxY;
        } else {
            m_min[i].y = 0.0f;
            m_min[i].x = 0.0f;
            m_max[i].x = 0.0f;
            m_max[i].y = 0.0f;
        }
    }
}

void GUITEXT::SetSize(const V2 &size)
{
    m_size = size;
    for (int i = 0; i < GUITEXT_MAX; i++) {
        TEXT *copy = (TEXT *)g_pResources->GetResource(m_copies[i]);
        if (copy)
            copy->SetSize(size * m_textMaster->m_size[i]);
    }
}

void GUITEXT::SetZSort(const int &zsort)
{
    m_zsort = zsort;
    for (int i = 0; i < GUITEXT_MAX; i++) {
        OBJECT *copy = (OBJECT *)g_pResources->GetResource(m_copies[i]);
        if (copy)
            copy->SetLayer(zsort + 1);
    }
}

void GUITEXT::SetOpacity(const float &opacity)
{
    m_opacity = opacity;
    for (int i = 0; i < GUITEXT_MAX; i++) {
        OBJECT *copy = (OBJECT *)g_pResources->GetResource(m_copies[i]);
        if (copy)
            copy->SetAlpha(((m_enabled + 1.0f) * 0.5f) * m_visible * m_slotOpacity[i] * opacity);
    }
}

bool GUITEXT::HitTest(float x, float y, int i)
{
    if (m_max[i].x - m_min[i].x <= 0.0f || m_max[i].y - m_min[i].y <= 0.0f)
        return false;
    if (x >= m_min[i].x && x <= m_max[i].x && y >= m_min[i].y && y <= m_max[i].y)
        return true;
    return false;
}

bool GUITEXT::OnLeftDown(MOUSESTATE state)
{
    for (int i = 0; i < GUITEXT_MAX; i++) {
        if (HitTest(state.x, state.y, i) && m_textMaster->m_onClick[i].Length() > 0)
            return true;
    }
    return false;
}

void GUITEXT::FindTopAt(V2 point, GUICONTROL **hit, int *zsort)
{
    if (m_topZSort < *zsort)
        return;
    for (int i = 0; i < GUITEXT_MAX; i++) {
        if (HitTest(point.x, point.y, i)) {
            *zsort = m_topZSort;
            *hit = this;
            return;
        }
    }
}

void GUITEXT::SetText(const int &i, const FIXEDWSTRING<0x200> &text)
{
    m_texts[i] = text;
    TEXT *copy = (TEXT *)g_pResources->GetResource(m_copies[i]);
    if (copy) {
        if (m_textMaster)
            copy->SetWrapWidth(m_textMaster->m_horizontalLimit[i]);
        copy->SetText(text.m_buf);
        SetSize(m_size);
        SetPosition(m_position);
    }
}

FIXEDSTRING<0x200> GUITEXT::GetBindVar(int i)
{
    return FIXEDSTRING<0x200>(m_textMaster->m_bindVar[i].c_str());
}

void GUITEXT::UpdateText(int i, bool force)
{
    SRESULT result;
    FIXEDSTRING<0x200> var = GetBindVar(i);
    if ((int)strlen(var) > 0) {
        if (!strchr(var, '.'))
            result.Assign(g_pGui->Run(STRING(var)));
        if (!result.IsSet()) {
            STRING expression(var);
            result.Assign(g_pCmdMgr->Evaluate(expression, m_handle));
        }
    }
    SRESULT &last = m_values[i];
    if (result == last && !force)
        return;
    last.Assign(result);
    FIXEDWSTRING<0x200> text;
    switch (result.GetType()) {
    case SRT_NOTSET:
        SafeSwprintf(text.m_buf, m_textMaster->m_text[i].c_str());
        break;
    case SRT_ANG:
        SafeSwprintf(text.m_buf, m_textMaster->m_text[i].c_str(), result.GetANG(ANG(0.0f, 0.0f, 0.0f)).GetEulerX(),
                     result.GetANG(ANG(0.0f, 0.0f, 0.0f)).GetEulerY(), result.GetANG(ANG(0.0f, 0.0f, 0.0f)).GetEulerZ());
        break;
    case SRT_BOOL:
        SafeSwprintf(text.m_buf, m_textMaster->m_text[i].c_str(), result.GetBool(false));
        break;
    case SRT_CHAR:
        SafeSwprintf(text.m_buf, m_textMaster->m_text[i].c_str(), result.GetChar(0));
        break;
    case SRT_FLOAT:
        SafeSwprintf(text.m_buf, m_textMaster->m_text[i].c_str(), result.GetFloat(0.0f));
        break;
    case SRT_INT:
        SafeSwprintf(text.m_buf, m_textMaster->m_text[i].c_str(), result.GetInt(0));
        break;
    case SRT_RGB:
        SafeSwprintf(text.m_buf, m_textMaster->m_text[i].c_str(), result.GetRGB(RGB()));
        break;
    case SRT_STRING:
        SafeSwprintf(text.m_buf, m_textMaster->m_text[i].c_str(), result.GetString(""));
        break;
    case SRT_UNICODESTRING:
        SafeSwprintf(text.m_buf, m_textMaster->m_text[i].c_str(), result.GetUnicodeString(L""));
        break;
    case SRT_TEXTURE:
        SafeSwprintf(text.m_buf, m_textMaster->m_text[i].c_str(), result.GetTexture(SRTEXTURE()).name.m_buf,
                     result.GetTexture(SRTEXTURE()).index);
        break;
    case SRT_V2:
        SafeSwprintf(text.m_buf, m_textMaster->m_text[i].c_str(), result.GetV2(V2()).x, result.GetV2(V2()).y);
        break;
    case SRT_V3:
        SafeSwprintf(text.m_buf, m_textMaster->m_text[i].c_str(), result.GetV3(V3()).x, result.GetV3(V3()).y,
                     result.GetV3(V3()).z);
        break;
    default:
        if (g_pLog)
            g_pLog->Error("VAR TYPE NOT RECONIZED");
        break;
    }
    {
        STRING command(m_textMaster->m_onChange[i].c_str());
        g_pCmdMgr->Run(command, m_handle);
    }
    SetText(i + 0, FIXEDWSTRING<0x200>(text.m_buf));
}

void GUITEXT::Update()
{
    GUICONTROL::Update();
    for (int i = 0; i < GUITEXT_MAX; i++) {
        TEXT *copy = (TEXT *)g_pResources->GetResource(m_copies[i]);
        if (copy) {
            copy->SetWrapWidth(m_textMaster->m_horizontalLimit[i]);
            UpdateText(i, false);
            m_min[i].x = copy->m_bounds.minX;
            m_min[i].y = copy->m_bounds.minY;
            m_max[i].x = copy->m_bounds.maxX;
            m_max[i].y = copy->m_bounds.maxY;
        } else {
            m_min[i].y = 0.0f;
            m_min[i].x = 0.0f;
            m_max[i].x = 0.0f;
            m_max[i].y = 0.0f;
        }
    }
}

void GUITEXT::Click(int i)
{
    if (IsEnabled() && IsVisible()) {
        g_pGui->PlayClick();
        STRING command(m_textMaster->m_onClick[i].c_str());
        g_pCmdMgr->Run(command, m_handle);
    }
}

SRESULT GUITEXT::MouseOverCommand(CMDCALL *call)
{
    int i = atoi(call->tokens->GetToken(1));
    if (!m_mouseOver[i]) {
        g_pGui->PlayMouseOver();
        {
            STRING command(m_textMaster->m_onMouseOver[i].c_str());
            g_pCmdMgr->Run(command, m_handle);
        }
        m_mouseOver[i] = true;
    }
    return SRESULT(true);
}

SRESULT GUITEXT::MouseOutCommand(CMDCALL *call)
{
    int i = atoi(call->tokens->GetToken(1));
    if (m_mouseOver[i]) {
        {
            STRING command(m_textMaster->m_onMouseOut[i].c_str());
            g_pCmdMgr->Run(command, m_handle);
        }
        m_mouseOver[i] = false;
    }
    return SRESULT(true);
}

bool GUITEXT::OnMouseMove(MOUSESTATE state)
{
    if (!IsEnabled() && IsVisible())
        return false;
    if (!m_textMaster->m_checkMouseMove)
        return false;
    for (int i = 0; i < GUITEXT_MAX; i++) {
        bool over = HitTest(state.x, state.y, i);
        if (over) {
            if (!m_mouseOver[i]) {
                g_pGui->PlayMouseOver();
                STRING command(m_textMaster->m_onMouseOver[i].c_str());
                g_pCmdMgr->Run(command, m_handle);
            }
        } else if (m_mouseOver[i]) {
            STRING command(m_textMaster->m_onMouseOut[i].c_str());
            g_pCmdMgr->Run(command, m_handle);
        }
        m_mouseOver[i] = over;
    }
    return false;
}

GUITEXT::GUITEXT(const RESHANDLE &master, GUIGROUP *parent) : GUICONTROL(master, parent)
{
    m_textMaster = 0;
    for (g_i = 0; g_i < GUITEXT_MAX; g_i++)
        m_texts[g_i] = L"";
    for (g_i = 0; g_i < GUITEXT_MAX; g_i++)
        m_slotOpacity[g_i] = 1.0f;
    m_textMaster = (GUITEXTMASTER *)g_pResources->GetMaster(master);
    if (!m_textMaster && g_pLog)
        g_pLog->Error("GUI TEXT Control with no master");
    bool bullet = false;
    wchar_t bulletChar;
    if (m_textMaster && m_textMaster->m_bullet.Length() > 0) {
        bullet = true;
        bulletChar = m_textMaster->m_bullet.c_str()[0];
    }
    for (int i = 0; i < GUITEXT_MAX; i++) {
        if (m_textMaster->m_font[i].Length() > 0) {
            TEXT *copy = (TEXT *)g_pTEXT->GetResource(m_textMaster->m_font[i].c_str());
            if (copy) {
                m_copies[i] = copy->m_handle;
                copy->m_alignH = m_textMaster->m_alignment[i];
                copy->m_alignV = m_textMaster->m_alignmentV[i];
                copy->m_lineSpacing = m_textMaster->m_lineSpacing[i];
                copy->SetWrapWidth(m_textMaster->m_horizontalLimit[i]);
                (V2 &)copy->m_screenPosition = m_position;
                copy->m_onScreen = true;
                (GUITEXTCOLOR &)copy->m_color = m_textMaster->m_color[i];
                if (bullet)
                    copy->SetBullet(bulletChar);
                else
                    copy->ClearBullet();
                FIXEDWSTRING<0x200> text(m_textMaster->m_text[i].c_str());
                SetText(i + 0, text);
                copy->SetActive(true);
                m_paragraphSpace = m_textMaster->m_paragraphSpace;
                UpdateText(i, true);
            }
        }
    }
    SetPosition(m_position);
    SetSize(m_size);
    SetZSort(m_zsort);
    SetTopZSort(parent->m_topZSort + 1);
    for (int j = 0; j < GUITEXT_MAX; j++) {
        m_mouseOver[j] = false;
        m_slotOpacity[j] = m_textMaster->m_opacity[j];
        m_min[j].y = 0.0f;
        m_min[j].x = 0.0f;
        m_max[j].y = 0.0f;
        m_max[j].x = 0.0f;
    }
}

bool GUITEXT::OnLeftUp(MOUSESTATE state)
{
    if (!m_textMaster->m_checkMouseClick)
        return false;
    for (int i = 0; i < GUITEXT_MAX; i++) {
        if (HitTest(state.x, state.y, i) && m_textMaster->m_onClick[i].Length() > 0) {
            Click(i);
            return true;
        }
    }
    return false;
}

SRESULT GUITEXT::ClickCommand(CMDCALL *call)
{
    Click(atoi(call->tokens->GetToken(1)));
    return SRESULT(true);
}
