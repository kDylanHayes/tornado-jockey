#include "Tongas2/SHADER.h"
#include "Tongas2/LOG.h"
#include "Tongas2/PATH.h"
#include "Tongas2/RESOURCES.h"

static RESOURCEMANAGER *CreateSHADER();

CREATEMANAGER g_pfnCreateSHADER = CreateSHADER;

static const char s_layerBlends[][30] = {
    "add",
    "multiply",
    "multiply2x",
    "replace",
    "",
};

static const char s_uvEffects[][30] = {
    "none",
    "reflection",
    "reflectionFast",
    "refraction",
    "refractionFast",
    "",
};

static const char s_blends[][30] = {
    "blend",
    "replace",
    "multiply",
    "add",
    "weightedadd",
    "",
};

static const char s_filters[][30] = {
    "none",
    "point",
    "lineal",
    "",
};

static const char s_cullings[][30] = {
    "none",
    "clock",
    "counterclock",
    "",
};

CMDENTRY SHADERMASTER::s_commands[] = {
    { 0, 0, 0, 0, 0 },
};

PROPENTRY SHADERMASTER::s_properties[] = {
    { "GridRows", 0, 0, 0, PROP_FIELD(SHADERMASTER, m_gridRows), false, 1, PROP_SETTER(SHADERMASTER, SetGridRows) },
    { "GridCols", 0, 0, 0, PROP_FIELD(SHADERMASTER, m_gridCols), false, 1, PROP_SETTER(SHADERMASTER, SetGridCols) },
    { "ZWrite", 0, 2, 0, PROP_FIELD(SHADERMASTER, m_zWrite), false, 1, PROP_SETTER(SHADERMASTER, SetZWrite) },
    { "ZTest", 0, 2, 0, PROP_FIELD(SHADERMASTER, m_zTest), false, 1, PROP_SETTER(SHADERMASTER, SetZTest) },
    { "Blend", 0, 1, s_blends, PROP_FIELD(SHADERMASTER, m_blend), false, 1, PROP_SETTER(SHADERMASTER, SetBlend) },
    { "Layers", 0, 0, 0, PROP_FIELD(SHADERMASTER, m_layers), false, 1, PROP_SETTER(SHADERMASTER, SetLayers) },
    { "Lighting", 0, 2, 0, PROP_FIELD(SHADERMASTER, m_lighting), false, 1, PROP_SETTER(SHADERMASTER, SetLighting) },
    { "RenderLines", 0, 2, 0, PROP_FIELD(SHADERMASTER, m_renderLines), false, 1, PROP_SETTER(SHADERMASTER, SetRenderLines) },
    { "Culling", 0, 1, s_cullings, PROP_FIELD(SHADERMASTER, m_culling), false, 1, PROP_SETTER(SHADERMASTER, SetCulling) },
    { "AllowResize", 0, 2, 0, PROP_FIELD(SHADERMASTER, m_allowResize), false, 1, PROP_SETTER(SHADERMASTER, SetAllowResize) },
    { "AllowCompress", 0, 2, 0, PROP_FIELD(SHADERMASTER, m_allowCompress), false, 1, PROP_SETTER(SHADERMASTER, SetAllowCompress) },
    { "DiffuseColor", 0, 13, 0, PROP_FIELD(SHADERMASTER, m_diffuseColor), false, 1, PROP_SETTER(SHADERMASTER, SetDiffuseColor) },
    { "DiffuseAlpha", 0, 0, 0, PROP_FIELD(SHADERMASTER, m_diffuseAlpha), false, 1, PROP_SETTER(SHADERMASTER, SetDiffuseAlpha) },
    { "AmbientColor", 0, 13, 0, PROP_FIELD(SHADERMASTER, m_ambientColor), false, 1, PROP_SETTER(SHADERMASTER, SetAmbientColor) },
    { "SpecularColor", 0, 13, 0, PROP_FIELD(SHADERMASTER, m_specularColor), false, 1, PROP_SETTER(SHADERMASTER, SetSpecularColor) },
    { "EmissiveColor", 0, 13, 0, PROP_FIELD(SHADERMASTER, m_emissiveColor), false, 1, PROP_SETTER(SHADERMASTER, SetEmissiveColor) },
    { "SpecularPowerValue", 0, 3, 0, PROP_FIELD(SHADERMASTER, m_specularPower), false, 1, PROP_SETTER(SHADERMASTER, SetSpecularPowerValue) },
    { "LayersDiffuseName", 0, 7, 0, PROP_FIELD(SHADERMASTER, m_layersDiffuseName), false, SHADER_MAX_LAYERS, PROP_SETTER(SHADERMASTER, SetLayersDiffuseName) },
    { "LayersDiffuseColor", 0, 13, 0, PROP_FIELD(SHADERMASTER, m_layersDiffuseColor), false, SHADER_MAX_LAYERS, PROP_SETTER(SHADERMASTER, SetLayersDiffuseColor) },
    { "LayersDiffuseWidth", 0, 0, 0, PROP_FIELD(SHADERMASTER, m_layersDiffuseWidth), false, SHADER_MAX_LAYERS, PROP_SETTER(SHADERMASTER, SetLayersDiffuseWidth) },
    { "LayersDiffuseHeight", 0, 0, 0, PROP_FIELD(SHADERMASTER, m_layersDiffuseHeight), false, SHADER_MAX_LAYERS, PROP_SETTER(SHADERMASTER, SetLayersDiffuseHeight) },
    { "LayersOpacityName", 0, 7, 0, PROP_FIELD(SHADERMASTER, m_layersOpacityName), false, SHADER_MAX_LAYERS, PROP_SETTER(SHADERMASTER, SetLayersOpacityName) },
    { "LayersBlend", 0, 1, s_layerBlends, PROP_FIELD(SHADERMASTER, m_layersBlend), false, SHADER_MAX_LAYERS, PROP_SETTER(SHADERMASTER, SetLayersBlend) },
    { "LayersUVEffect", 0, 1, s_uvEffects, PROP_FIELD(SHADERMASTER, m_layersUVEffect), false, SHADER_MAX_LAYERS, PROP_SETTER(SHADERMASTER, SetLayersUVEffect) },
    { "LayersMipmaps", 0, 0, 0, PROP_FIELD(SHADERMASTER, m_layersMipmaps), false, SHADER_MAX_LAYERS, PROP_SETTER(SHADERMASTER, SetLayersMipmaps) },
    { "ShifterScale", 0, 10, 0, PROP_FIELD(SHADERMASTER, m_shifterScale), false, SHADER_MAX_LAYERS, PROP_SETTER(SHADERMASTER, SetShifterScale) },
    { "ShifterSpeed", 0, 10, 0, PROP_FIELD(SHADERMASTER, m_shifterSpeed), false, SHADER_MAX_LAYERS, PROP_SETTER(SHADERMASTER, SetShifterSpeed) },
    { "FilterMipMap", 0, 1, s_filters, PROP_FIELD(SHADERMASTER, m_filterMipMap), false, SHADER_MAX_LAYERS, PROP_SETTER(SHADERMASTER, SetFilterMipMap) },
    { "FilterMinimize", 0, 1, s_filters, PROP_FIELD(SHADERMASTER, m_filterMinimize), false, SHADER_MAX_LAYERS, PROP_SETTER(SHADERMASTER, SetFilterMinimize) },
    { "FilterMaximize", 0, 1, s_filters, PROP_FIELD(SHADERMASTER, m_filterMaximize), false, SHADER_MAX_LAYERS, PROP_SETTER(SHADERMASTER, SetFilterMaximize) },
    { 0, 0, 15, 0, 0, false },
};

static RESOURCEMANAGER *CreateSHADER()
{
    if (g_pLog)
        g_pLog->Error("No default plugin for SHADERMANAGER");
    return 0;
}

void SHADERMASTER::RegisterCommands()
{
    g_classParents[s_classId] = RESOURCEMASTER::s_classId;
    g_classCommands[s_classId] = s_commands;
    HashCommands(s_commands);
    RegisterClassName(s_commands, "SHADERMASTER", "RESOURCEMASTER");
}

void SHADERMASTER::AddUser()
{
    m_users++;
    Virtual35();
}

void SHADERMASTER::RemoveUser()
{
    m_users--;
    Virtual36();
}

// Some setters store in both branches, which keeps the original's two returns.
void SHADERMASTER::SetLayers(const int &layers)
{
    if (m_users > 0 && m_layers != layers) {
        m_dirty |= SHADER_DIRTY_TEXTURES;
        m_layers = layers;
    } else
        m_layers = layers;
}

void SHADERMASTER::SetDiffuseColor(const V3I &color)
{
    if (m_users > 0 && (m_diffuseColor.x != color.x || m_diffuseColor.y != color.y || m_diffuseColor.z != color.z))
        m_dirty |= SHADER_DIRTY_MATERIAL;
    m_diffuseColor = color;
}

void SHADERMASTER::SetDiffuseAlpha(const int &alpha)
{
    if (m_users > 0 && m_diffuseAlpha != alpha) {
        m_dirty |= SHADER_DIRTY_MATERIAL;
        m_diffuseAlpha = alpha;
    } else
        m_diffuseAlpha = alpha;
}

void SHADERMASTER::SetAmbientColor(const V3I &color)
{
    if (m_users > 0 && (m_ambientColor.x != color.x || m_ambientColor.y != color.y || m_ambientColor.z != color.z))
        m_dirty |= SHADER_DIRTY_MATERIAL;
    m_ambientColor = color;
}

void SHADERMASTER::SetSpecularColor(const V3I &color)
{
    if (m_users > 0 && (m_specularColor.x != color.x || m_specularColor.y != color.y || m_specularColor.z != color.z))
        m_dirty |= SHADER_DIRTY_MATERIAL;
    m_specularColor = color;
}

void SHADERMASTER::SetEmissiveColor(const V3I &color)
{
    if (m_users > 0 && (m_emissiveColor.x != color.x || m_emissiveColor.y != color.y || m_emissiveColor.z != color.z))
        m_dirty |= SHADER_DIRTY_MATERIAL;
    m_emissiveColor = color;
}

void SHADERMASTER::SetSpecularPowerValue(const float &power)
{
    if (m_users > 0 && m_specularPower != power)
        m_dirty |= SHADER_DIRTY_MATERIAL;
    m_specularPower = power;
}

void SHADERMASTER::SetAllowResize(const bool &allow)
{
    if (m_users > 0 && m_allowResize != allow) {
        m_dirty |= SHADER_DIRTY_TEXTURES;
        m_allowResize = allow;
    } else
        m_allowResize = allow;
}

void SHADERMASTER::SetAllowCompress(const bool &allow)
{
    if (m_users > 0 && m_allowCompress != allow) {
        m_dirty |= SHADER_DIRTY_TEXTURES;
        m_allowCompress = allow;
    } else
        m_allowCompress = allow;
}

void SHADERMASTER::SetLayersDiffuseColor(const int &i, const V3I &color)
{
    if (m_users > 0) {
        const V3I &old = m_layersDiffuseColor[i];
        if (old.x != color.x || old.y != color.y || old.z != color.z)
            m_dirty |= SHADER_DIRTY_TEXTURES;
    }
    m_layersDiffuseColor[i] = color;
}

void SHADERMASTER::SetLayersDiffuseWidth(const int &i, const int &width)
{
    if (m_users > 0 && m_layersDiffuseWidth[i] != width)
        m_dirty |= SHADER_DIRTY_TEXTURES;
    m_layersDiffuseWidth[i] = width;
}

void SHADERMASTER::SetLayersDiffuseHeight(const int &i, const int &height)
{
    if (m_users > 0 && m_layersDiffuseHeight[i] != height)
        m_dirty |= SHADER_DIRTY_TEXTURES;
    m_layersDiffuseHeight[i] = height;
}

void SHADERMASTER::SetLayersMipmaps(const int &i, const int &mipmaps)
{
    if (m_users > 0 && m_layersMipmaps[i] != mipmaps)
        m_dirty |= SHADER_DIRTY_TEXTURES;
    m_layersMipmaps[i] = mipmaps;
}

SHADER::SHADER(SHADERMASTER *master, const RESHANDLE &handle) : RESOURCE(master, handle)
{
}

UVRECT SHADER::GetFrame(int frame)
{
    return UVRECT(0.0f, 1.0f, 0.0f, 1.0f);
}

SHADERMASTER::SHADERMASTER(RESOURCEMANAGER *manager, const char *name) : RESOURCEMASTER(manager, name)
{
    m_gridRows = 1;
    m_gridCols = 1;
    m_zWrite = true;
    m_zTest = true;
    m_blend = 0;
    m_layers = 1;
    m_lighting = true;
    m_renderLines = false;
    m_culling = 0;
    m_allowResize = true;
    m_allowCompress = true;
    m_diffuseColor = V3I(255, 255, 255);
    m_diffuseAlpha = 255;
    m_ambientColor = V3I(128, 128, 128);
    m_specularColor = V3I();
    m_emissiveColor = V3I();
    m_specularPower = 0.0f;
    for (g_i = 0; g_i < SHADER_MAX_LAYERS; g_i++)
        m_layersDiffuseName[g_i] = "";
    for (g_i = 0; g_i < SHADER_MAX_LAYERS; g_i++)
        m_layersDiffuseColor[g_i] = V3I();
    for (g_i = 0; g_i < SHADER_MAX_LAYERS; g_i++)
        m_layersDiffuseWidth[g_i] = 0;
    for (g_i = 0; g_i < SHADER_MAX_LAYERS; g_i++)
        m_layersDiffuseHeight[g_i] = 0;
    for (g_i = 0; g_i < SHADER_MAX_LAYERS; g_i++)
        m_layersOpacityName[g_i] = "";
    for (g_i = 0; g_i < SHADER_MAX_LAYERS; g_i++)
        m_layersBlend[g_i] = 1;
    for (g_i = 0; g_i < SHADER_MAX_LAYERS; g_i++)
        m_layersUVEffect[g_i] = 0;
    for (g_i = 0; g_i < SHADER_MAX_LAYERS; g_i++)
        m_layersMipmaps[g_i] = 4;
    for (g_i = 0; g_i < SHADER_MAX_LAYERS; g_i++)
        m_shifterScale[g_i] = V2(1.0f, 1.0f);
    for (g_i = 0; g_i < SHADER_MAX_LAYERS; g_i++)
        m_shifterSpeed[g_i] = V2(0.0f, 0.0f);
    for (g_i = 0; g_i < SHADER_MAX_LAYERS; g_i++)
        m_filterMipMap[g_i] = 2;
    for (g_i = 0; g_i < SHADER_MAX_LAYERS; g_i++)
        m_filterMinimize[g_i] = 2;
    for (g_i = 0; g_i < SHADER_MAX_LAYERS; g_i++)
        m_filterMaximize[g_i] = 2;
    m_users = 0;
    m_dirty = 0;
    m_148 = 0;
    m_14c = 0;
}

RESOURCE *SHADERMASTER::CreateCopy(const RESHANDLE &handle)
{
    return new SHADER(this, handle);
}

void SHADERMASTER::SetLayersDiffuseName(const int &i, const CSTRING &name)
{
    if (m_users > 0 && m_layersDiffuseName[i] != name)
        m_dirty |= SHADER_DIRTY_TEXTURES;
    m_layersDiffuseName[i] = name;
}

void SHADERMASTER::SetLayersOpacityName(const int &i, const CSTRING &name)
{
    if (m_users > 0 && m_layersOpacityName[i] != name)
        m_dirty |= SHADER_DIRTY_TEXTURES;
    m_layersOpacityName[i] = name;
}
