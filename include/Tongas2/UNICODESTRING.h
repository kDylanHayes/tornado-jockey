// The pool UNICODESTRING buffers come from: 0x1000 pages of 0x200 bytes,
// plus the "WC" encoding that stores wide text as 8-bit characters.
// Placeholder names apart from the class name, which its log messages give.
#ifndef TONGAS2_UNICODESTRING_H
#define TONGAS2_UNICODESTRING_H

#include "Tongas2/LOG.h"
#include "Tongas2/STRING.h"

// The bytes in a page.
#define PAGESIZE 0x200
#define NUMPAGES 0x1000

class UNICODESTRING_MANAGER
{
public:
    UNICODESTRING_MANAGER();
    virtual ~UNICODESTRING_MANAGER() {}

    // Fills s_decode and s_encode: A-Z, a-z, 0-9, '*' and '+' stand for the
    // 64 six-bit values.
    void InitTables();
    // The wide characters in text if it is "WC" and three characters for
    // each, otherwise -1.
    static int GetEncodedLength(const char *text);
    // Decodes n wide characters from "WC" text, and ends out.
    static void Decode(wchar_t *out, const char *in, int n);
    // Copies n 8-bit characters into wide ones, and ends out.
    static void Widen(wchar_t *out, const char *in, int n);
    // Copies n wide characters into 8-bit ones, '?' for those that don't
    // fit, and ends out.
    static void Narrow(char *out, const wchar_t *in, int n);
    // Writes "WC" and three characters for each of the n in in.
    static void Encode(STRING &out, const wchar_t *in, int n);

    // Moves the used pages down over the free ones.
    void Defrag();

    bool CheckIndex(int page)
    {
        if (page < 0 || page >= NUMPAGES) {
            if (g_pLog)
                g_pLog->Error("UNICODESTRING_MANAGER::CheckIndex>Error: number page %d is out of range", page);
            return false;
        }
        return true;
    }
    wchar_t *GetPointer(int page)
    {
        if (CheckIndex(page))
            // Steps in wide characters, so twice the page size.
            return (wchar_t *)m_pages + page * PAGESIZE;
        if (g_pLog)
            g_pLog->Error("UNICODESTRING_MANAGER::GetPointer>Error: Unable to return pointer to the page %d", page);
        return 0;
    }

    struct CACHE
    {
        CACHE() : m_index(-1), m_count(1) {}

        char m_data[0x4000];
        int m_index;
        int m_count;
    };

    static char s_decode[128];
    static char s_encode[64];

    CACHE m_cache[2];
    int m_8014;
    bool m_used[NUMPAGES];
    UNICODESTRING *m_owners[NUMPAGES];
    int m_d018;
    wchar_t m_pages[NUMPAGES][PAGESIZE / 2];
};

extern UNICODESTRING_MANAGER g_UnicodeStringManager;

#endif
