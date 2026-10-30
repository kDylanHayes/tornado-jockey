// Where the game keeps its files, and the shapes' geometry: the save and
// executable folders, making a folder and the ones above it, the distances
// between segments, points and boxes, whether two shapes touch, keeping
// boxes' and quads' corners and planes up to date, and easing a value in
// and out. Placeholder names apart from the class names.
#define ANG_INLINE_AXES
#define ANG_INLINE_RIGHT
#define FRUSTUM_MATHS
#define GEOMETRY_DEFINITIONS
#include <direct.h>
#include <string.h>
#include <string>

#include "Tongas2/CPRIMITIVE3.h"

// The shapes' headers bring in FreeImage's stand-ins for the Windows types,
// so the two calls are declared here rather than through windows.h.
#define MAX_PATH 260
#define FILE_ATTRIBUTE_DIRECTORY 0x10
extern "C" __declspec(dllimport) unsigned long __stdcall GetModuleFileNameA(void *module, char *path, unsigned long size);
extern "C" __declspec(dllimport) unsigned long __stdcall GetFileAttributesA(const char *path);

// Defined in another unit: the player's own folder, for the persistent
// files or the cache.
std::string __fastcall GetUserFolder(int persistent);

std::string GetSaveFolder(int type)
{
    if (type != 4)
        return GetUserFolder(1);
    return GetUserFolder(0);
}

std::string GetExeFolder();

std::string g_exeFolder = GetExeFolder();

// The folder the executable is in, ending in a backslash.
std::string GetExeFolder()
{
    char path[MAX_PATH];
    GetModuleFileNameA(0, path, MAX_PATH);
    char *slash = strrchr(path, '\\');
    if (slash)
        slash[1] = 0;
    g_exeFolder = path;
    return g_exeFolder;
}

// The path with forward slashes, and no two together.
static std::string FixSlashes(const std::string &path)
{
    std::string fixed = path;
    while (fixed.find("\\") != std::string::npos)
        fixed.replace(fixed.find("\\"), 1, "/");
    while (fixed.find("//") != std::string::npos)
        fixed.replace(fixed.find("//"), 2, "/");
    return fixed;
}

void CreateFolder(const std::string &folder)
{
    std::string path = folder;
    path = FixSlashes(path);
    // A file's path: make the folder it is in.
    if (path.find(".") != std::string::npos)
        path = path.substr(0, path.find_last_of("/"));
    unsigned long attributes = GetFileAttributesA(path.c_str());
    if (attributes != FILE_ATTRIBUTE_DIRECTORY && attributes != 0) {
        for (unsigned int i = 1; i < path.size(); i++) {
            if (path[i] == '/')
                _mkdir(path.substr(0, i).c_str());
        }
        _mkdir(path.c_str());
    }
}

static inline float Dot(const V3 &a, const V3 &b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

float SEGMENT3::DistanceSq(const V3 &point, float *t)
{
    V3 d = point - start;
    float dot = Dot(d, dir);
    float u;
    if (dot > 0.0f) {
        float lengthSq = Dot(dir, dir);
        if (dot < lengthSq) {
            u = dot / lengthSq;
            d = d - u * dir;
        } else {
            u = 1.0f;
            d = d - dir;
        }
    } else {
        u = 0.0f;
    }
    if (t)
        *t = u;
    return Dot(d, d);
}

V3 SEGMENT3::PointAt(const float &t)
{
    return start + dir * t;
}

float SEGMENT3::Distance(const V3 &point, float *t)
{
    return (float)sqrt(DistanceSq(point, t));
}

float SEGMENT3::Project(const V3 &point, float *distance)
{
    V3 d = point - start;
    float t = Dot(d, dir) / Dot(dir, dir);
    d = d - t * dir;
    if (distance)
        *distance = d.Length();
    return t;
}

// The squared distance between the segments, with the closest points on
// each when the pointers are set: Eberly's, with unit directions.
float SEGMENT3::SqrDistance(const SEGMENT3 &other, V3 *pa, V3 *pb)
{
    const float fA00 = 1.0f;
    const float fA11 = 1.0f;
    V3 kDiff = GetStart() - other.GetStart();
    V3 kDir1 = other.dir;
    float fA01 = -Dot(GetDirection(), kDir1);
    float fB0 = Dot(GetDirection(), kDiff);
    float fC = Dot(kDiff, kDiff);
    float fDet = (float)fabs(fA00 * fA11 - fA01 * fA01);
    float fB1, fS, fT, fSqrDist, fTmp;

    if (fDet >= 1e-06f) {
        // The segments are not parallel.
        fB1 = -Dot(other.GetDirection(), kDiff);
        fS = fA01 * fB1 - fA11 * fB0;
        fT = fA01 * fB0 - fA00 * fB1;

        if (fS >= 0.0f) {
            if (fS <= fDet) {
                if (fT >= 0.0f) {
                    if (fT <= fDet) {
                        float fInvDet = 1.0f / fDet;
                        fS *= fInvDet;
                        fT *= fInvDet;
                        fSqrDist = fS * (fA00 * fS + fA01 * fT + 2.0f * fB0) +
                                   fT * (fA01 * fS + fA11 * fT + 2.0f * fB1) + fC;
                    } else {
                        fT = 1.0f;
                        fTmp = fA01 + fB0;
                        if (fTmp >= 0.0f) {
                            fS = 0.0f;
                            fSqrDist = fA11 + 2.0f * fB1 + fC;
                        } else if (-fTmp >= fA00) {
                            fS = 1.0f;
                            fSqrDist = fA00 + fA11 + fC + 2.0f * (fB1 + fTmp);
                        } else {
                            fS = -fTmp / fA00;
                            fSqrDist = fTmp * fS + fA11 + 2.0f * fB1 + fC;
                        }
                    }
                } else {
                    fT = 0.0f;
                    if (fB0 >= 0.0f) {
                        fS = 0.0f;
                        fSqrDist = fC;
                    } else if (-fB0 >= fA00) {
                        fS = 1.0f;
                        fSqrDist = fA00 + 2.0f * fB0 + fC;
                    } else {
                        fS = -fB0 / fA00;
                        fSqrDist = fB0 * fS + fC;
                    }
                }
            } else {
                if (fT >= 0.0f) {
                    if (fT <= fDet) {
                        fS = 1.0f;
                        fTmp = fA01 + fB1;
                        if (fTmp >= 0.0f) {
                            fT = 0.0f;
                            fSqrDist = fA00 + 2.0f * fB0 + fC;
                        } else if (-fTmp >= fA11) {
                            fT = 1.0f;
                            fSqrDist = fA00 + fA11 + fC + 2.0f * (fB0 + fTmp);
                        } else {
                            fT = -fTmp / fA11;
                            fSqrDist = fTmp * fT + fA00 + 2.0f * fB0 + fC;
                        }
                    } else {
                        fTmp = fA01 + fB0;
                        if (-fTmp <= fA00) {
                            fT = 1.0f;
                            if (fTmp >= 0.0f) {
                                fS = 0.0f;
                                fSqrDist = fA11 + 2.0f * fB1 + fC;
                            } else {
                                fS = -fTmp / fA00;
                                fSqrDist = fTmp * fS + fA11 + 2.0f * fB1 + fC;
                            }
                        } else {
                            fS = 1.0f;
                            fTmp = fA01 + fB1;
                            if (fTmp >= 0.0f) {
                                fT = 0.0f;
                                fSqrDist = fA00 + 2.0f * fB0 + fC;
                            } else if (-fTmp >= fA11) {
                                fT = 1.0f;
                                fSqrDist = fA00 + fA11 + fC + 2.0f * (fB0 + fTmp);
                            } else {
                                fT = -fTmp / fA11;
                                fSqrDist = fTmp * fT + fA00 + 2.0f * fB0 + fC;
                            }
                        }
                    }
                } else {
                    if (-fB0 < fA00) {
                        fT = 0.0f;
                        if (fB0 >= 0.0f) {
                            fS = 0.0f;
                            fSqrDist = fC;
                        } else {
                            fS = -fB0 / fA00;
                            fSqrDist = fB0 * fS + fC;
                        }
                    } else {
                        fS = 1.0f;
                        fTmp = fA01 + fB1;
                        if (fTmp >= 0.0f) {
                            fT = 0.0f;
                            fSqrDist = fA00 + 2.0f * fB0 + fC;
                        } else if (-fTmp >= fA11) {
                            fT = 1.0f;
                            fSqrDist = fA00 + fA11 + fC + 2.0f * (fB0 + fTmp);
                        } else {
                            fT = -fTmp / fA11;
                            fSqrDist = fTmp * fT + fA00 + 2.0f * fB0 + fC;
                        }
                    }
                }
            }
        } else {
            if (fT >= 0.0f) {
                if (fT <= fDet) {
                    fS = 0.0f;
                    if (fB1 >= 0.0f) {
                        fT = 0.0f;
                        fSqrDist = fC;
                    } else if (-fB1 >= fA11) {
                        fT = 1.0f;
                        fSqrDist = fA11 + 2.0f * fB1 + fC;
                    } else {
                        fT = -fB1 / fA11;
                        fSqrDist = fB1 * fT + fC;
                    }
                } else {
                    fTmp = fA01 + fB0;
                    if (fTmp < 0.0f) {
                        fT = 1.0f;
                        if (-fTmp >= fA00) {
                            fS = 1.0f;
                            fSqrDist = fA00 + fA11 + fC + 2.0f * (fB1 + fTmp);
                        } else {
                            fS = -fTmp / fA00;
                            fSqrDist = fTmp * fS + fA11 + 2.0f * fB1 + fC;
                        }
                    } else {
                        fS = 0.0f;
                        if (fB1 >= 0.0f) {
                            fT = 0.0f;
                            fSqrDist = fC;
                        } else if (-fB1 >= fA11) {
                            fT = 1.0f;
                            fSqrDist = fA11 + 2.0f * fB1 + fC;
                        } else {
                            fT = -fB1 / fA11;
                            fSqrDist = fB1 * fT + fC;
                        }
                    }
                }
            } else {
                if (fB0 < 0.0f) {
                    fT = 0.0f;
                    if (-fB0 >= fA00) {
                        fS = 1.0f;
                        fSqrDist = fA00 + 2.0f * fB0 + fC;
                    } else {
                        fS = -fB0 / fA00;
                        fSqrDist = fB0 * fS + fC;
                    }
                } else {
                    fS = 0.0f;
                    if (fB1 >= 0.0f) {
                        fT = 0.0f;
                        fSqrDist = fC;
                    } else if (-fB1 >= fA11) {
                        fT = 1.0f;
                        fSqrDist = fA11 + 2.0f * fB1 + fC;
                    } else {
                        fT = -fB1 / fA11;
                        fSqrDist = fB1 * fT + fC;
                    }
                }
            }
        }
    } else {
        // The segments are parallel.
        if (fA01 > 0.0f) {
            // The directions form an obtuse angle.
            if (fB0 >= 0.0f) {
                fS = 0.0f;
                fT = 0.0f;
                fSqrDist = fC;
            } else if (-fB0 <= fA00) {
                fS = -fB0 / fA00;
                fT = 0.0f;
                fSqrDist = fB0 * fS + fC;
            } else {
                fB1 = -Dot(kDiff, kDir1.Normalized());
                fS = 1.0f;
                fTmp = fA00 + fB0;
                if (-fTmp >= fA01) {
                    fT = 1.0f;
                    fSqrDist = fA00 + fA11 + fC + 2.0f * (fA01 + fB0 + fB1);
                } else {
                    fT = -fTmp / fA01;
                    fSqrDist = fA00 + 2.0f * fB0 + fC + fT * (fA11 * fT + 2.0f * (fA01 + fB1));
                }
            }
        } else {
            // The directions form an acute angle.
            if (-fB0 >= fA00) {
                fS = 1.0f;
                fT = 0.0f;
                fSqrDist = fA00 + 2.0f * fB0 + fC;
            } else if (fB0 <= 0.0f) {
                fS = -fB0 / fA00;
                fT = 0.0f;
                fSqrDist = fB0 * fS + fC;
            } else {
                fB1 = -Dot(kDiff, kDir1.Normalized());
                fS = 0.0f;
                if (fB0 >= -fA01) {
                    fT = 1.0f;
                    fSqrDist = fA11 + 2.0f * fB1 + fC;
                } else {
                    fT = -fB0 / fA01;
                    fSqrDist = fC + fT * (2.0f * fB1 + fA11 * fT);
                }
            }
        }
    }

    if (pa)
        *pa = GetStart() + GetDirection() * fS;
    if (pb)
        *pb = other.GetStart() + other.GetDirection() * fT;
    return (float)fabs(fSqrDist);
}

void SEGMENT3::Update()
{
    dir = end - start;
    length = dir.Length();
}

bool CollideQuadSegment(QUAD3 *a, SEGMENT3 *b, V3 *pa, V3 *pb, float *depth, V3 *normal)
{
    if (!a || !b)
        return false;
    if (pa)
        *pa = a->GetNormal();
    float d0 = a->PlaneDot(b->GetStart());
    float d1 = a->PlaneDot(b->GetEnd());
    if ((d0 > 0.0f && d1 > 0.0f) || (d0 < 0.0f && d1 < 0.0f)) {
        if (depth) {
            float near0 = a->Distance(b->GetStart(), 0, 0);
            float near1 = a->Distance(b->GetEnd(), 0, 0);
            if (near0 < near1)
                *depth = near0;
            else
                *depth = near1;
        }
        return false;
    }
    V3 hit = a->Intersect(b->GetStart(), b->GetEnd(), 0);
    if (normal)
        *normal = hit;
    V3 *c = a->GetCorners();
    if ((hit.x - c[0].x) * (c[1].x - c[0].x) + (hit.y - c[0].y) * (c[1].y - c[0].y) +
                (hit.z - c[0].z) * (c[1].z - c[0].z) >= 0.0f &&
        (hit.x - c[0].x) * (c[2].x - c[0].x) + (hit.y - c[0].y) * (c[2].y - c[0].y) +
                (hit.z - c[0].z) * (c[2].z - c[0].z) >= 0.0f &&
        (hit.x - c[3].x) * (c[1].x - c[3].x) + (hit.y - c[3].y) * (c[1].y - c[3].y) +
                (hit.z - c[3].z) * (c[1].z - c[3].z) >= 0.0f &&
        (hit.x - c[3].x) * (c[2].x - c[3].x) + (hit.y - c[3].y) * (c[2].y - c[3].y) +
                (hit.z - c[3].z) * (c[2].z - c[3].z) >= 0.0f)
        return true;
    return false;
}

bool CollideSphereSegment(SPHERE3 *a, SEGMENT3 *b, V3 *pa, V3 *pb, float *depth, V3 *normal)
{
    float t;
    float distance = b->DistanceSq(a->GetCenter(), &t);
    if (normal)
        *normal = b->PointAt(t);
    if (depth)
        *depth = distance;
    if (pa)
        *pa = b->PointAt(t) - a->center;
    return distance <= a->GetRadiusSq();
}

bool CollideCapsuleCapsule(CAPSULE3 *a, CAPSULE3 *b, V3 *pa, V3 *pb, float *depth, V3 *normal)
{
    return (float)sqrt(a->GetSegment().SqrDistance(b->GetSegment(), 0, 0)) <= a->radius + b->radius;
}

bool CollideCapsuleSphere(CAPSULE3 *a, SPHERE3 *b, V3 *pa, V3 *pb, float *depth, V3 *normal)
{
    float distance = a->GetSegment().Distance(b->GetCenter(), 0);
    if (depth)
        *depth = distance;
    if (distance > b->radius + a->radius)
        return false;
    return true;
}

bool CollideCapsuleBox(CAPSULE3 *a, OOBOX3 *b, V3 *pa, V3 *pb, float *depth, V3 *normal)
{
    float distance = a->GetSegment().SqrDistance(b);
    if (depth)
        *depth = (float)sqrt(distance);
    if (distance > 0.0f)
        return false;
    return true;
}

// Whether the box min-max holds the box innerMin-innerMax.
bool BoxInside(const V3 &min, const V3 &max, const V3 &innerMin, const V3 &innerMax)
{
    return innerMax.x <= max.x && innerMax.y <= max.y && innerMax.z <= max.z && innerMin.x >= min.x &&
           innerMin.y >= min.y && innerMin.z >= min.z;
}

bool CollideSphereSphere(SPHERE3 *a, SPHERE3 *b, V3 *pa, V3 *pb, float *depth)
{
    if (!a || !b)
        return false;
    V3 ca = a->GetCenter();
    V3 cb = b->GetCenter();
    float distanceSq = (cb.x - ca.x) * (cb.x - ca.x) + (cb.y - ca.y) * (cb.y - ca.y) + (cb.z - ca.z) * (cb.z - ca.z);
    float reachSq = (a->radius + b->radius) * (a->radius + b->radius);
    if (pa || pb) {
        V3 n = (cb - ca).Normalized();
        if (pa)
            *pa = n * -1.0f;
        if (pb)
            *pb = n;
    }
    if (depth)
        *depth = (float)sqrt(distanceSq) - (float)sqrt(reachSq);
    if (distanceSq < reachSq)
        return true;
    return false;
}

V3 OOBOX3::GetUp()
{
    return angles.GetUpPoint();
}

V3 OOBOX3::GetDirection()
{
    return angles.GetDirection();
}

V3 OOBOX3::GetRight()
{
    return angles.GetRightPoint();
}

MATRIX OOBOX3::GetMatrix()
{
    return angles.GetMatrix();
}

V3 QUAD3::GetUp()
{
    return angles.GetUpPoint();
}

V3 QUAD3::GetDirection()
{
    return angles.GetDirection();
}

V3 QUAD3::GetRight()
{
    return angles.GetRightPoint();
}


// The separating axis test between two oriented boxes, each given by its
// center, axes and half sizes, after Eberly.
#define BOXES_TOUCH(ca, akA, ea, cb, akB, eb)                                                   \
    const float cutoff = 1.0f - 1e-06f;                                                         \
    bool parallel = false;                                                                      \
    V3 d = cb - ca;                                                                             \
    float c[3][3];                                                                              \
    float absC[3][3];                                                                           \
    float ad[3];                                                                                \
    float r0, r1, r, r01;                                                                       \
    int i;                                                                                      \
    for (i = 0; i < 3; i++) {                                                                   \
        c[0][i] = Dot(akA[0], akB[i]);                                                          \
        absC[0][i] = (float)fabs(c[0][i]);                                                      \
        if (absC[0][i] > cutoff)                                                                \
            parallel = true;                                                                    \
    }                                                                                           \
    ad[0] = Dot(akA[0], d);                                                                     \
    r = (float)fabs(ad[0]);                                                                     \
    r1 = eb.x * absC[0][0] + eb.y * absC[0][1] + eb.z * absC[0][2];                             \
    r01 = ea.x + r1;                                                                            \
    if (r > r01)                                                                                \
        return false;                                                                           \
    for (i = 0; i < 3; i++) {                                                                   \
        c[1][i] = Dot(akA[1], akB[i]);                                                          \
        absC[1][i] = (float)fabs(c[1][i]);                                                      \
        if (absC[1][i] > cutoff)                                                                \
            parallel = true;                                                                    \
    }                                                                                           \
    ad[1] = Dot(akA[1], d);                                                                     \
    r = (float)fabs(ad[1]);                                                                     \
    r1 = eb.x * absC[1][0] + eb.y * absC[1][1] + eb.z * absC[1][2];                             \
    r01 = ea.y + r1;                                                                            \
    if (r > r01)                                                                                \
        return false;                                                                           \
    for (i = 0; i < 3; i++) {                                                                   \
        c[2][i] = Dot(akA[2], akB[i]);                                                          \
        absC[2][i] = (float)fabs(c[2][i]);                                                      \
        if (absC[2][i] > cutoff)                                                                \
            parallel = true;                                                                    \
    }                                                                                           \
    ad[2] = Dot(akA[2], d);                                                                     \
    r = (float)fabs(ad[2]);                                                                     \
    r1 = eb.x * absC[2][0] + eb.y * absC[2][1] + eb.z * absC[2][2];                             \
    r01 = ea.z + r1;                                                                            \
    if (r > r01)                                                                                \
        return false;                                                                           \
    r = (float)fabs(Dot(akB[0], d));                                                            \
    r0 = ea.x * absC[0][0] + ea.y * absC[1][0] + ea.z * absC[2][0];                             \
    r01 = r0 + eb.x;                                                                            \
    if (r > r01)                                                                                \
        return false;                                                                           \
    r = (float)fabs(Dot(akB[1], d));                                                            \
    r0 = ea.x * absC[0][1] + ea.y * absC[1][1] + ea.z * absC[2][1];                             \
    r01 = r0 + eb.y;                                                                            \
    if (r > r01)                                                                                \
        return false;                                                                           \
    r = (float)fabs(Dot(akB[2], d));                                                            \
    r0 = ea.x * absC[0][2] + ea.y * absC[1][2] + ea.z * absC[2][2];                             \
    r01 = r0 + eb.z;                                                                            \
    if (r > r01)                                                                                \
        return false;                                                                           \
    /* A pair of axes lines up: the face tests were enough. */                                  \
    if (parallel)                                                                               \
        return true;                                                                            \
    r = (float)fabs(ad[2] * c[1][0] - ad[1] * c[2][0]);                                         \
    r0 = ea.y * absC[2][0] + ea.z * absC[1][0];                                                 \
    r1 = eb.y * absC[0][2] + eb.z * absC[0][1];                                                 \
    r01 = r0 + r1;                                                                              \
    if (r > r01)                                                                                \
        return false;                                                                           \
    r = (float)fabs(ad[2] * c[1][1] - ad[1] * c[2][1]);                                         \
    r0 = ea.y * absC[2][1] + ea.z * absC[1][1];                                                 \
    r1 = eb.x * absC[0][2] + eb.z * absC[0][0];                                                 \
    r01 = r0 + r1;                                                                              \
    if (r > r01)                                                                                \
        return false;                                                                           \
    r = (float)fabs(ad[2] * c[1][2] - ad[1] * c[2][2]);                                         \
    r0 = ea.y * absC[2][2] + ea.z * absC[1][2];                                                 \
    r1 = eb.x * absC[0][1] + eb.y * absC[0][0];                                                 \
    r01 = r0 + r1;                                                                              \
    if (r > r01)                                                                                \
        return false;                                                                           \
    r = (float)fabs(ad[0] * c[2][0] - ad[2] * c[0][0]);                                         \
    r0 = ea.x * absC[2][0] + ea.z * absC[0][0];                                                 \
    r1 = eb.y * absC[1][2] + eb.z * absC[1][1];                                                 \
    r01 = r0 + r1;                                                                              \
    if (r > r01)                                                                                \
        return false;                                                                           \
    r = (float)fabs(ad[0] * c[2][1] - ad[2] * c[0][1]);                                         \
    r0 = ea.x * absC[2][1] + ea.z * absC[0][1];                                                 \
    r1 = eb.x * absC[1][2] + eb.z * absC[1][0];                                                 \
    r01 = r0 + r1;                                                                              \
    if (r > r01)                                                                                \
        return false;                                                                           \
    r = (float)fabs(ad[0] * c[2][2] - ad[2] * c[0][2]);                                         \
    r0 = ea.x * absC[2][2] + ea.z * absC[0][2];                                                 \
    r1 = eb.x * absC[1][1] + eb.y * absC[1][0];                                                 \
    r01 = r0 + r1;                                                                              \
    if (r > r01)                                                                                \
        return false;                                                                           \
    r = (float)fabs(ad[1] * c[0][0] - ad[0] * c[1][0]);                                         \
    r0 = ea.x * absC[1][0] + ea.y * absC[0][0];                                                 \
    r1 = eb.y * absC[2][2] + eb.z * absC[2][1];                                                 \
    r01 = r0 + r1;                                                                              \
    if (r > r01)                                                                                \
        return false;                                                                           \
    r = (float)fabs(ad[1] * c[0][1] - ad[0] * c[1][1]);                                         \
    r0 = ea.x * absC[1][1] + ea.y * absC[0][1];                                                 \
    r1 = eb.x * absC[2][2] + eb.z * absC[2][0];                                                 \
    r01 = r0 + r1;                                                                              \
    if (r > r01)                                                                                \
        return false;                                                                           \
    r = (float)fabs(ad[1] * c[0][2] - ad[0] * c[1][2]);                                         \
    r0 = ea.x * absC[1][2] + ea.y * absC[0][2];                                                 \
    r1 = eb.x * absC[2][1] + eb.y * absC[2][0];                                                 \
    r01 = r0 + r1;                                                                              \
    if (r > r01)                                                                                \
        return false;                                                                           \
    return true;

bool CollideBoxBox(OOBOX3 *a, OOBOX3 *b, V3 *pa, V3 *pb, float *depth)
{
    V3 akA[3];
    akA[0] = a->angles.GetRightPoint();
    akA[1] = a->angles.GetUpPoint();
    akA[2] = a->angles.GetDirection();
    V3 akB[3];
    akB[0] = b->angles.GetRightPoint();
    akB[1] = b->angles.GetUpPoint();
    akB[2] = b->angles.GetDirection();
    V3 ea = a->GetSize() * 0.5f;
    V3 eb = b->GetSize() * 0.5f;
    BOXES_TOUCH(a->GetCenter(), akA, ea, b->GetCenter(), akB, eb)
}

// Whether a sphere radius r away along d touches a box with the axes and
// half sizes.
#define SPHERE_TOUCHES_BOX(d, axes, ex, ey, ez, r)                                              \
    float x = (float)fabs(Dot(d, axes[0]));                                                     \
    float y = (float)fabs(Dot(d, axes[1]));                                                     \
    float z = (float)fabs(Dot(d, axes[2]));                                                     \
    float dx = x - ex;                                                                          \
    float dy = y - ey;                                                                          \
    float dz = z - ez;                                                                          \
    if (x <= ex) {                                                                              \
        if (y <= ey) {                                                                          \
            if (z <= ez)                                                                        \
                return true;                                                                    \
            return dz <= r;                                                                     \
        }                                                                                       \
        if (z <= ez)                                                                            \
            return dy <= r;                                                                     \
        return dy * dy + dz * dz <= r * r;                                                      \
    }                                                                                           \
    if (y <= ey) {                                                                              \
        if (z <= ez)                                                                            \
            return dx <= r;                                                                     \
        return dx * dx + dz * dz <= r * r;                                                      \
    }                                                                                           \
    if (z <= ez)                                                                                \
        return dx * dx + dy * dy <= r * r;                                                      \
    return dx * dx + dy * dy + dz * dz <= r * r;

bool CollideSphereBox(SPHERE3 *a, OOBOX3 *b, V3 *pa, V3 *pb, float *depth)
{
    if (!a || !b)
        return false;
    b->GetPlanes();
    V3 d = a->GetCenter() - b->GetCenter();
    if (pa || pb)
        MATRIX m = b->GetMatrix();
    PRIMV3 axes[3];
    axes[0] = b->GetRight();
    axes[1] = b->GetUp();
    axes[2] = b->GetDirection();
    V3 size = b->GetSize();
    float ex = size.x * 0.5f;
    float ey = size.y * 0.5f;
    float ez = size.z * 0.5f;
    SPHERE_TOUCHES_BOX(d, axes, ex, ey, ez, a->radius)
}

bool CollideSphereQuad(SPHERE3 *a, QUAD3 *b, V3 *pa, V3 *pb, float *depth)
{
    if (!a || !b)
        return false;
    V3 d = a->GetCenter() - b->GetCenter();
    PRIMV3 axes[3];
    axes[0] = b->GetRight();
    axes[1] = b->GetUp();
    axes[2] = b->GetDirection();
    V3 size = b->GetSize();
    float ex = size.x * 0.5f;
    float ez = size.z * 0.5f;
    SPHERE_TOUCHES_BOX(d, axes, ex, 0.0001f, ez, a->radius)
}

bool CollideBoxSegment(OOBOX3 *a, SEGMENT3 *b, V3 *pa, V3 *pb, float *depth, V3 *normal)
{
    V3 start = b->start - a->center;
    V3 end = b->end - a->center;
    MATRIX m = a->angles.GetMatrix();
    m.Invert();
    MATRIX inverse = m;
    start = start.Transform(inverse);
    end = end.Transform(inverse);
    float hx = a->size.x * 0.5f;
    float hy = a->size.y * 0.5f;
    bool startInside = false;
    bool endInside = false;
    float hz = a->size.z * 0.5f;
    if (start.x <= hx && start.x >= -hx && start.y <= hy && start.y >= -hy && start.z <= hz && start.z >= -hz)
        startInside = true;
    if (end.x <= hx && end.x >= -hx && end.y <= hy && end.y >= -hy && end.z <= hz && end.z >= -hz)
        endInside = true;
    if (startInside || endInside) {
        if (depth)
            *depth = 0.0f;
        return true;
    }

    a->GetPlanes();
    unsigned int hits = 0;
    PRIMV3 first;
    PRIMV3 second;
    PRIMV3 at;
    PRIMV3 firstNormal;
    PRIMV3 secondNormal;
    for (int i = 0; i < 6; i++) {
        if (a->IntersectFace(i, b->GetStart(), b->GetEnd(), &at)) {
            if (hits) {
                second = at;
                hits++;
                PLANE *planes = a->GetPlanes();
                secondNormal = V3(planes[i].a, planes[i].b, planes[i].c);
                break;
            }
            first = at;
            hits = 1;
            PLANE *planes = a->GetPlanes();
            firstNormal = V3(planes[i].a, planes[i].b, planes[i].c);
        }
    }
    if (hits > 1) {
        if (depth || normal) {
            float d0 = b->Distance(first, 0);
            float d1 = b->Distance(second, 0);
            if (d0 <= d1) {
                d1 = d0;
                second = first;
                secondNormal = firstNormal;
            }
            if (depth)
                *depth = d1;
            if (normal)
                *normal = second;
        } else {
            secondNormal = V3(0.0f, 0.0f, 0.0f);
        }
        if (pa)
            *pa = secondNormal;
        return true;
    }
    return false;
}

bool CollideBoxQuad(OOBOX3 *a, QUAD3 *b, V3 *pa, V3 *pb, float *depth, int flags)
{
    V3 akA[3];
    akA[0] = a->angles.GetRightPoint();
    akA[1] = a->angles.GetUpPoint();
    akA[2] = a->angles.GetDirection();
    V3 akB[3];
    akB[0] = b->GetRight();
    akB[1] = b->GetUp();
    akB[2] = b->GetDirection();
    V3 ea = a->GetSize() * 0.5f;
    V3 eb(b->size.x * 0.5f, 0.0001f, b->size.z * 0.5f);
    BOXES_TOUCH(a->GetCenter(), akA, ea, b->center, akB, eb)
}

void CAPSULE3::Set(const V3 &center, const float &radius_, const float &height, const ANG &angles)
{
    radius = radius_;
    dirty = true;
    float top = height * 0.5f;
    float bottom = height * -0.5f;
    V3 up;
    V3 down;
    up = ((ANG &)angles).Transform(V3(0.0f, top, 0.0f));
    down = ((ANG &)angles).Transform(V3(0.0f, bottom, 0.0f));
    GetSegmentRef().Set(up + center, down + center);
}

void QUAD3::SetCenter(const V3 &center_)
{
    if (center.x != center_.x || center.y != center_.y || center.z != center_.z) {
        center = center_;
        planeDirty = true;
        cornersDirty = true;
    }
}

void QUAD3::SetSize(const V3 &size_)
{
    if (size.x != size_.x || size.y != size_.y || size.z != size_.z) {
        size = size_;
        cornersDirty = true;
        planeDirty = true;
    }
}

void QUAD3::SetAngles(const ANG &angles_)
{
    if (angles != angles_) {
        angles = angles_;
        cornersDirty = true;
        planeDirty = true;
    }
}

void QUAD3::UpdateCorners()
{
    V3 half = GetSize();
    float hx = half.x * 0.5f;
    float hz = half.z * 0.5f;
    corners[0].Set(-hx, 0.0f, -hz);
    corners[1].Set(hx, 0.0f, -hz);
    corners[2].Set(-hx, 0.0f, hz);
    corners[3].Set(hx, 0.0f, hz);
    corners[0] = center + angles.Transform(corners[0]);
    corners[1] = center + angles.Transform(corners[1]);
    corners[2] = center + angles.Transform(corners[2]);
    corners[3] = center + angles.Transform(corners[3]);
    planeDirty = true;
    cornersDirty = false;
}

V3 QUAD3::GetNormal()
{
    UpdatePlane();
    return plane.n;
}

float QUAD3::PlaneDot(const V3 &point)
{
    UpdatePlane();
    return point.x * plane.n.x + point.y * plane.n.y + point.z * plane.n.z + plane.d;
}

float QUAD3::Distance(const V3 &point, V3 *at, V3 *onPlane)
{
    UpdatePlane();
    return ((PLANE &)plane).Distance(point, at, onPlane);
}

V3 QUAD3::Intersect(const V3 &a, const V3 &b, float *t)
{
    UpdatePlane();
    return ((PLANE &)plane).Intersect(a, b, t);
}

V3 *QUAD3::GetCorners()
{
    if (cornersDirty)
        UpdateCorners();
    return corners;
}

void OOBOX3::SetCenter(const V3 &center_)
{
    if (center.x != center_.x || center.y != center_.y || center.z != center_.z) {
        center = center_;
        planesDirty = true;
        cornersDirty = true;
    }
}

void OOBOX3::SetSize(const V3 &size_)
{
    if (size.x != size_.x || size.y != size_.y || size.z != size_.z) {
        size = size_;
        cornersDirty = true;
        planesDirty = true;
    }
}

void OOBOX3::SetAngles(const ANG &angles_)
{
    if (angles != angles_) {
        angles = angles_;
        cornersDirty = true;
        planesDirty = true;
    }
}

void OOBOX3::UpdateCorners()
{
    float hx = size.x * 0.5f;
    float hy = size.y * 0.5f;
    float hz = size.z * 0.5f;
    corners[0].Set(-hx, -hy, -hz);
    corners[1].Set(hx, -hy, -hz);
    corners[2].Set(-hx, -hy, hz);
    corners[3].Set(hx, -hy, hz);
    corners[4].Set(-hx, hy, -hz);
    corners[5].Set(hx, hy, -hz);
    corners[6].Set(-hx, hy, hz);
    corners[7].Set(hx, hy, hz);
    corners[0] = angles.Transform(corners[0]) + center;
    corners[1] = angles.Transform(corners[1]) + center;
    corners[2] = angles.Transform(corners[2]) + center;
    corners[3] = angles.Transform(corners[3]) + center;
    corners[4] = angles.Transform(corners[4]) + center;
    corners[5] = angles.Transform(corners[5]) + center;
    corners[6] = angles.Transform(corners[6]) + center;
    corners[7] = angles.Transform(corners[7]) + center;
    planesDirty = true;
    cornersDirty = false;
}

void OOBOX3::UpdatePlanes()
{
    if (cornersDirty)
        UpdateCorners();
    Plane(0).Set(corners[0], corners[1], corners[3]);
    Plane(1).Set(corners[7], corners[5], corners[4]);
    Plane(2).Set(corners[7], corners[6], corners[3]);
    Plane(3).Set(corners[0], corners[4], corners[5]);
    Plane(4).Set(corners[2], corners[6], corners[4]);
    Plane(5).Set(corners[5], corners[7], corners[3]);
    planesDirty = false;
}

// Whether the segment a-b crosses the face, and where it crosses its plane.
bool OOBOX3::IntersectFace(int face, const V3 &a, const V3 &b, V3 *at)
{
    if (face < 0 || face > 6)
        return false;
    if (cornersDirty)
        UpdateCorners();
    if (planesDirty)
        UpdatePlanes();
    PRIMV3 c[4];
    PLANE &plane = Plane(face);
    float da = a.x * plane.a + plane.b * a.y + Plane(face).c * a.z + plane.d;
    float db = plane.a * b.x + plane.b * b.y + plane.c * b.z + plane.d;
    if (da > 0.0f && db > 0.0f)
        return false;
    if (da < 0.0f && db < 0.0f)
        return false;
    switch (face) {
    case 0:
        c[0] = corners[0];
        c[1] = corners[1];
        c[2] = corners[2];
        c[3] = corners[3];
        break;
    case 1:
        c[0] = corners[4];
        c[1] = corners[5];
        c[2] = corners[6];
        c[3] = corners[7];
        break;
    case 2:
        c[0] = corners[2];
        c[1] = corners[3];
        c[2] = corners[6];
        c[3] = corners[7];
        break;
    case 3:
        c[0] = corners[0];
        c[1] = corners[1];
        c[2] = corners[4];
        c[3] = corners[5];
        break;
    case 4:
        c[0] = corners[0];
        c[1] = corners[2];
        c[2] = corners[4];
        c[3] = corners[6];
        break;
    case 5:
        c[0] = corners[1];
        c[1] = corners[3];
        c[2] = corners[5];
        c[3] = corners[7];
        break;
    }
    V3 hit = plane.Intersect(a, b, 0);
    if (at)
        *at = hit;
    if ((hit.x - c[0].x) * (c[1].x - c[0].x) + (hit.y - c[0].y) * (c[1].y - c[0].y) +
                (hit.z - c[0].z) * (c[1].z - c[0].z) >= 0.0f &&
        (hit.x - c[0].x) * (c[2].x - c[0].x) + (hit.y - c[0].y) * (c[2].y - c[0].y) +
                (hit.z - c[0].z) * (c[2].z - c[0].z) >= 0.0f &&
        (hit.x - c[3].x) * (c[1].x - c[3].x) + (hit.y - c[3].y) * (c[1].y - c[3].y) +
                (hit.z - c[3].z) * (c[1].z - c[3].z) >= 0.0f &&
        (hit.x - c[3].x) * (c[2].x - c[3].x) + (hit.y - c[3].y) * (c[2].y - c[3].y) +
                (hit.z - c[3].z) * (c[2].z - c[3].z) >= 0.0f)
        return true;
    return false;
}

V3 OOBOX3::GetAxis(int axis)
{
    switch (axis) {
    case 0:
        return GetRight();
    case 1:
        return GetUp();
    case 2:
        return GetDirection();
    }
    return GetDirection();
}

// The point of the box closest to point, with the point and that one
// written to the pointers that are set.
void OOBOX3::ClosestPoint(const V3 &point, V3 *from, V3 *to)
{
    V3 d = point - center;
    float along[3];
    along[0] = 0.0f;
    along[1] = 0.0f;
    along[2] = 0.0f;
    int i;
    for (i = 0; i < 3; i++) {
        V3 axis = i == 0 ? angles.GetRightPoint() : i == 1 ? angles.GetUpPoint() : angles.GetDirection();
        along[i] = Dot(axis, d);
        if (along[i] < -(&size.x)[i])
            along[i] = -(&size.x)[i];
        else if (along[i] > (&size.x)[i])
            along[i] = (&size.x)[i];
    }
    if (from)
        *from = point;
    if (to) {
        *to = center;
        for (i = 0; i < 3; i++) {
            V3 axis = i == 0 ? angles.GetRightPoint() : i == 1 ? angles.GetUpPoint() : angles.GetDirection();
            *to += axis * along[i];
        }
    }
}

V3 *OOBOX3::GetCorners()
{
    if (cornersDirty)
        UpdateCorners();
    return corners;
}

PLANE *OOBOX3::GetPlanes()
{
    if (planesDirty)
        UpdatePlanes();
    return (PLANE *)planes;
}

float EaseInOut(float t)
{
    if (t < 0.0f)
        t = 0.0f;
    else if (t > 1.0f)
        t = 1.0f;
    if (t > 0.5)
        t = 1.0f - t;
    return t * t * 4.0f;
}

// Rises past 1 and settles back as t goes from 0 to 1.
float EaseOvershoot(float t)
{
    if (t < 0.0f)
        t = 0.0f;
    else if (t > 1.0f)
        t = 1.0f;
    return (3.133f - 2.133f * t) * t;
}
