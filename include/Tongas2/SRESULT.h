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
    V2(float x_, float y_) : x(x_), y(y_) {}
    V2(const V2 &other) : x(other.x), y(other.y) {}

    float Length() const { return (float)sqrt(x * x + y * y); }
};

inline V2 operator-(const V2 &a, const V2 &b)
{
    return V2(a.x - b.x, a.y - b.y);
}

struct RGB
{
    float r, g, b;
};

// Euler angles and the rotation matrix, each rebuilt from the other when
// its flag is set.
struct ANG
{
    enum
    {
        MATRIX_DIRTY = 1,
        EULER_DIRTY = 2
    };

    ANG() {}
    ANG(float x, float y, float z) : euler(x, y, z), flags(MATRIX_DIRTY) {}
    ANG(const MATRIX &m) : euler(0.0f, 0.0f, 0.0f), matrix(m), flags(EULER_DIRTY) {}
    // Looking along direction from position.
    ANG(const V3 &position, const V3 &direction) : euler(0.0f, 0.0f, 0.0f)
    {
        matrix.LookAt(position, direction);
        flags = EULER_DIRTY;
    }

    const V3 &GetEuler()
    {
        if (flags & EULER_DIRTY) {
            matrix.ToEuler(&euler.z, &euler.y, &euler.x);
            flags &= ~EULER_DIRTY;
        }
        return euler;
    }
    // Each euler angle, and all three.
    float GetEulerX()
    {
        if (flags & EULER_DIRTY) {
            matrix.ToEuler(&euler.z, &euler.y, &euler.x);
            flags &= ~EULER_DIRTY;
        }
        return euler.x;
    }
    float GetEulerY()
    {
        if (flags & EULER_DIRTY) {
            matrix.ToEuler(&euler.z, &euler.y, &euler.x);
            flags &= ~EULER_DIRTY;
        }
        return euler.y;
    }
    float GetEulerZ()
    {
        if (flags & EULER_DIRTY) {
            matrix.ToEuler(&euler.z, &euler.y, &euler.x);
            flags &= ~EULER_DIRTY;
        }
        return euler.z;
    }
    void GetEuler(float *x, float *y, float *z)
    {
        if (flags & EULER_DIRTY) {
            matrix.ToEuler(&euler.z, &euler.y, &euler.x);
            flags &= ~EULER_DIRTY;
        }
        *x = euler.x;
        *y = euler.y;
        *z = euler.z;
    }
    MATRIX GetMatrix()
    {
        if (flags & MATRIX_DIRTY) {
            matrix.FromEuler(euler.z, euler.y, euler.x);
            flags &= ~MATRIX_DIRTY;
        }
        return matrix;
    }

    void SetMatrix(const MATRIX &m)
    {
        matrix = m;
        flags = EULER_DIRTY;
    }
    void SetLookAt(const V3 &position, const V3 &direction)
    {
        matrix.LookAt(position, direction);
        flags = EULER_DIRTY;
    }

    // Defined in other units.
    bool IsZero();
    // A point turned by the angles.
    V3 Transform(V3 point);
    // Turns these angles by other.
    void Rotate(const ANG &other);
    // The direction the angles face.
    V3 GetDirection();
    // The translation plus the up axis of the matrix.
    V3 GetUpPoint();
    // The translation plus the right axis of the matrix.
    V3 GetRightPoint();
    // Turns the angles to face a direction.
    void PointAt(const V3 &direction);

    V3 euler;
    MATRIX matrix;
    unsigned char flags;
};

// Defined in another unit: whether the euler angles differ.
bool operator!=(const ANG &a, const ANG &b);

struct SRTEXTURE
{
    char name[SRESULT_MAX_STRING];
    int index;

    SRTEXTURE()
    {
        name[0] = 0;
        name[SRESULT_MAX_STRING - 1] = 0;
        index = 0;
    }
    SRTEXTURE(const SRTEXTURE &other)
    {
        name[0] = 0;
        name[SRESULT_MAX_STRING - 1] = 0;
        memcpy(name, other.name, sizeof(name));
        index = other.index;
    }
    SRTEXTURE &operator=(const SRTEXTURE &other)
    {
        memcpy(name, other.name, sizeof(name));
        index = other.index;
        return *this;
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
    SRESULT(const SRESULT &other)
    {
        m_type = SRT_NOTSET;
        m_value.i = 0;
        Assign(other);
    }
    SRESULT(const char *value);
    SRESULT(const wchar_t *value);
    virtual ~SRESULT() { FreeValue(); }

    void SetValue(const wchar_t *value);
    void SetValue(const char *value);

    SRESULTTYPE GetType() const { return m_type; }
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
    // Most of the exe's objects were built seeing FreeValue as not
    // throwing; the few that weren't define SRESULT_FREEVALUE_MAY_THROW
    // (see toolchain.md).
#ifdef SRESULT_FREEVALUE_MAY_THROW
    void FreeValue();
#else
    void FreeValue() throw();
#endif
    // Copies the value (strings and the larger types are duplicated).
    void Assign(const SRESULT &other);

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
