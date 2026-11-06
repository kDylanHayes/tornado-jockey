// Tongas2/BONEDMODELDX8SD -- the DX8 renderer plugin's skinned boned models.
#include "Tongas2/BONEDMODELDX8SD.h"
#include "Tongas2/LOG.h"
#include "Tongas2/TIMER.h"
#include "Tongas2/CMDMGR.h"
#include "Tongas2/RENDERER.h"
#include "Tongas2/APP.h"
#include <d3dx8.h>
#include <stdio.h>
#include <string.h>

const float kNegativeOne = -1.0f;
const double kDegToRad = 0.01745329252;
#include <algorithm>


// The display's renderer-side context.
class BONEDMODELDX8ANIMOBJ
{
public:
    void *Init(void *arg);
};

class BONEDMODELDX8T1
{
public:
    void Destroy();
};

class BONEDMODELDX8T2
{
public:
    void Destroy();
};

class BONEDMODELDX8T3
{
public:
    void Destroy();
};

class BONEDMODELDX8T4
{
public:
    void Destroy();
};

class BONEDMODELDX8T5
{
public:
    void Destroy();
};

class BONEDMODELDX8T6
{
public:
    void Destroy();
};

class BONEDMODELDX8DISPLAYCTX
{
public:
    void ReleaseModel(void *model);
    void DeleteMaterial(void *material);
    void Release(void *value);
    void Release2(void *value);
    void Release3(void *value);
    void *CreateModel(void *device, std::string a, std::string b, std::string name);
};

void FUN_004fde40(void *value);
void FUN_004fdf80(void *value);

// The display object the renderer hangs off.
class BONEDMODELDX8DISPLAY
{
public:
    char m_00[0x2bc];
    BONEDMODELDX8DISPLAYCTX *m_2bc;
};

extern BONEDMODELDX8DISPLAY *g_pDisplay;

void __fastcall BONEDMODELDX8_UPDATE(BONEDMODELRESOURCE *self);
void __fastcall BONEDMODELDX8_STOP(BONEDMODELRESOURCE *self);

#define CMD_FUNC(CLASS, method) (CMDFUNC)(SRESULT (CLASS::*)(TOKENS **))(SRESULT (CLASS::*)(CMDCALL *))&CLASS::method

int BONEDMODELDX8SDMASTER::s_classId;
int BONEDMODELDX8SDRESOURCE::s_classId;

CMDENTRY BONEDMODELDX8SDMASTER::s_commands[] = {
    { 0, 0, 0, 0, 0 },
};

CMDENTRY BONEDMODELDX8SDRESOURCE::s_commands[] = {
    { "LoadExtraAnimation", 0, 1, 1, CMD_FUNC(BONEDMODELDX8SDRESOURCE, LoadExtraAnimation) },
    { 0, 0, 0, 0, 0 },
};

BONEDMODELTAG::BONEDMODELTAG()
{
    m_material = 0;
    m_position.Set(0.0f, 0.0f, 0.0f);
    m_name = 0;
}

BONEDMODELTAG::~BONEDMODELTAG()
{
    if (m_name) {
        delete[] m_name;
        m_name = 0;
    }
}

BONEDMODELEVENTBASE::~BONEDMODELEVENTBASE()
{
}

BONEDMODELEVENT::BONEDMODELEVENT(BONEDMODELDX8SDRESOURCE *resource, int tag)
{
    m_resource = resource;
    m_tag = tag;
    m_10 = 0;
    m_10 = -1;
}


void __fastcall BONEDMODELDX8_MASTERDESTROY(BONEDMODELMASTER *self)
{
    if (self->m_artPath.m_str) {
        free(self->m_artPath.m_str);
        self->m_artPath.m_str = 0;
    }
    if (self->m_file.m_str) {
        free(self->m_file.m_str);
        self->m_file.m_str = 0;
    }
    self->RESOURCEMASTER::~RESOURCEMASTER();
}

BONEDMODELDX8SDMASTER::BONEDMODELDX8SDMASTER(RESOURCEMANAGER *manager, const char *name)
    : BONEDMODELMASTER(manager, name)
{
    m_bounds.set = false;
}

RESOURCE *BONEDMODELDX8SDMASTER::CreateCopy(const RESHANDLE &handle)
{
    return new BONEDMODELDX8SDRESOURCE(this, handle);
}

int BONEDMODELDX8SDMASTER::GetClassId()
{
    return s_classId;
}

int BONEDMODELDX8SDMASTER::GetParentClassId()
{
    return g_classParents[s_classId];
}

const char *BONEDMODELDX8SDMASTER::GetClassName()
{
    return g_classNames[s_classId];
}

SRESULT BONEDMODELDX8SDMASTER::Execute(CMDENTRY *command, TOKENS **args)
{
    return (this->*(SRESULT (BONEDMODELDX8SDMASTER::*)(TOKENS **))command->func)(args);
}

void BONEDMODELDX8SDMASTER::RegisterCommands()
{
    g_classParents[s_classId] = BONEDMODELMASTER::s_classId;
    g_classCommands[s_classId] = s_commands;
    HashCommands(s_commands);
    RegisterClassName(s_commands, "BONEDMODELDX8SDMASTER", "BONEDMODELMASTER");
}

BONEDMODELDX8SDRESOURCE::BONEDMODELDX8SDRESOURCE(BONEDMODELMASTER *master, const RESHANDLE &handle)
    : BONEDMODELRESOURCE(master, handle), m_448(false), m_647(false)
{
    {
        BONEDMODELDX8ANIMBASE guard;
    }
    m_model = 0;
    m_loader = 0;
    m_648 = false;
    m_tags = 0;
    m_tagCount = 0;
    m_materials = 0;
    m_444 = -1;
}

BONEDMODELDX8SDRESOURCE::~BONEDMODELDX8SDRESOURCE()
{
    if (m_model) {
        DestroyAnimations();
        g_pDisplay->m_2bc->ReleaseModel(m_model);
        m_model = 0;
    }
    if (m_loader) {
        operator delete(m_loader);
        m_loader = 0;
    }
    if (m_materials) {
        operator delete[](m_materials);
        m_materials = 0;
    }
    delete[] m_tags;
    m_tags = 0;
}

int BONEDMODELDX8SDRESOURCE::GetClassId()
{
    return s_classId;
}

int BONEDMODELDX8SDRESOURCE::GetParentClassId()
{
    return g_classParents[s_classId];
}

const char *BONEDMODELDX8SDRESOURCE::GetClassName()
{
    return g_classNames[s_classId];
}

SRESULT BONEDMODELDX8SDRESOURCE::Execute(CMDENTRY *command, TOKENS **args)
{
    return (this->*(SRESULT (BONEDMODELDX8SDRESOURCE::*)(TOKENS **))command->func)(args);
}


void BONEDMODELDX8SDRESOURCE::Virtual1()
{
    Virtual28();
}

void BONEDMODELDX8SDRESOURCE::Virtual2()
{
}

void BONEDMODELDX8SDRESOURCE::Virtual3()
{
}

void BONEDMODELDX8SDRESOURCE::Virtual4()
{
}

void BONEDMODELDX8SDRESOURCE::Virtual5()
{
}

void BONEDMODELDX8SDRESOURCE::Virtual88() {}
void BONEDMODELDX8SDRESOURCE::Virtual90() {}
void BONEDMODELDX8SDRESOURCE::Virtual92() {}
void BONEDMODELDX8SDRESOURCE::Virtual93() {}
void BONEDMODELDX8SDRESOURCE::Virtual94() {}
void BONEDMODELDX8SDRESOURCE::Virtual95() {}
void BONEDMODELDX8SDRESOURCE::Virtual96() {}
void BONEDMODELDX8SDRESOURCE::Virtual97() {}
void BONEDMODELDX8SDRESOURCE::Virtual98() {}
void BONEDMODELDX8SDRESOURCE::Virtual99() {}
void BONEDMODELDX8SDRESOURCE::Virtual100() {}
void BONEDMODELDX8SDRESOURCE::Virtual102() {}
void BONEDMODELDX8SDRESOURCE::Virtual103() {}
void BONEDMODELDX8SDRESOURCE::Virtual104() {}
void BONEDMODELDX8SDRESOURCE::Virtual105() {}

void BONEDMODELDX8SDRESOURCE::Start()
{
    BONEDMODELRESOURCE::Start();
    BONEDMODELDX8ANIMBASE::SetEnabled(true);
    if (m_master) {
        *(bool *)((char *)this + 0x424) = true;
        *(int *)((char *)this + 0x420) = *(int *)((char *)m_master + 100);
    }
}

void BONEDMODELDX8SDRESOURCE::Stop()
{
    BONEDMODELDX8ANIMBASE::SetEnabled(false);
    BONEDMODELDX8_STOP(this);
}

void BONEDMODELDX8SDRESOURCE::RegisterCommands()
{
    g_classParents[s_classId] = RESOURCE::s_classId;
    g_classCommands[s_classId] = s_commands;
    HashCommands(s_commands);
    RegisterClassName(s_commands, "BONEDMODELDX8SDRESOURCE", "RESOURCE");
}

RESOURCEMASTER *BONEDMODELMANAGER::CreateMaster(const char *name)
{
    return new BONEDMODELMASTER(this, name);
}

const char *BONEDMODELMANAGER::GetTypeCode()
{
    return "BMDL";
}

RESOURCEMASTER *BONEDMODELDX8SDMANAGER::CreateMaster(const char *name)
{
    return new BONEDMODELDX8SDMASTER(this, name);
}

const char *BONEDMODELDX8SDMANAGER::GetTypeCode()
{
    return "BMD8";
}

RESOURCEMANAGER *CreateBONEDMODELDX8SD()
{
    return new BONEDMODELDX8SDMANAGER;
}


// The material list the model holds at +0xc.
class BONEDMODELDX8MATERIALS
{
public:
    std::string GetName(int index);
    void *Get(int index);
    void Set(int index, void *material);

    char m_00[0x3c];
    std::vector<std::string> m_names;
};

// The DX8 skinned model the resource loads.
class BONEDMODELDX8MODEL
{
public:
    std::string GetMaterialName(int index);
    unsigned GetMaterialCount();
    std::string *GetMaterialNamePtr(int index);
    int GetFrameCount(void *animation);
    float GetFrameRate(void *animation);
    void SetTime(float time);
    void SetAnimationFrame(void *animation, float frame);
    void UpdateBones();
    void Render(void *material);
    void *CreateAnimator();
    void *CreateAnimatorFrom(void *arg);
    void DeleteAnimation(std::string name);
    int LoadExtraAnimations(std::string name);

    char m_00[0xc];
    BONEDMODELDX8MATERIALS *m_materials;
};

class BONEDMODELDX8RENDERERDEVICE
{
public:
    virtual void D0();
    virtual void D1();
    virtual void D2();
    virtual void D3();
    virtual void D4();
    virtual void D5();
    virtual void D6();
    virtual void *GetDevice();
};

class BONEDMODELDX8RENDERER7
{
public:
    virtual void D0();
    virtual void D1();
    virtual void D2();
    virtual void D3();
    virtual void D4();
    virtual void D5();
    virtual void D6();
    virtual void *Virtual07(int a, int b, float c);
};

void __stdcall FUN_00513330(void *animation, int a, int b);
void FUN_004fde40(void *context, void *material);
void FUN_005103d0(BONEDMODELDX8MODEL *model, void *material);

void BONEDMODELDX8SDRESOURCE::Update()
{
    if (*(float *)((char *)this + 0x1e8) != g_pTimer->m_time) {
        *(float *)((char *)this + 0x1e8) = g_pTimer->m_time;
        BONEDMODELDX8_UPDATE(this);
        if (*(char *)((char *)this + 0x1c) != 0)
            Virtual89();
    }
}

void BONEDMODELDX8SDRESOURCE::Virtual91(int animation, float frame)
{
    if (m_model) {
        FUN_00513330((void *)animation, 0, 0);
        float frames = (float)((BONEDMODELDX8MODEL *)m_model)->GetFrameCount((void *)animation);
        float rate = ((BONEDMODELDX8MODEL *)m_model)->GetFrameRate((void *)animation);
        ((BONEDMODELDX8MODEL *)m_model)->SetTime(frame / (frames / rate));
    }
}

void BONEDMODELDX8SDRESOURCE::Virtual101(int a, int b)
{
    if (m_model) {
        ((BONEDMODELDX8MODEL *)m_model)->Render(
            ((BONEDMODELDX8RENDERER7 *)g_pRenderer)->Virtual07(a, b, GetAlpha()));
    }
}

void BONEDMODELDX8SDRESOURCE::Virtual71()
{
    int count = ((BONEDMODELDX8MODEL *)m_model)->m_materials->m_names.size();
    for (int i = 0; i < count; i++) {
        void *material = ((BONEDMODELDX8MODEL *)m_model)->m_materials->Get(i);
        if (material && material != ((void **)m_materials)[i]) {
            ((BONEDMODELDX8MODEL *)m_model)->m_materials->Set(i, ((void **)m_materials)[i]);
            g_pDisplay->m_2bc->DeleteMaterial(material);
        }
    }
}

void BONEDMODELDX8SDRESOURCE::RunCommand(const char *command)
{
    STRING text(command);
    g_pCmdMgr->Run(text, *(RESHANDLE *)((char *)this + 0x30));
}

void BONEDMODELDX8SDRESOURCE::Virtual89()
{
    if (m_model && !m_648) {
        ((BONEDMODELDX8MODEL *)m_model)->SetTime(*(float *)((char *)this + 0x408) * g_pTimer->m_dt);
        for (unsigned int i = 0;
             i < ((std::vector<std::string> *)((char *)m_model + 0x38))->size(); i++)
            RunCommand((*(std::vector<std::string> *)((char *)m_model + 0x38))[i].c_str());
    }
}

void BONEDMODELDX8SDRESOURCE::SetBone(int index, const V3 &position, void *placement)
{
    unsigned char *data = (unsigned char *)placement;
    MATRIX *matrix = (MATRIX *)(data + 0xc);
    if (data[0x4c] & 1) {
        matrix->FromEuler(*(float *)(data + 8), *(float *)(data + 4), *(float *)(data + 0));
        data[0x4c] &= 0xfe;
    }
    MATRIX boneMatrix = *matrix;
    boneMatrix._41 = position.x;
    boneMatrix._42 = position.y;
    boneMatrix._43 = position.z;
    BONEDMODELDX8MODEL *model = (BONEDMODELDX8MODEL *)m_model;
    void *bone = ((void **)(*(int *)(*(int *)((char *)model + 0xc) + 0x38) + 8))[index];
    *(MATRIX *)((char *)bone + 0xc4) = boneMatrix;
    *(char *)((char *)bone + 0xc0) = 1;
}

std::string BONEDMODELDX8MATERIALS::GetName(int index)
{
    return m_names[index];
}


void BONEDMODELDX8SDRESOURCE::DestroyAnimations()
{
    while (m_strings.size() != 0) {
        ((BONEDMODELDX8MODEL *)m_model)->DeleteAnimation(m_strings.front());
        m_strings.erase(m_strings.begin());
    }
}


bool BONEDMODELDX8SDRESOURCE::FUN_004f7040(const char *name)
{
    if (!m_model) {
        if (g_pLog)
            g_pLog->Error("BONEDMODELDX8SDRESOURCE::LoadExtraAnimation - model not loaded");
        return false;
    }
    std::string model(((BONEDMODELMASTER *)m_master)->m_artPath.c_str());
    int forwards = model.find_last_of("/");
    int backwards = model.find_last_of("\\");
    int cut = (forwards > backwards ? forwards : backwards) + 1;
    std::string path = model.substr(0, cut) + name;
    for (unsigned int i = 0; i < m_strings.size(); i++) {
        if (m_strings[i].compare(path) == 0)
            return true;
    }
    int loaded = ((BONEDMODELDX8MODEL *)m_model)->LoadExtraAnimations(path);
    if (loaded)
        m_strings.push_back(path);
    return loaded != 0;
}

void BONEDMODELDX8SDRESOURCE::LoadTags()
{
    std::string path(((BONEDMODELMASTER *)m_master)->m_artPath.c_str());
    size_t dot = path.find_last_of(".");
    std::string file = std::string(g_basePath) + path.substr(0, dot + 1) + "tag";
    FILE *handle = fopen(file.c_str(), "rt");
    if (!handle)
        return;
    int count = 0;
    fscanf(handle, "TotalTags:%d\n", &count);
    m_tagCount = count;
    m_tags = new BONEDMODELTAG[count];
    for (int i = 0; i < count; i++) {
        char name[128];
        char materialName[128];
        V3 a, b, c;
        fscanf(handle, "%s %s (%f,%f,%f) (%f,%f,%f) (%f,%f,%f)", name, materialName,
               &a.x, &a.y, &a.z, &b.x, &b.y, &b.z, &c.x, &c.y, &c.z);
        a.SwapYZ();
        b.SwapYZ();
        c.SwapYZ();
        int material = 0;
        unsigned materials = ((BONEDMODELDX8MODEL *)m_model)->GetMaterialCount();
        for (unsigned j = 0; j < materials; j++) {
            std::string *known = ((BONEDMODELDX8MODEL *)m_model)->GetMaterialNamePtr(j);
            if (stricmp(known->c_str(), materialName) == 0) {
                material = j;
                break;
            }
        }
        BONEDMODELTAG *tag = &m_tags[i];
        tag->m_matrix.LookAt(a, b);
        tag->m_inverse = tag->m_matrix;
        tag->m_inverse.Invert();
        tag->m_material = material;
        tag->m_position = c;
        size_t length = strlen(name) + 1;
        tag->m_name = new char[length];
        strcpy(tag->m_name, name);
    }
    fclose(handle);
}


void FUN_0046365f(MATRIX *out, float width, float height, float zNear, float zFar);

std::string BONEDMODELDX8MODEL::GetMaterialName(int index)
{
    return m_materials->GetName(index);
}

int BONEDMODELDX8SDRESOURCE::FindMaterial(const char *name)
{
    int count = ((BONEDMODELDX8MODEL *)m_model)->m_materials->m_names.size();
    for (int i = 0; i < count; i++) {
        std::string material = ((BONEDMODELDX8MODEL *)m_model)->GetMaterialName(i);
        if (stricmp(material.c_str(), name) == 0)
            return i;
    }
    if (g_pLog)
        g_pLog->Error("Error - material %s not found in boned model:%s", name,
                      ((BONEDMODELMASTER *)m_master)->m_artPath.c_str());
    return -1;
}


class BONEDMODELDX8STAGE
{
public:
    void *Get(int what);
};

class BONEDMODELDX8RENDERERAPI;

class BONEDMODELDX8SETTINGS
{
public:
    int GetInt(int index);
    float GetFloat(int index);
};

class BONEDMODELDX8SYSTEM
{
public:
    int GetCount();
    void *GetItem(int index);
    void Prepare(void *item);
    void Finish(void *item);
    void Draw(void *box);
    void Render(BONEDMODELDX8RENDERERAPI *device, void *box, int mode);
};

class BONEDMODELDX8RENDERERAPI
{
public:
    char m_00[0x90];
    void (__stdcall *m_90)(BONEDMODELDX8RENDERERAPI *self, int a, int b, int c);
    void (__stdcall *m_94)(BONEDMODELDX8RENDERERAPI *self, int a, void *value);
    char m_98[0xc0 - 0x98];
    void (__stdcall *m_c0)(BONEDMODELDX8RENDERERAPI *self, int a, void *value);
    char m_c4[4];
    void (__stdcall *m_c8)(BONEDMODELDX8RENDERERAPI *self, int a, int b);
};

class BONEDMODELDX8RENDERHELPER
{
public:
    void FUN_00516380(BONEDMODELDX8RENDERERAPI *device);
};

class BONEDMODELDX8PARTICLESYSTEM
{
public:
    void Emit(void *effect, MATRIX *matrix, V3 *zero, float scale);
};

class BONEDMODELDX8EFFECT
{
public:
    void Release(int arg);
    bool IsActive();
};

class BONEDMODELDX8PARTICLE
{
public:
    void Virtual9(int unused);
    void Virtual10(int unused);
    void Virtual11(MATRIX *matrix, BONEDMODELDX8RENDERERAPI *renderer);
    void Virtual12(BONEDMODELDX8RENDERERAPI *device);
    void Virtual13(BONEDMODELDX8RENDERERAPI *device);
    void Render(int arg);
    void Destroy();

    BONEDMODELDX8STAGE *m_0;
    BONEDMODELDX8DISPLAYCTX *m_4;
    void *m_8;
    char m_0c[8];
    void *m_14;
    BONEDMODELDX8PARTICLESYSTEM *m_18;
    void *m_1c;
    BONEDMODELDX8EFFECT *m_20;
    void *m_24;
    void *m_28;
    void *m_2c;
    void *m_30;
    void *m_34;
    void *m_38;
    char m_3c[0x1c8];
    void *m_204;
    void *m_208;
    char m_20c[4];
    char m_210[0x10];
    int m_220;
    int m_224;
    char m_228[0x28];
    BONEDMODELDX8RENDERHELPER *m_250;
    int m_254;
};

void BONEDMODELDX8PARTICLE::Virtual9(int unused)
{
    m_0->Get(0xb);
}

void BONEDMODELDX8PARTICLE::Virtual10(int unused)
{
    if (m_20) {
        m_20->Release((int)m_24);
        if (m_0->Get(0xf)) {
            if (m_20->IsActive())
                Render(unused);
        }
    }
}

void BONEDMODELDX8PARTICLE::Virtual11(MATRIX *matrix, BONEDMODELDX8RENDERERAPI *renderer)
{
    if (m_18) {
        MATRIX m = *matrix;
        V3 zero(0.0f, 0.0f, 0.0f);
        renderer->m_c8(renderer, 0xe, 0);
        m_18->Emit(renderer, &m, &zero, 1.0f);
        renderer->m_c8(renderer, 0xe, 1);
    }
}


void BONEDMODELDX8SDRESOURCE::Virtual21()
{
    void *device = ((BONEDMODELDX8RENDERERDEVICE *)g_pRenderer)->GetDevice();
    m_model = g_pDisplay->m_2bc->CreateModel(
        device, std::string(""), std::string(""),
        std::string(((BONEDMODELMASTER *)m_master)->m_artPath.c_str()));
    if (m_model) {
        void *object = operator new(0x90);
        if (object) {
            void *a = ((BONEDMODELDX8MODEL *)m_model)->CreateAnimator();
            void *b = ((BONEDMODELDX8MODEL *)m_model)->CreateAnimatorFrom(a);
            m_loader = ((BONEDMODELDX8ANIMOBJ *)object)->Init(b);
        }
    }
    int count = ((BONEDMODELDX8MODEL *)m_model)->m_materials->m_names.size();
    m_materials = operator new[](count * 4);
    for (int i = 0; i < count; i++) {
        std::string material = ((BONEDMODELDX8MODEL *)m_model)->GetMaterialName(i);
        ((void **)m_materials)[i] = ((BONEDMODELDX8MODEL *)m_model)->m_materials->Get(i);
    }
    LoadTags();
}


void BONEDMODELDX8PARTICLE::Destroy()
{
    if (m_250) {
        ((BONEDMODELDX8T1 *)m_250)->Destroy();
        operator delete(m_250);
        m_250 = 0;
    }
    m_4->Release((void *)m_254);
    if (m_8) {
        ((BONEDMODELDX8T2 *)m_8)->Destroy();
        operator delete(m_8);
        m_8 = 0;
    }
    if (m_14) {
        ((BONEDMODELDX8T3 *)m_14)->Destroy();
        operator delete(m_14);
        m_14 = 0;
    }
    if (m_208) {
        ((BONEDMODELDX8T4 *)m_208)->Destroy();
        operator delete(m_208);
        m_208 = 0;
    }
    if (m_204) {
        ((BONEDMODELDX8T4 *)m_204)->Destroy();
        operator delete(m_204);
        m_204 = 0;
    }
    m_4->Release2(m_20);
    m_4->Release3(m_18);
    m_4->Release3(m_1c);
    if (m_24) {
        ((BONEDMODELDX8T5 *)m_24)->Destroy();
        operator delete(m_24);
        m_24 = 0;
    }
    if (m_38) {
        ((BONEDMODELDX8T6 *)m_38)->Destroy();
        operator delete(m_38);
        m_38 = 0;
    }
    if (m_224 >= 0x10)
        operator delete(m_210);
    m_224 = 0xf;
    m_220 = 0;
    m_210[0] = 0;
    if (m_2c) {
        operator delete(m_2c);
    } else {
        m_2c = 0;
        m_30 = 0;
        m_34 = 0;
    }
}

void BONEDMODELDX8PARTICLE::Virtual12(BONEDMODELDX8RENDERERAPI *device)
{
    if (m_20) {
        device->m_c8(device, 0x89, 0);
        MATRIX matrix;
        FUN_0046365f(&matrix, 640.0f, 480.0f, 1.0f, 10.0f);
        device->m_94(device, 3, &matrix);
        device->m_94(device, 0x100, &matrix);
        device->m_94(device, 2, &matrix);
        m_250->FUN_00516380(device);
    }
}


class BONEDMODELDX8PARTICLEDESC
{
public:
    BONEDMODELDX8PARTICLEDESC(const BONEDMODELDX8PARTICLEDESC &other);
    BONEDMODELDX8PARTICLEDESC(std::string name, int material, int layer,
                              const V3 *position, const V3 *direction, const V3 *color);
    void Init();

    std::string m_00;
    char m_1c[0x68];
    int m_84;
    std::string m_88;
    V3 m_a4;
    V3 m_b0;
    V3 m_bc;
    int m_c8;
    int m_cc;
};

BONEDMODELDX8PARTICLEDESC::BONEDMODELDX8PARTICLEDESC(std::string name, int material, int layer,
                                                     const V3 *position, const V3 *direction,
                                                     const V3 *color)
    : m_84(material)
{
    m_88 = name;
    m_a4 = *position;
    m_b0 = *direction;
    m_bc = *color;
    m_cc = 1;
    Init();
}

BONEDMODELDX8PARTICLEDESC::BONEDMODELDX8PARTICLEDESC(const BONEDMODELDX8PARTICLEDESC &other)
    : m_84(other.m_84)
{
    m_88 = other.m_88;
    m_a4 = other.m_a4;
    m_b0 = other.m_b0;
    m_bc = other.m_bc;
    m_c8 = *(int *)((char *)&other + 0x68);
    m_cc = other.m_cc;
    Init();
}


void FUN_00461e54(V3 *value);

void BONEDMODELDX8PARTICLEDESC::Init()
{
    int *p = (int *)((char *)this + 0x1c);
    for (int i = 0; i < 0x1a; i++)
        p[i] = 0;
    int *first = p;
    switch (m_84) {
    case 1:
        first[0] = 2;
        *(int *)((char *)this + 0x68) = *(int *)((char *)this + 0xc8);
        *(int *)((char *)this + 0x50) = *(int *)((char *)this + 0xa4);
        *(int *)((char *)this + 0x54) = *(int *)((char *)this + 0xa8);
        *(int *)((char *)this + 0x58) = *(int *)((char *)this + 0xac);
        FUN_00461e54(&m_b0);
        *(int *)((char *)this + 0x5c) = *(int *)((char *)this + 0xb0);
        *(int *)((char *)this + 0x60) = *(int *)((char *)this + 0xb4);
        *(int *)((char *)this + 0x64) = *(int *)((char *)this + 0xb8);
        break;
    case 2:
    case 4:
        m_84 = 2;
        first[0] = 3;
        FUN_00461e54(&m_b0);
        *(int *)((char *)this + 0x5c) = *(int *)((char *)this + 0xb0);
        *(int *)((char *)this + 0x60) = *(int *)((char *)this + 0xb4);
        *(int *)((char *)this + 0x64) = *(int *)((char *)this + 0xb8);
        goto lab_7a50;
    case 3:
        first[0] = 2;
        *(int *)((char *)this + 0x68) = *(int *)((char *)this + 0xc8);
        *(int *)((char *)this + 0x50) = *(int *)((char *)this + 0xa4);
        *(int *)((char *)this + 0x54) = *(int *)((char *)this + 0xa8);
        *(int *)((char *)this + 0x58) = *(int *)((char *)this + 0xac);
        FUN_00461e54(&m_b0);
        *(int *)((char *)this + 0x5c) = *(int *)((char *)this + 0xb0);
        *(int *)((char *)this + 0x60) = *(int *)((char *)this + 0xb4);
        *(int *)((char *)this + 0x64) = *(int *)((char *)this + 0xb8);
        *(int *)((char *)this + 0x7c) = 0x3f000000;
        *(int *)((char *)this + 0x80) = 0x3f800000;
        *(int *)((char *)this + 0x6c) = 0x3f800000;
lab_7a4d:
        *(int *)((char *)this + 0x70) = 0x3f800000;
lab_7a50:
        *(int *)((char *)this + 0x6c) = 0x3f800000;
        *(int *)((char *)this + 0x20) = *(int *)((char *)this + 0xbc);
        *(int *)((char *)this + 0x24) = *(int *)((char *)this + 0xc0);
        *(int *)((char *)this + 0x28) = *(int *)((char *)this + 0xc4);
        *(int *)((char *)this + 0x30) = *(int *)((char *)this + 0xbc);
        *(int *)((char *)this + 0x34) = *(int *)((char *)this + 0xc0);
        *(int *)((char *)this + 0x38) = *(int *)((char *)this + 0xc4);
        return;
    default:
        first[0] = 1;
        *(int *)((char *)this + 0x68) = *(int *)((char *)this + 0xc8);
        *(int *)((char *)this + 0x50) = *(int *)((char *)this + 0xa4);
        *(int *)((char *)this + 0x54) = *(int *)((char *)this + 0xa8);
        *(int *)((char *)this + 0x58) = *(int *)((char *)this + 0xac);
        goto lab_7a4d;
    }
    *(int *)((char *)this + 0x7c) = 0x3f000000;
    *(int *)((char *)this + 0x80) = 0x3f800000;
    *(int *)((char *)this + 0x6c) = 0x3f800000;
    goto lab_7a4d;
}


class BONEDMODELDX8MATH
{
public:
    void Basis(const V3 *a, const V3 *b);
};

void BONEDMODELDX8MATH::Basis(const V3 *a, const V3 *b)
{
    MATRIX *out = (MATRIX *)this;
    V3 n = *b * kNegativeOne;
    n.Normalize();
    V3 u;
    u.x = n.y * a->z - n.z * a->y;
    u.y = n.z * a->x - n.x * a->z;
    u.z = n.x * a->y - n.y * a->x;
    u.Normalize();
    V3 v;
    v.x = n.y * u.z - n.z * u.y;
    v.y = n.z * u.x - n.x * u.z;
    v.z = n.x * u.y - n.y * u.x;
    v.Normalize();
    out->_11 = u.x;
    out->_12 = v.x;
    out->_13 = n.x;
    out->_14 = 0.0f;
    out->_21 = u.y;
    out->_22 = v.y;
    out->_23 = n.y;
    out->_24 = 0.0f;
    out->_31 = u.z;
    out->_32 = v.z;
    out->_33 = n.z;
    out->_34 = 0.0f;
    out->_41 = 0.0f;
    out->_42 = 0.0f;
    out->_43 = 0.0f;
    out->_44 = 1.0f;
}


void FUN_004626de(MATRIX *out, const MATRIX *a, const MATRIX *b);
void FUN_00462bea(MATRIX *out, int unused, const MATRIX *in);
void FUN_00462927(MATRIX *out, const MATRIX *in);
void FUN_004620d9(V3 *out, const V3 *in, const MATRIX *matrix);
void FUN_004641c8(D3DXPLANE *plane, const V3 *a, const V3 *b, const V3 *c);
void FUN_00515db0(V3 *min, V3 *max, V3 value);

struct BONEDMODELDX8BOX
{
    V3 m_corners[8];
    D3DXPLANE m_planes[6];
    V3 m_min;
    V3 m_max;
    V3 m_center;
};

void FUN_004f72c0(BONEDMODELDX8BOX *box, const MATRIX *a, const MATRIX *b)
{
    box->m_corners[0] = V3(-1.0f, -1.0f, 0.0f);
    box->m_corners[1] = V3(1.0f, -1.0f, 0.0f);
    box->m_corners[2] = V3(-1.0f, 1.0f, 0.0f);
    box->m_corners[3] = V3(1.0f, 1.0f, 0.0f);
    box->m_corners[4] = V3(-1.0f, -1.0f, 1.0f);
    box->m_corners[5] = V3(1.0f, -1.0f, 1.0f);
    box->m_corners[6] = V3(-1.0f, 1.0f, 1.0f);
    box->m_corners[7] = V3(1.0f, 1.0f, 1.0f);

    MATRIX mul, inv, basis;
    FUN_004626de(&mul, a, b);
    FUN_00462bea(&inv, 0, &inv);
    FUN_00462927(&basis, a);
    box->m_center = V3(basis._41, basis._42, basis._43);
    for (int i = 0; i < 8; i++)
        FUN_004620d9(&box->m_corners[i], &box->m_corners[i], &inv);
    box->m_min = box->m_corners[0];
    box->m_max = box->m_corners[0];
    for (int i = 1; i < 8; i++)
        FUN_00515db0(&box->m_min, &box->m_max, box->m_corners[i]);
    FUN_004641c8(&box->m_planes[0], &box->m_corners[0], &box->m_corners[1], &box->m_corners[2]);
    FUN_004641c8(&box->m_planes[1], &box->m_corners[6], &box->m_corners[7], &box->m_corners[5]);
    FUN_004641c8(&box->m_planes[2], &box->m_corners[2], &box->m_corners[6], &box->m_corners[4]);
    FUN_004641c8(&box->m_planes[3], &box->m_corners[7], &box->m_corners[3], &box->m_corners[5]);
    FUN_004641c8(&box->m_planes[4], &box->m_corners[2], &box->m_corners[3], &box->m_corners[6]);
    FUN_004641c8(&box->m_planes[5], &box->m_corners[1], &box->m_corners[0], &box->m_corners[4]);
}



struct BONEDMODELDX8CORNERBOX
{
    V3 m_a;
    V3 m_b;
    V3 m_min;
    V3 m_max;
    D3DXPLANE m_planes[6];

    void Transform(const MATRIX *matrix);
    void BuildPlanes();
};

void BONEDMODELDX8CORNERBOX::Transform(const MATRIX *matrix)
{
    V3 corner;
    FUN_004620d9(&corner, &m_a, matrix);
    m_min = corner;
    m_max = corner;
    FUN_004620d9(&corner, &m_b, matrix);
    FUN_00515db0(&m_min, &m_max, corner);
    corner = V3(m_a.x, m_a.y, m_b.z);
    FUN_004620d9(&corner, &corner, matrix);
    FUN_00515db0(&m_min, &m_max, corner);
    corner = V3(m_a.x, m_b.y, m_b.z);
    FUN_004620d9(&corner, &corner, matrix);
    FUN_00515db0(&m_min, &m_max, corner);
    corner = V3(m_a.x, m_b.y, m_a.z);
    FUN_004620d9(&corner, &corner, matrix);
    FUN_00515db0(&m_min, &m_max, corner);
    corner = V3(m_b.x, m_b.y, m_a.z);
    FUN_004620d9(&corner, &corner, matrix);
    FUN_00515db0(&m_min, &m_max, corner);
    corner = V3(m_b.x, m_a.y, m_a.z);
    FUN_004620d9(&corner, &corner, matrix);
    FUN_00515db0(&m_min, &m_max, corner);
    corner = V3(m_b.x, m_a.y, m_b.z);
    FUN_004620d9(&corner, &corner, matrix);
    FUN_00515db0(&m_min, &m_max, corner);
    BuildPlanes();
}

void FUN_004f75f0(BONEDMODELDX8BOX *box, const MATRIX *a, const MATRIX *b)
{
    box->m_corners[0] = V3(-1.0f, 1.0f, 0.0f);
    box->m_corners[1] = V3(1.0f, 1.0f, 0.0f);
    box->m_corners[2] = V3(-1.0f, -1.0f, 0.0f);
    box->m_corners[3] = V3(1.0f, -1.0f, 0.0f);
    box->m_corners[4] = V3(-1.0f, 1.0f, 1.0f);
    box->m_corners[5] = V3(1.0f, 1.0f, 1.0f);
    box->m_corners[6] = V3(-1.0f, -1.0f, 1.0f);
    box->m_corners[7] = V3(1.0f, -1.0f, 1.0f);

    MATRIX mul, inv, basis;
    FUN_004626de(&mul, a, b);
    FUN_00462bea(&inv, 0, &inv);
    FUN_00462927(&basis, a);
    box->m_center = V3(basis._41, basis._42, basis._43);
    for (int i = 0; i < 8; i++)
        FUN_004620d9(&box->m_corners[i], &box->m_corners[i], &inv);
    box->m_min = box->m_corners[0];
    box->m_max = box->m_corners[0];
    for (int i = 1; i < 8; i++)
        FUN_00515db0(&box->m_min, &box->m_max, box->m_corners[i]);
    FUN_004641c8(&box->m_planes[0], &box->m_corners[0], &box->m_corners[1], &box->m_corners[2]);
    FUN_004641c8(&box->m_planes[1], &box->m_corners[6], &box->m_corners[7], &box->m_corners[5]);
    FUN_004641c8(&box->m_planes[2], &box->m_corners[2], &box->m_corners[6], &box->m_corners[4]);
    FUN_004641c8(&box->m_planes[3], &box->m_corners[7], &box->m_corners[3], &box->m_corners[5]);
    FUN_004641c8(&box->m_planes[4], &box->m_corners[2], &box->m_corners[3], &box->m_corners[6]);
    FUN_004641c8(&box->m_planes[5], &box->m_corners[1], &box->m_corners[0], &box->m_corners[4]);
}

void FUN_004635cb(MATRIX *out, float fov, float aspect, float zNear, float zFar);
void FUN_0046378f(MATRIX *out, const void *source);

void BONEDMODELDX8PARTICLE::Virtual13(BONEDMODELDX8RENDERERAPI *device)
{
    BONEDMODELDX8SYSTEM *system = (BONEDMODELDX8SYSTEM *)m_20;
    BONEDMODELDX8SETTINGS *settings = (BONEDMODELDX8SETTINGS *)m_0;
    int count = system->GetCount();
    float aspect = (float)settings->GetInt(0) / (float)settings->GetInt(1);
    float fov = settings->GetFloat(2) * kDegToRad;
    MATRIX projection;
    FUN_004635cb(&projection, fov, aspect, 10.0f, 1500.0f);
    device->m_94(device, 3, &projection);
    device->m_c8(device, 0x16, 2);
    for (int i = 0; i < count; i++) {
        void *item = system->GetItem(i);
        system->Prepare(item);
        item = system->GetItem(i);
        V3 position = *(V3 *)((char *)item + 0x1c4);
        MATRIX world, combined, copy;
        FUN_0046378f(&world, item);
        FUN_004626de(&combined, &world, (MATRIX *)((char *)m_24 + 0xe8));
        copy = combined;
        FUN_004f75f0((BONEDMODELDX8BOX *)((char *)this + 0x120), &copy, &copy);
        position.x *= -1.0f;
        position.y *= -1.0f;
        position.z *= -1.0f;
        position.z -= 1.0f;
        device->m_c8(device, 0x98, 1);
        device->m_c0(device, 0, &position);
        Virtual11(&copy, device);
        if (m_20) {
            system->Draw((char *)this + 0x120);
            system->Render(device, (char *)this + 0x120, 0);
        }
        device->m_c8(device, 0x89, 1);
        device->m_c8(device, 0x89, 0);
        device->m_c8(device, 0x16, 3);
        device->m_c8(device, 0x16, 2);
        device->m_c8(device, 0x89, 0);
        device->m_c8(device, 0x98, 0);
        item = system->GetItem(i);
        system->Finish(item);
    }
    device->m_c8(device, 0x16, 3);
}


std::string FUN_004f82a0(std::string text)
{
    for (unsigned int i = 0; i < text.size(); i++)
        text[i] = toupper(text[i]);
    return text;
}

SRESULT BONEDMODELDX8SDRESOURCE::LoadExtraAnimation(CMDCALL *call)
{
    return SRESULT(FUN_004f7040(call->tokens->GetToken(1)));
}
