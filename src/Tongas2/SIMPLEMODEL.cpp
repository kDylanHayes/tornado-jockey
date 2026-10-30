// Turning angles to face a direction, the frustum's planes and test, and the
// render plugin's default model type: SIMPLEMODELMANAGER makes masters
// whose copies are plain objects that draw nothing. The renderer replaces it
// with its own through g_pfnCreateSIMPLEMODEL. Placeholder names apart from
// the class names.
#define ANG_INLINE_DIRECTION
#define FRUSTUM_MATHS
#include "Tongas2/CAMERA.h"
#include "Tongas2/RESOURCE.h"
#include "Tongas2/RESOURCES.h"

static inline V3 Cross(const V3 &a, const V3 &b)
{
    return V3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

static inline V3 Normal(V3 v)
{
    v.Normalize();
    return v;
}

void ANG::PointAt(const V3 &direction)
{
    if (direction.Length() < 0.05f)
        return;
    V3 forward = GetDirection();
    V3 dir = Normal(direction);
    float dot = forward.x * dir.x + forward.y * dir.y + forward.z * dir.z;
    if (dot == 1.0f)
        return;
    // Too far round to turn at once: turn halfway first.
    if (fabs(dot) < 0.707f)
        PointAt((forward + dir) / 2.0f);
    V3 up = GetUpPoint();
    V3 right = Normal(Cross(up, dir));
    V3 newUp = Normal(Cross(dir, right));
    matrix.LookAt(newUp, dir);
    flags = EULER_DIRTY;
}

V3 MATRIX::Project(const V3 &v) const
{
    V3 r = v.Transform(*this);
    float inv = 1.0f / (v.x * _14 + v.y * _24 + v.z * _34 + _44);
    V3 out = inv * r;
    return out;
}

void BOXCORNERS::Transform(const MATRIX &m)
{
    corners[0] = m.Project(corners[0]);
    corners[1] = m.Project(corners[1]);
    corners[2] = m.Project(corners[2]);
    corners[3] = m.Project(corners[3]);
    corners[4] = m.Project(corners[4]);
    corners[5] = m.Project(corners[5]);
    corners[6] = m.Project(corners[6]);
    corners[7] = m.Project(corners[7]);
}

void PLANE::Set(const V3 &p1, const V3 &p2, const V3 &p3)
{
    a = (p1.z - p2.z) * p3.y + (p2.z - p3.z) * p1.y + (p3.z - p1.z) * p2.y;
    b = (p1.x - p2.x) * p3.z + (p3.x - p1.x) * p2.z + (p2.x - p3.x) * p1.z;
    c = (p2.y - p3.y) * p1.x + (p3.y - p1.y) * p2.x + (p1.y - p2.y) * p3.x;
    d = ((p2.y * p3.z - p2.z * p3.y) * p1.x + (p1.y * p2.z - p1.z * p2.y) * p3.x +
         (p1.z * p3.y - p1.y * p3.z) * p2.x) * -1.0f;
}

int FRUSTUM::Classify(BOXCORNERS *box)
{
    int inside = 0;
    for (int i = 0; i < 6; i++) {
        int count = 8;
        int all = 1;
        for (int j = 0; j < 8; j++) {
            if (box->corners[j].x * m_planes[i].a + box->corners[j].y * m_planes[i].b +
                box->corners[j].z * m_planes[i].c + m_planes[i].d < 0.0f) {
                all = 0;
                count--;
            }
        }
        if (count <= 0)
            return 2;
        inside += all;
    }
    return inside != 6;
}

void FRUSTUM::Build(const MATRIX &inverseViewProj, const MATRIX &rotation)
{
    m_corners.corners[0].Set(-1.0f, -1.0f, 0.0f);
    m_corners.corners[1].Set(1.0f, -1.0f, 0.0f);
    m_corners.corners[2].Set(-1.0f, 1.0f, 0.0f);
    m_corners.corners[3].Set(1.0f, 1.0f, 0.0f);
    m_corners.corners[4].Set(-1.0f, -1.0f, 1.0f);
    m_corners.corners[5].Set(1.0f, -1.0f, 1.0f);
    m_corners.corners[6].Set(-1.0f, 1.0f, 1.0f);
    m_corners.corners[7].Set(1.0f, 1.0f, 1.0f);
    m_c0 = V3(0.0f, 0.0f, 1.0f).Transform(rotation);
    m_corners.Transform(inverseViewProj);
    m_planes[0] = PLANE(m_corners.corners[0], m_corners.corners[1], m_corners.corners[2]);
    m_planes[1] = PLANE(m_corners.corners[6], m_corners.corners[7], m_corners.corners[5]);
    m_planes[2] = PLANE(m_corners.corners[2], m_corners.corners[6], m_corners.corners[4]);
    m_planes[3] = PLANE(m_corners.corners[7], m_corners.corners[3], m_corners.corners[5]);
    m_planes[4] = PLANE(m_corners.corners[2], m_corners.corners[3], m_corners.corners[6]);
    m_planes[5] = PLANE(m_corners.corners[1], m_corners.corners[0], m_corners.corners[4]);
}

// A 3D value that starts at zero.
struct SIMPLEMODELV3 : V3
{
    SIMPLEMODELV3() { x = y = z = 0.0f; }
    SIMPLEMODELV3 &operator=(const V3 &v) { V3::operator=(v); return *this; }
};

class SIMPLEMODELMASTER : public RESOURCEMASTER
{
public:
    SIMPLEMODELMASTER(RESOURCEMANAGER *manager, const char *name);
    virtual ~SIMPLEMODELMASTER() {}

    virtual RESOURCE *CreateCopy(const RESHANDLE &handle);
};

class SIMPLEMODELMANAGER : public RESOURCEMANAGER
{
public:
    SIMPLEMODELMANAGER()
    {
        m_loaded = true;
        m_initialized = true;
        m_8 = false;
        m_18 = false;
    }

    virtual RESOURCEMASTER *CreateMaster(const char *name) { return new SIMPLEMODELMASTER(this, name); }
    virtual const char *GetTypeCode();
};

class SIMPLEMODEL : public OBJECT
{
public:
    SIMPLEMODEL(SIMPLEMODELMASTER *master, const RESHANDLE &handle);
    virtual ~SIMPLEMODEL() {}

    virtual void Update() { OBJECT::Update(); }
    virtual bool IsLoaded() { return true; }
    virtual V3 GetScaleFactor() { return V3(1.0f, 1.0f, 1.0f); }
    virtual V3 GetSize() { return V3(1.0f, 1.0f, 1.0f); }

    virtual void SetPivot(V3 pivot);
    virtual void Virtual71(int a, int b, int c, int d) {}
    virtual void Virtual72(int a, int b, int c, int d) {}

    SIMPLEMODELV3 m_pivot;
    SIMPLEMODELV3 m_40c;
};

const char *SIMPLEMODELMANAGER::GetTypeCode()
{
    return "WTMD";
}

static RESOURCEMANAGER *CreateSIMPLEMODEL()
{
    return new SIMPLEMODELMANAGER;
}

CREATEMANAGER g_pfnCreateSIMPLEMODEL = CreateSIMPLEMODEL;

// Zero angles, matrix still to be built.
ANG g_zeroAngles(0.0f, 0.0f, 0.0f);

SIMPLEMODELMASTER::SIMPLEMODELMASTER(RESOURCEMANAGER *manager, const char *name)
    : RESOURCEMASTER(manager, name)
{
}

void SIMPLEMODEL::SetPivot(V3 pivot)
{
    m_pivot = pivot;
}

SIMPLEMODEL::SIMPLEMODEL(SIMPLEMODELMASTER *master, const RESHANDLE &handle)
    : OBJECT(master, handle)
{
    m_scale = V3(1.0f, 1.0f, 1.0f);
}

RESOURCE *SIMPLEMODELMASTER::CreateCopy(const RESHANDLE &handle)
{
    return new SIMPLEMODEL(this, handle);
}
