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

    void ClearTargets();
    void SetTarget(void *target);
    void SetAngles(float x, float y, float z);
    void Update();
};

// A path: the total time at 0x1c, the point at 0x20, its name at 0x80.
struct PATHOBJ
{
    char m_000[0x1c];
    float m_1c;
    V3 m_20;
    char m_2c[0x54];
    std::string m_name;

    V3 *GetPoint(V3 *out, float time);
};

// The map's object, path and model lists, at the offsets the accessors use.
struct MAPDATA
{
    char m_00[4];
    void **m_objectsBegin;
    void **m_objectsEnd;
    char m_0c[8];
    void **m_pathsBegin;
    void **m_pathsEnd;
    char m_1c[8];
    void **m_modelsBegin;
    void **m_modelsEnd;

    void *GetObject(int index);
    void *GetPath(int index);
    void *GetModel(int index);
};

// One entry of the map's object list: two strings (the name and the type),
// then the fields the special names use.
struct MAPOBJECT
{
    std::string m_name;
    std::string m_type;
    char m_38[0x1d4];
    std::string m_20c;
};

// The manager that loads a map resource by name. The target calls it with the
// map name, a resource path, a second (empty) path and three flags.
struct MAPMANAGER
{
    void *Load(const char *name, std::string path, std::string suffix,
               int a, int b, int c);
};

// A 0x60-byte map decoration (the wing and logo objects).
struct MAPDECOR
{
    MAPDECOR(void *manager, int type);
    void SetName(const char *name, std::string path, std::string ext, int flags);

    char m_000[0x60];
};

// The 0xd0-byte particle descriptor the map attaches to the camera. Its
// constructor and copy constructor live in BONEDMODELDX8SD.
class BONEDMODELDX8PARTICLEDESC
{
public:
    BONEDMODELDX8PARTICLEDESC(const BONEDMODELDX8PARTICLEDESC &other);
    BONEDMODELDX8PARTICLEDESC(std::string name, int material, int layer,
                              const V3 *position, const V3 *direction, const V3 *color);

    char m_000[0xd0];
};

extern void MatrixView(MATRIX *out, const V3 *eye, const V3 *at, const V3 *up);
extern void MatrixInverse(MATRIX *out, const MATRIX *in);
// Uppercases a string; the map compares names case-insensitively.
extern "C" std::string UpperString(std::string text);
// Whether a resource of the name and suffix is known.
extern "C" bool FindMapResource(const std::string *name, char *suffix);

// The out-of-line std::vector length check the map list's vector instantiation
// leaves in this object.
void VectorTooLong()
{
    throw std::length_error("vector<T> too long");
}

void MAPLOADER::LoadMap(const char *name)
{
    m_234 = V3(0.0f, 0.0f, 0.0f);
    m_240 = 0;
    m_244 = 0;

    for (int i = 0; ((MAPDATA *)m_20)->m_pathsBegin != 0 && i < ((MAPDATA *)m_20)->m_pathsEnd - ((MAPDATA *)m_20)->m_pathsBegin; i++) {
        PATHOBJ *path = (PATHOBJ *)((MAPDATA *)m_20)->GetPath(i);
        std::string pathName = UpperString(path->m_name);
        if (pathName.length() >= 10 && pathName.substr(0, 10) == "CAMERAPATH")
            m_240 = path;
        else if (pathName.length() >= 8 && pathName.substr(0, 8) == "WINGPATH")
            m_244 = path;
    }

    for (int i = 0; ((MAPDATA *)m_20)->m_modelsBegin != 0 && i < ((MAPDATA *)m_20)->m_modelsEnd - ((MAPDATA *)m_20)->m_modelsBegin; i++) {
        void *model = ((MAPDATA *)m_20)->GetModel(i);
        BONEDMODELDX8PARTICLEDESC *resource =
            new BONEDMODELDX8PARTICLEDESC(*(BONEDMODELDX8PARTICLEDESC *)model);
        ((CAMERAOBJ *)m_38)->SetTarget(resource);
    }

    void **objBegin = ((MAPDATA *)m_20)->m_objectsBegin;
    void **objEnd = ((MAPDATA *)m_20)->m_objectsEnd;
    for (int i = 0; objBegin != 0 && i < objEnd - objBegin; i++) {
        MAPOBJECT *object = (MAPOBJECT *)((MAPDATA *)m_20)->GetObject(i);
        std::string objectType = UpperString(object->m_type);
        std::string objectName = UpperString(object->m_name);

        if (objectType.length() >= 4 && objectType.substr(0, 4) == "WING") {
            if (objectName.length() > 0) {
                std::string full = m_20c + objectName;
                MAPDECOR *wing = new MAPDECOR(m_04, m_00);
                m_204 = wing;
                wing->SetName(name, full, std::string(""), 1);
            }
        } else if (objectType.length() >= 4 && objectType.substr(0, 4) == "LOGO") {
            if (objectName.length() > 0) {
                std::string full = m_20c + objectName;
                MAPDECOR *logo = new MAPDECOR(m_04, m_00);
                m_208 = logo;
                logo->SetName(name, full, std::string(""), 1);
                *(V3 *)((char *)logo + 0x50) = *(V3 *)((char *)object + 0x38);
            }
        } else if (objectType == "CAMERA") {
            ((CAMERAOBJ *)m_24)->m_1a8 = *(V3 *)((char *)object + 0x38);
        } else if (objectType == "TARGET") {
            m_234 = *(V3 *)((char *)object + 0x38);
            ((CAMERAOBJ *)m_24)->m_1b4 = *(V3 *)((char *)object + 0x38);
        } else if (objectType == "SKYBOX") {
            std::string full = m_20c + objectName;
            char suffix = 0;
            if (FindMapResource(&full, &suffix))
                m_18 = ((MAPMANAGER *)m_04)->Load(name, full, std::string(""), 1, 1, 0);
        } else if (objectType == "GLOWSKYBOX") {
            std::string full = m_20c + objectName;
            char suffix = 0;
            if (FindMapResource(&full, &suffix))
                m_1c = ((MAPMANAGER *)m_04)->Load(name, full, std::string(""), 1, 1, 0);
        }
    }
}

void MAPLOADER::Update(float dt)
{
    int zero = 0;
    V3 point;
    if (m_20 == (void *)zero)
        return;

    if (m_10) {
        float time = m_0c + dt;
        m_0c = time;
        if (time >= 1.0f) {
            m_0c = 1.0f;
            m_24c = true;

            CAMERAOBJ *camera = (CAMERAOBJ *)m_24;
            V3 start((float)zero, (float)zero, -15.0f);
            camera->m_1a8 = start;
            camera->m_1b4 = point;

            V3 direction = camera->m_188;
            ((CAMERAOBJ *)m_38)->ClearTargets();

            BONEDMODELDX8PARTICLEDESC *resource =
                new BONEDMODELDX8PARTICLEDESC(std::string("SUN"), 2, 0,
                    &V3((float)zero, (float)zero, -15.0f),
                    &direction,
                    &V3(30.0f, 30.0f, 30.0f));
            ((CAMERAOBJ *)m_38)->SetTarget(resource);
            ((CAMERAOBJ *)m_38)->SetAngles((float)zero, (float)zero, (float)zero);
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

        ((CAMERAOBJ *)m_24)->m_1a8 = *path->GetPoint(&point, m_228);
        ((CAMERAOBJ *)m_24)->m_1b4 = m_234;
    }

    if (m_244 != 0 && m_204 != 0) {
        m_22c = m_230 * ((PATHOBJ *)m_244)->m_1c;
        if (m_22c > ((PATHOBJ *)m_244)->m_1c - 2.0f)
            m_22c = ((PATHOBJ *)m_244)->m_1c - 2.0f;

        ((PATHOBJ *)m_244)->GetPoint(&point, m_22c);
        *(V3 *)((char *)m_204 + 0x50) = *((PATHOBJ *)m_244)->GetPoint(&point, m_22c);
        ((PATHOBJ *)m_244)->GetPoint(&point, m_22c + 2.0f);

        MATRIX view;
        MatrixView(&view, &point, (V3 *)((char *)m_204 + 0x50),
                   &V3((float)zero, 1.0f, (float)zero));
        MatrixInverse(&view, &view);
        view._14 = 0.0f; view._24 = 0.0f; view._34 = 0.0f;
        view._41 = 0.0f; view._42 = 0.0f; view._43 = 0.0f;
        memcpy((char *)m_204 + 0x10, &view, sizeof(MATRIX));
    }

    m_248 += dt;
    ((CAMERAOBJ *)m_24)->Update();
}
