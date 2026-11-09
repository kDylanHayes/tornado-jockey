// ACTOR accessors that the original GAMECORE1 translation unit defined.

void ACTOR::SetColliderColor(const RGB &color)
{
    m_colliderColor = color;
}

void ACTOR::SetShowCollider(const bool &show)
{
    m_showCollider = show;
}

void ACTOR::SetAttachBoneResId(const int &id)
{
    m_attachBoneResId = id;
}

void ACTOR::SetAttachPos(const V3 &position)
{
    m_attachPos = position;
}

void ACTOR::SetAttachScale(const V3 &scale)
{
    m_attachScale = scale;
}

void ACTOR::SetAttachOpacity(const float &opacity)
{
    m_attachOpacity = opacity;
}

void ACTOR::SetAttachZSort(const int &zsort)
{
    m_attachZSort = zsort;
}

void ACTOR::SetActivation(const int &activation)
{
    m_activation = activation;
}

void ACTOR::SetDeactivation(const int &deactivation)
{
    m_deactivation = deactivation;
}

void ACTOR::SetOnActivateRunInEditor(const bool &run)
{
    m_onActivateRunInEditor = run;
}

void ACTOR::SetBehavior(const int &behavior)
{
    m_behavior = behavior;
}

void ACTOR::SetVel(const V3 &vel)
{
    m_vel = vel;
}

void ACTOR::SetAngVel(const V3 &vel)
{
    m_angVel = vel;
}

void ACTOR::SetAngAceleration(const V3 &aceleration)
{
    m_angAceleration = aceleration;
}

void ACTOR::SetAceleration(const V3 &aceleration)
{
    m_aceleration = aceleration;
}

void ACTOR::SetPanSounds(const bool &pan)
{
    m_panSounds = pan;
}

void ACTOR::SetMoveWithScreenY(const bool &move)
{
    m_moveWithScreenY = move;
}

void ACTOR::SetPreviewBehavior(const bool &preview)
{
    m_previewBehavior = preview;
}

void ACTOR::SetRotateToMove(const bool &rotate)
{
    m_rotateToMove = rotate;
}

void ACTOR::SetForcedCamDist(const float &distance)
{
    m_forcedCamDist = distance;
}

void ACTOR::SetForceInScreen(const bool &force)
{
    m_forceInScreen = force;
}

void ACTOR::SetPathBehavior(const int &behavior)
{
    m_pathBehavior = behavior;
}

void ACTOR::SetVectorUp(const V3 &up)
{
    m_vectorUp = up;
}

void ACTOR::SetVectorForward(const V3 &forward)
{
    m_vectorForward = forward;
}

void ACTOR::SetLoop(const bool &loop)
{
    m_loop = loop;
}

void ACTOR::SetFlowAngle(const float &angle)
{
    m_flowAngle = angle;
}

void ACTOR::SetFlowRadius(const float &radius)
{
    m_flowRadius = radius;
}

void ACTOR::SetPathRelativePos(const bool &relative)
{
    m_pathRelativePos = relative;
}

void ACTOR::SetAlsoLineal(const bool &lineal)
{
    m_alsoLineal = lineal;
}

void ACTOR::SetOpacity(const float &opacity)
{
    m_opacity = opacity;
}

void ACTOR::SetStartFirstAnim(const bool &start)
{
    m_startFirstAnim = start;
}

void ACTOR::SetAnimSpeed(const float &speed)
{
    m_animSpeed = speed;
}

void ACTOR::SetLightMask(const int &mask)
{
    m_lightMask = mask;
}

void ACTOR::SetCheckRenderFrustrum(const bool &check)
{
    m_checkRenderFrustrum = check;
}

void ACTOR::SetRenderNotifications(const bool &notify)
{
    m_renderNotifications = notify;
}

V3 ACTOR::GetPosition()
{
    return m_position;
}

ANG ACTOR::GetAngles()
{
    return m_angles;
}

V3 ACTOR::GetScale()
{
    return m_scale;
}

V3 operator+(const V3 &a, const V3 &b)
{
    return V3(a.x + b.x, a.y + b.y, a.z + b.z);
}

V3 operator-(const V3 &a, const V3 &b)
{
    return V3(a.x - b.x, a.y - b.y, a.z - b.z);
}

V3 operator*(const V3 &v, const float &s)
{
    return V3(v.x * s, v.y * s, v.z * s);
}

V3 operator*(const float &s, const V3 &v)
{
    return V3(s * v.x, s * v.y, s * v.z);
}

V3 operator*(const V3 &a, const V3 &b)
{
    return V3(a.x * b.x, a.y * b.y, a.z * b.z);
}

V2 operator*(const V2 &v, const float &f)
{
    return V2(v.x * f, v.y * f);
}

V2 operator/(const V2 &v, const float &f)
{
    if (f == 0.0f)
        return V2(0.0f, 0.0f);
    float r = 1.0f / f;
    return V2(r * v.x, r * v.y);
}

V3 ANG::GetDirection()
{
    if (flags & MATRIX_DIRTY) {
        matrix.FromEuler(euler.z, euler.y, euler.x);
        flags &= ~MATRIX_DIRTY;
    }
    return V3(matrix._41 + matrix._31, matrix._42 + matrix._32, matrix._43 + matrix._33);
}

V3 ANG::GetUpPoint()
{
    if (flags & MATRIX_DIRTY) {
        matrix.FromEuler(euler.z, euler.y, euler.x);
        flags &= ~MATRIX_DIRTY;
    }
    return V3(matrix._41 + matrix._21, matrix._42 + matrix._22, matrix._43 + matrix._23);
}

MATRIX ANG::GetMatrix()
{
    if (flags & MATRIX_DIRTY) {
        matrix.FromEuler(euler.z, euler.y, euler.x);
        flags &= ~MATRIX_DIRTY;
    }
    return matrix;
}

void ACTOR::SetOnActivate(const CSTRING &name)
{
    m_onActivate = name;
}

V3 ACTOR::GetDirection()
{
    if (m_angles.flags & ANG::MATRIX_DIRTY) {
        m_angles.matrix.FromEuler(m_angles.euler.z, m_angles.euler.y, m_angles.euler.x);
        m_angles.flags &= ~ANG::MATRIX_DIRTY;
    }
    return V3(m_angles.matrix._41 + m_angles.matrix._31,
              m_angles.matrix._42 + m_angles.matrix._32,
              m_angles.matrix._43 + m_angles.matrix._33);
}

V3 ACTOR::GetUp()
{
    if (m_angles.flags & ANG::MATRIX_DIRTY) {
        m_angles.matrix.FromEuler(m_angles.euler.z, m_angles.euler.y, m_angles.euler.x);
        m_angles.flags &= ~ANG::MATRIX_DIRTY;
    }
    return V3(m_angles.matrix._41 + m_angles.matrix._21,
              m_angles.matrix._42 + m_angles.matrix._22,
              m_angles.matrix._43 + m_angles.matrix._23);
}

void OBJECT::Detach()
{
    m_parent.Invalidate();
    if (m_attached.IsValid())
        m_attached.Invalidate();
    if (m_tag) {
        delete m_tag;
        m_tag = 0;
    }
    Virtual31();
}

void OBJECT::SetParent(RESHANDLE parent)
{
    if (m_parent.IsValid())
        Detach();
    m_parent = parent;
    Virtual30();
}

float V3::Distance(const V3 &o) const
{
    float dx = o.x - x;
    float dy = o.y - y;
    float dz = o.z - z;
    return (float)sqrt(dx * dx + dy * dy + dz * dz);
}

template <class T, int N, int H, int KIND>
T *SAFEINDEX<T, N, H, KIND>::GetFirst(int *index)
{
    for (int i = 0; i < N; i++) {
        if (m_items[i]) {
            *index = i;
            return m_items[i];
        }
    }
    *index = -1;
    return 0;
}

template <class T, int N, int H, int KIND>
T *SAFEINDEX<T, N, H, KIND>::GetNext(int *index)
{
    for (int i = *index + 1; i < N; i++) {
        if (m_items[i]) {
            *index = i;
            return m_items[i];
        }
    }
    *index = -1;
    return 0;
}

template ACTOR *SAFEINDEX<ACTOR, 5000, 0xfb, 1>::GetFirst(int *);
template ACTOR *SAFEINDEX<ACTOR, 5000, 0xfb, 1>::GetNext(int *);

extern CMDBASE *g_pScriptRoot;

int RESOURCES::Virtual08()
{
    return (int)(size_t)g_pScriptRoot;
}

int ACTOR::Virtual08()
{
    return (int)(size_t)g_pScene;
}

int ACTOR::GetFactoryId()
{
    return s_factoryId;
}

V2 V2::Normalized() const
{
    float length = (float)sqrt(y * y + x * x);
    if (length == 0.0f)
        return V2(0.0f, 0.0f);
    float r = 1.0f / length;
    return V2(x * r, y * r);
}

V3 operator/(const V3 &v, const float &s)
{
    if (s == 0.0f)
        return V3(0.0f, 0.0f, 0.0f);
    float r = 1.0f / s;
    return V3(r * v.x, r * v.y, r * v.z);
}

float V3::Length() const
{
    return (float)sqrt(x * x + y * y + z * z);
}

bool operator==(const RGB &a, const RGB &b)
{
    return a.r == b.r && a.g == b.g && a.b == b.b;
}

float ANG::GetEulerX()
{
    if (flags & EULER_DIRTY) {
        matrix.ToEuler(&euler.z, &euler.y, &euler.x);
        flags &= ~EULER_DIRTY;
    }
    return euler.x;
}

PLANE::PLANE(const V3 &normal, const V3 &point)
{
    a = 0.0f;
    b = 0.0f;
    c = 0.0f;
    memcpy(&a, &normal, sizeof(V3));
    d = -(a * point.x + b * point.y + c * point.z);
}

template void DYNAMICARRAY<int>::Clear();

void MATRIX::RotateY(float degrees)
{
    float radians = -degrees;
    radians *= 0.017453292f;
    float c = (float)cos(radians);
    float s = (float)sin(radians);
    _11 = c;
    _12 = 0.0f;
    _13 = s;
    _14 = 0.0f;
    _21 = 0.0f;
    _22 = 1.0f;
    _23 = 0.0f;
    _24 = 0.0f;
    _31 = -s;
    _32 = 0.0f;
    _33 = c;
    _34 = 0.0f;
    _41 = 0.0f;
    _42 = 0.0f;
    _43 = 0.0f;
    _44 = 1.0f;
}
