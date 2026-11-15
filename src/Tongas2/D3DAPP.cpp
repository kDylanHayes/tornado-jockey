// Tongas2/D3DAPP -- the Direct3D 8 application framework.
//
// The retail translation unit spans 0x00506190-0x00508a4f.  It owns the
// application object whose virtual table the retail build places at
// 0x00678978 (nineteen entries) and whose scalar destructor sits at
// 0x00507220.  The class below reproduces that virtual table and the member
// layout the retail code uses; every offset in it is the offset the retail
// disassembly reads.
//
// The scaffold header include/Tongas2/D3DAPP.h cannot describe the virtual
// table (it declares D3DAPP as a plain blob of bytes), so the class is
// spelled out here.  See the notes at the bottom of the file for the header
// and config/symbols.txt changes the reconstruction needs.
//
// The retail names the class D3DAPP; the config's symbol table calls it
// GD507220 (after the address of its destructor), and the plugin's
// DX8APP class in the neighbouring unit derives from that name, so the
// class keeps it here.
#include <string>
#include <windows.h>
#include <d3d8.h>

// Win32 entry points include/windows.h does not declare yet.
extern "C" {
typedef struct HMENU__ *HMENU;
typedef struct tagGD507220MSG
{
    HWND hwnd;
    unsigned int message;
    unsigned int wParam;
    long lParam;
    unsigned int time;
    long x;
    long y;
} MSG;
WINUSERAPI BOOL WINAPI SetWindowTextA(HWND hWnd, LPCSTR lpString);
WINUSERAPI LONG WINAPI SetWindowLongA(HWND hWnd, int nIndex, LONG dwNewLong);
WINUSERAPI LONG WINAPI GetWindowLongA(HWND hWnd, int nIndex);
WINUSERAPI HMENU WINAPI GetMenu(HWND hWnd);
WINUSERAPI BOOL WINAPI SetMenu(HWND hWnd, HMENU hMenu);
WINUSERAPI BOOL WINAPI GetWindowRect(HWND hWnd, LPRECT lpRect);
WINUSERAPI BOOL WINAPI ClipCursor(const RECT *lpRect);
WINUSERAPI int WINAPI MessageBoxA(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption,
                                  UINT uType);
WINUSERAPI BOOL WINAPI PeekMessageA(MSG *lpMsg, HWND hWnd, UINT wMsgFilterMin,
                                    UINT wMsgFilterMax, UINT wRemoveMsg);
WINUSERAPI BOOL WINAPI TranslateMessage(const MSG *lpMsg);
WINUSERAPI LONG WINAPI DispatchMessageA(const MSG *lpMsg);
WINUSERAPI LONG WINAPI SendMessageA(HWND hWnd, UINT msg, unsigned int wParam,
                                    long lParam);
WINUSERAPI BOOL WINAPI SetRect(LPRECT lprc, int xLeft, int yTop, int xRight,
                               int yBottom);
WINUSERAPI BOOL WINAPI AdjustWindowRect(LPRECT lpRect, unsigned int dwStyle,
                                        BOOL bMenu);
WINUSERAPI BOOL WINAPI SetWindowPos(HWND hWnd, HWND hWndInsertAfter, int X,
                                    int Y, int cx, int cy, unsigned int uFlags);
WINUSERAPI HWND WINAPI SetFocus(HWND hWnd);
}

// The message entry points the retail reaches through the engine's own
// import table (0x006562bc and neighbours), so they are pointer variables
// here rather than direct imports: the difference is visible in the message
// pump, which keeps them as direct indirect calls.
typedef BOOL (WINAPI *GD507220PEEKMSG)(MSG *, HWND, UINT, UINT, UINT);
typedef BOOL (WINAPI *GD507220TRANSLATE)(const MSG *);
typedef LONG (WINAPI *GD507220DISPATCH)(const MSG *);
extern GD507220PEEKMSG g_pPeekMessageA;
extern GD507220TRANSLATE g_pTranslateMessage;
extern GD507220DISPATCH g_pDispatchMessageA;

// The engine log.  The retail calls LOG::Error with the log pointer on the
// stack (?Error@LOG@@QAAXPBDZZ), so the method is __cdecl here as well.
class LOG
{
public:
    void __cdecl Error(const char *format, ...);
};

extern LOG *g_pLog;

// The application pointer the constructor installs (retail 0x00a024f4).
class GD507220;
extern GD507220 *g_pApp;

// The display description the mode pickers fill in; the pointers at m_10
// and m_48 and those at m_14 and m_4c both reach it.
class GD507220DISPLAYINFO
{
public:
    char pad_00[0x04];
    int m_4;                     // 0x04, D3DDEVTYPE of the picked device
    char pad_08[0x20];
    unsigned int m_28;           // 0x28, capability flags
    unsigned char m_2c;          // 0x2c, capability flags
};

// The engine's Direct3D object.  Retail reaches EnumAdapterModes through
// vtable+0x20 and CreateDevice through vtable+0x3c.
class GD507220D3D8
{
public:
    virtual int __stdcall V00();
    virtual int __stdcall V01();
    virtual int __stdcall V02();
    virtual int __stdcall V03();
    virtual int __stdcall V04();
    virtual int __stdcall V05();
    virtual int __stdcall V06();
    virtual int __stdcall V07();
    virtual int __stdcall EnumAdapterModes(unsigned int adapter, int format,
                                           void *modes);
    virtual int __stdcall V09();
    virtual int __stdcall V10();
    virtual int __stdcall V11();
    virtual int __stdcall V12();
    virtual int __stdcall V13();
    virtual int __stdcall V14();
    virtual int __stdcall CreateDevice(unsigned int adapter, int type,
                                       HWND window, unsigned int behaviour,
                                       void *params,
                                       IDirect3DDevice8 **device);
};

// The object the application deletes from m_b0.  Its destructor lives in
// another unit (0x00538520).
class GD507220OBJECT
{
public:
    ~GD507220OBJECT();
    // Drops the pending display update (retail 0x00538930).
    void Reset();

    char pad_00[0x28];           // 0x00
    unsigned int m_28;           // 0x28, pending update count
};

// The Direct3D application object.
//
// The virtual order below is the retail virtual table at 0x00678978:
// Create, Run, Initialize3DEnvironment, the destructor, the full screen
// switch, nine empty hooks, two more empty hooks, a false returning hook
// and the two flag hooks.
class GD507220
{
public:
    GD507220();
    virtual int Create(HINSTANCE__ *instance);
    virtual int Run();
    virtual int Initialize3DEnvironment();
    virtual ~GD507220();
    virtual int ToggleFullScreen();
    virtual int Hook01() { return 0; }
    virtual int Hook02() { return 0; }
    virtual int Hook03() { return 0; }
    virtual int Hook04() { return 0; }
    virtual int Hook05() { return 0; }
    virtual int Hook06() { return 0; }
    virtual int Hook07() { return 0; }
    virtual int Hook08() { return 0; }
    virtual int Hook09() { return 0; }
    virtual void Hook10() {}
    virtual void Hook11() {}
    virtual bool Hook12() { return false; }
    virtual int ClearFlag();
    virtual void SetFlag();

    // The window text is written into m_1b4 and pushed into the window.
    void SetText(const std::string &text);
    // Picks the windowed presentation parameters (retail 0x00507510).
    __declspec(noinline) bool SetWindowedMode();
    // Picks the full screen presentation parameters (retail 0x005072c0).
    __declspec(noinline) bool SetFullScreenMode();
    // Re-applies the display mode until both pickers agree (retail 0x00507b30).
    int ApplyMode();
    // Releases the device and tears the 3D environment down.
    void Cleanup3DEnvironment();
    // Reports a failure to the player (retail 0x00506760).
    int ReportError(unsigned int code, int flag);
    // Rebuilds the 3D environment for the current mode (retail 0x00506360).
    int Tail();
    // Picks the new mode up when the window changed size (retail 0x00506a70).
    int HandleSizeChange();
    // One frame step; the message pump calls it while the device is live
    // (retail 0x005065a0).
    int Step();
    // Repairs the 3D environment after a device loss (retail 0x00507ba0).
    void Repair();
    // Builds the presentation parameters for the current mode
    // (retail 0x005061b0).
    void Recreate();
    // Puts the device back after a full screen switch (retail 0x00506fd0).
    int Restore();
    // The current display mode; retail reads it through this two-way pointer.
    int *GetSize() { return m_8 ? (int *)m_14 : (int *)m_4c; }

    int m_4;                     // 0x04
    volatile unsigned char m_8;  // 0x08
    int m_c;                     // 0x0c
    void *m_10;                  // 0x10
    void *m_14;                  // 0x14
    RECT m_18;                   // 0x18
    int m_28;                    // 0x28
    int m_2c;                    // 0x2c
    int m_30;                    // 0x30
    int m_34;                    // 0x34
    int m_38;                    // 0x38
    int m_3c;                    // 0x3c
    int m_40;                    // 0x40
    int m_44;                    // 0x44
    void *m_48;                  // 0x48
    void *m_4c;                  // 0x4c
    RECT m_50;                   // 0x50
    int m_60;                    // 0x60
    int m_64;                    // 0x64
    int m_68;                    // 0x68
    int m_6c;                    // 0x6c
    int m_70;                    // 0x70
    int m_74;                    // 0x74
    int m_78;                    // 0x78
    int m_7c;                    // 0x7c
    int m_80;                    // 0x80
    int m_84;                    // 0x84
    HWND m_88;                   // 0x88
    int m_8c;                    // 0x8c
    int m_90;                    // 0x90
    int m_94;                    // 0x94
    int m_98;                    // 0x98
    int m_9c;                    // 0x9c
    int m_a0;                    // 0xa0
    GD507220D3D8 *m_a4;          // 0xa4
    IDirect3DDevice8 *m_a8;      // 0xa8, the Direct3D device
    int m_ac;                    // 0xac
    GD507220OBJECT *m_b0;        // 0xb0
    char m_b4[0x188 - 0xb4];     // 0xb4, the device caps the reset reads
    char m_188[0x20];            // 0x188, the surface description the reset keeps
    HWND m_1a8;                  // 0x1a8, the window
    HMENU m_1ac;                 // 0x1ac, the saved menu
    LONG m_1b0;                  // 0x1b0, the saved window style
    std::string m_1b4;           // 0x1b4
    int m_1d0;                   // 0x1d0
    int m_1d4;                   // 0x1d4
    RECT m_1d8;                  // 0x1d8
    RECT m_1e8;                  // 0x1e8
    bool m_1f8;                  // 0x1f8
    bool m_1f9;                  // 0x1f9
    bool m_1fa;                  // 0x1fa
    bool m_1fb;                  // 0x1fb
    unsigned char m_1fc;         // 0x1fc, full screen
    bool m_1fd;                  // 0x1fd
    bool m_1fe;                  // 0x1fe
    bool m_1ff;                  // 0x1ff
    bool m_200;                  // 0x200
    bool m_201;                  // 0x201
    bool m_202;                  // 0x202
    bool m_203;                  // 0x203
    bool m_204;                  // 0x204
    bool m_205;                  // 0x205
    bool m_206;                  // 0x206
    bool m_207;                  // 0x207
    bool m_208;                  // 0x208
    bool m_209;                  // 0x209
    bool m_20a;                  // 0x20a
    bool m_20b;                  // 0x20b
    bool m_20c;                  // 0x20c
    bool m_20d;                  // 0x20d
    int m_210;                   // 0x210
    int m_214;                   // 0x214
    bool m_218;                  // 0x218
    int m_21c;                   // 0x21c
};

// The constructor (retail 0x005081a0).
GD507220::GD507220()
    : m_a4(0), m_a8(0), m_1a8(0), m_1ac(0), m_1b4("GAME", 4)
{
    m_1fc = true;
    m_20c = true;
    m_20d = true;
    m_205 = true;
    m_1f8 = false;
    m_1f9 = false;
    m_1fa = false;
    m_1fb = false;
    m_1fd = false;
    m_1fe = false;
    m_1ff = false;
    m_200 = false;
    m_201 = false;
    m_202 = false;
    m_203 = false;
    m_204 = false;
    m_206 = false;
    m_207 = false;
    m_208 = false;
    m_209 = false;
    m_20a = false;
    m_20b = false;
    m_210 = 0;
    m_214 = 0;
    m_218 = false;
    m_b0 = 0;
    m_1d0 = 640;
    m_1d4 = 480;
    g_pApp = this;
    m_4 = 2;
}

// The destructor (retail 0x00507220).
GD507220::~GD507220()
{
    if (m_b0) {
        delete m_b0;
        m_b0 = 0;
    }
}

// The flag hooks at the end of the virtual table (retail 0x005072b0 and
// 0x004481d0).
int GD507220::ClearFlag()
{
    m_1f8 = false;
    return 0;
}

void GD507220::SetFlag()
{
    m_1f8 = true;
}

// Writes the window caption (retail 0x00508a00).
void GD507220::SetText(const std::string &text)
{
    m_1b4.assign(text, 0, std::string::npos);
    if (m_1a8)
        SetWindowTextA(m_1a8, m_1b4.c_str());
}

// Switches between full screen and windowed mode (retail 0x00506510).
int GD507220::ToggleFullScreen()
{
    if (m_1fc) {
        SetWindowLongA(m_1a8, -16, m_1b0);
        if (m_1ac) {
            SetMenu(m_1a8, m_1ac);
            m_1ac = 0;
        }
    } else {
        SetWindowLongA(m_1a8, -16, 0x90080000);
        if (!m_1ac) {
            m_1ac = GetMenu(m_1a8);
            SetMenu(m_1a8, 0);
        }
    }
    return 0;
}

// Handles a change of the window size (retail 0x00506a70).
int GD507220::HandleSizeChange()
{
    int ret = 0;
    RECT old = m_1e8;
    if (m_201)
        return ret;
    GetWindowRect(m_1a8, &m_1d8);
    GetClientRect(m_1a8, &m_1e8);
    if (old.right - old.left != m_1e8.right - m_1e8.left ||
        old.bottom - old.top != m_1e8.bottom - m_1e8.top) {
        m_70 = m_1e8.right - m_1e8.left;
        m_1fd = false;
        m_74 = m_1e8.bottom - m_1e8.top;
        if (m_a8) {
            ret = Tail();
            if (ret < 0) {
                if (ret != 0x8876017c)
                    ret = 0x8200000c;
                if (g_pLog)
                    g_pLog->Error("Failed on HandlePossibleSizeChange");
                ReportError(ret, 1);
            }
        }
        m_1fd = true;
    }
    return ret;
}

// The message pump and frame loop (retail 0x00508040).
int GD507220::Run()
{
    MSG msg;
    msg.message = 0;
    g_pPeekMessageA(&msg, 0, 0, 0, 0);
    bool got;
    while (msg.message != 0x12) {
        if (m_1fd)
            got = g_pPeekMessageA(&msg, 0, 0, 0, 1) != 0;
        if (got) {
            if (msg.message == 0x12)
                break;
            g_pTranslateMessage(&msg);
            g_pDispatchMessageA(&msg);
        } else {
            if (m_1fb)
                continue;
            if (m_1fe)
                Sleep(100);
            if (!m_1f8)
                Sleep(100);
            if (m_1fd) {
                if (Step() < 0) {
                    if (++m_210 > 2) {
                        m_1fb = true;
                        SendMessageA(m_1a8, 0x10, 0, 0);
                    }
                } else {
                    m_210 = 0;
                }
                if (Hook12()) {
                    m_1fb = true;
                    if (!m_1fc)
                        Repair();
                    SendMessageA(m_1a8, 0x10, 0, 0);
                }
            }
        }
    }
    return msg.wParam;
}

// Tears the 3D environment down (retail 0x00506ba0).
void GD507220::Cleanup3DEnvironment()
{
    if (m_a8) {
        if (m_203) {
            m_203 = false;
            Hook07();
        }
        if (m_202) {
            m_202 = false;
            Hook08();
        }
        if (m_a8->Release() > 0) {
            if (g_pLog)
                g_pLog->Error("Failed on Cleanup3DEnvironment");
            ReportError(0x8200000a, 1);
        }
        m_a8 = 0;
    }
}

// The display sub-object game code reaches through g_pDisplay+0x44.  Retail
// 0x00507df0 (SetMode), 0x00506190 (SetSize) and 0x00508a50 (SetResolution)
// are members of this name; they share the application's own layout, so the
// class only adds the setter names on top of it.
class G2CORE4D3DAPP : public GD507220
{
public:
    void SetResolution(int index);
    // Rebuilds the swap chain for the stored mode (0x507df0).
    int SetMode(int width, int height);
    void SetSize(int width, int height);

};

void G2CORE4D3DAPP::SetSize(int width, int height)
{
    m_1d0 = width;
    m_1d4 = height;
}

void G2CORE4D3DAPP::SetResolution(int index)
{
    m_4 = index;
    SetMode(m_1d0, m_1d4);
}

// Rebuilds the device for a new back buffer size (retail 0x00507df0).
int G2CORE4D3DAPP::SetMode(int width, int height)
{
    int oldW = GetSize()[0];
    int oldH = GetSize()[1];
    m_1d0 = width;
    m_1d4 = height;
    if (m_1fc) {
        RECT r;
        SetRect(&r, 0, 0, width, height);
        AdjustWindowRect(&r, m_1b0, 0);
        m_1b0 = GetWindowLongA(m_1a8, -16);
        GetWindowRect(m_1a8, &m_1d8);
        GetClientRect(m_1a8, &m_1e8);
        m_1e8.right = m_1e8.left + m_1d0;
        m_1e8.bottom = m_1e8.top + m_1d4;
        m_1d8.right = r.right - r.left + m_1d8.left;
        m_1d8.bottom = m_1d8.top - r.top + r.bottom;
    }
    ToggleFullScreen();
    int hr;
    if (GetSize()[0] == oldW && GetSize()[1] == oldH) {
        hr = ApplyMode();
        if (hr >= 0) {
            Recreate();
            hr = Tail();
        }
    } else {
        Cleanup3DEnvironment();
        hr = Restore();
    }
    if (hr < 0) {
        if (hr != 0x8876017c)
            hr = 0x8200000c;
        if (!m_1fc) {
            m_1fc = true;
            m_8 = true;
            ToggleFullScreen();
            SetWindowPos(m_1a8, (HWND)-2, m_1d8.left, m_1d8.top,
                         m_1d8.right - m_1d8.left, m_1d8.bottom - m_1d8.top,
                         0x40);
        }
        if (g_pLog)
            g_pLog->Error("Failed on change resolution");
        return ReportError(hr, 1);
    }
    if (m_1fc)
        SetWindowPos(m_1a8, (HWND)-2, m_1d8.left, m_1d8.top,
                     m_1d8.right - m_1d8.left, m_1d8.bottom - m_1d8.top, 0x40);
    SetFocus(m_1a8);
    HandleSizeChange();
    return 0;
}

// ---------------------------------------------------------------------------
// The surface object the reset path gets back from the device.  Retail
// (0x00506360) calls vtable+0x20 to read the description and vtable+0x8 to
// release it, which is an eight-entry interface in front of GetDesc.
// ---------------------------------------------------------------------------
class GD507220SURFACE
{
public:
    virtual int __stdcall QueryInterface(void *riid, void **ppv) = 0;
    virtual int __stdcall AddRef() = 0;
    virtual int __stdcall Release() = 0;
    virtual int __stdcall V03() = 0;
    virtual int __stdcall V04() = 0;
    virtual int __stdcall V05() = 0;
    virtual int __stdcall V06() = 0;
    virtual int __stdcall V07() = 0;
    virtual int __stdcall GetDesc(void *desc) = 0;
};

// Resets the device for the presentation parameters at +0x70 and restores the
// rest of the 3D environment (retail 0x00506360).  Retail reaches the function
// through the ?Tail@DX8SUB@@QAEXXZ label.
int GD507220::Tail()
{
    if (m_203) {
        m_203 = false;
        Hook07();
    }
    int hr = m_a8->Reset(&m_70);
    if (hr < 0) {
        if (g_pLog)
            g_pLog->Error("Screwdiver Error - device could not reset");
        if (g_pLog)
            g_pLog->Error(" Error code:%u", hr);
        switch (hr) {
        case 0x8007000e:
            if (g_pLog)
                g_pLog->Error(" Out of memory");
            break;
        case 0x8876017c:
            if (g_pLog)
                g_pLog->Error(" Out of video memory");
            break;
        case 0x8876086c:
            if (g_pLog)
                g_pLog->Error(" Invalid call");
            break;
        default:
            if (g_pLog)
                g_pLog->Error(" Unknown Error");
            break;
        }
        return hr;
    }
    IDirect3DSurface8 *surface;
    m_a8->GetBackBuffer(0, 0, &surface);
    ((GD507220SURFACE *)surface)->GetDesc(m_188);
    ((GD507220SURFACE *)surface)->Release();
    if (m_204)
        m_a8->ShowCursor(1);
    if (m_205) {
        if (!m_1fc) {
            RECT rect;
            GetWindowRect(m_1a8, &rect);
            ClipCursor(&rect);
        } else {
            ClipCursor(0);
        }
    }
    hr = Hook04();
    if (hr < 0) {
        MessageBoxA(0, "FAILED RESTORE", "Load Error", 0x10);
        Hook07();
        return hr;
    }
    m_203 = true;
    return 0;
}

// Re-applies the display settings: the windowed and the full screen pickers
// both have to agree.  While they do not, the display object drops its
// pending update and the pair is tried again (retail 0x00507b30).
int GD507220::ApplyMode()
{
    for (;;) {
        bool windowed = SetWindowedMode();
        bool fullscreen = SetFullScreenMode();
        if (m_206 && windowed)
            m_8 = false;
        if (!fullscreen) {
            if (!windowed)
                goto retry;
            m_8 = fullscreen;
        }
        if (!windowed) {
            if (!fullscreen)
                goto retry;
        }
        if (m_206 || fullscreen)
            return 0;
    retry:
        if (m_b0->m_28 <= 0)
            return 0x82000003;
        m_b0->m_28 = 0;
        m_b0->Reset();
    }
}

// Builds the presentation parameters at m_70 for the current mode
// (retail 0x005061b0).
void GD507220::Recreate()
{
    memset(&m_70, 0, 0x34);
    m_88 = m_1a8;
    m_8c = m_8;
    m_7c = 1;
    m_80 = m_8 ? m_2c : m_64;
    m_84 = 1;
    if (m_1f9)
        m_1fa = true;
    else
        m_1fa = false;
    m_90 = 1;
    m_94 = m_8 ? m_28 : m_60;
    m_98 = 0;
    if (m_1fc) {
        m_70 = m_1e8.right - m_1e8.left;
        m_74 = m_1e8.bottom - m_1e8.top;
        m_78 = (m_8 ? (int *)m_14 : (int *)m_4c)[3];
        m_9c = 0;
        m_a0 = m_8 ? m_38 : m_6c;
    } else {
        RECT rect = m_8 ? m_18 : m_50;
        m_70 = rect.left;
        m_74 = rect.top;
        m_78 = (m_8 ? (int *)m_14 : (int *)m_4c)[3];
        m_9c = m_50.right;
        m_a0 = m_8 ? m_38 : m_6c;
        if (m_1fa) {
            if (((GD507220DISPLAYINFO *)(m_8 ? m_10 : m_48))->m_2c & 1)
                m_a0 = 1;
        }
    }
}

// Puts the device back after a full screen switch (retail 0x00506fd0).
int GD507220::Restore()
{
    GD507220DISPLAYINFO *info = (GD507220DISPLAYINFO *)(m_8 ? m_10 : m_48);
    m_1fc = m_8;
    ToggleFullScreen();
    Recreate();
    if (info->m_28 & 0x1000)
        ReportError(0x8200000d, 0);
    // The vertex processing the picked device wants becomes the behaviour
    // flags of the CreateDevice call.
    unsigned int behaviour = (m_8 ? m_34 : m_68) == 0 ? 0x20
                           : (m_8 ? m_34 : m_68) == 1 ? 0x80
                           : (m_8 ? m_34 : m_68) == 2 ? 0x40
                           : (m_8 ? m_34 : m_68) == 3 ? 0x40 : 0;
    int adapter = GetSize()[0];
    int hr = m_a4->CreateDevice(adapter,
                                info->m_4, m_1a8, behaviour, &m_70, &m_a8);
    if (hr >= 0) {
        if (m_1fc)
            SetWindowPos(m_1a8, (HWND)-2, m_1d8.left, m_1d8.top,
                         m_1d8.right - m_1d8.left, m_1d8.bottom - m_1d8.top,
                         0x40);
        m_a8->GetDeviceCaps((D3DCAPS8 *)m_b4);
        IDirect3DSurface8 *surface = 0;
        m_a8->GetBackBuffer(0, 0, &surface);
        ((GD507220SURFACE *)surface)->GetDesc(m_188);
        ((GD507220SURFACE *)surface)->Release();
        if (m_204)
            m_a8->ShowCursor(1);
        if (m_205) {
            if (!m_1fc) {
                RECT rect;
                GetWindowRect(m_1a8, &rect);
                ClipCursor(&rect);
            } else {
                ClipCursor(0);
            }
        }
        hr = Hook03();
        if (hr < 0) {
            Hook08();
        } else {
            m_202 = true;
            hr = Hook04();
            if (hr < 0) {
                Hook07();
            } else {
                m_203 = true;
                return 0;
            }
        }
        Cleanup3DEnvironment();
    }
    if (hr == 0x8200000b || info->m_4 != 1)
        return hr;
    if (g_pLog)
        g_pLog->Error("Failed on init 3d enviroment");
    return ReportError(hr, 1);
}

// ---------------------------------------------------------------------------
// Notes for the unit's config
//
// The class above is GD507220 in the retail's terms; config/symbols.txt calls
// it GD507220 after its destructor's address, and this file keeps that name.
//
// Symbols that still need a name in config/symbols.txt (the delinker calls
// them FUN_<address> today, so objdiff cannot pair them):
//
//   0x005061b0 void  GD507220::Recreate()          QAEXXZ  (present parameters)
//   0x00506fd0 int   GD507220::Restore()           QAEHXZ  (device back)
//   0x00507b30 int   GD507220::ApplyMode()         QAEHXZ  (was FUN_00507b30;
//          already matched, needs the name to show up in objdiff)
//   0x005072b0 int   GD507220::ClearFlag()         UAEHXZ  (the flag hooks
//          return the HRESULT 0, not void)
//   0x00507df0 int   G2CORE4D3DAPP::SetMode(int,int)   QAEHHH@Z
//          (retail returns 0 from the success path: "xor eax,eax" and "ret 8")
//   0x00507510 bool  GD507220::SetWindowedMode()   QAE_NXZ
//   0x005072c0 bool  GD507220::SetFullScreenMode() QAE_NXZ
//
// The eleven empty hooks are int-returning in the retail: the vtable at
// 0x00678978 points nine of them at 0x004ede40 ("xor eax,eax; ret") and the
// two after that at 0x00547f50 (a plain "ret"), and the callers test the
// result as an HRESULT.
//
// A sequence of write-then-read on m_8 (a plain "unsigned char" member) is
// what makes the retail reload the flag for every use instead of caching it;
// m_8 is therefore declared volatile here, which is what the m_8 reloads in
// Recreate, Restore, SetMode and ApplyMode need.
// ---------------------------------------------------------------------------
