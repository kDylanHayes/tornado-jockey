#include <string.h>

#include "Tongas2/LOG.h"
#include "Tongas2/SRESULT.h"

bool ToBool(const int &value)
{
    return value == 0 ? false : true;
}

bool ToBool(const float &value)
{
    return value == 0 ? false : true;
}

bool ToBool(const char &value)
{
    return value == 0 ? false : true;
}

char SRESULT::GetChar(const char &def) const
{
    if (m_type != SRT_CHAR)
    {
        if (g_pLog)
            g_pLog->Error("SRESULT::GetChar> Error - Wrong value type (%s)", GetTypeName());
        return def;
    }
    return m_value.c;
}

int SRESULT::GetInt(const int &def) const
{
    if (m_type == SRT_FLOAT)
        return (int)m_value.f;
    if (m_type == SRT_BOOL)
        return m_value.b;
    if (m_type != SRT_INT)
    {
        if (g_pLog)
            g_pLog->Error("SRESULT::GetInt> Error - Wrong value type (%s)", GetTypeName());
        return def;
    }
    return m_value.i;
}

float SRESULT::GetFloat(const float &def) const
{
    if (m_type == SRT_INT)
        return (float)m_value.i;
    if (m_type == SRT_BOOL)
        return m_value.b;
    if (m_type != SRT_FLOAT)
    {
        if (g_pLog)
            g_pLog->Error("SRESULT::GetFloat> Error - Wrong value type (%s)", GetTypeName());
        return def;
    }
    return m_value.f;
}

const char *SRESULT::GetString(const char *def) const
{
    if (m_type != SRT_STRING)
    {
        if (g_pLog)
            g_pLog->Error("SRESULT::GetString> Error - Wrong value type (%s)", GetTypeName());
        return def;
    }
    return m_value.str;
}

const wchar_t *SRESULT::GetUnicodeString(const wchar_t *def) const
{
    if (m_type != SRT_UNICODESTRING)
    {
        if (g_pLog)
            g_pLog->Error("SRESULT::GetUnicodeString> Error - Wrong value type (%s)", GetTypeName());
        return def;
    }
    return m_value.wstr;
}

bool SRESULT::GetBool(const bool &def) const
{
    if (m_type == SRT_INT)
        return m_value.i != 0;
    if (m_type == SRT_FLOAT)
        return m_value.f != 0.0f;
    if (m_type != SRT_BOOL)
    {
        if (g_pLog)
            g_pLog->Error("SRESULT::GetBool> Error - Wrong value type (%s)", GetTypeName());
        return def;
    }
    return m_value.b;
}

V3 SRESULT::GetV3(const V3 &def) const
{
    if (m_type != SRT_V3)
    {
        if (g_pLog)
            g_pLog->Error("SRESULT::GetV3> Error - Wrong value type (%s)", GetTypeName());
        return def;
    }
    return *m_value.v3;
}

V2 SRESULT::GetV2(const V2 &def) const
{
    if (m_type != SRT_V2)
    {
        if (g_pLog)
            g_pLog->Error("SRESULT::GetV2> Error - Wrong value type (%s)", GetTypeName());
        return def;
    }
    return *m_value.v2;
}

ANG SRESULT::GetANG(const ANG &def) const
{
    if (m_type != SRT_ANG)
    {
        if (g_pLog)
            g_pLog->Error("SRESULT::GetANG> Error - Wrong value type (%s)", GetTypeName());
        return def;
    }
    return *m_value.ang;
}

RGB SRESULT::GetRGB(const RGB &def) const
{
    if (m_type != SRT_RGB)
    {
        if (g_pLog)
            g_pLog->Error("SRESULT::GetRGB> Error - Wrong value type (%s)", GetTypeName());
        return def;
    }
    return *m_value.rgb;
}

SRESULT::SRESULT(const char *value)
{
    if (value)
    {
        if (strlen(value) > SRESULT_MAX_STRING - 1 && g_pLog)
            g_pLog->Error("SRESULT::SRESULT> Error - string lenght pass max:%s", value);
        SetType(SRT_STRING);
        if (m_value.str)
        {
            strncpy(m_value.str, value, SRESULT_MAX_STRING - 1);
            m_value.str[SRESULT_MAX_STRING - 1] = 0;
        }
    }
}

SRESULT::SRESULT(const wchar_t *value)
{
    if (value)
    {
        if (wcslen(value) > SRESULT_MAX_STRING - 1 && g_pLog)
            g_pLog->Error("SRESULT::SRESULT> Error - unicode string lenght pass max:%s", value);
        SetType(SRT_UNICODESTRING);
        if (m_value.wstr)
        {
            wcsncpy(m_value.wstr, value, SRESULT_MAX_STRING - 1);
            m_value.wstr[SRESULT_MAX_STRING - 1] = 0;
        }
    }
}

void SRESULT::SetValue(const wchar_t *value)
{
    if (value)
    {
        if (wcslen(value) > SRESULT_MAX_STRING - 1 && g_pLog)
            g_pLog->Error("SRESULT::SetValue> Error - unicode string lenght pass max:%s", value);
        SRESULTTYPE type = SRT_UNICODESTRING;
        if (m_type != type)
        {
            FreeValue();
            SetType(type);
        }
        if (m_value.wstr)
        {
            wcsncpy(m_value.wstr, value, SRESULT_MAX_STRING - 1);
            m_value.wstr[SRESULT_MAX_STRING - 1] = 0;
        }
    }
}

void SRESULT::SetValue(const char *value)
{
    if (value)
    {
        if (strlen(value) > SRESULT_MAX_STRING - 1 && g_pLog)
            g_pLog->Error("SRESULT::SetValue> Error - string lenght pass max:%s", value);
        SRESULTTYPE type = SRT_STRING;
        if (m_type != type)
        {
            FreeValue();
            SetType(type);
        }
        if (m_value.str)
        {
            strncpy(m_value.str, value, SRESULT_MAX_STRING - 1);
            m_value.str[SRESULT_MAX_STRING - 1] = 0;
        }
    }
}

SRTEXTURE SRESULT::GetTexture(const SRTEXTURE &def) const
{
    if (m_type != SRT_TEXTURE)
    {
        if (g_pLog)
            g_pLog->Error("SRESULT::GetTexture> Error - Wrong value type (%s)", GetTypeName());
        return def;
    }
    return *m_value.tex;
}
