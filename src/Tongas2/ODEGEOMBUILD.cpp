// Tongas2/ODEGEOMBUILD -- the ODE geometry trimesh builder and activator.
//
// Split out of ODEGEOM.cpp: with these in that translation unit VC7.1
// reorders the loop setup of DYNAMICARRAY<G2ODEINIT>::Resize, which the
// retail build keeps the other way round. On their own they are exact.
#include <string.h>

#include "Tongas2/ACTOR.h"
#include "Tongas2/MATHS.h"
#include "Tongas2/RESOURCE.h"
#include "Tongas2/DYNAMICARRAY.h"

extern "C" void *dGeomTriMeshDataCreate(void);
extern "C" void dGeomTriMeshDataBuildSingle(void *data, const void *vertices,
                                            int vertexStride, int vertexCount,
                                            const void *indices, int indexCount,
                                            int indexStride);
extern "C" void dGeomTriMeshSetData(void *geom, void *data);
extern "C" void dGeomSetBody(void *geom, void *body);

extern void *g_pOdeWorldX;

class ODEGEOMRESOURCEMASTERX
{
public:
    char pad_00[0x60];
    int m_60;
};

class ODEHELPNODE
{
public:
    void ShowHelpers(bool *visible);
};

class ODEWORLDFIND
{
public:
    WTACTOR *FindActor();
};

class G2ODESWAP
{
public:
    void Swap();
};

struct ODEHELPERITEM
{
    V3 m_pos;
    V3 m_dir;
    V3 m_scale;
    int m_value;
};

class GD549b40 : public OBJECT
{
public:
    void Cleanup();
    void Build(int vertexCount, const void *vertices, int triangleCount,
               const void *indices, bool swap);
    void AddToSpace(void *space);
    void Activate();
    void UpdateGeom();
    void AddHelper(void *source, const V3 &dir, const V3 &scale, int value);

    char pad_00[0x24];
    bool m_424;
    bool m_425;
    char pad_426[2];
    void *m_428;
    void *m_42c;
    bool m_430;
    bool m_431;
    char pad_432[2];
    void *m_434;
    void *m_438;
    void *m_43c;
    int m_440;
    void *m_444;
    int m_448;
    bool m_44c;
};

void GD549b40::Build(int vertexCount, const void *vertices, int triangleCount,
                     const void *indices, bool swap)
{
    if (!m_master)
        return;
    if (((ODEGEOMRESOURCEMASTERX *)m_master)->m_60 != 5)
        return;
    Cleanup();
    m_448 = vertexCount;
    m_440 = triangleCount;
    m_434 = dGeomTriMeshDataCreate();
    m_444 = new char[vertexCount * 12];
    memcpy(m_444, vertices, vertexCount * 12);
    m_43c = new char[triangleCount * 12];
    if (!indices)
        return;
    memcpy(m_43c, indices, triangleCount * 12);
    if (swap)
        ((G2ODESWAP *)this)->Swap();
    if (m_424) {
        bool shown = false;
        ((ODEHELPNODE *)this)->ShowHelpers(&shown);
        shown = true;
        ((ODEHELPNODE *)this)->ShowHelpers(&shown);
    }
    dGeomTriMeshDataBuildSingle(m_434, m_444, 12, vertexCount,
                                m_43c, triangleCount * 3, 12);
    if (m_428)
        dGeomTriMeshSetData(m_428, m_434);
}

void GD549b40::Activate()
{
    m_424 = false;
    m_430 = false;
    m_425 = true;
    AddToSpace(*(void **)((char *)g_pOdeWorldX + 0x60));
    m_431 = true;
    WTACTOR *actor = ((ODEWORLDFIND *)this)->FindActor();
    if (actor) {
        UpdateGeom();
        void *body = m_42c;
        if (body)
            dGeomSetBody(body, *(void **)((char *)actor + 0xccc));
        else
            dGeomSetBody(m_428, *(void **)((char *)actor + 0xccc));
        *(bool *)((char *)actor + 0xd14) = false;
    }
    m_431 = true;
}

void GD549b40::AddHelper(void *source, const V3 &dir, const V3 &scale, int value)
{
    m_430 = true;
    if (!m_44c)
        return;
    if (*(int *)((char *)this + 0x464) == *(int *)((char *)this + 0x460))
        return;
    ODEHELPERITEM item;
    item.m_pos = *(V3 *)((char *)source + 8);
    item.m_dir = dir;
    item.m_scale = scale;
    item.m_value = value;
    ((DYNAMICARRAY<ODEHELPERITEM> *)((char *)this + 0x450))->Insert(item);
}
template ODEHELPERITEM *DYNAMICARRAY<ODEHELPERITEM>::Insert(const ODEHELPERITEM &item);
