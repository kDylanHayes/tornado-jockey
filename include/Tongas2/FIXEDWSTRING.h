// A wide string in a fixed-size buffer, FIXEDSTRING's wide twin; too-long
// text is cut off. Placeholder name: only seen inlined and in its
// conversions, which the linker kept once.
#ifndef TONGAS2_FIXEDWSTRING_H
#define TONGAS2_FIXEDWSTRING_H

#include <string.h>
#include <wchar.h>

#include "Tongas2/FIXEDSTRING.h"
#include "Tongas2/UNICODESTRING.h"

template <int N>
class FIXEDWSTRING
{
public:
    FIXEDWSTRING()
    {
        m_buf[0] = 0;
        m_buf[N - 1] = 0;
    }
    FIXEDWSTRING(const wchar_t *text)
    {
        if (text)
            wcsncpy(m_buf, text, N - 1);
        else
            m_buf[0] = 0;
        m_buf[N - 1] = 0;
    }

    FIXEDWSTRING &operator=(const wchar_t *text)
    {
        if (!text) {
            m_buf[0] = 0;
            return *this;
        }
        wcsncpy(m_buf, text, N - 1);
        m_buf[N - 1] = 0;
        return *this;
    }

    // Takes text, either "WC" encoded or plain 8-bit text.
    void FromAnsi(const char *text)
    {
        int n = UNICODESTRING_MANAGER::GetEncodedLength(text);
        if (n >= 0) {
            UNICODESTRING_MANAGER::Decode(m_buf, text, n >= N - 1 ? N - 1 : n);
        } else {
            n = strlen(text);
            UNICODESTRING_MANAGER::Widen(m_buf, text, n >= N - 1 ? N - 1 : n);
        }
    }
    // The text as 8-bit characters, '?' for those that don't fit.
    FIXEDSTRING<0x200> ToAnsi()
    {
        if (!m_buf)
            return "";
        FIXEDSTRING<0x200> text;
        int n = wcslen(m_buf) > 0x1ff ? 0x1ff : wcslen(m_buf);
        UNICODESTRING_MANAGER::Narrow(text, m_buf, n);
        return text;
    }
    // The text "WC" encoded.
    void ToEncoded(STRING &out) { UNICODESTRING_MANAGER::Encode(out, m_buf, wcslen(m_buf)); }

    wchar_t m_buf[N];
};

#endif
