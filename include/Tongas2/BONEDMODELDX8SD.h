// The DX8 renderer plugin's skinned boned models: BONEDMODELDX8SDMASTER and
// BONEDMODELDX8SDRESOURCE derive from the engine's BONEDMODEL classes and
// draw through the DX8 skinned mesh.
#ifndef TONGAS2_BONEDMODELDX8SD_H
#define TONGAS2_BONEDMODELDX8SD_H

#include "Tongas2/BONEDMODEL.h"
#include "Tongas2/CMDBASE.h"
#include "Tongas2/RESOURCES.h"

#include <string>
#include <vector>

class BONEDMODELDX8SDRESOURCE;

// One entry of the model's tag list, read from the .tag file.
class BONEDMODELTAG
{
public:
    BONEDMODELTAG();
    virtual ~BONEDMODELTAG();

    int m_material;
    MATRIX m_matrix;
    MATRIX m_inverse;
    V3 m_position;
    char *m_name;
};

// The interface the resource's animation player derives from.
class BONEDMODELDX8ANIMBASE
{
public:
    BONEDMODELDX8ANIMBASE();
    virtual ~BONEDMODELDX8ANIMBASE();

    virtual void Virtual1() {}
    virtual void Virtual2() {}
    virtual void Virtual3() {}
    virtual void Virtual4() {}
    virtual void Virtual5() {}
    virtual void Virtual6();
    virtual bool Virtual7();

    void SetEnabled(bool enabled);
    void SetSomething(bool value);

    char m_data[0x28];
};

// An animation event: a resource and a tag index.
class BONEDMODELEVENTBASE
{
public:
    virtual ~BONEDMODELEVENTBASE();
    virtual void Virtual1() = 0;
    virtual void Virtual2() = 0;
};

class BONEDMODELEVENT : public BONEDMODELEVENTBASE
{
public:
    BONEDMODELEVENT(BONEDMODELDX8SDRESOURCE *resource, int tag);

    virtual void Virtual1() {}
    virtual void Virtual2() {}

    BONEDMODELDX8SDRESOURCE *m_resource;
    int m_tag;
    int m_10;
};

class BONEDMODELDX8SDMASTER : public BONEDMODELMASTER
{
public:
    BONEDMODELDX8SDMASTER(RESOURCEMANAGER *manager, const char *name);

    static void RegisterCommands();

    virtual int GetClassId();
    virtual int GetParentClassId();
    virtual const char *GetClassName();
    virtual SRESULT Execute(CMDENTRY *command, TOKENS **args);
    virtual RESOURCE *CreateCopy(const RESHANDLE &handle);

    static int s_classId;
    static CMDENTRY s_commands[];
};

class BONEDMODELDX8SDRESOURCE : public BONEDMODELRESOURCE, public BONEDMODELDX8ANIMBASE
{
public:
    BONEDMODELDX8SDRESOURCE(BONEDMODELMASTER *master, const RESHANDLE &handle);
    virtual ~BONEDMODELDX8SDRESOURCE();

    static void RegisterCommands();

    virtual int GetClassId();
    virtual int GetParentClassId();
    virtual const char *GetClassName();
    virtual SRESULT Execute(CMDENTRY *command, TOKENS **args);

    virtual void Virtual1();
    virtual void Virtual2();
    virtual void Virtual3();
    virtual void Virtual4();
    virtual void Virtual5();

    virtual void Virtual71();
    virtual void Start();
    virtual void Stop();
    virtual void Update();

    virtual void SetAnimationFrame(const char *animation, float frame) {}
    virtual void PlayAnimation(void *animation, bool loop, float speed) {}
    virtual void BlendAnimation(const char *animation, float blend, bool loop, float speed) {}

    void RunCommand(const char *command);
    void DestroyAnimations();
    void SetBone(int index, const V3 &position, void *placement);
    int FindMaterial(const char *name);

    // The model's tag list.
    bool FUN_004f7040(const char *name);

    // New virtuals of the DX8 resource, in declaration order.
    virtual void Virtual88();
    virtual void Virtual89();
    virtual void Virtual90();
    virtual void Virtual91(int animation, float frame);
    virtual void Virtual92();
    virtual void Virtual93();
    virtual void Virtual94();
    virtual void Virtual95();
    virtual void Virtual96();
    virtual void Virtual97();
    virtual void Virtual98();
    virtual void Virtual99();
    virtual void Virtual100();
    virtual void Virtual101(int a, int b);
    virtual void Virtual102();
    virtual void Virtual103();
    virtual void Virtual104();
    virtual void Virtual105();
    virtual SRESULT LoadExtraAnimation(CMDCALL *call);

    static int s_classId;
    static CMDENTRY s_commands[];

    void *m_model;
    void *m_loader;
    int m_444;
    bool m_448;
    char m_449[0x647 - 0x449];
    bool m_647;
    bool m_648;
    char m_649[0x64c - 0x649];
    BONEDMODELTAG *m_tags;
    int m_tagCount;
    void *m_materials;
    std::vector<std::string> m_strings;
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
