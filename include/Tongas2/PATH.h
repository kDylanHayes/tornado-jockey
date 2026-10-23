// Paths: up to 100 points joined by straight lines, Catmull-Rom or Hermite
// curves, with an up vector, a radius, an angle and a script command per
// point. PATHMASTER is the path type's file; a path has no copies. Placeholder
// names apart from the class name and properties.
#ifndef TONGAS2_PATH_H
#define TONGAS2_PATH_H

#include "Tongas2/RESOURCE.h"
#include "Tongas2/RESOURCES.h"
#include "Tongas2/STRING.h"

#define PATH_MAX_POINTS 100
// Each segment's length is measured at this many points along it.
#define PATH_ARC_STEPS 8

// Defined elsewhere: the loop counter at 0x6cbc88 that many units share.
extern int g_i;

// Defined elsewhere: runs script commands.
class CONSOLE
{
public:
    // Runs command on behalf of the resource with the handle.
    void Run(const STRING &command, const RESHANDLE &caller);
    // The value of a script expression, for the resource with the handle.
    SRESULT Evaluate(const STRING &expression, const RESHANDLE &caller);
};
extern CONSOLE *g_pConsole;

// A point of the path, at the origin until set.
struct PATHV3 : V3
{
    PATHV3() { x = 0.0f; y = 0.0f; z = 0.0f; }

    PATHV3 &operator=(const V3 &v)
    {
        V3::operator=(v);
        return *this;
    }
};

// The path's colour in the editor.
struct PATHCOLOR
{
    PATHCOLOR() { r = g = b = 0; }

    int r, g, b;
};

class PATHMASTER : public RESOURCEMASTER
{
public:
    enum
    {
        CURVE_LINEAR,
        CURVE_CATMULLROM,
        CURVE_HERMITE,
        CURVE_BEZIER
    };

    PATHMASTER(RESOURCEMANAGER *manager, const char *name);

    static void RegisterCommands();

    virtual int GetClassId();
    virtual int GetParentClassId();
    virtual const char *GetClassName();
    virtual SRESULT Execute(CMDENTRY *command, TOKENS **args);
    virtual void UpdateCopies();
    virtual void Prepare();

    virtual void SetStartEditingMode(const bool &value) {}
    virtual void SetStopEditingMode(const bool &value) {}
    virtual void SetTotalLength(const float &value) {}
    virtual void SetCurvature(const int &value);
    virtual void SetIsFixedPath(const bool &value) { m_isFixedPath = value; }
    virtual void SetIsLinear(const bool &value) { m_isLinear = value; }
    virtual void SetIsParamLength(const bool &value);
    virtual void SetTest(const bool &value) {}
    // Sets each point's direction from its neighbours.
    virtual void SetCalculateTangents(const bool &value);
    // Measures each segment again.
    virtual void SetCalculateLengths(const bool &value);
    virtual void SetIsClosed(const bool &value);
    virtual void SetCloseToPoint(const int &value);
    virtual void SetScalePoints(const float &value);
    virtual void SetDisplaySegments(const int &value);
    virtual void SetDisplayUp(const bool &value);
    virtual void SetDisplayRadius(const bool &value);
    virtual void SetDisplayPointsSegment(const bool &value);
    virtual void SetColor(const PATHCOLOR &value);
    virtual void SetAngle(const int &index, const float &value);
    virtual void SetRadius(const int &index, const float &value);
    virtual void SetContinuous(const int &index, const int &value);
    virtual void SetLength(const int &index, const float &value);
    virtual void SetPos(const int &index, const V3 &value);
    virtual void SetUp(const int &index, const V3 &value);
    virtual void SetDirection(const int &index, const V3 &value);
    virtual void SetCommand(const int &index, const STRING &value);
    virtual void SetDeletePoint(const int &index, const bool &value);
    virtual void SetInsertPoint(const int &index, const bool &value);

    // The index of point i of a closed path, or of an open one.
    int WrapIndex(int i);
    int NextIndex(int i);
    // The segment the path is at at t (0..1), and the distance along it.
    void FindSegment(float t, float *distance, int *segment);
    // The fraction of the segment's curve at fraction u of its length.
    float ArcParam(int segment, float u);
    bool GetLinear(float t, V3 *position, V3 *direction, V3 *up, float *radius, float *angle);
    bool GetCatmullRom(float t, V3 *position, V3 *direction, V3 *up, float *radius, float *angle);
    bool GetHermite(float t, V3 *position, V3 *direction, V3 *up, float *radius, float *angle);
    // The point at t (0..1) along the path, with its direction, up vector,
    // radius and angle.
    bool GetPoint(float t, V3 *position, V3 *direction, V3 *up, float *radius, float *angle);
    // Runs the commands of the points passed between from and to.
    void RunCommands(float from, float to, RESHANDLE caller);
    float GetLinearLength(int segment);
    // Measures the segment's curve at PATH_ARC_STEPS points.
    float MeasureCurve(int segment);
    float GetSegmentLength(int segment);
    void UpdateLengths();
    // How far point is from the segment, and where along it it is closest.
    bool GetLinearDistance(V3 point, int segment, float *distance, float *t);
    bool GetCurveDistance(V3 point, int segment, float *distance, float *t);
    bool GetSegmentDistance(V3 point, int segment, float *distance, float *t);
    // The segment and t (0..1) of the path closest to point.
    bool FindNearest(V3 point, int *segment, float *t);

    static int s_classId;
    static CMDENTRY s_commands[];
    static PROPENTRY s_properties[];

    bool m_startEditingMode;
    bool m_stopEditingMode;
    float m_totalLength;
    int m_curvature;
    bool m_isFixedPath;
    bool m_isLinear;
    bool m_isParamLength;
    bool m_test;
    bool m_calculateTangents;
    bool m_calculateLengths;
    bool m_isClosed;
    int m_closeToPoint;
    float m_scalePoints;
    int m_displaySegments;
    bool m_displayUp;
    bool m_displayRadius;
    bool m_displayPointsSegment;
    PATHCOLOR m_color;
    float m_angle[PATH_MAX_POINTS];
    float m_radius[PATH_MAX_POINTS];
    int m_continuous[PATH_MAX_POINTS];
    float m_length[PATH_MAX_POINTS];
    PATHV3 m_pos[PATH_MAX_POINTS];
    PATHV3 m_up[PATH_MAX_POINTS];
    PATHV3 m_direction[PATH_MAX_POINTS];
    STRING m_command[PATH_MAX_POINTS];
    bool m_deletePoint[PATH_MAX_POINTS];
    bool m_insertPoint[PATH_MAX_POINTS];
    int m_numPoints;
    bool m_dirty;
    bool m_displayDirty;
    // How far along the path each point is.
    float m_distance[PATH_MAX_POINTS];
    // How far along its segment each measured point is, as a fraction.
    float m_arcLength[PATH_MAX_POINTS][PATH_ARC_STEPS];
};

#endif
