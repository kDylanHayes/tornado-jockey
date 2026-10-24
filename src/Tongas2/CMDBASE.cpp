#include <list>

#include "Tongas2/CMDBASE.h"
#include "Tongas2/GUI.h"
#include "Tongas2/CMDMGR.h"
#include "Tongas2/CWSTRING.h"
#include "Tongas2/KERNELCOMMANDS.h"
#include "Tongas2/LOG.h"
#include "Tongas2/RESOURCE.h"
#include "Tongas2/RESOURCES.h"
#include "Tongas2/UNICODESTRING.h"

#define CMD_FUNC(CLASS, method) (CMDFUNC)(SRESULT (CLASS::*)(TOKENS **))(SRESULT (CLASS::*)(CMDCALL *))&CLASS::method

// A property's value, element index of it, as a T.
#define PROP_VALUE(T, object, p, index) (&(T &)((object)->*(p)->field))[index]
// A property's setter, as one taking a T (and the index, for arrays).
#define PROP_SET(T, p) ((void (CMDBASE::*)(const T &))(p)->setter)
#define PROP_SET_AT(T, p) ((void (CMDBASE::*)(const int &, const T &))(p)->setter)

CMDENTRY CMDBASE::s_commands[] = {
    { "set", 0, 2, 0x1000, CMD_FUNC(CMDBASE, SetAttribute) },
    { "seteval", 0, 2, 3, CMD_FUNC(CMDBASE, SetEval) },
    { "setindirect", 0, 2, 3, CMD_FUNC(CMDBASE, SetIndirect) },
    { "dumpprops", 0, 0, 0, CMD_FUNC(CMDBASE, DumpProps) },
    { 0, 0, 0, 0, 0 },
};

PROPENTRY CMDBASE::s_properties[] = {
    { "Name", 0, PROP_CSTRING, 0, PROP_FIELD(CMDBASE, m_name), false, 1, PROP_SETTER(CMDBASE, SetName) },
    { 0, 0, PROP_END, 0, 0, false },
};

void CMDBASE::RegisterCommands()
{
    g_classParents[g_CMDBASEClassId] = g_NONEClassId;
    g_classCommands[g_CMDBASEClassId] = s_commands;
    HashCommands(s_commands);
    RegisterClassName(s_commands, "CMDBASE", "NONE");
}

PROPENTRY *CMDBASE::FindProperty(const char *name, int classId)
{
    if (!strlen(name))
        return 0;
    if (classId == g_NONEClassId) {
        classId = GetClassId();
        if (classId == g_NONEClassId || classId == KERNELCOMMANDS::s_classId)
            return 0;
    }
    int hash = HashName(name);
    while (classId != g_NONEClassId && classId != KERNELCOMMANDS::s_classId) {
        for (PROPENTRY *p = g_classProps[classId]; p->name; p++) {
            if (p->hash == hash && !_strnicmp(p->name, name, 30))
                return p;
        }
        classId = g_classParents[classId];
    }
    return 0;
}

int FindEnum(const char (*names)[30], const char *name)
{
    if (!names)
        return -1;
    for (int i = 0; names[i][0]; i++) {
        if (!_strnicmp(names[i], name, 30))
            return i;
    }
    return -1;
}

void HashProperties(PROPENTRY *properties)
{
    for (PROPENTRY *p = properties; p->name; p++)
        p->hash = HashName(p->name);
}

// The object a handle names, whichever system keeps it.
static CMDBASE *GetObject(RESHANDLE handle)
{
    switch (handle.kind) {
    case 1:
        return g_pActors->GetActor(handle);
    case 2:
        return g_pResources->GetResource(handle);
    case 3:
        return g_pResources->GetMaster(handle);
    case 4:
        return g_pGui->GetObject(handle);
    }
    return 0;
}

CMDBASE::CMDBASE()
{
    m_arraySize = 0;
    m_name = "";
}

float CMDBASE::GetFloatAttribute(const char *name, int index)
{
    if (!name || !strlen(name))
        return 0.0f;
    PROPENTRY *p = FindProperty(name, g_NONEClassId);
    if (!p) {
        if (g_pLog)
            g_pLog->Error("CMDBASE::SetAttribute> ERROR: Unable to locate attribute: %s", name);
        return 0.0f;
    }
    if (!p->field) {
        if (g_pLog)
            g_pLog->Error("CMDBASE::SetAttribute> ERROR: The pointer of the attribute %s is NULL", name);
        return 0.0f;
    }
    if (index < 0 || index >= p->count) {
        if (g_pLog)
            g_pLog->Error("CMDBASE::GetShortStringAttribute> trying to get value of variable %s at invalid range %d (size=%d)", p->name, index, p->count);
        return 0.0f;
    }
    switch (p->type) {
    case PROP_INT:
    case PROP_ENUM:
        return (float)PROP_VALUE(int, this, p, index);
    case PROP_BOOL:
        return (float)PROP_VALUE(bool, this, p, index);
    case PROP_FLOAT:
        return PROP_VALUE(float, this, p, index);
    case PROP_V2:
        return PROP_VALUE(V2, this, p, index).Length();
    case PROP_V3:
        return PROP_VALUE(V3, this, p, index).Length();
    }
    return 0.0f;
}

void CMDBASE::GetShortStringAttribute(FIXEDSTRING<0x200> &out, const char *name, int index)
{
    if (!name || !strlen(name)) {
        out.m_buf[0] = 0;
        return;
    }
    PROPENTRY *p = FindProperty(name, g_NONEClassId);
    if (!p) {
        if (g_pLog)
            g_pLog->Error("CMDBASE::GetShortStringAttribute> ERROR: Unable to locate attribute: %s", name);
        out.m_buf[0] = 0;
        return;
    }
    if (!p->field) {
        if (g_pLog)
            g_pLog->Error("CMDBASE::GetShortStringAttribute> ERROR: The pointer of the attribute %s is NULL", name);
        out.m_buf[0] = 0;
        return;
    }
    if (index < 0 || index >= p->count) {
        if (g_pLog)
            g_pLog->Error("CMDBASE::GetShortStringAttribute> trying to get value of variable %s at invalid range %d (size=%d)", p->name, index, p->count);
        return;
    }
    switch (p->type) {
    case PROP_INT:
    case PROP_ENUM:
        SafeSprintf(out, "%d", PROP_VALUE(int, this, p, index));
        return;
    case PROP_BOOL:
        SafeSprintf(out, "%d", PROP_VALUE(bool, this, p, index));
        return;
    case PROP_FLOAT:
        SafeSprintf(out, "%f", PROP_VALUE(float, this, p, index));
        return;
    case PROP_CWSTRING:
        out = PROP_VALUE(CWSTRING, this, p, index).ToAnsi();
        return;
    case PROP_UNICODESTRING:
        out = PROP_VALUE(UNICODESTRING, this, p, index).ToAnsi();
        return;
    case PROP_WCHARS:
        out = PROP_VALUE(FIXEDWSTRING<0x200>, this, p, index).ToAnsi();
        return;
    case PROP_CSTRING:
        out = PROP_VALUE(CSTRING, this, p, index).c_str();
        return;
    case PROP_STRING:
        out = PROP_VALUE(STRING, this, p, index).c_str();
        return;
    case PROP_CHARS:
        out = PROP_VALUE(FIXEDSTRING<0x200>, this, p, index).m_buf;
        return;
    case PROP_V2: {
        V2 &v = PROP_VALUE(V2, this, p, index);
        SafeSprintf(out, "(%.4f %.4f)", v.x, v.y);
        return;
    }
    case PROP_V3: {
        V3 &v = PROP_VALUE(V3, this, p, index);
        SafeSprintf(out, "(%.4f %.4f %.4f)", v.x, v.y, v.z);
        return;
    }
    case PROP_ANG: {
        ANG &a = PROP_VALUE(ANG, this, p, index);
        SafeSprintf(out, "(%.4f %.4f %.4f)", a.GetEulerX(), a.GetEulerY(), a.GetEulerZ());
        return;
    }
    case PROP_RGB: {
        RGB &c = PROP_VALUE(RGB, this, p, index);
        SafeSprintf(out, "(%d %d %d)", c.r, c.g, c.b);
        return;
    }
    case PROP_TEXTURE: {
        SRTEXTURE &t = PROP_VALUE(SRTEXTURE, this, p, index);
        SafeSprintf(out, "(%s %d)", t.name, t.index);
        return;
    }
    }
}

SRESULT CMDBASE::GetProperty(PROPENTRY *p, int index)
{
    if (index < 0 || index >= p->count) {
        if (g_pLog)
            g_pLog->Error("CMDBASE::GetPropValue> trying to get value of variable %s at invalid range %d (size=%d)", p->name, index, p->count);
        return SRESULT();
    }
    switch (p->type) {
    case PROP_INT:
        return SRESULT(PROP_VALUE(int, this, p, index));
    case PROP_ENUM:
        return SRESULT(p->names[PROP_VALUE(int, this, p, index)]);
    case PROP_BOOL:
        return SRESULT(PROP_VALUE(bool, this, p, index));
    case PROP_FLOAT:
        return SRESULT(PROP_VALUE(float, this, p, index));
    case PROP_CWSTRING:
        return SRESULT(PROP_VALUE(CWSTRING, this, p, index).c_str());
    case PROP_UNICODESTRING:
        return SRESULT(PROP_VALUE(UNICODESTRING, this, p, index).c_str());
    case PROP_WCHARS:
        return SRESULT(PROP_VALUE(FIXEDWSTRING<0x200>, this, p, index).m_buf);
    case PROP_CSTRING:
        return SRESULT(PROP_VALUE(CSTRING, this, p, index).c_str());
    case PROP_STRING:
        return SRESULT(PROP_VALUE(STRING, this, p, index).c_str());
    case PROP_CHARS:
        return SRESULT(PROP_VALUE(FIXEDSTRING<0x200>, this, p, index).m_buf);
    case PROP_V2:
        return SRESULT(PROP_VALUE(V2, this, p, index));
    case PROP_V3:
        return SRESULT(PROP_VALUE(V3, this, p, index));
    case PROP_ANG:
        return SRESULT(PROP_VALUE(ANG, this, p, index));
    case PROP_RGB:
        return SRESULT(PROP_VALUE(RGB, this, p, index));
    case PROP_TEXTURE:
        return SRESULT(PROP_VALUE(SRTEXTURE, this, p, index));
    }
    return SRESULT();
}

CMDBASE *CMDBASE::FindObject(const char *path, RESHANDLE caller)
{
    FIXEDSTRING<0x200> name;
    // The first dot not escaped with a '#'.
    const char *dot = path;
    bool escaped = false;
    do {
        dot = strchr(dot, '.');
        if (!dot || dot[-1] != '#')
            break;
        dot++;
        escaped = true;
    } while (dot);
    FIXEDSTRING<0x200> unescaped;
    if (escaped) {
        char *out = unescaped.m_buf;
        for (unsigned i = 0; i < strlen(path); i++) {
            if (path[i] != '#')
                *out++ = path[i];
        }
        *out = 0;
        path = unescaped.m_buf;
    }
    if (dot) {
        // The length is taken from the escaped path even when path is now
        // the unescaped copy.
        unsigned n = dot - path;
        if (n > 0x7f)
            n = 0x7f;
        memcpy(name.m_buf, path, n);
        name.m_buf[n] = 0;
        path = name.m_buf;
        dot++;
    }
    CMDBASE *object;
    if (!_stricmp(path, "parent"))
        object = (CMDBASE *)Virtual08();
    else if (!_stricmp(path, "my"))
        object = GetObject(caller);
    else
        object = (CMDBASE *)Virtual09(path);
    if (!object)
        return 0;
    if (dot)
        return object->FindObject(dot, caller);
    return object;
}

// Copies element i of a property, through target's setter if it has one.
#define COPY_PROP(T)                                                                  \
    if (p->setter) {                                                                  \
        if ((unsigned)p->count > 1)                                                   \
            (target->*PROP_SET_AT(T, p))(i, PROP_VALUE(T, this, p, i));               \
        else                                                                          \
            (target->*PROP_SET(T, p))(PROP_VALUE(T, this, p, i));                     \
    } else                                                                            \
        PROP_VALUE(T, target, p, i) = PROP_VALUE(T, this, p, i);

void CMDBASE::CopyProperties(CMDBASE *target, int classId)
{
    int id = GetClassId();
    if (classId != g_NONEClassId && id > classId)
        id = classId;
    while (id != g_NONEClassId) {
        for (PROPENTRY *p = g_classProps[id]; p->name; p++) {
            for (unsigned i = 0; i < (unsigned)p->count; i++) {
                switch (p->type) {
                case PROP_INT:
                    COPY_PROP(int);
                    break;
                case PROP_ENUM:
                    COPY_PROP(int);
                    break;
                case PROP_BOOL:
                    COPY_PROP(bool);
                    break;
                case PROP_FLOAT:
                    COPY_PROP(float);
                    break;
                case PROP_CWSTRING:
                    COPY_PROP(CWSTRING);
                    break;
                case PROP_UNICODESTRING:
                    COPY_PROP(UNICODESTRING);
                    break;
                case PROP_WCHARS:
                    COPY_PROP(FIXEDWSTRING<0x200>);
                    break;
                case PROP_CSTRING:
                    // Copied as a CWSTRING in the original.
                    COPY_PROP(CWSTRING);
                    break;
                case PROP_STRING:
                    COPY_PROP(STRING);
                    break;
                case PROP_CHARS:
                    COPY_PROP(FIXEDSTRING<0x200>);
                    break;
                case PROP_V2:
                    COPY_PROP(V2);
                    break;
                case PROP_V3:
                    COPY_PROP(V3);
                    break;
                case PROP_ANG:
                    COPY_PROP(ANG);
                    break;
                case PROP_RGB:
                    COPY_PROP(RGB);
                    break;
                case PROP_TEXTURE:
                    COPY_PROP(SRTEXTURE);
                    break;
                }
            }
        }
        id = g_classParents[id];
    }
}

void CMDBASE::GetPropStringValue(PROPENTRY *p, int index, STRING &out)
{
    if (index < 0 || index >= p->count) {
        if (g_pLog)
            g_pLog->Error("CMDBASE::GetPropStringValue> trying to get value of variable %s at invalid range %d (size=%d)", p->name, index, p->count);
        return;
    }
    switch (p->type) {
    case PROP_INT:
        out.Format("%d ", PROP_VALUE(int, this, p, index));
        return;
    case PROP_ENUM:
        out.Format("%s ", p->names[PROP_VALUE(int, this, p, index)]);
        return;
    case PROP_BOOL:
        out.Format("%d ", PROP_VALUE(bool, this, p, index));
        return;
    case PROP_FLOAT:
        out.Format("%.4f ", PROP_VALUE(float, this, p, index));
        return;
    case PROP_CWSTRING:
        out.Format("\"%s\" ", PROP_VALUE(CWSTRING, this, p, index).c_str());
        return;
    case PROP_UNICODESTRING:
        out.Format("\"%s\" ", PROP_VALUE(UNICODESTRING, this, p, index).c_str());
        return;
    case PROP_WCHARS:
        out.Format("\"%s\" ", PROP_VALUE(FIXEDWSTRING<0x200>, this, p, index).m_buf);
        return;
    case PROP_CSTRING:
        out.Format("\"%s\" ", PROP_VALUE(CSTRING, this, p, index).c_str());
        return;
    case PROP_STRING:
        out.Format("\"%s\" ", PROP_VALUE(STRING, this, p, index).c_str());
        return;
    case PROP_CHARS:
        out.Format("\"%s\" ", PROP_VALUE(FIXEDSTRING<0x200>, this, p, index).m_buf);
        return;
    case PROP_V2:
        out.Format("(%.4f %.4f) ", PROP_VALUE(V2, this, p, index).x, PROP_VALUE(V2, this, p, index).y);
        return;
    case PROP_V3:
        out.Format("(%.4f %.4f %.4f) ", PROP_VALUE(V3, this, p, index).x, PROP_VALUE(V3, this, p, index).y, PROP_VALUE(V3, this, p, index).z);
        return;
    case PROP_ANG: {
        float x, y, z;
        PROP_VALUE(ANG, this, p, index).GetEuler(&x, &y, &z);
        out.Format("(%.4f %.4f %.4f) ", x, y, z);
        return;
    }
    case PROP_RGB:
        out.Format("(%d %d %d) ", PROP_VALUE(RGB, this, p, index).r, PROP_VALUE(RGB, this, p, index).g, PROP_VALUE(RGB, this, p, index).b);
        return;
    case PROP_TEXTURE:
        out.Format("(\"%s\" %d) ", PROP_VALUE(SRTEXTURE, this, p, index).name.m_buf, PROP_VALUE(SRTEXTURE, this, p, index).index);
        return;
    }
}

// Reallocates an array property to m_arraySize elements of T.
#define RESIZE_PROP(T)                                       \
    {                                                        \
        T *&array = (T *&)(this->*p->field);                 \
        T *resized = 0;                                      \
        if (m_arraySize > 0) {                               \
            resized = new T[m_arraySize];                    \
            for (int i = 0; i < m_arraySize; i++)            \
                resized[i] = array[i];                       \
        }                                                    \
        if (array) {                                         \
            delete[] array;                                  \
            array = 0;                                       \
        }                                                    \
        array = resized;                                     \
    }

void CMDBASE::ResizeArrays()
{
    int id = GetClassId();
    while (id != g_NONEClassId && id != KERNELCOMMANDS::s_classId) {
        for (PROPENTRY *p = g_classProps[id]; p->name; p++) {
            if (!p->isArray || m_arraySize >= p->count)
                continue;
            switch (p->type) {
            case PROP_INT:
                RESIZE_PROP(int);
                break;
            case PROP_ENUM:
                RESIZE_PROP(int);
                break;
            case PROP_BOOL:
                RESIZE_PROP(bool);
                break;
            case PROP_FLOAT:
                RESIZE_PROP(float);
                break;
            case PROP_CWSTRING:
                RESIZE_PROP(CWSTRING);
                break;
            case PROP_UNICODESTRING:
                RESIZE_PROP(UNICODESTRING);
                break;
            case PROP_WCHARS:
                RESIZE_PROP(FIXEDWSTRING<0x200>);
                break;
            case PROP_CSTRING:
                RESIZE_PROP(CSTRING);
                break;
            case PROP_STRING:
                RESIZE_PROP(STRING);
                break;
            case PROP_CHARS:
                RESIZE_PROP(FIXEDSTRING<0x200>);
                break;
            case PROP_V2:
                RESIZE_PROP(V2);
                break;
            case PROP_V3:
                RESIZE_PROP(V3);
                break;
            case PROP_ANG:
                RESIZE_PROP(ANG);
                break;
            case PROP_RGB:
                RESIZE_PROP(RGB);
                break;
            case PROP_TEXTURE:
                RESIZE_PROP(SRTEXTURE);
                break;
            }
        }
        id = g_classParents[id];
    }
}

// Sets element index of a property, through its setter if it has one.
#define SET_PROP(T, value)                                            \
    if (p->setter) {                                                  \
        if ((unsigned)p->count > 1)                                   \
            (this->*PROP_SET_AT(T, p))((int)index, value);            \
        else                                                          \
            (this->*PROP_SET(T, p))(value);                           \
    } else                                                            \
        PROP_VALUE(T, this, p, index) = value;

SRESULT CMDBASE::SetAttribute(CMDCALL *call)
{
    int step;
    for (int i = 1; i < call->tokens->m_count; i += step) {
        PROPENTRY *p = FindProperty(call->tokens->GetToken(i), g_NONEClassId);
        if (!p) {
            if (g_pLog)
                g_pLog->Error("CMDBASE::SetAttribute> ERROR: Unable to locate attribute: %s", call->tokens->GetToken(i));
            step = 1;
            continue;
        }
        if (!p->field) {
            if (g_pLog)
                g_pLog->Error("CMDBASE::SetAttribute> ERROR: The pointer of the attribute %s is NULL", call->tokens->GetToken(i));
            step = 1;
            continue;
        }
        unsigned index;
        int valueAt;
        if ((unsigned)p->count > 1) {
            step = 3;
            index = atoi(call->tokens->GetToken(i + 1));
            valueAt = 2;
        } else {
            step = 2;
            index = 0;
            valueAt = 1;
        }
        if ((unsigned)p->count > 1 && index >= (unsigned)p->count) {
            if (g_pLog)
                g_pLog->Error("CMDBASE::SetAttribute> ERROR: Index %d, of the attribute %s is out of range", index, call->tokens->GetToken(i));
            continue;
        }
        if (p->isArray && m_arraySize < (int)index + 1)
            m_arraySize = index + 1;
        const char *token = call->tokens->GetToken(i + valueAt);
        switch (p->type) {
        case PROP_INT:
            SET_PROP(int, atoi(token));
            break;
        case PROP_ENUM: {
            int value = FindEnum(p->names, token);
            if (value < 0)
                break;
            SET_PROP(int, value);
            break;
        }
        case PROP_BOOL:
            SET_PROP(bool, atoi(token) != 0);
            break;
        case PROP_FLOAT:
            SET_PROP(float, (float)atof(token));
            break;
        case PROP_CWSTRING:
            if (p->setter) {
                UNICODESTRING text;
                text.FromAnsi(token);
                if ((unsigned)p->count > 1)
                    (this->*PROP_SET_AT(CWSTRING, p))((int)index, CWSTRING(text.c_str()));
                else
                    (this->*PROP_SET(CWSTRING, p))(CWSTRING(text.c_str()));
            } else
                PROP_VALUE(CWSTRING, this, p, index).FromAnsi(token);
            break;
        case PROP_UNICODESTRING:
            if (p->setter) {
                UNICODESTRING text;
                text.FromAnsi(token);
                if ((unsigned)p->count > 1)
                    (this->*PROP_SET_AT(UNICODESTRING, p))((int)index, text);
                else
                    (this->*PROP_SET(UNICODESTRING, p))(text);
            } else
                PROP_VALUE(UNICODESTRING, this, p, index).FromAnsi(token);
            break;
        case PROP_WCHARS:
            if (p->setter) {
                FIXEDWSTRING<0x200> text;
                text.FromAnsi(token);
                if ((unsigned)p->count > 1)
                    (this->*PROP_SET_AT(FIXEDWSTRING<0x200>, p))((int)index, text);
                else
                    (this->*PROP_SET(FIXEDWSTRING<0x200>, p))(text);
            } else
                PROP_VALUE(FIXEDWSTRING<0x200>, this, p, index).FromAnsi(token);
            break;
        case PROP_CSTRING:
            if (p->setter) {
                STRING text(token);
                if ((unsigned)p->count > 1)
                    (this->*PROP_SET_AT(CSTRING, p))((int)index, CSTRING(text.c_str()));
                else
                    (this->*PROP_SET(CSTRING, p))(CSTRING(text.c_str()));
            } else
                PROP_VALUE(CSTRING, this, p, index) = token;
            break;
        case PROP_STRING:
            if (p->setter) {
                STRING text(token);
                if ((unsigned)p->count > 1)
                    (this->*PROP_SET_AT(STRING, p))((int)index, text);
                else
                    (this->*PROP_SET(STRING, p))(text);
            } else
                PROP_VALUE(STRING, this, p, index) = token;
            break;
        case PROP_CHARS:
            if (p->setter) {
                FIXEDSTRING<0x200> text(token);
                if ((unsigned)p->count > 1)
                    (this->*PROP_SET_AT(FIXEDSTRING<0x200>, p))((int)index, text);
                else
                    (this->*PROP_SET(FIXEDSTRING<0x200>, p))(text);
            } else
                PROP_VALUE(FIXEDSTRING<0x200>, this, p, index) = token;
            break;
        case PROP_V2:
            if (p->setter) {
                V2 v(0.0f, 0.0f);
                sscanf(token, "%f %f", &v.x, &v.y);
                if ((unsigned)p->count > 1)
                    (this->*PROP_SET_AT(V2, p))((int)index, v);
                else
                    (this->*PROP_SET(V2, p))(v);
            } else {
                V2 &v = PROP_VALUE(V2, this, p, index);
                sscanf(token, "%f %f", &v.x, &v.y);
            }
            break;
        case PROP_V3:
            if (p->setter) {
                V3 v(0.0f, 0.0f, 0.0f);
                sscanf(token, "%f %f %f", &v.x, &v.y, &v.z);
                if ((unsigned)p->count > 1)
                    (this->*PROP_SET_AT(V3, p))((int)index, v);
                else
                    (this->*PROP_SET(V3, p))(v);
            } else {
                V3 &v = PROP_VALUE(V3, this, p, index);
                sscanf(token, "%f %f %f", &v.x, &v.y, &v.z);
            }
            break;
        case PROP_ANG: {
            float x, y, z;
            sscanf(token, "%f %f %f", &x, &y, &z);
            SET_PROP(ANG, ANG(x, y, z));
            break;
        }
        case PROP_RGB:
            if (p->setter) {
                RGB c;
                sscanf(token, "%d %d %d", &c.r, &c.g, &c.b);
                if ((unsigned)p->count > 1)
                    (this->*PROP_SET_AT(RGB, p))((int)index, c);
                else
                    (this->*PROP_SET(RGB, p))(c);
            } else {
                RGB &c = PROP_VALUE(RGB, this, p, index);
                sscanf(token, "%d %d %d", &c.r, &c.g, &c.b);
            }
            break;
        case PROP_TEXTURE: {
            TOKENS parts;
            if (!parts.Parse(STRING(token), 0) || parts.m_count != 2)
                return SRESULT(false);
            if (p->setter) {
                SRTEXTURE t;
                t.name = parts.GetToken(0);
                t.index = atoi(parts.GetToken(1));
                if ((unsigned)p->count > 1)
                    (this->*PROP_SET_AT(SRTEXTURE, p))((int)index, t);
                else
                    (this->*PROP_SET(SRTEXTURE, p))(t);
            } else {
                SRTEXTURE &t = PROP_VALUE(SRTEXTURE, this, p, index);
                t.name = parts.GetToken(0);
                t.index = atoi(parts.GetToken(1));
            }
            break;
        }
        }
    }
    return SRESULT(true);
}

// The same for text, which the field takes as it is.
#define SET_PROP_TEXT(T, text)                                       \
    if (p->setter) {                                                  \
        if ((unsigned)p->count > 1)                                   \
            (this->*PROP_SET_AT(T, p))((int)index, T(text));          \
        else                                                          \
            (this->*PROP_SET(T, p))(T(text));                         \
    } else                                                            \
        PROP_VALUE(T, this, p, index) = text;

bool CMDBASE::SetAttributeEval(const char *name, int index, const char *expression, RESHANDLE caller)
{
    if (!name) {
        if (g_pLog)
            g_pLog->Error("CMDBASE::SetAttributeEval> ERROR: NULL attribute name ");
        return false;
    }
    PROPENTRY *p = FindProperty(name, g_NONEClassId);
    if (!p) {
        if (g_pLog)
            g_pLog->Error("CMDBASE::SetAttributeEval> ERROR: Unable to locate attribute: %s", name);
        return false;
    }
    if (!p->field) {
        if (g_pLog)
            g_pLog->Error("CMDBASE::SetAttributeEval> ERROR: The pointer of the attribute %s is NULL", name);
        return false;
    }
    if ((unsigned)p->count > 1 && (index < 0 || index >= p->count)) {
        if (g_pLog)
            g_pLog->Error("CMDBASE::SetAttributeEval> ERROR: Index %d, of the attribute %s is out of range", index, name);
        return false;
    }
    SRESULT value = g_pCmdMgr->Evaluate(STRING(expression), caller);
    if (value.GetType() == SRT_NOTSET) {
        if (g_pLog)
            g_pLog->Error("CMDBASE::SetAttributeEval> ERROR: Invalid expression: %s", expression);
        return false;
    }
    switch (p->type) {
    case PROP_ENUM: {
        int enumValue = FindEnum(p->names, value.GetString(""));
        if (enumValue < 0)
            break;
        SET_PROP(int, enumValue);
        break;
    }
    case PROP_INT:
        SET_PROP(int, value.GetInt(0));
        break;
    case PROP_BOOL:
        SET_PROP(bool, value.GetBool(false));
        break;
    case PROP_FLOAT:
        SET_PROP(float, value.GetFloat(0.0f));
        break;
    case PROP_CWSTRING:
        SET_PROP_TEXT(CWSTRING, value.GetUnicodeString(L""));
        break;
    case PROP_UNICODESTRING:
        SET_PROP_TEXT(UNICODESTRING, value.GetUnicodeString(L""));
        break;
    case PROP_WCHARS:
        SET_PROP_TEXT(FIXEDWSTRING<0x200>, value.GetUnicodeString(L""));
        break;
    case PROP_CSTRING:
        SET_PROP_TEXT(CSTRING, value.GetString(""));
        break;
    case PROP_STRING:
        SET_PROP_TEXT(STRING, value.GetString(""));
        break;
    case PROP_CHARS:
        SET_PROP_TEXT(FIXEDSTRING<0x200>, value.GetString(""));
        break;
    case PROP_V3:
        SET_PROP(V3, value.GetV3(V3(0.0f, 0.0f, 0.0f)));
        break;
    case PROP_V2:
        SET_PROP(V2, value.GetV2(V2(0.0f, 0.0f)));
        break;
    case PROP_ANG:
        SET_PROP(ANG, value.GetANG(ANG(0.0f, 0.0f, 0.0f)));
        break;
    case PROP_RGB:
        SET_PROP(RGB, value.GetRGB(RGB()));
        break;
    case PROP_TEXTURE:
        SET_PROP(SRTEXTURE, value.GetTexture(SRTEXTURE()));
        break;
    }
    return true;
}

void CMDBASE::GetPropStringValueEscaped(PROPENTRY *p, int index, STRING &out)
{
    if (index < 0 || index >= p->count) {
        if (g_pLog)
            g_pLog->Error("CMDBASE::GetPropStringValueEscaped> trying to get value of variable %s at invalid range %d (size=%d)", p->name, index, p->count);
        return;
    }
    TOKENS tokens;
    switch (p->type) {
    case PROP_CWSTRING:
        PROP_VALUE(CWSTRING, this, p, index).ToEncoded(out);
        tokens.Quote(out);
        break;
    case PROP_UNICODESTRING:
        PROP_VALUE(UNICODESTRING, this, p, index).ToEncoded(out);
        tokens.Quote(out);
        break;
    case PROP_WCHARS:
        PROP_VALUE(FIXEDWSTRING<0x200>, this, p, index).ToEncoded(out);
        tokens.Quote(out);
        break;
    case PROP_CSTRING:
        out = PROP_VALUE(CSTRING, this, p, index).c_str();
        tokens.Quote(out);
        break;
    case PROP_STRING:
        out = PROP_VALUE(STRING, this, p, index).c_str();
        tokens.Quote(out);
        break;
    case PROP_CHARS:
        out = PROP_VALUE(FIXEDSTRING<0x200>, this, p, index).m_buf;
        tokens.Quote(out);
        break;
    default:
        GetPropStringValue(p, index, out);
        break;
    }
}

void CMDBASE::DumpProperties()
{
    int id = GetClassId();
    if (g_pLog)
        Virtual14();
    while (id != g_NONEClassId) {
        for (PROPENTRY *p = g_classProps[id]; p->name; p++) {
            for (unsigned i = 0; i < 1 && i < (unsigned)p->count; i++) {
                FIXEDSTRING<0x200> line;
                if ((unsigned)p->count > 1)
                    SafeSprintf(line, "prop:%s idx:%d value:", p->name, i);
                else
                    SafeSprintf(line, "prop:%s value:", p->name);
                STRING value;
                GetPropStringValueEscaped(p, i, value);
                line += value.c_str();
            }
        }
        id = g_classParents[id];
    }
}

SRESULT CMDBASE::DumpProps(CMDCALL *call)
{
    DumpProperties();
    return SRESULT(true);
}

SRESULT CMDBASE::SetEval(CMDCALL *call)
{
    int index;
    int valueAt;
    if (call->tokens->m_count > 3) {
        index = atoi(call->tokens->GetToken(2));
        valueAt = 3;
    } else {
        index = 0;
        valueAt = 2;
    }
    return SRESULT(SetAttributeEval(call->tokens->GetToken(1), index, call->tokens->GetToken(valueAt), call->caller));
}

SRESULT CMDBASE::SetIndirect(CMDCALL *call)
{
    int index;
    int valueAt;
    if (call->tokens->m_count > 3) {
        index = g_pCmdMgr->Evaluate(STRING(call->tokens->GetToken(2)), RESHANDLE()).GetInt(0);
        valueAt = 3;
    } else {
        index = 0;
        valueAt = 2;
    }
    STRING text(call->tokens->GetToken(1));
    SRESULT name = g_pCmdMgr->Evaluate(text, call->caller);
    return SRESULT(SetAttributeEval(name.GetString(""), index, call->tokens->GetToken(valueAt), call->caller));
}

int FindCommonClass(int classA, int classB)
{
    std::list<int> parentsA;
    std::list<int> parentsB;
    int id;
    for (id = classA; id != g_NONEClassId && id != KERNELCOMMANDS::s_classId; id = g_classParents[id]) {
        if (id == classB)
            return id;
        parentsA.push_front(id);
    }
    for (id = classB; id != g_NONEClassId && id != KERNELCOMMANDS::s_classId; id = g_classParents[id]) {
        if (id == classA)
            return id;
        parentsB.push_front(id);
    }
    int common = g_NONEClassId;
    std::list<int>::iterator a = parentsA.begin();
    std::list<int>::iterator b = parentsB.begin();
    while (a != parentsA.end() && b != parentsB.end() && *a == *b) {
        common = *a;
        ++a;
        ++b;
    }
    return common;
}

CMDCONTEXT::CMDCONTEXT(TOKENS *tokens, const RESHANDLE &caller)
{
    m_next = 0;
    m_20 = false;
    m_call.tokens = tokens;
    m_call.caller = caller;
}
