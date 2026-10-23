#include "Tongas2/LOG.h"
#include "Tongas2/LOGSTREAM.h"

#include <stdarg.h>
#include <stdlib.h>
#include <time.h>

static const char *s_closeTags[] = {
    "",
    "</b></font>",
    "</b></font>",
    "</b></font>",
    "</b></font>",
    "</b></font>",
};

static const char *s_openTags[] = {
    "",
    "<font color='brown'><b>",
    "<font color='red'><b>",
    "<font color='#bb00bb'><b>",
    "<font color='#00d040'><b>",
    "<font color='#0070d0'><b>",
};

static const WORD s_consoleColors[] = { 7, 14, 12, 13, 10, 9 };

static LOG *CreateLog();

LOG *(*g_pfnCreateLog)() = CreateLog;

// Adds text to the end of a file, or starts it afresh with mode "wt".
static inline void WriteToFile(const char *path, const char *mode, const char *text)
{
    FILE *file = fopen(path, mode);
    if (file) {
        fprintf(file, "%s", text);
        fclose(file);
    }
}

void LOG::Toggle()
{
    m_enabled = !m_enabled;
}

void LOG::Error(const char *format, ...)
{
    SetColor(LOGCOLOR_RED);
    va_list args;
    va_start(args, format);
    if (_vsnprintf(m_message, sizeof(m_message) - 1, format, args) == -1)
        m_message[sizeof(m_message) - 1] = 0;
    _snprintf(m_text, sizeof(m_text) - 1, "%s%s", "ERROR: ", m_message);
    Write(false);
    m_errors++;
}

void LOG::ContentError(const char *format, ...)
{
    SetColor(LOGCOLOR_PURPLE);
    va_list args;
    va_start(args, format);
    if (_vsnprintf(m_message, sizeof(m_message) - 1, format, args) == -1)
        m_message[sizeof(m_message) - 1] = 0;
    _snprintf(m_text, sizeof(m_text) - 1, "%s%s", "CONTENT ERROR: ", m_message);
    Write(false);
    m_contentErrors++;
}

void LOG::SetColor(int color)
{
    if (color == m_color)
        return;
    WriteToFile(m_htmlPath, "at", s_closeTags[m_color]);
    m_color = color;
    WriteToFile(m_htmlPath, "at", s_openTags[color]);
    if (m_console) {
        m_console->flush();
        CONSOLE_SCREEN_BUFFER_INFO info;
        GetConsoleScreenBufferInfo(m_console->m_handle, &info);
        info.wAttributes = s_consoleColors[color];
        SetConsoleTextAttribute(m_console->m_handle, info.wAttributes);
    }
}

void LOG::Write(bool create)
{
    if (create) {
        if (m_textFile)
            fclose(m_textFile);
        m_textFile = fopen(m_textPath, "wt");
        if (m_textFile)
            fprintf(m_textFile, "%s<br>", m_text);
        WriteToFile(m_htmlPath, "wt", m_text);
        WriteToFile(m_htmlPath, "wt", "<br>");
    } else {
        if (m_textFile)
            fprintf(m_textFile, "%s\n", m_text);
        WriteToFile(m_htmlPath, "at", m_text);
        WriteToFile(m_htmlPath, "at", "<br>");
    }
    if (m_console)
        *m_console << m_text << "\n";
}

LOG::LOG()
{
    _tzset();
    m_console = 0;
    const char *temp = getenv("Temp");
    if (!temp)
        temp = getenv("Tmp");
    if (temp) {
        _snprintf(m_textPath, sizeof(m_textPath) - 2, "%s\\%s", temp, "log.txt");
        _snprintf(m_htmlPath, sizeof(m_htmlPath) - 2, "%s\\%s", temp, "log.html");
    } else {
        _snprintf(m_textPath, sizeof(m_textPath) - 2, "c:\\%s", "log.txt");
        _snprintf(m_htmlPath, sizeof(m_htmlPath) - 2, "c:\\%s", "log.html");
    }
    m_color = LOGCOLOR_DEFAULT;
    m_textFile = 0;
    _tzset();
    m_text[0] = 0;
    Write(true);
    WriteToFile(m_htmlPath, "at", "<html><body>");
    char buffer[128];
    _strtime(buffer);
    sprintf(m_text, "LOG: time:%s", buffer);
    Write(false);
    _strdate(buffer);
    sprintf(m_text, "LOG: date:%s", buffer);
    Write(false);
    sprintf(m_text, "************************************* Log Activated");
    Write(false);
    m_errors = 0;
    m_contentErrors = 0;
    m_enabled = true;
}

LOG::~LOG()
{
    SetColor(LOGCOLOR_DEFAULT);
    sprintf(m_text, "************************************* Log Deactivated");
    Write(false);
    char buffer[128];
    _strtime(buffer);
    sprintf(m_text, "LOG: time:%s", buffer);
    Write(false);
    sprintf(m_text, "LOG: founded:%d error", m_errors);
    Write(false);
    sprintf(m_text, "LOG: founded:%d content error", m_contentErrors);
    Write(false);
    WriteToFile(m_htmlPath, "at", "</body></html>");
    if (m_textFile)
        fclose(m_textFile);
    if (m_console) {
        m_console->Close();
        if (m_console) {
            delete m_console;
            m_console = 0;
        }
    }
}

static LOG *CreateLog()
{
    return new LOG;
}
