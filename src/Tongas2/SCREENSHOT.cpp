// Screenshots, grabbed from the window with GDI and saved with FreeImage.
// Placeholder file name.
#include "lib/freeimage/FreeImage.h"

#include <windows.h>

#include "Tongas2/LOG.h"
#include "Tongas2/SCREENSHOT.h"

SCREENCAPTURE::SCREENCAPTURE()
{
}

SCREENCAPTURE::~SCREENCAPTURE()
{
}

FIBITMAP *SCREENCAPTURE::Grab(HWND window, V2 center, V2 size, V2 outSize)
{
    HDC dc = GetDC(window);
    HDC memory = CreateCompatibleDC(dc);
    RECT rect;
    GetClientRect(window, &rect);
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    int x, y;
    if (size.x == 0.0f && size.y == 0.0f) {
        x = 0;
        y = 0;
    } else {
        float fullWidth = (float)width;
        x = (int)((center.x - size.x * 0.5f) * fullWidth);
        float fullHeight = (float)height;
        y = (int)((center.y - size.y * 0.5f) * fullHeight);
        width = (int)(fullWidth * size.x);
        height = (int)(fullHeight * size.y);
    }
    int outWidth, outHeight;
    if (outSize.x == 0.0f && outSize.y == 0.0f) {
        outWidth = width;
        outHeight = height;
    } else {
        outWidth = (int)outSize.x;
        outHeight = (int)outSize.y;
    }
    HBITMAP bitmap = CreateCompatibleBitmap(dc, width, height);
    HGDIOBJ old = SelectObject(memory, bitmap);
    BitBlt(memory, 0, 0, width, height, dc, x, y, SRCCOPY);
    BITMAP info;
    GetObjectA(bitmap, sizeof(info), &info);
    FIBITMAP *dib = FreeImage_Allocate(info.bmWidth, info.bmHeight, info.bmBitsPixel);
    HDC screen = GetDC(0);
    if (!GetDIBits(screen, bitmap, 0, FreeImage_GetHeight(dib), FreeImage_GetBits(dib),
                   (BITMAPINFO *)FreeImage_GetInfoHeader(dib), DIB_RGB_COLORS)) {
        if (g_pLog)
            g_pLog->Error("screenshot - could not GetDIBits!!");
        return 0;
    }
    if (outWidth != width || outHeight != height) {
        FIBITMAP *previous;
        if (FreeImage_GetBPP(dib) != 32) {
            previous = dib;
            dib = FreeImage_ConvertTo32Bits(previous);
            FreeImage_Unload(previous);
        }
        previous = dib;
        dib = FreeImage_Rescale(previous, outWidth, outHeight, FILTER_BILINEAR);
        if (!dib) {
            if (g_pLog)
                g_pLog->Error("COULD NOT RESCALE SCREENSHOT to %d %d (from %d %d)!!!", outWidth, outHeight, width, height);
            dib = previous;
        } else {
            FreeImage_Unload(previous);
        }
    }
    ReleaseDC(0, screen);
    SelectObject(memory, old);
    DeleteObject(bitmap);
    DeleteDC(memory);
    ReleaseDC(window, dc);
    return dib;
}

int SCREENCAPTURE::Save(const char *path, HWND window, int format, int flags, V2 center, V2 size, V2 outSize)
{
    FIBITMAP *dib = Grab(window, center, size, outSize);
    if (!dib)
        return false;
    bool saved;
    if (format == 0) {
        if (FreeImage_GetBPP(dib) != 24) {
            FIBITMAP *previous = dib;
            dib = FreeImage_ConvertTo24Bits(previous);
            FreeImage_Unload(previous);
        }
        // The game's FreeImage registers only JPEG and PNG, in that order.
        saved = FreeImage_Save((FREE_IMAGE_FORMAT)0, dib, path, flags) != 0;
    } else if (format == 1) {
        if (FreeImage_GetBPP(dib) != 32) {
            FIBITMAP *previous = dib;
            dib = FreeImage_ConvertTo32Bits(previous);
            FreeImage_Unload(previous);
        }
        saved = FreeImage_Save((FREE_IMAGE_FORMAT)1, dib, path, flags) != 0;
    } else {
        if (g_pLog)
            g_pLog->Error("UNKONWN FORMAT FOR SCREENSHOT!");
        saved = false;
    }
    if (!saved && g_pLog)
        g_pLog->Error("COULD NOT SAVE SCREENSHOT!");
    FreeImage_Unload(dib);
    return saved;
}

SCREENSHOT::SCREENSHOT()
{
    m_2c = 0;
}

SCREENSHOT::~SCREENSHOT()
{
}
