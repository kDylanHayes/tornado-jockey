#include <math.h>

#include "Tongas2/PATH.h"
#include "Tongas2/CMDMGR.h"

// The names of the curvature types, as the Curvature property shows them.
static const char s_curvatureNames[][30] = {
    "Linear",
    "Catmull-Rom",
    "Hermite",
    "Bezier",
    "",
};

// The point at t along the Hermite curve from p0 to p1 with tangents t0 and t1.
V3 Hermite(const V3 &p0, const V3 &t0, const V3 &p1, const V3 &t1, float t)
{
    float t2 = t * t;
    float t3 = t * t2;
    float h11 = t3 - t2;
    float h10 = t3 - (t2 + t2) + t;
    float h01 = t2 * 3.0f - (t3 + t3);
    float h00 = (t3 + t3) - t2 * 3.0f + 1.0f;
    return p0 * h00 + p1 * h01 + t0 * h10 + t1 * h11;
}

// The Hermite curve's derivative at t.
V3 HermiteTangent(const V3 &p0, const V3 &t0, const V3 &p1, const V3 &t1, float t)
{
    float t2 = 3.0f * t * t;
    float h11 = t2 - (t + t);
    float h10 = t2 - t * 4.0f + 1.0f;
    float a = t * 6.0f;
    float b = t * t * 6.0f;
    float h01 = a - b;
    float h00 = b - a;
    return p0 * h00 + p1 * h01 + t0 * h10 + t1 * h11;
}

void PATHMASTER::RegisterCommands()
{
    g_classParents[s_classId] = RESOURCEMASTER::s_classId;
    g_classCommands[s_classId] = s_commands;
    HashCommands(s_commands);
    RegisterClassName(s_commands, "PATHMASTER", "RESOURCEMASTER");
}

void PATHMASTER::Prepare()
{
    SetCalculateLengths(true);
}

void PATHMASTER::SetScalePoints(const float &value)
{
    m_scalePoints = value;
    m_displayDirty = true;
}

void PATHMASTER::SetDisplaySegments(const int &value)
{
    m_displaySegments = value;
    m_displayDirty = true;
}

void PATHMASTER::SetDisplayUp(const bool &value)
{
    m_displayUp = value;
    m_displayDirty = true;
    if (m_displayRadius)
        m_displayRadius = false;
}

void PATHMASTER::SetDisplayRadius(const bool &value)
{
    m_displayRadius = value;
    m_displayDirty = true;
    if (m_displayUp)
        m_displayUp = false;
}

void PATHMASTER::SetDisplayPointsSegment(const bool &value)
{
    m_displayPointsSegment = value;
    m_displayDirty = true;
}

void PATHMASTER::SetColor(const PATHCOLOR &value)
{
    m_color = value;
    m_displayDirty = true;
}

void PATHMASTER::SetRadius(const int &index, const float &value)
{
    m_radius[index] = value;
    m_dirty = true;
}

void PATHMASTER::SetAngle(const int &index, const float &value)
{
    m_angle[index] = value;
    m_dirty = true;
}

int PATHMASTER::WrapIndex(int i)
{
    while (i < 0)
        i += m_numPoints;
    if (i >= m_numPoints) {
        if (m_isClosed)
            return (i - m_closeToPoint) % (m_numPoints - m_closeToPoint) + m_closeToPoint;
        i = i % m_numPoints;
    }
    return i;
}

int PATHMASTER::NextIndex(int i)
{
    int next = i + 1;
    if (next >= m_numPoints) {
        if (m_isClosed)
            next += m_closeToPoint - m_numPoints;
        else
            next = m_numPoints - 1;
    }
    return next;
}

void PATHMASTER::FindSegment(float t, float *distance, int *segment)
{
    if (!m_isClosed) {
        t = (float)fmod((double)t, 1.0);
    } else if (t > 1.0f) {
        float start = m_distance[m_closeToPoint] / m_totalLength;
        t = fmod(t - start, 1.0f - start) + start;
    }
    float d = t * m_totalLength;
    *distance = d;
    *segment = 0;
    int lo = 0;
    int hi = m_numPoints;
    int mid = (int)(hi * 0.5f);
    if (hi > 1) {
        do {
            if (m_distance[mid] <= d)
                lo = mid;
            else
                hi = mid;
            mid = (int)((hi + lo) * 0.5f);
        } while (lo + 1 < hi);
    }
    *segment = mid;
}

bool PATHMASTER::GetLinear(float t, V3 *position, V3 *direction, V3 *up, float *radius, float *angle)
{
    int segment;
    FindSegment(t, &t, &segment);
    int next = segment + 1;
    float f = (t - m_distance[segment]) / m_length[segment];
    if (next >= m_numPoints) {
        if (m_isClosed)
            next += m_closeToPoint - m_numPoints;
        else
            next = m_numPoints - 1;
    }
    float g = 1.0f - f;
    *position = m_pos[segment] * g + m_pos[next] * f;
    *direction = m_pos[next] - m_pos[segment];
    *up = m_up[segment] * g + m_up[next] * f;
    *radius = g * m_radius[segment] + f * m_radius[next];
    *angle = g * m_angle[segment] + f * m_angle[next];
    return true;
}

void PATHMASTER::UpdateCopies()
{
    RESOURCEMASTER::UpdateCopies();
}

float PATHMASTER::ArcParam(int segment, float u)
{
    if (u < m_arcLength[segment][0])
        return u / m_arcLength[segment][0] * 0.125f;
    int lo = 0;
    int hi = 7;
    do {
        int mid = (int)((hi + lo) * 0.5f);
        if (m_arcLength[segment][mid] < u)
            lo = mid;
        else
            hi = mid;
    } while (lo + 1 < hi);
    return ((u - m_arcLength[segment][lo]) / (m_arcLength[segment][hi] - m_arcLength[segment][lo]) + (lo + 1)) * 0.125f;
}

class PATHMANAGER : public RESOURCEMANAGER
{
public:
    PATHMANAGER()
    {
        SafeSprintf(m_fileName, "PATH.txt");
        m_8 = false;
        m_18 = true;
    }

    virtual RESOURCEMASTER *CreateMaster(const char *name);
    virtual const char *GetTypeCode();
};

const char *PATHMANAGER::GetTypeCode()
{
    return "PATH";
}

static RESOURCEMANAGER *CreatePATH()
{
    return new PATHMANAGER;
}

CREATEMANAGER g_pfnCreatePATH = CreatePATH;

CMDENTRY PATHMASTER::s_commands[] = {
    { 0, 0, 0, 0, 0 },
};

PROPENTRY PATHMASTER::s_properties[] = {
    { "StartEditingMode", 0, 2, 0, PROP_FIELD(PATHMASTER, m_startEditingMode), false, 1, PROP_SETTER(PATHMASTER, SetStartEditingMode) },
    { "StopEditingMode", 0, 2, 0, PROP_FIELD(PATHMASTER, m_stopEditingMode), false, 1, PROP_SETTER(PATHMASTER, SetStopEditingMode) },
    { "TotalLength", 0, 3, 0, PROP_FIELD(PATHMASTER, m_totalLength), false, 1, PROP_SETTER(PATHMASTER, SetTotalLength) },
    { "Curvature", 0, 1, s_curvatureNames, PROP_FIELD(PATHMASTER, m_curvature), false, 1, PROP_SETTER(PATHMASTER, SetCurvature) },
    { "IsFixedPath", 0, 2, 0, PROP_FIELD(PATHMASTER, m_isFixedPath), false, 1, PROP_SETTER(PATHMASTER, SetIsFixedPath) },
    { "IsLinear", 0, 2, 0, PROP_FIELD(PATHMASTER, m_isLinear), false, 1, PROP_SETTER(PATHMASTER, SetIsLinear) },
    { "IsParamLength", 0, 2, 0, PROP_FIELD(PATHMASTER, m_isParamLength), false, 1, PROP_SETTER(PATHMASTER, SetIsParamLength) },
    { "Test", 0, 2, 0, PROP_FIELD(PATHMASTER, m_test), false, 1, PROP_SETTER(PATHMASTER, SetTest) },
    { "CalculateTangents", 0, 2, 0, PROP_FIELD(PATHMASTER, m_calculateTangents), false, 1, PROP_SETTER(PATHMASTER, SetCalculateTangents) },
    { "CalculateLengths", 0, 2, 0, PROP_FIELD(PATHMASTER, m_calculateLengths), false, 1, PROP_SETTER(PATHMASTER, SetCalculateLengths) },
    { "IsClosed", 0, 2, 0, PROP_FIELD(PATHMASTER, m_isClosed), false, 1, PROP_SETTER(PATHMASTER, SetIsClosed) },
    { "CloseToPoint", 0, 0, 0, PROP_FIELD(PATHMASTER, m_closeToPoint), false, 1, PROP_SETTER(PATHMASTER, SetCloseToPoint) },
    { "ScalePoints", 0, 3, 0, PROP_FIELD(PATHMASTER, m_scalePoints), false, 1, PROP_SETTER(PATHMASTER, SetScalePoints) },
    { "DisplaySegments", 0, 0, 0, PROP_FIELD(PATHMASTER, m_displaySegments), false, 1, PROP_SETTER(PATHMASTER, SetDisplaySegments) },
    { "DisplayUp", 0, 2, 0, PROP_FIELD(PATHMASTER, m_displayUp), false, 1, PROP_SETTER(PATHMASTER, SetDisplayUp) },
    { "DisplayRadius", 0, 2, 0, PROP_FIELD(PATHMASTER, m_displayRadius), false, 1, PROP_SETTER(PATHMASTER, SetDisplayRadius) },
    { "DisplayPointsSegment", 0, 2, 0, PROP_FIELD(PATHMASTER, m_displayPointsSegment), false, 1, PROP_SETTER(PATHMASTER, SetDisplayPointsSegment) },
    { "Color", 0, 13, 0, PROP_FIELD(PATHMASTER, m_color), false, 1, PROP_SETTER(PATHMASTER, SetColor) },
    { "Angle", 0, 3, 0, PROP_FIELD(PATHMASTER, m_angle), false, PATH_MAX_POINTS, PROP_SETTER(PATHMASTER, SetAngle) },
    { "Radius", 0, 3, 0, PROP_FIELD(PATHMASTER, m_radius), false, PATH_MAX_POINTS, PROP_SETTER(PATHMASTER, SetRadius) },
    { "Continuous", 0, 0, 0, PROP_FIELD(PATHMASTER, m_continuous), false, PATH_MAX_POINTS, PROP_SETTER(PATHMASTER, SetContinuous) },
    { "Length", 0, 3, 0, PROP_FIELD(PATHMASTER, m_length), false, PATH_MAX_POINTS, PROP_SETTER(PATHMASTER, SetLength) },
    { "Pos", 0, 11, 0, PROP_FIELD(PATHMASTER, m_pos), false, PATH_MAX_POINTS, PROP_SETTER(PATHMASTER, SetPos) },
    { "Up", 0, 11, 0, PROP_FIELD(PATHMASTER, m_up), false, PATH_MAX_POINTS, PROP_SETTER(PATHMASTER, SetUp) },
    { "Direction", 0, 11, 0, PROP_FIELD(PATHMASTER, m_direction), false, PATH_MAX_POINTS, PROP_SETTER(PATHMASTER, SetDirection) },
    { "Command", 0, 8, 0, PROP_FIELD(PATHMASTER, m_command), false, PATH_MAX_POINTS, PROP_SETTER(PATHMASTER, SetCommand) },
    { "DeletePoint", 0, 2, 0, PROP_FIELD(PATHMASTER, m_deletePoint), false, PATH_MAX_POINTS, PROP_SETTER(PATHMASTER, SetDeletePoint) },
    { "InsertPoint", 0, 2, 0, PROP_FIELD(PATHMASTER, m_insertPoint), false, PATH_MAX_POINTS, PROP_SETTER(PATHMASTER, SetInsertPoint) },
    { 0, 0, 15, 0, 0, false },
};

void PATHMASTER::SetCalculateTangents(const bool &value)
{
    if (!value)
        return;
    for (int i = 0; i < m_numPoints; i++) {
        if (i == 0) {
            if (m_isClosed && m_closeToPoint == 0)
                m_direction[0] = m_pos[1] - m_pos[m_numPoints - 1];
            else
                m_direction[0] = m_pos[1] - m_pos[0];
        }
        if (!m_isClosed && i == m_numPoints - 1)
            m_direction[i] = m_pos[m_numPoints - 1] - m_pos[m_numPoints - 2];
        int prev = WrapIndex(i - 1);
        int next = WrapIndex(i + 1);
        m_direction[i] = m_pos[next] - m_pos[prev];
    }
}

float PATHMASTER::GetLinearLength(int segment)
{
    int i = WrapIndex(segment);
    int next = NextIndex(i);
    return (m_pos[next] - m_pos[i]).Length();
}

float PATHMASTER::MeasureCurve(int segment)
{
    int i = WrapIndex(segment);
    int next = NextIndex(i);
    if (i == next)
        return 0.0f;
    float total = 0.0f;
    V3 last = m_pos[i];
    int step = 0;
    do {
        int n = step + 1;
        V3 point;
        if (n == PATH_ARC_STEPS)
            point = m_pos[next];
        else
            point = Hermite(m_pos[i], m_direction[i], m_pos[next], m_direction[next], n * 0.125f);
        total += (point - last).Length();
        m_arcLength[segment][step] = total;
        last = point;
        step = n;
    } while (step < PATH_ARC_STEPS);
    float scale = 1.0f / total;
    m_arcLength[segment][0] *= scale;
    m_arcLength[segment][1] *= scale;
    m_arcLength[segment][2] *= scale;
    m_arcLength[segment][3] *= scale;
    m_arcLength[segment][4] *= scale;
    m_arcLength[segment][5] *= scale;
    m_arcLength[segment][6] *= scale;
    m_arcLength[segment][7] *= scale;
    return total;
}

void PATHMASTER::RunCommands(float from, float to, RESHANDLE caller)
{
    if (from == 0.0f && to != 0.0f) {
        if (m_command[0].Length() > 0)
            g_pCmdMgr->Run(m_command[0], caller);
    }
    if (m_totalLength != 0.0f) {
        float distance;
        int segment;
        FindSegment(from, &distance, &segment);
        float scale = 1.0f / m_totalLength;
        distance *= scale;
        float end = (to - from) + distance;
        float point = scale * m_distance[segment];
        while (point <= end) {
            if (distance < point && point <= end) {
                int i = WrapIndex(segment);
                if (m_command[i].Length() > 0)
                    g_pCmdMgr->Run(m_command[i], caller);
            }
            int i = WrapIndex(segment);
            segment++;
            point += m_length[i] / m_totalLength;
        }
    }
}

bool PATHMASTER::GetCatmullRom(float t, V3 *position, V3 *direction, V3 *up, float *radius, float *angle)
{
    int segment;
    FindSegment(t, &t, &segment);
    int prev;
    if (!m_continuous[segment]) {
        prev = segment - 1;
        if (prev < 0) {
            if (!m_isClosed || m_closeToPoint != 0)
                prev = 0;
            else
                prev += m_numPoints;
        }
    } else {
        prev = segment;
    }
    int next = NextIndex(segment);
    int after = next;
    if (m_continuous[next])
        after = NextIndex(next);
    float length;
    if (!m_isParamLength)
        length = m_distance[next] - m_distance[segment];
    else
        length = m_length[segment];
    if (length < 0.0f)
        length = length + m_totalLength - m_distance[m_closeToPoint];
    float d = t - m_distance[segment];
    if (t < m_distance[segment])
        d = d + m_totalLength - m_distance[m_closeToPoint];
    float u = d / length;
    float s = ArcParam(segment, u);
    if (position)
        CatmullRom(position, m_pos[prev], m_pos[segment], m_pos[next], m_pos[after], s);
    if (direction)
        CatmullRomTangent(direction, m_pos[prev], m_pos[segment], m_pos[next], m_pos[after], s);
    if (up)
        *up = m_up[segment] * (1.0f - u) + m_up[next] * u;
    if (radius)
        *radius = u * m_radius[next] + (1.0f - u) * m_radius[segment];
    if (angle)
        *angle = u * m_angle[next] + (1.0f - u) * m_angle[segment];
    return true;
}

bool PATHMASTER::GetHermite(float t, V3 *position, V3 *direction, V3 *up, float *radius, float *angle)
{
    int segment;
    FindSegment(t, &t, &segment);
    int next = segment + 1;
    if (next >= m_numPoints) {
        if (m_isClosed)
            next += m_closeToPoint - m_numPoints;
        else
            next = m_numPoints - 1;
    }
    float length;
    if (!m_isParamLength)
        length = m_distance[next] - m_distance[segment];
    else
        length = m_length[segment];
    if (length < 0.0f)
        length = length + m_totalLength - m_distance[m_closeToPoint];
    float d = t - m_distance[segment];
    if (t < m_distance[segment])
        d = d + m_totalLength - m_distance[m_closeToPoint];
    float u = d / length;
    float s = ArcParam(segment, u);
    if (position)
        *position = Hermite(m_pos[segment], m_direction[segment], m_pos[next], m_direction[next], s);
    if (direction)
        *direction = HermiteTangent(m_pos[segment], m_direction[segment], m_pos[next], m_direction[next], s);
    if (up)
        *up = m_up[segment] * (1.0f - u) + m_up[next] * u;
    if (radius)
        *radius = (1.0f - u) * m_radius[segment] + u * m_radius[next];
    if (angle)
        *angle = (1.0f - u) * m_angle[segment] + u * m_angle[next];
    return true;
}

bool PATHMASTER::GetLinearDistance(V3 point, int segment, float *distance, float *t)
{
    int next = segment + 1;
    if (segment == m_numPoints - 1) {
        if (!m_isClosed)
            return false;
        next = m_closeToPoint;
    }
    float d0 = (m_pos[segment] - point).Length();
    float d1 = (m_pos[next] - point).Length();
    float length = (m_pos[next] - m_pos[segment]).Length();
    float along = 0.0f;
    if (length > 0.0f) {
        along = (pow(d0, 2) - pow(d1, 2) + pow(length, 2)) / (length + length);
        if (along < 0.0f) {
            *distance = (m_pos[segment] - point).Length();
            *t = 0.0f;
            return true;
        }
    }
    if (along > length) {
        *distance = (m_pos[next] - point).Length();
        *t = 1.0f;
        return true;
    }
    *distance = (float)sqrt(pow(d0, 2) - pow(along, 2));
    *t = along / length;
    return true;
}

bool PATHMASTER::GetCurveDistance(V3 point, int segment, float *distance, float *t)
{
    return GetLinearDistance(point, segment, distance, t);
}

float PATHMASTER::GetSegmentLength(int segment)
{
    if (segment >= m_numPoints - 1 && !m_isClosed)
        return 0.0f;
    if (m_isParamLength == true) {
        if (m_isClosed)
            return (float)fabs(m_length[WrapIndex(segment)]);
        return (float)fabs(m_length[segment]);
    }
    switch (m_curvature) {
    case CURVE_LINEAR:
    case CURVE_CATMULLROM:
    case CURVE_HERMITE:
        MeasureCurve(segment);
    }
    return GetLinearLength(segment);
}

bool PATHMASTER::GetPoint(float t, V3 *position, V3 *direction, V3 *up, float *radius, float *angle)
{
    if (!m_isLinear)
        return false;
    switch (m_curvature) {
    case CURVE_LINEAR:
        return GetLinear(t, position, direction, up, radius, angle);
    case CURVE_CATMULLROM:
        return GetCatmullRom(t, position, direction, up, radius, angle);
    case CURVE_HERMITE:
        return GetHermite(t, position, direction, up, radius, angle);
    case CURVE_BEZIER:
        return false;
    }
    return true;
}

bool PATHMASTER::GetSegmentDistance(V3 point, int segment, float *distance, float *t)
{
    bool result = false;
    if (m_isLinear) {
        switch (m_curvature) {
        case CURVE_LINEAR:
            return GetLinearDistance(point, segment, distance, t);
        case CURVE_CATMULLROM:
            return GetCurveDistance(point, segment, distance, t);
        case CURVE_HERMITE:
            return GetCurveDistance(point, segment, distance, t);
        case CURVE_BEZIER:
            return GetCurveDistance(point, segment, distance, t);
        }
    }
    return result;
}

bool PATHMASTER::FindNearest(V3 point, int *segment, float *t)
{
    if (!m_isLinear)
        return false;
    float best = (m_pos[0] - point).Length();
    int last = m_numPoints - 1;
    float bestT = 0.0f;
    int bestSegment = 0;
    if (m_isClosed)
        last = m_numPoints;
    for (int i = 0; i < last; i++) {
        float distance = (m_pos[i] - point).Length();
        float u;
        GetSegmentDistance(point, i, &distance, &u);
        if (distance < best) {
            best = distance;
            bestSegment = i;
            bestT = (u * m_length[i] + m_distance[i]) / m_totalLength;
        }
    }
    *segment = bestSegment;
    *t = bestT;
    return true;
}

PATHMASTER::PATHMASTER(RESOURCEMANAGER *manager, const char *name)
    : RESOURCEMASTER(manager, name), m_numPoints(0), m_dirty(true), m_distance(), m_arcLength()
{
    m_color.r = 255;
    m_color.g = 0;
    m_startEditingMode = false;
    m_stopEditingMode = false;
    m_totalLength = 0.0f;
    m_curvature = CURVE_LINEAR;
    m_isFixedPath = true;
    m_isLinear = true;
    m_isParamLength = false;
    m_test = false;
    m_calculateTangents = false;
    m_calculateLengths = false;
    m_isClosed = false;
    m_closeToPoint = 0;
    m_scalePoints = 1.0f;
    m_displaySegments = 5;
    m_displayUp = false;
    m_displayRadius = false;
    m_displayPointsSegment = false;
    m_color.b = 0;
    for (g_i = 0; g_i < PATH_MAX_POINTS; g_i++)
        m_angle[g_i] = 0.0f;
    for (g_i = 0; g_i < PATH_MAX_POINTS; g_i++)
        m_radius[g_i] = 1.0f;
    for (g_i = 0; g_i < PATH_MAX_POINTS; g_i++)
        m_continuous[g_i] = 1;
    for (g_i = 0; g_i < PATH_MAX_POINTS; g_i++)
        m_length[g_i] = 0.0f;
    for (g_i = 0; g_i < PATH_MAX_POINTS; g_i++)
        m_pos[g_i] = V3(0.0f, 0.0f, 0.0f);
    for (g_i = 0; g_i < PATH_MAX_POINTS; g_i++)
        m_up[g_i] = V3(0.0f, 1.0f, 0.0f);
    for (g_i = 0; g_i < PATH_MAX_POINTS; g_i++)
        m_direction[g_i] = V3(0.0f, 0.0f, 0.0f);
    for (g_i = 0; g_i < PATH_MAX_POINTS; g_i++)
        m_command[g_i] = "";
    for (g_i = 0; g_i < PATH_MAX_POINTS; g_i++)
        m_deletePoint[g_i] = false;
    for (g_i = 0; g_i < PATH_MAX_POINTS; g_i++)
        m_insertPoint[g_i] = false;
}

int PATHMASTER::GetClassId()
{
    return s_classId;
}

int PATHMASTER::GetParentClassId()
{
    return g_classParents[s_classId];
}

const char *PATHMASTER::GetClassName()
{
    return g_classNames[s_classId];
}

SRESULT PATHMASTER::Execute(CMDENTRY *command, TOKENS **args)
{
    return (this->*(SRESULT (PATHMASTER::*)(TOKENS **))command->func)(args);
}

void PATHMASTER::SetCommand(const int &index, const STRING &value)
{
    m_command[index] = value;
}

void PATHMASTER::UpdateLengths()
{
    float total = 0.0f;
    if (m_isParamLength == true) {
        for (int i = 0; i < m_numPoints; i++) {
            m_distance[i] = total;
            total += (float)fabs(m_length[i]);
        }
    } else {
        for (int i = 0; i < m_numPoints; i++) {
            float length = GetSegmentLength(i);
            m_length[i] = length;
            m_distance[i] = total;
            total += length;
        }
    }
    m_totalLength = total;
}

void PATHMASTER::SetInsertPoint(const int &index, const bool &value)
{
    int count = m_numPoints + 1 < PATH_MAX_POINTS ? m_numPoints + 1 : PATH_MAX_POINTS;
    for (unsigned int i = count - 1; (int)i > index; i--) {
        SetDirection(i, m_direction[i - 1]);
        SetPos(i, m_pos[i - 1]);
        SetCommand(i, m_command[i - 1]);
        SetLength(i, m_length[i - 1]);
        SetRadius(i, m_radius[i - 1]);
        SetUp(i, m_up[i - 1]);
        SetAngle(i, m_angle[i - 1]);
        SetContinuous(i, m_continuous[i - 1]);
    }
    if (value) {
        if ((unsigned)index > 0) {
            SetPos(index, m_pos[index - 1]);
            SetUp(index, m_up[index - 1]);
            SetAngle(index, m_angle[index - 1]);
            SetContinuous(index, m_continuous[index - 1]);
        } else {
            SetPos(index, g_pCamera->GetDirection() * 20.0f + g_pCamera->GetWorldPosition());
        }
    }
    m_numPoints++;
    UpdateLengths();
    m_dirty = true;
}

RESOURCEMASTER *PATHMANAGER::CreateMaster(const char *name)
{
    return new PATHMASTER(this, name);
}

void PATHMASTER::SetCalculateLengths(const bool &value)
{
    bool paramLength = m_isParamLength;
    m_isParamLength = false;
    for (int i = 0; i < m_numPoints; i++)
        m_length[i] = GetSegmentLength(i);
    m_isParamLength = paramLength;
    UpdateLengths();
    m_dirty = true;
}

void PATHMASTER::SetCurvature(const int &value)
{
    m_curvature = value;
    UpdateLengths();
    m_dirty = true;
}

void PATHMASTER::SetContinuous(const int &index, const int &value)
{
    m_continuous[index] = value;
    UpdateLengths();
    m_dirty = true;
}

void PATHMASTER::SetPos(const int &index, const V3 &value)
{
    m_pos[index] = value;
    if (!m_isParamLength)
        UpdateLengths();
    m_dirty = true;
}

void PATHMASTER::SetUp(const int &index, const V3 &value)
{
    m_up[index] = value;
    if (!m_isParamLength)
        UpdateLengths();
    m_dirty = true;
}

void PATHMASTER::SetDirection(const int &index, const V3 &value)
{
    m_direction[index] = value;
    if (!m_isParamLength)
        UpdateLengths();
    m_dirty = true;
}

void PATHMASTER::SetLength(const int &index, const float &value)
{
    m_length[index] = value;
    if (m_isParamLength == true)
        UpdateLengths();
    m_dirty = true;
}

void PATHMASTER::SetCloseToPoint(const int &value)
{
    m_closeToPoint = value;
    UpdateLengths();
    m_dirty = true;
}

void PATHMASTER::SetIsParamLength(const bool &value)
{
    m_isParamLength = value;
    UpdateLengths();
    m_dirty = true;
}

void PATHMASTER::SetIsClosed(const bool &value)
{
    m_isClosed = value;
    UpdateLengths();
    m_dirty = true;
}

void PATHMASTER::SetDeletePoint(const int &index, const bool &value)
{
    if (!value)
        return;
    unsigned int i = index;
    if (i < 1)
        return;
    int last = m_numPoints - 1;
    for (; (int)i < last; i++) {
        SetDirection(i, m_direction[i + 1]);
        SetPos(i, m_pos[i + 1]);
        SetCommand(i, m_command[i + 1]);
        SetLength(i, m_length[i + 1]);
        SetRadius(i, m_radius[i + 1]);
        SetUp(i, m_up[i + 1]);
        SetAngle(i, m_angle[i + 1]);
        SetContinuous(i, m_continuous[i + 1]);
    }
    SetDirection(i, V3(0.0f, 0.0f, 0.0f));
    SetPos(i, V3(0.0f, 0.0f, 0.0f));
    SetLength(i, 0.0f);
    SetCommand(i, "");
    SetRadius(i, 0.0f);
    SetUp(i, V3(0.0f, 0.0f, 0.0f));
    SetAngle(i, 0.0f);
    SetContinuous(i, 1);
    m_numPoints--;
    if (index < m_closeToPoint)
        m_closeToPoint--;
    UpdateLengths();
    m_dirty = true;
}
