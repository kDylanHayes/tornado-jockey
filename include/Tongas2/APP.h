// The application object. Defined elsewhere in the engine; only what other
// units need so far.
#ifndef TONGAS2_APP_H
#define TONGAS2_APP_H

class APPOPTIONS;

class APP
{
public:
    void SetResolution(int index);
    void SetFullscreen(bool on);
    void SetSound(bool on);
    void SetMusic(bool on);
    void SetDetail(int level);

    int m_0[4];
    APPOPTIONS *m_pOptions;
};

extern APP *g_pApp;

#endif
