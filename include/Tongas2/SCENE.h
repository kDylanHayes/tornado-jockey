// The scene scripts set the fog and lights of. Defined elsewhere; only
// what CMDMGR needs so far.
#ifndef TONGAS2_SCENE_H
#define TONGAS2_SCENE_H

#include "Tongas2/CMDBASE.h"

class SCENE
{
public:
    static void RegisterCommands();

    static int s_classId;
    static PROPENTRY s_properties[];
};

#endif
