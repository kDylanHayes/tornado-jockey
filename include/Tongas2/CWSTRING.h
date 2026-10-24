// A heap-allocated wide C string (_wcsdup/free), CSTRING's wide twin. Empty
// strings are stored as 0. Placeholder name.
#ifndef TONGAS2_CWSTRING_H
#define TONGAS2_CWSTRING_H

#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include "Tongas2/FIXEDWSTRING.h"

class CWSTRING
{
public:
    CWSTRING() : m_str(0) {}
    CWSTRING(const wchar_t *str) : m_str(0)
    {
        if (str && *str)
            m_str = _wcsdup(str);
    }
    ~CWSTRING()
    {
        if (m_str) {
            free(m_str);
            m_str = 0;
        }
    }

    CWSTRING &operator=(const wchar_t *str)
    {
        if (str != m_str) {
            if (m_str) {
                free(m_str);
                m_str = 0;
            }
            if (str && *str)
                m_str = _wcsdup(str);
        }
        return *this;
    }
    CWSTRING &operator=(const CWSTRING &other)
    {
        if (*this != other)
            *this = other.c_str();
        return *this;
    }
    bool operator!=(const CWSTRING &other) const { return wcscmp(c_str(), other.c_str()) != 0; }

    const wchar_t *c_str() const { return m_str ? m_str : L""; }
    int Length() const
    {
        if (!m_str)
            return 0;
        return wcslen(m_str);
    }

    // Takes text, either "WC" encoded or plain 8-bit text. The exe has
    // FIXEDWSTRING::FromAnsi inlined here, but VC7.1 won't inline it for us,
    // so it is written out by hand.
    void FromAnsi(const char *text)
    {
        FIXEDWSTRING<0x200> wide;
        int n = UNICODESTRING_MANAGER::GetEncodedLength(text);
        if (n >= 0) {
            UNICODESTRING_MANAGER::Decode(wide.m_buf, text, n >= 0x1ff ? 0x1ff : n);
        } else {
            n = strlen(text);
            UNICODESTRING_MANAGER::Widen(wide.m_buf, text, n >= 0x1ff ? 0x1ff : n);
        }
        *this = wide.m_buf;
    }
    // The text as 8-bit characters, '?' for those that don't fit.
    FIXEDSTRING<0x200> ToAnsi() const
    {
        FIXEDSTRING<0x200> text;
        int n = Length() > 0x1ff ? 0x1ff : Length();
        UNICODESTRING_MANAGER::Narrow(text, c_str(), n);
        return text;
    }
    // The text "WC" encoded.
    void ToEncoded(STRING &out) const { UNICODESTRING_MANAGER::Encode(out, c_str(), Length()); }

    wchar_t *m_str;
};

#endif
