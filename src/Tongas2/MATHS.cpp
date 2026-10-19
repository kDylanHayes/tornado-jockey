#include <math.h>

#include "Tongas2/MATHS.h"

#define RAD2DEG 57.29578f

void V3::SwapYZ()
{
    float t = y;
    y = z;
    z = t;
}

static inline V3 Transform(const V3 &v, const MATRIX &m)
{
    return V3(v.z * m._31 + v.y * m._21 + v.x * m._11 + m._41,
              v.z * m._32 + v.y * m._22 + v.x * m._12 + m._42,
              v.z * m._33 + v.y * m._23 + v.x * m._13 + m._43);
}

bool SegmentInBox(const V3 &a, const V3 &b, const V3 &center, const MATRIX &m, const V3 &size)
{
    V3 p = a - center;
    V3 q = b - center;
    p = Transform(p, m);
    q = Transform(q, m);
    V3 scale(2.0f / size.x, 2.0f / size.y, 2.0f / size.z);
    p = p * scale;
    q = q * scale;
    if (p.x <= 1.0f && p.y <= 1.0f && p.z <= 1.0f && p.x >= -1.0f && p.y >= -1.0f && p.z >= -1.0f)
        return true;
    if (q.x <= 1.0f && q.y <= 1.0f && q.z <= 1.0f && q.x >= -1.0f && q.y >= -1.0f && q.z >= -1.0f)
        return true;
    if (p.x != q.x) {
        float ky = (q.y - p.y) / (q.x - p.x);
        float kz = (q.z - p.z) / (q.x - p.x);
        float y = ky * (1.0f - p.x) + p.y;
        float z = kz * (1.0f - p.x) + p.z;
        if (y <= 1.0f && y >= -1.0f && z <= 1.0f && z >= -1.0f)
            return true;
        y = ky * (-1.0f - p.x) + p.y;
        z = kz * (-1.0f - p.x) + p.z;
        if (y <= 1.0f && y >= -1.0f && z <= 1.0f && z >= -1.0f)
            return true;
    }
    if (p.y != q.y) {
        float kx = (q.x - p.x) / (q.y - p.y);
        float kz = (q.z - p.z) / (q.y - p.y);
        float x = kx * (1.0f - p.y) + p.x;
        float z = kz * (1.0f - p.y) + p.z;
        if (x <= 1.0f && x >= -1.0f && z <= 1.0f && z >= -1.0f)
            return true;
        x = kx * (-1.0f - p.y) + p.x;
        z = kz * (-1.0f - p.y) + p.z;
        if (x <= 1.0f && x >= -1.0f && z <= 1.0f && z >= -1.0f)
            return true;
    }
    if (p.z != q.z) {
        float kx = (q.x - p.x) / (q.z - p.z);
        float ky = (q.y - p.y) / (q.z - p.z);
        float x = kx * (1.0f - p.z) + p.x;
        float y = ky * (1.0f - p.z) + p.y;
        if (x <= 1.0f && x >= -1.0f && y <= 1.0f && y >= -1.0f)
            return true;
        x = kx * (-1.0f - p.z) + p.x;
        y = ky * (-1.0f - p.z) + p.y;
        if (x <= 1.0f && x >= -1.0f && y <= 1.0f && y >= -1.0f)
            return true;
    }
    return false;
}

void CatmullRom(V3 *out, V3 p0, V3 p1, V3 p2, V3 p3, float t)
{
    // Declared in this order the compiler spills t2, as the original does.
    float t3 = t * t * t;
    float t2 = t * t;
    *out = ((p1 * 2) + (p2 - p0) * t + (p0 * 2 - p1 * 5 + p2 * 4 - p3) * t2 + (p1 * 3 - p2 * 3 + p3 - p0) * t3) * 0.5f;
}

void CatmullRomTangent(V3 *out, V3 p0, V3 p1, V3 p2, V3 p3, float t)
{
    float t2 = t * t * 3;
    float t1 = t * 2;
    *out = ((p2 - p0) + (p0 * 2 - p1 * 5 + p2 * 4 - p3) * t1 + (p1 * 3 - p2 * 3 + p3 - p0) * t2) * 0.5f;
}

V3 TriangleNormal(const V3 &a, const V3 &b, const V3 &c)
{
    // Named components; written straight into the constructor, the compiler
    // orders the terms differently.
    float x = a.y * (b.z - c.z) + b.y * (c.z - a.z) + c.y * (a.z - b.z);
    float y = a.z * (b.x - c.x) + b.z * (c.x - a.x) + c.z * (a.x - b.x);
    float z = a.x * (b.y - c.y) + b.x * (c.y - a.y) + c.x * (a.y - b.y);
    V3 n(x, y, z);
    n.Normalize();
    return n;
}

float V3::AngleY(const V3 &o) const
{
    float angle = atan2f(x, z) * RAD2DEG - atan2f(o.x, o.z) * RAD2DEG;
    if (angle > 360.0f || angle < -360.0f)
        angle = (float)((int)(angle * 100.0f) % 36000) * 0.01f;
    if (angle > 180.0f)
        return angle - 360.0f;
    if (angle < -180.0f)
        angle += 360.0f;
    return angle;
}
