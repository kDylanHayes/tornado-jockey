#include <string.h>

#include "Tongas2/APPWINDOW.h"
#include "Tongas2/INPUT.h"
#include "Tongas2/LOG.h"
#include "Tongas2/TIMER.h"

INPUT *CreateInput();

INPUT *(*g_pfnCreateInput)() = CreateInput;

// The handlers DispatchMouse and DispatchKey call, by number.
static bool (MOUSELISTENER::*s_mouseHandlers[])(MOUSESTATE) = {
    &MOUSELISTENER::OnMouseMove,
    &MOUSELISTENER::OnLeftDown,
    &MOUSELISTENER::OnLeftUp,
    &MOUSELISTENER::OnLeftDoubleClick,
    &MOUSELISTENER::OnLeftHeld,
    &MOUSELISTENER::OnLeftDrag,
    &MOUSELISTENER::OnRightDown,
    &MOUSELISTENER::OnRightUp,
    &MOUSELISTENER::OnRightDoubleClick,
    &MOUSELISTENER::OnRightHeld,
    &MOUSELISTENER::OnRightDrag,
    &MOUSELISTENER::OnMiddleDown,
    &MOUSELISTENER::OnMiddleUp,
    &MOUSELISTENER::OnMiddleDoubleClick,
    &MOUSELISTENER::OnMiddleHeld,
    &MOUSELISTENER::OnMiddleDrag,
    &MOUSELISTENER::OnWheelUp,
    &MOUSELISTENER::OnWheelDown,
};

static bool (INPUTLISTENER::*s_keyHandlers[])(unsigned int) = {
    &INPUTLISTENER::OnKeyDown,
    &INPUTLISTENER::OnKeyUp,
    &INPUTLISTENER::OnChar,
    &INPUTLISTENER::OnKeyDown,
};

INPUT *g_pInput;

// How often a held mouse button repeats, in seconds.
#define MOUSE_REPEAT 0.1f

void JOY::Activate(bool on)
{
    if (m_active != on) {
        unsigned int i;
        for (i = 0; i < m_numButtons; i++) {
            m_buttons[i] = 0;
            m_wasDown[i] = false;
        }
        for (i = 0; i < m_numAxes; i++)
            m_axes[i] = 0;
        m_active = on;
    }
}

void INPUT::DispatchMouse(int handler, MOUSESTATE state)
{
    bool taken = false;
    for (int priority = m_topPriority; priority >= 0; priority--) {
        for (int i = 0; i < m_numMouseListeners; i++) {
            if (!m_mouseListeners[i] || !(m_mouseTimes[i] < g_pTimer->m_soundTime))
                continue;
            bool send = false;
            if (m_exclusive) {
                if (m_mousePriorities[i] == -2)
                    send = true;
            } else if (m_mousePriorities[i] == priority || m_mousePriorities[i] == -1) {
                send = true;
            }
            if (m_topPriority == priority)
                m_mouseSent[i] = false;
            else if (m_mouseSent[i])
                continue;
            if (send) {
                m_mouseSent[i] = true;
                if ((m_mouseListeners[i]->*s_mouseHandlers[handler])(state) && m_mousePriorities[i] != -1)
                    taken = true;
            }
        }
        if (taken)
            return;
    }
}

void INPUT::DispatchKey(int handler, unsigned int key)
{
    bool taken = false;
    for (int priority = m_topPriority; priority >= 0; priority--) {
        for (int i = 0; i < m_numKeyListeners; i++) {
            if (!m_keyListeners[i] || !(m_keyTimes[i] < g_pTimer->m_soundTime))
                continue;
            bool send = false;
            if (m_exclusive) {
                if (m_keyPriorities[i] == -2)
                    send = true;
            } else if (m_keyPriorities[i] == m_topPriority || m_keyPriorities[i] == -1) {
                send = true;
            }
            if (m_topPriority == priority)
                m_keySent[i] = false;
            else if (m_keySent[i])
                continue;
            if (send) {
                m_keySent[i] = true;
                if ((m_keyListeners[i]->*s_keyHandlers[handler])(key))
                    taken = true;
            }
        }
        if (taken)
            return;
    }
}

INPUT::INPUT()
{
    memset(m_keys, 0, sizeof(m_keys));
    m_leftDown = false;
    m_leftTime = 0;
    m_leftHeld = false;
    m_rightDown = false;
    m_rightTime = 0;
    m_rightHeld = false;
    m_middleDown = false;
    m_middleTime = 0;
    m_middleHeld = false;
    m_mouseCapture = false;
    m_numMouseListeners = 0;
    m_numKeyListeners = 0;
    m_numJoyListeners = 0;
    m_4 = false;
    m_topPriority = 0;
    m_ignoreMove = false;
    m_numJoys = 0;
    m_joys = 0;
}

void INPUT::Update()
{
    if (m_leftDown) {
        m_leftTime += g_pTimer->m_soundDt;
        if (m_leftTime > MOUSE_REPEAT) {
            m_leftHeld = true;
            m_leftTime -= MOUSE_REPEAT;
            DispatchMouse(4, m_mouse);
        }
    }
    if (m_rightDown) {
        m_rightTime += g_pTimer->m_soundDt;
        if (m_rightTime > MOUSE_REPEAT) {
            m_rightHeld = true;
            m_rightTime -= MOUSE_REPEAT;
            DispatchMouse(9, m_mouse);
        }
    }
    if (m_middleDown) {
        m_middleTime += g_pTimer->m_soundDt;
        if (m_middleTime > MOUSE_REPEAT) {
            m_middleHeld = true;
            m_middleTime -= MOUSE_REPEAT;
            DispatchMouse(14, m_mouse);
        }
    }
    for (int i = 0; i < m_numJoys; i++) {
        JOY &joy = m_joys[i];
        for (unsigned int b = 0; b < joy.m_numButtons; b++) {
            if (joy.m_buttons[b] && joy.m_active)
                joy.m_wasDown[b] = true;
            else
                joy.m_wasDown[b] = false;
        }
        UpdateJoy(i);
    }
}

void INPUT::OnMouse(int event, int x, int y)
{
    MOUSESTATE state;
    state.buttons = m_mouse.buttons;
    state.lastX = m_mouse.x;
    state.lastY = m_mouse.y;
    state.x = (float)x / (float)g_pDisplay->GetWidth();
    state.y = (float)y / (float)g_pDisplay->GetHeight();
    if (g_pDisplay->NeedsAspectFix())
        g_pDisplay->UnfixY(&state.x);
    switch (event) {
    case INPUTMOUSE_MOVE:
        if (m_ignoreMove) {
            m_ignoreMove = false;
            return;
        }
        DispatchMouse(0, state);
        if (m_leftHeld)
            DispatchMouse(5, state);
        if (m_rightHeld)
            DispatchMouse(10, state);
        if (m_middleHeld)
            DispatchMouse(15, state);
        if (m_mouseCapture) {
            m_ignoreMove = true;
            g_pDisplay->MoveCursor(g_pDisplay->GetWidth() / 2, g_pDisplay->GetHeight() / 2);
        }
        break;
    case INPUTMOUSE_LDBLCLK:
        DispatchMouse(3, state);
        break;
    case INPUTMOUSE_LDOWN:
        state.buttons |= 1;
        m_leftDown = true;
        m_leftTime = 0;
        m_leftHeld = false;
        DispatchMouse(1, state);
        break;
    case INPUTMOUSE_LUP:
        state.buttons &= ~1;
        m_leftDown = false;
        m_leftHeld = false;
        DispatchMouse(2, state);
        break;
    case INPUTMOUSE_RDBLCLK:
        DispatchMouse(8, state);
        break;
    case INPUTMOUSE_RDOWN:
        state.buttons |= 2;
        m_rightDown = true;
        m_rightTime = 0;
        m_rightHeld = false;
        DispatchMouse(6, state);
        break;
    case INPUTMOUSE_RUP:
        state.buttons &= ~2;
        m_rightDown = false;
        m_rightHeld = false;
        DispatchMouse(7, state);
        break;
    case INPUTMOUSE_MDBLCLK:
        DispatchMouse(13, state);
        break;
    case INPUTMOUSE_MDOWN:
        state.buttons |= 4;
        m_middleDown = true;
        m_middleTime = 0;
        m_middleHeld = false;
        DispatchMouse(11, state);
        break;
    case INPUTMOUSE_MUP:
        state.buttons &= ~4;
        m_middleDown = false;
        m_middleHeld = false;
        DispatchMouse(12, state);
        break;
    case INPUTMOUSE_WHEELUP:
        DispatchMouse(16, state);
        break;
    case INPUTMOUSE_WHEELDOWN:
        DispatchMouse(17, state);
        break;
    }
    m_mouse = state;
}

void INPUT::OnKey(unsigned int key, int event)
{
    unsigned int c = key;
    if (event == INPUTKEY_CHAR) {
        DispatchKey(2, c);
        return;
    }
    if ((int)key < 0)
        key = 0;
    else if ((int)key > 0xff)
        key = 0xff;
    if (event == INPUTKEY_DOWN) {
        if (!m_keys[key]) {
            m_keys[key] = true;
            DispatchKey(0, c);
        }
    } else if (event == INPUTKEY_UP) {
        if (m_keys[key]) {
            m_keys[key] = false;
            DispatchKey(1, c);
        }
    }
}

bool INPUT::IsKeyDown(int key)
{
    return m_keys[key] != 0;
}

void INPUT::UpdateTopPriority()
{
    int i;
    m_topPriority = 0;
    m_exclusive = false;
    for (i = 0; i < m_numMouseListeners; i++) {
        if (m_mouseListeners[i]) {
            if (m_topPriority < m_mousePriorities[i])
                m_topPriority = m_mousePriorities[i];
            if (m_mousePriorities[i] == -2)
                m_exclusive = true;
        }
    }
    for (i = 0; i < m_numKeyListeners; i++) {
        if (m_keyListeners[i]) {
            if (m_topPriority < m_keyPriorities[i])
                m_topPriority = m_keyPriorities[i];
            if (m_keyPriorities[i] == -2)
                m_exclusive = true;
        }
    }
    for (i = 0; i < m_numJoyListeners; i++) {
        if (m_joyListeners[i]) {
            if (m_topPriority < m_joyPriorities[i])
                m_topPriority = m_joyPriorities[i];
            if (m_joyPriorities[i] == -2)
                m_exclusive = true;
        }
    }
}

void INPUT::SetMouseCapture(bool on)
{
    g_pDisplay->CaptureMouse(on);
    m_mouseCapture = on;
}

bool INPUT::IsJoyActive(int id)
{
    if (id > -1 && id < m_numJoys && m_joys[id].m_active)
        return true;
    return false;
}

bool INPUT::IsJoyButtonReleased(int id, unsigned int button)
{
    if (id > -1 && id < m_numJoys) {
        JOY &joy = m_joys[id];
        if (joy.m_active && button < joy.m_numButtons && !joy.m_buttons[button] && joy.m_wasDown[button])
            return true;
    }
    return false;
}

void INPUT::ActivateJoy(int id, bool on)
{
    if (id > -1 && id < m_numJoys) {
        m_joys[id].Activate(on);
        return;
    }
    if (g_pLog)
        g_pLog->Error("INPUT::ActivateJoy - Invalid joy ID");
}

INPUT *CreateInput()
{
    return new INPUT;
}

void INPUT::AddMouseListener(MOUSELISTENER *listener, int priority)
{
    int count = m_numMouseListeners;
    int i;
    for (i = 0; i < m_numMouseListeners; i++) {
        if (m_mouseListeners[i] == listener || !m_mouseListeners[i])
            break;
    }
    if (i >= count) {
        if (count >= MAX_INPUT_LISTENERS)
            return;
        m_numMouseListeners = count + 1;
    }
    m_mouseListeners[i] = listener;
    m_mousePriorities[i] = priority;
    m_mouseTimes[i] = g_pTimer->m_soundTime;
    UpdateTopPriority();
}

void INPUT::RemoveMouseListener(MOUSELISTENER *listener)
{
    for (int i = 0; i < m_numMouseListeners; i++) {
        if (m_mouseListeners[i] == listener) {
            m_mouseListeners[i] = 0;
            UpdateTopPriority();
            return;
        }
    }
}

void INPUT::AddKeyListener(INPUTLISTENER *listener, int priority)
{
    int count = m_numKeyListeners;
    int i;
    for (i = 0; i < m_numKeyListeners; i++) {
        if (m_keyListeners[i] == listener || !m_keyListeners[i])
            break;
    }
    if (i >= count) {
        if (count >= MAX_INPUT_LISTENERS)
            return;
        m_numKeyListeners = count + 1;
    }
    m_keyListeners[i] = listener;
    m_keyPriorities[i] = priority;
    m_keyTimes[i] = g_pTimer->m_soundTime;
    UpdateTopPriority();
}

void INPUT::RemoveKeyListener(INPUTLISTENER *listener)
{
    for (int i = 0; i < m_numKeyListeners; i++) {
        if (m_keyListeners[i] == listener) {
            m_keyListeners[i] = 0;
            UpdateTopPriority();
            return;
        }
    }
}

void MOUSELISTENER::SetPriority(int priority)
{
    if (g_pInput)
        g_pInput->AddMouseListener(this, priority);
}

void MOUSELISTENER::Unregister()
{
    if (g_pInput)
        g_pInput->RemoveMouseListener(this);
}

void INPUTLISTENER::SetPriority(int priority)
{
    if (g_pInput)
        g_pInput->AddKeyListener(this, priority);
}

INPUT::~INPUT()
{
    if (m_joys) {
        delete[] m_joys;
        m_joys = 0;
    }
}

void INPUT::SetNumJoys(int count)
{
    if (count > 0) {
        if (m_joys) {
            delete m_joys;
            m_joys = 0;
        }
        m_joys = new JOY[count];
        m_numJoys = count;
    }
}

MOUSELISTENER::MOUSELISTENER()
{
    if (g_pInput)
        g_pInput->AddMouseListener(this, 0);
}

MOUSELISTENER::~MOUSELISTENER()
{
    if (g_pInput)
        g_pInput->RemoveMouseListener(this);
}

INPUTLISTENER::INPUTLISTENER()
{
    if (g_pInput)
        g_pInput->AddKeyListener(this, 0);
}

INPUTLISTENER::~INPUTLISTENER()
{
    if (g_pInput)
        g_pInput->RemoveKeyListener(this);
}
