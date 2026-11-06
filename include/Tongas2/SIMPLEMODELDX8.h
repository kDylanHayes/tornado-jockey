// The renderer plugin's simple models: SIMPLEMODELDX8RESOURCE draws a
// SIMPLEMODEL through the DX8 mesh code, built from named mesh parts, and
// SIMPLEMODELDX8MASTER makes its copies.
//
// The class layouts, member offsets and virtual order are the engine's; the
// DX8 copies add the custom mesh data, the mesh handles and the shader name
// to the engine SIMPLEMODEL/SIMPLEMODELMASTER bases.
#ifndef TONGAS2_SIMPLEMODELDX8_H
#define TONGAS2_SIMPLEMODELDX8_H

#include "Tongas2/CSTRING.h"
#include "Tongas2/FIXEDSTRING.h"
#include "Tongas2/LOG.h"
#include "Tongas2/RESOURCE.h"
#include "Tongas2/RESOURCES.h"
#include "Tongas2/SHADER.h"

extern RESOURCEMANAGER *g_pMESH;
extern RESOURCEMANAGER *g_pSHADER;

// A corner of a simple model's custom mesh.
struct DECALVERTEX
{
    DECALVERTEX() : position(0.0f, 0.0f, 0.0f), u(0.0f), v(0.0f) {}

    V3 position;
    int color;
    float u, v;
};

// The DX8 mesh resource, which the engine mesh code supplies. Only the
// methods the simple model calls are declared here; they sit at the same
// virtual slots as the engine's MESH.
class SIMPLEMODELMESH : public RESOURCE
{
public:
    virtual void SetColor(const V3 &color);
    virtual void SetPosition(const V3 &position);
    virtual void SetScale(const V3 &scale);
    virtual void SetAngles(const ANG &angles);
    virtual void SetLayer(int layer);
    virtual void SetVertices(DECALVERTEX *vertices, int numVertices, const short *indices,
                             int numTriangles);

    int m_flags;
};

// The mesh manager, which the renderer supplies. CreateMesh is the
// manager's first new virtual, at slot 0x20.
class SIMPLEMODELMESHMANAGER : public RESOURCEMANAGER
{
public:
    virtual RESHANDLE CreateMesh(RESHANDLE shader, int layer, const char *suffix);
};

// The engine's simple model base, shared with SIMPLEMODEL.cpp.
struct SIMPLEMODELV3 : V3
{
    SIMPLEMODELV3() { x = y = z = 0.0f; }
    SIMPLEMODELV3 &operator=(const V3 &v)
    {
        V3::operator=(v);
        return *this;
    }
};

class SIMPLEMODELMASTER : public RESOURCEMASTER
{
public:
    SIMPLEMODELMASTER(RESOURCEMANAGER *manager, const char *name);

    virtual RESOURCE *CreateCopy(const RESHANDLE &handle);
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

class SIMPLEMODELDX8MASTER : public SIMPLEMODELMASTER
{
public:
    SIMPLEMODELDX8MASTER(RESOURCEMANAGER *manager, const char *name);
    virtual ~SIMPLEMODELDX8MASTER();

    virtual RESOURCE *CreateCopy(const RESHANDLE &handle);
    virtual void UpdateCopies();

    void SetShader(const char *name);
    void AcquireShader();
    void ReleaseShader();

    // How many copies are alive, and whether the shader is held.
    int m_numResources;
    bool m_shaderLoaded;
    // The shader's name, then its handle.
    FIXEDSTRING<0x200> m_shaderName;
    RESHANDLE m_shader;
};

class SIMPLEMODELDX8RESOURCE : public SIMPLEMODEL
{
public:
    SIMPLEMODELDX8RESOURCE(SIMPLEMODELMASTER *master, const RESHANDLE &handle);
    virtual ~SIMPLEMODELDX8RESOURCE();

    virtual void Start();
    virtual void Update();
    virtual bool IsLoaded();
    virtual void SetLayer(int layer);
    virtual void Virtual71(int a, int b, int c, int d);
    virtual void Virtual72(int a, int b, int c, int d);

    // Releases the mesh copy, then the custom data.
    void ReleaseMesh();
    // Builds the DX8 mesh resource for the master, under "__spl_<name>".
    void CreateMesh();
    // Builds the custom mesh from the caller's vertices and indices.
    bool SetCustomData(V3 *vertices, int numVertices, int numFaces);
    // Fills the mesh with the part the master names, and starts it.
    void AddMeshPart();

    // The custom index and vertex data, and their sizes.
    void *m_customIndices;
    DECALVERTEX *m_customVertices;
    int m_numFaces;
    int m_numVertices;
    // The mesh copy used to draw, and the master mesh it comes from.
    RESHANDLE m_mesh;
    RESHANDLE m_meshMaster;
    bool m_440;
    bool m_441;
};

class SIMPLEMODELDX8MANAGER : public RESOURCEMANAGER
{
public:
    virtual RESOURCEMASTER *CreateMaster(const char *name);
};

#endif
