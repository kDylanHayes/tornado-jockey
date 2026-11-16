// Tongas2/DX8APP -- the plugin's concrete Direct3D application object.
//
// Matched this pass (retail bytes, verified with build/maskcmp.py):
//   0x00508a70 DX8APP::DX8APP          0x508d60 CheckSize
//   0x00509170 BuildHardware            0x00509770 SetDetail
//   0x00509d30 RegisterDX8Plugin        0x0050a080 node SetWorld
//   0x0050a0f0 node Matches             0x0050a170 node Find
//   0x0050a3f0 node destructor          0x0050a230 SetIdentity
//   0x0050a500 holder destructor        0x0050a860 buffer destructor
//   0x0050aa90 vector<RESHANDLE>::_Insert_n
//   0x0050b110 vector<RESHANDLE>::insert
// Still open: 0x00508b10, 0x00508e00, 0x00508f50, 0x00509240, 0x005099f0
// (Init), 0x00509c20 (SetName, register colour: an extra push ebx and the
// argument load hoisted above the EH prologue), 0x00509fa0 / 0x00509ff0
// (loop cursor ownership), 0x0050a060 / 0x0050a1f0 / 0x0050a3c0 (argument
// setup register colour), 0x0050a2e0, 0x0050a5a0, 0x0050a670, 0x0050ada0,
// and the 0x0050b050/0x0050b180/0x0050b330 vector instantiations, which call
// the allocator-taking _Uninit_fill_n/_Uninit_copy and _Tidy of this toolkit's
// <vector> where retail calls the four-argument helpers and _m_12.
//
// Two scaffold gaps force the class declarations below: the headers give both
// bases no out-of-line constructor (retail 0x004e6000 and 0x005081a0) and
// HARDWARE::Detect no arguments (retail 0x00504ff0 pops three).  The guards
// below suppress those two headers so the retail shapes can be declared here.
#include <memory>
#define TONGAS2_HARDWARE_H
#define TONGAS2_DX8APP_H
class HARDWARE;
#include <string>
#include "Tongas2/SETTINGS.h"
#include "Tongas2/TIMER.h"
#include "Tongas2/RESOURCES.h"
#include "Tongas2/RENDERER.h"

class HARDWARE
{
public:
    HARDWARE();
    ~HARDWARE();
    void Detect(int width, int height, void *out);
    bool IsSupported(int index);
    int HardwareE(int index);

    char m_000[0x54];
};

// The window object at +0x00 (0x44 bytes) and the Direct3D object at +0x44
// (0x274 bytes).  Both constructors are out of line in the retail plugin.
class DX8APPBASE1
{
public:
    DX8APPBASE1();
    virtual ~DX8APPBASE1();

    char pad_04[0x40];
};

class GD507220
{
public:
    GD507220();
    virtual ~GD507220();
    void SetText(const std::string &text);

    char pad_04[0x270];
};

class DX8APPSTRING
{
public:
    ~DX8APPSTRING();
};

class DX8APP : public DX8APPBASE1, public GD507220
{
public:
    DX8APP();
    virtual ~DX8APP();
    void Init();
    void CheckSize();
    int BuildHardware();
    void SetDetail(int level);
    void SetName(const char *name);

private:
    DX8APPSTRING *m_2b8;
    HARDWARE *m_2bc;
    SETTINGS *m_2c0;
};

DX8APP *CreateDX8APP();
void RegisterDX8Plugin();

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

// The window base's SetSize as the constructor calls it: qualified, so the
// call goes straight to the base implementation instead of the vtable.
class APPWINDOW
{
public:
    virtual void SetSize(int width, int height);
};

// The two sizes the constructor hands to SetSize.
struct DX8APPCTOR
{
    char pad_0[0x214];
    int m_214;
    int m_218;
};

DX8APP::DX8APP()
    : m_2b8(0), m_2bc(0), m_2c0(0)
{
    DX8APPCTOR *self = (DX8APPCTOR *)this;
    ((APPWINDOW *)this)->APPWINDOW::SetSize(self->m_214, self->m_218);
}

// The application's primary vtable as its own methods call it.  Only the
// slots the unit calls are named; the count fixes the offsets.
class DX8APPVT
{
public:
    virtual void V00(); virtual void V01(); virtual void V02(); virtual void V03();
    virtual void V04(); virtual void V05(); virtual void V06(); virtual void V07();
    virtual void V08(); virtual void V09(); virtual void V10(); virtual void V11();
    virtual void V12(); virtual void V13(); virtual void V14(); virtual void V15();
    virtual void V16(); virtual void V17(); virtual void V18(); virtual void V19();
    virtual void V20(); virtual void V21(); virtual void V22(); virtual void V23();
    virtual void V24(); virtual void V25(); virtual void V26(); virtual void V27();
    virtual void V28(); virtual void V29();
    virtual void V30(int width, int height); virtual void V31();
    virtual void V32(); virtual void V33(); virtual void V34(); virtual void V35();
    virtual void V36(); virtual void V37();
    virtual void *V38();
    virtual void V39();
    virtual void V40(); virtual void V41(); virtual void V42(); virtual void V43();
    virtual void V44(); virtual void V45(); virtual void V46(); virtual void V47();
    virtual void V48(); virtual void V49(); virtual void V50(); virtual void V51();
    virtual void V52(); virtual void V53(); virtual void V54(); virtual void V55();
    virtual void V56(); virtual void V57(); virtual void V58(); virtual void V59();
    virtual void V60(); virtual void V61(); virtual void V62(); virtual void V63();
};

// The application's fields between the two base subobjects.  The scaffold
// header flattens them into padding, so they are named here by offset.
struct DX8APPFIELDS
{
    void *m_vtbl;           // 0x00
    int m_4;                // 0x04
    int m_8;                // 0x08
    char pad_c[0x10 - 0xc];
    bool m_10;              // 0x10
    char pad_11[0x18 - 0x11];
    int m_18;               // 0x18
    char pad_1c[0x34 - 0x1c];
    int m_34;               // 0x34
    char pad_38[0x70 - 0x38];
    char m_70[0x34];        // 0x70
    int m_a4;               // 0xa4
    int m_a8;               // 0xa8
    char pad_ac[0xb4 - 0xac];
    char m_b4[0x34];        // 0xb4
    char pad_e8[0x1ec - 0xe8];
    void *m_1ec;            // 0x1ec
    char pad_1f0[0x214 - 0x1f0];
    int m_214;              // 0x214
    int m_218;              // 0x218
    char pad_21c[0x243 - 0x21c];
    bool m_243;             // 0x243
    char pad_244[0x248 - 0x244];
    bool m_248;             // 0x248
    char pad_249[0x274 - 0x249];
    HARDWARE *m_274;        // 0x274
    char pad_278[4];
    void *m_27c;            // 0x27c
};

extern "C" __declspec(dllimport) int __stdcall IsIconic(HWND hwnd);

// 0x508d60: when the window is not minimised and its client area has changed,
// tells the application about the new size.
void DX8APP::CheckSize()
{
    DX8APPFIELDS *self = (DX8APPFIELDS *)this;
    if (self->m_243)
        return;
    if (IsIconic((HWND)((DX8APPVT *)this)->V38()))
        return;
    RECT rect;
    GetClientRect((HWND)self->m_1ec, &rect);
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    if (width == self->m_4 && height == self->m_8)
        return;
    self->m_8 = height;
    self->m_4 = width;
    ((DX8APPVT *)this)->V30(width, height);
}

// 0x509170: lets the hardware object detect itself against the window and
// then refreshes the settings that depend on it.
class GD4f9c40
{
public:
    void Changed();
};

int DX8APP::BuildHardware()
{
    DX8APPFIELDS *self = (DX8APPFIELDS *)this;
    self->m_274->Detect(self->m_a4, self->m_a8, self->m_70);
    ((GD4f9c40 *)self->m_27c)->Changed();
    return 0;
}

void DX8APP::Init()
{
}

DX8APP *CreateDX8APP()
{
    return new DX8APP;
}

// ---------------------------------------------------------------------------
// The plugin's registration entry point: installs the class registrars, the
// resource callbacks and the factories the engine makes the plugin with.
// ---------------------------------------------------------------------------

class APP;
class CAMERALENS;

extern CALLBACKLIST<void (*)()> g_classRegistrars;
extern APP *(*g_pfnCreateApp)();
extern APPWINDOW *(*g_pfnCreateAppWindow)();
extern RESOURCEMANAGER *(*g_pfnCreateBONEDMODEL)();
extern RESOURCEMANAGER *(*g_pfnCreateSHADER)();
extern CAMERALENS *(*g_pfnCreateCameraLens)();
extern RENDERER *(*g_pfnCreateRENDERER)();
extern void *(*g_pfnCreateRENDERTEXTURE)();

void RegisterModelDX8SDClasses();
void FUN_00503e40();
void FUN_005040e0();
void FUN_00504130();
CAMERALENS *FUN_00516000();
RENDERER *FUN_0053a100();
RESOURCEMANAGER *CreateBONEDMODELDX8SD();
void *CreateSHADERDX8SD();
void *CreateRENDERTEXTUREDX8SD();

class GD53b520FACTORY
{
public:
    static APP *Create();
};

// The list's own tail insert; the scaffold's CALLBACKLIST only has AddHead.
template <class T>
static void GDAddTail(CALLBACKLIST<T> *list, T data)
{
    CALLBACKLIST<T>::NODE *node = list->NewNode(data);
    node->prev = list->m_tail;
    if (list->m_tail)
        list->m_tail->next = node;
    list->m_tail = node;
    if (!list->m_head)
        list->m_head = node;
}

void RegisterDX8Plugin()
{
    GDAddTail(&g_classRegistrars, RegisterModelDX8SDClasses);
    GDAddTail(&RESOURCES::s_onInit, FUN_00503e40);
    GDAddTail(&RESOURCES::s_onCreate, FUN_005040e0);
    GDAddTail(&RESOURCES::s_onSetLevel, FUN_00504130);
    g_pfnCreateApp = GD53b520FACTORY::Create;
    g_pfnCreateAppWindow = (APPWINDOW *(*)())CreateDX8APP;
    g_pfnCreateBONEDMODEL = CreateBONEDMODELDX8SD;
    g_pfnCreateSHADER = (RESOURCEMANAGER *(*)())CreateSHADERDX8SD;
    g_pfnCreateCameraLens = FUN_00516000;
    g_pfnCreateRENDERER = FUN_0053a100;
    g_pfnCreateRENDERTEXTURE = CreateRENDERTEXTUREDX8SD;
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

// 0x50a860: the destructor of the plugin's buffer holder.  The three words
// after the first are a standard vector, so the compiler emits its tidy.
class GD50a860
{
public:
    ~GD50a860();

    void *m_0;
    std::vector<int> m_list;
};

GD50a860::~GD50a860()
{
    m_0 = 0;
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

// ---------------------------------------------------------------------------
// The scene-graph node the plugin's transform pass walks.  It keeps its world
// matrix at +0x80, its name at +0x104 and the child list at +0x124.
// ---------------------------------------------------------------------------

class GD50a080;

void __stdcall FUN_004626de(MATRIX *out, void *self, const MATRIX *parent);

class GD50a080
{
public:
    ~GD50a080();
    void SetWorld(MATRIX *parent);
    bool Matches(const std::string *name);
    GD50a080 *Find(const std::string *name);

    char pad_0[0x80];
    MATRIX m_matrix;
    char pad_c0[0x44];
    std::string m_name;
    int m_120;
    std::vector<GD50a080 *> m_children;
};

// 0x50a3f0: deletes the children, then the name.
GD50a080::~GD50a080()
{
    for (unsigned int i = 0; i < m_children.size(); i++) {
        if (m_children[i]) {
            delete m_children[i];
            m_children[i] = 0;
        }
    }
    m_children.~vector();
}

// 0x50a080: composes this node's world matrix with its parent's and hands it
// down to the children.
void GD50a080::SetWorld(MATRIX *parent)
{
    MATRIX world;
    FUN_004626de(&world, this, parent);
    m_matrix = world;
    for (unsigned int i = 0; i < m_children.size(); i++)
        m_children[i]->SetWorld(&m_matrix);
}

// 0x50a0f0: whether this node or one of its descendants carries the name.
bool GD50a080::Matches(const std::string *name)
{
    if (*name == m_name)
        return true;
    for (unsigned int i = 0; i < m_children.size(); i++)
        if (m_children[i]->Matches(name))
            return true;
    return false;
}

// 0x50a170: the first node in this subtree that carries the name.
GD50a080 *GD50a080::Find(const std::string *name)
{
    if (m_name == *name)
        return this;
    for (unsigned int i = 0; i < m_children.size(); i++) {
        GD50a080 *found = m_children[i]->Find(name);
        if (found)
            return found;
    }
    return 0;
}

// 0x50a500: the holder of a node and two lists.
class GD50a500
{
public:
    ~GD50a500();

    GD50a080 *m_0;
    std::vector<int> m_v1;
    std::vector<int> m_v2;
};

GD50a500::~GD50a500()
{
    if (m_0) {
        delete m_0;
        m_0 = 0;
    }
    m_v1.~vector();
}

// 0x50a230: resets the root node's transform to the identity.
class GD50a230
{
public:
    void SetIdentity();

    GD50a080 *m_0;
};

void GD50a230::SetIdentity()
{
    MATRIX m;
    m._12 = m._13 = m._14 = m._21 = m._23 = m._24 =
        m._31 = m._32 = m._34 = m._41 = m._42 = m._43 = 0.0f;
    m._11 = m._22 = m._33 = m._44 = 1.0f;
    m_0->SetWorld(&m);
}

// 0x509fa0 and 0x509ff0: the resource list the plugin keeps at +0x4c.  Each
// record carries its resource at +0x10 and its id at +0x18.
class GD509fa0ITEM
{
public:
    void *m_vtbl;           // 0x00
    char pad_4[0xc];
    void *m_10;             // 0x10
    int m_14;               // 0x14
    int m_18;               // 0x18
};

class GD509fa0LIST
{
public:
    char pad_0[0x6c];
    GD509fa0ITEM **m_first;
    GD509fa0ITEM **m_last;
};

class GD4fde40
{
public:
    void Release(void *value);
};

class GD53cac0
{
public:
    void Set(void *value);
};

class GD509fa0
{
public:
    void *Lookup(int id);
    void SetResource(int id, void *value);

    char pad_0[0x30];
    GD4fde40 *m_30;         // 0x30
    char pad_34[0x4c - 0x34];
    GD509fa0LIST *m_4c;     // 0x4c
};

void *GD509fa0::Lookup(int id)
{
    GD509fa0LIST *list = m_4c;
    GD509fa0ITEM **first = list->m_first;
    unsigned int i = 0;
    while (first != 0 && i < (unsigned int)(list->m_last - first)) {
        if (list->m_first[i]->m_18 == id)
            return list->m_first[i]->m_10;
        i++;
    }
    return 0;
}

void GD509fa0::SetResource(int id, void *value)
{
    GD509fa0LIST *list = m_4c;
    for (unsigned int i = 0; i < (unsigned int)(list->m_last - list->m_first); i++)
        if (list->m_first[i]->m_18 == id && list->m_first[i]->m_10 != value) {
            m_30->Release(list->m_first[i]->m_10);
            ((GD53cac0 *)list->m_first[i])->Set(value);
        }
}

// 0x509770: applies a detail level to the settings, then tells the resource
// system and the music about the change.
class SETTINGSTABLE
{
public:
    void SetInt(int index, int value);
};

class MUSICX
{
public:
    void Update();
};

struct GD509770SETTINGS
{
    char pad_0[0xff0];
    void *m_ff0;
};

extern void *g_pMusicX;
extern TIMER *g_pTimer;

void DX8APP::SetDetail(int level)
{
    DX8APPFIELDS *self = (DX8APPFIELDS *)this;
    if (self->m_34 < 0)
        self->m_34 = level;
    switch (level) {
    case 0:
        ((SETTINGSTABLE *)m_2c0)->SetInt(10, 0);
        ((SETTINGSTABLE *)m_2c0)->SetInt(11, 0);
        ((SETTINGSTABLE *)m_2c0)->SetInt(15, 0);
        ((SETTINGSTABLE *)m_2c0)->SetInt(0x20, 0);
        break;
    case 1:
        ((SETTINGSTABLE *)m_2c0)->SetInt(10, 0);
        ((SETTINGSTABLE *)m_2c0)->SetInt(11, 1);
        ((SETTINGSTABLE *)m_2c0)->SetInt(15, 0);
        ((SETTINGSTABLE *)m_2c0)->SetInt(0x20, 0);
        break;
    case 2:
        ((SETTINGSTABLE *)m_2c0)->SetInt(10, 0);
        ((SETTINGSTABLE *)m_2c0)->SetInt(11, 1);
        ((SETTINGSTABLE *)m_2c0)->SetInt(15, 1);
        ((SETTINGSTABLE *)m_2c0)->SetInt(0x20, 1);
        break;
    }
    if (!((HARDWARE *)((GD509770SETTINGS *)m_2c0)->m_ff0)->IsSupported(0))
        ((SETTINGSTABLE *)m_2c0)->SetInt(15, 0);
    if (level != self->m_34) {
        self->m_34 = level;
        g_pResources->SetGlobalScale(g_pTimer->m_soundTime);
        if (g_pMusicX)
            ((MUSICX *)g_pMusicX)->Update();
    }
}

// 0x509c20: hands a text to the display object at +0x44.
void DX8APP::SetName(const char *name)
{
    std::string text;
    text.assign(name, strlen(name));
    ((GD507220 *)((char *)this + 0x44))->SetText(text);
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
    char **m_begin;
    char **m_end;
};

unsigned int GD50a1f0::Find()
{
    unsigned int i = 0;
    while (m_begin != 0 && i < (unsigned int)(m_end - m_begin)) {
        if (*(int *)(m_begin[i] + 0x120) == m_value)
            return i;
        i++;
    }
    return 0;
}

// The out-of-line std::vector helpers the unit owns (retail 0x50b2c0,
// 0x50b230, 0x50aa10); explicit instantiations make the compiler emit them.
template void std::vector<int>::_Construct_n(unsigned int, const int &);
template void std::vector<int>::resize(unsigned int, int);
template void std::vector<bool>::_Trim(unsigned int);
template void std::vector<RESHANDLE>::_Insert_n(std::vector<RESHANDLE>::iterator, unsigned int, const RESHANDLE &);
template std::vector<RESHANDLE>::iterator std::vector<RESHANDLE>::insert(std::vector<RESHANDLE>::iterator, const RESHANDLE &);
template void std::vector<DX8APPTYPE40>::_Construct_n(unsigned int, const DX8APPTYPE40 &);
