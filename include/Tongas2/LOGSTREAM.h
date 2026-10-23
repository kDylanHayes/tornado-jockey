// The debug console's output stream, defined elsewhere. Only what LOG needs.
#ifndef TONGAS2_LOGSTREAM_H
#define TONGAS2_LOGSTREAM_H

#include <ostream>
#include <windows.h>

class LOGSTREAM : public std::ostream
{
public:
    virtual ~LOGSTREAM();
    // Closes the console.
    void Close();

    HANDLE m_handle;
};

#endif
