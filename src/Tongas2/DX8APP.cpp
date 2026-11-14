// Tongas2/DX8APP -- the plugin's concrete Direct3D application object.
//
// Discovery scaffold: bodies are empty until the unit is matched; each
// function has its exact target size in config/symbols.txt so a later delink
// cannot cut the target short. The destructor is virtual in the target
// (0x00509900 is the scalar-deleting destructor's non-deleting half).
#include <memory>
#include "Tongas2/DX8APP.h"
#include "Tongas2/RESOURCES.h"
#include "Tongas2/RENDERER.h"


DX8APP::~DX8APP()
{
    if (m_2bc) {
        delete m_2bc;
        m_2bc = 0;
    }
    if (m_2c0) {
        delete m_2c0;
        m_2c0 = 0;
    }
    if (m_2b8) {
        delete m_2b8;
        m_2b8 = 0;
    }
}

void DX8APP::Init()
{
}

DX8APP *CreateDX8APP()
{
    return new DX8APP;
}

void RegisterDX8Plugin()
{
}

struct G2DX8GET
{
    void * Get54(int index);
};

void * G2DX8GET::Get54(int index)
{
    return (char *)*(void **)((char *)this + 0x54) + index * 0x1c;
}

struct G2DX8T1
{
    void Call();
};

struct G2DX8T2
{
    void Call();
};

struct G2DX8TH1
{
    void Call();
};

struct G2DX8TH2
{
    void Call();
};

void G2DX8TH1::Call()
{
    ((G2DX8T1 *)*(void **)((char *)this + 0x4c))->Call();
}

void G2DX8TH2::Call()
{
    ((G2DX8T2 *)*(void **)((char *)this + 0x4c))->Call();
}

struct DX8APPM1
{
    float Get258();
};

float DX8APPM1::Get258()
{
    return *(float *)((char *)this + 0x258);
}

void __stdcall FUNC527150(int value);

void InitGraphics6()
{
    FUNC527150(6);
}

extern "C" __declspec(dllimport) int __stdcall ShowWindow(void *hwnd, int cmd);
extern "C" __declspec(dllimport) long __stdcall SendMessageA(void *hwnd, unsigned msg, unsigned wparam, long lparam);

class DX8SUB
{
public:
    virtual void V0();
    virtual void V1();
    virtual void V2();
    virtual void V3();
    virtual void V4();
    virtual void V5();
    virtual void V6();
    virtual void V7();
    virtual void V8();
    virtual void V9();
    virtual void V10();
    virtual void V11();
    virtual void V12();
    virtual void V13();
    virtual void V14();
    virtual void V15();
    virtual void V16();
    virtual void V17();
    virtual void V18();
    virtual void V19();
    virtual void V20();
    virtual void V21();
    virtual void V22();
    void Tail();
};

class DX8APPX
{
public:
    void ShowWindowCommand(int cmd);
    void SubVirtualCommand();
    void CloseCommand();
    void SubTailCommand();
    int SubAdjustedCommand();
};

void DX8APPX::ShowWindowCommand(int cmd)
{
    ShowWindow(*(void **)((char *)this + 0x1ec), cmd);
}

void DX8APPX::SubVirtualCommand()
{
    ((DX8SUB *)((char *)this + 0x44))->V1();
}

void DX8APPX::CloseCommand()
{
    *(bool *)((char *)this + 0x10) = true;
    SendMessageA(*(void **)((char *)this + 0x1ec), 0x10, 0, 0);
    *(bool *)((char *)this + 0x248) = true;
}

void DX8APPX::SubTailCommand()
{
    DX8SUB *sub = (DX8SUB *)((char *)this + 0x44);
    sub->V11();
    sub->Tail();
}

int DX8APPX::SubAdjustedCommand()
{
    ((DX8SUB *)((char *)this - 0x44))->V22();
    return 0;
}

void RenderFrame()
{
    g_pRenderer->Render();
}

extern RESOURCES *g_pResources;
extern RESOURCEMANAGER *g_pMESH;
extern "C" void m_37();

class MESHX
{
public:
    virtual void V00();
    virtual void V01();
    virtual void V02();
    virtual void V03();
    virtual void V04();
    virtual void V05();
    virtual void V06();
    virtual void V07();
    virtual void V08();
    virtual void V09();
};

int RenderInit()
{
    __asm {
        mov ecx, dword ptr [ecx + 0x278]
        test ecx, ecx
        je skip_call
        call m_37
    skip_call:
    }
    if (g_pResources)
        ((MESHX *)g_pMESH)->V09();
    return 0;
}

// Copies count ints from value to dest (0x509f70). The retail call site passes
// two extra unused arguments, so the definition keeps them.
class GD5383VEC;

extern "C" void GD5383CopyN(int *dest, unsigned int count, const int &value,
                            GD5383VEC *self, const int &value2)
{
    (void)self;
    (void)value2;
    while (count > 0) {
        if (dest)
            *dest = value;
        dest++;
        count--;
    }
}

// The 0x40-byte element this fill was instantiated for.
struct DX8APPTYPE40
{
    int m[16];
};

template void std::uninitialized_fill_n(
    DX8APPTYPE40 *, unsigned int, const DX8APPTYPE40 &);


// Destroys and frees the object held at +0x278.
class M39TARGET
{
public:
    void m_39();
};

class GD5091a0
{
public:
    int Release();

    char pad_0[0x278];
    M39TARGET *m_obj;
};

int GD5091a0::Release()
{
    M39TARGET *obj = m_obj;
    if (obj) {
        obj->m_39();
        ::operator delete(obj);
        m_obj = 0;
    }
    return 0;
}


// ---------------------------------------------------------------------------
// The 12-byte-element helpers of the plugin's vector code.  The retail unit
// keeps them as separate functions, so they are written as loops here: VC7.1
// does not inline functions that contain a loop.
// ---------------------------------------------------------------------------

// 0x509e80: copies one V3 into every element of [first, last).
void GD509e80(V3 *first, V3 *last, const V3 &value)
{
    while (first != last) {
        *first = value;
        first++;
    }
}

// 0x509ec0: the copy loop the linker folded with the five-argument caller
// below, so the definition keeps the two unused trailing arguments.
V3 *GD509ec0(V3 *first, V3 *last, V3 *dest, void *self, void *value2)
{
    (void)self;
    (void)value2;
    while (first != last) {
        if (dest)
            *dest = *first;
        first++;
        dest++;
    }
    return dest;
}

// 0x509f00: the V3 flavour of the fill helper at 0x509f70.
void GD509f00(V3 *dest, unsigned int count, const V3 &value, void *self,
              const V3 &value2)
{
    (void)self;
    (void)value2;
    while (count > 0) {
        if (dest)
            *dest = value;
        dest++;
        count--;
    }
}

// 0x50a060: the three-argument wrapper around the copy loop.
class GD50a060
{
public:
    void CopyRange(V3 *first, V3 *last, V3 *dest);
};

void GD50a060::CopyRange(V3 *first, V3 *last, V3 *dest)
{
    GD509ec0(first, last, dest, this, dest);
}

// 0x50a860: the destructor of the plugin's buffer holder.
class GD50a860
{
public:
    ~GD50a860();

    void *m_0;
    void *m_4;
    void *m_8;
    void *m_c;
    void *m_10;
};

GD50a860::~GD50a860()
{
    m_0 = 0;
    if (m_8)
        ::operator delete(m_8);
    m_8 = 0;
    m_c = 0;
    m_10 = 0;
}

// 0x50b010: frees the buffer of every 16-byte record in [first, last).
void GD50b010(char *first, char *last)
{
    while (first != last) {
        void *p = *(void **)(first + 4);
        if (p)
            ::operator delete(p);
        *(void **)(first + 4) = 0;
        *(void **)(first + 8) = 0;
        *(void **)(first + 0xc) = 0;
        first += 16;
    }
}


// ---------------------------------------------------------------------------
// The plugin's own vector storage.  The three pointers sit where the STL
// vector keeps them (the compressed allocator pair takes the first word),
// and the three "Buy" members are the same helper for the three element
// sizes the plugin uses.
// ---------------------------------------------------------------------------

class GDVEC
{
public:
    void ThrowLen();
    bool Buy12(unsigned int capacity);
    bool Buy64(unsigned int capacity);
    bool Buy4(unsigned int capacity);
    void Assign4(unsigned int count, const int &value);

    unsigned char pad_0[4];
    void *m_first;
    void *m_end;
    void *m_last;
};

// 0x50a900: the "vector<T> too long" thrower.
void GDVEC::ThrowLen()
{
    std::string msg;
    msg = "vector<T> too long";
    throw std::length_error(msg);
}

// 0x50a970: buys room for capacity twelve-byte elements.
bool GDVEC::Buy12(unsigned int capacity)
{
    m_first = 0;
    m_end = 0;
    m_last = 0;
    if (capacity == 0)
        return false;
    if (capacity > 0xffffffffu / 12)
        ThrowLen();
    V3 *p = (V3 *)::operator new(capacity * 12);
    m_last = (char *)(p + capacity);
    m_first = p;
    m_end = p;
    return true;
}

// 0x50a9c0: buys room for capacity sixty-four-byte elements.
bool GDVEC::Buy64(unsigned int capacity)
{
    m_first = 0;
    m_end = 0;
    m_last = 0;
    if (capacity == 0)
        return false;
    if (capacity > 0xffffffffu / 64)
        ThrowLen();
    DX8APPTYPE40 *p = (DX8APPTYPE40 *)::operator new(capacity * 64);
    m_last = (char *)(p + capacity);
    m_first = p;
    m_end = p;
    return true;
}

// 0x50ad50: buys room for capacity four-byte elements.
bool GDVEC::Buy4(unsigned int capacity)
{
    m_first = 0;
    m_end = 0;
    m_last = 0;
    if (capacity == 0)
        return false;
    if (capacity > 0xffffffffu / 4)
        ThrowLen();
    int *p = (int *)::operator new(capacity * 4);
    m_last = (char *)(p + capacity);
    m_first = p;
    m_end = p;
    return true;
}

// 0x50a3c0: the wrapper that fills a run of sixty-four-byte records.  The
// retail call site pushes five arguments, so the helper keeps the two the
// linker left over.  The retail keeps the helper out of line, so it is
// marked noinline here as well.
__declspec(noinline) void GD50a3c0Fill(DX8APPTYPE40 *first, unsigned int count,
                                       const DX8APPTYPE40 *value, void *self,
                                       const void *value2)
{
    (void)self;
    (void)value2;
    while (count > 0) {
        if (first)
            *first = *value;
        first++;
        count--;
    }
}

class GD50a3c0
{
public:
    char *Fill(DX8APPTYPE40 *first, unsigned int count,
               const DX8APPTYPE40 *value);
};

char *GD50a3c0::Fill(DX8APPTYPE40 *first, unsigned int count,
                     const DX8APPTYPE40 *value)
{
    GD50a3c0Fill(first, count, value, this, value);
    return (char *)first + count * 64;
}

// 0x50b2c0: fills a run of four-byte records with one value.
void GDVEC::Assign4(unsigned int count, const int &value)
{
    m_first = 0;
    m_end = 0;
    m_last = 0;
    if (count == 0)
        return;
    if (count > 0xffffffffu / 4)
        ThrowLen();
    unsigned int bytes = count * 4;
    int *p = (int *)::operator new(bytes);
    m_last = (char *)(p + count);
    m_first = p;
    m_end = p;
    int *q = p;
    while (count != 0) {
        *q = value;
        q++;
        count--;
    }
    m_end = (char *)p + bytes;
}

// 0x50a1f0: looks for the record whose id matches the one held here.
class GD50a1f0
{
public:
    unsigned int Find();

    int m_value;
    int m_pad;
    char *m_begin;
    char *m_end;
};

unsigned int GD50a1f0::Find()
{
    unsigned int i = 0;
    char *first = m_begin;
    while (first != 0 && i < (unsigned int)((m_end - first) >> 2)) {
        char *elem = *(char **)(m_begin + i * 4);
        if (*(int *)(elem + 0x120) == m_value)
            return i;
        i++;
    }
    return 0;
}
