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

class MESHDX8SDMASTER : public MESHMASTER
{
public:
    MESHDX8SDMASTER(RESOURCEMANAGER *manager, const char *name);
    virtual ~MESHDX8SDMASTER();

    virtual RESOURCE *CreateCopy(const RESHANDLE &handle);
    virtual void UpdateCopies();
};

class MESHDX8SDRESOURCE : public MESH
{
public:
    MESHDX8SDRESOURCE(MESHMASTER *master, const RESHANDLE &handle);
    virtual ~MESHDX8SDRESOURCE();

    virtual void Start();
    virtual void Stop();
    virtual void Update();
};

class MESHDX8SDMANAGER : public MESHMANAGER
{
public:
    MESHDX8SDMANAGER()
    {
        m_loaded = true;
        m_initialized = true;
        m_8 = false;
        m_18 = false;
    }
    virtual ~MESHDX8SDMANAGER();

    virtual RESOURCEMASTER *CreateMaster(const char *name);
    virtual const char *GetTypeCode();
    virtual void Update();
};

RESOURCEMANAGER *CreateMESHDX8SD();

#endif
