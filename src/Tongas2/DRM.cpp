#include "Tongas2/CMDMGR.h"
#include "Tongas2/DRM.h"

#define CMD_FUNC(CLASS, method) (CMDFUNC)(SRESULT (CLASS::*)(TOKENS **))(SRESULT (CLASS::*)(CMDCALL *))&CLASS::method

DRM *CreateDrm();

DRM *(*g_pfnCreateDrm)() = CreateDrm;

CMDENTRY DRM::s_commands[] = {
    { "IsUnlocked", 0, 0, 0, CMD_FUNC(DRM, IsUnlocked) },
    { 0, 0, 0, 0, 0 },
};

PROPENTRY DRM::s_properties[] = {
    { 0, 0, 15, 0, 0, false },
};

void DRM::RegisterCommands()
{
    g_classParents[s_classId] = g_CMDBASEClassId;
    g_classCommands[s_classId] = s_commands;
    HashCommands(s_commands);
    RegisterClassName(s_commands, "DRM", "CMDBASE");
}

void DRM::Decode(const char *data, const char *key, int length, int keyLength, char *out)
{
    int k = 0;
    for (int i = 0; i < length; i++) {
        out[i] = data[i] ^ key[k];
        k = (k + 1) % keyLength;
    }
}

DRM::DRM()
{
    SetName("drm");
}

DRM *g_pDrm;

DRM *CreateDrm()
{
    return new DRM;
}
