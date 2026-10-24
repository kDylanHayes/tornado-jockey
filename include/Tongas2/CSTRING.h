// A heap-allocated C string (strdup/free). Empty strings are stored as 0.
#ifndef TONGAS2_CSTRING_H
#define TONGAS2_CSTRING_H

#include <stdlib.h>
#include <string.h>

class CSTRING
{
public:
    CSTRING() : m_str(0) {}
    CSTRING(const char *str)
    {
        m_str = 0;
        if (str && *str)
            m_str = _strdup(str);
    }
    ~CSTRING()
    {
        if (m_str) {
            free(m_str);
            m_str = 0;
        }
    }

    CSTRING &operator=(const char *str)
    {
        if (str != m_str) {
            if (m_str) {
                free(m_str);
                m_str = 0;
            }
            if (str && *str)
                m_str = _strdup(str);
        }
        return *this;
    }
    CSTRING &operator=(const CSTRING &other)
    {
        if (*this != other)
            *this = other.c_str();
        return *this;
    }
    bool operator!=(const CSTRING &other) const { return strcmp(c_str(), other.c_str()) ? true : false; }

    const char *c_str() const { return m_str ? m_str : ""; }
    int Length() const { return !m_str ? 0 : (int)strlen(m_str); }

    char *m_str;
};

#endif
