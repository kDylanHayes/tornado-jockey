// The copy protection, at 0x6d09d8. Defined elsewhere; only what other units
// need so far.
#ifndef TONGAS2_DRM_H
#define TONGAS2_DRM_H

#include "Tongas2/CMDBASE.h"

class DRM : public CMDBASE
{
public:
    static void RegisterCommands();

    virtual void Init();
    // Runs once a frame.
    virtual void Update();
    virtual void Virtual21();
    virtual void Virtual22();

    static int s_classId;
    static PROPENTRY s_properties[];

    // The instance APP::Init was given.
    void *m_instance;
};

extern DRM *g_pDrm;

// Makes the copy protection.
extern DRM *(*g_pfnCreateDrm)();

#endif
