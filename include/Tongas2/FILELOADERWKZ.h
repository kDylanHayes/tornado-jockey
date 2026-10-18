// FILELOADER that reads name.wkz instead of name when it exists: a
// zlib-compressed copy of the file, loaded whole. Placeholder names.
#ifndef TONGAS2_FILELOADERWKZ_H
#define TONGAS2_FILELOADERWKZ_H

#include "Tongas2/FILELOADER.h"
#include "Tongas2/WKZ.h"

class FILELOADERWKZ : public FILELOADER
{
public:
    FILELOADERWKZ()
    {
        m_buffer = NULL;
        m_isWkz = false;
    }
    virtual ~FILELOADERWKZ();

    virtual bool Open(char *name);
    virtual bool IsOpen();
    virtual int IsError();
    virtual bool IsEof();
    virtual void Close();
    virtual char *ReadLine();
    virtual __int64 GetTime();
    virtual char *GetData();
    virtual int GetSize();
    virtual bool IsCompressed();

    bool m_isWkz;
    char *m_buffer;    // the uncompressed file
    char *m_cursor;    // ReadLine position in m_buffer
    int m_size;
    __int64 m_time;
    int m_error;       // from WKZ::Load
    bool m_eof;
};

void InitFileLoaderWKZ(void *);
void ShutdownFileLoaderWKZ();

extern WKZ *g_pWkz;

#endif
