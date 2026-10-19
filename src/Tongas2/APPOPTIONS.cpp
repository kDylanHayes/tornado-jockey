#include <stdio.h>
#include <string>

#include "Tongas2/APPOPTIONS.h"
#include "Tongas2/LOG.h"

// Elsewhere in the engine. CSaveFile folders (0 = the game's own).
std::string GetSaveFolder(int type);
void CreateFolder(const std::string &path);

static const DISPLAYMODE s_resolutions[APPOPTIONS_RESOLUTIONS] = {
    {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {640, 480}, {800, 600}, {1024, 768}, {1280, 960},
    {1024, 640}, {1152, 720}, {1280, 800}, {1440, 900}, {1680, 1050}, {1920, 1200},
};

static const DISPLAYMODE s_windowedResolutions[APPOPTIONS_RESOLUTIONS] = {
    {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {600, 450}, {760, 570}, {984, 738}, {1240, 930},
    {1024, 640}, {1152, 720}, {1280, 800}, {1440, 900}, {1680, 1050}, {1920, 1200},
};

APPOPTIONS::APPOPTIONS()
{
    m_music = true;
    m_sound = true;
    m_resolution = APPOPTIONS_DEFAULT_RESOLUTION;
    m_fullscreen = true;
    m_detail = 1;
    m_path[0] = 0;
    m_version = 0;
}

void APPOPTIONS::Apply()
{
    g_pApp->SetFullscreen(m_fullscreen);
    g_pApp->SetSound(m_sound);
    g_pApp->SetMusic(m_music);
    g_pApp->SetResolution(m_resolution);
    g_pApp->SetDetail(m_detail);
}

void APPOPTIONS::SetDefaults()
{
    m_resolution = APPOPTIONS_DEFAULT_RESOLUTION;
    m_detail = 1;
}

void APPOPTIONS::Read(FILE *fp)
{
    fread(&m_music, 1, sizeof(m_music), fp);
    fread(&m_sound, 1, sizeof(m_sound), fp);
    fread(&m_resolution, 1, sizeof(m_resolution), fp);
    fread(&m_fullscreen, 1, sizeof(m_fullscreen), fp);
    fread(&m_detail, 1, sizeof(m_detail), fp);
}

void APPOPTIONS::Write(FILE *fp)
{
    fwrite(&m_music, 1, sizeof(m_music), fp);
    fwrite(&m_sound, 1, sizeof(m_sound), fp);
    fwrite(&m_resolution, 1, sizeof(m_resolution), fp);
    fwrite(&m_fullscreen, 1, sizeof(m_fullscreen), fp);
    fwrite(&m_detail, 1, sizeof(m_detail), fp);
}

RESOLUTION APPOPTIONS::GetResolution(const int &index)
{
    if (index >= APPOPTIONS_RESOLUTIONS)
        return s_resolutions[APPOPTIONS_DEFAULT_RESOLUTION];
    return s_resolutions[index];
}

RESOLUTION APPOPTIONS::GetWindowedResolution(const int &index)
{
    if (index >= APPOPTIONS_RESOLUTIONS)
        return s_windowedResolutions[APPOPTIONS_DEFAULT_RESOLUTION];
    return s_windowedResolutions[index];
}

APPOPTIONS *CreateAppOptions()
{
    return new APPOPTIONS;
}

void APPOPTIONS::Load()
{
    bool loaded = false;
    SetDefaults();
    m_path[0] = 0;
    std::string folder = GetSaveFolder(0) + "/";
    std::string name = "optionswtj.dat";
    sprintf(m_path, "%s%s", folder.c_str(), name.c_str());
    FILE *fp = fopen(m_path, "rb");
    if (fp) {
        int version;
        fread(&version, 1, sizeof(version), fp);
        if (version == m_version) {
            Read(fp);
            loaded = true;
        }
        else if (g_pLog)
            g_pLog->Error("Error - APPOPTIONS::Load> Wrong version");
        fclose(fp);
    }
    else if (g_pLog)
        g_pLog->Error("could not find options file");
    if (!loaded)
        OnLoadFailed();
}

void APPOPTIONS::Save()
{
    std::string folder = GetSaveFolder(0) + "/";
    CreateFolder(folder);
    FILE *fp = fopen(m_path, "wb");
    if (fp) {
        int version = m_version;
        fwrite(&version, 1, sizeof(version), fp);
        Write(fp);
        fclose(fp);
    }
}
