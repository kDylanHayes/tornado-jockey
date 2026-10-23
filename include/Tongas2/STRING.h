// Engine strings. Buffers come from a pooled allocator in 256-byte pages.
// Placeholder names; only what other units need so far.
#ifndef TONGAS2_STRING_H
#define TONGAS2_STRING_H

#include <string.h>
#include <wchar.h>

#include "Tongas2/FIXEDSTRING.h"

class STRINGMANAGER
{
public:
    // Returns a buffer to the pool and updates the usage counters.
    void Free(int &block, int &pages);
};

extern STRINGMANAGER g_StringManager;

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
    virtual ~STRING() throw()
    {
        if (m_pages > 0)
            Free();
    }

    STRING &operator=(const STRING &other);
    // Defined elsewhere.
    STRING &operator=(const char *text);
    // Makes room for at least length characters.
    void Reserve(const int &length);
    // Defined elsewhere: printf into the string, up to 0x3fff characters.
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

    char *m_buf;
    int m_block;
    int m_pages;
    int m_length;
    bool m_lengthValid;
};

class UNICODESTRING
{
public:
    virtual ~UNICODESTRING();

    UNICODESTRING &operator=(const UNICODESTRING &other);
    // Defined elsewhere: makes room for at least size bytes.
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
