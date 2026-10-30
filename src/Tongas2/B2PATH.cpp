// The path behaviors scripts queue on actors and GUI controls:
// B2FolllowMaxPath, which moves along a path's points, and B2LookAtPath,
// which turns to face the point of a path as far along as the owner's
// progress. Placeholder file name.
#include "Tongas2/ACTORLINK.h"
#include "Tongas2/LOG.h"
#include "Tongas2/PATH.h"

#include <stdio.h>

extern RESOURCEMANAGER *g_pPATH;

static inline V3 Cross(const V3 &a, const V3 &b)
{
    return V3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

static inline V3 Normal(V3 v)
{
    v.Normalize();
    return v;
}

bool B2FOLLOWMAXPATH::Init(const char *params)
{
    if (!params)
        return false;
    char name[64];
    sscanf(params, "%s %f", name, &m_duration);
    PATHMASTER *path = (PATHMASTER *)g_pPATH->GetMaster(name);
    if (path) {
        m_path = path;
        m_numPoints = path->m_numPoints;
        m_index = 0;
        m_time = m_duration / (m_numPoints - 1.0f);
        return true;
    }
    if (g_pLog)
        g_pLog->Error("Path not found");
    return false;
}

bool B2FOLLOWMAXPATH::Update(float dt)
{
    dt = m_time - dt;
    m_time = dt;
    if (dt < 0.0f) {
        m_index++;
        int count = m_numPoints;
        if (m_index >= count - 1) {
            m_owner->Link06(m_path->m_pos[count - 1]);
            return false;
        }
        m_time = m_duration / (count - 1.0f) + dt;
    }
    float step = m_duration / (m_numPoints - 1.0f);
    V3 from = m_path->m_pos[m_index];
    V3 to = m_path->m_pos[m_index + 1];
    float t = 1.0f - m_time / step;
    m_owner->Link06((to - from) * t + from);
    return true;
}

bool B2LOOKATPATH::Init(const char *params)
{
    if (params) {
        PATHMASTER *path = (PATHMASTER *)g_pPATH->GetMaster(params);
        if (path) {
            m_path = path;
            return true;
        }
        if (g_pLog)
            g_pLog->Error("B2LookAtPath::Set - Path not found");
    }
    return false;
}

// Turns up square to dir.
void Orthonormalize(V3 &up, const V3 &dir)
{
    V3 right = Normal(Cross(up, dir));
    up = Normal(Cross(dir, right));
}

bool B2LOOKATPATH::Update(float dt)
{
    float t = m_owner->Link14();
    if (t < 0.0f)
        t = 0.0f;
    else if (t > 1.0f)
        t = 1.0f;
    PATHV3 target;
    m_path->GetPoint(t, &target, 0, 0, 0, 0);
    PATHV3 up;
    m_owner->Link07(&up, 0);
    up = V3(0.0f, 1.0f, 0.0f);
    V3 dir = Normal(target - m_owner->Link05());
    Orthonormalize(up, dir);
    m_owner->Link08(up, dir);
    return true;
}
