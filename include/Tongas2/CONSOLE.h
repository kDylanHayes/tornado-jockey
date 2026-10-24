// The debug console, opened with F11: the last 15 commands typed, the line
// being typed and a line of frame statistics, drawn as screen text, and the
// debug hot keys when it's closed. Placeholder names.
#ifndef TONGAS2_CONSOLE_H
#define TONGAS2_CONSOLE_H

#include "Tongas2/FIXEDSTRING.h"
#include "Tongas2/GUI.h"
#include "Tongas2/SAFEINDEX.h"

#define CONSOLE_LINES 15

class CONSOLE : public GUIKEYLISTENER
{
public:
    CONSOLE();
    virtual ~CONSOLE() {}

    virtual bool OnKeyDown(unsigned int key);
    virtual bool OnChar(unsigned int c);
    // The keys that work with the console closed; true if it was one.
    virtual bool OnHotKey(const int &key);
    // The font, size and line spacing of the console's text.
    virtual const char *GetFont() { return "TEXT:whitefont"; }
    virtual V2 GetScale() { return V2(0.02f, 0.02f); }
    virtual float GetLineHeight() { return 0.025f; }
    // Whether F11 opens it.
    virtual bool CanOpen() { return true; }

    // Lets it run commands.
    void Enable();
    // Opens or closes it.
    void Show(bool open);
    // Puts a line from the history in the line being typed.
    void SelectHistory(int index);
    // Draws it.
    void Update();
    // Runs the line being typed and keeps it in the history.
    void Run(const char *format, ...);

    RESHANDLE m_lines[CONSOLE_LINES];
    RESHANDLE m_prompt;
    RESHANDLE m_stats;
    bool m_open;
    bool m_d1;
    char m_text[0x200];
    // The line being typed.
    FIXEDSTRING<0x200> m_input;
    bool m_enabled;
    char m_history[CONSOLE_LINES][0x200];
    // The oldest line of the history, the one the next goes in, and the one
    // shown with the arrow keys (-1 for none).
    char m_first;
    char m_next;
    char m_browse;
    // While above 0, keys go past it.
    int m_22d8;
};

extern CONSOLE *g_pConsole;

// Makes the console.
extern CONSOLE *(*g_pfnCreateConsole)();

#endif
