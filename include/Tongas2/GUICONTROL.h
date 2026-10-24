// The GUI controls: what the GUI's screens are made of. Defined elsewhere;
// only what other units need so far. Placeholder names apart from the class
// name.
#ifndef TONGAS2_GUICONTROL_H
#define TONGAS2_GUICONTROL_H

#include "Tongas2/CMDBASE.h"

class GUICONTROL : public CMDBASE
{
public:
    static void RegisterCommands();

    // Defined elsewhere.
    virtual int Virtual08();
    virtual void *Virtual09(const char *name);
    virtual void Virtual19();
    virtual void Virtual20();
    virtual void Virtual21();
    // The placement on the screen, from 0 to 1, and the size.
    virtual void SetPosition(const V2 &position);
    virtual void SetSize(const V2 &size);
    virtual void SetZSort(const int &zsort);
    virtual void Virtual25(const bool &value);
    virtual void Virtual26(const bool &value);
    virtual void SetOpacity(const float &opacity);
    virtual void Virtual28();
    virtual void Virtual29();
    // Steps the control.
    virtual void Update();
    virtual void Virtual31();
    // The highest zsort of the controls in it that take the mouse.
    virtual int GetTopZSort();
    virtual void Virtual33();
    virtual void Virtual34();
    virtual void Virtual35();
    virtual void Virtual36();
    virtual void Virtual37();
    // Fades the control in or out.
    virtual void FadeTo(float opacity, float time);
    virtual void Virtual39();
    virtual void Virtual40();
    virtual void Virtual41();
    virtual void Virtual42();
    // The control was clicked.
    virtual void Click();
    virtual void Virtual44();
    virtual void Virtual45();
    virtual void Virtual46();
    virtual void Virtual47();
    virtual void Virtual48();
    virtual void Virtual49();
    virtual void Virtual50();
    virtual void Virtual51();
    virtual void Virtual52();
    virtual void Virtual53();
    virtual void Virtual54();
    virtual void Virtual55();
    // Whether the control can take the mouse now.
    virtual bool IsActive();

    // Whether it and the groups it is in are visible, and enabled.
    bool IsVisible();
    bool IsEnabled();

    static int s_classId;
    static PROPENTRY s_properties[];

    char m_1c[0x10];
    V2 m_position;
    char m_34[0xc];
    bool m_enabled;
    bool m_visible;
    char m_42[0xe];
    int m_zsort;
    char m_54[4];
    GUICONTROL *m_parent;
};

// Defined elsewhere: the behaviors controls can have (BLINK, GROW, TRAVEL
// and the rest), by name.
class GUIBEHAVIORS
{
public:
    GUIBEHAVIORS();
};
extern GUIBEHAVIORS *g_pGuiBehaviors;

#endif
