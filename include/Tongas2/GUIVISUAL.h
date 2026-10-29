// GUI visuals: GUIVISUALMASTER, from GUIVISUAL.txt, lists up to 32 textures
// or screen decals a GUI element draws, with their placement, and the
// progress bars they can show. Placeholder names apart from the class name
// and properties.
#ifndef TONGAS2_GUIVISUAL_H
#define TONGAS2_GUIVISUAL_H

#include "Tongas2/CSTRING.h"
#include "Tongas2/FIXEDSTRING.h"
#include "Tongas2/GUICONTROL.h"
#include "Tongas2/RESOURCE.h"
#include "Tongas2/RESOURCES.h"
#include "Tongas2/SHADER.h"
#include "Tongas2/SRESULT.h"

#define GUIVISUAL_MAX 32

// A 2D position or size that starts at zero.
struct GUIVISUALV2 : V2
{
    GUIVISUALV2() : V2(0.0f, 0.0f) {}
    GUIVISUALV2 &operator=(const V2 &v) { V2::operator=(v); return *this; }
};

class GUIVISUALMASTER : public RESOURCEMASTER
{
public:
    GUIVISUALMASTER(RESOURCEMANAGER *manager, const char *name);
    virtual ~GUIVISUALMASTER() {}

    static void RegisterCommands();

    virtual int GetClassId();
    virtual int GetParentClassId();
    virtual const char *GetClassName();
    virtual SRESULT Execute(CMDENTRY *command, TOKENS **args)
    {
        return (this->*(SRESULT (GUIVISUALMASTER::*)(TOKENS **))command->func)(args);
    }

    virtual void SetTexture(const int &i, const SRTEXTURE &texture) { m_textures[i] = texture; }
    virtual void SetScreenDecal(const int &i, const CSTRING &decal) { m_screenDecals[i] = decal; }
    virtual void SetPos(const int &i, const V2 &pos);
    virtual void SetRotation(const int &i, const float &rotation);
    virtual void SetSize(const int &i, const V2 &size);
    virtual void SetOpacity(const int &i, const float &opacity);
    virtual void SetZSort(const int &i, const int &zsort);
    virtual void SetBindProgressBar(const int &i, const CSTRING &bar) { m_bindProgressBar[i] = bar; }
    virtual void SetBindProgressBarMax(const int &i, const float &max);
    virtual void SetBarStartOffset(const int &i, const float &offset);
    virtual void SetBarEndOffset(const int &i, const float &offset);
    virtual void SetBarGrowDirection(const int &i, const int &direction);
    virtual void SetBarDiscrete(const int &i, const bool &discrete);
    virtual void SetBarDiscreteSteps(const int &i, const int &steps);
    virtual void SetBarGrowVel(const int &i, const float &vel);
    virtual void SetBarBehavior(const int &i, const int &behavior);
    virtual void SetBarMoveMax(const int &i, const V2 &max);
    virtual void SetReposOnScale(const bool &repos);

    static int s_classId;
    static CMDENTRY s_commands[];
    static PROPENTRY s_properties[];

    int m_60;
    SRTEXTURE m_textures[GUIVISUAL_MAX];
    CSTRING m_screenDecals[GUIVISUAL_MAX];
    GUIVISUALV2 m_pos[GUIVISUAL_MAX];
    float m_rotation[GUIVISUAL_MAX];
    GUIVISUALV2 m_size[GUIVISUAL_MAX];
    float m_opacity[GUIVISUAL_MAX];
    int m_zsort[GUIVISUAL_MAX];
    CSTRING m_bindProgressBar[GUIVISUAL_MAX];
    float m_bindProgressBarMax[GUIVISUAL_MAX];
    float m_barStartOffset[GUIVISUAL_MAX];
    float m_barEndOffset[GUIVISUAL_MAX];
    int m_barGrowDirection[GUIVISUAL_MAX];
    bool m_barDiscrete[GUIVISUAL_MAX];
    int m_barDiscreteSteps[GUIVISUAL_MAX];
    float m_barGrowVel[GUIVISUAL_MAX];
    int m_barBehavior[GUIVISUAL_MAX];
    GUIVISUALV2 m_barMoveMax[GUIVISUAL_MAX];
    bool m_reposOnScale;
};

// A texture's frame on screen that starts as the whole texture.
struct GUIVISUALUV : UVRECT
{
    GUIVISUALUV() : UVRECT(0.0f, 1.0f, 0.0f, 1.0f) {}
    GUIVISUALUV &operator=(const UVRECT &uv) { UVRECT::operator=(uv); return *this; }
};

// A GUI element made from a GUIVISUALMASTER: up to 32 decals or screen
// decals placed, sized and faded with the control, some of them progress
// bars that grow, shrink or move with a script value.
class GUIVISUAL : public GUICONTROL
{
public:
    GUIVISUAL(const RESHANDLE &master, GUIGROUP *parent);
    virtual ~GUIVISUAL();

    static void RegisterCommands();

    virtual int GetClassId() { return s_classId; }
    virtual int GetParentClassId() { return g_classParents[s_classId]; }
    virtual const char *GetClassName() { return g_classNames[s_classId]; }
    virtual SRESULT Execute(CMDENTRY *command, TOKENS **args)
    {
        return (this->*(SRESULT (GUIVISUAL::*)(TOKENS **))command->func)(args);
    }
    virtual int Virtual19() { return 4; }
    virtual void SetPosition(const V2 &position);
    virtual void SetSize(const V2 &size);
    virtual void SetZSort(const int &zsort);
    virtual void SetOpacity(const float &opacity);
    virtual void Update();
    virtual void Virtual34(V2 point, GUICONTROL **hit, int *zsort);

    // The properties' setters.
    virtual void SetTexture(const int &i, const SRTEXTURE &texture);
    virtual void SetScreenDecal(const int &i, const FIXEDSTRING<0x200> &name);
    virtual void SetDecalOrder(const int &i, const int &order);
    virtual void SetVisualOpacity(const int &i, const float &opacity)
    {
        if (i < GUIVISUAL_MAX) {
            m_visualOpacity[i] = opacity;
            SetOpacity(m_opacity);
        }
    }
    // The script command: brings every bar up to its value at once.
    virtual SRESULT UpdateBarsForced(CMDCALL *call);

    // Moves bar i toward its bound value, or straight to it if forced, and
    // sizes and places it for the value.
    void UpdateBar(int i, bool force);

    static int s_classId;
    static CMDENTRY s_commands[];
    static PROPENTRY s_properties[];

    SRTEXTURE m_textures[GUIVISUAL_MAX];
    FIXEDSTRING<0x200> m_screenDecals[GUIVISUAL_MAX];
    int m_decalOrder[GUIVISUAL_MAX];
    float m_visualOpacity[GUIVISUAL_MAX];
    GUIVISUALMASTER *m_visualMaster;
    // The decals made from the textures, and the screen decals.
    RESHANDLE m_textureCopies[GUIVISUAL_MAX];
    RESHANDLE m_decalCopies[GUIVISUAL_MAX];
    // Each bar's value as it grows toward the bound one.
    float m_barValue[GUIVISUAL_MAX];
    GUIVISUALUV m_frames[GUIVISUAL_MAX];
};

#endif
