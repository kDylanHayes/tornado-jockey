// Screen decals: SCREENDECALMASTER, from SCREENDECAL.txt, names a picture
// under data/art/ drawn flat on the screen, with its blend mode and scroll
// speed; each SCREENDECAL is one drawn copy, placed in screen coordinates.
// Placeholder names apart from the class name and properties.
#ifndef TONGAS2_SCREENDECAL_H
#define TONGAS2_SCREENDECAL_H

#include "Tongas2/CSTRING.h"
#include "Tongas2/RESOURCE.h"
#include "Tongas2/RESOURCES.h"
#include "Tongas2/SRESULT.h"

// A 2D position or size that starts at zero.
struct SCREENDECALV2 : V2
{
    SCREENDECALV2() : V2(0.0f, 0.0f) {}
    SCREENDECALV2(float x_, float y_) : V2(x_, y_) {}
    SCREENDECALV2 &operator=(const V2 &v) { V2::operator=(v); return *this; }
};

class SCREENDECALMASTER : public RESOURCEMASTER
{
public:
    SCREENDECALMASTER(RESOURCEMANAGER *manager, const char *name);
    virtual ~SCREENDECALMASTER() {}

    static void RegisterCommands();

    virtual int GetClassId();
    virtual int GetParentClassId();
    virtual const char *GetClassName();
    virtual SRESULT Execute(CMDENTRY *command, TOKENS **args)
    {
        return (this->*(SRESULT (SCREENDECALMASTER::*)(TOKENS **))command->func)(args);
    }
    virtual RESOURCE *CreateCopy(const RESHANDLE &handle);
    // Works out the picture's path under data/art/.
    virtual void Prepare();

    // Also names the master after the file.
    virtual void SetFileName(const CSTRING &file)
    {
        if (file.Length() > 0)
            SetName(file);
        m_file = file;
    }
    virtual void SetForceDuplicates(const bool &force) { m_forceDuplicates = force; }
    virtual void SetShifterSpeed(const V2 &speed);
    virtual void SetBlend(const int &blend) { m_blend = blend; }
    virtual void SetUserFolder(const bool &user) { m_userFolder = user; }
    virtual void SetCorrectSizeInWidescreen(const bool &correct) { m_correctSizeInWidescreen = correct; }

    static int s_classId;
    static CMDENTRY s_commands[];
    static PROPENTRY s_properties[];

    CSTRING m_file;
    bool m_forceDuplicates;
    SCREENDECALV2 m_shifterSpeed;
    int m_blend;
    bool m_userFolder;
    bool m_correctSizeInWidescreen;
    CSTRING m_artPath;
};

class SCREENDECAL : public OBJECT
{
public:
    SCREENDECAL(SCREENDECALMASTER *master, const RESHANDLE &handle);
    virtual ~SCREENDECAL() {}

    virtual bool IsLoaded() { return true; }
    virtual V3 GetScaleFactor() { return V3(1.0f, 1.0f, 1.0f); }
    // A world position, which stops the decal using its screen position.
    virtual void SetPosition(V3 position);
    virtual void SetLayer(int layer) {}

    void SetScreenPosition(V2 position);
    V2 GetScreenPosition();
    void SetScreenSize(const V2 &size);

    bool m_400;
    SCREENDECALV2 m_screenPosition;
    SCREENDECALV2 m_screenSize;
    bool m_onScreen;
};

#endif
