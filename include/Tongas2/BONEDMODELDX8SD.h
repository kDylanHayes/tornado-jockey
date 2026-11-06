// The DX8 renderer plugin's skinned boned models: BONEDMODELDX8SDMASTER and
// BONEDMODELDX8SDRESOURCE derive from the engine's BONEDMODEL classes and
// draw through the DX8 skinned mesh.
#ifndef TONGAS2_BONEDMODELDX8SD_H
#define TONGAS2_BONEDMODELDX8SD_H

#include "Tongas2/BONEDMODEL.h"
#include "Tongas2/CMDBASE.h"
#include "Tongas2/RESOURCES.h"

class BONEDMODELDX8SDMASTER : public BONEDMODELMASTER
{
public:
    BONEDMODELDX8SDMASTER(RESOURCEMANAGER *manager, const char *name);

    static void RegisterCommands();

    virtual int GetClassId();
    virtual int GetParentClassId();
    virtual const char *GetClassName();
    virtual SRESULT Execute(CMDENTRY *command, TOKENS **args);

    static int s_classId;
    static CMDENTRY s_commands[];

};

class BONEDMODELDX8SDRESOURCE : public BONEDMODELRESOURCE
{
public:
    static void RegisterCommands();

    virtual void Start();
    virtual void Stop();
    virtual void Update();

    virtual SRESULT LoadExtraAnimation(CMDCALL *call);

    bool FUN_004f7040(const char *name);

    static int s_classId;
    static CMDENTRY s_commands[];

    int m_410;
    int m_414;
    int m_418;
    int m_41c;
    int m_420;
    bool m_424;
};

class BONEDMODELMANAGER : public RESOURCEMANAGER
{
public:
    __forceinline BONEDMODELMANAGER()
    {
        SafeSprintf(m_fileName, "BONEDMODEL.txt");
        m_8 = false;
        m_18 = true;
    }

    virtual RESOURCEMASTER *CreateMaster(const char *name);
    virtual const char *GetTypeCode();
};

class BONEDMODELDX8SDMANAGER : public BONEDMODELMANAGER
{
public:
    __forceinline BONEDMODELDX8SDMANAGER() {}

    virtual RESOURCEMASTER *CreateMaster(const char *name);
    virtual const char *GetTypeCode();
};

RESOURCEMANAGER *CreateBONEDMODELDX8SD();

#endif
