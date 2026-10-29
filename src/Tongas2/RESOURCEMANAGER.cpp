#include "Tongas2/RESOURCES.h"
#include "Tongas2/APP.h"
#include "Tongas2/APPWINDOW.h"
#include "Tongas2/FILELOADER.h"
#include "Tongas2/LOG.h"
#include "Tongas2/STRING.h"
#include "Tongas2/SYSMONITOR.h"
#include "Tongas2/TOKENS.h"

#include <stdlib.h>
#include <string.h>

// Defined elsewhere: the folder the game runs from.
extern char g_basePath[];

RESOURCEMANAGER::~RESOURCEMANAGER()
{
    if (m_loader) {
        delete m_loader;
        m_loader = 0;
    }
    RESOURCEMASTER *master = m_first;
    while (master) {
        RESOURCEMASTER *next = master->m_next;
        g_pResources->RemoveMaster(master->m_handle);
        master = next;
    }
}

RESOURCEMASTER *RESOURCEMANAGER::CreateMaster(const char *name)
{
    return 0;
}

bool RESOURCEMANAGER::IsMyType(const char *name)
{
    return false;
}

const char *RESOURCEMANAGER::GetTypeCode()
{
    return "";
}

bool RESOURCEMANAGER::HasPrefix(const char *name)
{
    const char *code = GetTypeCode();
    for (int i = 0; i < 4; i++) {
        if (*name++ != *code++)
            return false;
    }
    if (*name != ':')
        return false;
    return true;
}

bool RESOURCEMANAGER::RemoveMaster(const RESHANDLE &handle)
{
    RESOURCEMASTER *master = g_pResources->GetMaster(handle);
    if (!master)
        return false;
    RESOURCEMASTER *prev = master->m_prev;
    RESOURCEMASTER *next = master->m_next;
    if (m_first == master)
        m_first = next;
    if (m_last == master)
        m_last = prev;
    if (prev)
        prev->m_next = next;
    if (next)
        next->m_prev = prev;
    g_pResources->RemoveMaster(handle);
    m_count--;
    return true;
}

void RESOURCEMANAGER::SetLevel(int level)
{
    RESOURCEMASTER *master = m_first;
    while (master) {
        RESOURCEMASTER *next = master->m_next;
        if (master->m_groupMask & level)
            master->RequestCopy();
        master = next;
    }
}

void RESOURCEMANAGER::Flush()
{
    RESOURCEMASTER *master = m_first;
    while (master) {
        RESOURCEMASTER *next = master->m_next;
        master->TrimCopies();
        master = next;
    }
}

void RESOURCEMANAGER::DeleteUnused(int level, bool all)
{
    RESOURCEMASTER *master = m_first;
    while (master) {
        RESOURCEMASTER *next = master->m_next;
        if (!(master->m_groupMask & level))
            master->DeleteCopies(all);
        master = next;
    }
}

void RESOURCEMANAGER::Update()
{
    RESOURCEMASTER *master = m_first;
    while (master) {
        RESOURCEMASTER *next = master->m_next;
        master->UpdateCopies();
        master = next;
    }
}

bool RESOURCEMANAGER::HasTypeCode(const char *name)
{
    if (*(const int *)name == *(const int *)GetTypeCode())
        return true;
    return false;
}

void RESOURCEMANAGER::PrepareMasters()
{
    RESOURCEMASTER *master = m_first;
    while (master) {
        RESOURCEMASTER *next = master->m_next;
        master->ResizeArrays();
        master->Prepare();
        master->m_prepared = true;
        master = next;
    }
}

void RESOURCEMANAGER::Dump(bool verbose, int index)
{
    RESOURCEMASTER *master = m_first;
    if (index >= 0) {
        while (index > 0) {
            if (!master)
                return;
            master = master->m_next;
            index--;
        }
        master->ReleaseAll();
        return;
    }
    while (master) {
        RESOURCEMASTER *next = master->m_next;
        master->Release(verbose);
        master = next;
    }
}

void RESOURCEMANAGER::Virtual06()
{
    RESOURCEMASTER *master = m_first;
    while (master) {
        RESOURCEMASTER *next = master->m_next;
        master->ResetCopies();
        master = next;
    }
}

void RESOURCEMANAGER::DumpPropsType()
{
    RESOURCEMASTER *master = m_first;
    while (master) {
        RESOURCEMASTER *next = master->m_next;
        master->DumpProperties();
        master = next;
    }
}

void RESOURCEMANAGER::FreeAll()
{
    RESOURCEMASTER *master = m_first;
    while (master) {
        RESOURCEMASTER *next = master->m_next;
        master->FreeUsed();
        master = next;
    }
}

void RESOURCEMANAGER::DumpTotals(bool verbose)
{
    for (RESOURCEMASTER *master = m_first; master; master = master->m_next)
        ;
}

bool RESOURCEMANAGER::CanPreload()
{
    int flags = RESOURCES::s_managerFlags[m_id];
    if (flags & 0x20)
        return true;
    if (flags & 4)
        return false;
    return true;
}

RESOURCEMANAGER::RESOURCEMANAGER()
{
    m_fileName[0] = '\0';
    m_fileName[sizeof(m_fileName) - 1] = '\0';
    m_count = 0;
    m_loaded = false;
    m_initialized = false;
    m_id = -1;
    m_first = 0;
    m_last = 0;
    m_loader = g_pfnCreateFileLoader();
}

RESOURCEMASTER *RESOURCEMANAGER::FindMaster(const char *name)
{
    if (!name || !*name)
        return 0;
    FIXEDSTRING<0x200> path;
    if (!HasPrefix(name)) {
        SafeSprintf(path, "%s:%s", GetTypeCode(), name);
        name = path;
    }
    return g_pResources->FindMaster(name);
}

void RESOURCEMANAGER::Insert(RESOURCEMASTER *master)
{
    if (m_last)
        m_last->m_next = master;
    else
        m_first = master;
    master->m_prev = m_last;
    m_last = master;
    FIXEDSTRING<0x200> path;
    SafeSprintf(path, "%s:%s", GetTypeCode(), master->m_name.c_str());
    master->m_path = path;
    g_pResources->RegisterMaster(master);
    g_pResources->Abort();
    m_count++;
}

RESOURCEMASTER *RESOURCEMANAGER::AddMaster(const char *name)
{
    RESOURCEMASTER *master = CreateMaster(name);
    Insert(master);
    return master;
}

void RESOURCEMANAGER::Init()
{
    if (!m_initialized) {
        STRING path;
        path.Format("%s%s%s", g_basePath, "data/templates/", m_fileName);
        g_pApp->m_monitor->Update();
        if (g_pLog)
            g_pLog->Trace(path.c_str());
        m_loader->Open((char *)path.c_str());
        m_loaded = false;
        m_initialized = true;
    }
}

RESHANDLE RESOURCEMANAGER::GetHandle(const char *name)
{
    RESOURCEMASTER *master = FindMaster(name);
    if (master)
        return master->m_handle;
    if (!CanPreload())
        return RESHANDLE();
    master = CreateMaster(name);
    Insert(master);
    return master->m_handle;
}

RESOURCE *RESOURCEMANAGER::GetResource(const char *name)
{
    RESHANDLE handle = GetHandle(name);
    return g_pResources->GetResource(handle);
}

bool RESOURCEMANAGER::IsListLoaded()
{
    if (m_loaded == true)
        return true;
    if (m_loader->IsError()) {
        if (g_pLog)
            g_pLog->Error("RESOURCEMANAGER::IsListLoaded> Template not found: %s. will assume empty file", m_fileName);
        m_loader->Close();
        PrepareMasters();
        m_loaded = true;
        return true;
    }
    if (!m_loader->IsOpen())
        return false;
    g_pApp->m_monitor->Update();
    STRING line;
    int lineNumber = 0;
    line.Free();
    while (!m_loader->IsEof()) {
        lineNumber++;
        char *text = m_loader->ReadLine();
        if (text) {
            if (!strstr(text, "GVIS:dplogo") && strlen(text) != 0) {
                if (memcmp(text, "end", 3) == 0) {
                    free(text);
                    m_loader->Close();
                    g_StringManager.Defrag();
                    PrepareMasters();
                    m_loaded = true;
                    return true;
                }
                if (text[0] != '#' && text[0] != '/') {
                    line.Append(text);
                    if (strchr(text, ';')) {
                        TOKENS tokens;
                        if (!tokens.Parse(line, 0)) {
                            if (g_pLog)
                                g_pLog->Error("RESOURCEMANAGER::IsListLoaded> Parse error in line %d: '%s'", lineNumber, line.c_str());
                            m_loader->Close();
                            free(text);
                            return false;
                        }
                        if (_stricmp(tokens.GetToken(0), "create") == 0) {
                            RESOURCEMASTER *master = CreateMaster(tokens.GetToken(1));
                            Insert(master);
                            if (tokens.m_count > 2)
                                master->Run(STRING(tokens.GetToken(2)));
                            line.Free();
                        }
                    }
                }
            }
            free(text);
        }
    }
    if (g_pLog)
        g_pLog->Error("RESOURCEMANAGER::IsListLoaded>Template finish without a \"end\"");
    m_loader->Close();
    PrepareMasters();
    m_loaded = true;
    g_StringManager.Defrag();
    return m_loaded;
}

RESOURCEMASTER *RESOURCEMANAGER::GetMaster(const char *name)
{
    RESHANDLE handle = GetHandle(name);
    return g_pResources->GetMaster(handle);
}

bool RESOURCEMANAGER::Load()
{
    if (!IsListLoaded())
        return false;
    float time = g_pDisplay->GetElapsedTime();
    bool loaded = true;
    RESOURCEMASTER *master = m_first;
    while (master) {
        RESOURCEMASTER *next = master->m_next;
        if (!master->Load())
            loaded = false;
        time += g_pDisplay->GetElapsedTime();
        if (time > 0.5f)
            return false;
        master = next;
    }
    return loaded;
}
