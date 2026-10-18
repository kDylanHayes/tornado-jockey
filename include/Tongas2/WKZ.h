// .wkz files: "WKZ", the uncompressed size, then a zlib stream.
// Placeholder names.
#ifndef TONGAS2_WKZ_H
#define TONGAS2_WKZ_H

#include <stdio.h>

// zlib's own error codes plus two of ours.
#define WKZ_NOT_FOUND (-7)
#define WKZ_NO_SIGNATURE (-8)

// A file, or a block of memory, read and written in order.
class WKZSTREAM
{
public:
    WKZSTREAM()
    {
        m_data = NULL;
        m_size = 0;
        m_pos = 0;
        m_fp = NULL;
    }

    int Read(void *buffer, int size);
    int Write(const void *buffer, int size);
    // Memory streams need a buffer to write into.
    void Reserve(int size)
    {
        if (!m_fp && !m_data) {
            m_data = new char[size + 1];
            m_size = size;
            m_data[size] = '\0';
        }
    }
    int Error() { return m_fp ? ferror(m_fp) : 0; }
    int Eof() { return m_fp ? feof(m_fp) : m_pos == m_size; }

    int m_size;
    int m_pos;
    FILE *m_fp;
    char *m_data;
};

class WKZ
{
public:
    int Compress(WKZSTREAM *source, WKZSTREAM *dest, int level);
    int Decompress(WKZSTREAM *source, WKZSTREAM *dest);
    const char *ErrorString(int error);
    // Reads a file and compresses or uncompresses it into a new buffer.
    int Load(char *name, char **data, int *size, int level, bool compress);
};

#endif
