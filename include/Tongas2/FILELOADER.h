// Reads a game file from disk. FILELOADERWKZ overrides it to read
// zlib-compressed .wkz files. Placeholder names.
#ifndef TONGAS2_FILELOADER_H
#define TONGAS2_FILELOADER_H

#include <stdio.h>

class FILELOADER
{
public:
    FILELOADER();
    virtual ~FILELOADER();

    virtual bool Open(char *name);
    virtual bool IsOpen() { return m_fp != NULL; }
    virtual bool IsError();
    // MSVC puts overloads in the vtable in reverse order: Read(buffer,
    // size) comes first.
    virtual int Read(void *buffer, int size, int count);
    virtual int Read(void *buffer, int size);
    virtual bool IsEof();
    virtual void Close();
    // Next line without its line break, strdup'ed; NULL at the end.
    virtual char *ReadLine();
    virtual __int64 GetTime();
    // The whole file, loaded on first use.
    virtual char *GetData();
    virtual int GetSize();
    virtual bool IsCompressed() { return false; }

    char *m_name;
    FILE *m_fp;
    char *m_data;
};

typedef FILELOADER *(*PFNCREATEFILELOADER)();

FILELOADER *CreateFileLoader();
// Makes the loader for every file; FILELOADERWKZ installs its own.
extern PFNCREATEFILELOADER g_pfnCreateFileLoader;

#endif
