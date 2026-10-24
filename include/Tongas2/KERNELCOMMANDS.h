// The script commands that belong to no object. Defined elsewhere; only
// what other units need so far.
#ifndef TONGAS2_KERNELCOMMANDS_H
#define TONGAS2_KERNELCOMMANDS_H

#include "Tongas2/CMDBASE.h"

class KERNELCOMMANDS : public CMDBASE
{
public:
    static void RegisterCommands();

    static int s_classId;
    static PROPENTRY s_properties[];
};

#endif
