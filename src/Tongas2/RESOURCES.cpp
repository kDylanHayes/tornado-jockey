#include <stdlib.h>
#include <string.h>

#include "lib/freeimage/FreeImage.h"
#include "Tongas2/APPOPTIONS.h"
#include "Tongas2/LOG.h"
#include "Tongas2/RESOURCES.h"

CMDENTRY RESOURCES::s_commands[] = {
    { "dumpall", 0, 0, 1, (CMDFUNC)&RESOURCES::CmdDumpAll },
    { "dump", 0, 1, 2, (CMDFUNC)&RESOURCES::CmdDump },
    { "dumptotals", 0, 0, 1, (CMDFUNC)&RESOURCES::CmdDumpTotals },
    { "dumppropstype", 0, 1, 1, (CMDFUNC)&RESOURCES::CmdDumpPropsType },
    { "DeleteUnused", 0, 0, 0, (CMDFUNC)&RESOURCES::CmdDeleteUnused },
    { 0, 0, 0, 0, 0 },
};

PROPENTRY RESOURCES::s_properties[] = {
    { "SoundMasterVolume", 0, 3, 0, (PROPFIELD)&RESOURCES::m_soundVolume, false, 1, (PROPSETTER)&RESOURCES::SetSoundVolume },
    { "MusicMasterVolume", 0, 3, 0, (PROPFIELD)&RESOURCES::m_musicVolume, false, 1, (PROPSETTER)&RESOURCES::SetMusicVolume },
    { 0, 0, 15, 0, 0, false },
};

CMDBASE *(*RESOURCES::s_pfnCreate)() = RESOURCES::Create;

// Each manager type's create function, and the global its manager is kept in.
#define RESOURCE_TYPE(TYPE) \
    extern CREATEMANAGER g_pfnCreate##TYPE; \
    RESOURCEMANAGER *g_p##TYPE;

RESOURCE_TYPE(BONEDMODEL)
RESOURCE_TYPE(SOUND)
RESOURCE_TYPE(SHADER)
RESOURCE_TYPE(MESH)
RESOURCE_TYPE(SIMPLEMODEL)
RESOURCE_TYPE(DECAL)
RESOURCE_TYPE(SCREENDECAL)
RESOURCE_TYPE(TEXT)
RESOURCE_TYPE(POLYTRAIL)
RESOURCE_TYPE(TESLA)
RESOURCE_TYPE(CPRIMITIVE3)
RESOURCE_TYPE(EMITTER)
RESOURCE_TYPE(LIGHT)
RESOURCE_TYPE(FX)
RESOURCE_TYPE(GLOBALDEFS)
RESOURCE_TYPE(PATH)
RESOURCE_TYPE(PATHREPOSITORY)
RESOURCE_TYPE(ACTORRESOURCES)
RESOURCE_TYPE(GUIGENERIC)
RESOURCE_TYPE(GUITEXT)
RESOURCE_TYPE(GUITEXTEDIT)
RESOURCE_TYPE(GUIVISUAL)
RESOURCE_TYPE(GUICHECKBOX)
RESOURCE_TYPE(GUICHECKGROUP)
RESOURCE_TYPE(GUISLIDER)
RESOURCE_TYPE(GUIGROUP)

char RESOURCES::s_managerNames[RESOURCES_MAX_MANAGERS][30];
RESOURCEMANAGER **RESOURCES::s_managerSlots[RESOURCES_MAX_MANAGERS];
RESOURCEMANAGER *RESOURCES::s_managers[RESOURCES_MAX_MANAGERS];
RESOURCES *g_pResources;
int RESOURCES::s_numManagers;
int RESOURCES::s_managerFlags[RESOURCES_MAX_MANAGERS];
CALLBACKLIST<RESOURCESCALLBACK> RESOURCES::s_onInit;
CALLBACKLIST<RESOURCESCALLBACK> RESOURCES::s_onCreate;
CALLBACKLIST<RESOURCESCALLBACK> RESOURCES::s_onSetLevel;

void RESOURCES::RegisterCommands()
{
    g_classParents[s_classId] = g_CMDBASEClassId;
    g_classCommands[s_classId] = s_commands;
    HashCommands(s_commands);
    RegisterClassName(s_commands, "RESOURCES", "CMDBASE");
}

static void FreeImageMessage(FREE_IMAGE_FORMAT fif, const char *message)
{
    if (g_pLog)
        g_pLog->Error("FREEIMAGE: %s (fif=%d)", message, fif);
}

// Creates a manager and records it under TYPE's name.
#define LOAD_MANAGER(TYPE, FLAGS) \
    { \
        s_managers[s_numManagers] = (*g_pfnCreate##TYPE)(); \
        s_managerSlots[s_numManagers] = &g_p##TYPE; \
        g_p##TYPE = s_managers[s_numManagers]; \
        g_p##TYPE->Init(); \
        s_managerFlags[s_numManagers] = FLAGS; \
        if (s_managers[s_numManagers]->m_18) \
            s_managerFlags[s_numManagers] |= 4; \
        strncpy(s_managerNames[s_numManagers], #TYPE, 20); \
        int id = s_numManagers++; \
        g_p##TYPE->m_id = id; \
    }

void RESOURCES::LoadManagers()
{
    LOAD_MANAGER(BONEDMODEL, 1)
    LOAD_MANAGER(SOUND, 1)
    LOAD_MANAGER(SHADER, 2)
    LOAD_MANAGER(MESH, 0)
    LOAD_MANAGER(SIMPLEMODEL, 1)
    LOAD_MANAGER(DECAL, 0x21)
    LOAD_MANAGER(SCREENDECAL, 1)
    LOAD_MANAGER(TEXT, 1)
    LOAD_MANAGER(POLYTRAIL, 1)
    LOAD_MANAGER(TESLA, 1)
    LOAD_MANAGER(CPRIMITIVE3, 1)
    LOAD_MANAGER(EMITTER, 1)
    LOAD_MANAGER(LIGHT, 0x11)
    LOAD_MANAGER(FX, 1)
    LOAD_MANAGER(GLOBALDEFS, 0x10)
    LOAD_MANAGER(PATH, 0x10)
    LOAD_MANAGER(PATHREPOSITORY, 0x10)
    LOAD_MANAGER(ACTORRESOURCES, 0x10)
    LOAD_MANAGER(GUIGENERIC, 0x18)
    LOAD_MANAGER(GUITEXT, 0x18)
    LOAD_MANAGER(GUITEXTEDIT, 0x18)
    LOAD_MANAGER(GUIVISUAL, 0x18)
    LOAD_MANAGER(GUICHECKBOX, 0x18)
    LOAD_MANAGER(GUICHECKGROUP, 0x18)
    LOAD_MANAGER(GUISLIDER, 0x18)
    LOAD_MANAGER(GUIGROUP, 0x18)
    if (s_numManagers >= RESOURCES_MAX_MANAGERS && g_pLog)
        g_pLog->Error("ERROR!! RESOURCES::LoadManagers>FATAL ERROR, max number of supported resource managers exceded");
}

void RESOURCES::FreeManagers()
{
    for (int i = s_numManagers - 1; i >= 0; i--) {
        if (s_managers[i]) {
            delete s_managers[i];
            s_managers[i] = 0;
        }
        *s_managerSlots[i] = 0;
    }
}

void RESOURCES::SetLevel(int level)
{
    m_d4 = 0;
    m_d8 = 0;
    int i;
    for (i = s_numManagers - 1; i >= 0; i--)
        if (s_managers[i])
            s_managers[i]->DeleteUnused(level, false);
    for (i = s_numManagers - 1; i >= 0; i--)
        if (s_managers[i])
            s_managers[i]->Flush();
    for (i = 0; i < s_numManagers; i++)
        if (s_managers[i])
            s_managers[i]->SetLevel(level);
    m_level = level;
    m_loaded = false;
    s_onSetLevel.Call();
}

bool RESOURCES::Load()
{
    if (m_loaded)
        return true;
    for (int i = 0; i < s_numManagers; i++) {
        if (s_managers[i]) {
            if (!s_managers[i]->Load())
                return false;
            if (m_abort) {
                m_abort = false;
                return false;
            }
        }
    }
    m_loaded = true;
    return true;
}

void RESOURCES::Update()
{
    for (int i = s_numManagers - 1; i >= 0; i--)
        if (s_managers[i])
            s_managers[i]->Update();
    m_lastMusicVolume = m_musicVolume;
    m_lastSoundVolume = m_soundVolume;
}

RESOURCEMANAGER *RESOURCES::GetManager(const char *&name)
{
    const char *str = name;
    RESOURCEMANAGER *manager = 0;
    int i;
    if (strlen(str) > 4 && (str[4] == ':' || str[4] == '*')) {
        for (i = 0; i < s_numManagers; i++) {
            if (s_managers[i] && s_managers[i]->HasTypeCode(str)) {
                manager = s_managers[i];
                break;
            }
        }
        str += 5;
    }
    if (!manager) {
        for (i = 0; i < s_numManagers; i++) {
            if (s_managers[i] && s_managers[i]->IsMyType(str)) {
                manager = s_managers[i];
                break;
            }
        }
    }
    if (!manager) {
        if (strlen(str) > 1 && g_pLog)
            g_pLog->Error("ERROR!! resource %s type not recognized!! use resource type codes!", str);
        return 0;
    }
    name = str;
    return manager;
}

RESOURCEMANAGER *RESOURCES::FindManager(const char *typeName)
{
    for (int i = 0; i < s_numManagers; i++)
        if (s_managers[i] && !_stricmp(s_managerNames[s_managers[i]->m_id], typeName))
            return s_managers[i];
    return 0;
}

int RESOURCES::ResetGroups()
{
    return InitGroups();
}

void RESOURCES::AddMainChannel()
{
    g_pResources->m_channels[g_pResources->m_numChannels++] = "Main";
}

int RESOURCES::InitGroups()
{
    m_groups[0] = "Ambient";
    return 1;
}

void RESOURCES::DumpAll(bool skipUnlisted)
{
    for (int i = 0; i < s_numManagers; i++)
        if (s_managers[i] && (!skipUnlisted || !(s_managerFlags[i] & 0x10)))
            s_managers[i]->Dump(false, -1);
}

void RESOURCES::Abort()
{
    m_abort = true;
    m_loaded = false;
}

int RESOURCES::GetDetailBias()
{
    switch (g_pApp->m_pOptions->m_detail) {
    case 0:
        return -2;
    case 1:
        return -1;
    }
    return 0;
}

float RESOURCES::GetDetailScale()
{
    if (g_pApp->m_pOptions->m_detail != 0)
        return 1.0f;
    return 0.25f;
}

float RESOURCES::GetDetailFactor()
{
    switch (g_pApp->m_pOptions->m_detail) {
    case 0:
        return 0.25f;
    case 1:
        return 0.5f;
    }
    return 1.0f;
}

float RESOURCES::GetGlobalScale()
{
    return m_globalScale;
}

void RESOURCES::SetGlobalScale(float scale)
{
    m_globalScale = scale;
}

int RESOURCES::GetDetailSteps()
{
    return g_pApp->m_pOptions->m_detail == 0 ? 2 : 1;
}

bool RESOURCES::IsHighDetail()
{
    return g_pApp->m_pOptions->m_detail == 2;
}

bool RESOURCES::IsLowDetail()
{
    return g_pApp->m_pOptions->m_detail == 0;
}

bool RESOURCES::IsDetailed()
{
    return g_pApp->m_pOptions->m_detail != 0;
}

void RESOURCES::InitManagers()
{
    s_numManagers = 0;
    s_onInit.AddHead(LoadManagers);
    s_onInit.Call();
    s_onCreate.AddHead(AddMainChannel);
    s_onCreate.Call();
}

RESHANDLE RESOURCES::CreateFromTemplate(const char *name, V3 position, int layer)
{
    if (strlen(name)) {
        OBJECT *object = (OBJECT *)g_pFX->GetResource(name);
        if (object) {
            object->SetActive(true);
            object->SetPosition(position);
            object->SetScale(1.0f, 1.0f, 1.0f);
            object->SetAlpha(1.0f);
            object->SetAngles(ANG(0.0f, 0.0f, 0.0f));
            object->SetHidden(false);
            object->SetLayer(layer);
            return object->m_handle;
        }
    }
    return RESHANDLE();
}

RESOURCES::RESOURCES()
{
    m_level = 0;
    m_loaded = false;
    m_abort = false;
    m_lastMusicVolume = -1.0f;
    m_lastSoundVolume = -1.0f;
    m_d4 = 0;
    m_d8 = 0;
    m_numChannels = 0;
    m_soundVolume = 1.0f;
    m_musicVolume = 1.0f;
    SetName("resources");
    FreeImage_Initialise(FALSE);
    FreeImage_SetOutputMessage(FreeImageMessage);
}

RESOURCES::~RESOURCES()
{
    for (int i = s_numManagers - 1; i >= 0; i--)
        if (s_managers[i])
            s_managers[i]->DeleteUnused(0, true);
    FreeManagers();
    if (m_masters.m_count > 0 && g_pLog)
        g_pLog->Error("THERE ARE %d MASTERS IN GLOBAL POOL!!! SHOULD BE 0", m_masters.m_count);
    if (m_copies.m_count > 0 && g_pLog)
        g_pLog->Error("THERE ARE %d COPIES IN GLOBAL POOL!!! SHOULD BE 0", m_copies.m_count);
    FreeImage_DeInitialise();
}

SRESULT RESOURCES::CmdDeleteUnused(TOKENS **args)
{
    for (int i = s_numManagers - 1; i >= 0; i--)
        if (s_managers[i])
            s_managers[i]->DeleteUnused(0, false);
    return SRESULT(true);
}

RESOURCE *RESOURCES::GetResource(RESHANDLE handle)
{
    if (handle.serial < 0 || handle.index < 0)
        return 0;
    if (handle.kind == 3) {
        // m_copies.Get, written out.
        if (handle.index >= 0x8000)
            return 0;
        COPY *copy = m_copies.m_items[handle.index];
        if (!copy || handle.serial != copy->m_handle.serial || handle.index != copy->m_handle.index || handle.kind != copy->m_handle.kind)
            return 0;
        return copy;
    }
    if (handle.kind == 4) {
        MASTER *master = m_masters.Get(handle);
        if (master)
            return master->GetCopy();
    }
    return 0;
}

MASTER *RESOURCES::FindMaster(const char *name)
{
    return m_masters.Find(name);
}

MASTER *RESOURCES::GetMaster(RESHANDLE handle)
{
    if (handle.serial < 0 || handle.index < 0)
        return 0;
    return m_masters.Get(handle);
}

RESHANDLE RESOURCES::GetHandle(const char *name)
{
    if (name && *name) {
        MASTER *master = m_masters.Find(name);
        if (master)
            return master->m_handle;
        RESOURCEMANAGER *manager = GetManager(name);
        if (manager)
            return manager->GetHandle(name);
    }
    return RESHANDLE();
}

SRESULT RESOURCES::Dump(TOKENS **args)
{
    for (int i = 0; i < s_numManagers; i++) {
        if (s_managers[i] && !_stricmp(s_managerNames[i], (*args)->GetToken(1))) {
            if ((*args)->m_count == 3) {
                s_managers[i]->Dump(true, atoi((*args)->GetToken(2)));
                return SRESULT(true);
            }
            s_managers[i]->Dump(true, -1);
            return SRESULT(true);
        }
    }
    return SRESULT(false);
}

void RESOURCES::UnregisterCopy(const RESHANDLE &handle)
{
    m_copies.Release(handle);
}

void RESOURCES::RemoveMaster(const RESHANDLE &handle)
{
    m_masters.Remove(handle);
}

SRESULT RESOURCES::CmdDumpAll(TOKENS **args)
{
    DumpAll(atoi((*args)->GetToken(1)) != 0);
    return SRESULT(true);
}

SRESULT RESOURCES::CmdDump(TOKENS **args)
{
    Dump(args);
    return SRESULT(true);
}

SRESULT RESOURCES::CmdDumpPropsType(TOKENS **args)
{
    const char *typeName = (*args)->GetToken(1);
    RESOURCEMANAGER *manager = FindManager(typeName);
    if (!manager) {
        if (g_pLog)
            g_pLog->Error("could not find resource type name: %s", typeName);
    } else
        manager->DumpPropsType();
    return SRESULT(true);
}

SRESULT RESOURCES::CmdDumpTotals(TOKENS **args)
{
    bool verbose = false;
    if ((*args)->m_count > 1)
        verbose = atoi((*args)->GetToken(1)) != 0;
    for (int i = 0; i < s_numManagers; i++)
        if (s_managers[i])
            s_managers[i]->DumpTotals(verbose);
    return SRESULT(true);
}

CMDBASE *RESOURCES::Create()
{
    return new RESOURCES;
}

RESOURCE *RESOURCES::GetResource(const char *name)
{
    RESHANDLE handle = GetHandle(name);
    return GetResource(handle);
}

void RESOURCES::RegisterCopy(COPY *copy)
{
    RESHANDLE handle = m_copies.Insert(&copy, 0);
    if (handle.serial < 0 || handle.index < 0) {
        if (g_pLog)
            g_pLog->Error("Could not register copy", copy);
        if (copy->m_master && g_pLog)
            g_pLog->Error(" from master:%s", copy->m_master->m_name.c_str());
    }
}

void RESOURCES::RenameMaster(RESHANDLE &handle, const char *name)
{
    m_masters.ChangeName(handle, name, 2);
}

void RESOURCES::RegisterMaster(MASTER *master)
{
    RESHANDLE handle = m_masters.Insert(&master, 0);
    if ((handle.serial < 0 || handle.index < 0) && g_pLog)
        g_pLog->Error("RESOURCES::RegisterMaster - Could not insert master:%s", master->m_fileName.c_str());
}
