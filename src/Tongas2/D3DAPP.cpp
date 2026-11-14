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
WINUSERAPI BOOL WINAPI SetWindowTextA(HWND hWnd, LPCSTR lpString);
WINUSERAPI LONG WINAPI SetWindowLongA(HWND hWnd, int nIndex, LONG dwNewLong);
WINUSERAPI HMENU WINAPI GetMenu(HWND hWnd);
WINUSERAPI BOOL WINAPI SetMenu(HWND hWnd, HMENU hMenu);
WINUSERAPI BOOL WINAPI GetWindowRect(HWND hWnd, LPRECT lpRect);
}

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

// The object the application deletes from m_b0.  Its destructor lives in
// another unit (0x00538520).
class GD507220OBJECT
{
public:
    ~GD507220OBJECT();
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
    virtual int ToggleFullScreen();    virtual void Hook01() {}
    virtual void Hook02() {}
    virtual void Hook03() {}
    virtual void Hook04() {}
    virtual void Hook05() {}
    virtual void Hook06() {}
    virtual void Hook07() {}
    virtual void Hook08() {}
    virtual void Hook09() {}
    virtual void Hook10() {}
    virtual void Hook11() {}
    virtual bool Hook12() { return false; }
    virtual void ClearFlag();
    virtual void SetFlag();

    // The window text is written into m_1b4 and pushed into the window.
    void SetText(const std::string &text);
    // Releases the device and tears the 3D environment down.
    void Cleanup3DEnvironment();
    // Reports a failure to the player (retail 0x00506760).
    void ReportError(unsigned int code, int flag);
    // Rebuilds the 3D environment for the current mode (retail 0x00506360).
    int Tail();
    // Picks the new mode up when the window changed size (retail 0x00506a70).
    int HandleSizeChange();

    int m_4;                     // 0x04
    unsigned char m_8;           // 0x08
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
    int m_a4;                    // 0xa4
    IDirect3DDevice8 *m_a8;      // 0xa8, the Direct3D device
    int m_ac;                    // 0xac
    GD507220OBJECT *m_b0;        // 0xb0
    char pad_b4[0x1a8 - 0xb4];   // 0xb4
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
    bool m_1fc;                  // 0x1fc, full screen
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
void GD507220::ClearFlag()
{
    m_1f8 = false;
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
        m_74 = m_1e8.bottom - m_1e8.top;
        m_1fd = false;
        if (m_a8) {
            ret = Tail();
            if (ret < 0) {
                if (ret != 0x8876017c)
                    ret = 0x8200000c;
                if (g_pLog)
                    g_pLog->Error("Failed on HandlePossibleSizeChange");
                ReportError(ret, 1);
                m_1fd = true;
            }
        }
    }
    return ret;
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

// The display object game code reaches through g_pDisplay+0x44. Only the
// resolution setter is in this block (retail 0x005070f0/0x00507130).
class G2CORE4D3DAPP
{
public:
    void SetResolution(int index);
    // Rebuilds the swap chain for the stored mode (0x507df0).
    void SetMode(int width, int height);
    void SetSize(int width, int height);

    char pad_00[4];
    int m_4;
    char pad_08[0x1d0 - 0x08];
    int m_1d0;
    int m_1d4;
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
