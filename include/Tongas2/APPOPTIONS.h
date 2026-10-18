// The player's display and sound settings, saved as optionswtj.dat in the
// WildTangent save folder.
#ifndef TONGAS2_APPOPTIONS_H
#define TONGAS2_APPOPTIONS_H

#include <stdio.h>
#include <string>

struct DISPLAYMODE
{
    int width, height;
};

// Built from a DISPLAYMODE the way ATL's CSize is built from a SIZE (a
// whole-struct copy). Having a constructor, it's returned through a hidden
// pointer, while the mode tables stay plain constant data.
struct RESOLUTION : public DISPLAYMODE
{
    RESOLUTION(const DISPLAYMODE &mode) { *static_cast<DISPLAYMODE *>(this) = mode; }
};

// Index of the 800x600 modes, used for an index out of range.
#define APPOPTIONS_DEFAULT_RESOLUTION 5
#define APPOPTIONS_RESOLUTIONS 14

class APPOPTIONS
{
public:
    APPOPTIONS();

    virtual const char *GetFileName() { return "options.dat"; }
    virtual ~APPOPTIONS() {}
    virtual void Read(FILE *fp);
    virtual void Write(FILE *fp);
    virtual void Apply();
    virtual void OnLoadFailed() {}
    virtual void SetDefaults();
    virtual RESOLUTION GetResolution(const int &index);
    virtual RESOLUTION GetWindowedResolution(const int &index);

    void Load();
    void Save();

    char m_path[0x104];
    bool m_music;
    bool m_sound;
    int m_resolution;
    bool m_fullscreen;
    int m_detail;
    int m_version;
};

APPOPTIONS *CreateAppOptions();

// Elsewhere in the engine; only what APPOPTIONS needs.

class APP
{
public:
    void SetResolution(int index);
    void SetFullscreen(bool on);
    void SetSound(bool on);
    void SetMusic(bool on);
    void SetDetail(int level);
};

extern APP *g_pApp;

// CSaveFile folders (0 = the game's own).
std::string GetSaveFolder(int type);
void CreateFolder(const std::string &path);

#endif
