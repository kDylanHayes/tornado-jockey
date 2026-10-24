// Saves screenshots. Placeholder name; defined elsewhere.
#ifndef TONGAS2_SCREENSHOT_H
#define TONGAS2_SCREENSHOT_H

#include "Tongas2/SRESULT.h"

struct HWND__;

// Grabs a window into a picture file. Placeholder names; defined elsewhere.
class SCREENCAPTURE
{
public:
    SCREENCAPTURE();
    virtual ~SCREENCAPTURE();

    void Save(const char *path, HWND__ *window, int a, int b, V2 c, V2 d, V2 e);
};

class SCREENSHOT
{
public:
    SCREENSHOT();
    virtual ~SCREENSHOT();

    char m_4[0x2c];
};

#endif
