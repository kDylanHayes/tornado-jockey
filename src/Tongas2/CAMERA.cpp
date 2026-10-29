#include "Tongas2/CAMERA.h"
#include "Tongas2/CSTRING.h"
#include "Tongas2/APPWINDOW.h"
#include "Tongas2/GUI.h"
#include "Tongas2/RESOURCE.h"
#include "Tongas2/TIMER.h"
#include "Tongas2/TOKENS.h"

#include <stdio.h>
#include <stdlib.h>

#define CMD_FUNC(CLASS, method) (CMDFUNC)(SRESULT (CLASS::*)(TOKENS **))(SRESULT (CLASS::*)(CMDCALL *))&CLASS::method


static CAMERALENS *CreateCameraLens();
static CAMERA *CreateCamera();

CAMERALENS *(*g_pfnCreateCameraLens)() = CreateCameraLens;
CAMERA *(*g_pfnCreateCamera)() = CreateCamera;

CMDENTRY CAMERA::s_commands[] = {
    { "jitter", 0, 2, 2, CMD_FUNC(CAMERA, Jitter) },
    { "panTo", 0, 2, 2, CMD_FUNC(CAMERA, PanTo) },
    { "rotateTo", 0, 2, 2, CMD_FUNC(CAMERA, RotateTo) },
    { 0, 0, 0, 0, 0 },
};

PROPENTRY CAMERA::s_properties[] = {
    { "Fov", 0, 3, 0, PROP_FIELD(CAMERA, m_fov), false, 1, PROP_SETTER(CAMERA, SetFov) },
    { "ReflectionFov", 0, 3, 0, PROP_FIELD(CAMERA, m_reflectionFov), false, 1, PROP_SETTER(CAMERA, SetReflectionFov) },
    { "Clipping", 0, 3, 0, PROP_FIELD(CAMERA, m_clipping), false, 1, PROP_SETTER(CAMERA, SetClipping) },
    { "PausedMovement", 0, 2, 0, PROP_FIELD(CAMERA, m_pausedMovement), false, 1, PROP_SETTER(CAMERA, SetPausedMovement) },
    { "UseOrthoProjection", 0, 2, 0, PROP_FIELD(CAMERA, m_useOrthoProjection), false, 1, PROP_SETTER(CAMERA, SetUseOrthoProjection) },
    { "OrthoBoxWidth", 0, 3, 0, PROP_FIELD(CAMERA, m_orthoBoxWidth), false, 1, PROP_SETTER(CAMERA, SetOrthoBoxWidth) },
    { 0, 0, 15, 0, 0, false },
};

void CAMERA::RegisterCommands()
{
    g_classParents[s_classId] = ACTOR::s_classId;
    g_classCommands[s_classId] = s_commands;
    HashCommands(s_commands);
    RegisterClassName(s_commands, "CAMERA", "ACTOR");
}

CAMERA::CAMERA()
    : m_ce0(0.0f, 0.0f, 0.0f), m_cec(0.0f, 0.0f, 0.0f), m_jitter(0.0f, 0.0f, 0.0f),
      m_panTarget(0.0f, 0.0f, 0.0f), m_rotateTarget(0.0f, 0.0f, 0.0f), m_up(0.0f, 0.0f, 0.0f),
      m_direction(0.0f, 0.0f, 0.0f)
{
    m_initialized = false;
    m_cd0.x = 0.0f;
    m_cd0.y = 0.0f;
    m_cd0.z = 0.0f;
    m_cdc = false;
    m_ce0.x = 0.0f;
    m_ce0.y = 2000.0f;
    m_ce0.z = 0.0f;
    m_cec.x = 0.0f;
    m_cec.y = 0.0f;
    m_cec.z = 0.0f;
    m_jitterTime = 0.0f;
    m_jitterAmount = 0.0f;
    m_panTime = 0.0f;
    m_rotateTime = 0.0f;
    m_viewHeight = 1.0f;
    m_savedFov = 60.0f;
    m_up.x = 0.0f;
    m_up.y = 1.0f;
    m_up.z = 0.0f;
    m_direction.x = 0.0f;
    m_direction.y = 0.0f;
    m_direction.z = 1.0f;
    m_fov = 84.0f;
    m_reflectionFov = 60.0f;
    m_clipping = 500.0f;
    m_pausedMovement = false;
    m_useOrthoProjection = false;
    m_orthoBoxWidth = 50.0f;
    m_lens = g_pfnCreateCameraLens();
}

CAMERA::~CAMERA()
{
    if (m_lens) {
        delete m_lens;
        m_lens = 0;
    }
}

void CAMERA::StopJitter()
{
    m_jitterTime = 0.0f;
    m_jitterAmount = 0.0f;
    m_jitter.x = m_jitter.y = m_jitter.z = 0.0f;
}

void CAMERA::ResetMotion(V3 position)
{
    StopJitter();
    StopPan();
    StopRotate();
    SetPosition(position);
    SetAngles(ANG(0.0f, 0.0f, 0.0f));
}

void CAMERA::SetPosition(const V3 &position)
{
    ACTOR::SetPosition(position);
    m_lens->SetPosition(position);
}

void CAMERA::SetFov(const float &fov)
{
    m_fov = fov;
    m_viewHeight = 2.0f * (float)tan(DEG2RAD(m_fov * 0.5f));
    m_lens->SetFov(fov);
}

void CAMERA::SetReflectionFov(const float &fov)
{
    m_reflectionFov = fov;
    m_lens->SetReflectionFov(fov);
}

void CAMERA::WorldToScreen(V3 point, V2 *screen)
{
    if (screen) {
        *screen = ((CAMERA *)g_pCamera)->m_lens->Project(point);
        screen->x /= g_pDisplay->GetWidth();
        screen->y /= g_pDisplay->GetHeight();
    }
}

void CAMERA::SetUseOrthoProjection(const bool &ortho)
{
    if (m_useOrthoProjection != ortho) {
        if (ortho) {
            m_lens->SetOrtho(true);
            m_savedFov = m_fov;
            SetFov(0.0f);
        } else {
            m_lens->SetOrtho(false);
            SetFov(m_savedFov);
        }
        m_useOrthoProjection = ortho;
    }
}

float CAMERALENS::GetFar()
{
    return m_far;
}

float CAMERALENS::GetNear()
{
    return m_near;
}

void CAMERALENS::UpdateBillboard()
{
    MATRIX m = m_view;
    m.Invert();
    m_billboard = m;
    m_billboard._41 = 0.0f;
    m_billboard._42 = 0.0f;
    m_billboard._43 = 0.0f;
    m_billboard._14 = 0.0f;
    m_billboard._24 = 0.0f;
    m_billboard._34 = 0.0f;
    m_billboardDirty = false;
}

void CAMERALENS::UpdateFrustum()
{
    m_frustum.Build(m_inverseViewProj, m_billboard);
    m_frustumDirty = false;
}

V2 CAMERA::GetViewSize()
{
    float width = (float)g_pDisplay->GetWidth();
    float height = (float)g_pDisplay->GetHeight();
    if (g_pDisplay->NeedsAspectFix())
        g_pDisplay->FixHeight(&width);
    float x = 1.0f;
    float y = 1.0f;
    if (width > height)
        y = height / width;
    else if (width < height)
        x = width / height;
    return V2(x * m_viewHeight, y * m_viewHeight);
}

void MATRIX::Perspective(float fov, float aspect, float zNear, float zFar)
{
    float half = fov * 0.5f;
    float c = (float)cos(half);
    float s = (float)sin(half);
    float cot = c / s;
    _11 = cot / aspect;
    _12 = 0.0f;
    _13 = 0.0f;
    _14 = 0.0f;
    _21 = 0.0f;
    _22 = cot;
    _23 = 0.0f;
    _24 = 0.0f;
    _31 = 0.0f;
    _32 = 0.0f;
    _33 = zFar / (zFar - zNear);
    _34 = 1.0f;
    _41 = 0.0f;
    _42 = 0.0f;
    _43 = zNear * zFar / (zNear - zFar);
    _44 = 0.0f;
}

static inline V3 Unit(V3 v)
{
    v.Normalize();
    return v;
}

static inline V3 Cross(const V3 &a, const V3 &b)
{
    return V3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

static inline float Dot(const V3 &a, const V3 &b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

static inline void Set(MATRIX &m, float _11, float _12, float _13, float _14, float _21, float _22, float _23,
                       float _24, float _31, float _32, float _33, float _34, float _41, float _42, float _43,
                       float _44)
{
    m._11 = _11;
    m._12 = _12;
    m._13 = _13;
    m._14 = _14;
    m._21 = _21;
    m._22 = _22;
    m._23 = _23;
    m._24 = _24;
    m._31 = _31;
    m._32 = _32;
    m._33 = _33;
    m._34 = _34;
    m._41 = _41;
    m._42 = _42;
    m._43 = _43;
    m._44 = _44;
}

void MATRIX::View(const V3 &eye, const V3 &at, const V3 &up)
{
    V3 z = Unit(at - eye);
    V3 x = Unit(Cross(up, z));
    V3 y = Cross(z, x);
    Set(*this, x.x, y.x, z.x, 0.0f, x.y, y.y, z.y, 0.0f, x.z, y.z, z.z, 0.0f, x.z * eye.z * -1.0f - x.y * eye.y - x.x * eye.x,
        y.z * eye.z * -1.0f - y.y * eye.y - y.x * eye.x, z.z * eye.z * -1.0f - z.y * eye.y - z.x * eye.x, 1.0f);
}

float MATRIX::Determinant()
{
    MATRIX a;
    a = Adjoint();
    return _14 * a._14 + _13 * a._13 + _11 * a._11 + _12 * a._12;
}

MATRIX MATRIX::Inverse()
{
    MATRIX m;
    m = Adjoint();
    m.Invert();
    MATRIX t = m;
    return t / Determinant();
}

static CAMERA *CreateCamera()
{
    return new CAMERA;
}

void CAMERA::Reset()
{
    if (!m_initialized) {
        m_jitterTime = 0.0f;
        m_jitterAmount = 0.5f;
        m_initialized = true;
        SetClipping(1500.0f);
        SetPosition(V3(0.0f, 0.0f, 0.0f));
        SetAngles(ANG(0.0f, 0.0f, 0.0f));
        SetName(CSTRING("camera"));
        SetFov(60.0f);
        ResetMotion(V3(0.0f, 0.0f, 0.0f));
    }
}

V3 CAMERA::ScreenToWorld(const V2 &point, const float &depth)
{
    if (m_useOrthoProjection) {
        float width = m_orthoBoxWidth;
        float height = (float)g_pDisplay->GetHeight() * m_orthoBoxWidth / (float)g_pDisplay->GetWidth();
        V3 r = LocalToWorld(V3((point.x - 0.5f) * width, -((point.y - 0.5f) * height), 0.0f));
        return r;
    }
    float t = (float)tan(DEG2RAD(m_fov * 0.5f));
    float x = point.x * 2.0f - 1.0f;
    float y = point.y * 2.0f - 1.0f;
    float aspect = (float)g_pDisplay->GetHeight() / (float)g_pDisplay->GetWidth();
    x = x * t;
    y = y * aspect * t;
    if (g_pDisplay->NeedsAspectFix())
        g_pDisplay->Virtual33(&y);
    V3 v(x * depth, -y * depth, depth);
    MATRIX m;
    m.LookAt(GetUp(), GetDirection());
    V3 w = v * m;
    V3 r = w + GetPosition();
    return r;
}

FRUSTUM::FRUSTUM()
{
    m_c0.x = 0.0f;
    m_c0.y = 0.0f;
    m_c0.z = 0.0f;
}

void CAMERALENS::UpdateProjection()
{
    if (m_ortho)
        m_projection.Ortho(m_orthoWidth, m_orthoWidth / m_aspect, m_near, m_far);
    else
        m_projection.Perspective(m_fovY, m_aspect, m_near, m_far);
    m_projDirty = false;
}

void CAMERALENS::UpdateView()
{
    m_view.View(V3(m_position), V3(m_position) + m_direction, m_up);
    m_viewDirty = false;
}

void CAMERALENS::Update(bool force)
{
    bool changed = false;
    if (force || m_projDirty) {
        UpdateProjection();
        m_frustumDirty = true;
        changed = true;
    }
    if (force || m_viewDirty) {
        UpdateView();
        m_frustumDirty = true;
        m_billboardDirty = true;
        changed = true;
    }
    if (force || m_billboardDirty)
        UpdateBillboard();
    if (force || changed) {
        m_inverseViewProj = m_view * m_projection;
        m_inverseViewProj = m_inverseViewProj.Inverse();
        m_frustumDirty = true;
    }
    if (force || m_frustumDirty)
        UpdateFrustum();
}

void CAMERA::Update()
{
    if (m_9a1)
        SetFov(60.0f);
    m_lens->Virtual13();
    ACTOR::Update();
    if (m_jitterTime > 0.0f) {
        float r = m_jitterAmount * 2.0f > 0.0f ? rand() * (m_jitterAmount * 2.0f) * (1.0f / RAND_MAX) : 0.0f;
        m_jitter.x = 0.0f;
        m_jitter.z = 0.0f;
        m_jitter.y = r - m_jitterAmount;
        SetPosition(m_position + m_jitter);
        m_jitterTime -= g_pTimer->m_dt;
    }
    if (g_pGui)
        g_pGui->UpdateLayout();
    m_lens->Update(false);
}

void CAMERA::Virtual103()
{
    m_lens->Virtual13();
    m_lens->Update(false);
}

void CAMERA::SetAngles(const ANG &angles)
{
    ACTOR::SetAngles(angles);
    m_transform.GetAxes(&m_direction, &m_up);
    m_lens->SetOrientation(m_direction, m_up);
}

SRESULT CAMERA::PanTo(CMDCALL *call)
{
    V3 target(0.0f, 0.0f, 0.0f);
    sscanf(call->tokens->GetToken(1), "%f %f %f", &target.x, &target.y, &target.z);
    m_panTime = (float)atof(call->tokens->GetToken(2));
    m_panTarget = target;
    return SRESULT(true);
}

SRESULT CAMERA::RotateTo(CMDCALL *call)
{
    float x, y, z = 0.0f;
    sscanf(call->tokens->GetToken(1), "%f %f %f", &x, &y, &z);
    m_rotateTime = (float)atof(call->tokens->GetToken(2));
    m_rotateTarget = ANG(x, y, z);
    return SRESULT(true);
}

SRESULT CAMERA::Jitter(CMDCALL *call)
{
    float amount = (float)atof(call->tokens->GetToken(2));
    m_jitterTime = (float)atof(call->tokens->GetToken(1));
    m_jitterAmount = amount;
    return SRESULT(true);
}

CAMERALENS::CAMERALENS() throw() : m_position(0.0f, 0.0f, 0.0f), m_direction(0.0f, 0.0f, 0.0f), m_up(0.0f, 0.0f, 0.0f)
{
    m_position.x = m_position.y = m_position.z = 0.0f;
    m_direction.x = 0.0f;
    m_direction.y = 0.0f;
    m_direction.z = 1.0f;
    m_up.x = 0.0f;
    m_up.y = 1.0f;
    m_up.z = 0.0f;
    m_orthoWidth = 1.0f;
    m_viewDirty = true;
    m_billboardDirty = true;
    m_frustumDirty = true;
    m_228 = true;
    m_projDirty = true;
    m_ortho = false;
    m_4c = false;
    m_far = 1000.0f;
    m_near = 0.9f;
    m_aspect = 4.0f / 3.0f;
    m_fov = 30.0f;
    m_reflectionFov = 60.0f;
    m_fovY = VerticalFov(m_fov);
    m_reflectionFovY = VerticalFov(m_reflectionFov);
}

FRUSTUM *CAMERALENS::GetFrustum()
{
    if (m_frustumDirty || m_viewDirty || m_projDirty)
        Update(false);
    return &m_frustum;
}

static CAMERALENS *CreateCameraLens()
{
    return new CAMERALENS;
}
