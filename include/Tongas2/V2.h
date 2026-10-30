// The engine's 2D vector.
#ifndef TONGAS2_V2_H
#define TONGAS2_V2_H

#include <math.h>

struct V2
{
    float x, y;

    V2() {}
    V2(float x_, float y_) : x(x_), y(y_) {}
    V2(const V2 &other) : x(other.x), y(other.y) {}

    float Length() const { return (float)sqrt(x * x + y * y); }
    // The vector scaled to length 1; zero stays zero. Defined elsewhere.
    V2 Normalized() const;
};

inline V2 operator-(const V2 &a, const V2 &b)
{
    return V2(a.x - b.x, a.y - b.y);
}

inline V2 operator+(const V2 &a, const V2 &b)
{
    return V2(a.x + b.x, a.y + b.y);
}

inline V2 operator*(const V2 &v, const float &f)
{
    return V2(v.x * f, v.y * f);
}

#endif
