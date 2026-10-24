#include "Tongas2/FX.h"
#include "Tongas2/DECAL.h"
#include "Tongas2/PATH.h"
#include "Tongas2/CMDMGR.h"
#include "Tongas2/TIMER.h"
#include "Tongas2/TOKENS.h"

#include <stdlib.h>
#include <string.h>

class FXMANAGER : public RESOURCEMANAGER
{
public:
    FXMANAGER()
    {
        SafeSprintf(m_fileName, "FX.txt");
        m_8 = false;
        m_18 = true;
    }

    virtual RESOURCEMASTER *CreateMaster(const char *name) { return new FXMASTER(this, name); }
    virtual const char *GetTypeCode();
};

void FX::Virtual31()
{
    if (!m_active) {
        Virtual20();
        return;
    }
    FXMASTER *master = Master();
    if (master->m_forceCompletePlay && !master->m_looping) {
        m_finishing = true;
        if (master->m_detachToScreen) {
            ((CAMERA *)g_pCamera)->WorldToScreen(GetPosition(), &m_screenPos);
            m_forceScreenPos = true;
        }
        return;
    }
    Virtual20();
}

static RESOURCEMANAGER *CreateFX();

CREATEMANAGER g_pfnCreateFX = CreateFX;

CMDENTRY FXMASTER::s_commands[] = {
    { 0, 0, 0, 0, 0 },
};

PROPENTRY FXMASTER::s_properties[] = {
    { "RandomMode", 0, 2, 0, PROP_FIELD(FXMASTER, m_randomMode), false, 1, PROP_SETTER(FXMASTER, SetRandomMode) },
    { "Looping", 0, 2, 0, PROP_FIELD(FXMASTER, m_looping), false, 1, PROP_SETTER(FXMASTER, SetLooping) },
    { "ForceCompletePlay", 0, 2, 0, PROP_FIELD(FXMASTER, m_forceCompletePlay), false, 1, PROP_SETTER(FXMASTER, SetForceCompletePlay) },
    { "Wait", 0, 3, 0, PROP_FIELD(FXMASTER, m_wait), false, FX_MAX, PROP_SETTER(FXMASTER, SetWait) },
    { "Pos", 0, 11, 0, PROP_FIELD(FXMASTER, m_pos), false, FX_MAX, PROP_SETTER(FXMASTER, SetPos) },
    { "Ang", 0, 12, 0, PROP_FIELD(FXMASTER, m_ang), false, FX_MAX, PROP_SETTER(FXMASTER, SetAng) },
    { "Scale", 0, 11, 0, PROP_FIELD(FXMASTER, m_scale), false, FX_MAX, PROP_SETTER(FXMASTER, SetScale) },
    { "Opacity", 0, 3, 0, PROP_FIELD(FXMASTER, m_opacity), false, FX_MAX, PROP_SETTER(FXMASTER, SetOpacity) },
    { "ZSort", 0, 0, 0, PROP_FIELD(FXMASTER, m_zsort), false, FX_MAX, PROP_SETTER(FXMASTER, SetZSort) },
    { "Specific", 0, 0, 0, PROP_FIELD(FXMASTER, m_specific), false, FX_MAX, PROP_SETTER(FXMASTER, SetSpecific) },
    { "Life", 0, 3, 0, PROP_FIELD(FXMASTER, m_life), false, FX_MAX, PROP_SETTER(FXMASTER, SetLife) },
    { "BoneAttach", 0, 7, 0, PROP_FIELD(FXMASTER, m_boneAttach), false, FX_MAX, PROP_SETTER(FXMASTER, SetBoneAttach) },
    { "Command", 0, 7, 0, PROP_FIELD(FXMASTER, m_command), false, FX_MAX, PROP_SETTER(FXMASTER, SetCommand) },
    { "Resource", 0, 7, 0, PROP_FIELD(FXMASTER, m_resource), false, FX_MAX, PROP_SETTER(FXMASTER, SetResource) },
    { "", 0, 15, 0, 0, false, FX_MAX, 0 },
    { "", 0, 15, 0, 0, false, FX_MAX, 0 },
    { "ScreenPos", 0, 10, 0, PROP_FIELD(FXMASTER, m_screenPos), false, 1, PROP_SETTER(FXMASTER, SetScreenPos) },
    { "ForceScreenPos", 0, 2, 0, PROP_FIELD(FXMASTER, m_forceScreenPos), false, 1, PROP_SETTER(FXMASTER, SetForceScreenPos) },
    { "DetachToScreen", 0, 2, 0, PROP_FIELD(FXMASTER, m_detachToScreen), false, 1, PROP_SETTER(FXMASTER, SetDetachToScreen) },
    { 0, 0, 15, 0, 0, false },
};

void FXMASTER::RegisterCommands()
{
    g_classParents[s_classId] = RESOURCEMASTER::s_classId;
    g_classCommands[s_classId] = s_commands;
    HashCommands(s_commands);
    RegisterClassName(s_commands, "FXMASTER", "RESOURCEMASTER");
}

int FXMASTER::PickRandom()
{
    int pick = 0;
    if (m_count > 1) {
        int used = 0;
        int i;
        for (i = 0; i < m_count; i++)
            if (m_used[i])
                used++;
        if (used == m_count) {
            for (i = 0; i < m_count; i++)
                m_used[i] = false;
            if (m_last >= 0)
                m_used[m_last] = true;
        }
        int tries = 0;
        do {
            tries++;
            pick = m_count - 1 > 0 ? rand() % m_count : 0;
            if (!m_used[pick])
                return pick;
        } while (tries < 50);
        for (i = 0; i < m_count; i++)
            if (!m_used[i])
                return i;
    }
    return pick;
}

const char *FXMANAGER::GetTypeCode()
{
    return "FXFX";
}

static RESOURCEMANAGER *CreateFX()
{
    return new FXMANAGER;
}

void FX::Reset()
{
    FXMASTER *master = Master();
    float time = 0.0f;
    float duration = 0.0f;
    for (int i = 0; i < FX_MAX; i++) {
        time += master->m_wait[i];
        float end = time + master->m_life[i];
        if (duration < end)
            duration = end;
        m_start[i] = time;
        m_spawned[i] = false;
        m_done[i] = false;
        OBJECT *child = (OBJECT *)g_pResources->GetResource(m_children[i]);
        if (child)
            child->Detach();
        m_children[i].Invalidate();
    }
    m_duration = duration;
    m_time = 0.0f;
    m_finishing = false;
    if (master->m_randomMode) {
        m_chosen = master->PickRandom();
        for (int j = 0; j < master->m_count; j++)
            if (j != m_chosen)
                m_spawned[j] = true;
    }
    m_forceScreenPos = master->m_forceScreenPos;
    m_screenPos = master->m_screenPos;
}

FXMASTER::FXMASTER(RESOURCEMANAGER *manager, const char *name)
    : RESOURCEMASTER(manager, name)
{
    m_count = 0;
    m_randomMode = false;
    m_looping = false;
    m_forceCompletePlay = false;
    for (g_i = 0; g_i < FX_MAX; g_i++)
        m_wait[g_i] = 0.0f;
    for (g_i = 0; g_i < FX_MAX; g_i++)
        m_pos[g_i] = V3(0.0f, 0.0f, 0.0f);
    for (g_i = 0; g_i < FX_MAX; g_i++)
        m_ang[g_i] = ANG(0.0f, 0.0f, 0.0f);
    for (g_i = 0; g_i < FX_MAX; g_i++)
        m_scale[g_i] = V3(1.0f, 1.0f, 1.0f);
    for (g_i = 0; g_i < FX_MAX; g_i++)
        m_opacity[g_i] = 1.0f;
    for (g_i = 0; g_i < FX_MAX; g_i++)
        m_zsort[g_i] = 0;
    for (g_i = 0; g_i < FX_MAX; g_i++)
        m_specific[g_i] = 0;
    for (g_i = 0; g_i < FX_MAX; g_i++)
        m_life[g_i] = 0.0f;
    for (g_i = 0; g_i < FX_MAX; g_i++)
        m_boneAttach[g_i] = "";
    for (g_i = 0; g_i < FX_MAX; g_i++)
        m_command[g_i] = "";
    for (g_i = 0; g_i < FX_MAX; g_i++)
        m_resource[g_i] = "";
    m_forceScreenPos = false;
    m_detachToScreen = false;
    m_screenPos = V2(0.5f, 0.5f);
    // Cleared four at a time.
    for (int i = 0; i < FX_MAX / 4; i++)
        ((int *)m_used)[i] = 0;
    m_last = -1;
}

int FXMASTER::GetClassId()
{
    return s_classId;
}

int FXMASTER::GetParentClassId()
{
    return g_classParents[s_classId];
}

const char *FXMASTER::GetClassName()
{
    return g_classNames[s_classId];
}

void FXMASTER::SetRandomMode(const bool &random)
{
    m_randomMode = random;
}

void FXMASTER::SetLooping(const bool &looping)
{
    m_looping = looping;
}

void FXMASTER::SetForceCompletePlay(const bool &force)
{
    m_forceCompletePlay = force;
}

void FXMASTER::SetWait(const int &i, const float &wait)
{
    m_wait[i] = wait;
}

void FXMASTER::SetPos(const int &i, const V3 &pos)
{
    m_pos[i] = pos;
}

void FXMASTER::SetAng(const int &i, const ANG &ang)
{
    m_ang[i] = ang;
}

void FXMASTER::SetScale(const int &i, const V3 &scale)
{
    m_scale[i] = scale;
}

void FXMASTER::SetOpacity(const int &i, const float &opacity)
{
    m_opacity[i] = opacity;
}

void FXMASTER::SetZSort(const int &i, const int &zsort)
{
    m_zsort[i] = zsort;
}

void FXMASTER::SetSpecific(const int &i, const int &specific)
{
    m_specific[i] = specific;
}

void FXMASTER::SetLife(const int &i, const float &life)
{
    m_life[i] = life;
}

void FXMASTER::SetScreenPos(const V2 &pos)
{
    m_screenPos = pos;
}

void FXMASTER::SetForceScreenPos(const bool &force)
{
    m_forceScreenPos = force;
}

void FXMASTER::SetDetachToScreen(const bool &detach)
{
    m_detachToScreen = detach;
}

FX::FX(FXMASTER *master, const RESHANDLE &handle)
    : OBJECT(master, handle)
{
    for (int i = 0; i < FX_MAX; i++) {
        m_done[i] = false;
        m_start[i] = 0.0f;
    }
    m_chosen = -1;
    m_finishing = false;
    m_forceScreenPos = false;
}

RESOURCE *FXMASTER::CreateCopy(const RESHANDLE &handle)
{
    return new FX(this, handle);
}

void FX::Spawn(int i)
{
    FXMASTER *master = Master();
    OBJECT *child = (OBJECT *)g_pResources->GetResource(master->m_handles[i]);
    if (!child) {
        child = (OBJECT *)g_pResources->GetResource(master->m_resource[i].c_str());
        if (!child)
            return;
    }
    m_children[i] = child->m_handle;
    bool attached = false;
    if (master->m_boneAttach[i].Length() <= 0) {
        child->AttachTo(0, 0);
    } else {
        TOKENS tokens;
        {
            STRING attach(master->m_boneAttach[i].c_str());
            tokens.Split(attach, ",");
        }
        int index = atoi(tokens.GetToken(0));
        if (index < 0)
            index = 0;
        else if (index > FX_MAX - 1)
            index = FX_MAX - 1;
        WTACTOR *other = (WTACTOR *)g_pResources->GetResource(m_children[index]);
        if (other) {
            child->AttachTo(other, tokens.GetToken(1));
            child->SetOffsetV(master->m_pos[i]);
            child->SetRelAngles(master->m_ang[i]);
            attached = true;
        } else {
            child->AttachTo(0, 0);
        }
    }
    if (m_parent.IsValid()) {
        if (!attached) {
            V3 pos = GetRelAngles().Transform(master->m_pos[i]);
            pos = pos * GetSize();
            pos += GetOffset();
            child->SetOffsetV(pos);
            ANG angles(0.0f, 0.0f, 0.0f);
            angles.SetMatrix(master->m_ang[i].GetMatrix() * GetRelAngles().GetMatrix());
            child->SetRelAngles(angles);
        }
        child->SetScaleFactorV(master->m_scale[i] * GetSize());
        child->SetAlphaFactor(GetAlphaFactor() * master->m_opacity[i]);
        child->SetRelLayer(GetRelLayer() + master->m_zsort[i]);
        child->SetFrame(master->m_specific[i]);
        child->SetParent(m_parent);
        if (!attached && m_attached.IsValid()) {
            WTACTOR *actor = (WTACTOR *)g_pResources->GetResource(m_attached);
            if (actor)
                child->AttachTo(actor, m_tagName);
        }
    } else {
        if (!attached) {
            V3 pos = GetAngles().Transform(master->m_pos[i]);
            pos = pos * GetScale();
            pos += GetPosition();
            child->SetPosition(pos);
            ANG angles(0.0f, 0.0f, 0.0f);
            angles.SetMatrix(master->m_ang[i].GetMatrix() * GetAngles().GetMatrix());
            child->SetAngles(angles);
        }
        child->SetScaleV(master->m_scale[i] * GetScale());
        child->SetAlpha(GetAlpha() * master->m_opacity[i]);
        child->SetLayer(GetLayer() + master->m_zsort[i]);
        child->SetFrame(master->m_specific[i]);
    }
    child->SetActive(true);
    {
        STRING command(master->m_command[i].c_str());
        g_pCmdMgr->Run(command, m_parent);
    }
}

// The camera-facing block of FX::Update is matched with these small helpers;
// the cross products keep the operand order the compiler saw.
static inline void FxScaleTo(V3 *out, const V3 &v, float f)
{
    out->x = v.x * f;
    out->y = v.y * f;
    out->z = v.z * f;
}

static inline V3 FxNormalized(const V3 &v)
{
    V3 r = v;
    r.Normalize();
    return r;
}

static inline V3 FxCross(const V3 &b, const V3 &a)
{
    return V3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

static inline V3 FxCross2(const V3 &s, const V3 &b)
{
    return V3(s.z * b.y - s.y * b.z, s.x * b.z - s.z * b.x, s.y * b.x - s.x * b.y);
}

void FX::Update()
{
    OBJECT::Update();
    if (!m_active)
        return;
    if (m_forceScreenPos) {
        float depth = 1.0f;
        SetPosition(((CAMERA *)g_pCamera)->ScreenToWorld(m_screenPos, depth));
        V3 up = GetAngles().GetUpPoint();
        V3 back;
        FxScaleTo(&back, g_pCamera->GetDirection(), -1.0f);
        V3 side = FxNormalized(FxCross(back, up));
        up = FxNormalized(FxCross2(side, back));
        SetAngles(ANG(up, back));
    }
    FXMASTER *master = Master();
    for (int i = 0; i < master->m_count; i++) {
        if (master->m_randomMode && i != m_chosen)
            continue;
        if (m_done[i])
            continue;
        OBJECT *child = (OBJECT *)g_pResources->GetResource(m_children[i]);
        if (!child) {
            if (m_time >= m_start[i] && !m_spawned[i]) {
                m_spawned[i] = true;
                master->m_used[i] = true;
                master->m_last = i;
                Spawn(i);
            }
            continue;
        }
        if (m_time - m_start[i] > master->m_life[i]) {
            child->Detach();
            m_done[i] = true;
            continue;
        }
        if (m_parent.IsValid()) {
            V3 pos = GetRelAngles().Transform(master->m_pos[i]);
            pos = pos * GetSize();
            pos += GetOffset();
            child->SetOffsetV(pos);
            ANG angles(0.0f, 0.0f, 0.0f);
            angles.SetMatrix(master->m_ang[i].GetMatrix() * GetRelAngles().GetMatrix());
            child->SetRelAngles(angles);
            child->SetScaleFactorV(master->m_scale[i] * GetSize());
            child->SetAlphaFactor(GetAlphaFactor() * master->m_opacity[i]);
            child->SetRelLayer(GetRelLayer() + master->m_zsort[i]);
        } else {
            V3 pos = GetAngles().Transform(master->m_pos[i]);
            pos = pos * GetScale();
            pos += GetPosition();
            child->SetPosition(pos);
            ANG angles(0.0f, 0.0f, 0.0f);
            angles.SetMatrix(master->m_ang[i].GetMatrix() * GetAngles().GetMatrix());
            child->SetAngles(angles);
            child->SetScaleV(master->m_scale[i] * GetScale());
            child->SetAlpha(GetAlpha() * master->m_opacity[i]);
            child->SetLayer(GetLayer() + master->m_zsort[i]);
        }
    }
    m_time += g_pTimer->m_dt;
    if (m_time >= m_duration) {
        if (master->m_looping) {
            float time = m_time - m_duration;
            Reset();
            m_time = time;
            return;
        }
        Virtual20();
    }
}
