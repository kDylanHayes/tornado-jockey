// This file was built seeing SRESULT::FreeValue as one that can throw.
#define SRESULT_FREEVALUE_MAY_THROW

#include <stdio.h>

#include "Tongas2/PATHREPOSITORY.h"
#include "Tongas2/LOG.h"

extern char g_basePath[];
extern RESOURCEMANAGER *g_pPATH;

class PATHREPOSITORYMANAGER : public RESOURCEMANAGER
{
public:
    PATHREPOSITORYMANAGER()
    {
        SafeSprintf(m_fileName, "PATHREPOSITORY.txt");
        m_8 = false;
        m_18 = true;
    }

    virtual RESOURCEMASTER *CreateMaster(const char *name) { return new PATHREPOSITORYMASTER(this, name); }
    virtual const char *GetTypeCode();
};

static RESOURCEMANAGER *CreatePATHREPOSITORY();

CREATEMANAGER g_pfnCreatePATHREPOSITORY = CreatePATHREPOSITORY;

CMDENTRY PATHREPOSITORYMASTER::s_commands[] = {
    { 0, 0, 0, 0, 0 },
};

PROPENTRY PATHREPOSITORYMASTER::s_properties[] = {
    { "FileName", 0, 7, 0, PROP_FIELD(PATHREPOSITORYMASTER, m_file), false, 1, PROP_SETTER(PATHREPOSITORYMASTER, SetFileName) },
    { 0, 0, 15, 0, 0, false },
};

void PATHREPOSITORYMASTER::RegisterCommands()
{
    g_classParents[s_classId] = RESOURCEMASTER::s_classId;
    g_classCommands[s_classId] = s_commands;
    HashCommands(s_commands);
    RegisterClassName(s_commands, "PATHREPOSITORYMASTER", "RESOURCEMASTER");
}

const char *PATHREPOSITORYMANAGER::GetTypeCode()
{
    return "PARE";
}

static RESOURCEMANAGER *CreatePATHREPOSITORY()
{
    return new PATHREPOSITORYMANAGER;
}

PATHREPOSITORYMASTER::PATHREPOSITORYMASTER(RESOURCEMANAGER *manager, const char *name)
    : RESOURCEMASTER(manager, name)
{
    m_file = "";
}

int PATHREPOSITORYMASTER::GetClassId()
{
    return s_classId;
}

int PATHREPOSITORYMASTER::GetParentClassId()
{
    return g_classParents[s_classId];
}

const char *PATHREPOSITORYMASTER::GetClassName()
{
    return g_classNames[s_classId];
}

SRESULT PATHREPOSITORYMASTER::Execute(CMDENTRY *command, TOKENS **args)
{
    return (this->*(SRESULT (PATHREPOSITORYMASTER::*)(TOKENS **))command->func)(args);
}

void PATHREPOSITORYMASTER::Clear()
{
    for (unsigned int i = 0; i < m_paths.m_size; i++)
        m_paths[i].clear();
    m_paths.m_size = 0;
    m_names.clear();
}

PATHREPOSITORYMASTER::~PATHREPOSITORYMASTER()
{
    Clear();
}

void PATHREPOSITORYMASTER::SetFileName(const CSTRING &name)
{
    m_file = name;
    if (m_file.Length() <= 0)
        return;

    STRING path;
    path.Format("%s%s%s", g_basePath, "data/art/", m_file.c_str());
    FILE *file = fopen(path.c_str(), "rt");
    if (!file) {
        if (g_pLog)
            g_pLog->Error("PATHREPOSITYMASTER::SetFileName - file %s not found", path.c_str());
        return;
    }

    char pathName[0x200];
    pathName[0] = 0;
    pathName[0x1ff] = 0;
    int total = 0;
    fscanf(file, "Total:%d\n", &total);
    m_paths.SetSize(total);
    m_names.resize(total);
    for (int i = 0; i < total; i++) {
        fscanf(file, "%s[^\n]\n", pathName);
        PATHMASTER *master = (PATHMASTER *)g_pPATH->GetMaster(pathName);
        if (!master) {
            master = (PATHMASTER *)g_pPATH->AddMaster(pathName);
            master->SetName(CSTRING(pathName));
            master->m_48 = false;
        }
        int count = 0;
        fscanf(file, "%d\n", &count);
        m_paths[i].resize(count);
        m_names[i].assign(pathName, strlen(pathName));
        master->m_numPoints = count;
        for (int j = 0; j < count; j++) {
            // The file's y and z are the other way round.
            float x1, y1, z1, x2, y2, z2;
            int unused;
            fscanf(file, "[%f,%f,%f] [%f,%f,%f] %d\n", &x1, &y1, &z1, &x2, &y2, &z2, &unused);
            master->m_direction[j] = V3(x2, z2, y2) - V3(x1, z1, y1);
            master->m_pos[j] = V3(x1, z1, y1);
            m_paths[i][j].pos = V3(x1, z1, y1);
            m_paths[i][j].step = V3(x2, z2, y2) - V3(x1, z1, y1);
        }
        master->SetCalculateLengths(true);
        master->SetCurvature(0);
        master->SetCalculateLengths(true);
    }
    fclose(file);
}
