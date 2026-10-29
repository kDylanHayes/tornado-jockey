#include <math.h>
#include <string.h>

#include "Tongas2/FIXEDSTRING.h"
#include "Tongas2/LOG.h"
#include "Tongas2/RESOURCES.h"

CMDENTRY RESOURCEMASTER::s_commands[] = {
    { 0, 0, 0, 0, 0 },
};

PROPENTRY RESOURCEMASTER::s_properties[] = {
    { "GroupMask", 0, 0, 0, PROP_FIELD(RESOURCEMASTER, m_groupMask), false, 1, PROP_SETTER(RESOURCEMASTER, SetGroupMask) },
    { "MinCopies", 0, 0, 0, PROP_FIELD(RESOURCEMASTER, m_minCopies), false, 1, PROP_SETTER(RESOURCEMASTER, SetMinCopies) },
    { "Description", 0, 7, 0, PROP_FIELD(RESOURCEMASTER, m_description), false, 1, PROP_SETTER(RESOURCEMASTER, SetDescription) },
    { "Preview", 0, 2, 0, PROP_FIELD(RESOURCEMASTER, m_preview), false, 1, PROP_SETTER(RESOURCEMASTER, SetPreview) },
    { 0, 0, 15, 0, 0, false },
};

CMDENTRY RESOURCE::s_commands[] = {
    { 0, 0, 0, 0, 0 },
};

PROPENTRY RESOURCE::s_properties[] = {
    { 0, 0, 15, 0, 0, false },
};

void OBJECT::UpdateAngles()
{
    if (g_pLog)
        g_pLog->Debug("%f %f %f", m_angles.GetEuler().x, m_angles.GetEuler().y, m_angles.GetEuler().z);
    if (m_parent.IsValid() && g_pLog)
        g_pLog->Debug("%f %f %f", m_relAngles.GetEuler().x, m_relAngles.GetEuler().y, m_relAngles.GetEuler().z);
}

void OBJECT::Update()
{
    if (m_parent.IsValid()) {
        WTACTOR *parent = g_pScene->GetActor(m_parent);
        if (parent && parent->m_placed) {
            FollowParent(parent);
        } else {
            Detach();
            if (!m_initialized)
                return;
        }
    }
    RESOURCE *attached = g_pResources->GetResource(m_attached);
    if (attached && m_tag) {
        attached->Update();
        if (!m_tag)
            return;
        ANG angles = m_tag->GetAngles();
        SetPosition(m_tag->GetPosition() + angles.Transform(m_offset));
        if (!m_relAngles.IsZero()) {
            if (m_54)
                angles.Rotate(m_relAngles);
            else
                angles = m_relAngles;
        }
        SetAngles(angles);
    }
    if (m_fadeDistance > 0.0f) {
        V3 camera = g_pCamera->GetWorldPosition();
        float f = m_fadeDistance / (m_position - camera).Length();
        m_drawPosition = (m_position - camera) * f + camera;
        m_drawScale = m_scale * f;
    } else {
        m_drawPosition = m_position;
        m_drawScale = m_scale;
    }
}

MATRIX OBJECT::GetWorldMatrix()
{
    MATRIX m = m_angles.GetMatrix();
    m.Invert();
    return m;
}

V3 OBJECT::WorldToLocal(const V3 &point)
{
    V3 p = point - m_position;
    p = p.Transform(GetWorldMatrix());
    p.x /= m_scale.x;
    p.y /= m_scale.y;
    p.z /= m_scale.z;
    return p;
}

V3 OBJECT::LocalToWorld(const V3 &point)
{
    V3 p = point * m_scale;
    p = p.Transform(m_angles.GetMatrix());
    p += m_position;
    return p;
}

void OBJECT::FollowParent(WTACTOR *parent)
{
    if (!g_pResources->GetResource(m_attached)) {
        if (!m_offset.IsZero()) {
            V3 position = parent->LocalToWorld(m_offset);
            SetPosition(position);
        } else {
            SetPosition(parent->GetWorldPosition());
        }
        if (!m_relAngles.IsZero()) {
            ANG angles(0.0f, 0.0f, 0.0f);
            if (m_54)
                angles.SetMatrix(m_relAngles.GetMatrix() * parent->GetWorldAngles().GetMatrix());
            else
                angles.SetMatrix(m_relAngles.GetMatrix());
            SetAngles(angles);
        } else if (m_54) {
            SetAngles(parent->GetWorldAngles());
        } else {
            SetRotation(0.0f, 0.0f, 0.0f);
        }
    }
    SetScaleV(parent->GetWorldScale() * m_scaleFactor);
    SetAlpha(parent->m_alpha * m_alphaFactor);
    if (m_a8)
        SetLayer(parent->m_layer + m_ac);
    else
        SetLayer(m_ac);
    m_fadeDistance = parent->m_fadeDistance;
    SetVisibility(parent->m_visibility);
}

void OBJECT::AttachTo(WTACTOR *actor, const char *tag)
{
    if (!actor)
        return;
    m_attached.Invalidate();
    if (m_tag) {
        delete m_tag;
        m_tag = 0;
    }
    if (!actor->IsLoaded()) {
        if (g_pLog)
            g_pLog->Error("ERROR! trying to attach to a tag %s of a WTACTOR that is not loaded yet!", tag);
        return;
    }
    m_tag = actor->GetTag(tag);
    if (!m_tag) {
        if (g_pLog)
            g_pLog->Error("ERROR! trying to attach to not existing tag %s", tag);
        return;
    }
    m_attached = actor->m_handle;
    m_tagName = tag;
}

void RESOURCEMASTER::RegisterCommands()
{
    g_classParents[s_classId] = g_CMDBASEClassId;
    g_classCommands[s_classId] = s_commands;
    HashCommands(s_commands);
    RegisterClassName(s_commands, "RESOURCEMASTER", "CMDBASE");
}

void RESOURCEMASTER::RequestCopy()
{
    if (m_numCopies < m_minCopies) {
        m_requested = m_minCopies;
        m_loaded = false;
        if (m_numCopies == 0) {
            g_pResources->m_d4++;
            m_level++;
        }
    }
}

void RESOURCEMASTER::TrimCopies()
{
    int unused = m_numCopies - m_numUsed;
    RESOURCE **list;
    if (m_numUsed > 0) {
        list = &m_free;
    } else {
        if (unused <= 1)
            return;
        if (!m_free)
            return;
        list = &m_free->m_next;
    }
    RESOURCE *copy = *list;
    while (copy) {
        RESOURCE *next = copy->m_next;
        delete copy;
        m_numCopies--;
        copy = next;
    }
    if (list)
        *list = 0;
}

void RESOURCEMASTER::CreateCopies()
{
    if (!m_prepared) {
        Prepare();
        m_prepared = true;
    }
    if (g_pGame && g_pGame->Virtual25() && m_numCopies == 0)
        m_manager->CanPreload();
    m_loaded = false;
    bool first = m_numCopies == 0;
    int count = GetCopiesPerBatch();
    while (count > 0) {
        RESOURCE *copy = CreateCopy(RESHANDLE());
        copy->Virtual21();
        copy->m_next = m_free;
        m_free = copy;
        m_numCopies++;
        count--;
    }
    if (first)
        g_pResources->Abort();
}

void RESOURCEMASTER::ReleaseCopy(RESOURCE *copy)
{
    if (copy->m_prev)
        copy->m_prev->m_next = copy->m_next;
    else
        m_used = copy->m_next;
    if (copy->m_next)
        copy->m_next->m_prev = copy->m_prev;
    copy->m_next = m_free;
    copy->m_prev = 0;
    m_free = copy;
    m_numUsed--;
    g_pResources->UnregisterCopy(copy->m_handle);
}

bool RESOURCEMASTER::Load()
{
    if (m_loaded)
        return true;
    if (m_requested > 0) {
        if (m_numCopies < m_requested) {
            for (int i = m_numCopies; i < m_requested; i++)
                CreateCopies();
            m_requested = 0;
            return false;
        }
        m_requested = 0;
    }
    RESOURCE *copy = m_used;
    while (copy) {
        RESOURCE *next = copy->m_next;
        if (!copy->IsLoaded())
            return false;
        if (m_level > 0) {
            m_level--;
            g_pResources->m_d8++;
        }
        copy = next;
    }
    copy = m_free;
    while (copy) {
        RESOURCE *next = copy->m_next;
        if (m_level > 0) {
            m_level--;
            g_pResources->m_d8++;
        }
        copy = next;
    }
    m_loaded = true;
    return true;
}

void RESOURCEMASTER::UpdateCopies()
{
    RESOURCE *copy = m_used;
    while (copy) {
        RESOURCE *next = copy->m_next;
        if (copy->m_active)
            copy->Update();
        copy = next;
    }
}

RESOURCE *RESOURCEMASTER::CreateCopy(const RESHANDLE &handle)
{
    if (g_pLog) {
        int id = m_manager->m_id;
        g_pLog->Error("ERROR! trying to create a copy of %s (%s), this resource type should NOT have copies (or there is sth wrong defined in the declarations)",
                      m_name.c_str(), RESOURCES::s_managerNames[id]);
    }
    return 0;
}

void RESOURCEMASTER::Release(bool all)
{
    Virtual20();
    RESOURCE *copy = m_used;
    while (copy) {
        RESOURCE *next = copy->m_next;
        if (all)
            copy->UpdateAngles();
        copy = next;
    }
    copy = m_free;
    while (copy) {
        RESOURCE *next = copy->m_next;
        if (all)
            copy->UpdateAngles();
        copy = next;
    }
}

void RESOURCEMASTER::ReleaseAll()
{
    Virtual20();
    RESOURCE *copy = m_used;
    while (copy) {
        RESOURCE *next = copy->m_next;
        copy->UpdateAngles();
        copy = next;
    }
    copy = m_free;
    while (copy) {
        RESOURCE *next = copy->m_next;
        copy->UpdateAngles();
        copy = next;
    }
}

void RESOURCEMASTER::ResetCopies()
{
    RESOURCE *copy = m_used;
    while (copy) {
        RESOURCE *next = copy->m_next;
        copy->Virtual28();
        copy = next;
    }
}

void RESOURCEMASTER::FreeUsed()
{
    RESOURCE *copy = m_used;
    while (copy) {
        RESOURCE *next = copy->m_next;
        copy->Virtual20();
        copy = next;
    }
    m_used = 0;
}

RESOURCEMASTER::RESOURCEMASTER(RESOURCEMANAGER *manager, const char *name)
{
    m_manager = manager;
    m_loaded = false;
    m_numCopies = 0;
    m_numUsed = 0;
    m_used = 0;
    m_free = 0;
    m_prev = 0;
    m_next = 0;
    m_prepared = false;
    m_requested = 0;
    m_level = 0;
    m_groupMask = 0;
    m_minCopies = 1;
    m_description = "";
    m_preview = false;
    m_name = name;
    m_48 = true;
}

int RESOURCEMASTER::GetClassId()
{
    return s_classId;
}

int RESOURCEMASTER::GetParentClassId()
{
    return g_classParents[s_classId];
}

const char *RESOURCEMASTER::GetClassName()
{
    return g_classNames[s_classId];
}

void RESOURCEMASTER::DeleteCopies(bool all)
{
    if (m_numCopies > 0) {
        if (all)
            FreeUsed();
        RESOURCE *copy = m_free;
        while (copy) {
            RESOURCE *next = copy->m_next;
            delete copy;
            m_numCopies--;
            copy = next;
        }
        m_free = 0;
        m_requested = 0;
    }
}

RESOURCE *RESOURCEMASTER::GetCopy()
{
    if ((RESOURCES::s_managerFlags[m_manager->m_id] & 2) && m_numUsed > 0)
        return m_used;
    if (m_numUsed == m_numCopies)
        CreateCopies();
    RESOURCE *copy = m_free;
    if (!copy) {
        if (g_pLog)
            g_pLog->Error("ERROR!! - not unused copies for resource %s", m_name.c_str());
        return 0;
    }
    m_free = copy->m_next;
    if (m_used)
        m_used->m_prev = copy;
    copy->m_next = m_used;
    copy->m_prev = 0;
    m_used = copy;
    g_pResources->RegisterCopy(copy);
    copy->Init();
    m_numUsed++;
    return copy;
}

void RESOURCEMASTER::SetMinCopies(const int &count)
{
    m_minCopies = count;
    if (RESOURCES::s_managerFlags[m_manager->m_id] & 0x10)
        m_minCopies = 0;
}

RESOURCEMASTER::~RESOURCEMASTER()
{
    DeleteCopies(true);
}

void RESOURCEMASTER::SetName(const CSTRING &path)
{
    if (path.Length() > 0) {
        FIXEDSTRING<512> fullPath;
        SafeSprintf(fullPath, "%s:%s", m_manager->GetTypeCode(), path.m_str);
        g_pResources->RenameMaster(m_handle, fullPath);
        m_name = m_path.c_str() + 5;
        FIXEDSTRING<512> fileName;
        char *out = fileName;
        for (int i = 0; i < m_path.Length(); i++) {
            char c = m_path.c_str()[i];
            if (c == '.')
                *out++ = '#';
            *out++ = c;
        }
        *out = '\0';
        m_fileName = fileName;
    }
}

SRESULT RESOURCEMASTER::Execute(CMDENTRY *command, TOKENS **args)
{
    return (this->*(SRESULT (RESOURCEMASTER::*)(TOKENS **))command->func)(args);
}

void RESOURCE::RegisterCommands()
{
    g_classParents[s_classId] = g_CMDBASEClassId;
    g_classCommands[s_classId] = s_commands;
    HashCommands(s_commands);
    RegisterClassName(s_commands, "RESOURCE", "CMDBASE");
}

int RESOURCE::GetManagerId()
{
    return m_master->m_manager->m_id;
}

RESOURCE::RESOURCE(RESOURCEMASTER *master, const RESHANDLE &handle)
{
    m_handle = handle;
    m_initialized = false;
    m_active = false;
    m_next = 0;
    m_prev = 0;
    m_master = master;
}
