// Tongas2/GAMECORE2 -- the game-code block at 0x4162f0. These are the ANG,
// V3, FIXEDSTRING and SPAWNER members the original translation unit emitted
// out of line.
#define ANG_EULER_OUTOFLINE
#define V3_ISZERO_OUTOFLINE
#define SPAWNER_SETTERS_OUTOFLINE

#include "Tongas2/FIXEDSTRING.h"
#include "Tongas2/MATHS.h"
#include "Tongas2/SPAWNER.h"
#include "Tongas2/SRESULT.h"

float ANG::GetEulerY()
{
    if (flags & EULER_DIRTY) {
        matrix.ToEuler(&euler.z, &euler.y, &euler.x);
        flags &= ~EULER_DIRTY;
    }
    return euler.y;
}

float ANG::GetEulerZ()
{
    if (flags & EULER_DIRTY) {
        matrix.ToEuler(&euler.z, &euler.y, &euler.x);
        flags &= ~EULER_DIRTY;
    }
    return euler.z;
}

void ANG::GetEuler(float *x, float *y, float *z)
{
    if (flags & EULER_DIRTY) {
        matrix.ToEuler(&euler.z, &euler.y, &euler.x);
        flags &= ~EULER_DIRTY;
    }
    *x = euler.x;
    *y = euler.y;
    *z = euler.z;
}

bool V3::IsZero() const
{
    return x == 0.0f && y == 0.0f && z == 0.0f;
}

bool operator==(const V3 &a, const V3 &b)
{
    return a.x == b.x && a.y == b.y && a.z == b.z;
}

V3 operator*(const V3 &v, const MATRIX &m)
{
    return V3(v.z * m._31 + v.x * m._11 + v.y * m._21 + m._41,
              v.z * m._32 + v.x * m._12 + v.y * m._22 + m._42,
              v.z * m._33 + v.x * m._13 + v.y * m._23 + m._43);
}

template <int N>
void FIXEDSTRING<N>::operator+=(const char *text)
{
    if (strlen(text) + strlen(m_buf) < N - 1)
        strcpy(m_buf + strlen(m_buf), text);
}

template void FIXEDSTRING<0x200>::operator+=(const char *);

void SPAWNER::SetMaxSpawnTime(const float &time)
{
    m_maxSpawnTime = time;
}

void SPAWNER::SetMaxSpawns(const int &max)
{
    m_maxSpawns = max;
}
