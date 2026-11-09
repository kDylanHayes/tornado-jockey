// Tongas2/GAMECORE1 -- the core engine block at 0x401d20: small string,
// handle, primitive and result support code. The class definitions live in
// their usual headers; TONGAS2_CORE_OUTOFLINE makes the compiler emit these
// inline members here instead of inlining every use.
#define TONGAS2_CORE_OUTOFLINE
#define STRING_CORE_OUTOFLINE
#define PRIMV2_OUTOFLINE

#include <stdarg.h>
#include <stdio.h>

#include "Tongas2/CSTRING.h"
#include "Tongas2/CMDBASE.h"
#include "Tongas2/CPRIMITIVE3.h"
#include "Tongas2/FIXEDWSTRING.h"
#include "Tongas2/SAFEINDEX.h"
#include "Tongas2/SRESULT.h"
#include "Tongas2/STRING.h"
#include "Tongas2/TOKENS.h"

bool operator==(const RESHANDLE &a, const RESHANDLE &b)
{
    return a.serial == b.serial && a.index == b.index && a.kind == b.kind;
}

STRING::STRING()
{
    m_block = -1;
    m_pages = 0;
    m_buf = 0;
    m_length = 0;
    m_lengthValid = true;
}

int STRING::Length()
{
    if (m_lengthValid)
        return m_length;
    if (m_pages > 0)
        m_length = strlen(m_buf);
    else
        m_length = 0;
    m_lengthValid = true;
    return m_length;
}

PRIMV2::PRIMV2()
    : V2(0.0f, 0.0f)
{
}

PRIMV3::PRIMV3()
    : V3(0.0f, 0.0f, 0.0f)
{
}

RGB::RGB()
{
    r = g = b = 0;
}

ANG::ANG()
    : euler(0.0f, 0.0f, 0.0f), flags(MATRIX_DIRTY)
{
}

CSTRING &CSTRING::operator=(const char *str)
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

const char *TOKENS::GetToken(int i)
{
    if (i < 0 || i >= m_count)
        return "";
    return m_string.c_str() + m_offsets[i];
}

void SRESULT::FreeValue() throw()
{
    switch (m_type) {
    case SRT_STRING:
    case SRT_UNICODESTRING:
        if (m_value.str) {
            free(m_value.str);
            m_value.str = 0;
        }
        break;
    case SRT_V3:
        if (m_value.v3) {
            delete m_value.v3;
            m_value.v3 = 0;
        }
        break;
    case SRT_V2:
        if (m_value.v2) {
            delete m_value.v2;
            m_value.v2 = 0;
        }
        break;
    case SRT_ANG:
        if (m_value.ang) {
            delete m_value.ang;
            m_value.ang = 0;
        }
        break;
    case SRT_RGB:
        if (m_value.rgb) {
            delete m_value.rgb;
            m_value.rgb = 0;
        }
        break;
    case SRT_TEXTURE:
        if (m_value.tex) {
            delete m_value.tex;
            m_value.tex = 0;
        }
        break;
    }
}

SRESULT::SRESULT(const bool &value)
{
    SetType(SRT_BOOL);
    m_value.b = value;
}

SRESULT::~SRESULT()
{
    FreeValue();
}

int SafeSprintf(char *buf, const char *format, ...) throw()
{
    va_list args;
    va_start(args, format);
    return _vsnprintf(buf, 0x1fe, format, args);
}

int SafeSwprintf(wchar_t *buf, const wchar_t *format, ...) throw()
{
    va_list args;
    va_start(args, format);
    return vswprintf(buf, 0x1fe, format, args);
}

void V3::Normalize()
{
    float length = (float)sqrt(x * x + y * y + z * z);
    if (length != 0.0f) {
        x /= length;
        y /= length;
        z /= length;
    }
    else {
        x = y = z = 0.0f;
    }
}

V3 V3::Normalized() const
{
    V3 out = *this;
    out.Normalize();
    return out;
}

STRING::~STRING() throw()
{
    if (m_pages > 0)
        Free();
}

CSTRING &CSTRING::operator=(const CSTRING &other)
{
    if (strcmp(c_str(), other.c_str()) != 0)
        *this = other.c_str();
    return *this;
}

CSTRING::CSTRING(const char *str)
{
    m_str = 0;
    if (str && *str)
        m_str = _strdup(str);
}

STRING::STRING(const char *text)
{
    m_block = -1;
    m_pages = 0;
    m_buf = 0;
    m_length = 0;
    m_lengthValid = true;
    Reserve((int)strlen(text) + 1);
    *this = text;
}

STRING &STRING::operator=(const char *text)
{
    if (!text) {
        Free();
        return *this;
    }
    int length = (int)strlen(text) + 1;
    Reserve(length);
    if (m_block >= 0) {
        char *dst = m_buf;
        const char *src = text;
        while ((*dst++ = *src++) != 0) {
        }
        m_lengthValid = false;
    }
    else {
        Free();
    }
    return *this;
}

TOKENS::~TOKENS()
{
    m_string.Free();
}

template <int N>
FIXEDSTRING<N>::FIXEDSTRING()
{
    m_buf[0] = '\0';
    m_buf[N - 1] = '\0';
}

template <int N>
FIXEDSTRING<N> &FIXEDSTRING<N>::operator=(const char *text)
{
    if (!text) {
        m_buf[0] = '\0';
        return *this;
    }
    strncpy(m_buf, text, N - 1);
    m_buf[N - 1] = '\0';
    return *this;
}

template FIXEDSTRING<0x200>::FIXEDSTRING();
template FIXEDSTRING<0x200> &FIXEDSTRING<0x200>::operator=(const char *);

template <int N>
FIXEDWSTRING<N>::FIXEDWSTRING()
{
    m_buf[0] = 0;
    m_buf[N - 1] = 0;
}

template FIXEDWSTRING<0x200>::FIXEDWSTRING();

void CMDBASE::SetName(const CSTRING &name)
{
    m_name = name;
}

static V3 Cross(const V3 &a, const V3 &b)
{
    return V3(a.y * b.z - a.z * b.y,
              a.z * b.x - a.x * b.z,
              a.x * b.y - a.y * b.x);
}

void MATRIX::LookAt(const V3 &dir, const V3 &up)
{
    V3 right = Cross(dir, up);
    right.Normalize();
    _11 = right.x;
    _12 = right.y;
    _13 = right.z;
    _21 = dir.x;
    _22 = dir.y;
    _23 = dir.z;
    _31 = up.x;
    _32 = up.y;
    _33 = up.z;
    _14 = _24 = _34 = 0.0f;
    _41 = _42 = _43 = 0.0f;
    _44 = 1.0f;
}
