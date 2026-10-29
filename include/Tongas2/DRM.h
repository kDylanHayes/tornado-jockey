// The base of the copy protection at 0x6d09d8, which the game provides: the
// IsUnlocked command, and the decoding of its hidden strings. Placeholder
// names apart from the class name and the names its commands give.
#ifndef TONGAS2_DRM_H
#define TONGAS2_DRM_H

#include "Tongas2/CMDBASE.h"

extern char g_classNames[][30];

class DRM : public CMDBASE
{
public:
    DRM();

    static void RegisterCommands();

    virtual int GetClassId() { return s_classId; }
    virtual int GetParentClassId() { return g_classParents[s_classId]; }
    virtual const char *GetClassName() { return g_classNames[s_classId]; }
    virtual SRESULT Execute(CMDENTRY *command, TOKENS **args)
    {
        return (this->*(SRESULT (DRM::*)(TOKENS **))command->func)(args);
    }
    // Defined elsewhere.
    virtual int Virtual08();
    virtual void *Virtual09(const char *name);
    virtual void Init() {}
    // Runs once a frame.
    virtual void Update() {}
    virtual void Virtual21() {}
    virtual void Virtual22() {}
    virtual void Virtual23() {}
    // The IsUnlocked command.
    virtual SRESULT IsUnlocked(CMDCALL *call) { return SRESULT(true); }
    virtual bool Virtual25() { return true; }

    // XORs length bytes of data with the key, repeated, into out.
    void Decode(const char *data, const char *key, int length, int keyLength, char *out);

    static int s_classId;
    static CMDENTRY s_commands[];
    static PROPENTRY s_properties[];

    // The instance APP::Init was given.
    void *m_instance;
};

extern DRM *g_pDrm;

// Makes the copy protection.
extern DRM *(*g_pfnCreateDrm)();

#endif
