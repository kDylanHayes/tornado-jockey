// GUI text boxes: GUITEXTEDITMASTER, from GUITEXTEDIT.txt, is GUI text the
// player can type into, with its caret, length limit, allowed characters
// and the script command run on Enter. Placeholder names apart from the
// class name and properties.
#ifndef TONGAS2_GUITEXTEDIT_H
#define TONGAS2_GUITEXTEDIT_H

#include "Tongas2/GUITEXT.h"

class GUITEXTEDITMASTER : public GUITEXTMASTER
{
public:
    GUITEXTEDITMASTER(RESOURCEMANAGER *manager, const char *name);
    virtual ~GUITEXTEDITMASTER() {}

    static void RegisterCommands();

    virtual int GetClassId();
    virtual int GetParentClassId();
    virtual const char *GetClassName();
    virtual SRESULT Execute(CMDENTRY *command, TOKENS **args)
    {
        return (this->*(SRESULT (GUITEXTEDITMASTER::*)(TOKENS **))command->func)(args);
    }

    virtual void SetCaretChar(const int &i, const CWSTRING &caret) { m_caretChar[i] = caret; }
    virtual void SetCaretBlink(const int &i, const bool &blink) { m_caretBlink[i] = blink; }
    virtual void SetSeparators(const int &i, const CWSTRING &separators) { m_separators[i] = separators; }
    virtual void SetMaxLen(const int &i, const int &maxLen) { m_maxLen[i] = maxLen; }
    virtual void SetLimitChars(const int &i, const CWSTRING &chars) { m_limitChars[i] = chars; }
    virtual void SetOnEnterKey(const int &i, const CSTRING &command) { m_onEnterKey[i] = command; }
    virtual void SetDontAllowStartSpaces(const int &i, const bool &dont) { m_dontAllowStartSpaces[i] = dont; }

    static int s_classId;
    static CMDENTRY s_commands[];
    static PROPENTRY s_properties[];

    int m_9f0;
    CWSTRING m_caretChar[GUITEXT_MAX];
    bool m_caretBlink[GUITEXT_MAX];
    CWSTRING m_separators[GUITEXT_MAX];
    int m_maxLen[GUITEXT_MAX];
    CWSTRING m_limitChars[GUITEXT_MAX];
    CSTRING m_onEnterKey[GUITEXT_MAX];
    bool m_dontAllowStartSpaces[GUITEXT_MAX];
};

// The GUI text box control: GUI text the player types into while it has
// the input, showing each value through its template's separators mask
// with a blinking caret, and running the template's Enter command.
// Placeholder names apart from the class name, properties and commands.
class GUITEXTEDIT : public GUITEXT, public INPUTLISTENER
{
public:
    GUITEXTEDIT(const RESHANDLE &master, GUIGROUP *parent);
    virtual ~GUITEXTEDIT();

    static void RegisterCommands();

    virtual int GetClassId() { return s_classId; }
    virtual int GetParentClassId() { return g_classParents[s_classId]; }
    virtual const char *GetClassName() { return g_classNames[s_classId]; }
    virtual SRESULT Execute(CMDENTRY *command, TOKENS **args)
    {
        return (this->*(SRESULT (GUITEXTEDIT::*)(TOKENS **))command->func)(args);
    }
    virtual int Virtual19() { return 3; }
    // Blinks the caret.
    virtual void Update();
    // Shows the value through the separators mask, with the caret.
    virtual void SetText(const int &i, const FIXEDWSTRING<0x200> &text);
    // Binds the texts to the values once asked.
    virtual FIXEDSTRING<0x200> GetBindVar(int i);

    // The property's setter.
    virtual void SetValue(const int &i, const FIXEDWSTRING<0x200> &value) { m_values[i] = value; }
    // The script commands.
    virtual SRESULT DoneInput(CMDCALL *call);
    virtual SRESULT Length(CMDCALL *call);
    virtual SRESULT IsEmpty(CMDCALL *call);

    virtual bool OnKeyDown(unsigned int key);
    virtual bool OnChar(unsigned int c);

    // Types c into the value being edited, if it is allowed and fits.
    void AddChar(wchar_t c);
    // Types the clipboard's text.
    void Paste();

    static int s_classId;
    static CMDENTRY s_commands[];
    static PROPENTRY s_properties[];

    FIXEDWSTRING<0x200> m_values[GUITEXT_MAX];
    GUITEXTEDITMASTER *m_editMaster;
    // Once the texts show the values.
    bool m_bound;
    float m_blinkTime;
    bool m_caretOn;
    // The value being edited.
    int m_active;
};

#endif
