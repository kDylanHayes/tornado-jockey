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

#endif
