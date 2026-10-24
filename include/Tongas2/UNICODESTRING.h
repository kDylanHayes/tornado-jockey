// The pool UNICODESTRING buffers come from: 0x1000 pages of 0x200 bytes,
// plus the "WC" encoding that stores wide text as 8-bit characters.
// Placeholder names apart from the class name, which its log messages give.
#ifndef TONGAS2_UNICODESTRING_H
#define TONGAS2_UNICODESTRING_H

#include <stdarg.h>

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
    wchar_t *GetPointer(const int &page)
    {
        if (CheckIndex(page))
            // Steps in wide characters, so twice the page size.
            return (wchar_t *)m_pages + page * PAGESIZE;
        if (g_pLog)
            g_pLog->Error("UNICODESTRING_MANAGER::GetPointer>Error: Unable to return pointer to the page %d", page);
        return 0;
    }

    // A stack of pages that were freed, to hand out again first.
    struct CACHE
    {
        enum
        {
            SOME,
            EMPTY,
            FULL
        };

        CACHE() : m_index(-1), m_state(EMPTY) {}

        bool IsEmpty() const { return m_state == EMPTY; }
        int Pop()
        {
            int index = m_index;
            if (index == 0)
                m_state = EMPTY;
            int page = m_pages[index];
            m_index = index - 1;
            return page;
        }
        void Push(int page)
        {
            m_state = SOME;
            if (m_index == NUMPAGES - 2)
                m_state = FULL;
            m_pages[++m_index] = page;
        }

        int m_pages[NUMPAGES];
        int m_index;
        int m_state;
    };

    // Takes count free pages in a row for owner and gives the first, or
    // NUMPAGES if there aren't enough even after a Defrag. Single pages and
    // pairs come from the caches first.
    int FindAndAllocatePages(const int &count, UNICODESTRING *owner)
    {
        while (count <= NUMPAGES - m_usedPagesCount) {
            if (count == 1) {
                while (!m_cache[0].IsEmpty()) {
                    int page = m_cache[0].Pop();
                    if (!m_used[page]) {
                        m_used[page] = true;
                        m_owners[page] = owner;
                        m_usedPagesCount++;
                        return page;
                    }
                }
                // Each pair gives one page and leaves the other cached.
                while (!m_cache[1].IsEmpty()) {
                    int page = m_cache[1].Pop();
                    m_cache[0].Push(page + 1);
                    if (!m_used[page]) {
                        m_used[page] = true;
                        m_owners[page] = owner;
                        m_usedPagesCount++;
                        return page;
                    }
                }
            }
            if (count == 2) {
                while (!m_cache[1].IsEmpty()) {
                    int page = m_cache[1].Pop();
                    if (!m_used[page] && !m_used[page + 1]) {
                        m_used[page] = true;
                        m_used[page + 1] = true;
                        m_owners[page] = owner;
                        m_owners[page + 1] = owner;
                        m_usedPagesCount += 2;
                        return page;
                    }
                }
            }
            // Pages never used yet.
            if (m_next + count < NUMPAGES) {
                int page = m_next;
                for (int i = 0; i < count; i++) {
                    m_used[page + i] = true;
                    m_owners[page + i] = owner;
                }
                m_next += count;
                m_usedPagesCount += count;
                return page;
            }
            int run = 0;
            for (int i = 0; i < NUMPAGES; i++) {
                if (!m_used[i]) {
                    run++;
                    if (run == count) {
                        int page = i - run + 1;
                        for (int j = 0; j < count; j++) {
                            m_used[page + j] = true;
                            m_owners[page + j] = owner;
                        }
                        m_usedPagesCount += count;
                        return page;
                    }
                } else
                    run = 0;
            }
            if (g_pLog)
                g_pLog->Error("UNICODESTRING_MANAGER::FindAndAllocatePages>Error: There is no space to allocate %d pages, num pages: %d, m_usedPagesCount: %d", count, NUMPAGES, m_usedPagesCount);
            Defrag();
        }
        return NUMPAGES;
    }

    static char s_decode[128];
    static char s_encode[64];

    CACHE m_cache[2];
    // The first page never used.
    int m_next;
    bool m_used[NUMPAGES];
    UNICODESTRING *m_owners[NUMPAGES];
    int m_usedPagesCount;
    wchar_t m_pages[NUMPAGES][PAGESIZE / 2];
};

extern UNICODESTRING_MANAGER g_UnicodeStringManager;

// STRING's, here because it logs.
inline void STRING::Format(const char *format, ...)
{
    char text[0x4000];
    va_list args;
    va_start(args, format);
    if (_vsnprintf(text, 0x3fff, format, args) == -1) {
        text[0x3fff] = 0;
        if (g_pLog)
            g_pLog->Error("ERROR!! - Tried to write more than %d chars to a string, it was truncated!", 0x4000);
    }
    *this = text;
}

inline void UNICODESTRING::Reserve(const int &size)
{
    int pages = size / 256 + 1;
    if (pages > m_pages) {
        UNICODESTRING old;
        if (m_pages > 0) {
            if (Length() > 0)
                old = m_buf;
            ((STRINGMANAGER *)&g_UnicodeStringManager)->Free(m_block, m_pages);
        }
        m_pages = pages;
        m_block = g_UnicodeStringManager.FindAndAllocatePages(m_pages, this);
        m_buf = g_UnicodeStringManager.GetPointer(m_block);
        if (old.Length() > 0)
            wcscpy(m_buf, old.c_str());
        else
            m_buf[0] = 0;
    }
}

inline void UNICODESTRING::operator=(const wchar_t *text)
{
    if (!text) {
        Free();
        return;
    }
    int size = wcslen(text);
    if (size > 0) {
        size++;
        Reserve(size);
        if (m_block >= 0) {
            wcscpy(m_buf, text);
            m_lengthValid = false;
            return;
        }
    }
    Free();
}

inline void UNICODESTRING::operator=(const UNICODESTRING &other)
{
    int length = const_cast<UNICODESTRING &>(other).Length();
    Reserve(length);
    *this = other.c_str();
}

#endif
