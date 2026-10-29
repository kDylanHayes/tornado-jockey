#include <stdlib.h>
#include <string.h>

#include "Tongas2/ACTOR.h"
#include "Tongas2/CAMERA.h"
#include "Tongas2/CMDMGR.h"
#include "Tongas2/CPRIMITIVE3.h"
#include "Tongas2/GAME.h"
#include "Tongas2/LOG.h"
#include "Tongas2/PVS.h"
#include "Tongas2/SCENE.h"
#include "Tongas2/SPAWNER.h"
#include "Tongas2/TIMER.h"
#include "Tongas2/TRIGGER.h"

#define CMD_FUNC(CLASS, method) (CMDFUNC)(SRESULT (CLASS::*)(TOKENS **))(SRESULT (CLASS::*)(CMDCALL *))&CLASS::method

SCENE *CreateScene();

SCENE *(*g_pfnCreateScene)() = CreateScene;

int g_rootFactoryId = -1;
int ACTOR::s_factoryId = -1;
int CAMERA::s_factoryId = -1;
int SPAWNER::s_factoryId = -1;
int TRIGGER::s_factoryId = -1;

int g_factoryClassIds[MAX_ACTOR_FACTORIES];
int g_classFactoryIds[0x200];
ACTOR *(*g_factoryCreators[MAX_ACTOR_FACTORIES])();
SCENE *g_pScene;
char g_factoryNames[MAX_ACTOR_FACTORIES][30];
int g_factoryFlags[MAX_ACTOR_FACTORIES];
int g_numFactories;
// Run by Init, and after the actors are stepped each frame.
TIMERHOOKS g_actorStartHooks;
TIMERHOOKS g_actorUpdateHooks;

CMDENTRY SCENE::s_commands[] = {
    { "testCmd", 0, 1, 1, CMD_FUNC(SCENE, TestCmd) },
    { "cols", 0, 1, 1, CMD_FUNC(SCENE, ToggleCollisions) },
    { "CreatePVS", 0, 2, 2, CMD_FUNC(SCENE, CreatePVS) },
    { "DumpPVS", 0, 0, 0, CMD_FUNC(SCENE, DumpPVS) },
    { 0, 0, 0, 0, 0 },
};

PROPENTRY SCENE::s_properties[] = {
    { "FogColor", 0, 13, 0, PROP_FIELD(SCENE, m_fogColor), false, 1, PROP_SETTER(SCENE, SetFogColor) },
    { 0, 0, 15, 0, 0, false },
};

static const char s_pvsTypes[][30] = {
    "None",
    "Octree",
    "OneNode",
    "",
};

#define ACTOR_CREATOR(CLASS) \
    ACTOR *Create##CLASS() \
    { \
        return new CLASS; \
    }

ACTOR_CREATOR(ACTOR)
ACTOR_CREATOR(CAMERA)
ACTOR_CREATOR(TRIGGER)
ACTOR_CREATOR(SPAWNER)

void SCENE::RegisterCommands()
{
    g_classParents[s_classId] = g_CMDBASEClassId;
    g_classCommands[s_classId] = s_commands;
    HashCommands(s_commands);
    RegisterClassName(s_commands, "SCENE", "CMDBASE");
}

#define REGISTER_ACTOR(CLASS, flags) \
    g_factoryCreators[g_numFactories] = Create##CLASS; \
    g_factoryFlags[g_numFactories] = flags; \
    g_factoryClassIds[g_numFactories] = CLASS::s_classId; \
    g_classFactoryIds[CLASS::s_classId] = g_numFactories; \
    strncpy(g_factoryNames[g_numFactories], #CLASS, 30); \
    CLASS::s_factoryId = g_numFactories++;

void SCENE::LoadActorsInformation()
{
    g_numFactories = 0;
    REGISTER_ACTOR(ACTOR, 1)
    REGISTER_ACTOR(CAMERA, 0)
    REGISTER_ACTOR(SPAWNER, 1)
    REGISTER_ACTOR(TRIGGER, 1)
    if (g_numFactories >= MAX_ACTOR_FACTORIES && g_pLog)
        g_pLog->Error("ERROR!! SCENE::LoadActorsInformation>FATAL ERROR, max number of supported actor types exceeded");
}

bool SCENE::Init()
{
    if (m_initialized) {
        if (g_pLog)
            g_pLog->Error("already initted!!!!");
        return false;
    }
    for (TIMERHOOK *hook = g_actorStartHooks.head; hook; hook = hook->next)
        hook->func();
    m_initialized = true;
    return true;
}

void SCENE::PVSDump()
{
    FRUSTUM *frustum = ((CAMERA *)g_pCamera)->m_lens->GetFrustum();
    switch (m_pvsType) {
    case PVS_OCTREE:
        m_octree->Dump(frustum);
        break;
    case PVS_ONENODE:
        m_oneNode->Dump(frustum);
        break;
    default:
        if (g_pLog)
            g_pLog->Error("SCENE::PVSDump - PVS not supported");
        break;
    }
}

void SCENE::LoadMap(const char *name)
{
    g_pCmdMgr->LoadMap(name);
}

int SCENE::AddScreenActor(ACTOR *actor)
{
    if (!actor)
        return -1;
    int slot;
    if (m_numFreeScreenSlots > 0) {
        m_numFreeScreenSlots--;
        slot = m_freeScreenSlots[m_numFreeScreenSlots];
    } else {
        slot = m_numScreenSlots++;
    }
    if (slot != -1) {
        m_screenActors[slot] = actor;
        actor->m_screenIndex = slot;
    }
    return slot;
}

void SCENE::RemoveScreenActor(ACTOR *actor)
{
    if (actor && actor->m_screenIndex > -1 && actor->m_screenIndex < 1500) {
        m_freeScreenSlots[m_numFreeScreenSlots] = actor->m_screenIndex;
        m_numFreeScreenSlots++;
        m_screenActors[actor->m_screenIndex] = 0;
        actor->m_screenIndex = -1;
    }
}

void SCENE::ToggleHelpers()
{
    ((CPRIMITIVE3MANAGER *)g_pCPRIMITIVE3)->ToggleHelpers();
}

bool SCENE::CanUpdate(ACTOR *actor)
{
    return g_pGame && g_pGame->Virtual25();
}

void SCENE::SetFogColor(const RGB &color)
{
    m_fogColor = color;
}

void SCENE::DestroyPVS()
{
    m_pvsType = PVS_NONE;
    if (m_oneNode) {
        delete m_oneNode;
        m_oneNode = 0;
    }
    if (m_octree) {
        delete m_octree;
        m_octree = 0;
    }
}

void SCENE::CreatePVS(const char *type, const char *args)
{
    DestroyPVS();
    for (int i = 0; i < 3; i++) {
        if (!_stricmp(s_pvsTypes[i], type)) {
            switch (i) {
            case PVS_ONENODE:
                m_oneNode = new ONENODE;
                m_pvsType = PVS_ONENODE;
                break;
            case PVS_OCTREE: {
                V3 min(0.0f, 0.0f, 0.0f);
                V3 max(0.0f, 0.0f, 0.0f);
                int depth;
                sscanf(args, "(%f,%f,%f) (%f,%f,%f) %d", &min.x, &min.y, &min.z, &max.x, &max.y, &max.z, &depth);
                m_octree = new OCTREE(min, max, depth);
                m_pvsType = PVS_OCTREE;
                break;
            }
            }
        }
    }
}

void SCENE::PVSAddMember(ACTOR *actor)
{
    if (m_pvsType == PVS_NONE)
        CreatePVS("OneNode", "");
    switch (m_pvsType) {
    case PVS_OCTREE:
        m_octree->AddMember(actor);
        break;
    case PVS_ONENODE:
        m_oneNode->AddMember(actor);
        break;
    default:
        if (g_pLog)
            g_pLog->Error("SCENE::PVSAddMember - PVS not supported");
        break;
    }
}

void SCENE::OnScriptLoaded()
{
    int i;
    ACTOR *actor = m_actors.GetFirst(&i);
    while (i != -1) {
        if (actor)
            actor->Virtual106();
        actor = m_actors.GetNext(&i);
    }
}

void SCENE::DumpActors(int which)
{
    if (which == -1) {
        int i;
        ACTOR *actor = m_actors.GetFirst(&i);
        while (i != -1) {
            if (actor && actor->m_9a0) {
                if (g_pLog)
                    actor->GetFactoryId();
                actor->Virtual100();
            }
            actor = m_actors.GetNext(&i);
        }
    } else {
        for (int i = 0; i < m_numScreenSlots; i++) {
            ACTOR *actor = m_screenActors[i];
            if (actor) {
                if (g_pLog)
                    actor->GetFactoryId();
                actor->Virtual100();
            }
        }
    }
}

ACTOR *SCENE::PickActor(float x, float y, bool activeOnly, int classId)
{
    float nearestDistance = 1000.0f;
    ACTOR *nearest = 0;
    int i;
    ACTOR *actor = m_actors.GetFirst(&i);
    while (i != -1) {
        if (actor && (actor->m_9a0 && !actor->m_91 || !activeOnly)) {
            if (FindCommonClass(actor->GetFactoryId(), classId) == classId) {
                V2 screen(0.0f, 0.0f);
                ((CAMERA *)g_pCamera)->WorldToScreen(actor->m_position, &screen);
                float dx = x - screen.x;
                float dy = y - screen.y;
                float distance = dx * dx + dy * dy;
                if (distance < nearestDistance) {
                    nearest = actor;
                    nearestDistance = distance;
                }
            }
        }
        actor = m_actors.GetNext(&i);
    }
    return nearest;
}

SCENE::SCENE()
{
    m_initialized = false;
    m_29.Set("");
    m_229 = false;
    m_22a = false;
    m_updating = false;
    m_numPendingRemoves = 0;
    m_pvsType = PVS_NONE;
    m_oneNode = 0;
    m_octree = 0;
    m_numFreeScreenSlots = 0;
    m_numScreenSlots = 0;
    int i = 0;
    while (i < 1500) {
        m_freeScreenSlots[i] = 0;
        m_screenActors[i] = 0;
        i++;
    }
    m_fogColor = RGB(64, 64, 64);
    SetName("scene");
    TIMERHOOK *hook = new TIMERHOOK;
    hook->func = LoadActorsInformation;
    hook->next = g_actorStartHooks.head;
    if (g_actorStartHooks.head)
        g_actorStartHooks.head->prev = hook;
    g_actorStartHooks.head = hook;
    if (!g_actorStartHooks.tail)
        g_actorStartHooks.tail = hook;
}

SRESULT SCENE::DumpPVS(CMDCALL *call)
{
    PVSDump();
    return SRESULT(true);
}

SRESULT SCENE::CreatePVS(CMDCALL *call)
{
    CreatePVS(call->tokens->GetToken(1), call->tokens->GetToken(2));
    return SRESULT(true);
}

void SCENE::Reset()
{
    int i;
    ACTOR *actor = m_actors.GetFirst(&i);
    while (i != -1) {
        if (actor) {
            actor->Virtual89(false);
            m_actors.Remove(actor->m_handle);
        }
        actor = m_actors.GetNext(&i);
    }
    m_actors.Clear();
    m_numPendingRemoves = 0;
    m_numFreeScreenSlots = 0;
    m_numScreenSlots = 0;
    for (i = 0; i < 1500; i++) {
        m_freeScreenSlots[i] = 0;
        m_screenActors[i] = 0;
    }
    switch (m_pvsType) {
    case PVS_ONENODE:
        m_oneNode->Clear();
        break;
    case PVS_OCTREE:
        m_octree->Clear();
    default:
        if (g_pLog)
            g_pLog->Error("SCENE::PVSAddMember - PVS not supported");
        break;
    }
}

void SCENE::RemoveAll()
{
    int i;
    ACTOR *actor = m_actors.GetFirst(&i);
    while (i != -1) {
        if (actor) {
            actor->Virtual89(false);
            m_actors.Remove(actor->m_handle);
        }
        actor = m_actors.GetNext(&i);
    }
}

WTACTOR *SCENE::GetActor(RESHANDLE handle)
{
    return (WTACTOR *)m_actors.Get(handle);
}

WTACTOR *SCENE::FindActor(const char *name)
{
    return (WTACTOR *)m_actors.Find(name);
}

SRESULT SCENE::TestCmd(CMDCALL *call)
{
    if (atof(call->tokens->GetToken(1)) > 0.0)
        return SRESULT(true);
    return SRESULT(false);
}

SRESULT SCENE::ToggleCollisions(CMDCALL *call)
{
    ((CPRIMITIVE3MANAGER *)g_pCPRIMITIVE3)->ToggleHelpers();
    return SRESULT(true);
}

SRESULT SCENE::Virtual22(CMDCALL *call)
{
    return SRESULT(true);
}

SCENE *CreateScene()
{
    return new SCENE;
}

SCENE::~SCENE()
{
    Reset();
    DestroyPVS();
}

void SCENE::Remove(RESHANDLE handle)
{
    if (handle.IsValid()) {
        SAFEINDEX<ACTOR, 5000, 0xfb, 1> &actors = m_actors;
        ACTOR *actor = actors.Get(handle);
        if (actor) {
            if (m_updating) {
                m_pendingRemoves[m_numPendingRemoves] = handle;
                m_numPendingRemoves++;
                return;
            }
            actor->Virtual89(false);
            actors.Remove(actor->m_handle);
        }
    }
}

void SCENE::MessageSpawned(RESHANDLE spawner, STRING message)
{
    STRING command;
    if (!spawner.IsValid())
        return;
    int i;
    for (ACTOR *actor = m_actors.GetFirst(&i); i != -1; actor = m_actors.GetNext(&i)) {
        if (actor && actor->m_9a0 && !actor->m_90 && actor->m_spawner.serial == spawner.serial
            && actor->m_spawner.index == spawner.index && actor->m_spawner.kind == spawner.kind) {
            command.Format("msg %s (%s)", actor->m_name.c_str(), message.c_str());
            g_pCmdMgr->Run(command, RESHANDLE());
        }
    }
}

void SCENE::Update()
{
    if (!m_initialized)
        return;
    if (g_pGame && !g_pGame->Virtual23())
        return;
    m_updating = true;
    m_numPendingRemoves = 0;
    FRUSTUM *frustum = ((CAMERA *)g_pCamera)->m_lens->GetFrustum();
    int i;
    ACTOR *actor;
    for (actor = m_actors.GetFirst(&i); i != -1; actor = m_actors.GetNext(&i)) {
        if (actor)
            actor->UpdateVisibility();
    }
    switch (m_pvsType) {
    case PVS_OCTREE:
        m_octree->Update(frustum);
        break;
    case PVS_ONENODE:
        m_oneNode->Update(frustum);
        break;
    }
    actor = m_actors.GetFirst(&i);
    while (i != -1) {
        if (actor && !actor->m_90) {
            if (!actor->m_9a0) {
                switch (actor->m_128) {
                case 0:
                    if (actor->m_a08 <= 0)
                        actor->Virtual89(true);
                    break;
                case 1:
                    actor->Virtual89(true);
                    break;
                case 2:
                    if (CanUpdate(actor) && actor->m_cbd)
                        actor->Virtual89(true);
                    break;
                }
            } else {
                if (actor->m_12c == 0 && !actor->m_cbd)
                    actor->Virtual89(false);
            }
        }
        actor = m_actors.GetNext(&i);
    }
    for (TIMERHOOK *hook = g_actorUpdateHooks.head; hook; hook = hook->next)
        hook->func();
    BeginUpdate();
    actor = m_actors.GetFirst(&i);
    while (i != -1) {
        if (actor && !actor->m_90) {
            if (g_pGame) {
                g_pGame->Virtual22(actor);
                if (actor->m_cbc)
                    g_pScene->Remove(actor->m_handle);
            } else if (actor->m_9a0) {
                ((WTACTOR *)actor)->Step();
            }
        }
        actor = m_actors.GetNext(&i);
    }
    EndUpdate();
    m_updating = false;
    for (i = 0; i < m_numPendingRemoves; i++) {
        if (GetActor(m_pendingRemoves[i]))
            Remove(m_pendingRemoves[i]);
    }
    m_numPendingRemoves = 0;
}

void SCENE::RenameActor(RESHANDLE handle, const char *name)
{
    m_actors.ChangeName(handle, name, 2);
}

ACTOR *SCENE::CreateActor(int factoryId)
{
    ACTOR *actor = g_factoryCreators[factoryId]();
    RESHANDLE handle = m_actors.Insert(&actor, 2);
    if (!handle.IsValid() && g_pLog)
        g_pLog->Error("SCENE::CreateActor - Could create insert actor type:%s", g_factoryNames[factoryId]);
    return actor;
}

ACTOR *SCENE::CreateActor(const char *type)
{
    int id = -1;
    for (int i = 0; i < g_numFactories; i++) {
        if (!_strnicmp(g_factoryNames[i], type, 30)) {
            id = i;
            break;
        }
    }
    if (id < 0)
        return 0;
    return CreateActor(id);
}
