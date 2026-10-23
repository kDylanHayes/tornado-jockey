// Engine log. Every message goes to log.txt and, colored by kind, to
// log.html in the temp folder, and to the debug console when one is open.
// Placeholder names apart from the class name.
#ifndef TONGAS2_LOG_H
#define TONGAS2_LOG_H

#include <stdio.h>

class LOGSTREAM;

// The colors messages are written in. Errors are red and content errors
// purple.
enum LOGCOLOR
{
    LOGCOLOR_DEFAULT,
    LOGCOLOR_BROWN,
    LOGCOLOR_RED,
    LOGCOLOR_PURPLE,
    LOGCOLOR_GREEN,
    LOGCOLOR_BLUE,
};

class LOG
{
public:
    LOG();
    // Reached only through another class's identical function.
    virtual bool Virtual00() { return false; }
    virtual ~LOG();
    // Switches the color later messages are written in.
    virtual void SetColor(int color);
    // Writes m_text; create starts the files afresh.
    virtual void Write(bool create);

    void Toggle();
    // Writes "ERROR: <message>" and counts it.
    void Error(const char *format, ...);
    // Writes "CONTENT ERROR: <message>", for mistakes in the game's data,
    // and counts it.
    void ContentError(const char *format, ...);
    // Compiled out of the release build, though its arguments are still
    // evaluated. Placeholder: only seen through those arguments.
    void Debug(const char *format, float x, float y, float z) {}
    // Compiled out of the release build too.
    void Trace(const char *text) {}

    char m_text[0x200];
    char m_message[0x200];
    char m_textPath[0x104];
    char m_htmlPath[0x104];
    int m_errors;
    int m_contentErrors;
    int m_color;
    FILE *m_textFile;
    bool m_enabled;
    // The debug console, when one is open.
    LOGSTREAM *m_console;
};

extern LOG *g_pLog;
// Makes the log.
extern LOG *(*g_pfnCreateLog)();

#endif
