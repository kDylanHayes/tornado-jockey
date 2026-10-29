// The GUI, at 0x6d09bc: the screen being shown, made from a GUI group
// template, the screens closed since the last frame, the controls by handle,
// moving between them with the arrow keys, the black screen it fades in and
// out, and the menu sounds. Placeholder names apart from the class name and
// the names its log messages, commands and properties give.
#ifndef TONGAS2_GUI_H
#define TONGAS2_GUI_H

#include <set>

#include "Tongas2/CMDBASE.h"
#include "Tongas2/FIXEDSTRING.h"
#include "Tongas2/FIXEDWSTRING.h"
#include "Tongas2/INPUT.h"

#define GUI_MAX_CONTROLS 0x400

class GUICONTROL;
class GUIGROUP;

// Defined in another unit; each name is 30 characters, indexed by class id.
extern char g_classNames[][30];

// Ways to move between the controls with the keyboard.
enum GUIDIRECTION
{
    GUIDIR_UP,
    GUIDIR_DOWN,
    GUIDIR_LEFT,
    GUIDIR_RIGHT,
};

class GUI : public CMDBASE, public INPUTLISTENER
{
public:
    GUI();
    virtual ~GUI();

    static void RegisterCommands();

    virtual int GetClassId() { return s_classId; }
    virtual int GetParentClassId() { return g_classParents[s_classId]; }
    virtual const char *GetClassName() { return g_classNames[s_classId]; }
    virtual SRESULT Execute(CMDENTRY *command, TOKENS **args)
    {
        return (this->*(SRESULT (GUI::*)(TOKENS **))command->func)(args);
    }
    // Defined elsewhere.
    virtual int Virtual08();
    // The object a script names, in the screen.
    virtual void *Virtual09(const char *name);
    // Called at the end of each Update.
    virtual void OnUpdate() {}
    // Shows the screen made from the GUI group template with the name.
    virtual void SetScreen(const FIXEDSTRING<0x200> &name);
    virtual void SetFps(const float &fps) { m_fps = fps; }
    virtual void SetLoadingProgress(const float &progress) { m_loadingProgress = progress; }
    virtual void SetVersionString(const FIXEDWSTRING<0x200> &version) { m_versionString = version; }
    // The blackopacity, blackfade and blackzsort commands.
    virtual SRESULT BlackOpacity(CMDCALL *call);
    virtual SRESULT BlackFade(CMDCALL *call);
    virtual SRESULT BlackZSort(CMDCALL *call);
    // Called once a screen opens.
    virtual void OnScreenOpened() {}
    // Opens the screen again, as when the screen's shape changes.
    virtual void ReloadScreen();
    // Called when the game goes fullscreen or windowed; pauses the game.
    virtual void OnFullscreenChanged();
    // The zsort screens open at.
    virtual int GetScreenZSort() { return 0x78; }

    virtual bool OnKeyDown(unsigned int key);

    // The black screen's zsort and opacity, and a fade of it to an opacity
    // over a time.
    void SetBlackZSort(int zsort);
    void SetBlackOpacity(float opacity);
    void FadeBlack(float from, float to, float time);
    // A group made from a template, put in the screen.
    GUIGROUP *AddGroup(const char *name);
    // Pauses the game, or with toggle carries on from a pause.
    void Pause(bool toggle);
    // The control with the handle.
    CMDBASE *GetObject(const RESHANDLE &handle);
    // Gives a control a handle, or lets the handle go.
    void AddControl(GUICONTROL *control);
    void RemoveControl(GUICONTROL *control);
    // Places the screen's controls again.
    void UpdateLayout();
    // Whether the control is one the keyboard can move to, under the mouse
    // or not.
    bool IsSelectable(GUICONTROL *control, const V2 &mouse);
    bool IsSelectableElsewhere(GUICONTROL *control, const V2 &mouse);
    // Moves the mouse to the control.
    bool MoveMouseTo(GUICONTROL *control);
    // Clicks the control under the mouse.
    bool ClickSelected();
    // The highest zsort in the screen, or -1 without one.
    int GetTopZSort();
    // Plays a menu sound in the next Update.
    void PlayClick();
    void PlayMouseOver();
    // Stops input reaching the screen, and lets it again.
    void LockInput();
    void UnlockInput();
    // Shows or hides the control with the name in the screen.
    void ShowControl(const char *name, float time);
    void HideControl(const char *name, float time);
    // How far the control is in the direction from a point, and whether it
    // is that way at all.
    bool GetDistance(const int &direction, const V2 &from, GUICONTROL *control, float *distance);
    // The control the keyboard moves to in the direction.
    GUICONTROL *FindNext(const int &direction);
    // The sound the next menu sound plays instead, or none.
    void SetSound(const char *name);
    bool MoveSelection(const int &direction);
    // A group made from a template and placed, put in the screen.
    GUIGROUP *AddGroup(const char *name, const char *groupName, const V2 &position, const V2 &size,
        const float &opacity, const int &zsort);
    // Deletes the screens closed since the last frame.
    void DeleteClosedScreens();
    // Fades the black screen, passes on the frame rate and steps the screen.
    void Update();
    // Opens the screen made from the GUI group template.
    void OpenScreen(RESHANDLE master);
    // Closes the screen; it is deleted in the next Update.
    void CloseScreen()
    {
        if (m_screen) {
            m_closedScreens.insert(m_screen);
            m_screen = 0;
        }
    }

    static int s_classId;
    static CMDENTRY s_commands[];
    static PROPENTRY s_properties[];

    // While above 0, keys don't reach the screen.
    int m_inputLock;
    // Whether the screen's shape called for corrections, and the game was
    // fullscreen, last frame.
    bool m_aspectFix;
    bool m_fullscreen;
    // The name of the screen's template.
    FIXEDSTRING<0x200> m_screenName;
    float m_fps;
    float m_loadingProgress;
    FIXEDWSTRING<0x200> m_versionString;
    // The controls by handle index, the slots handed out and the free ones.
    GUICONTROL *m_controls[GUI_MAX_CONTROLS];
    int m_nextFree;
    int m_serial;
    int m_freeList[GUI_MAX_CONTROLS];
    int m_numFree;
    int m_count;
    // One above the input system's top listener priority when the screen
    // opened.
    int m_openPriority;
    GUIGROUP *m_screen;
    // The black screen decal, the opacity it fades to and the time left.
    RESHANDLE m_black;
    float m_fadeTarget;
    float m_fadeTime;
    std::set<GUIGROUP *> m_closedScreens;
    bool m_playClick;
    bool m_playMouseOver;
    FIXEDSTRING<0x200> m_sound;
    bool m_hasSound;
};

extern GUI *g_pGui;

// Makes the GUI; the game points it at its own.
extern GUI *(*g_pfnCreateGui)();

#endif
