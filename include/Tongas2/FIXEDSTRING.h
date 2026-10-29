// A string in a fixed-size buffer; too-long text is cut off. Placeholder
// name: only seen inlined and in operator+=, which the linker kept once.
#ifndef TONGAS2_FIXEDSTRING_H
#define TONGAS2_FIXEDSTRING_H

#include <string.h>

template <int N>
class FIXEDSTRING
{
public:
    FIXEDSTRING()
    {
        m_buf[0] = '\0';
        m_buf[N - 1] = '\0';
    }
    FIXEDSTRING(const char *text)
    {
        if (text)
            strncpy(m_buf, text, N - 1);
        else
            m_buf[0] = '\0';
        m_buf[N - 1] = '\0';
    }

    // Out of line; defined in another unit.
    FIXEDSTRING &operator=(const char *text);

    // What operator= does, inline: the exe inlines it in some places and
    // calls it in others.
    void Assign(const char *text)
    {
        if (!text) {
            m_buf[0] = '\0';
            return;
        }
        strncpy(m_buf, text, N - 1);
        m_buf[N - 1] = '\0';
    }
    void Set(const char *text)
    {
        strncpy(m_buf, text, N - 1);
        m_buf[N - 1] = '\0';
    }
    void Replace(char from, char to)
    {
        for (int i = 0; i < N; i++) {
            if (!m_buf[i])
                return;
            if (m_buf[i] == from)
                m_buf[i] = to;
        }
    }

    // Appends text only when all of it fits. Out of line; defined in
    // another unit (the exe never inlines it).
    void operator+=(const char *text);

    operator char *() { return m_buf; }

    char m_buf[N];
};

#endif
