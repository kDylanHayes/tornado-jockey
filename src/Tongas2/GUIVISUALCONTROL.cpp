// GUIVISUAL, the GUI's pictures: up to 32 decals or screen decals placed,
// sized and faded with the control, some of them progress bars that grow,
// shrink or slide with a script value. The original file name is not known;
// the masters are in GUIVISUAL.cpp.
#include "Tongas2/GUIVISUAL.h"
#include "Tongas2/CMDMGR.h"
#include "Tongas2/DECAL.h"
#include "Tongas2/GUI.h"
#include "Tongas2/LOG.h"
#include "Tongas2/PATH.h"
#include "Tongas2/SCREENDECAL.h"
#include "Tongas2/STRING.h"
#include "Tongas2/TIMER.h"

#include <float.h>
#include <math.h>
#include <string.h>

#define CMD_FUNC(CLASS, method) (CMDFUNC)(SRESULT (CLASS::*)(TOKENS **))(SRESULT (CLASS::*)(CMDCALL *))&CLASS::method

// Defined in RESOURCES.cpp.
extern RESOURCEMANAGER *g_pSCREENDECAL;

CMDENTRY GUIVISUAL::s_commands[] = {
    { "UpdateBarsForced", 0, 0, 0, CMD_FUNC(GUIVISUAL, UpdateBarsForced) },
    { 0, 0, 0, 0, 0 },
};

PROPENTRY GUIVISUAL::s_properties[] = {
    { "Texture", 0, 14, 0, PROP_FIELD(GUIVISUAL, m_textures), false, GUIVISUAL_MAX, PROP_SETTER(GUIVISUAL, SetTexture) },
    { "ScreenDecal", 0, 9, 0, PROP_FIELD(GUIVISUAL, m_screenDecals), false, GUIVISUAL_MAX, PROP_SETTER(GUIVISUAL, SetScreenDecal) },
    { "DecalOrder", 0, 0, 0, PROP_FIELD(GUIVISUAL, m_decalOrder), false, GUIVISUAL_MAX, PROP_SETTER(GUIVISUAL, SetDecalOrder) },
    { "VisualOpacity", 0, 3, 0, PROP_FIELD(GUIVISUAL, m_visualOpacity), false, GUIVISUAL_MAX, PROP_SETTER(GUIVISUAL, SetVisualOpacity) },
    { 0, 0, 15, 0, 0, false },
};

void GUIVISUAL::RegisterCommands()
{
    g_classParents[s_classId] = GUICONTROL::s_classId;
    g_classCommands[s_classId] = s_commands;
    HashCommands(s_commands);
    RegisterClassName(s_commands, "GUIVISUAL", "GUICONTROL");
}

GUIVISUAL::~GUIVISUAL()
{
    for (int i = 0; i < GUIVISUAL_MAX; i++) {
        OBJECT *copy = (OBJECT *)g_pResources->GetResource(m_textureCopies[i]);
        if (copy)
            copy->Detach();
        m_textureCopies[i].Invalidate();
        copy = (OBJECT *)g_pResources->GetResource(m_decalCopies[i]);
        if (copy)
            copy->Detach();
        m_decalCopies[i].Invalidate();
    }
}

void GUIVISUAL::SetPosition(const V2 &position)
{
    m_position = position;
    for (int i = 0; i < GUIVISUAL_MAX; i++) {
        if (m_textureCopies[i].IsValid()) {
            DECAL *decal = (DECAL *)g_pResources->GetResource(m_textureCopies[i]);
            decal->m_screenPosition = m_visualMaster->m_pos[i] * m_size + position;
            decal->m_screenSpace = true;
        }
        if (m_decalCopies[i].IsValid()) {
            SCREENDECAL *decal = (SCREENDECAL *)g_pResources->GetResource(m_decalCopies[i]);
            decal->SetScreenPosition(m_visualMaster->m_pos[i] * m_size + position);
        }
    }
}

void GUIVISUAL::SetZSort(const int &zsort)
{
    m_zsort = zsort;
    for (int i = 0; i < GUIVISUAL_MAX; i++) {
        if (m_decalCopies[i].IsValid())
            ((OBJECT *)g_pResources->GetResource(m_decalCopies[i]))->SetLayer(m_visualMaster->m_zsort[i] + zsort);
        if (m_textureCopies[i].IsValid())
            ((OBJECT *)g_pResources->GetResource(m_textureCopies[i]))->SetLayer(m_visualMaster->m_zsort[i] + zsort);
    }
}

void GUIVISUAL::SetOpacity(const float &opacity)
{
    m_opacity = opacity;
    for (int i = 0; i < GUIVISUAL_MAX; i++) {
        if (m_decalCopies[i].IsValid())
            ((OBJECT *)g_pResources->GetResource(m_decalCopies[i]))->SetAlpha(((m_enabled + 1.0f) * 0.5f) * m_visible * m_visualOpacity[i] * m_opacity);
        if (m_textureCopies[i].IsValid())
            ((OBJECT *)g_pResources->GetResource(m_textureCopies[i]))->SetAlpha(((m_enabled + 1.0f) * 0.5f) * m_visible * m_visualOpacity[i] * m_opacity);
    }
}

void GUIVISUAL::Virtual34(V2 point, GUICONTROL **hit, int *zsort)
{
    float y, height;
    for (int i = 0; i < GUIVISUAL_MAX; i++) {
        int layer = -1;
        float x, width;
        if (m_decalCopies[i].IsValid()) {
            SCREENDECAL *decal = (SCREENDECAL *)g_pResources->GetResource(m_decalCopies[i]);
            layer = decal->GetLayer();
            x = decal->GetScreenPosition().x;
            y = decal->GetScreenPosition().y;
            width = decal->GetScale().x;
            height = decal->GetScale().y;
        } else if (m_textureCopies[i].IsValid()) {
            DECAL *decal = (DECAL *)g_pResources->GetResource(m_textureCopies[i]);
            layer = decal->GetLayer();
            x = decal->m_screenPosition.x;
            y = decal->m_screenPosition.y;
            width = decal->GetScale().x;
            height = decal->GetScale().y;
        }
        if (layer >= *zsort) {
            width *= 0.5f;
            height *= 0.5f;
            if (x - width <= point.x && x + width >= point.x && y - height <= point.y && y + height >= point.y) {
                *zsort = layer;
                *hit = this;
            }
        }
    }
}

void GUIVISUAL::SetDecalOrder(const int &i, const int &order)
{
    m_decalOrder[i] = order;
    m_textures[i].index = order;
    DECAL *decal = (DECAL *)g_pResources->GetResource(m_textureCopies[i]);
    if (decal)
        decal->SetFrame(order);
    SHADER *shader = (SHADER *)g_pResources->GetResource(decal->GetShaderHandle());
    if (shader) {
        m_frames[i] = shader->GetFrame(order);
    } else if (g_pLog) {
        g_pLog->Error("trying to activate an unexisting shader? visual %s idx %d, shader %s", m_visualMaster->m_name.c_str(), i,
                      m_textures[i].name.m_buf);
    }
}

void GUIVISUAL::SetTexture(const int &i, const SRTEXTURE &texture)
{
    m_textures[i] = texture;
    RESOURCE *old = g_pResources->GetResource(m_textureCopies[i]);
    if (old)
        old->Virtual20();
    m_textureCopies[i].Invalidate();
    DECAL *decal = (DECAL *)g_pDECAL->GetResource(texture.name.m_buf);
    if (decal) {
        m_textureCopies[i] = decal->m_handle;
        decal->SetRotation(m_visualMaster->m_rotation[i], 0.0f, 0.0f);
        decal->m_screenPosition = m_visualMaster->m_pos[i];
        decal->m_screenSpace = true;
        decal->SetSize(m_visualMaster->m_size[i]);
        decal->SetAlpha(m_visualOpacity[i] * m_opacity);
        decal->SetActive(true);
        SetDecalOrder(i, texture.index);
    }
}

void GUIVISUAL::SetScreenDecal(const int &i, const FIXEDSTRING<0x200> &name)
{
    m_screenDecals[i] = name;
    RESOURCE *old = g_pResources->GetResource(m_decalCopies[i]);
    if (old)
        old->Virtual20();
    m_decalCopies[i].Invalidate();
    SCREENDECAL *decal = (SCREENDECAL *)g_pSCREENDECAL->GetResource(name.m_buf);
    if (decal) {
        m_decalCopies[i] = decal->m_handle;
        decal->SetScreenPosition(m_visualMaster->m_pos[i]);
        decal->SetScreenSize(m_visualMaster->m_size[i]);
        decal->SetAlpha(m_visualOpacity[i] * m_opacity);
        decal->SetActive(true);
    }
}

void GUIVISUAL::UpdateBar(int i, bool force)
{
    float value = m_barValue[i];
    SRESULT result;
    if (m_visualMaster->m_bindProgressBar[i].Length() > 0) {
        if (!strchr(m_visualMaster->m_bindProgressBar[i].c_str(), '.'))
            result.Assign(g_pGui->Run(STRING(m_visualMaster->m_bindProgressBar[i].c_str())));
        if (!result.IsSet())
            result.Assign(g_pCmdMgr->Evaluate(STRING(m_visualMaster->m_bindProgressBar[i].c_str()), RESHANDLE()));
        if (result.GetType() == SRT_FLOAT)
            value = result.GetFloat(0.0f);
        if (result.GetType() == SRT_INT)
            value = (float)result.GetInt(0);
    }
    if (value == m_barValue[i] && !force)
        return;
    bool screen;
    if (m_textureCopies[i].IsValid())
        screen = false;
    else if (m_decalCopies[i].IsValid())
        screen = true;
    else
        return;
    bool moving = false;
    if (m_visualMaster->m_barGrowVel[i] > 0.0f) {
        float target = value;
        value = (value - m_barValue[i]) * g_pTimer->m_dt * m_visualMaster->m_barGrowVel[i] + m_barValue[i];
        if (fabs(target - value) > 1.0f)
            moving = true;
        else
            value = target;
    }
    m_barValue[i] = value;
    if (m_visualMaster->m_barDiscrete[i] && !moving) {
        float step = m_visualMaster->m_bindProgressBarMax[i] / m_visualMaster->m_barDiscreteSteps[i];
        value = (int)(value / step) * step;
    }
    V2 size(m_size.x * m_visualMaster->m_size[i].x, m_size.y * m_visualMaster->m_size[i].y);
    UVRECT uv = m_frames[i];
    V2 position(m_position.x + m_visualMaster->m_pos[i].x, m_position.y + m_visualMaster->m_pos[i].y);
    float ratio = value / m_visualMaster->m_bindProgressBarMax[i];
    if (m_visualMaster->m_barBehavior[i] == 0) {
        ratio = ratio * (1.0f - m_visualMaster->m_barStartOffset[i] - m_visualMaster->m_barEndOffset[i]) + m_visualMaster->m_barStartOffset[i];
        if (ratio < 0.0f)
            ratio = 0.0f;
        else if (ratio > 1.0f)
            ratio = 1.0f;
        switch (m_visualMaster->m_barGrowDirection[i]) {
        case 2:
            position.x += (0.5f - ratio * 0.5f) * size.x;
            size.x *= ratio;
            uv.left += (uv.right - uv.left) * (1.0f - ratio);
            break;
        case 3:
            position.x -= (0.5f - ratio * 0.5f) * size.x;
            size.x *= ratio;
            uv.right -= (uv.right - uv.left) * (1.0f - ratio);
            break;
        case 1:
            position.y += (0.5f - ratio * 0.5f) * size.y;
            size.y *= ratio;
            uv.top += (uv.bottom - uv.top) * (1.0f - ratio);
            break;
        case 0:
            position.y -= (0.5f - ratio * 0.5f) * size.y;
            size.y *= ratio;
            uv.bottom -= (uv.bottom - uv.top) * (1.0f - ratio);
            break;
        }
    } else {
        if (ratio < 0.0f)
            ratio = 0.0f;
        else if (ratio > 1.0f)
            ratio = 1.0f;
        position.x += ratio * m_visualMaster->m_barMoveMax[i].x;
        position.y += ratio * m_visualMaster->m_barMoveMax[i].y;
    }
    if (!screen) {
        DECAL *decal = (DECAL *)g_pResources->GetResource(m_textureCopies[i]);
        if (!decal)
            return;
        decal->m_screenPosition = position;
        decal->m_screenSpace = true;
        decal->SetSize(size);
        decal->SetUV(uv.left, uv.right, uv.top, uv.bottom);
    } else {
        SCREENDECAL *decal = (SCREENDECAL *)g_pResources->GetResource(m_decalCopies[i]);
        decal->SetScreenSize(size);
        decal->SetScreenPosition(position);
        if (_isnan(size.x) && g_pLog) {
            g_pLog->Error("ERROR!!!! Nan (2)");
            if (g_pLog) {
                g_pLog->Error(" %s", m_visualMaster->m_bindProgressBar[i].c_str());
                if (g_pLog)
                    g_pLog->Error(" %d %f %f %d", result.GetType(), value, m_barValue[i], force);
            }
        }
    }
}

SRESULT GUIVISUAL::UpdateBarsForced(CMDCALL *call)
{
    for (int i = 0; i < GUIVISUAL_MAX; i++) {
        if (m_textureCopies[i].IsValid() || m_decalCopies[i].IsValid())
            UpdateBar(i, true);
    }
    return SRESULT(true);
}

GUIVISUAL::GUIVISUAL(const RESHANDLE &master, GUIGROUP *parent) : GUICONTROL(master, parent)
{
    m_visualMaster = 0;
    for (g_i = 0; g_i < GUIVISUAL_MAX; g_i++)
        m_textures[g_i] = SRTEXTURE();
    for (g_i = 0; g_i < GUIVISUAL_MAX; g_i++)
        m_screenDecals[g_i].Set("");
    for (g_i = 0; g_i < GUIVISUAL_MAX; g_i++)
        m_decalOrder[g_i] = 0;
    for (g_i = 0; g_i < GUIVISUAL_MAX; g_i++)
        m_visualOpacity[g_i] = 1.0f;
    m_visualMaster = (GUIVISUALMASTER *)g_pResources->GetMaster(master);
    if (!m_visualMaster && g_pLog)
        g_pLog->Error("ERROR CREATING GUIVISUAL CONTROL!!");
    for (int i = 0; i < GUIVISUAL_MAX; i++) {
        m_visualOpacity[i] = m_visualMaster->m_opacity[i];
        m_barValue[i] = m_visualMaster->m_bindProgressBarMax[i];
        if (m_visualMaster->m_screenDecals[i].Length() > 0) {
            m_decalCopies[i].Invalidate();
            SetScreenDecal(i + 0, FIXEDSTRING<0x200>(m_visualMaster->m_screenDecals[i].c_str()));
        } else {
            m_decalCopies[i].Invalidate();
        }
        m_textureCopies[i].Invalidate();
        if (strlen(m_visualMaster->m_textures[i].name.m_buf) > 0) {
            SetTexture(i + 0, m_visualMaster->m_textures[i]);
            if (m_visualMaster->m_bindProgressBar[i].Length() > 0)
                UpdateBar(i, true);
        }
    }
}

void GUIVISUAL::Update()
{
    GUICONTROL::Update();
    for (int i = 0; i < GUIVISUAL_MAX; i++) {
        if (m_textureCopies[i].IsValid() || m_decalCopies[i].IsValid())
            UpdateBar(i, false);
    }
}

void GUIVISUAL::SetSize(const V2 &size)
{
    m_size = size;
    for (int i = 0; i < GUIVISUAL_MAX; i++) {
        if (m_decalCopies[i].IsValid()) {
            if (m_visualMaster->m_bindProgressBar[i].Length() <= 0) {
                SCREENDECAL *decal = (SCREENDECAL *)g_pResources->GetResource(m_decalCopies[i]);
                decal->SetScreenSize(m_visualMaster->m_size[i] * size);
            } else {
                UpdateBar(i, true);
            }
        }
        if (m_textureCopies[i].IsValid()) {
            if (m_visualMaster->m_bindProgressBar[i].Length() <= 0) {
                DECAL *decal = (DECAL *)g_pResources->GetResource(m_textureCopies[i]);
                decal->SetSize(m_visualMaster->m_size[i] * size);
            } else {
                UpdateBar(i, true);
            }
        }
    }
    if (m_visualMaster->m_reposOnScale && (m_size.x != 1.0f || m_size.y != 1.0f))
        SetPosition(m_position);
}
