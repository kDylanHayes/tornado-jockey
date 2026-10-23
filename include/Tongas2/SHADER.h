// Shaders: SHADERMASTER, from a shader file, holds the render states, the
// material colours and up to two texture layers with their blending, UV
// effect, scrolling and filtering. The engine has no shaders of its own:
// the renderer plugin supplies the manager, its masters and SHADER copies.
// Placeholder names apart from the class names and properties.
#ifndef TONGAS2_SHADER_H
#define TONGAS2_SHADER_H

#include "Tongas2/EMITTER.h"
#include "Tongas2/RESOURCE.h"
#include "Tongas2/SRESULT.h"

#define SHADER_MAX_LAYERS 2

// What a master's changes call for: new textures, or new material values.
#define SHADER_DIRTY_TEXTURES 1
#define SHADER_DIRTY_MATERIAL 2

// A layer's texture scroll, still until set.
struct SHADERSCROLL : V2
{
    SHADERSCROLL() : V2(0.0f, 0.0f) {}
    SHADERSCROLL &operator=(const V2 &other)
    {
        V2::operator=(other);
        return *this;
    }
};

class SHADERMASTER : public RESOURCEMASTER
{
public:
    SHADERMASTER(RESOURCEMANAGER *manager, const char *name);
    virtual ~SHADERMASTER() {}

    static void RegisterCommands();

    virtual int GetClassId() { return s_classId; }
    virtual int GetParentClassId() { return g_classParents[s_classId]; }
    virtual const char *GetClassName() { return g_classNames[s_classId]; }
    virtual SRESULT Execute(CMDENTRY *command, TOKENS **args)
    {
        return (this->*(SRESULT (SHADERMASTER::*)(TOKENS **))command->func)(args);
    }
    virtual RESOURCE *CreateCopy(const RESHANDLE &handle);

    // For the plugin: applying the shader, and its first and last users.
    virtual void Virtual34() {}
    virtual void Virtual35() {}
    virtual void Virtual36() {}

    virtual void SetGridRows(const int &rows) { m_gridRows = rows; }
    virtual void SetGridCols(const int &cols) { m_gridCols = cols; }
    virtual void SetZWrite(const bool &write) { m_zWrite = write; }
    virtual void SetZTest(const bool &test) { m_zTest = test; }
    virtual void SetBlend(const int &blend) { m_blend = blend; }
    virtual void SetLayers(const int &layers);
    virtual void SetLighting(const bool &lighting) { m_lighting = lighting; }
    virtual void SetRenderLines(const bool &lines) { m_renderLines = lines; }
    virtual void SetCulling(const int &culling) { m_culling = culling; }
    virtual void SetAllowResize(const bool &allow);
    virtual void SetAllowCompress(const bool &allow);
    virtual void SetDiffuseColor(const V3I &color);
    virtual void SetDiffuseAlpha(const int &alpha);
    virtual void SetAmbientColor(const V3I &color);
    virtual void SetSpecularColor(const V3I &color);
    virtual void SetEmissiveColor(const V3I &color);
    virtual void SetSpecularPowerValue(const float &power);
    virtual void SetLayersDiffuseName(const int &i, const CSTRING &name);
    virtual void SetLayersDiffuseColor(const int &i, const V3I &color);
    virtual void SetLayersDiffuseWidth(const int &i, const int &width);
    virtual void SetLayersDiffuseHeight(const int &i, const int &height);
    virtual void SetLayersOpacityName(const int &i, const CSTRING &name);
    virtual void SetLayersBlend(const int &i, const int &blend) { m_layersBlend[i] = blend; }
    virtual void SetLayersUVEffect(const int &i, const int &effect) { m_layersUVEffect[i] = effect; }
    virtual void SetLayersMipmaps(const int &i, const int &mipmaps);
    virtual void SetShifterScale(const int &i, const V2 &scale) { m_shifterScale[i] = scale; }
    virtual void SetShifterSpeed(const int &i, const V2 &speed) { m_shifterSpeed[i] = speed; }
    virtual void SetFilterMipMap(const int &i, const int &filter) { m_filterMipMap[i] = filter; }
    virtual void SetFilterMinimize(const int &i, const int &filter) { m_filterMinimize[i] = filter; }
    virtual void SetFilterMaximize(const int &i, const int &filter) { m_filterMaximize[i] = filter; }

    // Counts the decals and curves drawn with the shader.
    void AddUser();
    void RemoveUser();

    static int s_classId;
    static CMDENTRY s_commands[];
    static PROPENTRY s_properties[];

    int m_users;
    int m_gridRows;
    int m_gridCols;
    bool m_zWrite;
    bool m_zTest;
    int m_blend;
    int m_layers;
    bool m_lighting;
    bool m_renderLines;
    int m_culling;
    bool m_allowResize;
    bool m_allowCompress;
    V3I m_diffuseColor;
    int m_diffuseAlpha;
    V3I m_ambientColor;
    V3I m_specularColor;
    V3I m_emissiveColor;
    float m_specularPower;
    CSTRING m_layersDiffuseName[SHADER_MAX_LAYERS];
    V3I m_layersDiffuseColor[SHADER_MAX_LAYERS];
    int m_layersDiffuseWidth[SHADER_MAX_LAYERS];
    int m_layersDiffuseHeight[SHADER_MAX_LAYERS];
    CSTRING m_layersOpacityName[SHADER_MAX_LAYERS];
    int m_layersBlend[SHADER_MAX_LAYERS];
    int m_layersUVEffect[SHADER_MAX_LAYERS];
    int m_layersMipmaps[SHADER_MAX_LAYERS];
    SHADERSCROLL m_shifterScale[SHADER_MAX_LAYERS];
    SHADERSCROLL m_shifterSpeed[SHADER_MAX_LAYERS];
    int m_filterMipMap[SHADER_MAX_LAYERS];
    int m_filterMinimize[SHADER_MAX_LAYERS];
    int m_filterMaximize[SHADER_MAX_LAYERS];
    // SHADER_DIRTY_ flags for the plugin.
    int m_dirty;
    int m_148;
    int m_14c;
};

// A frame's part of a shader's texture.
struct UVRECT
{
    UVRECT() {}
    UVRECT(float left_, float right_, float top_, float bottom_)
    {
        top = top_;
        left = left_;
        bottom = bottom_;
        right = right_;
    }

    float left, right, top, bottom;
};

// One use of a shader; the plugin's copies derive from it.
class SHADER : public RESOURCE
{
public:
    SHADER(SHADERMASTER *master, const RESHANDLE &handle);

    // Defined elsewhere.
    virtual bool IsLoaded();

    virtual void Virtual29() {}
    virtual int Virtual30() { return 0; }
    virtual int Virtual31() { return 0; }
    // The whole texture, for a shader without frames.
    virtual UVRECT GetFrame(int frame);
};

#endif
