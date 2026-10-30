// The renderer plugin's heightmaps: terrain made from a greyscale height
// image, with a color image, a user color image and up to eight alpha maps
// that blend the shaders of each tile's layers. HEIGHTMAPDX8MASTER loads the
// images and the scripts' properties, HEIGHTMAPDX8 builds the terrain's
// vertices and normals from them, cuts each tile's grid down where it is flat
// enough, puts the tiles' layers into meshes, and culls the tiles with a
// quadtree. Placeholder names apart from the class names the command tables
// and the log give.
#ifndef TONGAS2_HEIGHTMAPDX8_H
#define TONGAS2_HEIGHTMAPDX8_H

#include <d3dx8.h>

#include "Tongas2/RESOURCE.h"
#include "Tongas2/RENDERSHADER.h"
#include "Tongas2/RESOURCES.h"
#include "Tongas2/STRING.h"

#define HEIGHTMAP_MAX_LAYERS 8
#define HEIGHTMAP_MAX_SHADERS 64
#define HEIGHTMAP_MAX_MESHES 64

// Defined in the plugin: a mesh made of submeshes, each a range of its
// vertex and index buffers drawn with a shader.
class MESHDX8
{
public:
    MESHDX8();
    virtual ~MESHDX8();
    virtual void Virtual01();
    virtual void Virtual02();
    virtual void Virtual03();
    virtual void Virtual04();
    virtual void Virtual05();
    virtual void Virtual06();
    virtual void Virtual07();
    virtual void SetVisibility(int visibility);

    // Makes the buffers with room for the vertices and indices.
    void Create(int format, int vertices, int indices, int dynamic);
    void SetLightMask(int mask);
    void SetLit(bool lit);
    void SetMatrix(const D3DXMATRIX *matrix);
    void *LockVertices(int start, int count);
    void UnlockVertices();
    void *LockIndices(int start, int count);
    void UnlockIndices();
    // Shows a submesh, or hides it.
    void ShowSubmesh(int submesh, bool show);
    void RemoveSubmesh(int submesh);
    // A new submesh drawn with the shader; its index.
    int AddSubmesh(const RESHANDLE &shader, int firstVertex, int numVertices, int firstIndex, int numFaces, int layer, int flags);
    void SetSubmeshShader(int submesh, const RESHANDLE &shader);

    char m_04[0xc];
    int m_passes;
};

// Each tile's grid, cut down: its vertices, as indices into the full grid,
// and its triangles, as indices into those.
struct SUBMESHINDEX
{
    SUBMESHINDEX();
    ~SUBMESHINDEX();
    // Shrinks the arrays to what is used.
    void Shrink();
    // Copies the triangles to out, each index plus base.
    void CopyFaces(unsigned short *out, unsigned short base);
    // The index of a grid point, added if it isn't there yet.
    unsigned short AddVertex(unsigned short point);

    unsigned short m_numVertices;
    unsigned short m_numFaces;
    unsigned short m_maxVertices;
    unsigned short m_maxFaces;
    unsigned short *m_faces;
    unsigned short *m_vertices;
};

// One layer of a tile: a submesh of one of the meshes.
struct SUBMESH
{
    bool m_hasShader;
    bool m_uvsDone;
    char m_shader;
    char m_orientation;
    int m_mesh;
    int m_submesh;
    int m_firstVertex;
    int m_firstIndex;
};

// The quadtree the tiles are culled with.
struct QUADNODE
{
    ~QUADNODE();
    // The box around the children's boxes.
    void UpdateBox();

    QUADNODE *m_children[4];
    BOXCORNERS m_box;
    // The tile of a leaf, or -1.
    int m_tile;
    bool m_visible;
};

// Defined elsewhere: an object placed with a stack temporary.
struct HEIGHTMAPTEMP
{
    HEIGHTMAPTEMP();
};

class HEIGHTMAPDX8MASTER : public RESOURCEMASTER
{
public:
    HEIGHTMAPDX8MASTER(RESOURCEMANAGER *manager, const char *name);
    virtual ~HEIGHTMAPDX8MASTER();

    static void RegisterCommands();

    virtual int GetClassId();
    virtual int GetParentClassId();
    virtual const char *GetClassName();
    virtual SRESULT Execute(CMDENTRY *command, TOKENS **args);
    virtual RESOURCE *CreateCopy(const RESHANDLE &handle);
    virtual void UpdateCopies();
    virtual void Prepare();

    virtual void SetFileName(const CSTRING &name);
    virtual void SetFileNameColors(const CSTRING &name);
    virtual void SetFileNameUserColors(const CSTRING &name);
    virtual void SetPointsPerTile(const int &points);
    virtual void SetAspectRatioModifier(const float &value);
    virtual void SetMaxHeight(const float &value);
    virtual void SetZeroValue(const int &value);
    virtual void SetAlphaReductionTolerance(const float &value);
    virtual void SetPolyReductionTolerance(const float &value);
    virtual void SetRenderPassesFlags(const int &flags) { m_renderPassesFlags = flags; }
    virtual void SetVertexColorLighting(const bool &value);
    virtual void SetStartEditingMode(const bool &value) {}
    virtual void SetStopEditingMode(const bool &value) {}
    virtual void SetLayerAlphaMap(const int &index, const CSTRING &name);
    virtual void SetCheckUnused(const bool &value) {}
    virtual void SetRenderShader(const int &index, const CSTRING &name);
    virtual void SetShowCorners(const bool &value) {}
    virtual void SetLayerVisible(const int &index, const bool &value) { m_layerVisible[index] = value; }
    virtual void SetDeleteShader(const bool &value) { m_deleteShader = value; }
    virtual void SetShaderActiveForEditing(const int &index, const bool &value) { m_shaderActiveForEditing[index] = value; }
    virtual void SetTilesInfo(const STRING &info);
    virtual void SetEditingLayer(const int &layer) { m_editingLayer = layer; }
    virtual void SetEditingOrientation(const int &orientation) {}
    virtual void SetEditingOrientationRandom(const bool &value) { m_editingOrientationRandom = value; }

    // Frees the images.
    void FreeImages();
    // Whether the vertices carry their own normals for hardware lighting.
    bool UseHardwareLighting();
    // Works out the lighting mode, and tells the shaders whether they are lit.
    void UpdateLighting();
    // Loads the images.
    void LoadImages();
    // Tells the copies to make their meshes again.
    void FreeCopiesMeshes();
    // A layer's alpha at a pixel, clamped to the image.
    float GetAlpha(int x, int y, int layer)
    {
        if (x < 0)
            x = 0;
        if (x > m_imageWidth - 1)
            x = m_imageWidth - 1;
        if (y < 0)
            y = 0;
        if (y > m_imageHeight - 1)
            y = m_imageHeight - 1;
        return (float)m_alphas[(m_imageWidth * y + x) * HEIGHTMAP_MAX_LAYERS + layer] * (1.0f / 255.0f);
    }

    static int s_classId;
    static CMDENTRY s_commands[];
    static PROPENTRY s_properties[];

    // The height image's height over its width.
    float m_aspect;
    // The height image, one byte a pixel.
    unsigned char *m_heightImage;
    // A height a grid point: the pixels around it averaged.
    unsigned char *m_heights;
    V3I *m_colors;
    // Eight alphas a pixel, one for each layer.
    unsigned char *m_alphas;
    // For each tile, whether each layer is wholly transparent.
    unsigned char *m_emptyLayers;
    V3I *m_userColors;
    int m_imageHeight;
    int m_imageWidth;
    int m_gridHeight;
    int m_gridWidth;
    int m_tilesX;
    int m_tilesY;
    bool m_hardwareLighting;
    bool m_shaderHasAlpha[HEIGHTMAP_MAX_SHADERS];

    CSTRING m_fileName;
    CSTRING m_fileNameColors;
    CSTRING m_fileNameUserColors;
    int m_pointsPerTile;
    float m_aspectRatioModifier;
    float m_maxHeight;
    int m_zeroValue;
    float m_alphaReductionTolerance;
    float m_polyReductionTolerance;
    int m_renderPassesFlags;
    bool m_vertexColorLighting;
    bool m_startEditingMode;
    bool m_stopEditingMode;
    CSTRING m_layerAlphaMap[HEIGHTMAP_MAX_LAYERS];
    bool m_checkUnused;
    CSTRING m_renderShader[HEIGHTMAP_MAX_SHADERS];
    bool m_showCorners;
    bool m_layerVisible[HEIGHTMAP_MAX_LAYERS];
    bool m_deleteShader;
    bool m_shaderActiveForEditing[HEIGHTMAP_MAX_SHADERS];
    STRING m_tilesInfo;
    int m_editingLayer;
    int m_editingOrientation;
    bool m_editingOrientationRandom;
};

class HEIGHTMAPDX8 : public OBJECT
{
public:
    HEIGHTMAPDX8(RESOURCEMASTER *master, const RESHANDLE &handle);
    virtual ~HEIGHTMAPDX8();

    virtual void Start();
    virtual void Stop();
    virtual void Update();
    virtual bool IsLoaded() { return true; }
    virtual V3 GetScaleFactor() { return ZEROV3(); }
    virtual void SetLayer(int layer);

    // Frees the meshes and the vertices; they are made again when needed.
    void FreeMeshes();
    // Frees the tiles too.
    void FreeTiles();
    // Makes a tile's layer's submesh.
    void CreateSubmesh(int x, int y, int layer);
    // Writes the tiles' triangles to the index buffers.
    void UpdateIndices();
    // Works out the grid's vertices and the tiles' boxes.
    void UpdatePositions();
    // Shows or hides a tile's layers.
    void ShowTile(int tile, bool show);
    // The image pixel, and the tile, under a point.
    void PointToPixel(const V3 &point, int *x, int *y);
    void PointToTile(const V3 &point, int *x, int *y);
    // The height under a point, and the normal there if asked for.
    float GetHeight(const V3 &point, V3 *normal);
    // The quadtree over a rectangle of tiles.
    QUADNODE *BuildQuadtree(int x, int y, int width, int height);
    // Hides the tiles under the node.
    void HideNode(QUADNODE *node);
    // Shows the tiles under the node in the camera's frustum and hides the
    // others.
    void CullNode(QUADNODE *node);
    // Cuts down the grid of a tile's triangle where it is flat enough.
    void Reduce(SUBMESHINDEX *index, int tileX, int tileY, unsigned short a, unsigned short b, unsigned short c, unsigned short d);
    // Room for a submesh in one of the meshes.
    MESHDX8 *AllocMesh(int *mesh, int *firstVertex, int *firstIndex, int vertices, int faces, bool count);
    void UpdateBoxes();
    void UpdateNormals();
    PLANE GetPlane();
    PLANE GetDefaultPlane();
    PLANE GetTrianglePlane(int x, int y);
    PLANE GetFlatPlane(int x, int y);
    void PointToPixelWorld(const V3 &point, int *x, int *y);
    void PointToTileWorld(const V3 &point, int *x, int *y);
    V3 GetGroundPoint(const V3 &point);
    V3 GetNormal(const V3 &point);
    void BuildIndices();
    void UpdateMatrix();
    void UpdateVertices();
    ANG GetOrientation(const V3 &point, const V3 &direction);
    void ParseTilesInfo(int tilesX, int tilesY, const STRING &info);
    void Load();

    HEIGHTMAPDX8MASTER *Master() { return (HEIGHTMAPDX8MASTER *)m_master; }

    bool m_loaded;
    MESHDX8 *m_meshes[HEIGHTMAP_MAX_MESHES];
    int m_meshVertices[HEIGHTMAP_MAX_MESHES];
    int m_meshIndices[HEIGHTMAP_MAX_MESHES];
    int m_numMeshes;
    int m_nextVertex;
    int m_nextIndex;
    int m_710;
    int m_714;
    V3 *m_positions;
    bool m_normalsDone;
    V3 *m_normals;
    int m_matrixSerial;
    bool m_matrixDone;
    D3DXMATRIX m_matrix;
    bool m_verticesDone;
    bool m_colorsDone;
    bool m_uvsDone;
    bool m_indicesDone;
    bool m_boxesDone;
    BOXCORNERS *m_tileBoxes;
    QUADNODE *m_quadtree;
    QUADNODE **m_tileNodes;
    SUBMESHINDEX *m_indices;
    int m_maxTileVertices;
    int m_maxTileFaces;
    int m_numSubmeshes;
    int m_submeshesMade;
    SUBMESH *m_submeshes;
    int m_tilesX;
    int m_tilesY;
    int m_lightCount;
    bool m_7a4;
};

class HEIGHTMAPDX8MANAGER : public RESOURCEMANAGER
{
public:
    HEIGHTMAPDX8MANAGER();

    virtual RESOURCEMASTER *CreateMaster(const char *name);
    virtual const char *GetTypeCode();
    virtual void Update();
    virtual void Init();

    // The heightmap being edited.
    RESHANDLE m_editing;
};

extern RESOURCEMANAGER *g_pHEIGHTMAPDX8;

#endif
