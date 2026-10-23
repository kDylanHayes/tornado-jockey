// Collision primitives: a CPRIMITIVE3MASTER says what shape its copies
// take (a box, a sphere, a plane, a quad or a capsule), what they collide
// with and whether the manager tests them. A CPRIMITIVE3RESOURCE follows
// its parent actor, can show helper models of its shape, and the manager
// tests the registered ones against each other, in the world or on the
// screen, telling the actors what they touched. The shape maths is in
// another unit. Placeholder names apart from the class names, properties
// and what the log messages give.
#ifndef TONGAS2_CPRIMITIVE3_H
#define TONGAS2_CPRIMITIVE3_H

#include "Tongas2/DECAL.h"
#include "Tongas2/DYNAMICARRAY.h"
#include "Tongas2/RESOURCE.h"
#include "Tongas2/RESOURCES.h"
#include "Tongas2/SRESULT.h"

class CPRIMITIVE3RESOURCE;

// The shapes, in the order s_types names them.
enum
{
    PRIMITIVE3_OOBOX,
    PRIMITIVE3_SPHERE,
    PRIMITIVE3_PLANE,
    PRIMITIVE3_QUAD,
    PRIMITIVE3_SEGMENT,
    PRIMITIVE3_CAPSULE,
    PRIMITIVE3_NONE
};

// Where the manager tests a primitive, in the order s_spaces names them.
enum
{
    PRIMITIVE3_WORLD,
    PRIMITIVE3_SCREEN,
    PRIMITIVE3_WORLDANDSCREEN
};

// A vector, zero until set.
struct PRIMV3 : V3
{
    PRIMV3() : V3(0.0f, 0.0f, 0.0f) {}
    PRIMV3 &operator=(const V3 &other)
    {
        V3::operator=(other);
        return *this;
    }
};

// A point on the screen, zero until set.
struct PRIMV2 : V2
{
    PRIMV2() : V2(0.0f, 0.0f) {}
};

// Defined elsewhere: the up vector, and angles that do not turn.
extern V3 g_up;
extern ANG g_zeroAngles;

// The plane through point with the normal: n.p + d = 0.
struct PLANE3
{
    PLANE3() { n.x = n.y = n.z = d = 0.0f; }
    void Set(const V3 &normal, const V3 &point);

    V3 n;
    float d;
};

struct SPHERE3
{
    SPHERE3() : center(0.0f, 0.0f, 0.0f), radius(0.0f), m_10(0.0f), dirty(false) {}

    V3 center;
    float radius;
    float m_10;
    bool dirty;
};

// An oriented box.
struct OOBOX3
{
    OOBOX3();

    // Defined elsewhere.
    void SetCenter(const V3 &center);
    void SetSize(const V3 &size);
    void SetAngles(const ANG &angles);
    // The eight corners, brought up to date.
    V3 *GetCorners();

    V3 center;
    V3 size;
    ANG angles;
    PRIMV3 corners[8];
    PLANE3 planes[6];
    bool cornersDirty;
    bool planesDirty;
};

// A rectangle in space.
struct QUAD3
{
    QUAD3();

    // Defined elsewhere.
    void SetCenter(const V3 &center);
    void SetSize(const V3 &size);
    void SetAngles(const ANG &angles);

    PLANE3 plane;
    PRIMV3 corners[4];
    bool cornersDirty;
    bool planeDirty;
    V3 center;
    V3 size;
    V3 m_5c;
    char m_68[0x40];
    bool dirty;
};

struct CAPSULE3
{
    CAPSULE3();

    // Defined elsewhere.
    void Set(const V3 &center, const float &radius, const float &height, const ANG &angles);

    int m_0;
    int m_4;
    bool m_8;
    V3 m_c;
    V3 m_18;
    V3 m_24;
    float m_30;
};

// Not made by this unit; its layout is not known.
struct SEGMENT3
{
};

// Defined elsewhere: whether two shapes touch, with the points of contact,
// how deep, and (for some) the normal.
bool CollideBoxBox(OOBOX3 *a, OOBOX3 *b, V3 *pa, V3 *pb, float *depth);
bool CollideSphereSphere(SPHERE3 *a, SPHERE3 *b, V3 *pa, V3 *pb, float *depth);
bool CollideSphereBox(SPHERE3 *a, OOBOX3 *b, V3 *pa, V3 *pb, float *depth);
bool CollideBoxQuad(OOBOX3 *a, QUAD3 *b, V3 *pa, V3 *pb, float *depth, int flags);
bool CollideSphereQuad(SPHERE3 *a, QUAD3 *b, V3 *pa, V3 *pb, float *depth);
bool CollideBoxSegment(OOBOX3 *a, SEGMENT3 *b, V3 *pa, V3 *pb, float *depth, V3 *normal);
bool CollideSphereSegment(SPHERE3 *a, SEGMENT3 *b, V3 *pa, V3 *pb, float *depth, V3 *normal);
bool CollideQuadSegment(QUAD3 *a, SEGMENT3 *b, V3 *pa, V3 *pb, float *depth, V3 *normal);
bool CollideCapsuleBox(CAPSULE3 *a, OOBOX3 *b, V3 *pa, V3 *pb, float *depth, V3 *normal);
bool CollideCapsuleSphere(CAPSULE3 *a, SPHERE3 *b, V3 *pa, V3 *pb, float *depth, V3 *normal);
bool CollideCapsuleCapsule(CAPSULE3 *a, CAPSULE3 *b, V3 *pa, V3 *pb, float *depth, V3 *normal);

// One of the shapes, owned.
class PRIMITIVE3
{
public:
    PRIMITIVE3()
    {
        m_data = 0;
        m_type = PRIMITIVE3_NONE;
    }
    virtual ~PRIMITIVE3()
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

    // Deletes the shape.
    void Clear();
    // Takes the shape, deleting the one before.
    void SetOOBox(OOBOX3 *box);
    void SetPlane(PLANE3 *plane);
    void SetSphere(SPHERE3 *sphere);
    void SetQuad(QUAD3 *quad);
    void SetCapsule(CAPSULE3 *capsule);
    // Places the shape.
    void Update(const V3 &position, const ANG &angles, const V3 &scale);

    OOBOX3 *GetOOBox() { return m_type == PRIMITIVE3_OOBOX ? (OOBOX3 *)m_data : 0; }
    SPHERE3 *GetSphere() { return m_type == PRIMITIVE3_SPHERE ? (SPHERE3 *)m_data : 0; }

    int m_type;
    void *m_data;
};

// Where two primitives touch, for the actors.
struct CONTACT
{
    CONTACT() { Reset(); }
    // Forgets the contact, but not the normal.
    void Reset()
    {
        point = g_up;
        m_c = 0;
        self = 0;
        other = 0;
    }

    PRIMV3 point;
    int m_c;
    CPRIMITIVE3RESOURCE *self;
    CPRIMITIVE3RESOURCE *other;
    PRIMV3 normal;
};

// A link of an intrusive list.
struct LISTNODE
{
    LISTNODE() : next(0), prev(0) {}
    // Puts node after the last of this one's.
    void Append(LISTNODE *node);

    LISTNODE *next;
    LISTNODE *prev;
};

// The items that are in it, linked through their LISTNODE base.
template <class T>
class LIST
{
public:
    LIST() : m_head(0) {}
    virtual ~LIST();

    // The item after node's, or 0.
    static T *Next(LISTNODE *node) { return static_cast<T *>(node->next); }
    // Puts item at the front.
    void Add(T *item);
    void Remove(T *item);

    T *m_head;
};

template <class T>
LIST<T>::~LIST()
{
    T *item = m_head;
    while (item) {
        T *next = static_cast<T *>(item->next);
        Remove(item);
        delete item;
        item = next;
    }
}

template <class T>
void LIST<T>::Add(T *item)
{
    if (m_head)
        item->Append(m_head);
    m_head = item;
}

template <class T>
void LIST<T>::Remove(T *item)
{
    if (m_head == item)
        m_head = static_cast<T *>(item->next);
    if (item->prev) {
        item->prev->next = item->next;
        if (item->next) {
            item->next->prev = item->prev;
            item->next = 0;
        }
        item->prev = 0;
    } else if (item->next) {
        item->next->prev = 0;
        item->next = 0;
    }
}

// Told when the registered primitives change.
class CPRIMITIVE3LISTENER
{
public:
    virtual void OnChanged();
};

class CPRIMITIVE3MANAGER : public RESOURCEMANAGER
{
public:
    CPRIMITIVE3MANAGER();

    virtual RESOURCEMASTER *CreateMaster(const char *name);
    virtual const char *GetTypeCode();
    virtual void Update();
    virtual void Init();

    // Shows or hides every primitive's helpers.
    void ToggleHelpers();
    // Whether two primitives touch, and where.
    bool Collide(PRIMITIVE3 *a, PRIMITIVE3 *b, CONTACT *ca, CONTACT *cb);
    // Whether a's shape covers b on the screen; tells their actors.
    void TestScreen(CPRIMITIVE3RESOURCE *a, CPRIMITIVE3RESOURCE *b);
    // Whether the two touch in the world; adds the contacts.
    void Test(CPRIMITIVE3RESOURCE *a, CPRIMITIVE3RESOURCE *b, DYNAMICARRAY<CONTACT> *contacts);
    // Tests every pair, then gives each actor its contacts.
    void UpdateCollisions();

    LIST<CPRIMITIVE3RESOURCE> m_list;
    DYNAMICARRAY<CONTACT> m_contacts;
    bool m_showHelpers;
    CPRIMITIVE3LISTENER *m_listener;
    bool m_changed;
};

extern RESOURCEMANAGER *g_pCPRIMITIVE3;

class CPRIMITIVE3MASTER : public RESOURCEMASTER
{
public:
    CPRIMITIVE3MASTER(RESOURCEMANAGER *manager, const char *name);

    static void RegisterCommands();

    virtual int GetClassId();
    virtual int GetParentClassId();
    virtual const char *GetClassName();
    virtual SRESULT Execute(CMDENTRY *command, TOKENS **args);
    virtual RESOURCE *CreateCopy(const RESHANDLE &handle);

    // Restarts the active copies with the new shape.
    virtual void SetPrimitiveType(const int &type);
    virtual void SetCollisionMask(const int &mask) { m_collisionMask = mask; }
    virtual void SetCollisionGroup(const int &group) { m_collisionGroup = group; }
    virtual void SetRegisterInManager(const bool &value) { m_registerInManager = value; }
    virtual void SetBoxColor(const V3I &color);
    virtual void SetCollisionTest(const int &test) { m_collisionTest = test; }

    // Shows or hides the active copies' helpers.
    void ShowHelpers(const bool &show);

    static int s_classId;
    static CMDENTRY s_commands[];
    static PROPENTRY s_properties[];

    int m_primitiveType;
    // What the copies hit (the other's group), and what hits them.
    int m_collisionMask;
    int m_collisionGroup;
    bool m_registerInManager;
    V3I m_boxColor;
    int m_collisionTest;
};

// Defined elsewhere: a copy of a simple model, as the helpers are.
class SIMPLEMODEL : public OBJECT
{
public:
    virtual void SetColor(V3I color);
};

extern RESOURCEMANAGER *g_pSIMPLEMODEL;

#define CPRIMITIVE3_MAX_HELPERS 3

class CPRIMITIVE3RESOURCE : public OBJECT, public LISTNODE
{
public:
    CPRIMITIVE3RESOURCE(CPRIMITIVE3MASTER *master, const RESHANDLE &handle);
    virtual ~CPRIMITIVE3RESOURCE();

    // Makes the shape and registers it with the manager.
    virtual void Start();
    virtual void Stop();
    virtual void Update();
    virtual bool IsLoaded() { return true; }
    virtual V3 GetScaleFactor() { return GetScale(); }
    // These also move the helpers.
    virtual void SetOffsetV(V3 offset);
    virtual void SetScaleFactorV(V3 scale);
    virtual V3 GetSize() { return m_scaleFactor; }
    virtual void SetRelAngles(ANG angles);

    CPRIMITIVE3MASTER *Master() { return (CPRIMITIVE3MASTER *)m_master; }
    int GetCollisionMask() { return Master() ? Master()->m_collisionMask : 0; }
    int GetCollisionGroup() { return Master() ? Master()->m_collisionGroup : 0; }
    bool IsType(int type) { return Master() && Master()->m_primitiveType == type; }
    int GetHelperCount()
    {
        int count = 1;
        if (Master() && Master()->m_primitiveType == PRIMITIVE3_CAPSULE)
            count = 3;
        return count;
    }

    // Shows or hides helper i, a model of the shape.
    void ShowHelper(const bool &show, int i);
    // A capsule has three helpers (a cylinder, for now), the rest one.
    void ShowAllHelpers(const bool &show);

    RESHANDLE m_helpers[CPRIMITIVE3_MAX_HELPERS];
    bool m_helpersShown;
    // Whether to show the helpers once the manager does.
    bool m_showHelpers;
    PRIMITIVE3 m_primitive;
    // Whether anything hit it this frame.
    bool m_hit;
};

#endif
