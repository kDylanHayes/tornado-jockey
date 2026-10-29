// The GUI controls: what the GUI's screens are made of. A control has a
// place and size on the screen, a depth, an opacity and a behavior such as
// fading or blinking, and the move, fade and close script commands; the
// GUI keeps a list of them. Placeholder names apart from the class name,
// the commands and the properties.
#ifndef TONGAS2_GUICONTROL_H
#define TONGAS2_GUICONTROL_H

#include "Tongas2/ACTORLINK.h"
#include "Tongas2/CMDBASE.h"
#include "Tongas2/CSTRING.h"
#include "Tongas2/SAFEINDEX.h"

class GUIGROUP;
class GUICONTROL;

extern char g_classNames[][30];

// A 2D position or size that starts at zero.
struct GUICONTROLV2 : V2
{
    GUICONTROLV2() : V2(0.0f, 0.0f) {}
    GUICONTROLV2 &operator=(const V2 &v) { V2::operator=(v); return *this; }
    GUICONTROLV2 &operator+=(const V2 &other)
    {
        x += other.x;
        y += other.y;
        return *this;
    }
};

inline V2 operator*(const V2 &v, const float &f)
{
    return V2(v.x * f, v.y * f);
}

// Defined elsewhere: v divided by f, or zero when f is.
V2 operator/(const V2 &v, const float &f);

// The behaviors a control can have, made when its Behavior is set.
// Defined elsewhere.
class GUIBEHAVIOR
{
public:
    virtual ~GUIBEHAVIOR();
    // Steps the behavior.
    virtual void Update();
    virtual void Virtual02();
    // Reads the control's BehaviorParams.
    virtual void SetParams(char *params);
};

class GUIJITTER : public GUIBEHAVIOR
{
public:
    GUIJITTER(GUICONTROL *control);

    char m_4[0x38];
};

class GUIFADE : public GUIBEHAVIOR
{
public:
    GUIFADE(GUICONTROL *control);

    char m_4[0x2c];
};

class GUISIZE : public GUIBEHAVIOR
{
public:
    GUISIZE(GUICONTROL *control);

    char m_4[0x38];
};

class GUIBLINK : public GUIBEHAVIOR
{
public:
    GUIBLINK(GUICONTROL *control);

    char m_4[0x2c];
};

class GUIDIEFADE : public GUIBEHAVIOR
{
public:
    GUIDIEFADE(GUICONTROL *control);

    char m_4[0x18];
};

// The values of a control's Behavior.
enum GUIBEHAVIORTYPE
{
    GUIBEHAVIOR_NONE,
    GUIBEHAVIOR_JITTER,
    GUIBEHAVIOR_FADE,
    GUIBEHAVIOR_SIZE,
    GUIBEHAVIOR_BLINK,
    GUIBEHAVIOR_DIEFADE,
};

class GUICONTROL : public CMDBASE, public ACTORLINK
{
public:
    // Adds itself to the GUI's controls.
    GUICONTROL(const RESHANDLE &master, GUIGROUP *parent);
    virtual ~GUICONTROL();

    static void RegisterCommands();

    virtual int GetClassId() { return s_classId; }
    virtual int GetParentClassId() { return g_classParents[s_classId]; }
    virtual const char *GetClassName() { return g_classNames[s_classId]; }
    virtual SRESULT Execute(CMDENTRY *command, TOKENS **args)
    {
        return (this->*(SRESULT (GUICONTROL::*)(TOKENS **))command->func)(args);
    }
    // The group it is in, or the GUI.
    virtual int Virtual08();
    virtual void *Virtual09(const char *name) { return 0; }
    virtual int Virtual19() { return 0; }
    virtual int Virtual20() { return -1; }
    // Whether it goes by game time (0) or real time.
    virtual void SetTimeSpace(const int &timeSpace) { m_timeSpace = timeSpace; }
    // The placement on the screen, from 0 to 1, and the size.
    virtual void SetPosition(const V2 &position) { m_position = position; }
    virtual void SetSize(const V2 &size) { m_size = size; }
    virtual void SetZSort(const int &zsort) { m_zsort = zsort; }
    virtual void SetVisible(const bool &visible);
    virtual void SetEnabled(const bool &enabled);
    virtual void SetOpacity(const float &opacity) { m_opacity = opacity; }
    // Makes the behavior.
    virtual void SetBehavior(const int &behavior);
    // Passes the parameters on to the behavior.
    virtual void SetBehaviorParams(const CSTRING &params);
    // Steps the control: its fade and move, and its behaviors.
    virtual void Update();
    virtual void SetTopZSort(int zsort) { m_topZSort = zsort; }
    // The highest zsort of the controls in it that take the mouse.
    virtual int GetTopZSort() { return m_topZSort; }
    virtual void Virtual33(int a, int b, int c, int d) {}
    virtual void Virtual34(int a, int b, int c, int d) {}
    // The move command: moves to a point over a time.
    virtual SRESULT Move(CMDCALL *call);
    // The moverel command: moves by an offset over a time.
    virtual SRESULT MoveRel(CMDCALL *call);
    // The moveAndDie command: moves, then closes.
    virtual SRESULT MoveAndDie(CMDCALL *call);
    // Fades the control in or out.
    virtual void FadeTo(float opacity, float time);
    // The fade command.
    virtual SRESULT Fade(CMDCALL *call);
    // The gparent command: runs a command on the group it is in.
    virtual SRESULT GParent(CMDCALL *call);
    // The Close command.
    virtual SRESULT CloseCommand(CMDCALL *call);
    // The QueueBh2 command: queues a behavior with its parameters.
    virtual SRESULT QueueBehaviorCommand(CMDCALL *call);

    virtual V2 Link01() { return m_position; }
    virtual void Link02(const V2 &position) { SetPosition(position); }
    virtual V2 Link03() { return m_size; }
    virtual void Link04(const V2 &size) { SetSize(size); }
    virtual V3 Link05() { return V3(0.0f, 0.0f, 0.0f); }
    virtual void Link06(const V3 &position) {}
    virtual void Link07(V3 *direction, V3 *up) {}
    virtual void Link08(const V3 &position, const V3 &direction) {}
    virtual V3 Link09() { return V3(1.0f, 1.0f, 1.0f); }
    virtual void Link10(const V3 &scale) {}
    virtual float Link11() { return m_opacity; }
    virtual void Link12(float opacity) { SetOpacity(opacity); }
    virtual void Link13() { Close(); }

    // Takes itself out of the group it is in, once.
    void Close();
    // Moves to target over time, closing at the end if die.
    void MoveTo(const V2 &target, float time, bool die);
    // Whether it and the groups it is in are enabled, and visible.
    bool IsEnabled();
    bool IsVisible();

    static int s_classId;
    static CMDENTRY s_commands[];
    static PROPENTRY s_properties[];

    int m_timeSpace;
    GUICONTROLV2 m_position;
    GUICONTROLV2 m_size;
    int m_zsort;
    bool m_visible;
    bool m_enabled;
    float m_opacity;
    int m_behavior;
    CSTRING m_behaviorParams;
    int m_topZSort;
    int m_54;
    GUIGROUP *m_parent;
    RESHANDLE m_master;
    bool m_closed;
    GUIBEHAVIOR *m_behaviorObject;
    char m_70[8];
    // The move: the time left, where to and where it is.
    float m_moveTime;
    GUICONTROLV2 m_moveTarget;
    GUICONTROLV2 m_movePos;
    // Close when the move or fade ends.
    bool m_dieWhenDone;
    // The fade: the time left, the opacity now and the one to reach.
    float m_fadeTime;
    float m_fadeOpacity;
    float m_fadeTarget;
    int m_9c;
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
