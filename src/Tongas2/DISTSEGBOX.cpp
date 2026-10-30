// The squared distance between a segment and a box: Eberly's line-box test
// (Wild Magic's DistLine3Box3) on the line the segment lies on, with the
// segment running from its start along dir for t from 0 to 1, and the
// box's point test at whichever end is closest when t falls outside that.
// The box's size is taken as its extents. Placeholder names.
#define ANG_INLINE_AXES
#define ANG_INLINE_RIGHT
#define FRUSTUM_MATHS
#define GEOMETRY_DEFINITIONS
#include "Tongas2/CPRIMITIVE3.h"

static inline float Dot(const V3 &a, const V3 &b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

static inline float &At(V3 &v, int i)
{
    return (&v.x)[i];
}

static inline const float &At(const V3 &v, int i)
{
    return (&v.x)[i];
}

class DISTSEGBOX
{
public:
    DISTSEGBOX(SEGMENT3 *segment, OOBOX3 *box) : m_box(box), m_segment(segment), m_closest0(0.0f, 0.0f, 0.0f), m_closest1(0.0f, 0.0f, 0.0f)
    {
        m_param = 0.0f;
    }

    float GetSquared();
    float GetLineSquared();

    void Face(int i0, int i1, int i2, V3 &pnt, const V3 &dir, const V3 &pme, float &sqrDistance);
    void CaseNoZeros(V3 &pnt, const V3 &dir, float &sqrDistance);
    void Case0(int i0, int i1, int i2, V3 &pnt, const V3 &dir, float &sqrDistance);
    void Case00(int i0, int i1, int i2, V3 &pnt, const V3 &dir, float &sqrDistance);
    void Case000(V3 &pnt, float &sqrDistance);

    // How far along the segment the closest point is, 0 to 1.
    float m_param;
    OOBOX3 *m_box;
    SEGMENT3 *m_segment;
    // The closest points on the segment and on the box.
    V3 m_closest0;
    V3 m_closest1;
};

void DISTSEGBOX::Face(int i0, int i1, int i2, V3 &pnt, const V3 &dir, const V3 &pme, float &sqrDistance)
{
    V3 ppe(0.0f, 0.0f, 0.0f);
    float lSqr, inv, tmp, param, t, delta;

    At(ppe, i1) = At(pnt, i1) + At(m_box->size, i1);
    At(ppe, i2) = At(pnt, i2) + At(m_box->size, i2);
    if (At(dir, i0) * At(ppe, i1) >= At(dir, i1) * At(pme, i0)) {
        if (At(dir, i0) * At(ppe, i2) >= At(dir, i2) * At(pme, i0)) {
            // v[i1] >= -e[i1], v[i2] >= -e[i2] (distance = 0)
            At(pnt, i0) = At(m_box->size, i0);
            inv = 1.0f / At(dir, i0);
            At(pnt, i1) -= At(dir, i1) * At(pme, i0) * inv;
            At(pnt, i2) -= At(dir, i2) * At(pme, i0) * inv;
            m_param = -At(pme, i0) * inv;
        } else {
            // v[i1] >= -e[i1], v[i2] < -e[i2]
            lSqr = At(dir, i0) * At(dir, i0) + At(dir, i2) * At(dir, i2);
            tmp = lSqr * At(ppe, i1) - At(dir, i1) * (At(dir, i0) * At(pme, i0) + At(dir, i2) * At(ppe, i2));
            if (tmp <= 2.0f * lSqr * At(m_box->size, i1)) {
                t = tmp / lSqr;
                lSqr += At(dir, i1) * At(dir, i1);
                tmp = At(ppe, i1) - t;
                delta = At(dir, i0) * At(pme, i0) + At(dir, i1) * tmp + At(dir, i2) * At(ppe, i2);
                param = -delta / lSqr;
                sqrDistance += At(pme, i0) * At(pme, i0) + tmp * tmp + At(ppe, i2) * At(ppe, i2) + delta * param;

                m_param = param;
                At(pnt, i0) = At(m_box->size, i0);
                At(pnt, i1) = t - At(m_box->size, i1);
                At(pnt, i2) = -At(m_box->size, i2);
            } else {
                lSqr += At(dir, i1) * At(dir, i1);
                delta = At(dir, i0) * At(pme, i0) + At(dir, i1) * At(pme, i1) + At(dir, i2) * At(ppe, i2);
                param = -delta / lSqr;
                sqrDistance += At(pme, i0) * At(pme, i0) + At(pme, i1) * At(pme, i1) + At(ppe, i2) * At(ppe, i2) + delta * param;

                m_param = param;
                At(pnt, i0) = At(m_box->size, i0);
                At(pnt, i1) = At(m_box->size, i1);
                At(pnt, i2) = -At(m_box->size, i2);
            }
        }
    } else {
        if (At(dir, i0) * At(ppe, i2) >= At(dir, i2) * At(pme, i0)) {
            // v[i1] < -e[i1], v[i2] >= -e[i2]
            lSqr = At(dir, i0) * At(dir, i0) + At(dir, i1) * At(dir, i1);
            tmp = lSqr * At(ppe, i2) - At(dir, i2) * (At(dir, i0) * At(pme, i0) + At(dir, i1) * At(ppe, i1));
            if (tmp <= 2.0f * lSqr * At(m_box->size, i2)) {
                t = tmp / lSqr;
                lSqr += At(dir, i2) * At(dir, i2);
                tmp = At(ppe, i2) - t;
                delta = At(dir, i0) * At(pme, i0) + At(dir, i1) * At(ppe, i1) + At(dir, i2) * tmp;
                param = -delta / lSqr;
                sqrDistance += At(pme, i0) * At(pme, i0) + At(ppe, i1) * At(ppe, i1) + tmp * tmp + delta * param;

                m_param = param;
                At(pnt, i0) = At(m_box->size, i0);
                At(pnt, i1) = -At(m_box->size, i1);
                At(pnt, i2) = t - At(m_box->size, i2);
            } else {
                lSqr += At(dir, i2) * At(dir, i2);
                delta = At(dir, i0) * At(pme, i0) + At(dir, i1) * At(ppe, i1) + At(dir, i2) * At(pme, i2);
                param = -delta / lSqr;
                sqrDistance += At(pme, i0) * At(pme, i0) + At(ppe, i1) * At(ppe, i1) + At(pme, i2) * At(pme, i2) + delta * param;

                m_param = param;
                At(pnt, i0) = At(m_box->size, i0);
                At(pnt, i1) = -At(m_box->size, i1);
                At(pnt, i2) = At(m_box->size, i2);
            }
        } else {
            // v[i1] < -e[i1], v[i2] < -e[i2]
            lSqr = At(dir, i0) * At(dir, i0) + At(dir, i2) * At(dir, i2);
            tmp = lSqr * At(ppe, i1) - At(dir, i1) * (At(dir, i0) * At(pme, i0) + At(dir, i2) * At(ppe, i2));
            if (tmp >= 0.0f) {
                // v[i1]-edge is closest
                if (tmp <= 2.0f * lSqr * At(m_box->size, i1)) {
                    t = tmp / lSqr;
                    lSqr += At(dir, i1) * At(dir, i1);
                    tmp = At(ppe, i1) - t;
                    delta = At(dir, i0) * At(pme, i0) + At(dir, i1) * tmp + At(dir, i2) * At(ppe, i2);
                    param = -delta / lSqr;
                    sqrDistance += At(pme, i0) * At(pme, i0) + tmp * tmp + At(ppe, i2) * At(ppe, i2) + delta * param;

                    m_param = param;
                    At(pnt, i0) = At(m_box->size, i0);
                    At(pnt, i1) = t - At(m_box->size, i1);
                    At(pnt, i2) = -At(m_box->size, i2);
                } else {
                    lSqr += At(dir, i1) * At(dir, i1);
                    delta = At(dir, i0) * At(pme, i0) + At(dir, i1) * At(pme, i1) + At(dir, i2) * At(ppe, i2);
                    param = -delta / lSqr;
                    sqrDistance += At(pme, i0) * At(pme, i0) + At(pme, i1) * At(pme, i1) + At(ppe, i2) * At(ppe, i2) + delta * param;

                    m_param = param;
                    At(pnt, i0) = At(m_box->size, i0);
                    At(pnt, i1) = At(m_box->size, i1);
                    At(pnt, i2) = -At(m_box->size, i2);
                }
                return;
            }

            lSqr = At(dir, i0) * At(dir, i0) + At(dir, i1) * At(dir, i1);
            tmp = lSqr * At(ppe, i2) - At(dir, i2) * (At(dir, i0) * At(pme, i0) + At(dir, i1) * At(ppe, i1));
            if (tmp >= 0.0f) {
                // v[i2]-edge is closest
                if (tmp <= 2.0f * lSqr * At(m_box->size, i2)) {
                    t = tmp / lSqr;
                    lSqr += At(dir, i2) * At(dir, i2);
                    tmp = At(ppe, i2) - t;
                    delta = At(dir, i0) * At(pme, i0) + At(dir, i1) * At(ppe, i1) + At(dir, i2) * tmp;
                    param = -delta / lSqr;
                    sqrDistance += At(pme, i0) * At(pme, i0) + At(ppe, i1) * At(ppe, i1) + tmp * tmp + delta * param;

                    m_param = param;
                    At(pnt, i0) = At(m_box->size, i0);
                    At(pnt, i1) = -At(m_box->size, i1);
                    At(pnt, i2) = t - At(m_box->size, i2);
                } else {
                    lSqr += At(dir, i2) * At(dir, i2);
                    delta = At(dir, i0) * At(pme, i0) + At(dir, i1) * At(ppe, i1) + At(dir, i2) * At(pme, i2);
                    param = -delta / lSqr;
                    sqrDistance += At(pme, i0) * At(pme, i0) + At(ppe, i1) * At(ppe, i1) + At(pme, i2) * At(pme, i2) + delta * param;

                    m_param = param;
                    At(pnt, i0) = At(m_box->size, i0);
                    At(pnt, i1) = -At(m_box->size, i1);
                    At(pnt, i2) = At(m_box->size, i2);
                }
                return;
            }

            // (v[i1],v[i2])-corner is closest
            lSqr += At(dir, i2) * At(dir, i2);
            delta = At(dir, i0) * At(pme, i0) + At(dir, i1) * At(ppe, i1) + At(dir, i2) * At(ppe, i2);
            param = -delta / lSqr;
            sqrDistance += At(pme, i0) * At(pme, i0) + At(ppe, i1) * At(ppe, i1) + At(ppe, i2) * At(ppe, i2) + delta * param;

            m_param = param;
            At(pnt, i0) = At(m_box->size, i0);
            At(pnt, i1) = -At(m_box->size, i1);
            At(pnt, i2) = -At(m_box->size, i2);
        }
    }
}

void DISTSEGBOX::CaseNoZeros(V3 &pnt, const V3 &dir, float &sqrDistance)
{
    V3 pme(pnt.x - m_box->size.x, pnt.y - m_box->size.y, pnt.z - m_box->size.z);

    float prodDxPy = dir.x * pme.y;
    float prodDyPx = dir.y * pme.x;
    float prodDzPx, prodDxPz, prodDzPy, prodDyPz;

    if (prodDyPx >= prodDxPy) {
        prodDzPx = dir.z * pme.x;
        prodDxPz = dir.x * pme.z;
        if (prodDzPx >= prodDxPz) {
            // line intersects x = e0
            Face(0, 1, 2, pnt, dir, pme, sqrDistance);
        } else {
            // line intersects z = e2
            Face(2, 0, 1, pnt, dir, pme, sqrDistance);
        }
    } else {
        prodDzPy = dir.z * pme.y;
        prodDyPz = dir.y * pme.z;
        if (prodDzPy >= prodDyPz) {
            // line intersects y = e1
            Face(1, 2, 0, pnt, dir, pme, sqrDistance);
        } else {
            // line intersects z = e2
            Face(2, 0, 1, pnt, dir, pme, sqrDistance);
        }
    }
}

void DISTSEGBOX::Case0(int i0, int i1, int i2, V3 &pnt, const V3 &dir, float &sqrDistance)
{
    float pmE0 = At(pnt, i0) - At(m_box->size, i0);
    float pmE1 = At(pnt, i1) - At(m_box->size, i1);
    float prod0 = At(dir, i1) * pmE0;
    float prod1 = At(dir, i0) * pmE1;
    float delta, invLSqr, inv;

    if (prod0 >= prod1) {
        // line intersects P[i0] = e[i0]
        At(pnt, i0) = At(m_box->size, i0);

        float ppE1 = At(pnt, i1) + At(m_box->size, i1);
        delta = prod0 - At(dir, i0) * ppE1;
        if (delta >= 0.0f) {
            invLSqr = 1.0f / (At(dir, i0) * At(dir, i0) + At(dir, i1) * At(dir, i1));
            sqrDistance += delta * delta * invLSqr;
            At(pnt, i1) = -At(m_box->size, i1);
            m_param = -(At(dir, i0) * pmE0 + At(dir, i1) * ppE1) * invLSqr;
        } else {
            inv = 1.0f / At(dir, i0);
            At(pnt, i1) -= prod0 * inv;
            m_param = -pmE0 * inv;
        }
    } else {
        // line intersects P[i1] = e[i1]
        At(pnt, i1) = At(m_box->size, i1);

        float ppE0 = At(pnt, i0) + At(m_box->size, i0);
        delta = prod1 - At(dir, i1) * ppE0;
        if (delta >= 0.0f) {
            invLSqr = 1.0f / (At(dir, i0) * At(dir, i0) + At(dir, i1) * At(dir, i1));
            sqrDistance += delta * delta * invLSqr;
            At(pnt, i0) = -At(m_box->size, i0);
            m_param = -(At(dir, i0) * ppE0 + At(dir, i1) * pmE1) * invLSqr;
        } else {
            inv = 1.0f / At(dir, i1);
            At(pnt, i0) -= prod1 * inv;
            m_param = -pmE1 * inv;
        }
    }

    if (At(pnt, i2) < -At(m_box->size, i2)) {
        delta = At(pnt, i2) + At(m_box->size, i2);
        sqrDistance += delta * delta;
        At(pnt, i2) = -At(m_box->size, i2);
    } else if (At(pnt, i2) > At(m_box->size, i2)) {
        delta = At(pnt, i2) - At(m_box->size, i2);
        sqrDistance += delta * delta;
        At(pnt, i2) = At(m_box->size, i2);
    }
}

void DISTSEGBOX::Case00(int i0, int i1, int i2, V3 &pnt, const V3 &dir, float &sqrDistance)
{
    float delta;

    m_param = (At(m_box->size, i0) - At(pnt, i0)) / At(dir, i0);

    At(pnt, i0) = At(m_box->size, i0);

    if (At(pnt, i1) < -At(m_box->size, i1)) {
        delta = At(pnt, i1) + At(m_box->size, i1);
        sqrDistance += delta * delta;
        At(pnt, i1) = -At(m_box->size, i1);
    } else if (At(pnt, i1) > At(m_box->size, i1)) {
        delta = At(pnt, i1) - At(m_box->size, i1);
        sqrDistance += delta * delta;
        At(pnt, i1) = At(m_box->size, i1);
    }

    if (At(pnt, i2) < -At(m_box->size, i2)) {
        delta = At(pnt, i2) + At(m_box->size, i2);
        sqrDistance += delta * delta;
        At(pnt, i2) = -At(m_box->size, i2);
    } else if (At(pnt, i2) > At(m_box->size, i2)) {
        delta = At(pnt, i2) - At(m_box->size, i2);
        sqrDistance += delta * delta;
        At(pnt, i2) = At(m_box->size, i2);
    }
}

void DISTSEGBOX::Case000(V3 &pnt, float &sqrDistance)
{
    float delta;

    if (At(pnt, 0) < -At(m_box->size, 0)) {
        delta = At(pnt, 0) + At(m_box->size, 0);
        sqrDistance += delta * delta;
        At(pnt, 0) = -At(m_box->size, 0);
    } else if (At(pnt, 0) > At(m_box->size, 0)) {
        delta = At(pnt, 0) - At(m_box->size, 0);
        sqrDistance += delta * delta;
        At(pnt, 0) = At(m_box->size, 0);
    }

    if (At(pnt, 1) < -At(m_box->size, 1)) {
        delta = At(pnt, 1) + At(m_box->size, 1);
        sqrDistance += delta * delta;
        At(pnt, 1) = -At(m_box->size, 1);
    } else if (At(pnt, 1) > At(m_box->size, 1)) {
        delta = At(pnt, 1) - At(m_box->size, 1);
        sqrDistance += delta * delta;
        At(pnt, 1) = At(m_box->size, 1);
    }

    if (At(pnt, 2) < -At(m_box->size, 2)) {
        delta = At(pnt, 2) + At(m_box->size, 2);
        sqrDistance += delta * delta;
        At(pnt, 2) = -At(m_box->size, 2);
    } else if (At(pnt, 2) > At(m_box->size, 2)) {
        delta = At(pnt, 2) - At(m_box->size, 2);
        sqrDistance += delta * delta;
        At(pnt, 2) = At(m_box->size, 2);
    }
}

float DISTSEGBOX::GetLineSquared()
{
    // The line in the box's coordinates.
    V3 diff = m_segment->GetStart() - m_box->GetCenter();
    V3 pnt(Dot(diff, m_box->angles.GetRightPoint()), Dot(diff, m_box->angles.GetUpPoint()), Dot(diff, m_box->angles.GetDirection()));
    V3 dir(Dot(m_segment->GetDir(), m_box->angles.GetRightPoint()), Dot(m_segment->GetDir(), m_box->angles.GetUpPoint()),
           Dot(m_segment->GetDir(), m_box->angles.GetDirection()));

    // Reflected so that the direction has no negative components.
    bool reflect[3];
    int i;
    for (i = 0; i < 3; i++) {
        if (At(dir, i) < 0.0f) {
            At(pnt, i) = -At(pnt, i);
            At(dir, i) = -At(dir, i);
            reflect[i] = true;
        } else {
            reflect[i] = false;
        }
    }

    float sqrDistance = 0.0f;
    m_param = 0.0f;

    if (dir.x > 0.0f) {
        if (dir.y > 0.0f) {
            if (dir.z > 0.0f)
                CaseNoZeros(pnt, dir, sqrDistance);
            else
                Case0(0, 1, 2, pnt, dir, sqrDistance);
        } else {
            if (dir.z > 0.0f)
                Case0(0, 2, 1, pnt, dir, sqrDistance);
            else
                Case00(0, 1, 2, pnt, dir, sqrDistance);
        }
    } else {
        if (dir.y > 0.0f) {
            if (dir.z > 0.0f)
                Case0(1, 2, 0, pnt, dir, sqrDistance);
            else
                Case00(1, 0, 2, pnt, dir, sqrDistance);
        } else {
            if (dir.z > 0.0f)
                Case00(2, 0, 1, pnt, dir, sqrDistance);
            else
                Case000(pnt, sqrDistance);
        }
    }

    m_closest0 = m_segment->GetStart() + m_segment->GetDir() * m_param;
    m_closest1 = m_box->GetCenter();
    for (i = 0; i < 3; i++) {
        if (reflect[i])
            At(pnt, i) = -At(pnt, i);
        m_closest1 += m_box->GetAxis(i) * At(pnt, i);
    }
    return sqrDistance;
}

float DISTSEGBOX::GetSquared()
{
    float sqrDistance = GetLineSquared();
    V3 point;
    float t = m_param;
    if (t >= 0.0f) {
        if (t <= 1.0f)
            return sqrDistance;
        point = m_segment->GetEnd();
    } else {
        point = m_segment->GetStart();
    }
    return m_box->ClosestPoint(point, &m_closest0, &m_closest1);
}

float SEGMENT3::SqrDistance(OOBOX3 *box)
{
    DISTSEGBOX dist(this, box);
    return dist.GetSquared();
}
