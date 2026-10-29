#define LOG_VALUE
#define ANG_INLINE_AXES
#include "Tongas2/ACTOR.h"
#include "Tongas2/ACTORRESOURCES.h"
#include "Tongas2/BONEDMODEL.h"
#include "Tongas2/CMDMGR.h"
#include "Tongas2/CPRIMITIVE3.h"
#include "Tongas2/CSTRING.h"
#include "Tongas2/FIXEDSTRING.h"
#include "Tongas2/LOG.h"
#include "Tongas2/PATH.h"
#include "Tongas2/RESOURCE.h"
#include "Tongas2/RESOURCES.h"
#include "Tongas2/SCENE.h"
#include "Tongas2/STRING.h"
#include "Tongas2/TIMER.h"
#include "Tongas2/TOKENS.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CMD_FUNC(CLASS, method) (CMDFUNC)(SRESULT (CLASS::*)(TOKENS **))(SRESULT (CLASS::*)(CMDCALL *))&CLASS::method

// Defined elsewhere.
extern RESOURCEMANAGER *g_pFX;
extern RESOURCEMANAGER *g_pACTORRESOURCES;
extern RESOURCEMANAGER *g_pPATH;
extern RESOURCEMANAGER *g_pBONEDMODEL;
// A vector at the origin.
extern V3 g_zero;

// Defined elsewhere: a node of the scene's visibility tree.
class PVSNODE
{
public:
    void RemoveMember(ACTOR *actor);
};

// Defined elsewhere: a behavior addBehavior can add, by name.
struct BEHAVIORENTRY
{
    const char *name;
    int hash;
    BEHAVIOR *(*create)(const RESHANDLE &owner, CMDCALL *call);
};

BEHAVIORENTRY *FindBehavior(const char *name);

static const char s_behaviorNames[][30] = {
    "LINEAL",
    "STATIC",
    "ATTACHED",
    "BONEATTACHED",
    "CAMERAATTACHED",
    "PATH",
    "",
};

static const char s_activationNames[][30] = {
    "OnStart",
    "AlwaysActive",
    "UseOccludingBox",
    "Manually",
    "",
};

static const char s_deactivationNames[][30] = {
    "UseOccludingBox",
    "Manually",
    "",
};

static const char s_pathBehaviorNames[][30] = {
    "Follow",
    "Relaxed follow",
    "Flow",
    "Path finder",
    "",
};

CMDENTRY ACTOR::s_commands[] = {
    { "remove", 0, 0, 0, CMD_FUNC(ACTOR, Remove) },
    { "setanimationframe", 0, 3, 3, CMD_FUNC(ACTOR, SetAnimationFrame) },
    { "playanimation", 0, 3, 3, CMD_FUNC(ACTOR, PlayAnimation) },
    { "stopanimation", 0, 1, 1, CMD_FUNC(ACTOR, StopAnimation) },
    { "blendanimation", 0, 4, 4, CMD_FUNC(ACTOR, BlendAnimation) },
    { "pauseAnimation", 0, 1, 2, CMD_FUNC(ACTOR, PauseAnimation) },
    { "attachFX", 0, 4, 4, CMD_FUNC(ACTOR, AttachFX) },
    { "createFX", 0, 1, 1, CMD_FUNC(ACTOR, CreateFX) },
    { "createTagFX", 0, 4, 4, CMD_FUNC(ACTOR, CreateTagFX) },
    { "fade", 0, 3, 3, CMD_FUNC(ACTOR, Fade) },
    { "resetResources", 0, 0, 0, CMD_FUNC(ACTOR, ResetResources) },
    { "addBehavior", 0, 1, 4096, CMD_FUNC(ACTOR, AddBehavior) },
    { "clearBehaviors", 0, 0, 0, CMD_FUNC(ACTOR, ClearBehaviors) },
    { "Activate", 0, 0, 0, CMD_FUNC(ACTOR, Activate) },
    { "Deactivate", 0, 0, 0, CMD_FUNC(ACTOR, Deactivate) },
    { "SetLight", 0, 1, 1, CMD_FUNC(ACTOR, SetLight) },
    { "SetSpeed", 0, 1, 1, CMD_FUNC(ACTOR, SetSpeed) },
    { "ActivateResource", 0, 1, 1, CMD_FUNC(ACTOR, ActivateResource) },
    { "DeactivateResource", 0, 1, 1, CMD_FUNC(ACTOR, DeactivateResource) },
    { "QueueBh2", 0, 2, 2, CMD_FUNC(ACTOR, QueueBehaviorCommand) },
    { "ChangeResourceMaterial", 0, 3, 3, CMD_FUNC(ACTOR, ChangeResourceMaterialCommand) },
    { 0, 0, 0, 0, 0 },
};

PROPENTRY ACTOR::s_properties[] = {
    { "Pos", 0, 11, 0, PROP_FIELD(ACTOR, m_position), false, 1, PROP_SETTER(ACTOR, SetPosition) },
    { "Ang", 0, 12, 0, PROP_FIELD(ACTOR, m_angles), false, 1, PROP_SETTER(ACTOR, SetAngles) },
    { "Scale", 0, 11, 0, PROP_FIELD(ACTOR, m_scale), false, 1, PROP_SETTER(ACTOR, SetScale) },
    { "IsTemplate", 0, 2, 0, PROP_FIELD(ACTOR, m_isTemplate), false, 1, PROP_SETTER(ACTOR, SetIsTemplate) },
    { "Frozen", 0, 2, 0, PROP_FIELD(ACTOR, m_frozen), false, 1, PROP_SETTER(ACTOR, SetFrozen) },
    { "Comments", 0, 7, 0, PROP_FIELD(ACTOR, m_comments), false, 1, PROP_SETTER(ACTOR, SetComments) },
    { "ColliderColorOverride", 0, 2, 0, PROP_FIELD(ACTOR, m_colliderColorOverride), false, 1, PROP_SETTER(ACTOR, SetColliderColorOverride) },
    { "ColliderColor", 0, 13, 0, PROP_FIELD(ACTOR, m_colliderColor), false, 1, PROP_SETTER(ACTOR, SetColliderColor) },
    { "ShowCollider", 0, 2, 0, PROP_FIELD(ACTOR, m_showCollider), false, 1, PROP_SETTER(ACTOR, SetShowCollider) },
    { "AttachTo", 0, 7, 0, PROP_FIELD(ACTOR, m_attachTo), false, 1, PROP_SETTER(ACTOR, SetAttachTo) },
    { "AttachBone", 0, 7, 0, PROP_FIELD(ACTOR, m_attachBone), false, 1, PROP_SETTER(ACTOR, SetAttachBone) },
    { "AttachBoneResId", 0, 0, 0, PROP_FIELD(ACTOR, m_attachBoneResId), false, 1, PROP_SETTER(ACTOR, SetAttachBoneResId) },
    { "AttachPos", 0, 11, 0, PROP_FIELD(ACTOR, m_attachPos), false, 1, PROP_SETTER(ACTOR, SetAttachPos) },
    { "AttachAng", 0, 12, 0, PROP_FIELD(ACTOR, m_attachAng), false, 1, PROP_SETTER(ACTOR, SetAttachAng) },
    { "AttachScale", 0, 11, 0, PROP_FIELD(ACTOR, m_attachScale), false, 1, PROP_SETTER(ACTOR, SetAttachScale) },
    { "AttachOpacity", 0, 3, 0, PROP_FIELD(ACTOR, m_attachOpacity), false, 1, PROP_SETTER(ACTOR, SetAttachOpacity) },
    { "AttachZSort", 0, 0, 0, PROP_FIELD(ACTOR, m_attachZSort), false, 1, PROP_SETTER(ACTOR, SetAttachZSort) },
    { "Activation", 0, 1, s_activationNames, PROP_FIELD(ACTOR, m_activation), false, 1, PROP_SETTER(ACTOR, SetActivation) },
    { "Deactivation", 0, 1, s_deactivationNames, PROP_FIELD(ACTOR, m_deactivation), false, 1, PROP_SETTER(ACTOR, SetDeactivation) },
    { "OnActivate", 0, 7, 0, PROP_FIELD(ACTOR, m_onActivate), false, 1, PROP_SETTER(ACTOR, SetOnActivate) },
    { "TestOnActivate", 0, 2, 0, PROP_FIELD(ACTOR, m_testOnActivate), false, 1, PROP_SETTER(ACTOR, SetTestOnActivate) },
    { "OnActivateRunInEditor", 0, 2, 0, PROP_FIELD(ACTOR, m_onActivateRunInEditor), false, 1, PROP_SETTER(ACTOR, SetOnActivateRunInEditor) },
    { "Behavior", 0, 1, s_behaviorNames, PROP_FIELD(ACTOR, m_behavior), false, 1, PROP_SETTER(ACTOR, SetBehavior) },
    { "Vel", 0, 11, 0, PROP_FIELD(ACTOR, m_vel), false, 1, PROP_SETTER(ACTOR, SetVel) },
    { "AngVel", 0, 11, 0, PROP_FIELD(ACTOR, m_angVel), false, 1, PROP_SETTER(ACTOR, SetAngVel) },
    { "AngAceleration", 0, 11, 0, PROP_FIELD(ACTOR, m_angAceleration), false, 1, PROP_SETTER(ACTOR, SetAngAceleration) },
    { "Aceleration", 0, 11, 0, PROP_FIELD(ACTOR, m_aceleration), false, 1, PROP_SETTER(ACTOR, SetAceleration) },
    { "PanSounds", 0, 2, 0, PROP_FIELD(ACTOR, m_panSounds), false, 1, PROP_SETTER(ACTOR, SetPanSounds) },
    { "MoveWithScreenZ", 0, 2, 0, PROP_FIELD(ACTOR, m_moveWithScreenZ), false, 1, PROP_SETTER(ACTOR, SetMoveWithScreenZ) },
    { "MoveWithScreenY", 0, 2, 0, PROP_FIELD(ACTOR, m_moveWithScreenY), false, 1, PROP_SETTER(ACTOR, SetMoveWithScreenY) },
    { "PreviewBehavior", 0, 2, 0, PROP_FIELD(ACTOR, m_previewBehavior), false, 1, PROP_SETTER(ACTOR, SetPreviewBehavior) },
    { "RotateToMove", 0, 2, 0, PROP_FIELD(ACTOR, m_rotateToMove), false, 1, PROP_SETTER(ACTOR, SetRotateToMove) },
    { "ForcedCamDist", 0, 3, 0, PROP_FIELD(ACTOR, m_forcedCamDist), false, 1, PROP_SETTER(ACTOR, SetForcedCamDist) },
    { "ForceInScreen", 0, 2, 0, PROP_FIELD(ACTOR, m_forceInScreen), false, 1, PROP_SETTER(ACTOR, SetForceInScreen) },
    { "PathName", 0, 7, 0, PROP_FIELD(ACTOR, m_pathName), false, 1, PROP_SETTER(ACTOR, SetPathName) },
    { "PathBehavior", 0, 1, s_pathBehaviorNames, PROP_FIELD(ACTOR, m_pathBehavior), false, 1, PROP_SETTER(ACTOR, SetPathBehavior) },
    { "TestPath", 0, 2, 0, PROP_FIELD(ACTOR, m_testPath), false, 1, PROP_SETTER(ACTOR, SetTestPath) },
    { "VectorUp", 0, 11, 0, PROP_FIELD(ACTOR, m_vectorUp), false, 1, PROP_SETTER(ACTOR, SetVectorUp) },
    { "VectorForward", 0, 11, 0, PROP_FIELD(ACTOR, m_vectorForward), false, 1, PROP_SETTER(ACTOR, SetVectorForward) },
    { "Loop", 0, 2, 0, PROP_FIELD(ACTOR, m_loop), false, 1, PROP_SETTER(ACTOR, SetLoop) },
    { "FlowAngle", 0, 3, 0, PROP_FIELD(ACTOR, m_flowAngle), false, 1, PROP_SETTER(ACTOR, SetFlowAngle) },
    { "FlowRadius", 0, 3, 0, PROP_FIELD(ACTOR, m_flowRadius), false, 1, PROP_SETTER(ACTOR, SetFlowRadius) },
    { "PathRelativePos", 0, 2, 0, PROP_FIELD(ACTOR, m_pathRelativePos), false, 1, PROP_SETTER(ACTOR, SetPathRelativePos) },
    { "AlsoLineal", 0, 2, 0, PROP_FIELD(ACTOR, m_alsoLineal), false, 1, PROP_SETTER(ACTOR, SetAlsoLineal) },
    { "DisplayPath", 0, 2, 0, PROP_FIELD(ACTOR, m_displayPath), false, 1, PROP_SETTER(ACTOR, SetDisplayPath) },
    { "Resources", 0, 7, 0, PROP_FIELD(ACTOR, m_resources), false, ACTOR_MAX_RESOURCES, PROP_SETTER(ACTOR, SetResources) },
    { "ResourcesPos", 0, 11, 0, PROP_FIELD(ACTOR, m_resourcesPos), false, ACTOR_MAX_RESOURCES, PROP_SETTER(ACTOR, SetResourcesPos) },
    { "ResourcesAng", 0, 12, 0, PROP_FIELD(ACTOR, m_resourcesAng), false, ACTOR_MAX_RESOURCES, PROP_SETTER(ACTOR, SetResourcesAng) },
    { "ResourcesScale", 0, 11, 0, PROP_FIELD(ACTOR, m_resourcesScale), false, ACTOR_MAX_RESOURCES, PROP_SETTER(ACTOR, SetResourcesScale) },
    { "ResourcesOpacity", 0, 3, 0, PROP_FIELD(ACTOR, m_resourcesOpacity), false, ACTOR_MAX_RESOURCES, PROP_SETTER(ACTOR, SetResourcesOpacity) },
    { "ResourcesZSort", 0, 0, 0, PROP_FIELD(ACTOR, m_resourcesZSort), false, ACTOR_MAX_RESOURCES, PROP_SETTER(ACTOR, SetResourcesZSort) },
    { "ResourcesSpecific", 0, 0, 0, PROP_FIELD(ACTOR, m_resourcesSpecific), false, ACTOR_MAX_RESOURCES, PROP_SETTER(ACTOR, SetResourcesSpecific) },
    { "ResourcesBoneAttach", 0, 7, 0, PROP_FIELD(ACTOR, m_resourcesBoneAttach), false, ACTOR_MAX_RESOURCES, PROP_SETTER(ACTOR, SetResourcesBoneAttach) },
    { "Opacity", 0, 3, 0, PROP_FIELD(ACTOR, m_opacity), false, 1, PROP_SETTER(ACTOR, SetOpacity) },
    { "StartFirstAnim", 0, 2, 0, PROP_FIELD(ACTOR, m_startFirstAnim), false, 1, PROP_SETTER(ACTOR, SetStartFirstAnim) },
    { "OccludingBox", 0, 11, 0, PROP_FIELD(ACTOR, m_occludingBox), false, 1, PROP_SETTER(ACTOR, SetOccludingBox) },
    { "ZSort", 0, 0, 0, PROP_FIELD(ACTOR, m_zsort), false, 1, PROP_SETTER(ACTOR, SetZSort) },
    { "AnimSpeed", 0, 3, 0, PROP_FIELD(ACTOR, m_animSpeed), false, 1, PROP_SETTER(ACTOR, SetAnimSpeed) },
    { "LightMask", 0, 0, 0, PROP_FIELD(ACTOR, m_lightMask), false, 1, PROP_SETTER(ACTOR, SetLightMask) },
    { "CheckRenderFrustrum", 0, 2, 0, PROP_FIELD(ACTOR, m_checkRenderFrustrum), false, 1, PROP_SETTER(ACTOR, SetCheckRenderFrustrum) },
    { "RenderNotifications", 0, 2, 0, PROP_FIELD(ACTOR, m_renderNotifications), false, 1, PROP_SETTER(ACTOR, SetRenderNotifications) },
    { "ActorResourcesTemplate", 0, 7, 0, PROP_FIELD(ACTOR, m_actorResourcesTemplate), false, 1, PROP_SETTER(ACTOR, SetActorResourcesTemplate) },
    { 0, 0, 15, 0, 0, false },
};

bool operator!=(const V3 &a, const V3 &b)
{
    if (a.x == b.x && a.y == b.y && a.z == b.z)
        return false;
    return true;
}

void BOXCORNERS::Set(const V3 &size)
{
    float x = size.x * 0.5f;
    float y = size.y * 0.5f;
    float z = size.z * 0.5f;
    corners[0] = V3(-x, -y, z);
    corners[1] = V3(x, -y, z);
    corners[2] = V3(-x, -y, -z);
    corners[3] = V3(x, -y, -z);
    corners[4] = V3(-x, y, z);
    corners[5] = V3(x, y, z);
    corners[6] = V3(-x, y, -z);
    corners[7] = V3(x, y, -z);
}

void BOXCORNERS::Set(const float &width, const float &height, const float &depth)
{
    float x = width * 0.5f;
    float y = height * 0.5f;
    float z = depth * 0.5f;
    corners[0] = V3(-x, -y, z);
    corners[1] = V3(x, -y, z);
    corners[2] = V3(-x, -y, -z);
    corners[3] = V3(x, -y, -z);
    corners[4] = V3(-x, y, z);
    corners[5] = V3(x, y, z);
    corners[6] = V3(-x, y, -z);
    corners[7] = V3(x, y, -z);
}

void BOXCORNERS::Move(const V3 &offset)
{
    corners[0] += offset;
    corners[1] += offset;
    corners[2] += offset;
    corners[3] += offset;
    corners[4] += offset;
    corners[5] += offset;
    corners[6] += offset;
    corners[7] += offset;
}

void ACTOR::RegisterCommands()
{
    g_classParents[s_classId] = g_CMDBASEClassId;
    g_classCommands[s_classId] = s_commands;
    HashCommands(s_commands);
    RegisterClassName(s_commands, "ACTOR", "CMDBASE");
}

void ACTOR::SetActive(bool active)
{
    m_lastAngles = ANG(-1.0f, -1.0f, -1.0f);
    if (!active) {
        if (m_active) {
            Stop();
            m_active = false;
            g_pScene->RemoveScreenActor(this);
        }
    } else if (!m_active) {
        if (!m_placed) {
            Place();
            m_placed = true;
        }
        if (!m_scriptLoaded) {
            OnScriptLoaded();
            m_scriptLoaded = true;
        }
        Start();
        m_activations++;
        m_active = true;
        g_pScene->AddScreenActor(this);
    }
}

void ACTOR::GetBoxTop(V3 *a, V3 *b)
{
    *a = m_box.corners[2];
    *b = m_box.corners[5];
}

BOXCORNERS *ACTOR::GetBox()
{
    return &m_box;
}

void ACTOR::PlayResourceAnimation(int i, const char *animation, bool loop, float speed)
{
    BONEDMODELRESOURCE *copy = (BONEDMODELRESOURCE *)g_pResources->GetResource(m_copies[i]);
    if (copy)
        copy->PlayAnimation((void *)animation, loop, speed);
}

void ACTOR::SetResourceFrame(int i, const char *animation, float frame)
{
    BONEDMODELRESOURCE *copy = (BONEDMODELRESOURCE *)g_pResources->GetResource(m_copies[i]);
    if (copy)
        copy->SetAnimationFrame(animation, frame);
}

void ACTOR::BlendResourceAnimation(int i, const char *animation, float blend, bool loop, float speed)
{
    BONEDMODELRESOURCE *copy = (BONEDMODELRESOURCE *)g_pResources->GetResource(m_copies[i]);
    if (copy)
        copy->BlendAnimation(animation, blend, loop, speed);
}

void ACTOR::ReleaseResource(int i)
{
    OBJECT *copy = (OBJECT *)g_pResources->GetResource(m_copies[i]);
    if (copy)
        copy->Detach();
    m_copies[i].Invalidate();
}

void ACTOR::SetResourcesZSort(const int &i, const int &zsort)
{
    if ((unsigned)i < ACTOR_MAX_RESOURCES) {
        m_resourcesZSort[i] = zsort;
        OBJECT *copy = (OBJECT *)g_pResources->GetResource(m_copies[i]);
        if (copy)
            copy->SetRelLayer(zsort);
    } else if (g_pLog) {
        g_pLog->Error("ACTOR::SetResourcesZSort>Error: index %d out of range", i);
    }
}

void ACTOR::SetResourcesSpecific(const int &i, const int &specific)
{
    if ((unsigned)i < ACTOR_MAX_RESOURCES) {
        m_resourcesSpecific[i] = specific;
        OBJECT *copy = (OBJECT *)g_pResources->GetResource(m_copies[i]);
        if (copy)
            copy->SetFrame(specific);
    } else if (g_pLog) {
        g_pLog->Error("ACTOR::SetResourcesSpecific>Error: index %d out of range", i);
    }
}

void ACTOR::SetResourcesPos(const int &i, const V3 &position)
{
    if ((unsigned)i < ACTOR_MAX_RESOURCES) {
        m_resourcesPos[i] = position;
        OBJECT *copy = (OBJECT *)g_pResources->GetResource(m_copies[i]);
        if (copy)
            copy->SetOffsetV(position);
    } else if (g_pLog) {
        g_pLog->Error("ACTOR::SetResourcesPos>Error: index %d out of range", i);
    }
}

void ACTOR::SetResourcesAng(const int &i, const ANG &angles)
{
    if ((unsigned)i < ACTOR_MAX_RESOURCES) {
        m_resourcesAng[i] = angles;
        OBJECT *copy = (OBJECT *)g_pResources->GetResource(m_copies[i]);
        if (copy)
            copy->SetRelAngles(angles);
    } else if (g_pLog) {
        g_pLog->Error("ACTOR::SetResourcesAng>Error: index %d out of range", i);
    }
}

void ACTOR::SetResourcesScale(const int &i, const V3 &scale)
{
    if ((unsigned)i < ACTOR_MAX_RESOURCES) {
        m_resourcesScale[i] = scale;
        OBJECT *copy = (OBJECT *)g_pResources->GetResource(m_copies[i]);
        if (copy)
            copy->SetScaleFactorV(scale);
    } else if (g_pLog) {
        g_pLog->Error("ACTOR::SetResourcesScale>Error: index %d out of range", i);
    }
}

void ACTOR::SetResourcesOpacity(const int &i, const float &opacity)
{
    if ((unsigned)i < ACTOR_MAX_RESOURCES) {
        m_resourcesOpacity[i] = opacity;
        OBJECT *copy = (OBJECT *)g_pResources->GetResource(m_copies[i]);
        if (copy)
            copy->SetAlphaFactor(opacity);
    } else if (g_pLog) {
        g_pLog->Error("ACTOR::SetResourcesOpacity>Error: index %d out of range", i);
    }
}

V3 ACTOR::GetSize()
{
    return m_occludingBox * m_scale;
}

void ACTOR::SetOccludingBox(const V3 &size)
{
    m_occludingBox = size;
    if (m_node) {
        m_node->RemoveMember(this);
        m_node = 0;
    }
}

void ACTOR::UpdateOccludingBox()
{
    m_occludingBox.x = m_occludingBox.y = m_occludingBox.z = 0.0f;
    for (int i = 0; i < m_numResources; i++) {
        OBJECT *copy = (OBJECT *)g_pResources->GetResource(m_copies[i]);
        if (copy) {
            V3 size = copy->GetScaleFactor();
            if (m_occludingBox.x < size.x)
                m_occludingBox.x = size.x;
            if (m_occludingBox.y < size.y)
                m_occludingBox.y = size.y;
            if (m_occludingBox.z < size.z)
                m_occludingBox.z = size.z;
        }
    }
}

void ACTOR::SetZSort(const int &zsort)
{
    m_zsort = zsort;
}

void ACTOR::RotateToMove()
{
    V3 delta = m_position - m_lastPosition;
    if (m_moveWithScreenZ)
        delta.z -= ((ACTOR *)g_pCamera)->m_position.z - ((ACTOR *)g_pCamera)->m_lastPosition.z;
    if (m_moveWithScreenY)
        delta.y -= ((ACTOR *)g_pCamera)->m_position.y - ((ACTOR *)g_pCamera)->m_lastPosition.y;
    m_angles.PointAt(delta);
}

void ACTOR::SetPosition(const V3 &position)
{
    m_position = position;
}

void ACTOR::SetRotation(const MATRIX &rotation)
{
    SetAngles(ANG(rotation));
}

void ACTOR::Step()
{
    if (m_lastPosition != m_position && m_node && g_pScene->m_pvsType != 2) {
        m_node->RemoveMember(this);
        m_node = 0;
    }
    if (m_lastTime == g_pTimer->m_time) {
        if (g_pTimer->m_paused)
            Virtual103();
    } else {
        m_lastTime = g_pTimer->m_time;
        Update();
    }
}

ACTOR *ACTOR::Clone(ACTOR *target)
{
    if (!target)
        target = g_pScene->CreateActor(GetFactoryId());
    CopyProperties(target, g_NONEClassId);
    return target;
}

void ACTOR::OnScriptLoaded()
{
    m_scriptLoaded = true;
    SetAttachTo(m_attachTo);
}

bool ACTOR::UpdateAttachTag()
{
    if (m_behavior == ACTORBEHAVIOR_BONEATTACHED && m_attachBone.Length() > 0) {
        ACTOR *parent = (ACTOR *)g_pScene->GetActor(m_attachParent);
        if (parent) {
            WTACTOR *copy = (WTACTOR *)g_pResources->GetResource(parent->m_copies[m_attachBoneResId]);
            if (copy) {
                if (copy->GetManagerId() == g_pBONEDMODEL->m_id) {
                    if (m_attachTag) {
                        delete m_attachTag;
                        m_attachTag = 0;
                    }
                    m_attachTag = copy->GetTag(m_attachBone.c_str());
                    if (m_attachTag)
                        return true;
                }
                if (g_pLog)
                    g_pLog->Error("ERROR! ACTOR::SetAttachBone> bone not found attaching to resource %d of actor %s, bone %s, my name: %s",
                                  m_attachBoneResId, parent->m_name.c_str(), m_attachBone.c_str(), m_name.c_str());
            }
        }
    }
    return false;
}

void ACTOR::AttachTo(const RESHANDLE &parent)
{
    m_attachParent = parent;
    if (m_behavior != ACTORBEHAVIOR_BONEATTACHED)
        m_behavior = ACTORBEHAVIOR_ATTACHED;
}

void ACTOR::SetMoveWithScreenZ(const bool &move)
{
    m_moveWithScreenZ = move;
}

void ACTOR::SetFrozen(const bool &frozen)
{
    m_frozen = frozen;
}

RESOURCE *ACTOR::FindResourceCopy(int managerId)
{
    for (int i = 0; i < ACTOR_MAX_RESOURCES; i++) {
        RESOURCE *copy = g_pResources->GetResource(m_copies[i]);
        if (copy && copy->GetManagerId() == managerId)
            return copy;
    }
    return 0;
}

bool ACTOR::ChangeResourceMaterial(int i, const char *from, const char *to)
{
    // Never true: any index gets through.
    if (i < 0 && i > 15) {
        if (g_pLog)
            g_pLog->Error("ACTOR::ChangeResourceMaterial - Invalid resource ID");
        return false;
    }
    BONEDMODELRESOURCE *copy = (BONEDMODELRESOURCE *)g_pResources->GetResource(m_copies[i]);
    if (copy && copy->m_master) {
        if (!strcmp(copy->m_master->m_manager->GetTypeCode(), "BNMD"))
            return copy->ChangeMaterial(from, to);
        if (g_pLog)
            g_pLog->Error("ACTOR::ChangeResourceMaterial - Do not support resource type:%s", copy->m_master->m_manager->GetTypeCode());
    }
    return false;
}

V2 ACTOR::Link01()
{
    return V2(0.0f, 0.0f);
}

V2 ACTOR::Link03()
{
    return V2(1.0f, 1.0f);
}

float ACTOR::Link11()
{
    return m_opacity;
}

void ACTOR::Link12(float opacity)
{
    SetOpacity(opacity);
}

V3 ACTOR::Link05()
{
    return m_position;
}

void ACTOR::Link06(const V3 &position)
{
    SetPosition(position);
}

V3 ACTOR::Link09()
{
    return m_scale;
}

void ACTOR::Link10(const V3 &scale)
{
    SetScale(scale);
}

void ACTOR::Link13()
{
    m_removeRequested = true;
}

float ACTOR::Link14()
{
    return m_pathPos;
}

void *ACTOR::Virtual09(const char *name)
{
    if (name) {
        int i;
        if (sscanf(name, "res%d", &i) == 1) {
            if (i >= 0 && i < ACTOR_MAX_RESOURCES)
                return g_pResources->GetResource(m_copies[i]);
            if (!g_pLog)
                return 0;
            g_pLog->Error("ACTOR::FindObjectInMyContext - Invalid resource index");
        }
        if (g_pLog)
            g_pLog->Error("Nope:%s", name);
    }
    return 0;
}

bool ANG::IsZero()
{
    return GetEuler().IsZero();
}

BOXCORNERS::BOXCORNERS()
{
    Set(1.0f, 1.0f, 1.0f);
}

void ACTOR::UpdateVisibility()
{
    m_inView = false;
    if (m_activation == ACTORACTIVATION_USEOCCLUDINGBOX || m_deactivation == ACTORDEACTIVATION_USEOCCLUDINGBOX) {
        V3 size = GetSize();
        if (size.x * size.x + size.y * size.y + size.z * size.z < 0.25f) {
            UpdateOccludingBox();
            size = GetSize();
            if (size.x * size.x + size.y * size.y + size.z * size.z < 0.25f)
                m_occludingBox = V3(2.0f, 2.0f, 2.0f);
        }
        m_box.Set(GetSize());
        m_box.Move(GetPosition());
    }
    if (!m_node) {
        if (m_activation == ACTORACTIVATION_USEOCCLUDINGBOX || m_deactivation == ACTORDEACTIVATION_USEOCCLUDINGBOX)
            g_pScene->PVSAddMember(this);
    } else if (m_activation != ACTORACTIVATION_USEOCCLUDINGBOX && m_deactivation != ACTORDEACTIVATION_USEOCCLUDINGBOX) {
        m_node->RemoveMember(this);
        m_node = 0;
    }
}

void ACTOR::Stop()
{
    for (int i = 0; i < m_numResources; i++) {
        if (m_masters[i].IsValid()) {
            OBJECT *copy = (OBJECT *)g_pResources->GetResource(m_copies[i]);
            if (copy)
                copy->Detach();
            m_copies[i].Invalidate();
        }
    }
    if (m_attachTag) {
        delete m_attachTag;
        m_attachTag = 0;
    }
}

void ACTOR::ClearResources()
{
    for (int i = 0; i < ACTOR_MAX_RESOURCES; i++) {
        OBJECT *copy = (OBJECT *)g_pResources->GetResource(m_copies[i]);
        if (copy)
            copy->Detach();
        m_copies[i].Invalidate();
        SetResources(i, CSTRING());
        SetResourcesZSort(i, 0);
        SetResourcesPos(i, V3(0.0f, 0.0f, 0.0f));
        SetResourcesAng(i, ANG(0.0f, 0.0f, 0.0f));
        SetResourcesScale(i, V3(1.0f, 1.0f, 1.0f));
        SetResourcesSpecific(i, 0);
        SetResourcesBoneAttach(i, CSTRING());
    }
    m_numResources = 0;
}

void ACTOR::CreateResource(int i)
{
    if (m_copies[i].IsValid())
        ReleaseResource(i);
    OBJECT *copy = (OBJECT *)g_pResources->GetResource(m_masters[i]);
    if (copy) {
        m_copies[i] = copy->m_handle;
        AttachResource(i);
        copy->SetOffsetV(m_resourcesPos[i]);
        copy->SetRelAngles(m_resourcesAng[i]);
        copy->SetScaleFactorV(m_resourcesScale[i]);
        copy->SetRelLayer(m_resourcesZSort[i]);
        copy->SetAlphaFactor(m_resourcesOpacity[i]);
        copy->SetFrame(m_resourcesSpecific[i]);
        SetResourcesBoneAttach(i, m_resourcesBoneAttach[i]);
        copy->SetActive(true);
    }
}

void ACTOR::AttachResource(int i)
{
    OBJECT *copy = (OBJECT *)g_pResources->GetResource(m_copies[i]);
    if (copy) {
        copy->SetParent(m_handle);
        if (m_active)
            copy->SetActive(true);
    }
}

void ACTOR::SetDirection(const V3 &direction, const V3 &position)
{
    SetAngles(ANG(position, direction));
}

void ACTOR::SetAttachTo(const CSTRING &name)
{
    m_attachTo = name;
    if (name.Length() && m_placed) {
        WTACTOR *parent = g_pScene->FindActor(m_attachTo.c_str());
        if (parent) {
            AttachTo(parent->m_handle);
            return;
        }
        if (g_pLog)
            g_pLog->Error("ACTOR::SetAttachTo> can't attach to %s, my name: %s", m_attachTo.c_str(), m_name.c_str());
    }
}

void ACTOR::SetAttachBone(const CSTRING &bone)
{
    m_attachBone = bone;
    if (m_attachTag) {
        delete m_attachTag;
        m_attachTag = 0;
    }
    if (bone.Length() && m_placed)
        m_behavior = ACTORBEHAVIOR_BONEATTACHED;
}

void ACTOR::Link08(const V3 &position, const V3 &direction)
{
    SetAngles(ANG(position, direction));
}

void ACTOR::SetPathName(const CSTRING &name)
{
    if (m_pathName != name)
        m_pathName = name;
}

void ACTOR::SetActorResourcesTemplate(const CSTRING &name)
{
    m_actorResourcesTemplate = name;
    RESOURCEMASTER *master = g_pACTORRESOURCES->GetMaster(name.c_str());
    if (master)
        ApplyResourcesTemplate((ACTORRESOURCESMASTER *)master);
}

void ACTOR::ApplyResourcesTemplate(ACTORRESOURCESMASTER *master)
{
    for (int i = 0; i < ACTOR_MAX_RESOURCES; i++) {
        m_template = master;
        SetResources(i, master->m_resources[i].c_str());
        SetResourcesPos(i, master->m_pos[i]);
        SetResourcesAng(i, master->m_ang[i]);
        SetResourcesScale(i, master->m_scale[i]);
        SetResourcesOpacity(i, master->m_opacity[i]);
        SetResourcesZSort(i, master->m_zsort[i]);
        SetResourcesSpecific(i, master->m_specific[i]);
        SetResourcesBoneAttach(i, master->m_boneAttach[i].c_str());
    }
}

SRESULT ACTOR::Activate(CMDCALL *call)
{
    SetActive(true);
    return SRESULT(true);
}

SRESULT ACTOR::Deactivate(CMDCALL *call)
{
    SetActive(false);
    return SRESULT(true);
}

void ACTOR::Update()
{
    static V3 s_zero(0.0f, 0.0f, 0.0f);

    m_lastPosition = m_position;
    m_lastAngles = m_angles;
    m_lastVel = m_vel;
    m_behaviors.Update();
    switch (m_behavior) {
    case ACTORBEHAVIOR_LINEAL:
        SetPosition(m_position + g_pTimer->GetDt() * m_vel);
        m_vel += g_pTimer->GetDt() * m_aceleration;
        if (!m_angVel.IsZero()) {
            float x, y, z;
            m_angles.GetEuler(&x, &y, &z);
            V3 euler = V3(x, y, z) + m_angVel * g_pTimer->GetDt();
            SetAngles(ANG(euler.x, euler.y, euler.z));
        }
        m_angVel += g_pTimer->GetDt() * m_angAceleration;
        break;
    case ACTORBEHAVIOR_BONEATTACHED: {
        ACTOR *parent;
        if (m_attachParent.IsValid() && (parent = (ACTOR *)g_pScene->GetActor(m_attachParent)) != 0) {
            parent->Step();
            if (m_attachTag || UpdateAttachTag()) {
                ANG angles = m_attachTag->GetAngles();
                SetPosition(m_attachTag->GetPosition() + angles.Transform(m_attachPos));
                if (!m_attachAng.IsZero())
                    angles.Rotate(m_attachAng);
                SetAngles(angles);
            }
        } else {
            m_removeRequested = true;
        }
        break;
    }
    case ACTORBEHAVIOR_ATTACHED: {
        ACTOR *parent;
        if (m_attachParent.IsValid() && (parent = (ACTOR *)g_pScene->GetActor(m_attachParent)) != 0) {
            parent->Step();
            SetPosition(parent->LocalToWorld(m_attachPos));
            SetScale(parent->GetScale() * m_attachScale);
            SetOpacity(parent->m_opacity * m_attachOpacity);
            SetZSort(parent->m_zsort + m_attachZSort);
            if (!m_attachAng.IsZero()) {
                ANG angles(0.0f, 0.0f, 0.0f);
                angles.SetMatrix(m_attachAng.GetMatrix() * parent->m_angles.GetMatrix());
                SetAngles(angles);
            } else {
                SetAngles(parent->m_angles);
            }
        } else {
            m_removeRequested = true;
        }
        break;
    }
    case ACTORBEHAVIOR_CAMERAATTACHED:
        if (g_pCamera) {
            ((ACTOR *)g_pCamera)->Step();
            V3 position = m_attachPos * ((ACTOR *)g_pCamera)->GetAngles().GetMatrix();
            position += ((ACTOR *)g_pCamera)->m_position;
            SetPosition(position);
            ANG angles(0.0f, 0.0f, 0.0f);
            angles.SetMatrix(((ACTOR *)g_pCamera)->GetAngles().GetMatrix() * m_attachAng.GetMatrix());
            SetAngles(angles);
        }
        break;
    case ACTORBEHAVIOR_PATH:
        FollowPath();
        break;
    }
    if (m_rotateToMove)
        RotateToMove();
    if (m_fadeTime > 0.0f) {
        if (m_fadeTime <= 0.0f) {
            SetOpacity(m_fadeTarget);
        } else {
            float opacity = (m_fadeTarget - m_opacity) * (g_pTimer->m_dt / m_fadeTime) + m_opacity;
            if (opacity < 0.0f)
                opacity = 0.0f;
            else if (opacity > 1.0f)
                opacity = 1.0f;
            SetOpacity(opacity);
        }
        m_fadeTime -= g_pTimer->m_dt;
    }
    UpdateBehaviors(g_pTimer->m_dt);
}

SRESULT ACTOR::ClearBehaviors(CMDCALL *call)
{
    m_behaviors.Clear();
    return SRESULT(true);
}

SRESULT ACTOR::PlayAnimation(CMDCALL *call)
{
    PlayResourceAnimation(atol(call->tokens->GetToken(1)), call->tokens->GetToken(2), atol(call->tokens->GetToken(3)) != 0, 1.0f);
    return SRESULT(true);
}

SRESULT ACTOR::SetAnimationFrame(CMDCALL *call)
{
    SetResourceFrame(atol(call->tokens->GetToken(1)), call->tokens->GetToken(2), (float)atof(call->tokens->GetToken(3)));
    return SRESULT(true);
}

SRESULT ACTOR::StopAnimation(CMDCALL *call)
{
    BONEDMODELRESOURCE *copy = (BONEDMODELRESOURCE *)g_pResources->GetResource(m_copies[atol(call->tokens->GetToken(1))]);
    if (copy) {
        copy->StopAnimation();
        return SRESULT(true);
    }
    return SRESULT(false);
}

SRESULT ACTOR::BlendAnimation(CMDCALL *call)
{
    BlendResourceAnimation(atol(call->tokens->GetToken(1)), call->tokens->GetToken(2), (float)atof(call->tokens->GetToken(3)),
                           atol(call->tokens->GetToken(4)) != 0, 1.0f);
    return SRESULT(true);
}

inline void ACTOR::PauseResourceAnimation(int i, bool pause)
{
    BONEDMODELRESOURCE *copy = (BONEDMODELRESOURCE *)g_pResources->GetResource(m_copies[i]);
    if (copy)
        copy->PauseAnimation(pause);
}

SRESULT ACTOR::PauseAnimation(CMDCALL *call)
{
    if (call->tokens->m_count == 2)
        PauseResourceAnimation(atol(call->tokens->GetToken(1)), true);
    else
        PauseResourceAnimation(atol(call->tokens->GetToken(1)), atol(call->tokens->GetToken(2)) != 0);
    return SRESULT(true);
}

SRESULT ACTOR::ResetResources(CMDCALL *call)
{
    ClearResources();
    return SRESULT(true);
}

void ACTOR::SetResources(const int &i, const CSTRING &name)
{
    if ((unsigned)i < ACTOR_MAX_RESOURCES) {
        ReleaseResource(i);
        m_resources[i] = name;
        m_masters[i] = g_pResources->GetHandle(name.c_str());
        if (m_masters[i].IsValid()) {
            if ((unsigned)m_numResources < (unsigned)(i + 1))
                m_numResources = i + 1;
            if (m_active)
                CreateResource(i);
        }
        m_c1c = -1.0f;
        return;
    }
    if (g_pLog)
        g_pLog->Error("ACTOR::SetResources>Error: index %d out of range", i);
}

SRESULT ACTOR::ActivateResource(CMDCALL *call)
{
    CreateResource(atol(call->tokens->GetToken(1)));
    return SRESULT(true);
}

SRESULT ACTOR::DeactivateResource(CMDCALL *call)
{
    ReleaseResource(atol(call->tokens->GetToken(1)));
    return SRESULT(true);
}

inline void ACTOR::StartFade(float from, float to, float time)
{
    m_fadeTime = time;
    SetOpacity(from);
    m_fadeTarget = to;
}

SRESULT ACTOR::Fade(CMDCALL *call)
{
    StartFade((float)atof(call->tokens->GetToken(1)), (float)atof(call->tokens->GetToken(2)),
              (float)atof(call->tokens->GetToken(3)));
    return SRESULT(true);
}

void ACTOR::SetAngles(const ANG &angles)
{
    if (m_angles != angles) {
        m_angles = angles;
        m_anglesChanged = true;
    }
}

MATRIX ACTOR::GetMatrix()
{
    return m_angles.GetMatrix();
}

MATRIX ACTOR::GetInverseMatrix()
{
    MATRIX m = m_angles.GetMatrix();
    m.Invert();
    return m;
}

void ACTOR::SetAttachAng(const ANG &angles)
{
    if (m_attachAng != angles)
        m_attachAng = angles;
}

V3 ACTOR::WorldToLocal(V3 point)
{
    V3 p = point - m_position;
    p = p.Transform(GetInverseMatrix());
    p.x /= m_scale.x;
    p.y /= m_scale.y;
    p.z /= m_scale.z;
    return p;
}

V3 ACTOR::LocalToWorld(V3 point)
{
    V3 p = point * m_scale;
    p = p.Transform(m_angles.GetMatrix());
    p += m_position;
    return p;
}

V3 ACTOR::LocalToWorldDir(V3 direction)
{
    V3 p = direction * m_scale;
    p = p.Transform(m_angles.GetMatrix());
    return p;
}

SRESULT ACTOR::Remove(CMDCALL *call)
{
    m_removeRequested = true;
    return SRESULT(true);
}

void ACTOR::Dump()
{
    if (g_pLog)
        g_pLog->Value("Angles: %f %f %f", m_angles.GetEulerX(), m_angles.GetEulerY(), m_angles.GetEulerZ());
}

SRESULT ACTOR::SetSpeed(CMDCALL *call)
{
    float speed = (float)atof(call->tokens->GetToken(1));
    SetVel(GetAngles().GetDirection() * speed);
    return SRESULT(true);
}

SRESULT ACTOR::ChangeResourceMaterialCommand(CMDCALL *call)
{
    return SRESULT(ChangeResourceMaterial(atol(call->tokens->GetToken(1)), call->tokens->GetToken(2), call->tokens->GetToken(3)));
}

void ACTOR::Link07(V3 *direction, V3 *up)
{
    if (direction)
        *direction = m_angles.GetDirection();
    if (up)
        *up = m_angles.GetUpPoint();
}

void ACTOR::FollowPath()
{
    if (!m_pathMaster) {
        m_pathMaster = (PATHMASTER *)g_pPATH->GetMaster(m_pathName.c_str());
        if (!m_pathMaster) {
            if (g_pLog)
                g_pLog->Error("%s error loading path %s!", m_name.c_str(), m_pathName.c_str());
            return;
        }
    }
    if (m_pathBehavior != ACTORPATH_PATHFINDER && m_behavior == ACTORBEHAVIOR_PATH) {
        m_lastVel = m_vel;
        float dt = g_pTimer->m_dt;
        float speed = m_vel.Length();
        V3 position(2.0f, 2.0f, 2.0f);
        V3 direction(0.0f, 0.0f, 0.0f);
        V3 up(0.0f, 0.0f, 0.0f);
        float radius;
        float angle;
        m_lastPathPos = m_pathPos;
        m_pathPos += dt / m_pathMaster->m_totalLength * speed;
        if (!m_loop && m_pathPos > 1.0f)
            m_pathPos = 1.0f;
        m_pathMaster->RunCommands(m_lastPathPos, m_pathPos, m_handle);
        m_pathMaster->GetPoint(m_pathPos, &position, &direction, &up, &radius, &angle);
        if (m_pathRelativePos) {
            direction += position;
            up += position;
            position = position * m_scale * GetMatrix();
            direction = direction * m_scale * GetMatrix() - position;
            up = up * m_scale * GetMatrix() - position;
        }
        if (m_pathBehavior == ACTORPATH_FOLLOW) {
            m_lastPosition = m_position;
            m_position = position;
            V3 vectorUp = m_vectorUp;
            vectorUp.Normalize();
            V3 vectorForward = m_vectorForward;
            vectorForward.Normalize();
            vectorUp = vectorUp - vectorForward * (vectorUp.x * vectorForward.x + vectorUp.y * vectorForward.y + vectorUp.z * vectorForward.z);
            MATRIX base;
            base.LookAt(vectorUp, vectorForward);
            direction.Normalize();
            up.Normalize();
            up = up - direction * (up.x * direction.x + up.y * direction.y + up.z * direction.z);
            up.Normalize();
            MATRIX path;
            path.LookAt(up, direction);
            SetRotation(base * path);
            m_vel = direction * speed;
        }
        if (m_pathBehavior == ACTORPATH_FLOW) {
            direction.Normalize();
            up.Normalize();
            up = up - direction * (up.x * direction.x + up.y * direction.y + up.z * direction.z);
            up.Normalize();
            angle = DEG2RAD(angle + m_flowAngle);
            V3 offset = up.Rotate(direction, angle);
            radius *= m_flowRadius;
            m_lastPosition = m_position;
            m_position = offset * radius + position;
            V3 delta = m_position - m_lastPosition;
            delta.Normalize();
            if (delta.Length() == 0.0f)
                m_vel = direction * speed;
            else
                m_vel = delta * speed;
        }
        if (m_alsoLineal) {
            speed += m_aceleration.Length() * dt;
            if (m_vel == g_zero)
                m_vel = V3(1.0f, 1.0f, 1.0f);
            else
                m_vel.Normalize();
            m_vel = m_vel * speed;
            if (!m_angVel.IsZero()) {
                float x, y, z;
                m_angles.GetEuler(&x, &y, &z);
                V3 euler = V3(x, y, z) + m_angVel * g_pTimer->GetDt();
            SetAngles(ANG(euler.x, euler.y, euler.z));
            }
            m_angVel += g_pTimer->GetDt() * m_angAceleration;
        }
    }
}

SRESULT ACTOR::CreateFX(CMDCALL *call)
{
    FIXEDSTRING<0x200> name(call->tokens->GetToken(1));
    g_pResources->CreateFromTemplate(name, m_position, m_zsort + 5);
    return SRESULT(true);
}

SRESULT ACTOR::CreateTagFX(CMDCALL *call)
{
    WTACTOR *copy = (WTACTOR *)g_pResources->GetResource(m_copies[atol(call->tokens->GetToken(3))]);
    if (copy) {
        TAG *tag = copy->GetTag(call->tokens->GetToken(2));
        if (tag) {
            FIXEDSTRING<0x200> name(call->tokens->GetToken(1));
            g_pResources->CreateFromTemplate(name, tag->GetPosition(), m_zsort + 5);
            delete tag;
            return SRESULT(true);
        }
        if (g_pLog)
            g_pLog->Error("ERROR ACTOR::CreateTagFx> Tag not found");
    }
    return SRESULT(false);
}

void ACTOR::SetName(const CSTRING &name)
{
    if (name.Length() > 0 && g_pScene) {
        FIXEDSTRING<0x200> newName;
        newName.Set(name.c_str());
        g_pScene->RenameActor(m_handle, newName);
        m_name = m_path.c_str();
    }
}

void ACTOR::Start()
{
    SetAttachBone(m_attachBone);
    for (int i = 0; i < m_numResources; i++) {
        if (m_masters[i].IsValid())
            CreateResource(i);
    }
    g_pCmdMgr->Run(STRING(m_onActivate.c_str()), m_handle);
}

void ACTOR::SetResourcesBoneAttach(const int &i, const CSTRING &bone)
{
    if ((unsigned)i < ACTOR_MAX_RESOURCES) {
        m_resourcesBoneAttach[i] = bone;
        OBJECT *copy = (OBJECT *)g_pResources->GetResource(m_copies[i]);
        if (copy) {
            if (m_resourcesBoneAttach[i].Length() > 0) {
                TOKENS tokens;
                tokens.Split(STRING(bone.c_str()), ",");
                int resource = atoi(tokens.GetToken(0));
                if (resource < 0)
                    resource = 0;
                else if (resource > 15)
                    resource = 15;
                WTACTOR *target = (WTACTOR *)g_pResources->GetResource(m_copies[resource]);
                const char *tag;
                if (!target) {
                    if (g_pLog)
                        g_pLog->Error("ERROR! tried to attach to a bone of a not defined resource (index %d)!", resource);
                    tag = 0;
                    target = 0;
                } else if (!strlen(tokens.GetToken(1))) {
                    if (g_pLog)
                        g_pLog->Error("ERROR! tried to attach to a bone but not specified a name!");
                    tag = 0;
                    target = 0;
                } else {
                    tag = tokens.GetToken(1);
                }
                copy->AttachTo(target, tag);
            } else {
                copy->AttachTo(0, 0);
            }
        }
    } else if (g_pLog) {
        g_pLog->Error("ACTOR::SetResourcesBoneAttach>Error: index %d out of range", i);
    }
}

RESHANDLE ACTOR::MakeFX(const char *name, RESHANDLE parent, V3 position, ANG angles, FIXEDSTRING<0x200> bone)
{
    if (strlen(name) && parent.IsValid()) {
        OBJECT *fx = (OBJECT *)g_pFX->GetResource(name);
        if (fx) {
            fx->SetOffsetV(position);
            fx->SetRelAngles(angles);
            fx->SetAlphaFactor(1.0f);
            fx->SetScaleFactor(1.0f, 1.0f, 1.0f);
            fx->SetFrame(0);
            ACTOR *actor = (ACTOR *)g_pScene->GetActor(parent);
            if (actor)
                fx->SetRelLayer(actor->m_zsort + 5);
            else
                fx->SetLayer(0);
            fx->SetParent(parent);
            if ((int)strlen(bone) > 0) {
                TOKENS tokens;
                tokens.Split(STRING(bone), ",");
                int resource = atol(tokens.GetToken(0));
                if (resource < 0)
                    resource = 0;
                else if (resource > 15)
                    resource = 15;
                WTACTOR *target = (WTACTOR *)g_pResources->GetResource(m_copies[resource]);
                if (!target) {
                    if (g_pLog)
                        g_pLog->Error("ERROR! tried to attach to a bone of a not defined resource (index %d)!", resource);
                    fx->AttachTo(0, 0);
                    return RESHANDLE();
                }
                if (!strlen(tokens.GetToken(1))) {
                    if (g_pLog)
                        g_pLog->Error("ERROR! tried to attach to a bone but not specified a name!");
                    fx->AttachTo(0, 0);
                    return RESHANDLE();
                }
                fx->AttachTo(target, tokens.GetToken(1));
            }
            fx->SetActive(true);
            return fx->m_handle;
        }
    }
    return RESHANDLE();
}

SRESULT ACTOR::SetLight(CMDCALL *call)
{
    int mask = 0;
    TOKENS tokens;
    tokens.Split(STRING(call->tokens->GetToken(1)), "+");
    for (int i = 0; i < tokens.m_count; i++) {
        for (int j = 0; j < RESOURCES_MAX_GROUPS; j++) {
            if (!_strnicmp(g_pResources->m_groups[j].c_str(), tokens.GetToken(i), 0x200))
                mask += 1 << j;
        }
    }
    SetLightMask(mask);
    return SRESULT(true);
}

ACTOR::~ACTOR()
{
    if (g_pScene) {
        for (int i = 0; i < ACTOR_MAX_RESOURCES; i++) {
            OBJECT *copy = (OBJECT *)g_pResources->GetResource(m_copies[i]);
            if (copy)
                copy->Detach();
            m_copies[i].Invalidate();
        }
        if (m_active) {
            if (g_pLog)
                g_pLog->Error("ERROR!! - deleting an active object: %s!! must have called activate(false) before!", m_name.c_str());
            SetActive(false);
        }
        if (m_attachTag) {
            delete m_attachTag;
            m_attachTag = 0;
        }
        if (m_node)
            m_node->RemoveMember(this);
    }
}

SRESULT ACTOR::AttachFX(CMDCALL *call)
{
    V3 position;
    V3 angles;
    sscanf(call->tokens->GetToken(2), "%f %f %f", &position.x, &position.y, &position.z);
    sscanf(call->tokens->GetToken(3), "%f %f %f", &angles.x, &angles.y, &angles.z);
    FIXEDSTRING<0x200> name(call->tokens->GetToken(1));
    FIXEDSTRING<0x200> bone(call->tokens->GetToken(4));
    MakeFX(name, m_handle, position, ANG(angles.x, angles.y, angles.z), bone);
    return SRESULT(true);
}

ACTOR::ACTOR()
{
    m_placed = false;
    m_scriptLoaded = false;
    m_screenIndex = 0;
    m_active = false;
    m_9a1 = false;
    m_lastTime = 0.0f;
    m_activations = 0;
    m_anglesChanged = true;
    m_lastPosition.x = m_lastPosition.y = m_lastPosition.z = 0.0f;
    m_lastAngles.euler.x = m_lastAngles.euler.y = m_lastAngles.euler.z = 0.0f;
    m_lastAngles.flags = ANG::MATRIX_DIRTY;
    m_lastVel.x = m_lastVel.y = m_lastVel.z = 0.0f;
    m_origin = m_position;
    m_position = V3(0.0f, 0.0f, 0.0f);
    m_angles = ANG(0.0f, 0.0f, 0.0f);
    m_scale = V3(1.0f, 1.0f, 1.0f);
    m_attachTag = 0;
    m_template = 0;
    m_numResources = 0;
    m_c1c = -1.0f;
    m_pathMaster = 0;
    m_c24 = 0;
    m_pathPos = 0.0f;
    m_collectContacts = false;
    m_fadeTime = 0.0f;
    m_fadeTarget = 0.0f;
    m_removeRequested = false;

    m_isTemplate = false;
    m_frozen = false;
    m_comments = "";
    m_colliderColor = RGB(0, 0, 0);
    m_colliderColorOverride = false;
    m_showCollider = true;
    m_attachTo = "";
    m_attachBone = "";
    m_attachPos = V3(0.0f, 0.0f, 0.0f);
    m_attachAng = ANG(0.0f, 0.0f, 0.0f);
    m_attachScale = V3(1.0f, 1.0f, 1.0f);
    m_attachBoneResId = 0;
    m_attachOpacity = 1.0f;
    m_attachZSort = 0;
    m_activation = ACTORACTIVATION_USEOCCLUDINGBOX;
    m_deactivation = ACTORDEACTIVATION_USEOCCLUDINGBOX;
    m_onActivate = "";
    m_vel = V3(0.0f, 0.0f, 0.0f);
    m_angVel = V3(0.0f, 0.0f, 0.0f);
    m_angAceleration = V3(0.0f, 0.0f, 0.0f);
    m_aceleration = V3(0.0f, 0.0f, 0.0f);
    m_testOnActivate = false;
    m_onActivateRunInEditor = false;
    m_behavior = ACTORBEHAVIOR_STATIC;
    m_panSounds = true;
    m_moveWithScreenZ = false;
    m_moveWithScreenY = false;
    m_previewBehavior = false;
    m_rotateToMove = false;
    m_forcedCamDist = -1.0f;
    m_forceInScreen = false;
    m_pathName = "";
    m_vectorUp = V3(0.0f, 1.0f, 0.0f);
    m_vectorForward = V3(0.0f, 0.0f, 1.0f);
    m_pathBehavior = ACTORPATH_FOLLOW;
    m_testPath = false;
    m_loop = true;
    m_flowAngle = 0.0f;
    m_flowRadius = 1.0f;
    m_pathRelativePos = false;
    m_alsoLineal = true;
    m_displayPath = false;
    for (g_i = 0; g_i < ACTOR_MAX_RESOURCES; g_i++)
        m_resources[g_i] = "";
    for (g_i = 0; g_i < ACTOR_MAX_RESOURCES; g_i++)
        m_resourcesPos[g_i] = V3(0.0f, 0.0f, 0.0f);
    for (g_i = 0; g_i < ACTOR_MAX_RESOURCES; g_i++)
        m_resourcesAng[g_i] = ANG(0.0f, 0.0f, 0.0f);
    for (g_i = 0; g_i < ACTOR_MAX_RESOURCES; g_i++)
        m_resourcesScale[g_i] = V3(1.0f, 1.0f, 1.0f);
    for (g_i = 0; g_i < ACTOR_MAX_RESOURCES; g_i++)
        m_resourcesOpacity[g_i] = 1.0f;
    for (g_i = 0; g_i < ACTOR_MAX_RESOURCES; g_i++)
        m_resourcesZSort[g_i] = 0;
    for (g_i = 0; g_i < ACTOR_MAX_RESOURCES; g_i++)
        m_resourcesSpecific[g_i] = 0;
    for (g_i = 0; g_i < ACTOR_MAX_RESOURCES; g_i++)
        m_resourcesBoneAttach[g_i] = "";
    m_occludingBox = V3(-1.0f, -1.0f, -1.0f);
    m_opacity = 1.0f;
    m_startFirstAnim = true;
    m_zsort = 0;
    m_animSpeed = 1.0f;
    m_lightMask = 1;
    m_checkRenderFrustrum = true;
    m_renderNotifications = false;
    m_actorResourcesTemplate = "";
    m_node = 0;
    m_next = 0;
    m_prev = 0;
    m_inView = false;
}

void ACTOR::OnContact(CONTACT *contact)
{
    if (m_collectContacts) {
        if (m_contacts.size() <= 100)
            m_contacts.push_back(*contact);
        else if (g_pLog)
            g_pLog->Error("ERROR!!! MAX COLLISIONS DETECTED!!! ARE YOU CLEANING THE LIST!!??");
    }
}

SRESULT ACTOR::AddBehavior(CMDCALL *call)
{
    BEHAVIORENTRY *entry = FindBehavior(call->tokens->GetToken(1));
    if (entry) {
        m_behaviors.Add(entry->create(m_handle, call));
        return SRESULT(true);
    }
    if (g_pLog)
        g_pLog->Error("Behavior not found '%s' ...", call->tokens->GetToken(1));
    return SRESULT(false);
}
