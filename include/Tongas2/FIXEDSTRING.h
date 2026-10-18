// A string in a fixed-size buffer; too-long text is cut off. Placeholder
// name: only seen inlined and in operator+=, which the linker kept once.
#ifndef TONGAS2_FIXEDSTRING_H
#define TONGAS2_FIXEDSTRING_H

#include <string.h>

template <int N>
class FIXEDSTRING
{
public:
    FIXEDSTRING(const char *text)
    {
        if (text)
            strncpy(m_buf, text, N - 1);
        else
            m_buf[0] = '\0';
        m_buf[N - 1] = '\0';
    }

    // Appends text only when all of it fits.
    void operator+=(const char *text)
    {
        if ((int)(strlen(text) + strlen(m_buf)) < N - 1)
            strcat(m_buf, text);
    }

    operator char *() { return m_buf; }

    char m_buf[N];
};

#endif
