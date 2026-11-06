// Tongas2/MAPLOADER -- the map-list object at 0x004f8350.
//
// Discovery scaffold: LoadMap is still a stub. Update follows the decompiler's
// shape closely enough to keep iterating; the helper names are ours and are
// paired with the target's FUN_ symbols through the unit's symbol_mappings.
#include "Tongas2/MAPLOADER.h"

#include <string.h>
#include <string>

// The camera holder the two paths drive. Only the offsets this function
// touches are known.
struct CAMERAOBJ
{
    char m_000[0x188];
    V3 m_188;
    char m_194[0x14];
    V3 m_1a8;
    V3 m_1b4;

    void SetPosition(V3 position);
    void SetTarget(void *target);
    void SetAngles(float x, float y, float z);
    void Update();
};

// A path: the total time at 0x1c, the point at 0x20 and the points after
// that. GetPoint returns a point at a time.
struct PATHOBJ
{
    char m_000[0x1c];
    float m_1c;
    V3 m_20;
    char m_2c[8];
    V3 *GetPoint(V3 *out, float time);
};

// The 0xd0-byte model resource Update makes for the sun.
struct MAPRESOURCE
{
    MAPRESOURCE(int type, int flags, V3 *a, V3 *b, V3 *c, std::string name);
    char m_000[0xd0];
};

extern void MatrixView(MATRIX *out, const V3 *eye, const V3 *at, const V3 *up);
extern void MatrixInverse(MATRIX *out, const MATRIX *in);

// The out-of-line std::vector length check the map list's vector instantiation
// leaves in this object.
void VectorTooLong()
{
    throw std::length_error("vector<T> too long");
}

void MAPLOADER::LoadMap(const char *name)
{
    (void)name;
}

void MAPLOADER::Update(float dt)
{
    int zero = 0;
    if (m_20 == (void *)zero)
        return;

    if (m_10) {
        float time = m_0c + dt;
        m_0c = time;
        if (time >= 1.0f) {
            m_0c = 1.0f;
            m_24c = true;

            CAMERAOBJ *camera = (CAMERAOBJ *)m_24;
            camera->m_1a8 = V3(0.0f, 0.0f, -15.0f);
            camera->m_1b4 = m_234;

            V3 position = camera->m_188;
            CAMERAOBJ *holder = (CAMERAOBJ *)m_38;
            holder->SetPosition(position);

            V3 a((float)zero, (float)zero, -15.0f);
            V3 b(15.0f, 15.0f, 15.0f);
            V3 c((float)zero, (float)zero, -15.0f);
            MAPRESOURCE *resource = new MAPRESOURCE(2, 0, &a, &b, &c, std::string("SUN"));
            holder->SetTarget(resource);
            holder->SetAngles((float)zero, (float)zero, (float)zero);
            m_258 = true;
        }
    }

    m_230 += dt * 0.4f;
    if (m_230 > 2.0f) {
        m_230 = 2.0f;
        if (!m_10) {
            m_10 = true;
            m_0c = (float)zero;
        }
    }

    if (m_240 != 0) {
        PATHOBJ *path = (PATHOBJ *)m_240;
        m_228 = m_230 * path->m_1c;
        if (path->m_1c < m_228)
            m_228 = path->m_1c;

        V3 point;
        path->GetPoint(&point, m_228);

        CAMERAOBJ *camera = (CAMERAOBJ *)m_24;
        camera->m_1a8 = point;
        camera->m_1b4 = m_234;
    }

    if (m_244 != 0 && m_204 != 0) {
        PATHOBJ *path = (PATHOBJ *)m_244;
        m_22c = m_230 * path->m_1c;
        if (path->m_1c - 2.0f < m_22c)
            m_22c = path->m_1c - 2.0f;

        V3 point;
        path->GetPoint(&point, m_22c);
        V3 *target = (V3 *)((char *)m_204 + 0x50);
        *target = point;

        V3 eye;
        path->GetPoint(&eye, m_22c + 2.0f);
        V3 up((float)zero, 1.0f, (float)zero);

        MATRIX view;
        MatrixView(&view, &eye, target, &up);

        MATRIX inverse;
        MatrixInverse(&inverse, &view);
        memcpy((char *)m_204 + 0x10, &inverse, sizeof(MATRIX));
    }

    m_248 += dt;
    ((CAMERAOBJ *)m_24)->Update();
}
