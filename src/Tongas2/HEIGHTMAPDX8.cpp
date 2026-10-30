#define ANG_INLINE_DIRECTION
#define HEIGHTMAP_BOXES
#define FRUSTUM_MATHS
#define GEOMETRY_DEFINITIONS
#define TOKENS_INLINE_DTOR
#include "lib/freeimage/FreeImage.h"
#include "Tongas2/HEIGHTMAPDX8.h"
#include "Tongas2/CAMERA.h"
#include "Tongas2/FILELOADER.h"
#include "Tongas2/LIGHT.h"
#include "Tongas2/LOG.h"
#include "Tongas2/TOKENS.h"
#include "Tongas2/V2.h"

extern char g_basePath[];
// The loop counter the original shares between its loops.
extern int g_i;
// (0, 0, 1).
extern V3 g_forward;

static const char s_orientations[][30] = {
    "ROT_0",
    "ROT_90",
    "ROT_180",
    "ROT_270",
    "FLIP_ROT_0",
    "FLIP_ROT_90",
    "FLIP_ROT_180",
    "FLIP_ROT_270",
    "",
};

void BOXCORNERS::TransformAffine(const D3DXMATRIX &m)
{
    for (int i = 0; i < 8; i++) {
        V3 &c = corners[i];
        c = V3(m._21 * c.y + m._11 * c.x + m._31 * c.z + m._41,
               m._22 * c.y + m._12 * c.x + m._32 * c.z + m._42,
               m._23 * c.y + m._13 * c.x + m._33 * c.z + m._43);
    }
}

void BOXCORNERS::ExtendHeight(float height)
{
    if (corners[0].z < height)
        corners[0].z = height;
    if (corners[1].z < height)
        corners[1].z = height;
    if (corners[2].z > height)
        corners[2].z = height;
    if (corners[3].z > height)
        corners[3].z = height;
    if (corners[4].z < height)
        corners[4].z = height;
    if (corners[5].z < height)
        corners[5].z = height;
    if (corners[6].z > height)
        corners[6].z = height;
    if (corners[7].z > height)
        corners[7].z = height;
}

QUADNODE::~QUADNODE()
{
    for (int i = 0; i < 4; i++) {
        if (m_children[i]) {
            delete m_children[i];
            m_children[i] = 0;
        }
    }
}

SUBMESHINDEX::SUBMESHINDEX()
{
    m_numVertices = 0;
    m_numFaces = 0;
    m_maxVertices = 0;
    m_maxFaces = 0;
    m_faces = 0;
    m_vertices = 0;
}

SUBMESHINDEX::~SUBMESHINDEX()
{
    if (m_faces)
        delete[] m_faces;
    if (m_vertices)
        delete[] m_vertices;
}

void SUBMESHINDEX::Shrink()
{
    unsigned short numVertices = m_numVertices;
    unsigned short *vertices = m_vertices;
    unsigned short *faces = m_faces;
    unsigned short numFaces = m_numFaces;
    m_vertices = new unsigned short[numVertices];
    m_faces = new unsigned short[numFaces * 3];
    m_maxVertices = numVertices;
    m_maxFaces = numFaces;
    unsigned short i;
    for (i = 0; i < m_numVertices; i++)
        m_vertices[i] = vertices[i];
    for (i = 0; i < m_numFaces * 3; i++)
        m_faces[i] = faces[i];
    if (faces)
        delete[] faces;
    if (vertices)
        delete[] vertices;
}

void SUBMESHINDEX::CopyFaces(unsigned short *out, unsigned short base)
{
    unsigned short *in = m_faces;
    for (unsigned short i = 0; i < m_numFaces; i++) {
        out[0] = in[0] + base;
        out[1] = in[1] + base;
        out[2] = in[2] + base;
        in += 3;
        out += 3;
    }
}

unsigned short SUBMESHINDEX::AddVertex(unsigned short point)
{
    unsigned short count = m_numVertices;
    for (unsigned short i = 0; i < m_numVertices; i++)
        if (m_vertices[i] == point)
            return i;
    if (count >= m_maxVertices) {
        if (g_pLog)
            g_pLog->Error("MAX VERTS ACHIEVED IN AN OPTIMIZED TILE!!");
        return 0;
    }
    m_vertices[count] = point;
    m_numVertices++;
    return m_numVertices - 1;
}

void HEIGHTMAPDX8::SetLayer(int layer)
{
    m_19c = layer;
    if (m_submeshes) {
        for (int i = 0; i < m_numMeshes; i++)
            if (m_meshes[i])
                m_meshes[i]->SetVisibility(layer);
    }
}

void HEIGHTMAPDX8::CreateSubmesh(int x, int y, int layer)
{
    int index = (m_tilesX * y + x) * HEIGHTMAP_MAX_LAYERS + layer;
    SUBMESH *submesh = &m_submeshes[index];
    RESOURCE *shader = g_pRENDERSHADER->Find(Master()->m_renderShader[m_submeshes[index].m_shader].c_str());
    if (shader) {
        g_pRENDERSHADER->Get(shader->m_handle);
        MESHDX8 *mesh = m_meshes[submesh->m_mesh];
        if (!mesh) {
            if (g_pLog)
                g_pLog->Error("INVALID MESH FOR SUBMESH!!");
            return;
        }
        SUBMESHINDEX *tile = &m_indices[m_tilesX * y + x];
        submesh->m_submesh = mesh->AddSubmesh(shader->m_handle, submesh->m_firstVertex, tile->m_numVertices, submesh->m_firstIndex, tile->m_numFaces, layer, 0);
        submesh->m_hasShader = true;
        mesh->ShowSubmesh(submesh->m_submesh, m_tileNodes[m_tilesX * y + x]->m_visible);
        submesh->m_uvsDone = false;
        m_uvsDone = false;
        m_indicesDone = false;
    } else if (g_pLog)
        g_pLog->Error("Shader not found! --> SUBMESH NOT CREATED for tile %d %d %d", x, y, layer);
}

void HEIGHTMAPDX8::UpdateIndices()
{
    if (m_indicesDone)
        return;
    unsigned short *indices[HEIGHTMAP_MAX_MESHES];
    int i;
    for (i = 0; i < m_numMeshes; i++) {
        indices[i] = (unsigned short *)m_meshes[i]->LockIndices(0, m_meshIndices[i]);
        if (!indices[i]) {
            if (g_pLog)
                g_pLog->Error("could not lock face buffer %d! %s", i, m_master->m_name.c_str());
            return;
        }
    }
    for (int tile = 0; tile < m_tilesY * m_tilesX; tile++) {
        for (int layer = 0; layer < HEIGHTMAP_MAX_LAYERS; layer++) {
            SUBMESH *submesh = &m_submeshes[tile * HEIGHTMAP_MAX_LAYERS + layer];
            if (m_submeshes[tile * HEIGHTMAP_MAX_LAYERS + layer].m_submesh >= 0)
                m_indices[tile].CopyFaces(indices[submesh->m_mesh] + submesh->m_firstIndex * 3, submesh->m_firstVertex);
        }
    }
    for (i = 0; i < m_numMeshes; i++)
        m_meshes[i]->UnlockIndices();
    m_indicesDone = true;
}

void HEIGHTMAPDX8::UpdatePositions()
{
    HEIGHTMAPDX8MASTER *master = Master();
    m_normalsDone = false;
    m_verticesDone = false;
    m_colorsDone = false;
    float tileSize = master->m_aspectRatioModifier * master->m_aspect;
    float maxHeight = master->m_maxHeight;
    int gridHeight = master->m_gridHeight;
    int points = master->m_pointsPerTile;
    float offset = tileSize * -0.5f;
    int gridWidth = master->m_gridWidth;
    int x, y, px, py;
    for (y = 0, py = 0; y < m_tilesY; y++, py += points) {
        for (x = 0, px = 0; x < m_tilesX; x++, px += points) {
            V3 min = V3(1.0f, 0.0f, 0.0f) * (float)px / (float)(gridWidth - 1) + V3(-0.5f, offset, 0.0f)
                + V3(0.0f, tileSize, 0.0f) * (float)py / (float)(gridHeight - 1);
            V3 max = V3(1.0f, 0.0f, 0.0f) * (float)(px + points) / (float)(gridWidth - 1) + V3(-0.5f, offset, 0.0f)
                + V3(0.0f, tileSize, 0.0f) * (float)(py + points) / (float)(gridHeight - 1);
            m_tileBoxes[m_tilesX * y + x].Set(min, max);
        }
    }
    for (y = 0; y < gridHeight; y++) {
        for (x = 0; x < gridWidth; x++) {
            V3 &p = m_positions[gridWidth * y + x];
            float h = ((float)master->m_heights[master->m_gridWidth * y + x] - (float)master->m_zeroValue) / (float)(255 - master->m_zeroValue);
            p = V3(0.0f, 0.0f, maxHeight) * h
                + (V3(1.0f, 0.0f, 0.0f) * (float)x / (float)(gridWidth - 1) + V3(-0.5f, offset, 0.0f)
                    + V3(0.0f, tileSize, 0.0f) * (float)y / (float)(gridHeight - 1));
            int tileY = y / points;
            int tileX = x / points;
            if (y % points == 0) {
                if (x % points == 0) {
                    if (tileX > 0) {
                        if (tileY > 0)
                            m_tileBoxes[(tileY - 1) * m_tilesX + tileX - 1].ExtendHeight(p.z);
                        if (tileY < m_tilesY)
                            m_tileBoxes[m_tilesX * tileY + tileX - 1].ExtendHeight(p.z);
                    }
                    if (tileX < m_tilesX) {
                        if (tileY > 0)
                            m_tileBoxes[(tileY - 1) * m_tilesX + tileX].ExtendHeight(p.z);
                        if (tileY < m_tilesY)
                            m_tileBoxes[m_tilesX * tileY + tileX].ExtendHeight(p.z);
                    }
                } else {
                    if (tileY > 0)
                        m_tileBoxes[(tileY - 1) * m_tilesX + tileX].ExtendHeight(p.z);
                    if (tileY < m_tilesY)
                        m_tileBoxes[m_tilesX * tileY + tileX].ExtendHeight(p.z);
                }
            } else if (x % points == 0) {
                if (tileX > 0)
                    m_tileBoxes[m_tilesX * tileY + tileX - 1].ExtendHeight(p.z);
                if (tileX < m_tilesX)
                    m_tileBoxes[m_tilesX * tileY + tileX].ExtendHeight(p.z);
            } else
                m_tileBoxes[m_tilesX * tileY + tileX].ExtendHeight(p.z);
        }
    }
    m_boxesDone = false;
}

void HEIGHTMAPDX8::ShowTile(int tile, bool show)
{
    HEIGHTMAPDX8MASTER *master = Master();
    for (int layer = HEIGHTMAP_MAX_LAYERS - 1; layer >= 0; layer--) {
        SUBMESH *submesh = &m_submeshes[tile * HEIGHTMAP_MAX_LAYERS + layer];
        if (submesh->m_submesh >= 0) {
            m_meshes[submesh->m_mesh]->ShowSubmesh(submesh->m_submesh, show);
            if (master->m_shaderHasAlpha[submesh->m_shader] && master->m_emptyLayers[tile * HEIGHTMAP_MAX_LAYERS + layer] == 1)
                show = false;
        }
    }
}

void HEIGHTMAPDX8::HideNode(QUADNODE *node)
{
    if (!node->m_visible)
        return;
    node->m_visible = false;
    if (node->m_tile != -1) {
        ShowTile(node->m_tile, false);
        return;
    }
    for (int i = 0; i < 4; i++)
        if (node->m_children[i])
            HideNode(node->m_children[i]);
}

void HEIGHTMAPDX8::PointToPixel(const V3 &point, int *x, int *y)
{
    HEIGHTMAPDX8MASTER *master = Master();
    *x = (int)floor((point.x + 0.5f) * (float)master->m_imageWidth);
    *y = (int)floor((point.y / (master->m_aspectRatioModifier * master->m_aspect) + 0.5f) * (float)master->m_imageHeight);
    if (*x < 0)
        *x = 0;
    if (*x > master->m_imageWidth - 1)
        *x = master->m_imageWidth - 1;
    if (*y < 0)
        *y = 0;
    if (*y > master->m_imageHeight - 1)
        *y = master->m_imageHeight - 1;
}

void HEIGHTMAPDX8::PointToTile(const V3 &point, int *x, int *y)
{
    HEIGHTMAPDX8MASTER *master = Master();
    *x = (int)floor((point.x + 0.5f) * (float)m_tilesX);
    *y = (int)floor((point.y / (master->m_aspectRatioModifier * master->m_aspect) + 0.5f) * (float)m_tilesY);
    if (*x < 0)
        *x = 0;
    if (*x > m_tilesX - 1)
        *x = m_tilesX - 1;
    if (*y < 0)
        *y = 0;
    if (*y > m_tilesY - 1)
        *y = m_tilesY - 1;
}

float HEIGHTMAPDX8::GetHeight(const V3 &point, V3 *normal)
{
    HEIGHTMAPDX8MASTER *master = Master();
    V3 a(0.0f, 0.0f, 0.0f);
    V3 b(0.0f, 0.0f, 0.0f);
    V3 c(0.0f, 0.0f, 0.0f);
    V3 local = WorldToLocal(point);
    float fx = (local.x + 0.5f) * (float)master->m_imageWidth;
    float fy = (local.y / (master->m_aspectRatioModifier * master->m_aspect) + 0.5f) * (float)master->m_imageHeight;
    float u = fx - (float)floor(fx);
    float v = fy - (float)floor(fy);
    int x = (int)fx;
    int y = (int)fy;
    if (x < 0 || x >= master->m_imageWidth || y < 0 || y >= master->m_imageHeight || !m_positions)
        return 0.0f;
    if (v + u < 1.0f) {
        b = m_positions[master->m_gridWidth * y + x];
        a = m_positions[master->m_gridWidth * y + 1 + x];
        c = m_positions[(y + 1) * master->m_gridWidth + x];
    } else {
        u = 1.0f - u;
        v = 1.0f - v;
        b = m_positions[x + 1 + (y + 1) * master->m_gridWidth];
        a = m_positions[(y + 1) * master->m_gridWidth + x];
        c = m_positions[master->m_gridWidth * y + 1 + x];
    }
    b = b * *(MATRIX *)&m_matrix;
    a = a * *(MATRIX *)&m_matrix;
    c = c * *(MATRIX *)&m_matrix;
    if (normal)
        *normal = TriangleNormal(b, a, c);
    return (a.z - b.z) * u + (c.z - b.z) * v + b.z;
}

void HEIGHTMAPDX8MASTER::RegisterCommands()
{
    g_classParents[s_classId] = RESOURCEMASTER::s_classId;
    g_classCommands[s_classId] = s_commands;
    HashCommands(s_commands);
    RegisterClassName(s_commands, "HEIGHTMAPDX8MASTER", "RESOURCEMASTER");
}

void HEIGHTMAPDX8MASTER::FreeImages()
{
    if (m_heights) {
        delete[] m_heights;
        m_heights = 0;
    }
    if (m_heightImage) {
        delete[] m_heightImage;
        m_heightImage = 0;
    }
    if (m_colors) {
        delete[] m_colors;
        m_colors = 0;
    }
    if (m_alphas) {
        delete[] m_alphas;
        m_alphas = 0;
    }
    if (m_emptyLayers) {
        delete[] m_emptyLayers;
        m_emptyLayers = 0;
    }
    if (m_userColors) {
        delete[] m_userColors;
        m_userColors = 0;
    }
}

bool HEIGHTMAPDX8MASTER::UseHardwareLighting()
{
    if (!m_vertexColorLighting && g_pResources->IsHighDetail())
        return false;
    return true;
}

void HEIGHTMAPDX8MASTER::UpdateLighting()
{
    m_hardwareLighting = UseHardwareLighting();
    for (int i = 0; i < HEIGHTMAP_MAX_SHADERS; i++) {
        RENDERSHADERMASTER *shader = (RENDERSHADERMASTER *)g_pRENDERSHADER->GetMaster(m_renderShader[i].c_str());
        if (shader)
            shader->m_lightingOff = !m_hardwareLighting;
    }
}

static RESOURCEMANAGER *CreateHEIGHTMAPDX8();

CREATEMANAGER g_pfnCreateHEIGHTMAPDX8 = CreateHEIGHTMAPDX8;

static char s_codes[] = "?0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz$%";

// What each character of s_codes stands for: its position less one.
static char s_codeValues[0x100];

CMDENTRY HEIGHTMAPDX8MASTER::s_commands[] = {
    { 0, 0, 0, 0, 0 },
};

PROPENTRY HEIGHTMAPDX8MASTER::s_properties[] = {
    { "FileName", 0, 7, 0, PROP_FIELD(HEIGHTMAPDX8MASTER, m_fileName), false, 1, PROP_SETTER(HEIGHTMAPDX8MASTER, SetFileName) },
    { "FileNameColors", 0, 7, 0, PROP_FIELD(HEIGHTMAPDX8MASTER, m_fileNameColors), false, 1, PROP_SETTER(HEIGHTMAPDX8MASTER, SetFileNameColors) },
    { "FileNameUserColors", 0, 7, 0, PROP_FIELD(HEIGHTMAPDX8MASTER, m_fileNameUserColors), false, 1, PROP_SETTER(HEIGHTMAPDX8MASTER, SetFileNameUserColors) },
    { "PointsPerTile", 0, 0, 0, PROP_FIELD(HEIGHTMAPDX8MASTER, m_pointsPerTile), false, 1, PROP_SETTER(HEIGHTMAPDX8MASTER, SetPointsPerTile) },
    { "AspectRatioModifier", 0, 3, 0, PROP_FIELD(HEIGHTMAPDX8MASTER, m_aspectRatioModifier), false, 1, PROP_SETTER(HEIGHTMAPDX8MASTER, SetAspectRatioModifier) },
    { "MaxHeight", 0, 3, 0, PROP_FIELD(HEIGHTMAPDX8MASTER, m_maxHeight), false, 1, PROP_SETTER(HEIGHTMAPDX8MASTER, SetMaxHeight) },
    { "ZeroValue", 0, 0, 0, PROP_FIELD(HEIGHTMAPDX8MASTER, m_zeroValue), false, 1, PROP_SETTER(HEIGHTMAPDX8MASTER, SetZeroValue) },
    { "AlphaReductionTolerance", 0, 3, 0, PROP_FIELD(HEIGHTMAPDX8MASTER, m_alphaReductionTolerance), false, 1, PROP_SETTER(HEIGHTMAPDX8MASTER, SetAlphaReductionTolerance) },
    { "PolyReductionTolerance", 0, 3, 0, PROP_FIELD(HEIGHTMAPDX8MASTER, m_polyReductionTolerance), false, 1, PROP_SETTER(HEIGHTMAPDX8MASTER, SetPolyReductionTolerance) },
    { "RenderPassesFlags", 0, 0, 0, PROP_FIELD(HEIGHTMAPDX8MASTER, m_renderPassesFlags), false, 1, PROP_SETTER(HEIGHTMAPDX8MASTER, SetRenderPassesFlags) },
    { "VertexColorLighting", 0, 2, 0, PROP_FIELD(HEIGHTMAPDX8MASTER, m_vertexColorLighting), false, 1, PROP_SETTER(HEIGHTMAPDX8MASTER, SetVertexColorLighting) },
    { "StartEditingMode", 0, 2, 0, PROP_FIELD(HEIGHTMAPDX8MASTER, m_startEditingMode), false, 1, PROP_SETTER(HEIGHTMAPDX8MASTER, SetStartEditingMode) },
    { "StopEditingMode", 0, 2, 0, PROP_FIELD(HEIGHTMAPDX8MASTER, m_stopEditingMode), false, 1, PROP_SETTER(HEIGHTMAPDX8MASTER, SetStopEditingMode) },
    { "LayerAlphaMap", 0, 7, 0, PROP_FIELD(HEIGHTMAPDX8MASTER, m_layerAlphaMap), false, HEIGHTMAP_MAX_LAYERS, PROP_SETTER(HEIGHTMAPDX8MASTER, SetLayerAlphaMap) },
    { "CheckUnused", 0, 2, 0, PROP_FIELD(HEIGHTMAPDX8MASTER, m_checkUnused), false, 1, PROP_SETTER(HEIGHTMAPDX8MASTER, SetCheckUnused) },
    { "RenderShader", 0, 7, 0, PROP_FIELD(HEIGHTMAPDX8MASTER, m_renderShader), false, HEIGHTMAP_MAX_SHADERS, PROP_SETTER(HEIGHTMAPDX8MASTER, SetRenderShader) },
    { "ShowCorners", 0, 2, 0, PROP_FIELD(HEIGHTMAPDX8MASTER, m_showCorners), false, 1, PROP_SETTER(HEIGHTMAPDX8MASTER, SetShowCorners) },
    { "LayerVisible", 0, 2, 0, PROP_FIELD(HEIGHTMAPDX8MASTER, m_layerVisible), false, HEIGHTMAP_MAX_LAYERS, PROP_SETTER(HEIGHTMAPDX8MASTER, SetLayerVisible) },
    { "DeleteShader", 0, 2, 0, PROP_FIELD(HEIGHTMAPDX8MASTER, m_deleteShader), false, 1, PROP_SETTER(HEIGHTMAPDX8MASTER, SetDeleteShader) },
    { "ShaderActiveForEditing", 0, 2, 0, PROP_FIELD(HEIGHTMAPDX8MASTER, m_shaderActiveForEditing), false, HEIGHTMAP_MAX_SHADERS, PROP_SETTER(HEIGHTMAPDX8MASTER, SetShaderActiveForEditing) },
    { "TilesInfo", 0, 8, 0, PROP_FIELD(HEIGHTMAPDX8MASTER, m_tilesInfo), false, 1, PROP_SETTER(HEIGHTMAPDX8MASTER, SetTilesInfo) },
    { "EditingLayer", 0, 0, 0, PROP_FIELD(HEIGHTMAPDX8MASTER, m_editingLayer), false, 1, PROP_SETTER(HEIGHTMAPDX8MASTER, SetEditingLayer) },
    { "EditingOrientation", 0, 1, s_orientations, PROP_FIELD(HEIGHTMAPDX8MASTER, m_editingOrientation), false, 1, PROP_SETTER(HEIGHTMAPDX8MASTER, SetEditingOrientation) },
    { "EditingOrientationRandom", 0, 2, 0, PROP_FIELD(HEIGHTMAPDX8MASTER, m_editingOrientationRandom), false, 1, PROP_SETTER(HEIGHTMAPDX8MASTER, SetEditingOrientationRandom) },
    { 0, 0, 15, 0, 0, false },
};

void HEIGHTMAPDX8MANAGER::Init()
{
    RESOURCEMANAGER::Init();
    for (int i = 0; i < 0x41; i++)
        s_codeValues[s_codes[i]] = (char)i - 1;
}

void HEIGHTMAPDX8MANAGER::Update()
{
    ((LIGHTMANAGER *)g_pLIGHT)->UpdateLights();
    RESOURCEMANAGER::Update();
}

void BOXCORNERS::Merge(const BOXCORNERS &other)
{
    V3 min, max;
    min.x = corners[0].x < other.corners[0].x ? corners[0].x : other.corners[0].x;
    min.y = corners[0].y < other.corners[0].y ? corners[0].y : other.corners[0].y;
    min.z = corners[0].z < other.corners[0].z ? corners[0].z : other.corners[0].z;
    max.x = corners[0].x > other.corners[0].x ? corners[0].x : other.corners[0].x;
    max.y = corners[0].y > other.corners[0].y ? corners[0].y : other.corners[0].y;
    max.z = corners[0].z > other.corners[0].z ? corners[0].z : other.corners[0].z;
    for (int i = 1; i < 8; i++) {
        if (corners[i].x <= min.x)
            min.x = corners[i].x;
        if (corners[i].y <= min.y)
            min.y = corners[i].y;
        if (corners[i].z <= min.z)
            min.z = corners[i].z;
        if (max.x <= corners[i].x)
            max.x = corners[i].x;
        if (max.y <= corners[i].y)
            max.y = corners[i].y;
        if (max.z <= corners[i].z)
            max.z = corners[i].z;
        if (other.corners[i].x <= min.x)
            min.x = other.corners[i].x;
        if (other.corners[i].y <= min.y)
            min.y = other.corners[i].y;
        if (other.corners[i].z <= min.z)
            min.z = other.corners[i].z;
        if (max.x <= other.corners[i].x)
            max.x = other.corners[i].x;
        if (max.y <= other.corners[i].y)
            max.y = other.corners[i].y;
        if (max.z <= other.corners[i].z)
            max.z = other.corners[i].z;
    }
    Set(min, max);
}

void QUADNODE::UpdateBox()
{
    bool first = false;
    for (int i = 0; i < 4; i++) {
        if (m_children[i]) {
            if (m_children[i]->m_tile == -1)
                m_children[i]->UpdateBox();
            if (!first) {
                first = true;
                m_box = m_children[i]->m_box;
            } else
                m_box.Merge(m_children[i]->m_box);
        }
    }
}

HEIGHTMAPDX8MANAGER::HEIGHTMAPDX8MANAGER()
{
    SafeSprintf(m_fileName, "HEIGHTMAPDX8.txt");
    m_8 = false;
    m_18 = true;
}

const char *HEIGHTMAPDX8MANAGER::GetTypeCode()
{
    return "HMAP";
}

static RESOURCEMANAGER *CreateHEIGHTMAPDX8()
{
    return new HEIGHTMAPDX8MANAGER;
}

void HEIGHTMAPDX8::FreeMeshes()
{
    int y, x, layer;
    for (y = 0; y < m_tilesY; y++) {
        for (x = 0; x < m_tilesX; x++) {
            for (layer = 0; layer < HEIGHTMAP_MAX_LAYERS; layer++) {
                SUBMESH *submesh = &m_submeshes[(m_tilesX * y + x) * HEIGHTMAP_MAX_LAYERS + layer];
                if (submesh->m_submesh >= 0) {
                    if (m_meshes[submesh->m_mesh])
                        m_meshes[submesh->m_mesh]->RemoveSubmesh(submesh->m_submesh);
                    submesh->m_submesh = -1;
                }
            }
        }
    }
    if (m_positions) {
        delete[] m_positions;
        m_positions = 0;
    }
    if (m_normals) {
        delete[] m_normals;
        m_normals = 0;
    }
    if (m_indices) {
        delete[] m_indices;
        m_indices = 0;
    }
    m_loaded = false;
    for (int i = 0; i < HEIGHTMAP_MAX_MESHES; i++) {
        if (m_meshes[i]) {
            delete m_meshes[i];
            m_meshes[i] = 0;
        }
    }
    m_numMeshes = 0;
    m_nextIndex = 0;
}

void HEIGHTMAPDX8::FreeTiles()
{
    FreeMeshes();
    if (m_submeshes) {
        delete[] m_submeshes;
        m_submeshes = 0;
    }
    if (m_tileNodes) {
        delete[] m_tileNodes;
        m_tileNodes = 0;
    }
    if (m_tileBoxes) {
        delete[] m_tileBoxes;
        m_tileBoxes = 0;
    }
    if (m_quadtree) {
        delete m_quadtree;
        m_quadtree = 0;
    }
    m_tilesX = -1;
    m_tilesY = -1;
}

QUADNODE *HEIGHTMAPDX8::BuildQuadtree(int x, int y, int width, int height)
{
    if (width == 1 && height == 1)
        return m_tileNodes[m_tilesX * y + x];
    QUADNODE *node = new QUADNODE;
    if (width == 1) {
        int half = height / 2;
        node->m_children[0] = BuildQuadtree(x, y, 1, half);
        node->m_children[1] = BuildQuadtree(x, y + half, 1, height - half);
        return node;
    }
    int halfWidth = width / 2;
    if (height == 1) {
        node->m_children[0] = BuildQuadtree(x, y, halfWidth, 1);
        node->m_children[1] = BuildQuadtree(x + halfWidth, y, width - halfWidth, 1);
        return node;
    }
    int halfHeight = height / 2;
    node->m_children[0] = BuildQuadtree(x, y, halfWidth, halfHeight);
    node->m_children[1] = BuildQuadtree(x + halfWidth, y, width - halfWidth, halfHeight);
    node->m_children[2] = BuildQuadtree(x, y + halfHeight, halfWidth, height - halfHeight);
    node->m_children[3] = BuildQuadtree(x + halfWidth, y + halfHeight, width - halfWidth, height - halfHeight);
    return node;
}

void HEIGHTMAPDX8::Reduce(SUBMESHINDEX *index, int tileX, int tileY, unsigned short a, unsigned short b, unsigned short c, unsigned short d)
{
    HEIGHTMAPDX8MASTER *master = Master();
    int points = master->m_pointsPerTile;
    int side = points + 1;
    int ay = index->m_vertices[a] / side;
    int ax = index->m_vertices[a] % side;
    int dy = index->m_vertices[d] / side;
    int dx = index->m_vertices[d] % side;
    int baseX = points * tileX;
    int baseY = points * tileY;
    int midX = (dx + ax) / 2;
    int midY = (ay + dy) / 2;
    int gridWidth = master->m_gridWidth;
    int height = dy - ay;
    int width = dx - ax;
    if (width > 1 && height > 1) {
        int row = (ay + baseY) * gridWidth + baseX;
        bool flat = true;
        float h00 = m_positions[row + ax].z;
        float h01 = m_positions[row + dx].z;
        int row2 = (baseY + dy) * gridWidth + baseX;
        float h10 = m_positions[ax + row2].z;
        float h11 = m_positions[row2 + dx].z;
        int i, j;
        for (i = ay; i <= dy && flat; i++) {
            for (j = ax; j <= dx; j++) {
                if ((j != ax || (i != ay && i != dy)) && (j != dx || (i != ay && i != dy))) {
                    float z = m_positions[row + j].z;
                    float h;
                    if (i + j <= midY + midX)
                        h = (h01 - h00) * (float)(j - ax) / (float)width + (h10 - h00) * (float)(i - ay) / (float)height + h00;
                    else
                        h = (h10 - h11) * (float)(dx - j) / (float)width + (h01 - h11) * (float)(dy - i) / (float)height + h11;
                    if (fabs(h - z) > master->m_polyReductionTolerance) {
                        flat = false;
                        break;
                    }
                }
            }
            row += gridWidth;
        }
        if (flat) {
            for (int layer = 0; layer < HEIGHTMAP_MAX_LAYERS && flat; layer++) {
                float a00 = master->GetAlpha(baseX + ax, baseY + ay, layer);
                float a01 = master->GetAlpha(baseX + dx, baseY + ay, layer);
                float a10 = master->GetAlpha(baseX + ax, baseY + dy, layer);
                float a11 = master->GetAlpha(baseX + dx, baseY + dy, layer);
                for (i = ay; i <= dy && flat; i++) {
                    for (j = ax; j <= dx; j++) {
                        if ((j != ax || (i != ay && i != dy)) && (j != dx || (i != ay && i != dy))) {
                            float alpha;
                            if (i + j <= midY + midX)
                                alpha = (a01 - a00) * (float)(j - ax) / (float)width + (a10 - a00) * (float)(i - ay) / (float)height + a00;
                            else
                                alpha = (a10 - a11) * (float)(dx - j) / (float)width + (a01 - a11) * (float)(dy - i) / (float)height + a11;
                            if (fabs(alpha - master->GetAlpha(j + baseX, baseY + i, layer)) > master->m_alphaReductionTolerance) {
                                flat = false;
                                break;
                            }
                        }
                    }
                }
            }
        }
        if (!flat) {
            unsigned short top = index->AddVertex(side * ay + midX);
            unsigned short left = index->AddVertex(side * midY + ax);
            unsigned short center = index->AddVertex(side * midY + midX);
            unsigned short right = index->AddVertex(side * midY + dx);
            unsigned short bottom = index->AddVertex(side * dy + midX);
            Reduce(index, tileX, tileY, a, top, left, center);
            Reduce(index, tileX, tileY, top, b, center, right);
            Reduce(index, tileX, tileY, left, center, c, bottom);
            Reduce(index, tileX, tileY, center, right, bottom, d);
            return;
        }
        for (i = ay; i <= dy; i++) {
            for (j = ax; j <= dx; j++) {
                if ((j != ax || (i != ay && i != dy)) && (j != dx || (i != ay && i != dy))) {
                    float h;
                    if (i + j <= midY + midX)
                        h = (h01 - h00) * (float)(j - ax) / (float)width + (h10 - h00) * (float)(i - ay) / (float)height + h00;
                    else
                        h = (h10 - h11) * (float)(dx - j) / (float)width + (h01 - h11) * (float)(dy - i) / (float)height + h11;
                    m_positions[(baseY + i) * gridWidth + j + baseX].z = h;
                }
            }
        }
    }
    if (index->m_numFaces < index->m_maxFaces) {
        unsigned short f = index->m_numFaces * 3;
        index->m_faces[f] = a;
        index->m_faces[(unsigned short)(f + 1)] = b;
        index->m_faces[(unsigned short)(f + 2)] = c;
        index->m_numFaces++;
    } else if (g_pLog)
        g_pLog->Error("MAX FACES ACHIEVED IN AN OPTIMIZED TILE!!");
    if (index->m_numFaces < index->m_maxFaces) {
        unsigned short f = index->m_numFaces * 3;
        index->m_faces[f] = c;
        index->m_faces[(unsigned short)(f + 1)] = b;
        index->m_faces[(unsigned short)(f + 2)] = d;
        index->m_numFaces++;
    } else if (g_pLog)
        g_pLog->Error("MAX FACES ACHIEVED IN AN OPTIMIZED TILE!!");
}

MESHDX8 *HEIGHTMAPDX8::AllocMesh(int *mesh, int *firstVertex, int *firstIndex, int vertices, int faces, bool count)
{
    HEIGHTMAPDX8MASTER *master = Master();
    if (m_nextIndex + faces < 0xffdc && m_numMeshes > 0) {
        if (!count) {
            *mesh = m_numMeshes - 1;
            *firstIndex = m_nextIndex;
            *firstVertex = m_nextVertex;
        }
        m_nextIndex += faces;
        m_nextVertex += vertices;
        if (count) {
            m_meshIndices[m_numMeshes - 1] = m_nextIndex;
            m_meshVertices[m_numMeshes - 1] = m_nextVertex;
            return 0;
        }
        return m_meshes[*mesh];
    }
    if (!count) {
        *firstIndex = 0;
        *firstVertex = 0;
    }
    m_nextIndex = faces;
    m_nextVertex = vertices;
    if (count) {
        m_meshIndices[m_numMeshes] = faces;
        m_meshVertices[m_numMeshes] = m_nextVertex;
        m_numMeshes++;
        return 0;
    }
    if (m_numMeshes == HEIGHTMAP_MAX_MESHES) {
        if (g_pLog)
            g_pLog->Error("max meshes for heightmap reached!!!!");
        *mesh = 0;
        return m_meshes[0];
    }
    MESHDX8 *newMesh = new MESHDX8;
    newMesh->Create(master->UseHardwareLighting() ? 1 : 3, m_meshVertices[m_numMeshes], m_meshIndices[m_numMeshes], 1);
    newMesh->SetVisibility(m_19c);
    newMesh->m_passes = master->m_renderPassesFlags;
    newMesh->SetLightMask(m_1ac);
    if (g_pLog)
        master->UseHardwareLighting();
    newMesh->SetLit(true);
    m_meshes[m_numMeshes] = newMesh;
    *mesh = m_numMeshes;
    m_numMeshes++;
    return newMesh;
}

void HEIGHTMAPDX8::UpdateBoxes()
{
    if (m_boxesDone)
        return;
    for (int i = 0; i < m_tilesX * m_tilesY; i++) {
        BOXCORNERS box = m_tileBoxes[i];
        box.TransformAffine(m_matrix);
        m_tileNodes[i]->m_box = box;
    }
    m_quadtree->UpdateBox();
    m_boxesDone = true;
}

void HEIGHTMAPDX8::UpdateNormals()
{
    if (m_normalsDone)
        return;
    m_verticesDone = false;
    m_colorsDone = false;
    HEIGHTMAPDX8MASTER *master = Master();
    int gridWidth = master->m_gridWidth;
    int gridHeight = master->m_gridHeight;
    V3 dx, dy, n;
    for (int y = 1; y < gridHeight - 1; y++) {
        for (int x = 1; x < gridWidth - 1; x++) {
            dx = m_positions[gridWidth * y + x + 1] - m_positions[gridWidth * y + x - 1];
            dy = m_positions[gridWidth * (y + 1) + x] - m_positions[gridWidth * (y - 1) + x];
            n = V3(dy.z * dx.y - dy.y * dx.z, dx.z * dy.x - dy.z * dx.x, dy.y * dx.x - dx.y * dy.x);
            float length = n.Length();
            if (length != 0.0f) {
                float inv = 1.0f / length;
                n.x *= inv;
                n.y *= inv;
                n.z = inv * n.z;
            } else
                n.x = n.y = n.z = 0.0f;
            m_normals[gridWidth * y + x] = n;
        }
    }
    m_normalsDone = true;
}

void HEIGHTMAPDX8::CullNode(QUADNODE *node)
{
    FRUSTUM *frustum = ((CAMERA *)g_pCamera)->m_lens->GetFrustum();
    bool visible = frustum->Classify(&node->m_box) != 2;
    if (node->m_tile != -1) {
        if (node->m_visible != visible) {
            node->m_visible = visible;
            ShowTile(node->m_tile, visible);
        }
    } else if (visible || node->m_visible) {
        node->m_visible = visible;
        int i;
        if (visible) {
            for (i = 0; i < 4; i++)
                if (node->m_children[i])
                    CullNode(node->m_children[i]);
        } else {
            for (i = 0; i < 4; i++)
                if (node->m_children[i])
                    HideNode(node->m_children[i]);
        }
    }
}

static inline V3 TransformPoint(const D3DXMATRIX &m, const float &x, const float &y, const float &z)
{
    return V3(m._11 * x + m._21 * y + m._31 * z + m._41,
              m._12 * x + m._22 * y + m._32 * z + m._42,
              m._13 * x + m._23 * y + m._33 * z + m._43);
}

PLANE HEIGHTMAPDX8::GetPlane()
{
    V3 a, b, c;
    a = TransformPoint(m_matrix, 0.0f, -1.0f, 0.0f);
    b = TransformPoint(m_matrix, 1.0f, 0.0f, 0.0f);
    c = TransformPoint(m_matrix, -1.0f, 0.0f, 0.0f);
    return PLANE(a, b, c);
}

PLANE HEIGHTMAPDX8::GetDefaultPlane()
{
    V3 a(0.0f, -1.0f, 0.0f);
    V3 b(1.0f, 0.0f, 0.0f);
    V3 c(-1.0f, 0.0f, 0.0f);
    return PLANE(a, b, c);
}

PLANE HEIGHTMAPDX8::GetTrianglePlane(int x, int y)
{
    HEIGHTMAPDX8MASTER *master = Master();
    if (!m_positions)
        return GetPlane();
    int gridWidth = master->m_gridWidth;
    V3 a = m_positions[gridWidth * y + x];
    V3 b = m_positions[gridWidth * y + 1 + x];
    V3 c = m_positions[(y + 1) * gridWidth + x];
    a = a * *(MATRIX *)&m_matrix;
    b = b * *(MATRIX *)&m_matrix;
    c = c * *(MATRIX *)&m_matrix;
    return PLANE(a, b, c);
}

PLANE HEIGHTMAPDX8::GetFlatPlane(int x, int y)
{
    HEIGHTMAPDX8MASTER *master = Master();
    if (!m_positions)
        return GetDefaultPlane();
    int gridWidth = master->m_gridWidth;
    V3 a, b, c;
    a = m_positions[gridWidth * y + x];
    b = m_positions[gridWidth * y + x + 1];
    c = m_positions[(y + 1) * gridWidth + x];
    return PLANE(a, b, c);
}

void HEIGHTMAPDX8::PointToPixelWorld(const V3 &point, int *x, int *y)
{
    PointToPixel(WorldToLocal(point), x, y);
}

void HEIGHTMAPDX8::PointToTileWorld(const V3 &point, int *x, int *y)
{
    PointToTile(WorldToLocal(point), x, y);
}

V3 HEIGHTMAPDX8::GetGroundPoint(const V3 &point)
{
    HEIGHTMAPDX8MASTER *master = Master();
    int x, y;
    PointToPixel(point, &x, &y);
    PLANE plane;
    if (x < 0 || y < 0 || x >= master->m_imageWidth || y >= master->m_imageHeight)
        plane = GetDefaultPlane();
    else
        plane = GetFlatPlane(x, y);
    V3 other(g_forward.x + point.x, g_forward.y + point.y, g_forward.z + point.z);
    return plane.Intersect(point, other, 0);
}

V3 HEIGHTMAPDX8::GetNormal(const V3 &point)
{
    HEIGHTMAPDX8MASTER *master = Master();
    int x, y;
    PointToPixel(point, &x, &y);
    PLANE plane;
    if (x < 0 || y < 0 || x >= master->m_imageWidth || y >= master->m_imageHeight)
        plane = GetDefaultPlane();
    else
        plane = GetFlatPlane(x, y);
    V3 normal(plane.a, plane.b, plane.c);
    normal.Normalize();
    return normal;
}

void HEIGHTMAPDX8MASTER::Prepare()
{
    UpdateLighting();
}

void HEIGHTMAPDX8MASTER::SetPointsPerTile(const int &points)
{
    m_pointsPerTile = points;
    FreeImages();
}

void HEIGHTMAPDX8MASTER::SetFileName(const CSTRING &name)
{
    m_fileName = name;
    FreeImages();
}

void HEIGHTMAPDX8MASTER::SetFileNameColors(const CSTRING &name)
{
    m_fileNameColors = name;
    FreeImages();
}

void HEIGHTMAPDX8MASTER::SetLayerAlphaMap(const int &index, const CSTRING &name)
{
    m_layerAlphaMap[index] = name;
    FreeImages();
}

void HEIGHTMAPDX8MASTER::SetFileNameUserColors(const CSTRING &name)
{
    m_fileNameUserColors = name;
    FreeImages();
}

HEIGHTMAPDX8::~HEIGHTMAPDX8()
{
    FreeMeshes();
    FreeTiles();
    for (int i = 0; i < HEIGHTMAP_MAX_MESHES; i++) {
        if (m_meshes[i]) {
            delete m_meshes[i];
            m_meshes[i] = 0;
        }
    }
}

void HEIGHTMAPDX8::Stop()
{
    FreeMeshes();
    SetVisibility(1);
    HEIGHTMAPDX8MANAGER *manager = (HEIGHTMAPDX8MANAGER *)g_pHEIGHTMAPDX8;
    if (manager->m_editing == m_handle)
        manager->m_editing.Invalidate();
}

void HEIGHTMAPDX8::BuildIndices()
{
    int points = Master()->m_pointsPerTile;
    int levels = 0;
    int side = points + 1;
    for (int n = side; n > 1; n >>= 1)
        levels++;
    for (; levels > 0; levels--) {
        if (m_indices)
            delete[] m_indices;
        m_indices = new SUBMESHINDEX[m_tilesX * m_tilesY];
        for (int y = 0; y < m_tilesY; y++) {
            for (int x = 0; x < m_tilesX; x++) {
                unsigned short maxVertices = (unsigned short)m_maxTileVertices;
                SUBMESHINDEX *index = &m_indices[m_tilesX * y + x];
                unsigned short maxFaces = (unsigned short)m_maxTileFaces;
                index->m_vertices = new unsigned short[maxVertices];
                index->m_faces = new unsigned short[maxFaces * 3];
                index->m_maxVertices = maxVertices;
                index->m_maxFaces = maxFaces;
                unsigned short first = index->AddVertex(0);
                unsigned short b = index->AddVertex(points);
                unsigned short c = index->AddVertex(points * side);
                unsigned short d = index->AddVertex(side * side - 1);
                Reduce(index, x, y, first, b, c, d);
                index->Shrink();
            }
        }
    }
}

void HEIGHTMAPDX8::UpdateMatrix()
{
    if (m_matrixDone)
        return;
    m_verticesDone = false;
    m_boxesDone = false;
    ((MATRIX *)&m_matrix)->Scaling(m_scale.x, m_scale.y, m_scale.z);
    *(MATRIX *)&m_matrix = *(MATRIX *)&m_matrix * GetAngles().GetMatrix();
    m_matrix._41 = m_position.x;
    m_matrix._42 = m_position.y;
    m_matrix._43 = m_position.z;
    m_matrixSerial++;
    m_matrixDone = true;
}

ANG HEIGHTMAPDX8::GetOrientation(const V3 &point, const V3 &direction)
{
    HEIGHTMAPDX8MASTER *master = Master();
    int x, y;
    PointToPixelWorld(point, &x, &y);
    PLANE plane;
    if (x >= 0 && y >= 0 && x < master->m_imageWidth && y < master->m_imageHeight)
        plane = GetTrianglePlane(x, y);
    else
        plane = GetPlane();
    V3 ahead = point + direction;
    V3 up = GetAngles().GetDirection() + ahead;
    V3 hit = plane.Intersect(ahead, up, 0);
    V3 forward = hit - point;
    forward.Normalize();
    V3 normal(plane.a, plane.b, plane.c);
    normal.Normalize();
    ANG result(normal, forward);
    return result;
}

static FIBITMAP *LoadImage(const char *name)
{
    if (!name)
        return 0;
    int length = strlen(name);
    if (!length)
        return 0;
    FIBITMAP *bitmap = 0;
    FIXEDSTRING<0x200> path;
    SafeSprintf(path, "%s%s%s", g_basePath, "data/art/", name);
    _strlwr(path);
    path.Replace('/', '\\');
    FILELOADER *loader = g_pfnCreateFileLoader();
    if (!loader->Open(path)) {
        if (g_pLog)
            g_pLog->Error("Could not open file %s", &length);
        delete loader;
        return 0;
    }
    FIMEMORY *memory = FreeImage_OpenMemory((BYTE *)loader->GetData(), loader->GetSize());
    FREE_IMAGE_FORMAT format = FreeImage_GetFileTypeFromMemory(memory, 0);
    if (format == FIF_ICO || format == FIF_BMP)
        bitmap = FreeImage_LoadFromMemory(format, memory, 2);
    else if (g_pLog)
        g_pLog->Error("Unsupported heightmap image format, must be JPG or PNG! %s", &length);
    FreeImage_CloseMemory(memory);
    loader->Close();
    delete loader;
    if (!bitmap)
        return 0;
    FIBITMAP *result = bitmap;
    if (FreeImage_GetBPP(bitmap) != 24 && FreeImage_GetBPP(bitmap) != 32) {
        result = FreeImage_ConvertTo24Bits(bitmap);
        FreeImage_Unload(bitmap);
    }
    return result;
}

inline void HEIGHTMAPDX8MASTER::FreeCopiesMeshes()
{
    RESOURCE *copy = m_used;
    while (copy) {
        RESOURCE *next = copy->m_next;
        ((HEIGHTMAPDX8 *)copy)->FreeMeshes();
        copy = next;
    }
}

void HEIGHTMAPDX8MASTER::LoadImages()
{
    if (m_heightImage)
        return;
    FIBITMAP *bitmap = LoadImage(m_fileName.c_str());
    if (!bitmap) {
        if (g_pLog)
            g_pLog->Error("Could not load heightmap image!");
        return;
    }
    m_imageWidth = FreeImage_GetWidth(bitmap);
    m_imageHeight = FreeImage_GetHeight(bitmap);
    m_tilesX = m_imageWidth / m_pointsPerTile;
    m_tilesY = m_imageHeight / m_pointsPerTile;
    m_emptyLayers = new unsigned char[m_tilesY * m_tilesX * HEIGHTMAP_MAX_LAYERS];
    memset(m_emptyLayers, 1, m_tilesY * m_tilesX * HEIGHTMAP_MAX_LAYERS);
    int height = m_tilesY * m_pointsPerTile;
    int width = m_tilesX * m_pointsPerTile;
    m_imageHeight = height;
    m_imageWidth = width;
    m_aspect = (float)height / (float)width;
    m_colors = new V3I[height * width];
    m_alphas = new unsigned char[m_imageWidth * m_imageHeight * HEIGHTMAP_MAX_LAYERS];
    m_heightImage = new unsigned char[m_imageWidth * m_imageHeight];
    int y, x;
    for (y = 0; y < m_imageHeight; y++) {
        for (x = 0; x < m_imageWidth; x++) {
            RGBQUAD color;
            FreeImage_GetPixelColor(bitmap, x, m_imageHeight - y - 1, &color);
            m_heightImage[m_imageWidth * y + x] = (color.rgbBlue + color.rgbRed + color.rgbGreen) / 3;
            V3I &c = m_colors[m_imageWidth * y + x];
            c.x = 0xff;
            c.y = 0xff;
            c.z = 0xff;
            for (int layer = 0; layer < HEIGHTMAP_MAX_LAYERS; layer++)
                m_alphas[(m_imageWidth * y + x) * HEIGHTMAP_MAX_LAYERS + layer] = 0xff;
        }
    }
    FreeImage_Unload(bitmap);
    bitmap = LoadImage(m_fileNameColors.c_str());
    if (bitmap) {
        for (y = 0; y < m_imageHeight; y++) {
            for (x = 0; x < m_imageWidth; x++) {
                RGBQUAD color;
                FreeImage_GetPixelColor(bitmap, x, m_imageHeight - y - 1, &color);
                m_colors[m_imageWidth * y + x].x = color.rgbRed;
                m_colors[m_imageWidth * y + x].y = color.rgbGreen;
                m_colors[m_imageWidth * y + x].z = color.rgbBlue;
            }
        }
        FreeImage_Unload(bitmap);
    }
    bitmap = LoadImage(m_fileNameUserColors.c_str());
    if (bitmap) {
        m_userColors = new V3I[m_imageWidth * m_imageHeight];
        for (y = 0; y < m_imageHeight; y++) {
            for (x = 0; x < m_imageWidth; x++) {
                RGBQUAD color;
                FreeImage_GetPixelColor(bitmap, x, m_imageHeight - y - 1, &color);
                m_userColors[m_imageWidth * y + x].x = color.rgbRed;
                m_userColors[m_imageWidth * y + x].y = color.rgbGreen;
                m_userColors[m_imageWidth * y + x].z = color.rgbBlue;
            }
        }
        FreeImage_Unload(bitmap);
    }
    for (int layer = 0; layer < HEIGHTMAP_MAX_LAYERS; layer++) {
        bitmap = LoadImage(m_layerAlphaMap[layer].c_str());
        if (!bitmap)
            continue;
        for (y = 0; y < m_imageHeight; y++) {
            for (x = 0; x < m_imageWidth; x++) {
                RGBQUAD color;
                FreeImage_GetPixelColor(bitmap, x, m_imageHeight - y - 1, &color);
                m_alphas[(m_imageWidth * y + x) * HEIGHTMAP_MAX_LAYERS + layer] = (color.rgbBlue + color.rgbRed + color.rgbGreen) / 3;
            }
        }
        for (int tileY = 0; tileY < m_tilesY; tileY++) {
            for (int tileX = 0; tileX < m_tilesX; tileX++) {
                int row = m_pointsPerTile * tileY;
                for (int i = 0; i < m_pointsPerTile; i++, row++) {
                    unsigned char *alpha = &m_alphas[(m_imageWidth * row + m_pointsPerTile * tileX) * HEIGHTMAP_MAX_LAYERS + layer];
                    for (int j = 0; j < m_pointsPerTile; j++, alpha += HEIGHTMAP_MAX_LAYERS) {
                        if (*alpha != 0xff) {
                            m_emptyLayers[(m_tilesX * tileY + tileX) * HEIGHTMAP_MAX_LAYERS + layer] = 0;
                            i = m_pointsPerTile;
                            break;
                        }
                    }
                }
            }
        }
        FreeImage_Unload(bitmap);
    }
    m_gridWidth = m_imageWidth + 1;
    m_gridHeight = m_imageHeight + 1;
    m_heights = new unsigned char[m_gridWidth * m_gridHeight];
    m_heights[0] = m_heightImage[0];
    m_heights[(m_gridHeight - 1) * m_gridWidth] = m_heightImage[(m_imageHeight - 1) * m_imageWidth];
    m_heights[m_gridWidth - 1] = m_heightImage[m_imageWidth - 1];
    m_heights[m_gridWidth * m_gridHeight - 1] = m_heightImage[m_imageWidth * m_imageHeight - 1];
    int i;
    for (i = 1; i < m_gridWidth - 1; i++) {
        m_heights[i] = (m_heightImage[i - 1] + m_heightImage[i]) >> 1;
        unsigned char *last = &m_heightImage[(m_imageHeight - 1) * m_imageWidth + i];
        m_heights[(m_gridHeight - 1) * m_gridWidth + i] = (last[-1] + last[0]) >> 1;
    }
    for (i = 1; i < m_gridHeight - 1; i++) {
        m_heights[m_gridWidth * i] = (m_heightImage[m_imageWidth * i] + m_heightImage[(i - 1) * m_imageWidth]) >> 1;
        m_heights[m_gridWidth * (i + 1) - 1] = (m_heightImage[m_imageWidth * i - 1] + m_heightImage[(i + 1) * m_imageWidth - 1]) >> 1;
        for (int j = 1; j < m_gridWidth - 1; j++) {
            unsigned char *p = &m_heightImage[m_imageWidth * i + j];
            unsigned char *q = &m_heightImage[(i - 1) * m_imageWidth + j];
            m_heights[m_gridWidth * i + j] = (p[-1] + q[-1] + q[0] + p[0]) >> 2;
        }
    }
    FreeCopiesMeshes();
}

void HEIGHTMAPDX8MASTER::SetRenderShader(const int &index, const CSTRING &name)
{
    m_renderShader[index] = name;
    RESOURCEMASTER *shader = g_pRENDERSHADER->GetMaster(name.c_str());
    if (!shader)
        m_shaderHasAlpha[index] = true;
    else
        m_shaderHasAlpha[index] = ((RENDERSHADERMASTER *)shader)->HasAlpha();
    FreeCopiesMeshes();
}

void HEIGHTMAPDX8MASTER::SetZeroValue(const int &value)
{
    m_zeroValue = value;
    FreeCopiesMeshes();
}

void HEIGHTMAPDX8MASTER::SetPolyReductionTolerance(const float &value)
{
    m_polyReductionTolerance = value;
    FreeCopiesMeshes();
}

void HEIGHTMAPDX8MASTER::SetAlphaReductionTolerance(const float &value)
{
    m_alphaReductionTolerance = value;
    FreeCopiesMeshes();
}

void HEIGHTMAPDX8MASTER::SetVertexColorLighting(const bool &value)
{
    m_vertexColorLighting = value;
    UpdateLighting();
    FreeCopiesMeshes();
}

void HEIGHTMAPDX8MASTER::SetAspectRatioModifier(const float &value)
{
    m_aspectRatioModifier = value;
    FreeCopiesMeshes();
}

void HEIGHTMAPDX8MASTER::SetMaxHeight(const float &value)
{
    m_maxHeight = value;
    FreeCopiesMeshes();
}

void HEIGHTMAPDX8MASTER::UpdateCopies()
{
    if (!m_heightImage && (m_used || m_free))
        LoadImages();
    if (UseHardwareLighting() != m_hardwareLighting) {
        UpdateLighting();
        FreeCopiesMeshes();
    }
    RESOURCEMASTER::UpdateCopies();
}

SRESULT HEIGHTMAPDX8MASTER::Execute(CMDENTRY *command, TOKENS **args)
{
    return (this->*(SRESULT (HEIGHTMAPDX8MASTER::*)(TOKENS **))command->func)(args);
}

HEIGHTMAPDX8::HEIGHTMAPDX8(RESOURCEMASTER *master, const RESHANDLE &handle)
    : OBJECT(master, handle)
{
    HEIGHTMAPTEMP();
    m_numSubmeshes = 0;
    m_submeshesMade = 0;
    m_maxTileVertices = 0;
    m_maxTileFaces = 0;
    m_submeshes = 0;
    m_tileNodes = 0;
    m_indices = 0;
    m_tileBoxes = 0;
    m_tilesX = -1;
    m_tilesY = -1;
    m_positions = 0;
    m_normals = 0;
    m_loaded = false;
    m_numMeshes = 0;
    m_nextIndex = 0;
    for (int i = 0; i < HEIGHTMAP_MAX_MESHES; i++)
        m_meshes[i] = 0;
    m_normalsDone = false;
    m_matrixDone = false;
    m_verticesDone = false;
    m_indicesDone = false;
    m_colorsDone = false;
    m_lightCount = -1;
    m_quadtree = 0;
    m_710 = 0;
    m_matrixSerial = 0;
    m_7a4 = false;
    ((HEIGHTMAPDX8MASTER *)master)->LoadImages();
}

HEIGHTMAPDX8MASTER::HEIGHTMAPDX8MASTER(RESOURCEMANAGER *manager, const char *name)
    : RESOURCEMASTER(manager, name)
{
    m_fileName = "";
    m_fileNameColors = "";
    m_fileNameUserColors = "";
    m_pointsPerTile = 8;
    m_aspectRatioModifier = 1.0f;
    m_maxHeight = 1.0f;
    m_zeroValue = 0x7f;
    m_alphaReductionTolerance = 0.05f;
    m_polyReductionTolerance = 0.001f;
    m_renderPassesFlags = 1;
    m_vertexColorLighting = true;
    m_startEditingMode = false;
    m_stopEditingMode = false;
    for (g_i = 0; g_i < HEIGHTMAP_MAX_LAYERS; g_i++)
        m_layerAlphaMap[g_i] = "";
    m_checkUnused = false;
    for (g_i = 0; g_i < HEIGHTMAP_MAX_SHADERS; g_i++)
        m_renderShader[g_i] = "";
    m_showCorners = true;
    for (g_i = 0; g_i < HEIGHTMAP_MAX_LAYERS; g_i++)
        m_layerVisible[g_i] = true;
    m_deleteShader = false;
    for (g_i = 0; g_i < HEIGHTMAP_MAX_SHADERS; g_i++)
        m_shaderActiveForEditing[g_i] = false;
    m_tilesInfo = "";
    m_editingLayer = 0;
    m_editingOrientation = 0;
    m_editingOrientationRandom = false;
    m_heightImage = 0;
    m_heights = 0;
    m_colors = 0;
    m_alphas = 0;
    m_emptyLayers = 0;
    m_userColors = 0;
    m_hardwareLighting = false;
}

HEIGHTMAPDX8MASTER::~HEIGHTMAPDX8MASTER()
{
    FreeImages();
}

int HEIGHTMAPDX8MASTER::GetClassId()
{
    return s_classId;
}

int HEIGHTMAPDX8MASTER::GetParentClassId()
{
    return g_classParents[s_classId];
}

const char *HEIGHTMAPDX8MASTER::GetClassName()
{
    return g_classNames[s_classId];
}

void HEIGHTMAPDX8MASTER::SetTilesInfo(const STRING &info)
{
    m_tilesInfo = info;
    FreeCopiesMeshes();
    RESOURCE *copy = m_used;
    while (copy) {
        RESOURCE *next = copy->m_next;
        ((HEIGHTMAPDX8 *)copy)->FreeTiles();
        copy = next;
    }
}

RESOURCE *HEIGHTMAPDX8MASTER::CreateCopy(const RESHANDLE &handle)
{
    return new HEIGHTMAPDX8(this, handle);
}

RESOURCEMASTER *HEIGHTMAPDX8MANAGER::CreateMaster(const char *name)
{
    return new HEIGHTMAPDX8MASTER(this, name);
}

void HEIGHTMAPDX8::ParseTilesInfo(int tilesX, int tilesY, const STRING &info)
{
    FreeTiles();
    m_tilesX = tilesX;
    m_tilesY = tilesY;
    m_tileBoxes = new BOXCORNERS[tilesX * tilesY];
    m_tileNodes = new QUADNODE *[m_tilesY * m_tilesX];
    int i;
    for (i = 0; i < m_tilesY * m_tilesX; i++) {
        m_tileNodes[i] = new QUADNODE;
        m_tileNodes[i]->m_tile = i;
    }
    m_quadtree = BuildQuadtree(0, 0, m_tilesX, m_tilesY);
    m_submeshes = new SUBMESH[m_tilesY * m_tilesX * HEIGHTMAP_MAX_LAYERS];
    memset(m_submeshes, 0, m_tilesY * m_tilesX * HEIGHTMAP_MAX_LAYERS * sizeof(SUBMESH));
    for (i = 0; i < m_tilesY * m_tilesX * HEIGHTMAP_MAX_LAYERS; i++) {
        m_submeshes[i].m_shader = i % HEIGHTMAP_MAX_LAYERS == 0 ? 0 : -1;
        m_submeshes[i].m_submesh = -1;
        m_submeshes[i].m_mesh = -1;
    }
    HEIGHTMAPDX8MASTER *master = Master();
    m_numSubmeshes = 0;
    TOKENS rows;
    TOKENS cols;
    TOKENS cell;
    rows.Split(info, ";\n\r");
    for (int y = 0; y < (rows.m_count < m_tilesY ? rows.m_count : m_tilesY); y++) {
        cols.Split(STRING(rows.GetToken(y)), ",");
        for (int x = 0; x < (cols.m_count < m_tilesX ? cols.m_count : m_tilesX); x++) {
            cell.Split(STRING(cols.GetToken(x)), " \t");
            int layers, count, last, layer;
            if (cell.m_count == 1) {
                const char *codes = cols.GetToken(x);
                layers = strlen(codes) / 2;
                if (layers == 0 || layers > HEIGHTMAP_MAX_LAYERS) {
                    if (g_pLog)
                        g_pLog->Error("invalid number of layers in heightmap info for tile %d %d (%d layers)", y, x, layers);
                    if (layers == 0)
                        layers = 1;
                    else if (layers > HEIGHTMAP_MAX_LAYERS)
                        layers = HEIGHTMAP_MAX_LAYERS;
                }
                count = 1;
                last = 0;
                for (layer = 0; layer < layers; layer++) {
                    SUBMESH *submesh = &m_submeshes[(m_tilesX * y + x) * HEIGHTMAP_MAX_LAYERS + layer];
                    int shader = s_codeValues[codes[layer * 2]];
                    submesh->m_orientation = s_codeValues[codes[layer * 2 + 1]];
                    if (layer == 0 && shader < 0)
                        count--;
                    if (shader < -1 || shader >= HEIGHTMAP_MAX_SHADERS) {
                        if (g_pLog)
                            g_pLog->Error("invalid shader idx tile %d %d %d: %d", y, x, layer, shader);
                        if (shader < -1)
                            shader = -1;
                        else if (shader >= HEIGHTMAP_MAX_SHADERS)
                            shader = HEIGHTMAP_MAX_SHADERS - 1;
                    }
                    if (shader >= 0) {
                        count++;
                        m_numSubmeshes++;
                        if (master->m_shaderHasAlpha[shader] && master->m_emptyLayers[(master->m_tilesX * y + x) * HEIGHTMAP_MAX_LAYERS + layer] == 1)
                            last = layer;
                    }
                    submesh->m_shader = shader;
                }
            } else {
                layers = cell.m_count / 2;
                if (layers < 1 || layers > HEIGHTMAP_MAX_LAYERS) {
                    if (g_pLog)
                        g_pLog->Error("invalid number of layers in heightmap info for tile %d %d (%d layers)", y, x, layers);
                    if (layers < 1)
                        layers = 1;
                    else if (layers > HEIGHTMAP_MAX_LAYERS)
                        layers = HEIGHTMAP_MAX_LAYERS;
                }
                count = 1;
                last = 0;
                for (layer = 0; layer < layers; layer++) {
                    SUBMESH *submesh = &m_submeshes[(m_tilesX * y + x) * HEIGHTMAP_MAX_LAYERS + layer];
                    int shader = atol(cell.GetToken(layer * 2));
                    submesh->m_orientation = (char)atol(cell.GetToken(layer * 2 + 1));
                    if (layer == 0 && shader < 0)
                        count--;
                    if (shader < -1 || shader >= HEIGHTMAP_MAX_SHADERS) {
                        if (g_pLog)
                            g_pLog->Error("invalid shader idx tile %d %d %d", y, x, layer);
                        if (shader < -1)
                            shader = -1;
                        else if (shader >= HEIGHTMAP_MAX_SHADERS)
                            shader = HEIGHTMAP_MAX_SHADERS - 1;
                    }
                    if (shader >= 0) {
                        count++;
                        m_numSubmeshes++;
                        if (master->m_shaderHasAlpha[shader] && master->m_emptyLayers[(master->m_tilesX * y + x) * HEIGHTMAP_MAX_LAYERS + layer] == 1)
                            last = layer;
                    }
                    submesh->m_shader = shader;
                }
            }
            if (count == 0) {
                if (g_pLog)
                    g_pLog->Error("there must be at least one layer per tile! shader 0 will be forced for tile %d %d", y, x);
                m_submeshes[(m_tilesX * y + x) * HEIGHTMAP_MAX_LAYERS].m_shader = 0;
                m_numSubmeshes++;
            }
            if (last > 0) {
                for (layer = 0; layer < last; layer++) {
                    SUBMESH *submesh = &m_submeshes[(m_tilesX * y + x) * HEIGHTMAP_MAX_LAYERS + layer];
                    if (submesh->m_shader >= 0) {
                        submesh->m_shader = -1;
                        m_numSubmeshes--;
                    }
                }
            }
        }
    }
}

void HEIGHTMAPDX8::Load()
{
    if (m_loaded)
        return;
    HEIGHTMAPDX8MASTER *master = Master();
    if (m_tilesX != master->m_tilesX || m_tilesY != master->m_tilesY || !m_submeshes)
        ParseTilesInfo(master->m_tilesX, master->m_tilesY, master->m_tilesInfo);
    m_positions = new ZEROV3[master->m_gridHeight * master->m_gridWidth];
    m_normals = new ZEROV3[master->m_gridHeight * master->m_gridWidth];
    UpdatePositions();
    m_maxTileVertices = (master->m_pointsPerTile + 1) * (master->m_pointsPerTile + 1);
    m_maxTileFaces = master->m_pointsPerTile * master->m_pointsPerTile * 2;
    BuildIndices();
    m_nextIndex = 0;
    m_nextVertex = 0;
    m_numMeshes = 0;
    int y, x, layer;
    for (y = 0; y < m_tilesY; y++) {
        for (x = 0; x < m_tilesX; x++) {
            SUBMESHINDEX *index = &m_indices[m_tilesX * y + x];
            for (layer = 0; layer < HEIGHTMAP_MAX_LAYERS; layer++) {
                if (m_submeshes[(m_tilesX * y + x) * HEIGHTMAP_MAX_LAYERS + layer].m_shader >= 0) {
                    int dummy;
                    AllocMesh(&dummy, &dummy, &dummy, index->m_numVertices, index->m_numFaces, true);
                }
            }
        }
    }
    m_submeshesMade = 0;
    m_nextIndex = 0;
    m_nextVertex = 0;
    m_numMeshes = 0;
    for (y = 0; y < m_tilesY; y++) {
        for (x = 0; x < m_tilesX; x++) {
            SUBMESHINDEX *index = &m_indices[m_tilesX * y + x];
            for (layer = 0; layer < HEIGHTMAP_MAX_LAYERS; layer++) {
                SUBMESH *submesh = &m_submeshes[(m_tilesX * y + x) * HEIGHTMAP_MAX_LAYERS + layer];
                if (submesh->m_shader < 0)
                    continue;
                if (m_submeshesMade >= m_numSubmeshes) {
                    if (g_pLog)
                        g_pLog->Error("DID NOT CREATE ENOUGH VERTEX BUFFERS!!!! ... was at %d %d %d", y, x, layer);
                    break;
                }
                if (!AllocMesh(&submesh->m_mesh, &submesh->m_firstVertex, &submesh->m_firstIndex, index->m_numVertices, index->m_numFaces, false)) {
                    if (g_pLog)
                        g_pLog->Error("error creating mesh!! %d %d %d", y, x, layer);
                    return;
                }
                CreateSubmesh(x, y, layer);
                m_submeshesMade++;
            }
        }
    }
    m_loaded = true;
}

void HEIGHTMAPDX8::Start()
{
    SetVisibility(1);
    Load();
    RESHANDLE &editing = ((HEIGHTMAPDX8MANAGER *)g_pHEIGHTMAPDX8)->m_editing;
    if (!editing.IsValid())
        editing = m_handle;
}

void HEIGHTMAPDX8::Update()
{
    OBJECT::Update();
    if (!m_active)
        return;
    Load();
    if (m_scale.x != m_d8.x || m_scale.y != m_d8.y || m_scale.z != m_d8.z) {
        m_d8 = m_scale;
        m_colorsDone = false;
        m_matrixDone = false;
        m_verticesDone = false;
    }
    if (GetAngles() != m_134) {
        m_134 = m_angles;
        m_matrixDone = false;
    }
    if (m_position.x != m_c0.x || m_position.y != m_c0.y || m_position.z != m_c0.z) {
        m_c0 = m_position;
        m_matrixDone = false;
    }
    if (m_layer != m_lastLayer) {
        m_lastLayer = m_layer;
        m_colorsDone = false;
        m_verticesDone = false;
    }
    if ((Master()->m_vertexColorLighting || !g_pResources->IsHighDetail()) && m_7a4) {
        int count = ((LIGHTMANAGER *)g_pLIGHT)->GetMaxCount(m_1ac);
        if (m_lightCount != count) {
            m_lightCount = count;
            m_colorsDone = false;
            m_verticesDone = false;
        }
    }
    UpdateIndices();
    UpdateNormals();
    UpdateMatrix();
    UpdateBoxes();
    CullNode(m_quadtree);
    UpdateVertices();
}

// The vertices: hardware lit ones with their normals, or lit here with
// their colors, and each layer's texture coordinates by its orientation.
struct LITVERTEX
{
    V3 position;
    V3 normal;
    DWORD color;
    V2 uv;
};

struct COLORVERTEX
{
    V3 position;
    DWORD color;
    V2 uv;
};

void HEIGHTMAPDX8::UpdateVertices()
{
    if (m_verticesDone || !m_meshes)
        return;
    HEIGHTMAPDX8MASTER *master = Master();
    int side = master->m_pointsPerTile + 1;
    int gridHeight = master->m_gridHeight;
    int gridWidth = master->m_gridWidth;
    char *vertices[HEIGHTMAP_MAX_MESHES];
    int i;
    if (!m_colorsDone || !m_uvsDone) {
        for (i = 0; i < m_numMeshes; i++) {
            vertices[i] = (char *)m_meshes[i]->LockVertices(0, m_meshVertices[i]);
            if (!vertices[i]) {
                if (g_pLog)
                    g_pLog->Error("could not lock vertex buffer %d! %s", i, master->m_name.c_str());
                return;
            }
        }
    }
    D3DXMATRIX matrix = m_matrix;
    for (i = 0; i < m_numMeshes; i++)
        m_meshes[i]->SetMatrix(&matrix);
    for (int y = 0; y < m_tilesY; y++) {
        for (int x = 0; x < m_tilesX; x++) {
            for (int layer = 0; layer < HEIGHTMAP_MAX_LAYERS; layer++) {
                SUBMESH *submesh = &m_submeshes[(m_tilesX * y + x) * HEIGHTMAP_MAX_LAYERS + layer];
                if (submesh->m_submesh < 0) {
                    if (submesh->m_shader < 0)
                        continue;
                    CreateSubmesh(x, y, layer);
                } else if (submesh->m_shader == -1) {
                    if (m_meshes[submesh->m_mesh])
                        m_meshes[submesh->m_mesh]->RemoveSubmesh(submesh->m_submesh);
                    submesh->m_submesh = -1;
                    continue;
                } else if (!submesh->m_hasShader) {
                    RESOURCE *shader = g_pRENDERSHADER->Find(master->m_renderShader[submesh->m_shader].c_str());
                    if (shader) {
                        g_pRENDERSHADER->Get(shader->m_handle);
                        m_meshes[submesh->m_mesh]->SetSubmeshShader(submesh->m_submesh, shader->m_handle);
                        submesh->m_hasShader = true;
                    } else {
                        if (g_pLog)
                            g_pLog->Error("Shader not found! --> SUBMESH NOT UPDATED for tile %d %d %d", x, y, layer);
                        continue;
                    }
                }
                int first = submesh->m_firstVertex;
                if (m_colorsDone && submesh->m_uvsDone)
                    continue;
                SUBMESHINDEX *index = &m_indices[m_tilesX * y + x];
                char *out;
                if (!master->UseHardwareLighting())
                    out = vertices[submesh->m_mesh] + first * sizeof(LITVERTEX);
                else
                    out = vertices[submesh->m_mesh] + first * sizeof(COLORVERTEX);
                int points = master->m_pointsPerTile;
                for (int v = 0; v < index->m_numVertices; v++) {
                    int point = index->m_vertices[v];
                    int px = points * x + point % side;
                    int py = point / side + points * y;
                    int cx = px < gridWidth - 2 ? px : gridWidth - 2;
                    int cy = py < gridHeight - 2 ? py : gridHeight - 2;
                    V3 normal = m_normals[py * gridWidth + px];
                    V3 *position = &m_positions[py * gridWidth + px];
                    int pixel = master->m_imageWidth * cy + cx;
                    V3I *color = &master->m_colors[pixel];
                    unsigned char alpha = (unsigned char)((unsigned int)master->m_alphas[pixel * HEIGHTMAP_MAX_LAYERS + layer] * m_layer / 0xff);
                    if (!master->UseHardwareLighting()) {
                        LITVERTEX *vertex = (LITVERTEX *)out;
                        normal.x *= m_scale.x;
                        normal.y *= m_scale.y;
                        normal.z *= m_scale.z;
                        vertex->color = (((alpha << 8) | (color->x & 0xff)) << 8 | (color->y & 0xff)) << 8 | (color->z & 0xff);
                        vertex->normal = normal;
                        vertex->position = *position;
                        out += sizeof(LITVERTEX);
                    } else {
                        COLORVERTEX *vertex = (COLORVERTEX *)out;
                        MATRIX m = m_angles.GetMatrix();
                        normal = V3(m._11 * normal.x + m._21 * normal.y + m._31 * normal.z + m._41,
                                    m._12 * normal.x + m._22 * normal.y + m._32 * normal.z + m._42,
                                    m._13 * normal.x + m._23 * normal.y + m._33 * normal.z + m._43);
                        LIGHTRGB lit = { 0, 0, 0 };
                        LIGHTRGB ambient = { 0xff, 0xff, 0xff };
                        ((LIGHTMANAGER *)g_pLIGHT)->LightVertex(ambient, *color, normal, m_1ac, lit);
                        vertex->color = (((alpha << 8) | (lit.r & 0xff)) << 8 | (lit.g & 0xff)) << 8 | (lit.b & 0xff);
                        vertex->position = *position;
                        out += sizeof(COLORVERTEX);
                    }
                }
                if (submesh->m_uvsDone)
                    continue;
                RESOURCE *shader = g_pRENDERSHADER->Find(master->m_renderShader[submesh->m_shader].c_str());
                if (!shader) {
                    if (g_pLog)
                        g_pLog->Error("Shader not found: '%s' (%d)! --> SUBMESH ORIENTATION NOT UPDATED for tile %d %d %d", master->m_renderShader[submesh->m_shader].c_str(), submesh->m_shader, x, y, layer);
                    continue;
                }
                RENDERSHADER *render = g_pRENDERSHADER->Get(shader->m_handle);
                float w = (float)render->GetWidth();
                float h = (float)render->GetHeight();
                float u0 = 0.5f * (1.0f / w);
                float v0 = 1.0f / h * 0.5f;
                float su = (float)master->m_pointsPerTile / (1.0f - 1.0f / w);
                float sv = (float)master->m_pointsPerTile / (1.0f - 1.0f / h);
                float n = (float)master->m_pointsPerTile;
                switch (submesh->m_orientation) {
                case 0:
                    if (!master->UseHardwareLighting()) {
                        LITVERTEX *vertex = (LITVERTEX *)(vertices[submesh->m_mesh] + first * sizeof(LITVERTEX));
                        for (int k = 0; k < index->m_numVertices; k++) {
                            int point = index->m_vertices[k];
                            vertex->uv = V2((float)(point % side) / su + u0, (float)(point / side) / sv + v0);
                            vertex++;
                            submesh->m_uvsDone = true;
                        }
                    } else {
                        COLORVERTEX *vertex = (COLORVERTEX *)(vertices[submesh->m_mesh] + first * sizeof(COLORVERTEX));
                        for (int k = 0; k < index->m_numVertices; k++) {
                            int point = index->m_vertices[k];
                            vertex->uv = V2((float)(point % side) / su + u0, (float)(point / side) / sv + v0);
                            vertex++;
                            submesh->m_uvsDone = true;
                        }
                    }
                    break;
                case 1:
                    if (!master->UseHardwareLighting()) {
                        LITVERTEX *vertex = (LITVERTEX *)(vertices[submesh->m_mesh] + first * sizeof(LITVERTEX));
                        for (int k = 0; k < index->m_numVertices; k++) {
                            int point = index->m_vertices[k];
                            vertex->uv = V2((n - (float)(point / side)) / sv + v0, (float)(point % side) / su + u0);
                            vertex++;
                            submesh->m_uvsDone = true;
                        }
                    } else {
                        COLORVERTEX *vertex = (COLORVERTEX *)(vertices[submesh->m_mesh] + first * sizeof(COLORVERTEX));
                        for (int k = 0; k < index->m_numVertices; k++) {
                            int point = index->m_vertices[k];
                            vertex->uv = V2((n - (float)(point / side)) / sv + v0, (float)(point % side) / su + u0);
                            vertex++;
                            submesh->m_uvsDone = true;
                        }
                    }
                    break;
                case 2:
                    if (!master->UseHardwareLighting()) {
                        LITVERTEX *vertex = (LITVERTEX *)(vertices[submesh->m_mesh] + first * sizeof(LITVERTEX));
                        for (int k = 0; k < index->m_numVertices; k++) {
                            int point = index->m_vertices[k];
                            vertex->uv = V2((n - (float)(point % side)) / su + u0, (n - (float)(point / side)) / sv + v0);
                            vertex++;
                            submesh->m_uvsDone = true;
                        }
                    } else {
                        COLORVERTEX *vertex = (COLORVERTEX *)(vertices[submesh->m_mesh] + first * sizeof(COLORVERTEX));
                        for (int k = 0; k < index->m_numVertices; k++) {
                            int point = index->m_vertices[k];
                            vertex->uv = V2((n - (float)(point % side)) / su + u0, (n - (float)(point / side)) / sv + v0);
                            vertex++;
                            submesh->m_uvsDone = true;
                        }
                    }
                    break;
                case 3:
                    if (!master->UseHardwareLighting()) {
                        LITVERTEX *vertex = (LITVERTEX *)(vertices[submesh->m_mesh] + first * sizeof(LITVERTEX));
                        for (int k = 0; k < index->m_numVertices; k++) {
                            int point = index->m_vertices[k];
                            vertex->uv = V2((float)(point / side) / sv + v0, (n - (float)(point % side)) / su + u0);
                            vertex++;
                            submesh->m_uvsDone = true;
                        }
                    } else {
                        COLORVERTEX *vertex = (COLORVERTEX *)(vertices[submesh->m_mesh] + first * sizeof(COLORVERTEX));
                        for (int k = 0; k < index->m_numVertices; k++) {
                            int point = index->m_vertices[k];
                            vertex->uv = V2((float)(point / side) / sv + v0, (n - (float)(point % side)) / su + u0);
                            vertex++;
                            submesh->m_uvsDone = true;
                        }
                    }
                    break;
                case 4:
                    if (!master->UseHardwareLighting()) {
                        LITVERTEX *vertex = (LITVERTEX *)(vertices[submesh->m_mesh] + first * sizeof(LITVERTEX));
                        for (int k = 0; k < index->m_numVertices; k++) {
                            int point = index->m_vertices[k];
                            vertex->uv = V2((n - (float)(point % side)) / su + u0, (float)(point / side) / sv + v0);
                            vertex++;
                            submesh->m_uvsDone = true;
                        }
                    } else {
                        COLORVERTEX *vertex = (COLORVERTEX *)(vertices[submesh->m_mesh] + first * sizeof(COLORVERTEX));
                        for (int k = 0; k < index->m_numVertices; k++) {
                            int point = index->m_vertices[k];
                            vertex->uv = V2((n - (float)(point % side)) / su + u0, (float)(point / side) / sv + v0);
                            vertex++;
                            submesh->m_uvsDone = true;
                        }
                    }
                    break;
                case 5:
                    if (!master->UseHardwareLighting()) {
                        LITVERTEX *vertex = (LITVERTEX *)(vertices[submesh->m_mesh] + first * sizeof(LITVERTEX));
                        for (int k = 0; k < index->m_numVertices; k++) {
                            int point = index->m_vertices[k];
                            vertex->uv = V2((float)(point / side) / sv + v0, (float)(point % side) / su + u0);
                            vertex++;
                            submesh->m_uvsDone = true;
                        }
                    } else {
                        COLORVERTEX *vertex = (COLORVERTEX *)(vertices[submesh->m_mesh] + first * sizeof(COLORVERTEX));
                        for (int k = 0; k < index->m_numVertices; k++) {
                            int point = index->m_vertices[k];
                            vertex->uv = V2((float)(point / side) / sv + v0, (float)(point % side) / su + u0);
                            vertex++;
                            submesh->m_uvsDone = true;
                        }
                    }
                    break;
                case 6:
                    if (!master->UseHardwareLighting()) {
                        LITVERTEX *vertex = (LITVERTEX *)(vertices[submesh->m_mesh] + first * sizeof(LITVERTEX));
                        for (int k = 0; k < index->m_numVertices; k++) {
                            int point = index->m_vertices[k];
                            vertex->uv = V2((float)(point % side) / su + u0, (n - (float)(point / side)) / sv + v0);
                            vertex++;
                            submesh->m_uvsDone = true;
                        }
                    } else {
                        COLORVERTEX *vertex = (COLORVERTEX *)(vertices[submesh->m_mesh] + first * sizeof(COLORVERTEX));
                        for (int k = 0; k < index->m_numVertices; k++) {
                            int point = index->m_vertices[k];
                            vertex->uv = V2((float)(point % side) / su + u0, (n - (float)(point / side)) / sv + v0);
                            vertex++;
                            submesh->m_uvsDone = true;
                        }
                    }
                    break;
                case 7:
                    if (!master->UseHardwareLighting()) {
                        LITVERTEX *vertex = (LITVERTEX *)(vertices[submesh->m_mesh] + first * sizeof(LITVERTEX));
                        for (int k = 0; k < index->m_numVertices; k++) {
                            int point = index->m_vertices[k];
                            vertex->uv = V2((n - (float)(point / side)) / sv + v0, (n - (float)(point % side)) / su + u0);
                            vertex++;
                            submesh->m_uvsDone = true;
                        }
                    } else {
                        COLORVERTEX *vertex = (COLORVERTEX *)(vertices[submesh->m_mesh] + first * sizeof(COLORVERTEX));
                        for (int k = 0; k < index->m_numVertices; k++) {
                            int point = index->m_vertices[k];
                            vertex->uv = V2((n - (float)(point / side)) / sv + v0, (n - (float)(point % side)) / su + u0);
                            vertex++;
                            submesh->m_uvsDone = true;
                        }
                    }
                    break;
                }
            }
        }
    }
    if (!m_colorsDone || !m_uvsDone) {
        for (i = 0; i < m_numMeshes; i++)
            m_meshes[i]->UnlockVertices();
    }
    m_colorsDone = true;
    m_uvsDone = true;
    m_verticesDone = true;
}
