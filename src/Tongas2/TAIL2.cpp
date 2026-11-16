// Tongas2/TAIL2
#include <string.h>
#include <string>

#include "Tongas2/LOG.h"
// Tongas2/TAIL2 -- discovery scaffold for the remaining 0x54eec0-range code.

class KERNELCOMMANDS
{
public:
    virtual ~KERNELCOMMANDS();
};

class GD556050
{
public:
    virtual ~GD556050();
};

class GD5529f0 : public KERNELCOMMANDS
{
public:
    GD5529f0();
    virtual ~GD5529f0() throw();

    // Retail allocates 0x62c bytes for this class through the factory below;
    // the rest of the layout belongs to the 0x00553070 helper.
    char m_pad[0x62c - sizeof(KERNELCOMMANDS)];
};

__declspec(noinline) GD5529f0::~GD5529f0() throw()
{
}

class GD558000
{
public:
    virtual ~GD558000();
};

__declspec(noinline) GD558000::~GD558000()
{
}

class GD553820 : public GD556050
{
public:
    virtual ~GD553820() throw();
};

__declspec(noinline) GD553820::~GD553820() throw()
{
}

class GD553990 : public GD556050
{
public:
    virtual ~GD553990() throw();
};

__declspec(noinline) GD553990::~GD553990() throw()
{
}

class GD553e60 : public GD556050
{
public:
    virtual ~GD553e60() throw();
};

__declspec(noinline) GD553e60::~GD553e60() throw()
{
}

extern "C" void (__stdcall *g_fn656140)(void *);

struct G2TL2CALL
{
    void Call();
};

void G2TL2CALL::Call()
{
    g_fn656140(*(void **)((char *)this + 4));
}

struct DRMPALX
{
    void ViewGlobalScoresError();
    void PostGlobalScoresError();
    char *GetClearKey(char *out);
    char *GetClearLeaderBoardsKey(char *out);
};

void DRMPALX::ViewGlobalScoresError()
{
    if (g_pLog)
        g_pLog->Error("************************************************");
    if (g_pLog)
        g_pLog->Error("CRITIC ERROR: DRMPAL::ViewGlobalScores method called");
    if (g_pLog)
        g_pLog->Error("You must overload this class with an apropiate information to call ViewToGlobalScores");
    if (g_pLog)
        g_pLog->Error("************************************************");
}

void DRMPALX::PostGlobalScoresError()
{
    if (g_pLog)
        g_pLog->Error("************************************************");
    if (g_pLog)
        g_pLog->Error("CRITIC ERROR: DRMPAL::PostGlobalScores method called");
    if (g_pLog)
        g_pLog->Error("You must overload this class with an apropiate information to call SendToGlobalScores");
    if (g_pLog)
        g_pLog->Error("************************************************");
}

char *DRMPALX::GetClearKey(char *out)
{
    if (g_pLog)
        g_pLog->Error("************************************************");
    if (g_pLog)
        g_pLog->Error("CRITIC ERROR: DRMPAL::GetClearKey method called");
    if (g_pLog)
        g_pLog->Error("You must overload this class with an apropiate GetClearKey method");
    if (g_pLog)
        g_pLog->Error("************************************************");

    char buffer[0x200];
    strncpy(buffer, "ERROR-ERROR-ERROR-ERROR", 0x1ff);
    buffer[0x1ff] = 0;
    memcpy(out, buffer, sizeof(buffer));
    return out;
}

char *DRMPALX::GetClearLeaderBoardsKey(char *out)
{
    if (g_pLog)
        g_pLog->Error("************************************************");
    if (g_pLog)
        g_pLog->Error("CRITIC ERROR: DRMPAL::GetClearLeaderBoardsKey method called");
    if (g_pLog)
        g_pLog->Error("You must overload this class with an apropiate GetClearLeaderBoardsKey method");
    if (g_pLog)
        g_pLog->Error("************************************************");

    char buffer[0x200];
    strncpy(buffer, "-ERROR--", 0x1ff);
    buffer[0x1ff] = 0;
    memcpy(out, buffer, sizeof(buffer));
    return out;
}

// The 0x55xxxx block of this unit is the WildTangent property/exception
// layer. Its wide-string temporaries are std::basic_string<wchar_t>, and
// retail kept two of those members out of line here (0x5521c0 and 0x552220),
// so the whole wchar_t instantiation is requested.
template class std::basic_string<wchar_t>;

// 0x00554c40 converts an 8-bit std::string to a wide one. Retail passes the
// result buffer in ecx and the source in edx, i.e. a __fastcall helper
// returning the wide string by value.
std::wstring __fastcall ToWideText(const std::string &text, int mode);

// 0x00552980: reads one property through slot 0x6c of the wrapped object.
struct GD552980IFACE
{
    virtual void v00();
    virtual void v01();
    virtual void v02();
    virtual void v03();
    virtual void v04();
    virtual void v05();
    virtual void v06();
    virtual void v07();
    virtual void v08();
    virtual void v09();
    virtual void v10();
    virtual void v11();
    virtual void v12();
    virtual void v13();
    virtual void v14();
    virtual void v15();
    virtual void v16();
    virtual void v17();
    virtual void v18();
    virtual void v19();
    virtual void v20();
    virtual void v21();
    virtual void v22();
    virtual void v23();
    virtual void v24();
    virtual void v25();
    virtual void v26();
    virtual bool Read(const std::wstring &text);
};

struct GD552980
{
    char pad_0[8];
    GD552980IFACE *m_8;

    void Get(const std::string &name, char *out);
};

void GD552980::Get(const std::string &name, char *out)
{
    *out = m_8->Read(ToWideText(name, 0));
}

// 0x00552850: writes one property through slot 0x28 of the wrapped object.
struct GD552850IFACE
{
    virtual void v00();
    virtual void v01();
    virtual void v02();
    virtual void v03();
    virtual void v04();
    virtual void v05();
    virtual void v06();
    virtual void v07();
    virtual void v08();
    virtual void v09();
    virtual void Set(int value, const std::wstring &text);
};

struct GD552850
{
    char pad_0[8];
    GD552850IFACE *m_8;

    void Set(const std::string &name, unsigned char *value);
};

void GD552850::Set(const std::string &name, unsigned char *value)
{
    m_8->Set(*value, ToWideText(name, 0));
}

// 0x00552150: writes one property through slot 0x5c of the wrapped object.
struct GD552150IFACE
{
    virtual void v00();
    virtual void v01();
    virtual void v02();
    virtual void v03();
    virtual void v04();
    virtual void v05();
    virtual void v06();
    virtual void v07();
    virtual void v08();
    virtual void v09();
    virtual void v10();
    virtual void v11();
    virtual void v12();
    virtual void v13();
    virtual void v14();
    virtual void v15();
    virtual void v16();
    virtual void v17();
    virtual void v18();
    virtual void v19();
    virtual void v20();
    virtual void v21();
    virtual void v22();
    virtual void Set(int value, const std::wstring &text);
};

struct GD552150
{
    char pad_0[8];
    GD552150IFACE *m_8;

    void Set(const std::string &name, int value);
};

void GD552150::Set(const std::string &name, int value)
{
    m_8->Set(value, ToWideText(name, 0));
}

// 0x00552340: reads one property through slot 0xa0 of the wrapped object.
// That slot returns the std::string by value, so the sret slot is pushed as
// the interface's first argument and reused as assign's source.
struct GD552340IFACE
{
    virtual void v00();
    virtual void v01();
    virtual void v02();
    virtual void v03();
    virtual void v04();
    virtual void v05();
    virtual void v06();
    virtual void v07();
    virtual void v08();
    virtual void v09();
    virtual void v10();
    virtual void v11();
    virtual void v12();
    virtual void v13();
    virtual void v14();
    virtual void v15();
    virtual void v16();
    virtual void v17();
    virtual void v18();
    virtual void v19();
    virtual void v20();
    virtual void v21();
    virtual void v22();
    virtual void v23();
    virtual void v24();
    virtual void v25();
    virtual void v26();
    virtual void v27();
    virtual void v28();
    virtual void v29();
    virtual void v30();
    virtual void v31();
    virtual void v32();
    virtual void v33();
    virtual void v34();
    virtual void v35();
    virtual void v36();
    virtual void v37();
    virtual void v38();
    virtual void v39();
    virtual std::string Get(const std::string &name);
};

struct GD552340
{
    char pad_0[8];
    GD552340IFACE *m_8;

    void Get(const std::string &name, std::string *out);
};

void GD552340::Get(const std::string &name, std::string *out)
{
    out->assign(m_8->Get(name));
}

// 0x00553240: the factory the game installs in g_pfnCreateDrm.  Retail
// allocates the 0x62c-byte object and calls its constructor (0x005531c0)
// under the compiler's new-expression handler; it then returns whatever the
// constructor left in eax.
class DRM;

DRM *CreateDrmObject()
{
    return (DRM *)new GD5529f0;
}

// 0x005532a0: registers the DRM factory and the module's callback, called
// from the start-up chain at 0x00454120 (which passes an argument it ignores).
typedef void (*TAIL2CALLBACK)();

template <class T>
class CALLBACKLIST
{
public:
    struct NODE
    {
        NODE *prev;
        NODE *next;
        T data;
    };

    NODE *NewNode(T data);

    NODE *m_head;
};

class CBDRMLIST : public CALLBACKLIST<TAIL2CALLBACK>
{
};

extern CBDRMLIST g_drmCallbacks;
extern CALLBACKLIST<TAIL2CALLBACK>::NODE *g_drmCallbacksTail;
void DrmModuleCleanup();
extern DRM *(*g_pfnCreateDrm)();

void RegisterDrm()
{
    CALLBACKLIST<TAIL2CALLBACK>::NODE *node = g_drmCallbacks.NewNode(DrmModuleCleanup);
    node->prev = g_drmCallbacksTail;
    if (g_drmCallbacksTail)
        g_drmCallbacksTail->next = node;
    g_drmCallbacksTail = node;
    if (!g_drmCallbacks.m_head)
        g_drmCallbacks.m_head = node;
    g_pfnCreateDrm = &CreateDrmObject;
}

// 0x00553330: destroys four std::string members (sizeof(std::string) is
// 0x1c here, so they sit 0x1c apart) and then releases the handle at +4
// through the imported callback.  The handle release is the base subobject's
// inline destructor, so it runs after the members.
class GD553330BASE
{
public:
    char m_0[4];
    void *m_handle;
    char m_8[4];

    ~GD553330BASE()
    {
        g_fn656140(m_handle);
    }
};

class GD553330 : public GD553330BASE
{
public:
    std::string m_a;
    std::string m_b;
    std::string m_c;
    std::string m_d;

    ~GD553330();
};

GD553330::~GD553330()
{
}
