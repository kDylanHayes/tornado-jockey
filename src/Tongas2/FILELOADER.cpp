#include <new>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "Tongas2/FILELOADER.h"

static char s_line[0x10000];

bool FILELOADER::Open(char *name)
{
    for (char *p = name; *p; p++) {
        if (*p == '/')
            *p = '\\';
    }
    if (name != m_name) {
        if (m_name) {
            free(m_name);
            m_name = NULL;
        }
        if (*name)
            m_name = _strdup(name);
    }
    m_fp = fopen(name, "rb");
    return m_fp ? true : false;
}

int FILELOADER::IsError()
{
    return m_fp == NULL;
}

bool FILELOADER::IsEof()
{
    if (!m_fp)
        return true;
    return feof(m_fp) != 0;
}

void FILELOADER::Close()
{
    if (m_fp) {
        fclose(m_fp);
        m_fp = NULL;
    }
    if (m_data) {
        delete[] m_data;
        m_data = NULL;
    }
}

char *FILELOADER::ReadLine()
{
    if (!m_fp)
        return NULL;
    if (!fgets(s_line, sizeof(s_line), m_fp))
        return NULL;
    for (char *p = s_line + strlen(s_line) - 1; p >= s_line && (*p == '\n' || *p == '\r'); p--)
        *p = 0;
    return _strdup(s_line);
}

int FILELOADER::Read(void *buffer, int size)
{
    if (!m_fp)
        return -1;
    return fread(buffer, 1, size, m_fp);
}

int FILELOADER::Read(void *buffer, int size, int count)
{
    if (!m_fp)
        return -1;
    return fread(buffer, size, count, m_fp);
}

char *FILELOADER::GetData()
{
    int size = GetSize();
    if (size <= 0)
        return NULL;
    if (!m_data) {
        m_data = new char[size];
        fread(m_data, 1, size, m_fp);
    }
    return m_data;
}

int FILELOADER::GetSize()
{
    if (!m_fp)
        return 0;
    return _filelength(_fileno(m_fp));
}

__int64 FILELOADER::GetTime()
{
    struct __stat64 st;
    if (!m_fp)
        return 0;
    if (_fstat64(_fileno(m_fp), &st))
        return 0;
    return st.st_mtime;
}

FILELOADER::FILELOADER()
{
    m_name = NULL;
    m_fp = NULL;
    m_data = NULL;
}

FILELOADER::~FILELOADER()
{
    Close();
    if (m_name) {
        free(m_name);
        m_name = NULL;
    }
}

FILELOADER *CreateFileLoader()
{
    return new FILELOADER;
}

PFNCREATEFILELOADER g_pfnCreateFileLoader = CreateFileLoader;
