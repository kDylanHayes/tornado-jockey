// SRESULT: a typed value returned by script commands (CMDBASE, KERNELCOMMANDS).
#ifndef TONGAS2_SRESULT_H
#define TONGAS2_SRESULT_H

#include <string.h>
#include <wchar.h>

#include "Tongas2/MATHS.h"

#define SRESULT_MAX_STRING 512

enum SRESULTTYPE
{
    SRT_NOTSET,
    SRT_CHAR,
    SRT_INT,
    SRT_STRING,
    SRT_UNICODESTRING,
    SRT_FLOAT,
    SRT_BOOL,
    SRT_V3,
    SRT_V2,
    SRT_ANG,
    SRT_RGB,
    SRT_TEXTURE,
    SRT_COUNT
};

struct V2
{
    float x, y;

    V2() {}
    V2(const V2 &other) : x(other.x), y(other.y) {}
};

struct RGB
{
    float r, g, b;
};

// Euler angles and the rotation matrix built from them, rebuilt when dirty.
struct ANG
{
    ANG() {}
    ANG(float x, float y, float z) : euler(x, y, z), dirty(true) {}

    V3 euler;
    MATRIX matrix;
    bool dirty;
};

struct SRTEXTURE
{
    char name[SRESULT_MAX_STRING];
    int index;

    SRTEXTURE(const SRTEXTURE &other)
    {
        name[0] = 0;
        name[SRESULT_MAX_STRING - 1] = 0;
        memcpy(name, other.name, sizeof(name));
        index = other.index;
    }
};

// Indexed by SRESULTTYPE. Defined here, so every unit that includes this
// header has its own copy.
// STRING and UNICODESTRING are swapped in the original table, so error
// messages name the wrong one.
static const char s_TypeNames[SRT_COUNT][15] = {
    "NOTSET",
    "CHAR",
    "INT",
    "UNICODESTRING",
    "STRING",
    "FLOAT",
    "BOOL",
    "V3",
    "V2",
    "ANG",
    "RGB",
    "TEXTURE",
};


class SRESULT
{
public:
    SRESULT(bool value)
    {
        SetType(SRT_BOOL);
        m_value.b = value;
    }
    SRESULT(const char *value);
    SRESULT(const wchar_t *value);
    virtual ~SRESULT() { FreeValue(); }

    void SetValue(const wchar_t *value);
    void SetValue(const char *value);

    const char *GetTypeName() const
    {
        if (m_type >= 0 && m_type < SRT_COUNT)
            return s_TypeNames[m_type];
        return s_TypeNames[0];
    }

    char GetChar(const char &def) const;
    int GetInt(const int &def) const;
    float GetFloat(const float &def) const;
    const char *GetString(const char *def) const;
    const wchar_t *GetUnicodeString(const wchar_t *def) const;
    bool GetBool(const bool &def) const;
    V3 GetV3(const V3 &def) const;
    V2 GetV2(const V2 &def) const;
    ANG GetANG(const ANG &def) const;
    RGB GetRGB(const RGB &def) const;
    SRTEXTURE GetTexture(const SRTEXTURE &def) const;

    // Defined in another unit.
    void SetType(const SRESULTTYPE &type);
    void FreeValue();

private:
    union
    {
        char c;
        int i;
        float f;
        bool b;
        char *str;
        wchar_t *wstr;
        V3 *v3;
        V2 *v2;
        ANG *ang;
        RGB *rgb;
        SRTEXTURE *tex;
    } m_value;
    SRESULTTYPE m_type;
};

bool ToBool(const int &value);
bool ToBool(const float &value);
bool ToBool(const char &value);

#endif
