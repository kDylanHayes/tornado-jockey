#include <new>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "Tongas2/FIXEDSTRING.h"
#include "Tongas2/FILELOADERWKZ.h"
#include "Tongas2/LOG.h"

WKZ *g_pWkz;

FILELOADER *CreateFileLoaderWKZ();

void InitFileLoaderWKZ(void *)
{
    g_pWkz = new WKZ;
    g_pfnCreateFileLoader = CreateFileLoaderWKZ;
}

void ShutdownFileLoaderWKZ()
{
    if (g_pWkz) {
        delete g_pWkz;
        g_pWkz = NULL;
    }
}

bool FILELOADERWKZ::IsCompressed()
{
    return m_isWkz;
}

void FILELOADERWKZ::Close()
{
    if (!m_isWkz)
        FILELOADER::Close();
    if (m_buffer) {
        delete m_buffer;
        m_buffer = NULL;
    }
}

bool FILELOADERWKZ::IsOpen()
{
    if (!m_isWkz)
        return FILELOADER::IsOpen();
    return m_buffer != NULL;
}

int FILELOADERWKZ::IsError()
{
    if (!m_isWkz)
        return FILELOADER::IsError();
    if (m_error && g_pLog)
        g_pLog->Error("Fileloader DX8 error: %s", g_pWkz->ErrorString(m_error));
    return m_error;
}

bool FILELOADERWKZ::IsEof()
{
    if (!m_isWkz)
        return FILELOADER::IsEof();
    return m_eof;
}

char *FILELOADERWKZ::ReadLine()
{
    if (!m_isWkz)
        return FILELOADER::ReadLine();
    char *end = strstr(m_cursor, "\n");
    if (end) {
        *end = '\0';
        char *line = _strdup(m_cursor);
        m_cursor = end + 1;
        return line;
    }
    char *line = _strdup(m_cursor);
    m_eof = true;
    return line;
}

char *FILELOADERWKZ::GetData()
{
    if (!m_buffer)
        return FILELOADER::GetData();
    return m_buffer;
}

int FILELOADERWKZ::GetSize()
{
    if (m_buffer)
        return m_size;
    return FILELOADER::GetSize();
}

__int64 FILELOADERWKZ::GetTime()
{
    if (!m_buffer)
        return FILELOADER::GetTime();
    return m_time;
}

FILELOADER *CreateFileLoaderWKZ()
{
    return new FILELOADERWKZ;
}

FILELOADERWKZ::~FILELOADERWKZ()
{
    Close();
}

bool FILELOADERWKZ::Open(char *name)
{
    if (m_buffer) {
        delete m_buffer;
        m_buffer = NULL;
    }
    FIXEDSTRING<512> path(name);
    path += ".wkz";
    m_error = 0;
    int error = g_pWkz->Load(path, &m_buffer, &m_size, -1, false);
    if (error != 0) {
        if (error != WKZ_NOT_FOUND) {
            m_error = error;
            if (g_pLog)
                g_pLog->Error("FILELOADERWKZ (file %s) %s", (char *)path, g_pWkz->ErrorString(error));
        }
        m_isWkz = false;
        return FILELOADER::Open(name);
    }
    m_time = 0;
    FILE *fp = fopen(path, "r");
    if (fp) {
        struct __stat64 st;
        if (!_fstat64(_fileno(fp), &st))
            m_time = st.st_mtime;
        fclose(fp);
    }
    m_eof = false;
    m_isWkz = true;
    m_cursor = m_buffer;
    return true;
}
