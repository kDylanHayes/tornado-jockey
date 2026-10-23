// GUI sliders: GUISLIDERMASTER, from GUISLIDER.txt, is a GUI group with a
// slider visual and a command run when the slider moves. Placeholder names
// apart from the class name and properties.
#ifndef TONGAS2_GUISLIDER_H
#define TONGAS2_GUISLIDER_H

#include "Tongas2/GUIGROUP.h"

class GUISLIDERMASTER : public GUIGROUPMASTER
{
public:
    GUISLIDERMASTER(RESOURCEMANAGER *manager, const char *name);
    virtual ~GUISLIDERMASTER() {}

    static void RegisterCommands();

    virtual int GetClassId();
    virtual int GetParentClassId();
    virtual const char *GetClassName();
    virtual SRESULT Execute(CMDENTRY *command, TOKENS **args)
    {
        return (this->*(SRESULT (GUISLIDERMASTER::*)(TOKENS **))command->func)(args);
    }

    virtual void SetSliderVisualName(const CSTRING &name) { m_sliderVisualName = name; }
    virtual void SetSliderVisualIdx(const int &idx) { m_sliderVisualIdx = idx; }
    virtual void SetOnChange(const CSTRING &command) { m_onChange = command; }

    static int s_classId;
    static CMDENTRY s_commands[];
    static PROPENTRY s_properties[];

    CSTRING m_sliderVisualName;
    int m_sliderVisualIdx;
    CSTRING m_onChange;
};

#endif
