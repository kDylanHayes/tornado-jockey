#include "Tongas2/BONEDMODEL.h"
#include "Tongas2/LOG.h"
#include "Tongas2/SAFEINDEX.h"
#include "Tongas2/SOUND.h"

#include <math.h>
#include <stdlib.h>

class BONEDMODELMANAGER : public RESOURCEMANAGER
{
public:
    BONEDMODELMANAGER()
    {
        SafeSprintf(m_fileName, "BONEDMODEL.txt");
        m_8 = false;
        m_18 = true;
    }

    virtual RESOURCEMASTER *CreateMaster(const char *name) { return new BONEDMODELMASTER(this, name); }
    virtual const char *GetTypeCode();
};

void BONEDMODELRESOURCE::Start()
{
    bool restart = false;
    if (m_parent.IsValid()) {
        WTACTOR *parent = g_pScene->GetActor(m_parent);
        if (parent) {
            if (parent->m_974)
                restart = true;
            FollowParent(parent);
        }
    }
    if (m_animation != 0) {
        int index;
        if (m_animation > 0) {
            index = m_animation - 1;
            void *animation = GetAnimation(&index);
            if (animation)
                PlayAnimation(animation, 1, 1.0f);
            else if (g_pLog)
                g_pLog->Error("BONEDMODELRESOURCE::OnActivate - Invalid animation index (+) for:%s", m_master->m_name.c_str());
        } else {
            index = abs(m_animation) - 1;
            void *animation = GetAnimation(&index);
            if (animation)
                PlayAnimation(animation, 0, 1.0f);
            else if (g_pLog)
                g_pLog->Error("BONEDMODELRESOURCE::OnActivate - Invalid animation index (-) for:%s", m_master->m_name.c_str());
        }
        m_animation = 0;
    } else if (restart) {
        RestartAnimation();
    }
}

bool BONEDMODELRESOURCE::ChangeMaterial(const char *from, const char *to)
{
    return false;
}

void BONEDMODELRESOURCE::GetBounds(V3 *min, V3 *max)
{
    BONEDMODELMASTER *master = (BONEDMODELMASTER *)m_master;
    if (!master->m_bounds.set && g_pLog)
        g_pLog->Error("ERROR! Using size without being set!! %s", master->m_name.c_str());
    *min = V3(master->m_bounds.min.x * m_scaleFactor.x, master->m_bounds.min.y * m_scaleFactor.y, master->m_bounds.min.z * m_scaleFactor.z);
    *max = V3(master->m_bounds.max.x * m_scaleFactor.x, master->m_bounds.max.y * m_scaleFactor.y, master->m_bounds.max.z * m_scaleFactor.z);
}

V3 BONEDMODELRESOURCE::GetScaleFactor()
{
    BONEDMODELV3 min;
    BONEDMODELV3 max;
    GetBounds(&min, &max);
    V3 size;
    size.x = (fabs(min.x) > fabs(max.x) ? fabs(min.x) : fabs(max.x)) * 2.0f;
    size.y = (fabs(min.y) > fabs(max.y) ? fabs(min.y) : fabs(max.y)) * 2.0f;
    size.z = (fabs(min.z) > fabs(max.z) ? fabs(min.z) : fabs(max.z)) * 2.0f;
    return size;
}

void BONEDMODELRESOURCE::SetSpeed(const float &speed)
{
    m_speed = speed;
}

static RESOURCEMANAGER *CreateBONEDMODEL();

CREATEMANAGER g_pfnCreateBONEDMODEL = CreateBONEDMODEL;

CMDENTRY BONEDMODELMASTER::s_commands[] = {
    { 0, 0, 0, 0, 0 },
};

PROPENTRY BONEDMODELMASTER::s_properties[] = {
    { "FileName", 0, 7, 0, PROP_FIELD(BONEDMODELMASTER, m_file), false, 1, PROP_SETTER(BONEDMODELMASTER, SetFileName) },
    { "RenderPassesFlags", 0, 1, 0, PROP_FIELD(BONEDMODELMASTER, m_renderPassesFlags), false, 1, PROP_SETTER(BONEDMODELMASTER, SetRenderPassesFlags) },
    { "NormalizeNormals", 0, 2, 0, PROP_FIELD(BONEDMODELMASTER, m_normalizeNormals), false, 1, PROP_SETTER(BONEDMODELMASTER, SetNormalizeNormals) },
    { 0, 0, 15, 0, 0, false },
};

void BONEDMODELMASTER::RegisterCommands()
{
    g_classParents[s_classId] = RESOURCEMASTER::s_classId;
    g_classCommands[s_classId] = s_commands;
    HashCommands(s_commands);
    RegisterClassName(s_commands, "BONEDMODELMASTER", "RESOURCEMASTER");
}

void BONEDMODELRESOURCE::FollowParent(WTACTOR *parent)
{
    OBJECT::FollowParent(parent);
    m_speed = parent->m_988;
}

const char *BONEDMODELMANAGER::GetTypeCode()
{
    return "BNMD";
}

static RESOURCEMANAGER *CreateBONEDMODEL()
{
    return new BONEDMODELMANAGER;
}

BONEDMODELMASTER::BONEDMODELMASTER(RESOURCEMANAGER *manager, const char *name)
    : RESOURCEMASTER(manager, name)
{
    m_file = "";
    m_normalizeNormals = false;
    m_renderPassesFlags = 1;
}

int BONEDMODELMASTER::GetClassId()
{
    return s_classId;
}

int BONEDMODELMASTER::GetParentClassId()
{
    return g_classParents[s_classId];
}

const char *BONEDMODELMASTER::GetClassName()
{
    return g_classNames[s_classId];
}

void BONEDMODELMASTER::SetNormalizeNormals(const bool &normalize)
{
    m_normalizeNormals = normalize;
}

BONEDMODELRESOURCE::BONEDMODELRESOURCE(BONEDMODELMASTER *master, const RESHANDLE &handle)
    : OBJECT(master, handle)
{
    if (master->m_artPath.Length() <= 0 && g_pLog)
        g_pLog->Error("Tried to create a copy on a BONEDMODELRESOURCE without path!! %s", master->m_name.c_str());
    m_scale.x = 1.0f;
    m_scale.y = 1.0f;
    m_scale.z = 1.0f;
    m_speed = 1.0f;
    m_400 = false;
    m_animation = 0;
    m_40c = 0;
}

void BONEDMODELMASTER::Prepare()
{
    FIXEDSTRING<0x200> path;
    if (m_file.Length() <= 0)
        SafeSprintf(path, "%s%s", "data/art/", m_name.c_str());
    else
        SafeSprintf(path, "%s%s", "data/art/", m_file.c_str());
    ReplaceChar(path, '/', '\\');
    m_artPath = path;
}
