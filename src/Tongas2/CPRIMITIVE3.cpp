#include "Tongas2/CPRIMITIVE3.h"
#include "Tongas2/LOG.h"
#include "Tongas2/STRING.h"

static const char s_types[][30] = {
    "OOBox",
    "Sphere",
    "Plane",
    "Quad",
    "Segment",
    "Capsule",
    "",
};

static const char s_spaces[][30] = {
    "World",
    "Screen",
    "WorldAndScreen",
    "",
};

static RESOURCEMANAGER *CreateCPRIMITIVE3();

CREATEMANAGER g_pfnCreateCPRIMITIVE3 = CreateCPRIMITIVE3;

CMDENTRY CPRIMITIVE3MASTER::s_commands[] = {
    { 0, 0, 0, 0, 0 },
};

PROPENTRY CPRIMITIVE3MASTER::s_properties[] = {
    { "PrimitiveType", 0, 1, (int)s_types, PROP_FIELD(CPRIMITIVE3MASTER, m_primitiveType), false, 1, PROP_SETTER(CPRIMITIVE3MASTER, SetPrimitiveType) },
    { "CollisionMask", 0, 0, 0, PROP_FIELD(CPRIMITIVE3MASTER, m_collisionMask), false, 1, PROP_SETTER(CPRIMITIVE3MASTER, SetCollisionMask) },
    { "CollisionGroup", 0, 0, 0, PROP_FIELD(CPRIMITIVE3MASTER, m_collisionGroup), false, 1, PROP_SETTER(CPRIMITIVE3MASTER, SetCollisionGroup) },
    { "RegisterInManager", 0, 2, 0, PROP_FIELD(CPRIMITIVE3MASTER, m_registerInManager), false, 1, PROP_SETTER(CPRIMITIVE3MASTER, SetRegisterInManager) },
    { "BoxColor", 0, 13, 0, PROP_FIELD(CPRIMITIVE3MASTER, m_boxColor), false, 1, PROP_SETTER(CPRIMITIVE3MASTER, SetBoxColor) },
    { "CollisionTest", 0, 1, (int)s_spaces, PROP_FIELD(CPRIMITIVE3MASTER, m_collisionTest), false, 1, PROP_SETTER(CPRIMITIVE3MASTER, SetCollisionTest) },
    { 0, 0, 15, 0, 0, false },
};

// A type's name for the log.
static inline const char *TypeName(int type)
{
    return type >= 0 && type < PRIMITIVE3_NONE ? s_types[type] : "Not valid primitive";
}

void LISTNODE::Append(LISTNODE *node)
{
    if (next) {
        next->Append(node);
    } else {
        next = node;
        node->prev = this;
    }
}

void PLANE3::Set(const V3 &normal, const V3 &point)
{
    n = normal;
    d = (normal.x * point.x + normal.y * point.y + normal.z * point.z) * -1.0f;
}

OOBOX3::OOBOX3()
    : center(0.0f, 0.0f, 0.0f), size(0.0f, 0.0f, 0.0f), angles(g_zeroAngles)
{
    cornersDirty = true;
    planesDirty = true;
}

QUAD3::QUAD3()
    : cornersDirty(true), planeDirty(true), center(0.0f, 0.0f, 0.0f), size(1.0f, 1.0f, 1.0f),
      m_5c(0.0f, 0.0f, 0.0f), dirty(true)
{
}

CAPSULE3::CAPSULE3()
    : m_0(0), m_4(0), m_8(false), m_c(0.0f, 0.0f, 0.0f), m_18(0.0f, 0.0f, 1.0f), m_24(0.0f, 0.0f, 1.0f), m_30(0.0f)
{
}

void PRIMITIVE3::Clear()
{
    switch (m_type) {
    case PRIMITIVE3_OOBOX:
        if (m_data) {
            delete (OOBOX3 *)m_data;
            m_data = 0;
        }
        break;
    case PRIMITIVE3_SPHERE:
        if (m_data) {
            delete (SPHERE3 *)m_data;
            m_data = 0;
        }
        break;
    case PRIMITIVE3_PLANE:
        if (m_data) {
            delete (PLANE3 *)m_data;
            m_data = 0;
        }
        break;
    case PRIMITIVE3_QUAD:
        if (m_data) {
            delete (QUAD3 *)m_data;
            m_data = 0;
        }
        break;
    case PRIMITIVE3_SEGMENT:
        if (m_data) {
            delete (SEGMENT3 *)m_data;
            m_data = 0;
        }
        break;
    case PRIMITIVE3_CAPSULE:
        if (m_data) {
            delete (CAPSULE3 *)m_data;
            m_data = 0;
        }
        break;
    }
    m_type = PRIMITIVE3_NONE;
}

void PRIMITIVE3::SetOOBox(OOBOX3 *box)
{
    if (box) {
        Clear();
        m_data = box;
        m_type = PRIMITIVE3_OOBOX;
    }
}

void PRIMITIVE3::SetPlane(PLANE3 *plane)
{
    if (plane) {
        Clear();
        m_data = plane;
        m_type = PRIMITIVE3_PLANE;
    }
}

void PRIMITIVE3::SetSphere(SPHERE3 *sphere)
{
    if (sphere) {
        Clear();
        m_data = sphere;
        m_type = PRIMITIVE3_SPHERE;
    }
}

void PRIMITIVE3::SetQuad(QUAD3 *quad)
{
    if (quad) {
        Clear();
        m_data = quad;
        m_type = PRIMITIVE3_QUAD;
    }
}

void PRIMITIVE3::SetCapsule(CAPSULE3 *capsule)
{
    if (capsule) {
        Clear();
        m_data = capsule;
        m_type = PRIMITIVE3_CAPSULE;
    }
}

void CPRIMITIVE3RESOURCE::SetOffsetV(V3 offset)
{
    m_offset = offset;
    if (m_helpersShown) {
        for (int i = 0; i < CPRIMITIVE3_MAX_HELPERS; i++) {
            OBJECT *helper = (OBJECT *)g_pResources->GetResource(m_helpers[i]);
            if (helper)
                helper->SetOffsetV(offset);
        }
    }
}

void CPRIMITIVE3RESOURCE::SetScaleFactorV(V3 scale)
{
    m_scaleFactor = scale;
    if (m_helpersShown) {
        for (int i = 0; i < CPRIMITIVE3_MAX_HELPERS; i++) {
            OBJECT *helper = (OBJECT *)g_pResources->GetResource(m_helpers[i]);
            if (helper)
                helper->SetScaleFactorV(scale);
        }
    }
}

void CPRIMITIVE3RESOURCE::SetRelAngles(ANG angles)
{
    m_relAngles = angles;
    if (m_helpersShown) {
        for (int i = 0; i < CPRIMITIVE3_MAX_HELPERS; i++) {
            OBJECT *helper = (OBJECT *)g_pResources->GetResource(m_helpers[i]);
            if (helper)
                helper->SetRelAngles(angles);
        }
    }
}

void CPRIMITIVE3MASTER::RegisterCommands()
{
    g_classParents[s_classId] = RESOURCEMASTER::s_classId;
    g_classCommands[s_classId] = s_commands;
    HashCommands(s_commands);
    RegisterClassName(s_commands, "CPRIMITIVE3MASTER", "RESOURCEMASTER");
}

CPRIMITIVE3MASTER::CPRIMITIVE3MASTER(RESOURCEMANAGER *manager, const char *name)
    : RESOURCEMASTER(manager, name)
{
    m_boxColor = V3I(255, 100, 255);
    m_primitiveType = PRIMITIVE3_OOBOX;
    m_collisionMask = 0;
    m_collisionGroup = 0;
    m_collisionTest = PRIMITIVE3_WORLD;
    m_registerInManager = true;
}

int CPRIMITIVE3MASTER::GetClassId()
{
    return s_classId;
}

int CPRIMITIVE3MASTER::GetParentClassId()
{
    return g_classParents[s_classId];
}

const char *CPRIMITIVE3MASTER::GetClassName()
{
    return g_classNames[s_classId];
}

void CPRIMITIVE3MASTER::SetBoxColor(const V3I &color)
{
    m_boxColor = color;
}

void CPRIMITIVE3MASTER::SetPrimitiveType(const int &type)
{
    if (m_primitiveType != type) {
        m_primitiveType = type;
        RESOURCE *next;
        for (RESOURCE *copy = m_used; copy; copy = next) {
            next = copy->m_next;
            if (copy->m_active) {
                copy->Stop();
                copy->m_active = false;
            }
            copy->SetActive(true);
        }
    } else {
        m_primitiveType = type;
    }
}

// Whether p is inside the quad a, b, c, d: the edges it crosses going right.
static int PointInQuad(const V2 &a, const V2 &b, const V2 &c, const V2 &d, const V2 &p)
{
    bool inside = false;
    if (((a.y <= p.y && p.y < d.y) || (d.y <= p.y && p.y < a.y)) && p.x < (d.x - a.x) * (p.y - a.y) / (d.y - a.y) + a.x)
        inside = !inside;
    if (((b.y <= p.y && p.y < a.y) || (a.y <= p.y && p.y < b.y)) && p.x < (a.x - b.x) * (p.y - b.y) / (a.y - b.y) + b.x)
        inside = !inside;
    if (((c.y <= p.y && p.y < b.y) || (b.y <= p.y && p.y < c.y)) && p.x < (b.x - c.x) * (p.y - c.y) / (b.y - c.y) + c.x)
        inside = !inside;
    if (((d.y <= p.y && p.y < c.y) || (c.y <= p.y && p.y < d.y)) && p.x < (c.x - d.x) * (p.y - d.y) / (c.y - d.y) + d.x)
        inside = !inside;
    return inside;
}

bool CPRIMITIVE3MANAGER::Collide(PRIMITIVE3 *a, PRIMITIVE3 *b, CONTACT *ca, CONTACT *cb)
{
    bool hit = false;
    V3 pa(0.0f, 0.0f, 0.0f);
    V3 pb(0.0f, 0.0f, 0.0f);
    V3 normal(0.0f, 0.0f, 0.0f);
    float depth;
    if (a->m_type == PRIMITIVE3_OOBOX && b->m_type == PRIMITIVE3_OOBOX)
        hit = CollideBoxBox((OOBOX3 *)a->m_data, (OOBOX3 *)b->m_data, &pa, &pb, &depth);
    else if (a->m_type == PRIMITIVE3_SPHERE && b->m_type == PRIMITIVE3_SPHERE)
        hit = CollideSphereSphere((SPHERE3 *)a->m_data, (SPHERE3 *)b->m_data, &pa, &pb, &depth);
    else if (a->m_type == PRIMITIVE3_SPHERE && b->m_type == PRIMITIVE3_OOBOX)
        hit = CollideSphereBox((SPHERE3 *)a->m_data, (OOBOX3 *)b->m_data, &pa, &pb, &depth);
    else if (a->m_type == PRIMITIVE3_OOBOX && b->m_type == PRIMITIVE3_SPHERE)
        hit = CollideSphereBox((SPHERE3 *)b->m_data, (OOBOX3 *)a->m_data, &pb, &pa, &depth);
    else if (a->m_type == PRIMITIVE3_OOBOX && b->m_type == PRIMITIVE3_QUAD)
        hit = CollideBoxQuad((OOBOX3 *)a->m_data, (QUAD3 *)b->m_data, &pa, &pb, &depth, 0);
    else if (a->m_type == PRIMITIVE3_QUAD && b->m_type == PRIMITIVE3_OOBOX)
        hit = CollideBoxQuad((OOBOX3 *)b->m_data, (QUAD3 *)a->m_data, &pb, &pa, &depth, 0);
    else if (a->m_type == PRIMITIVE3_SPHERE && b->m_type == PRIMITIVE3_QUAD)
        hit = CollideSphereQuad((SPHERE3 *)a->m_data, (QUAD3 *)b->m_data, &pa, &pb, &depth);
    else if (a->m_type == PRIMITIVE3_QUAD && b->m_type == PRIMITIVE3_SPHERE)
        hit = CollideSphereQuad((SPHERE3 *)b->m_data, (QUAD3 *)a->m_data, &pb, &pa, &depth);
    else if (a->m_type == PRIMITIVE3_OOBOX && b->m_type == PRIMITIVE3_SEGMENT)
        hit = CollideBoxSegment((OOBOX3 *)a->m_data, (SEGMENT3 *)b->m_data, &pa, &pb, &depth, &normal);
    else if (a->m_type == PRIMITIVE3_SEGMENT && b->m_type == PRIMITIVE3_OOBOX)
        hit = CollideBoxSegment((OOBOX3 *)b->m_data, (SEGMENT3 *)a->m_data, &pb, &pa, &depth, &normal);
    else if (a->m_type == PRIMITIVE3_SPHERE && b->m_type == PRIMITIVE3_SEGMENT)
        hit = CollideSphereSegment((SPHERE3 *)a->m_data, (SEGMENT3 *)b->m_data, &pa, &pb, &depth, &normal);
    else if (a->m_type == PRIMITIVE3_SEGMENT && b->m_type == PRIMITIVE3_SPHERE)
        hit = CollideSphereSegment((SPHERE3 *)b->m_data, (SEGMENT3 *)a->m_data, &pb, &pa, &depth, &normal);
    else if (a->m_type == PRIMITIVE3_QUAD && b->m_type == PRIMITIVE3_SEGMENT)
        hit = CollideQuadSegment((QUAD3 *)a->m_data, (SEGMENT3 *)b->m_data, &pa, &pb, &depth, &normal);
    else if (a->m_type == PRIMITIVE3_SEGMENT && b->m_type == PRIMITIVE3_QUAD)
        hit = CollideQuadSegment((QUAD3 *)b->m_data, (SEGMENT3 *)a->m_data, &pb, &pa, &depth, &normal);
    else if (a->m_type == PRIMITIVE3_CAPSULE && b->m_type == PRIMITIVE3_OOBOX)
        hit = CollideCapsuleBox((CAPSULE3 *)a->m_data, (OOBOX3 *)b->m_data, &pa, &pb, &depth, &normal);
    else if (a->m_type == PRIMITIVE3_OOBOX && b->m_type == PRIMITIVE3_CAPSULE)
        hit = CollideCapsuleBox((CAPSULE3 *)b->m_data, (OOBOX3 *)a->m_data, &pb, &pa, &depth, &normal);
    else if (a->m_type == PRIMITIVE3_CAPSULE && b->m_type == PRIMITIVE3_SPHERE)
        hit = CollideCapsuleSphere((CAPSULE3 *)a->m_data, (SPHERE3 *)b->m_data, &pa, &pb, &depth, &normal);
    else if (a->m_type == PRIMITIVE3_SPHERE && b->m_type == PRIMITIVE3_CAPSULE)
        hit = CollideCapsuleSphere((CAPSULE3 *)b->m_data, (SPHERE3 *)a->m_data, &pb, &pa, &depth, &normal);
    else if (a->m_type == PRIMITIVE3_CAPSULE && b->m_type == PRIMITIVE3_CAPSULE)
        hit = CollideCapsuleCapsule((CAPSULE3 *)a->m_data, (CAPSULE3 *)b->m_data, &pa, &pb, &depth, &normal);
    ca->normal = normal;
    cb->normal = normal;
    ca->point = pa;
    return hit;
}

void CPRIMITIVE3RESOURCE::ShowHelper(const bool &show, int i)
{
    WTACTOR *actor = g_pActors->GetActor(m_parent);
    bool visible = show;
    if (actor)
        visible &= actor->m_a8;
    if (visible) {
        if (m_helpersShown)
            return;
        SIMPLEMODEL *helper;
        CPRIMITIVE3MASTER *master = Master();
        switch (master ? master->m_primitiveType : -1) {
        case PRIMITIVE3_SPHERE:
            helper = (SIMPLEMODEL *)((RESOURCEMANAGER *)g_pSIMPLEMODEL)->GetResource("sph");
            helper->SetScaleFactor(GetSize().x, GetSize().x, GetSize().x);
            break;
        case PRIMITIVE3_PLANE:
            helper = (SIMPLEMODEL *)g_pSIMPLEMODEL->GetResource("box");
            helper->SetScaleFactor(100.0f, 0.1f, 100.0f);
            break;
        case PRIMITIVE3_QUAD:
            helper = (SIMPLEMODEL *)g_pSIMPLEMODEL->GetResource("box");
            helper->SetScaleFactorV(V3(GetSize().x, 0.1f, GetSize().z));
            break;
        case PRIMITIVE3_OOBOX:
            helper = (SIMPLEMODEL *)g_pSIMPLEMODEL->GetResource("box");
            helper->SetScaleFactorV(GetSize());
            break;
        case PRIMITIVE3_CAPSULE:
            helper = (SIMPLEMODEL *)g_pSIMPLEMODEL->GetResource("cylinder");
            helper->SetScaleFactorV(V3(GetSize().x, GetSize().y, GetSize().x));
            break;
        default:
            if (g_pLog)
                g_pLog->Error("Error - CPRIMITIVE3RESOURCE::ShowHelpers> Primitive %s not supported", TypeName(Master()->m_primitiveType));
            return;
        }
        if (actor && actor->m_98)
            helper->SetColor(actor->m_9c);
        else
            helper->SetColor(Master()->m_boxColor);
        helper->SetAlphaFactor(0.5f);
        helper->SetOffsetV(GetOffset());
        helper->SetRelAngles(GetRelAngles());
        helper->SetParent(m_parent);
        helper->m_a8 = false;
        helper->SetRelLayer(125);
        WTACTOR *attached = (WTACTOR *)g_pResources->GetResource(m_attached);
        if (attached)
            helper->AttachTo(attached, m_tagName);
        helper->SetActive(true);
        m_helpers[i] = helper->m_handle;
        m_helpersShown = true;
    } else if (m_helpersShown) {
        RESOURCE *helper = g_pResources->GetResource(m_helpers[i]);
        if (helper)
            helper->Virtual20();
        m_helpers[i].Invalidate();
        m_helpersShown = false;
    }
}

static RESOURCEMANAGER *CreateCPRIMITIVE3()
{
    return new CPRIMITIVE3MANAGER;
}

CPRIMITIVE3MANAGER::CPRIMITIVE3MANAGER()
{
    SafeSprintf(m_fileName, "CPRIMITIVE3.txt");
    m_8 = false;
    m_18 = true;
}

const char *CPRIMITIVE3MANAGER::GetTypeCode()
{
    return "CLP3";
}

RESOURCEMASTER *CPRIMITIVE3MANAGER::CreateMaster(const char *name)
{
    return new CPRIMITIVE3MASTER(this, name);
}

void PRIMITIVE3::Update(const V3 &position, const ANG &angles, const V3 &scale)
{
    switch (m_type) {
    case PRIMITIVE3_OOBOX:
        ((OOBOX3 *)m_data)->SetCenter(position);
        ((OOBOX3 *)m_data)->SetSize(scale);
        ((OOBOX3 *)m_data)->SetAngles(angles);
        break;
    case PRIMITIVE3_SPHERE: {
        ((SPHERE3 *)m_data)->center = position;
        float radius = scale.x * 0.5f;
        SPHERE3 *sphere = (SPHERE3 *)m_data;
        if (sphere->radius != radius) {
            sphere->radius = radius;
            sphere->dirty = true;
        }
        break;
    }
    case PRIMITIVE3_PLANE:
        ((PLANE3 *)m_data)->Set(((ANG &)angles).GetUpPoint(), position);
        break;
    case PRIMITIVE3_QUAD:
        ((QUAD3 *)m_data)->SetCenter(position);
        ((QUAD3 *)m_data)->SetSize(V3(scale.x, 0.0f, scale.z));
        ((QUAD3 *)m_data)->SetAngles(angles);
        break;
    case PRIMITIVE3_CAPSULE: {
        float radius = scale.x * 0.5f;
        ((CAPSULE3 *)m_data)->Set(position, radius, scale.y, angles);
        break;
    }
    }
}

CPRIMITIVE3RESOURCE::CPRIMITIVE3RESOURCE(CPRIMITIVE3MASTER *master, const RESHANDLE &handle)
    : OBJECT(master, handle)
{
    m_helpersShown = false;
    m_hit = false;
}

CPRIMITIVE3RESOURCE::~CPRIMITIVE3RESOURCE()
{
}

void CPRIMITIVE3RESOURCE::Start()
{
    m_helpersShown = false;
    m_hit = false;
    CPRIMITIVE3MASTER *master = Master();
    if (master) {
        switch (master->m_primitiveType) {
        case PRIMITIVE3_OOBOX:
            m_primitive.SetOOBox(new OOBOX3);
            break;
        case PRIMITIVE3_SPHERE:
            m_primitive.SetSphere(new SPHERE3);
            break;
        case PRIMITIVE3_PLANE:
            m_primitive.SetPlane(new PLANE3);
            break;
        case PRIMITIVE3_QUAD:
            m_primitive.SetQuad(new QUAD3);
            break;
        case PRIMITIVE3_CAPSULE:
            m_primitive.SetCapsule(new CAPSULE3);
            break;
        default:
            if (g_pLog)
                g_pLog->Error("Error - CPRIMITIVE3RESOURCE::OnActivate> Primitive %s not supported for attaching to actor", TypeName(master->m_primitiveType));
            break;
        }
    }
    m_showHelpers = true;
    if (Master() && Master()->m_registerInManager) {
        CPRIMITIVE3MANAGER *manager = (CPRIMITIVE3MANAGER *)g_pCPRIMITIVE3;
        manager->m_list.Add(this);
        manager->m_changed = true;
    }
}

void CPRIMITIVE3RESOURCE::ShowAllHelpers(const bool &show)
{
    int count = GetHelperCount();
    for (int i = 0; i < count; i++)
        ShowHelper(show, i);
}

void CPRIMITIVE3MASTER::ShowHelpers(const bool &show)
{
    RESOURCE *next;
    for (RESOURCE *copy = m_used; copy; copy = next) {
        next = copy->m_next;
        if (copy->m_active) {
            CPRIMITIVE3RESOURCE *primitive = (CPRIMITIVE3RESOURCE *)copy;
            int count = primitive->GetHelperCount();
            for (int i = 0; i < count; i++)
                primitive->ShowHelper(show, i);
        }
    }
}

void CPRIMITIVE3MANAGER::Init()
{
    RESOURCEMANAGER::Init();
    m_showHelpers = false;
    m_changed = false;
    m_listener = 0;
    m_contacts.Resize(100);
}

void CPRIMITIVE3MANAGER::ToggleHelpers()
{
    m_showHelpers = !m_showHelpers;
    RESOURCEMASTER *next;
    for (RESOURCEMASTER *master = m_first; master; master = next) {
        next = master->m_next;
        ((CPRIMITIVE3MASTER *)master)->ShowHelpers(m_showHelpers);
    }
}

void CPRIMITIVE3MANAGER::TestScreen(CPRIMITIVE3RESOURCE *a, CPRIMITIVE3RESOURCE *b)
{
    if (!(a->GetCollisionMask() & b->GetCollisionGroup()))
        return;
    bool hit = false;
    if (a->IsType(PRIMITIVE3_OOBOX)) {
        PRIMV2 corners[8];
        V3 *points = a->m_primitive.GetOOBox()->GetCorners();
        PRIMV2 point;
        ((CAMERA *)g_pCamera)->WorldToScreen(b->GetPosition(), &point);
        for (int i = 0; i < 8; i++)
            ((CAMERA *)g_pCamera)->WorldToScreen(points[i], &corners[i]);
        hit = PointInQuad(corners[0], corners[1], corners[3], corners[2], point)
            || PointInQuad(corners[5], corners[4], corners[6], corners[7], point)
            || PointInQuad(corners[4], corners[5], corners[1], corners[0], point)
            || PointInQuad(corners[2], corners[3], corners[7], corners[6], point)
            || PointInQuad(corners[4], corners[0], corners[2], corners[6], point)
            || PointInQuad(corners[1], corners[5], corners[7], corners[3], point);
    } else if (b->IsType(PRIMITIVE3_OOBOX)) {
        PRIMV2 corners[8];
        V3 *points = b->m_primitive.GetOOBox()->GetCorners();
        PRIMV2 point;
        ((CAMERA *)g_pCamera)->WorldToScreen(a->GetPosition(), &point);
        for (int i = 0; i < 8; i++)
            ((CAMERA *)g_pCamera)->WorldToScreen(points[i], &corners[i]);
        hit = PointInQuad(corners[0], corners[1], corners[3], corners[2], point)
            || PointInQuad(corners[5], corners[4], corners[6], corners[7], point)
            || PointInQuad(corners[4], corners[5], corners[1], corners[0], point)
            || PointInQuad(corners[2], corners[3], corners[7], corners[6], point)
            || PointInQuad(corners[4], corners[0], corners[2], corners[6], point)
            || PointInQuad(corners[1], corners[5], corners[7], corners[3], point);
    } else if (a->IsType(PRIMITIVE3_SPHERE)) {
        PRIMV2 center;
        PRIMV2 edge;
        PRIMV2 point;
        ((CAMERA *)g_pCamera)->WorldToScreen(b->GetPosition(), &point);
        float radius = a->m_primitive.GetSphere()->radius;
        ((CAMERA *)g_pCamera)->WorldToScreen(a->m_primitive.GetSphere()->center, &center);
        V3 offset = radius * a->GetAngles().GetRightPoint();
        ((CAMERA *)g_pCamera)->WorldToScreen(a->m_primitive.GetSphere()->center + offset, &edge);
        hit = (center - point).Length() < (edge - center).Length();
    } else if (b->IsType(PRIMITIVE3_SPHERE)) {
        PRIMV2 center;
        PRIMV2 edge;
        PRIMV2 point;
        ((CAMERA *)g_pCamera)->WorldToScreen(a->GetPosition(), &point);
        float radius = b->m_primitive.GetSphere()->radius;
        ((CAMERA *)g_pCamera)->WorldToScreen(b->m_primitive.GetSphere()->center, &center);
        V3 offset = radius * b->GetAngles().GetRightPoint();
        ((CAMERA *)g_pCamera)->WorldToScreen(b->m_primitive.GetSphere()->center + offset, &edge);
        hit = (center - point).Length() < (edge - center).Length();
    }
    if (hit) {
        a->m_hit = true;
        b->m_hit = true;
        WTACTOR *actorA = g_pActors->GetActor(a->GetParent());
        WTACTOR *actorB = g_pActors->GetActor(b->GetParent());
        if (actorA)
            actorA->OnCollide(actorB);
        if (actorB)
            actorB->OnCollide(actorA);
    }
}

void CPRIMITIVE3MANAGER::Test(CPRIMITIVE3RESOURCE *a, CPRIMITIVE3RESOURCE *b, DYNAMICARRAY<CONTACT> *contacts)
{
    CONTACT ca;
    CONTACT cb;
    if (!(a->GetCollisionMask() & b->GetCollisionGroup()))
        return;
    if (a->GetParent() == b->GetParent())
        return;
    ca.Reset();
    cb.Reset();
    if (Collide(&a->m_primitive, &b->m_primitive, &ca, &cb)) {
        a->m_hit = true;
        b->m_hit = true;
        ca.self = a;
        ca.other = b;
        cb.self = b;
        cb.other = a;
        if (contacts->m_count < contacts->m_capacity) {
            contacts->Insert(ca);
            if (contacts->m_count < contacts->m_capacity)
                contacts->Insert(cb);
        }
    }
}

RESOURCE *CPRIMITIVE3MASTER::CreateCopy(const RESHANDLE &handle)
{
    return new CPRIMITIVE3RESOURCE(this, handle);
}

SRESULT CPRIMITIVE3MASTER::Execute(CMDENTRY *command, TOKENS **args)
{
    return (this->*(SRESULT (CPRIMITIVE3MASTER::*)(TOKENS **))command->func)(args);
}

void CPRIMITIVE3RESOURCE::Stop()
{
    ShowAllHelpers(false);
    if (Master() && Master()->m_registerInManager) {
        CPRIMITIVE3MANAGER *manager = (CPRIMITIVE3MANAGER *)g_pCPRIMITIVE3;
        manager->m_list.Remove(this);
        manager->m_changed = true;
    }
}

void CPRIMITIVE3RESOURCE::Update()
{
    OBJECT::Update();
    m_primitive.Update(GetPosition(), GetAngles(), GetScale());
    if (m_showHelpers && ((CPRIMITIVE3MANAGER *)g_pCPRIMITIVE3)->m_showHelpers) {
        ShowAllHelpers(true);
        m_showHelpers = false;
    }
    for (int i = 0; i < CPRIMITIVE3_MAX_HELPERS; i++) {
        OBJECT *helper = (OBJECT *)g_pResources->GetResource(m_helpers[i]);
        if (helper) {
            if (m_hit)
                helper->SetAlphaFactor(0.9f);
            else
                helper->SetAlphaFactor(0.5f);
        }
    }
    m_hit = false;
}

void CPRIMITIVE3MANAGER::UpdateCollisions()
{
    m_contacts.Clear();
    for (CPRIMITIVE3RESOURCE *a = m_list.m_head; a && m_contacts.m_count < m_contacts.m_capacity; a = (CPRIMITIVE3RESOURCE *)a->next) {
        for (CPRIMITIVE3RESOURCE *b = m_list.Next(a); b; b = (CPRIMITIVE3RESOURCE *)b->next) {
            if (a == b)
                continue;
            if (a->Master()->m_collisionTest && b->Master()->m_collisionTest)
                TestScreen(a, b);
            if (a->Master()->m_collisionTest != PRIMITIVE3_SCREEN && b->Master()->m_collisionTest != PRIMITIVE3_SCREEN)
                Test(a, b, &m_contacts);
        }
    }
    for (int i = m_contacts.First(); i != -1; i = m_contacts.Next(i)) {
        CONTACT *contact = m_contacts.At(i);
        if (!contact) {
            if (g_pLog)
                g_pLog->Error("null contact");
        } else if (contact->self && g_pActors) {
            WTACTOR *actor = g_pActors->GetActor(contact->self->m_parent);
            if (actor)
                actor->OnContact(contact);
        }
    }
}

void CPRIMITIVE3MANAGER::Update()
{
    RESOURCEMANAGER::Update();
    UpdateCollisions();
    if (m_changed && m_listener)
        m_listener->OnChanged();
    m_changed = false;
}
