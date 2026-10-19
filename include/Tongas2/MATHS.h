// MATHS: the engine's vector type and the small geometry helpers that sit
// just before APPOPTIONS in the exe.
#ifndef TONGAS2_MATHS_H
#define TONGAS2_MATHS_H

struct V3
{
    float x, y, z;

    V3() {}
    V3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}

    V3 operator+(const V3 &o) const { return V3(x + o.x, y + o.y, z + o.z); }
    V3 operator-(const V3 &o) const { return V3(x - o.x, y - o.y, z - o.z); }
    V3 operator*(float s) const { return V3(x * s, y * s, z * s); }
    // Componentwise.
    V3 operator*(const V3 &o) const { return V3(x * o.x, y * o.y, z * o.z); }

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
};

// Whether the segment a-b touches the box with the given center, rotation and
// full extents.
bool SegmentInBox(const V3 &a, const V3 &b, const V3 &center, const MATRIX &m, const V3 &size);
void CatmullRom(V3 *out, V3 p0, V3 p1, V3 p2, V3 p3, float t);
void CatmullRomTangent(V3 *out, V3 p0, V3 p1, V3 p2, V3 p3, float t);
V3 TriangleNormal(const V3 &a, const V3 &b, const V3 &c);

#endif
