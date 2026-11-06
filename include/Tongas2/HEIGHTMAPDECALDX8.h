// The DX8 renderer plugin's heightmap decals: HEIGHTMAPDECALDX8MASTER names a
// heightmap decal, drawn by stretching a heightmap onto a mesh, and
// HEIGHTMAPDECALDX8RESOURCE is one placed copy.
#ifndef TONGAS2_HEIGHTMAPDECALDX8_H
#define TONGAS2_HEIGHTMAPDECALDX8_H

#include <d3dx8.h>

#include "Tongas2/HEIGHTMAPDX8.h"
#include "Tongas2/RESOURCE.h"
#include "Tongas2/RESOURCES.h"

class HEIGHTMAPDECALDX8MASTER : public RESOURCEMASTER
{
public:
    HEIGHTMAPDECALDX8MASTER(RESOURCEMANAGER *manager, const char *name);
    virtual ~HEIGHTMAPDECALDX8MASTER();

    static void RegisterCommands();

    virtual int GetClassId();
    virtual int GetParentClassId();
    virtual const char *GetClassName();
    virtual SRESULT Execute(CMDENTRY *command, TOKENS **args);
    virtual RESOURCE *CreateCopy(const RESHANDLE &handle);
    virtual void UpdateCopies();
    virtual void Prepare();

    virtual void SetShader(const CSTRING &name);
    virtual void SetSize(const V2 &size);
    virtual void SetResolution(const int &value) { m_70 = value; }
    virtual void SetUserFolder(const bool &user) { m_userFolder = user; }
    virtual void SetCorrectSizeInWidescreen(const bool &correct) { m_correctSizeInWidescreen = correct; }

    // Tells each copy to free its mesh and vertices.
    void FreeCopiesMeshes();

    static int s_classId;
    static CMDENTRY s_commands[];
    static PROPENTRY s_properties[];

    bool m_60;
    CSTRING m_shader;
    V2 m_size;
    int m_70;
    bool m_userFolder;
    bool m_correctSizeInWidescreen;
};

class HEIGHTMAPDECALDX8RESOURCE : public OBJECT
{
public:
    HEIGHTMAPDECALDX8RESOURCE(HEIGHTMAPDECALDX8MASTER *master, const RESHANDLE &handle);
    virtual ~HEIGHTMAPDECALDX8RESOURCE();

    static void RegisterCommands();

    virtual int GetClassId();
    virtual int GetParentClassId();
    virtual const char *GetClassName();
    virtual void Virtual30();
    virtual void Start();
    virtual void Stop();
    virtual void Update();
    virtual bool IsLoaded() { return true; }
    virtual V3 GetScaleFactor() { return m_scaleFactor; }
    virtual void SetLayer(int layer);
    virtual void SetFrame(int frame);

    // Deletes the mesh and the vertex and normal arrays.
    void FreeMeshes();
    // The heightmap the decal is drawn from, or 0.
    HEIGHTMAPDX8 *FindHeightmap();
    // Keeps the mesh lit only while the box is in the camera's frustum.
    void UpdateVisibility();

    static int s_classId;
    static CMDENTRY s_commands[];

    int m_400;
    int m_404;
    int m_408;
    int m_40c;
    int m_410;
    BOXCORNERS m_box;
    RESHANDLE m_heightmap;
    bool m_480;
    MESHDX8 *m_mesh;
    int m_488;
    bool m_48c;
    V3 *m_vertices;
    V3 *m_normals;
    int m_498;
    bool m_49c;
    D3DXMATRIX m_matrix;
    bool m_4e0;
    int m_4e4;
    RESHANDLE m_hmap;
    RESHANDLE m_actor;
    int m_500;
};

class HEIGHTMAPDECALDX8MANAGER : public RESOURCEMANAGER
{
public:
    HEIGHTMAPDECALDX8MANAGER()
    {
        SafeSprintf(m_fileName, "HEIGHTMAPDECALDX8.txt");
        m_8 = false;
        m_18 = true;
    }

    virtual RESOURCEMASTER *CreateMaster(const char *name);
    virtual const char *GetTypeCode() { return "HMDC"; }
};

RESOURCEMANAGER *CreateHEIGHTMAPDECALDX8();

#endif
