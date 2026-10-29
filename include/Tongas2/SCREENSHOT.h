// Screenshots: SCREENCAPTURE grabs a window's client area, or part of it,
// into a picture, scaled if asked, and saves it as JPEG or PNG. SCREENSHOT
// is the application's, with a table it never fills. Placeholder names.
#ifndef TONGAS2_SCREENSHOT_H
#define TONGAS2_SCREENSHOT_H

#include "Tongas2/DYNAMICARRAY.h"
#include "Tongas2/SRESULT.h"

struct HWND__;
struct FIBITMAP;

class SCREENCAPTURE
{
public:
    SCREENCAPTURE();
    virtual ~SCREENCAPTURE();

    // The part of the window around center, size across (all of it if size
    // is zero), scaled to outSize pixels unless that is zero. Fractions of
    // the client area.
    FIBITMAP *Grab(HWND__ *window, V2 center, V2 size, V2 outSize);
    // Format 0 saves a JPEG, 1 a PNG; flags are FreeImage's.
    int Save(const char *path, HWND__ *window, int format, int flags, V2 center, V2 size, V2 outSize);
};

class SCREENSHOT
{
public:
    SCREENSHOT();
    virtual ~SCREENSHOT();

    DYNAMICARRAY<int> m_frames;
    int m_2c;
};

#endif
