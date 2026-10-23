// A heap-allocated wide C string (_wcsdup/free), CSTRING's wide twin. Empty
// strings are stored as 0. Placeholder name.
#ifndef TONGAS2_CWSTRING_H
#define TONGAS2_CWSTRING_H

#include <stdlib.h>
#include <string.h>
#include <wchar.h>

class CWSTRING
{
public:
    CWSTRING() : m_str(0) {}
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

    wchar_t *m_str;
};

#endif
