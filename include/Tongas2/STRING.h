// Engine strings. Buffers come from a pooled allocator in 256-byte pages.
// Placeholder names; only what other units need so far.
#ifndef TONGAS2_STRING_H
#define TONGAS2_STRING_H

#include <string.h>

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
    // Defined elsewhere.
    STRING();
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
    virtual ~STRING()
    {
        if (m_pages > 0)
            Free();
    }

    STRING &operator=(const STRING &other);
    // Defined elsewhere.
    STRING &operator=(const char *text);
    // Makes room for at least length characters.
    void Reserve(const int &length);

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

    const wchar_t *c_str() const { return !m_buf ? L"" : m_buf; }

    wchar_t *m_buf;
    int m_block;
    int m_pages;
    int m_length;
    bool m_lengthValid;
};

#endif
