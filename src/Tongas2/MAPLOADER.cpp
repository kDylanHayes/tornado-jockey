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

// The manager that loads a map resource by name.
struct MAPMANAGER
{
    void *Load(const char *name);
};

// A 0x60-byte map decoration (the wing and logo objects).
struct MAPDECOR
{
    MAPDECOR(const char *text, int length);
    void SetName(const char *name);

    char m_000[0x60];
};

// The 0xd0-byte model resource the map creates.
struct MAPRESOURCE
{
    MAPRESOURCE(int type, int flags, V3 *a, V3 *b, V3 *c, std::string name);
    MAPRESOURCE(void *model);
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

    MAPDATA *map = (MAPDATA *)m_20;
    for (int i = 0; map->m_pathsBegin != 0 && i < map->m_pathsEnd - map->m_pathsBegin; i++) {
        PATHOBJ *path = (PATHOBJ *)map->GetPath(i);
        std::string pathName = UpperString(path->m_name);
        if (pathName.length() >= 10 && std::string(pathName, 0, 10) == "CAMERAPATH")
            m_240 = path;
        else if (pathName.length() >= 8 && std::string(pathName, 0, 8) == "WINGPATH")
            m_244 = path;
    }

    for (int i = 0; map->m_modelsBegin != 0 && i < map->m_modelsEnd - map->m_modelsBegin; i++) {
        void *model = map->GetModel(i);
        MAPRESOURCE *resource = new MAPRESOURCE(model);
        ((CAMERAOBJ *)m_38)->SetTarget(resource);
    }

    for (int i = 0; map->m_objectsBegin != 0 && i < map->m_objectsEnd - map->m_objectsBegin; i++) {
        MAPOBJECT *object = (MAPOBJECT *)map->GetObject(i);
        std::string objectType = UpperString(object->m_type);
        std::string objectName = UpperString(object->m_name);

        if (objectType.length() >= 4 && std::string(objectType, 0, 4) == "WING") {
            if (objectName.length() > 0) {
                std::string full = m_20c + objectName;
                MAPDECOR *wing = new MAPDECOR(objectName.c_str(), objectName.length());
                m_204 = wing;
                wing->SetName(full.c_str());
            }
        } else if (objectType.length() >= 4 && std::string(objectType, 0, 4) == "LOGO") {
            if (objectName.length() > 0) {
                std::string full = m_20c + objectName;
                MAPDECOR *logo = new MAPDECOR(objectName.c_str(), objectName.length());
                m_208 = logo;
                logo->SetName(full.c_str());
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
                m_18 = ((MAPMANAGER *)m_04)->Load(name);
        } else if (objectType == "GLOWSKYBOX") {
            std::string full = m_20c + objectName;
            char suffix = 0;
            if (FindMapResource(&full, &suffix))
                m_1c = ((MAPMANAGER *)m_04)->Load(name);
        }
    }
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
            V3 b(30.0f, 30.0f, 30.0f);
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
        MatrixInverse(&view, &view);
        memcpy((char *)m_204 + 0x10, &view, sizeof(MATRIX));
    }

    m_248 += dt;
    ((CAMERAOBJ *)m_24)->Update();
}
