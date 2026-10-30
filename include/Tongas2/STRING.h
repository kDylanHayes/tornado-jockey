// Engine strings. Buffers come from a pooled allocator in 256-byte pages.
// Placeholder names; only what other units need so far.
#ifndef TONGAS2_STRING_H
#define TONGAS2_STRING_H

#include <string.h>
#include <wchar.h>

#include "Tongas2/FIXEDSTRING.h"

class STRING;

// The pool STRING buffers come from: 0x1000 pages of 0x100 bytes, laid out
// like UNICODESTRING_MANAGER's (UNICODESTRING.h). Placeholder names apart
// from the class name, which its log messages give.
class STRINGMANAGER
{
public:
    // Returns a buffer to the pool and updates the usage counters.
    // CMDMGR was built seeing it as not throwing; it defines
    // STRINGMANAGER_FREE_NOTHROW (see toolchain.md).
#ifdef STRINGMANAGER_FREE_NOTHROW
    void Free(int &block, int &pages) throw();
#else
    void Free(int &block, int &pages);
#endif
    // Moves the used pages down over the free ones.
    void Defrag();

    // Only STRINGMANAGER.cpp sees the layout: FX's code comes out
    // different when it sees the destructor and the members too.
#ifdef STRINGMANAGER_LAYOUT
    STRINGMANAGER();
    virtual ~STRINGMANAGER() {}

    // Defined in STRINGMANAGER.cpp.
    bool CheckIndex(int page);
    char *GetPointer(const int &page);

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

        int m_pages[0x1000];
        int m_index;
        int m_state;
    };

    CACHE m_cache[2];
    // The first page never used.
    int m_next;
    bool m_used[0x1000];
    STRING *m_owners[0x1000];
    int m_usedPagesCount;
    char m_pages[0x1000][0x100];
#endif
};

extern STRINGMANAGER g_StringManager;

// The pool UNICODESTRING buffers come from (UNICODESTRING.h). Its Free is
// the same code as STRINGMANAGER's, and the linker kept that one.
class UNICODESTRING_MANAGER;
extern UNICODESTRING_MANAGER g_UnicodeStringManager;

class STRING
{
public:
    STRING()
    {
        m_block = -1;
        m_pages = 0;
        m_buf = 0;
        m_length = 0;
        m_lengthValid = true;
    }
    STRING(const char *text)
    {
        m_block = -1;
        m_pages = 0;
        m_buf = 0;
        m_length = 0;
        m_lengthValid = true;
        Reserve(strlen(text) + 1);
        *this = text;
    }
    STRING(const STRING &other)
    {
        m_block = -1;
        m_pages = 0;
        m_buf = 0;
        m_length = 0;
        m_lengthValid = true;
        if (const_cast<STRING &>(other).Length() > 0) {
            Reserve(const_cast<STRING &>(other).Length() + 1);
            *this = other.c_str();
        }
    }
    virtual ~STRING() throw()
    {
        if (m_pages > 0)
            Free();
    }

    STRING &operator=(const STRING &other)
    {
        Reserve(const_cast<STRING &>(other).Length());
        *this = other.c_str();
        return *this;
    }
    // Defined elsewhere.
    STRING &operator=(const char *text);
    // Defined elsewhere: appends text.
    void Append(const char *text);
    // Makes room for at least length characters.
    void Reserve(const int &length);
    // printf into the string, up to 0x3fff characters (UNICODESTRING.h).
    void Format(const char *format, ...);

    const char *c_str() const { return !m_buf ? "" : m_buf; }

    int Length()
    {
        if (m_lengthValid)
            return m_length;
        if (m_pages > 0)
            m_length = strlen(m_buf);
        else
            m_length = 0;
        m_lengthValid = true;
        return m_length;
    }

#ifdef STRING_FREE_OUTOFLINE
    // The GUI behaviors were built seeing it out of line.
    void Free();
#else
    void Free()
    {
        if (m_block >= 0)
            g_StringManager.Free(m_block, m_pages);
        m_block = -1;
        m_pages = 0;
        m_buf = 0;
        m_length = 0;
        m_lengthValid = true;
    }
#endif

    char *m_buf;
    int m_block;
    int m_pages;
    int m_length;
    bool m_lengthValid;
};

class UNICODESTRING
{
public:
    UNICODESTRING()
    {
        m_block = -1;
        m_pages = 0;
        m_buf = 0;
        m_length = 0;
        m_lengthValid = true;
    }
    UNICODESTRING(const wchar_t *text)
    {
        m_block = -1;
        m_pages = 0;
        m_buf = 0;
        m_length = 0;
        m_lengthValid = true;
        Reserve((wcslen(text) + 1) * 2);
        *this = text;
    }
    // Seen as not throwing: the units that destroy one keep no unwind
    // state for it.
    virtual ~UNICODESTRING() throw()
    {
        if (m_pages > 0)
            Free();
    }

    // These three are defined in UNICODESTRING.h.
    void operator=(const UNICODESTRING &other);
    // 0 empties the string.
    void operator=(const wchar_t *text);
    // Makes room for at least size bytes, keeping the text.
    void Reserve(const int &size);

    const wchar_t *c_str() const { return !m_buf ? L"" : m_buf; }

    int Length()
    {
        if (m_lengthValid)
            return m_length;
        if (m_pages > 0)
            m_length = wcslen(m_buf);
        else
            m_length = 0;
        m_lengthValid = true;
        return m_length;
    }

    void Free()
    {
        if (m_block >= 0)
            ((STRINGMANAGER *)&g_UnicodeStringManager)->Free(m_block, m_pages);
        m_block = -1;
        m_pages = 0;
        m_buf = 0;
        m_length = 0;
        m_lengthValid = true;
    }

    // Takes text, either "WC" and three characters per wide character (as
    // ToEncoded writes it) or plain 8-bit text.
    void FromAnsi(const char *text);
    // The text as 8-bit characters, '?' for those that don't fit.
    FIXEDSTRING<0x200> ToAnsi();
    // The text as "WC" and three characters per wide character.
    void ToEncoded(STRING &out);

    wchar_t *m_buf;
    int m_block;
    int m_pages;
    int m_length;
    bool m_lengthValid;
};

#endif
