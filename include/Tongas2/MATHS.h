// MATHS: the engine's vector type and the small geometry helpers that sit
// just before APPOPTIONS in the exe.
#ifndef TONGAS2_MATHS_H
#define TONGAS2_MATHS_H

#include <math.h>

struct MATRIX;

struct V3
{
    float x, y, z;

    V3() {}
    V3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}

    V3 operator-(const V3 &o) const { return V3(x - o.x, y - o.y, z - o.z); }
    V3 &operator+=(const V3 &o) { x += o.x; y += o.y; z += o.z; return *this; }
    V3 operator*(float s) const { return V3(x * s, y * s, z * s); }

    // The point transformed by m, translation included.
    V3 Transform(const MATRIX &m) const;
    bool IsZero() const { return x == 0.0f && y == 0.0f && z == 0.0f; }
    float Length() const { return (float)sqrt(x * x + y * y + z * z); }

    // 0x402c90, out of line and called from everywhere.
    void Normalize();
    void SwapYZ();
    // Signed heading difference about the Y axis, in degrees (-180..180].
    float AngleY(const V3 &o) const;
};

// Laid out like D3DMATRIX.
struct MATRIX
{
    float _11, _12, _13, _14;
    float _21, _22, _23, _24;
    float _31, _32, _33, _34;
    float _41, _42, _43, _44;

    // Defined in other units. The rotation from euler angles (in degrees),
    // as ANG passes them: z, y, then x.
    void FromEuler(float z, float y, float x);
    void ToEuler(float *z, float *y, float *x);
    void Invert();
};

inline V3 operator+(const V3 &a, const V3 &b)
{
    return V3(a.x + b.x, a.y + b.y, a.z + b.z);
}

// Componentwise.
inline V3 operator*(const V3 &a, const V3 &b)
{
    return V3(a.x * b.x, a.y * b.y, a.z * b.z);
}

inline V3 V3::Transform(const MATRIX &m) const
{
    return V3(x * m._11 + y * m._21 + z * m._31 + m._41,
              x * m._12 + y * m._22 + z * m._32 + m._42,
              x * m._13 + y * m._23 + z * m._33 + m._43);
}

// Defined in another unit.
MATRIX operator*(const MATRIX &a, const MATRIX &b);

// Whether the segment a-b touches the box with the given center, rotation and
// full extents.
bool SegmentInBox(const V3 &a, const V3 &b, const V3 &center, const MATRIX &m, const V3 &size);
void CatmullRom(V3 *out, V3 p0, V3 p1, V3 p2, V3 p3, float t);
void CatmullRomTangent(V3 *out, V3 p0, V3 p1, V3 p2, V3 p3, float t);
V3 TriangleNormal(const V3 &a, const V3 &b, const V3 &c);

#endif
