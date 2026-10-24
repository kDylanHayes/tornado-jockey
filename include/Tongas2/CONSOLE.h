// The script console: runs script commands and evaluates expressions.
// Defined elsewhere; only what other units need so far.
#ifndef TONGAS2_CONSOLE_H
#define TONGAS2_CONSOLE_H

#include "Tongas2/SAFEINDEX.h"
#include "Tongas2/SRESULT.h"
#include "Tongas2/STRING.h"

class CONSOLE
{
public:
    // Runs command on behalf of the resource with the handle.
    void Run(const STRING &command, const RESHANDLE &caller);
    // The value of a script expression, for the resource with the handle.
    SRESULT Evaluate(const STRING &expression, const RESHANDLE &caller);
};

extern CONSOLE *g_pConsole;

#endif
