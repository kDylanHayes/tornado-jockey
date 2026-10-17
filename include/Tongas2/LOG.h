// Engine log (log.txt / log.html). Only what other units need so far.
#ifndef TONGAS2_LOG_H
#define TONGAS2_LOG_H

class LOG
{
public:
    // Writes "ERROR: <message>" and counts it.
    void Error(const char *format, ...);
};

extern LOG *g_pLog;

#endif
