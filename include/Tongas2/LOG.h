// Engine log (log.txt / log.html). Only what other units need so far.
#ifndef TONGAS2_LOG_H
#define TONGAS2_LOG_H

class LOG
{
public:
    // Writes "ERROR: <message>" and counts it.
    void Error(const char *format, ...);
    // Compiled out of the release build, though its arguments are still
    // evaluated. Placeholder: only seen through those arguments.
    void Debug(const char *format, float x, float y, float z) {}
    // Compiled out of the release build too.
    void Trace(const char *text) {}
};

extern LOG *g_pLog;

#endif
