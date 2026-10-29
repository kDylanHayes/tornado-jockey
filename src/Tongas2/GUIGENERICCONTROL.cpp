// GUIGENERIC, the GUI's generic elements: up to 32 objects placed in the
// world in front of the camera, at screen positions, so that they look
// fixed on the screen. The original file name is not known; the masters are
// in GUIGENERIC.cpp.
// This file was built seeing TOKENS' destructor inline.
#define TOKENS_INLINE_DTOR

#include "Tongas2/GUIGENERIC.h"
#include "Tongas2/BONEDMODEL.h"
#include "Tongas2/CAMERA.h"
#include "Tongas2/DECAL.h"
#include "Tongas2/EMITTER.h"
#include "Tongas2/LOG.h"
#include "Tongas2/PATH.h"
#include "Tongas2/STRING.h"
#include "Tongas2/TIMER.h"
#include "Tongas2/TOKENS.h"

#include <stdlib.h>
#include <windows.h>

#define CMD_FUNC(CLASS, method) (CMDFUNC)(SRESULT (CLASS::*)(TOKENS **))(SRESULT (CLASS::*)(CMDCALL *))&CLASS::method

CMDENTRY GUIGENERIC::s_commands[] = {
    { "actorShowPart", 0, 3, 3, CMD_FUNC(GUIGENERIC, ActorShowPart) },
    { "playAnimation", 0, 3, 3, CMD_FUNC(GUIGENERIC, PlayAnimation) },
    { "blendAnimation", 0, 4, 4, CMD_FUNC(GUIGENERIC, BlendAnimation) },
    { 0, 0, 0, 0, 0 },
};

PROPENTRY GUIGENERIC::s_properties[] = {
    { "ResourcesAng", 0, 12, 0, PROP_FIELD(GUIGENERIC, m_ang), false, GUIGENERIC_MAX, PROP_SETTER(GUIGENERIC, SetAng) },
    { "ResourcesAngVel", 0, 11, 0, PROP_FIELD(GUIGENERIC, m_angVel), false, GUIGENERIC_MAX, PROP_SETTER(GUIGENERIC, SetAngVel) },
    { "ResourcesScale", 0, 11, 0, PROP_FIELD(GUIGENERIC, m_scale), false, GUIGENERIC_MAX, PROP_SETTER(GUIGENERIC, SetScale) },
    { "ResourcesScreenPos", 0, 10, 0, PROP_FIELD(GUIGENERIC, m_screenPos), false, GUIGENERIC_MAX, PROP_SETTER(GUIGENERIC, SetScreenPos) },
    { "ResourcesScreenPosZ", 0, 3, 0, PROP_FIELD(GUIGENERIC, m_screenPosZ), false, GUIGENERIC_MAX, PROP_SETTER(GUIGENERIC, SetScreenPosZ) },
    { 0, 0, 15, 0, 0, false },
};

void GUIGENERIC::RegisterCommands()
{
    g_classParents[s_classId] = GUICONTROL::s_classId;
    g_classCommands[s_classId] = s_commands;
    HashCommands(s_commands);
    RegisterClassName(s_commands, "GUIGENERIC", "GUICONTROL");
}

GUIGENERIC::~GUIGENERIC()
{
    for (int i = 0; i < GUIGENERIC_MAX; i++) {
        RESOURCE *copy = g_pResources->GetResource(m_handles[i]);
        if (copy)
            copy->Virtual20();
        m_handles[i].Invalidate();
    }
}

void GUIGENERIC::SetPosition(const V2 &position)
{
    m_position = position;
    for (int i = 0; i < GUIGENERIC_MAX; i++) {
        if (m_handles[i].IsValid()) {
            OBJECT *copy = (OBJECT *)g_pResources->GetResource(m_handles[i]);
            if (copy) {
                if (m_attach[i] >= 0)
                    copy->SetOffset(m_screenPos[i].x, m_screenPos[i].y, m_screenPosZ[i]);
                else {
                    V3 world = ((CAMERA *)g_pCamera)->ScreenToWorld(m_screenPos[i] + position, m_screenPosZ[i]);
                    copy->SetPosition(world);
                }
            }
        }
    }
    m_cameraPos = ((ACTOR *)g_pCamera)->GetPosition();
}

void GUIGENERIC::SetSize(const V2 &size)
{
    m_size = size;
    for (int i = 0; i < GUIGENERIC_MAX; i++) {
        if (m_handles[i].IsValid()) {
            OBJECT *copy = (OBJECT *)g_pResources->GetResource(m_handles[i]);
            if (copy) {
                int attach = m_attach[i];
                if (attach >= 0) {
                    if (attach >= GUIGENERIC_MAX && g_pLog)
                        g_pLog->Error("Error: %s tried to attach himself to resource number %i, id: %i", m_genericMaster->m_name.c_str(), i, attach);
                    float scale = m_screenPosZ[attach] * ((CAMERA *)g_pCamera)->GetViewSize().x;
                    copy->SetScale(m_scale[i].x * size.x * scale, m_scale[i].y * size.x * scale, m_scale[i].z * size.x * scale);
                } else {
                    V2 view = ((CAMERA *)g_pCamera)->GetViewSize();
                    float scale = m_screenPosZ[i] * view.x;
                    copy->SetScale(m_scale[i].x * size.x * scale, m_scale[i].y * size.x * scale, m_scale[i].z * size.x * scale);
                }
            }
        }
    }
    m_cameraFov = ((CAMERA *)g_pCamera)->m_fov;
}

void GUIGENERIC::SetZSort(const int &zsort)
{
    m_zsort = zsort;
    for (int i = 0; i < GUIGENERIC_MAX; i++) {
        if (m_handles[i].IsValid()) {
            OBJECT *copy = (OBJECT *)g_pResources->GetResource(m_handles[i]);
            if (copy)
                copy->SetLayer(m_genericMaster->m_zsort[i] + zsort);
        }
    }
}

void GUIGENERIC::SetOpacity(const float &opacity)
{
    m_opacity = opacity;
    for (int i = 0; i < GUIGENERIC_MAX; i++) {
        if (m_handles[i].IsValid()) {
            OBJECT *copy = (OBJECT *)g_pResources->GetResource(m_handles[i]);
            if (copy)
                copy->SetAlpha(((m_enabled + 1.0f) * 0.5f) * m_genericMaster->m_opacity[i] * m_visible * opacity);
        }
    }
}

void GUIGENERIC::UpdateAngles()
{
    for (int i = 0; i < GUIGENERIC_MAX; i++) {
        if (m_handles[i].IsValid()) {
            OBJECT *copy = (OBJECT *)g_pResources->GetResource(m_handles[i]);
            if (copy) {
                if (m_attach[i] >= 0) {
                    copy->SetRelAngles(m_ang[i]);
                } else {
                    ANG angles(((CAMERA *)g_pCamera)->m_up, ((CAMERA *)g_pCamera)->m_direction);
                    angles.SetMatrix(m_ang[i].GetMatrix() * angles.GetMatrix());
                    copy->SetAngles(angles);
                }
            }
        }
    }
    m_cameraDirection = ((CAMERA *)g_pCamera)->m_direction;
    m_cameraUp = ((CAMERA *)g_pCamera)->m_up;
}

SRESULT GUIGENERIC::ActorShowPart(CMDCALL *call)
{
    bool show = atoi(call->tokens->GetToken(3)) != 0;
    BONEDMODELRESOURCE *copy = (BONEDMODELRESOURCE *)g_pResources->GetResource(m_handles[atoi(call->tokens->GetToken(1))]);
    if (copy) {
        int id = g_pBONEDMODEL->m_id;
        if (copy->GetManagerId() != id) {
            if (g_pLog)
                g_pLog->Error("trying to show a part from a resource that is not a WTACTOR! generic master=%s resource master=%s",
                              m_genericMaster->m_name.c_str(), copy->m_master->m_name.c_str());
            return SRESULT(false);
        }
        RESOURCE *hide = g_pSHADER->GetResource("hide");
        if (!hide) {
            if (g_pLog)
                g_pLog->Error("Could not get hide shader!");
            return SRESULT(false);
        }
        while (!hide->IsLoaded())
            Sleep(50);
        if (show)
            copy->ShowPart(call->tokens->GetToken(2));
        else
            copy->HidePart(hide, call->tokens->GetToken(2));
        return SRESULT(true);
    }
    return SRESULT(false);
}

inline void GUIGENERIC::PlayResourceAnimation(int i, const char *animation, bool loop, float speed)
{
    BONEDMODELRESOURCE *copy = (BONEDMODELRESOURCE *)g_pResources->GetResource(m_handles[i]);
    if (copy)
        copy->PlayAnimation((void *)animation, loop, speed);
}

inline void GUIGENERIC::BlendResourceAnimation(int i, const char *animation, float blend, bool loop, float speed)
{
    BONEDMODELRESOURCE *copy = (BONEDMODELRESOURCE *)g_pResources->GetResource(m_handles[i]);
    if (copy)
        copy->BlendAnimation(animation, blend, loop, speed);
}

SRESULT GUIGENERIC::PlayAnimation(CMDCALL *call)
{
    PlayResourceAnimation(atoi(call->tokens->GetToken(1)), call->tokens->GetToken(2), atoi(call->tokens->GetToken(3)) != 0, 1.0f);
    return SRESULT(true);
}

SRESULT GUIGENERIC::BlendAnimation(CMDCALL *call)
{
    BlendResourceAnimation(atoi(call->tokens->GetToken(1)), call->tokens->GetToken(2), (float)atof(call->tokens->GetToken(3)),
                           atoi(call->tokens->GetToken(4)) != 0, 1.0f);
    return SRESULT(true);
}

// Whether two points differ.
inline bool Differs(const V3 &a, const V3 &b)
{
    return a.x != b.x || a.y != b.y || a.z != b.z;
}

void GUIGENERIC::Update()
{
    if (Differs(m_cameraPos, ((ACTOR *)g_pCamera)->GetPosition()))
        SetPosition(m_position);
    if (m_cameraFov != ((CAMERA *)g_pCamera)->m_fov)
        SetSize(m_size);
    for (int i = 0; i < GUIGENERIC_MAX; i++) {
        if (m_handles[i].IsValid()) {
            // The exe passes SetAng a copy of i.
            if (Differs(m_angVel[i], V3(0.0f, 0.0f, 0.0f)))
                SetAng(i + 0, ANG(m_ang[i].GetEulerX() + g_pTimer->GetSoundDt() * m_angVel[i].x,
                              m_ang[i].GetEulerY() + g_pTimer->GetSoundDt() * m_angVel[i].y,
                              m_ang[i].GetEulerZ() + g_pTimer->GetSoundDt() * m_angVel[i].z));
        }
    }
    if (Differs(m_cameraDirection, ((CAMERA *)g_pCamera)->m_direction) || Differs(m_cameraUp, ((CAMERA *)g_pCamera)->m_up))
        UpdateAngles();
    GUICONTROL::Update();
}

GUIGENERIC::GUIGENERIC(const RESHANDLE &master, GUIGROUP *parent)
    : GUICONTROL(master, parent), m_cameraPos(0.0f, 0.0f, 0.0f), m_cameraDirection(0.0f, 0.0f, 0.0f), m_cameraUp(0.0f, 0.0f, 0.0f)
{
    m_genericMaster = 0;
    for (g_i = 0; g_i < GUIGENERIC_MAX; g_i++)
        m_ang[g_i] = ANG(0.0f, 0.0f, 0.0f);
    for (g_i = 0; g_i < GUIGENERIC_MAX; g_i++)
        m_angVel[g_i] = V3(0.0f, 0.0f, 0.0f);
    for (g_i = 0; g_i < GUIGENERIC_MAX; g_i++)
        m_scale[g_i] = V3(1.0f, 1.0f, 1.0f);
    for (g_i = 0; g_i < GUIGENERIC_MAX; g_i++)
        m_screenPos[g_i] = V2(0.0f, 0.0f);
    for (g_i = 0; g_i < GUIGENERIC_MAX; g_i++)
        m_screenPosZ[g_i] = 1.0f;
    m_genericMaster = (GUIGENERICMASTER *)g_pResources->GetMaster(master);
    if (!m_genericMaster && g_pLog)
        g_pLog->Error("ERROR CREATING GUIGENERIC CONTROL!!");
    for (int i = 0; i < GUIGENERIC_MAX; i++) {
        m_attach[i] = -1;
        if (m_genericMaster->m_resources[i].Length() > 0) {
            OBJECT *copy = (OBJECT *)g_pResources->GetResource(m_genericMaster->m_resources[i].c_str());
            if (copy) {
                copy->SetFrame(m_genericMaster->m_specific[i]);
                copy->SetActive(true);
                m_handles[i] = copy->m_handle;
                SetScreenPos(i, m_genericMaster->m_screenPos[i]);
                SetScreenPosZ(i, m_genericMaster->m_screenPosZ[i]);
                SetScale(i, m_genericMaster->m_scale[i]);
                SetAng(i, m_genericMaster->m_ang[i]);
                SetAngVel(i, m_genericMaster->m_angVel[i]);
                m_attach[i] = -1;
                if (m_genericMaster->m_boneAttach[i].Length() > 0) {
                    TOKENS tokens;
                    tokens.Split(STRING(m_genericMaster->m_boneAttach[i].c_str()), ",");
                    if (tokens.m_count != 2) {
                        if (g_pLog)
                            g_pLog->Error("bone attach attribute must be in format 'residx,tagname', example: '0,tag_head'");
                    } else {
                        int resource = atoi(tokens.GetToken(0));
                        const char *bone = tokens.GetToken(1);
                        WTACTOR *target = (WTACTOR *)g_pResources->GetResource(m_handles[resource]);
                        if (!target) {
                            if (g_pLog)
                                g_pLog->Error("invalid resource at index %d to set bone attachment");
                        } else {
                            copy->AttachTo(target, bone);
                            m_attach[i] = resource;
                        }
                    }
                }
            } else if (g_pLog) {
                g_pLog->Error("GUIGENERIC - could not find resource:%s", m_genericMaster->m_resources[i].c_str());
            }
        } else {
            m_handles[i].Invalidate();
        }
    }
    UpdateAngles();
    SetSize(m_size);
    SetPosition(m_position);
    SetZSort(m_zsort);
    SetOpacity(m_opacity);
    m_cameraFov = -10.0f;
}
