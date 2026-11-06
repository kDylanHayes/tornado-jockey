// The renderer plugin's mesh objects: a manager of type "MESH" and its
// MESHMASTER-derived master and MESH copy, used for heightmap decal tiles.
//
// Discovery scaffold. The class names come from the manager's "MESH" type
// code and the MESH/MESHMASTER constructor calls in the decompiler. The
// member layouts are added as the functions are matched.
#ifndef TONGAS2_MESHDX8SD_H
#define TONGAS2_MESHDX8SD_H

#define MESH_DEFINITIONS
#include "Tongas2/DECAL.h"
#include "Tongas2/HEIGHTMAPDX8.h"

// Defined in the MESH unit; repeated here so this unit can derive from it.
class MESHMASTER : public RESOURCEMASTER
{
public:
    MESHMASTER(RESOURCEMANAGER *manager, const char *name);
    virtual ~MESHMASTER();

    virtual RESOURCE *CreateCopy(const RESHANDLE &handle);
    virtual int GetCopiesPerBatch();
    virtual void Virtual34() {}
    virtual void SetLayer(int layer) {}

    void AcquireShader();
    void ReleaseShader();
    void SetShader(const RESHANDLE &shader);

    bool m_acquired;
    RESHANDLE m_shader;
};

class MESHDX8SDRESOURCE;

class MESHDX8SDMASTER : public MESHMASTER, public HEIGHTMAPTEMP
{
public:
    MESHDX8SDMASTER(RESOURCEMANAGER *manager, const char *name);
    virtual ~MESHDX8SDMASTER();

    virtual RESOURCE *CreateCopy(const RESHANDLE &handle);
    virtual void UpdateCopies();
    virtual void ReleaseVertexBuffer();
    virtual void LockVertexBuffer();
    virtual void Render();

    void Update(bool force);
    void Stop();

    MESHDX8SDRESOURCE *m_first;
    MESHDX8SDRESOURCE *m_last;
    int m_a4;
    int m_a8;
    int m_ac;
    int m_b0;
    int m_b4;
    unsigned char m_b8;
    unsigned char m_b9;
    char m_ba[2];
    void *m_bc;
    void *m_c0;
    void *m_c4;
    void *m_c8;
};

class MESHDX8SDRESOURCE : public MESH
{
public:
    MESHDX8SDRESOURCE(MESHMASTER *master, const RESHANDLE &handle);
    virtual ~MESHDX8SDRESOURCE();

    // Clears the mesh's build state.
    __declspec(noinline) void Reset();

    virtual void Start();
    virtual void Stop();
    virtual void Update();

    V3 m_34;
    V3 m_40;
    int m_4c;
    ANG m_angles;
    void *m_a0;
    int m_a4;
    bool m_a8;
    int m_ac;
    int m_b0;
    int m_b4;
    MESHDX8SDRESOURCE *m_next;
    MESHDX8SDRESOURCE *m_prev;
};

class MESHDX8SDMANAGERBASE : public MESHMANAGER
{
public:
    MESHDX8SDMANAGERBASE()
    {
        m_loaded = true;
        m_initialized = true;
        m_8 = false;
        m_18 = false;
    }
};

class MESHDX8SDMANAGER : public MESHDX8SDMANAGERBASE
{
public:
    MESHDX8SDMANAGER() {}
    virtual ~MESHDX8SDMANAGER();

    virtual RESOURCEMASTER *CreateMaster(const char *name);
    virtual const char *GetTypeCode();
    virtual void Update();
};

RESOURCEMANAGER *CreateMESHDX8SD();

// The two start/stop hooks of the screen-decal resource that follows the
// mesh master in the address space. They only toggle the embedded
// HEIGHTMAPTEMP subobject.
class MESHDX8SDSETTER
{
public:
    __declspec(noinline) void Empty();
    void Enable();
    void Disable();

    char m_pad[0x418];
    HEIGHTMAPTEMP m_temp;
};

#endif
