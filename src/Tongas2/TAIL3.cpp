#define CSTRING_CTOR_OUTOFLINE
#include <new>
#include <string>
#include <malloc.h>
#include <string.h>

#include "Tongas2/CSTRING.h"
#include <string>
#include <stdexcept>

// IAT thunks the retail objects call through (InterlockedIncrement/Decrement).
extern "C" void (__stdcall *g_fn6561b4)(void *);
extern "C" long (__stdcall *g_fn6561ac)(long *);

// Tongas2/TAIL3 -- discovery scaffold for the remaining 0x54eec0-range code.

class GD556050
{
public:
    virtual ~GD556050();
};

class V5547C0
{
public:
    V5547C0();
    virtual ~V5547C0();
    V5547C0(const V5547C0 &other);

    int m_4;
};

V5547C0::V5547C0()
{
    m_4 = 0;
}

__declspec(noinline) V5547C0::~V5547C0()
{
}

V5547C0::V5547C0(const V5547C0 &other)
{
    m_4 = other.m_4;
}

class V5547C4 : public V5547C0
{
public:
    virtual ~V5547C4();
    V5547C4(const V5547C4 &other);
    // Drops the reference count; deletes this at zero (0x5547e0).
    void Release();
};

V5547C4::V5547C4(const V5547C4 &other) : V5547C0(other)
{
}

__declspec(noinline) V5547C4::~V5547C4()
{
}

__declspec(noinline) void V5547C4::Release()
{
    if (g_fn6561ac((long *)&m_4) == 0)
        delete this;
}

class V554E60
{
public:
    virtual ~V554E60();
};

__declspec(noinline) V554E60::~V554E60()
{
}

class GD557e70
{
public:
    virtual ~GD557e70();
};

__declspec(noinline) GD557e70::~GD557e70()
{
}

class GD5545d0 : public GD556050
{
public:
    virtual ~GD5545d0() throw();
};

__declspec(noinline) GD5545d0::~GD5545d0() throw()
{
}

struct G2TL3SET
{
    void Set4c(int value);
};

void G2TL3SET::Set4c(int value)
{
    *(int *)((char *)this + 0x4c) = value;
}

extern "C" void (__stdcall *g_fn6561b4)(void *);

struct G2TL3CALL
{
    void Call();
};

void G2TL3CALL::Call()
{
    g_fn6561b4((char *)this + 4);
}

struct G2TL3TGT1
{
    ~G2TL3TGT1();

    std::string m_0;
    V5547C4 *m_1c;
    void *m_20;
};

G2TL3TGT1::~G2TL3TGT1()
{
    V5547C4 *sub = m_1c;
    if (m_20) {
        if (g_fn6561ac((long *)m_20) == 0) {
            delete sub;
            _aligned_free(m_20);
            m_20 = 0;
        }
    }
}

struct G2TL3TG2
{
    ~G2TL3TG2();

    std::string m_0;
    V5547C4 *m_1c;
};

G2TL3TG2::~G2TL3TG2()
{
    if (m_1c)
        m_1c->Release();
}

struct G2TL3TH1
{
    void Call();
};

struct G2TL3TH2
{
    void Call();
};

void G2TL3TH1::Call()
{
    ((G2TL3TGT1 *)((char *)this + 0xc))->~G2TL3TGT1();
}

void G2TL3TH2::Call()
{
    ((G2TL3TG2 *)((char *)this + 0xc))->~G2TL3TG2();
}

struct G2TL3ALLOC
{
    void *Alloc(unsigned int n);
};

void *G2TL3ALLOC::Alloc(unsigned int n)
{
    return operator new(n * 2);
}

struct TL3GET
{
    int Get0(void);
    char Get1(void);
    int Get2(void);
};

int TL3GET::Get0(void)
{
    return *(int *)((char *)this + 0x8);
}

char TL3GET::Get1(void)
{
    return *(char *)((char *)this + 0x50);
}

int TL3GET::Get2(void)
{
    return *(int *)((char *)this + 0x4);
}

struct TL3PTR
{
    void *Get0();
    void *Get1();
};

void *TL3PTR::Get0()
{
    return (char *)this + 0x10;
}

void *TL3PTR::Get1()
{
    return (char *)this + 0x2c;
}

class TAIL3X
{
public:
    void Call4C(int a);
};

void TAIL3X::Call4C(int a)
{
    void (__fastcall *func)(int) = *(void (__fastcall **)(int))((char *)this + 0x4c);
    if (func)
        func(a);
}

CSTRING::CSTRING() : m_str(0)
{
}

class TAIL1ERR : public std::logic_error
{
public:
    TAIL1ERR(const std::string &message) : std::logic_error(message) {}
    TAIL1ERR(const TAIL1ERR &other);
};

struct TAIL3ROOT;
void *GetTail3Root();

// This translation unit emitted the wide-string member functions of the
// shared std::basic_string<wchar_t> instantiation (0x554960-0x554d80).
template class std::basic_string<wchar_t>;

// The per-user folder under the game's root path (0x554320).
std::string __fastcall GetUserFolder(int folder)
{
    if (folder < 0 || folder >= 2) {
        std::string message("CSaveFile::GetRootPath called with invalid Type");
        throw TAIL1ERR(message);
    }
    void *root = GetTail3Root();
    std::string result(*(std::string *)((char *)root + 0x28));
    if (folder == 0)
        result.append("\\Cache", 6);
    else
        result.append("\\Persistent", 11);
    return result;
}

// ---------------------------------------------------------------------------
// The save-file root singleton (0x554260 GetTail3Root, 0x554020 its
// constructor).  The object is 0x80 bytes: four path strings and the
// "Windows 5 or later" probe at +0x7c.

struct TAIL3ROOT
{
    int m_00;           // 0x00
    int m_04;           // 0x04
    int m_08;           // 0x08
    std::string m_0c;   // 0x0c
    std::string m_28;   // 0x28: the game root path
    std::string m_44;   // 0x44
    std::string m_60;   // 0x60
    bool m_7c;          // 0x7c: running on Windows 2000/XP or later
    char m_pad[3];

    TAIL3ROOT();
    ~TAIL3ROOT();
};

// The owning handle the singleton lives in: assignment replaces and deletes.
struct TAIL3OWNER
{
    TAIL3ROOT *m_p;

    TAIL3OWNER(TAIL3ROOT *p) : m_p(p) {}

    TAIL3ROOT *operator=(TAIL3ROOT *p)
    {
        if (p != m_p) {
            delete m_p;
            m_p = p;
        }
        return p;
    }

    ~TAIL3OWNER()
    {
        delete m_p;
    }
};

// The lazy-init guard the compiler put next to the handle, and the registered
// teardown thunk.
extern unsigned int DAT_00a0f210;
extern TAIL3OWNER DAT_00a0f20c;
extern "C" void __cdecl FUN_00655280();

// The root singleton (0x554260).  It is a function-local static handle, so
// the compiler emits the guarded initialisation and the atexit teardown.
void *GetTail3Root()
{
    static TAIL3OWNER s_root = 0;

    if (s_root.m_p)
        return s_root.m_p;
    return s_root = new TAIL3ROOT();
}

// ---------------------------------------------------------------------------
// The std::map machinery this translation unit emitted out of line
// (0x554ef0-0x555d20).
//
// The unit was compiled with the fastcall default convention, which is why the
// free helpers below take their first two arguments in ecx/edx and only the
// leftovers on the stack. The map node holds a std::string key (0x1c bytes: the
// allocator at +0, _Bx at +4) followed by an 8-byte reference-counted handle,
// so a node is 0x34 bytes and _Color/_Isnil sit at 0x30/0x31 - the layout of
// tools/vc71/include/xtree. None of the helpers looks inside the value, so it
// is modelled as raw storage of the right size.

// The shared count a handle points at (0x5553c0 copy, 0x555410 release).
struct TL3REF
{
    char *m_str;        // 0x00

    TL3REF();
};

// The node value: an object plus its shared count.
struct TL3HANDLE
{
    V5547C0 *m_ptr;     // 0x00
    TL3REF m_ref;       // 0x04

    TL3HANDLE();
    TL3HANDLE(const TL3HANDLE &o);
    ~TL3HANDLE();
};

TL3HANDLE::TL3HANDLE(const TL3HANDLE &o)
{
    m_ptr = o.m_ptr;
    m_ref.m_str = o.m_ref.m_str;
    if (m_ref.m_str == 0) {
        if (m_ptr != 0) {
            m_ref.m_str = (char *)_aligned_malloc(4, 4);
            *(int *)m_ref.m_str = 1;
        }
    } else {
        g_fn6561b4(m_ref.m_str);
    }
}

TL3HANDLE::~TL3HANDLE()
{
    V5547C0 *ptr = m_ptr;

    if (m_ref.m_str && g_fn6561ac((long *)m_ref.m_str) == 0) {
        delete ptr;
        _aligned_free(m_ref.m_str);
        m_ref.m_str = 0;
    }
}

// The map's value_type: the key followed by the handle.
struct TL3VALUE
{
    std::string m_key;  // 0x00
    TL3HANDLE m_handle; // 0x1c
};

struct TL3NODE
{
    TL3NODE *m_left;    // 0x00
    TL3NODE *m_parent;  // 0x04
    TL3NODE *m_right;   // 0x08
    std::string m_key;  // 0x0c: the pair's first member
    TL3HANDLE m_handle; // 0x28: the pair's second member
    char m_color;       // 0x30
    char m_isnil;       // 0x31

    TL3NODE(TL3NODE *l, TL3NODE *p, TL3NODE *r, const TL3VALUE &v,
            char color);
};

// A fresh node holding a copy of the value (0x555d20).
TL3NODE::TL3NODE(TL3NODE *l, TL3NODE *p, TL3NODE *r, const TL3VALUE &v,
                 char color)
    : m_left(l), m_parent(p), m_right(r), m_key(v.m_key),
      m_handle(v.m_handle), m_color(color), m_isnil(0)
{
}

class TL3ITER;

struct TL3TREE
{
    int m_comp;         // 0x00: std::less<..., std::string>
    TL3NODE *m_head;    // 0x04
    unsigned int m_size;// 0x08

    static TL3NODE * __fastcall Max(TL3NODE *p);
    static TL3NODE * __fastcall Min(TL3NODE *p);
    TL3NODE *Lbound(const std::string &keyval) const;
    TL3ITER Find(const std::string &keyval) const;
    void Lrotate(TL3NODE *where);
    void Rrotate(TL3NODE *where);
    TL3NODE *Buynode();
};

// Rightmost node of the subtree at p (0x554f50).
TL3NODE * __fastcall TL3TREE::Max(TL3NODE *p)
{
    while (!p->m_right->m_isnil)
        p = p->m_right;
    return p;
}

// Leftmost node of the subtree at p (0x554f70).
TL3NODE * __fastcall TL3TREE::Min(TL3NODE *p)
{
    while (!p->m_left->m_isnil)
        p = p->m_left;
    return p;
}

// Leftmost node not less than keyval (0x555510).
__declspec(noinline) TL3NODE *TL3TREE::Lbound(const std::string &keyval) const
{
    TL3NODE *p = m_head->m_parent;  // the root
    TL3NODE *where = m_head;        // end() if search fails

    while (!p->m_isnil)
        if (p->m_key < keyval)
            p = p->m_right;     // descend right subtree
        else
        {   // p is not less than keyval, remember it
            where = p;
            p = p->m_left;      // descend left subtree
        }

    return where;   // return best remembered candidate
}

// Promote the left node to the root of the subtree at where (0x554ef0).
__declspec(noinline) void TL3TREE::Rrotate(TL3NODE *where)
{
    TL3NODE *p = where->m_left;
    where->m_left = p->m_right;

    if (!p->m_right->m_isnil)
        p->m_right->m_parent = where;
    p->m_parent = where->m_parent;

    if (where == m_head->m_parent)
        m_head->m_parent = p;
    else if (where == where->m_parent->m_right)
        where->m_parent->m_right = p;
    else
        where->m_parent->m_left = p;

    p->m_right = where;
    where->m_parent = p;
}

// Promote the right node to the root of the subtree at where (0x555060).
__declspec(noinline) void TL3TREE::Lrotate(TL3NODE *where)
{
    TL3NODE *p = where->m_right;
    where->m_right = p->m_left;

    if (!p->m_left->m_isnil)
        p->m_left->m_parent = where;
    p->m_parent = where->m_parent;

    if (where == m_head->m_parent)
        m_head->m_parent = p;
    else if (where == where->m_parent->m_left)
        where->m_parent->m_left = p;
    else
        where->m_parent->m_right = p;

    p->m_left = where;
    where->m_parent = p;
}

// The head/nil node, links zeroed, black and not a nil marker (0x555380).
__declspec(noinline) TL3NODE *TL3TREE::Buynode()
{
    TL3NODE *where = (TL3NODE *)::operator new(sizeof(TL3NODE));

    ::new ((void *)&where->m_left) TL3NODE *(0);
    ::new ((void *)&where->m_parent) TL3NODE *(0);
    ::new ((void *)&where->m_right) TL3NODE *(0);
    where->m_color = 1;
    where->m_isnil = 0;
    return where;
}

// An iterator is just the node it points at.
class TL3ITER
{
public:
    TL3ITER(TL3NODE *p) : m_ptr(p) {}
    TL3ITER(const TL3ITER &other) : m_ptr(other.m_ptr) {}

    void Inc();
    void Dec();

    TL3NODE *m_ptr;
};

// Next larger value (0x554f90).
__declspec(noinline) void TL3ITER::Inc()
{
    if (m_ptr->m_isnil)
        ;   // end() shouldn't be incremented, don't move
    else if (!m_ptr->m_right->m_isnil)
        m_ptr = TL3TREE::Min(m_ptr->m_right);
    else
    {   // climb looking for right subtree
        TL3NODE *p;
        while (!(p = m_ptr->m_parent)->m_isnil && m_ptr == p->m_right)
            m_ptr = p;  // ==> parent while right subtree
        m_ptr = p;      // ==> parent (head if end())
    }
}

// Next smaller value (0x5550c0).
__declspec(noinline) void TL3ITER::Dec()
{
    if (m_ptr->m_isnil)
        m_ptr = m_ptr->m_right;     // end() ==> rightmost
    else if (!m_ptr->m_left->m_isnil)
        m_ptr = TL3TREE::Max(m_ptr->m_left);
    else
    {   // climb looking for left subtree
        TL3NODE *p;
        while (!(p = m_ptr->m_parent)->m_isnil && m_ptr == p->m_left)
            m_ptr = p;  // ==> parent while left subtree
        if (!p->m_isnil)
            m_ptr = p;  // ==> parent if not head
    }
}

// Lower-bound lookup that reports failure as end() (0x555690).
TL3ITER TL3TREE::Find(const std::string &keyval) const
{
    TL3ITER it(Lbound(keyval));

    if (it.m_ptr == m_head || keyval < it.m_ptr->m_key)
        return TL3ITER(m_head);
    return TL3ITER(it);
}

// ---------------------------------------------------------------------------
// The named object whose full name is split at the last "::" (0x555700).

struct TL3NAMEOBJ
{
    char m_00[0x10];    // 0x00
    std::string m_10;   // 0x10: the short name
    std::string m_2c;   // 0x2c: the full name

    void SetName(const std::string &name);
};

void TL3NAMEOBJ::SetName(const std::string &name)
{
    m_2c = name;

    unsigned int pos = m_2c.find_last_of("::", std::string::npos, 2);
    if (pos == std::string::npos)
        m_10 = m_2c;
    else
        m_10 = m_2c.substr(pos + 1, std::string::npos);
}

// ---------------------------------------------------------------------------
// The name getter that chains two virtual calls (0x554660).

class TL3NAMEHOLDER
{
public:
    virtual void v0();
    virtual void v1();
    virtual const std::string &v2();
};

class TL3NAMESOURCE
{
public:
    virtual void v0();
    virtual TL3NAMEHOLDER *v1();

    std::string GetName();
};

std::string TL3NAMESOURCE::GetName()
{
    return v1()->v2();
}

// std::_Vector_base<...>::_Xlen (0x555cb0): the vector length overflow throw.
void TL3VECXLEN()
{
    throw std::length_error("vector<T> too long");
}

// The narrow-character transform the string helpers share (0x5549e0):
// it applies fn to [first, last) and reports the advanced output cursor.
void __fastcall TL3TRANSFORM(char **result, int (__cdecl *fn)(int),
                             const char *first, const char *last, char *dest)
{
    for (; first != last; ++first)
        *dest++ = fn(*first);
    *result = dest;
}

// The lowercase copy of a narrow string (0x554d80).
std::string __fastcall TL3LOWER(const std::string &src)
{
    std::string out;

    out.resize(src.size(), 0);
    char *end;
    TL3TRANSFORM(&end, _tolower, src.c_str(), src.c_str() + src.size(), &out[0]);
    return out;
}

// The two element sizes the unit's fills and copies work on: a 4-byte scalar
// and an 8-byte pair.
struct TL3A4
{
    int m_a;
};

struct TL3A8
{
    int m_a;
    int m_b;
};

// The allocator's placement construct, spelled out so it inlines the same way.
template<class T>
struct TL3ALLOC
{
    void construct(T *p, const T &value)
    {
        ::new ((void *)p) T(value);
    }
};

struct TL3STL
{
    static void __fastcall Fill8(TL3A8 *first, TL3A8 *last, const TL3A8 &value);
    static void __fastcall Fill4(TL3A4 *first, TL3A4 *last, const TL3A4 &value);
    static TL3A8 * __fastcall CopyBack8(TL3A8 *first, TL3A8 *last, TL3A8 *dest);
    static TL3A4 * __fastcall CopyBack4(TL3A4 *first, TL3A4 *last, TL3A4 *dest);
    static TL3A8 * __fastcall UninitCopy8(TL3A8 *first, TL3A8 *last,
                                          TL3A8 *dest, TL3ALLOC<TL3A8> &al,
                                          int tag);
    static void __fastcall UninitFill8(TL3A8 *first, unsigned int count,
                                       const TL3A8 &value, TL3ALLOC<TL3A8> &al,
                                       int tag);
};

// std::fill<TL3A8*, TL3A8> (0x554ff0).
__declspec(noinline) void __fastcall TL3STL::Fill8(TL3A8 *first, TL3A8 *last,
                                                   const TL3A8 &value)
{
    for (; first != last; ++first)
        *first = value;
}

// std::fill<TL3A4*, TL3A4> (0x555020).
__declspec(noinline) void __fastcall TL3STL::Fill4(TL3A4 *first, TL3A4 *last,
                                                   const TL3A4 &value)
{
    for (; first != last; ++first)
        *first = value;
}

// std::copy_backward for the 8-byte element (0x555120).
__declspec(noinline) TL3A8 * __fastcall TL3STL::CopyBack8(TL3A8 *first,
                                                          TL3A8 *last,
                                                          TL3A8 *dest)
{
    for (; first != last; *--dest = *--last)
        ;
    return dest;
}

// std::copy_backward for the 4-byte element, through memmove (0x555150).
__declspec(noinline) TL3A4 * __fastcall TL3STL::CopyBack4(TL3A4 *first,
                                                          TL3A4 *last,
                                                          TL3A4 *dest)
{
    int off = last - first;     // NB: non-overlapping move

    return (TL3A4 *)memmove(dest - off, first, off * sizeof(TL3A4));
}

// std::uninitialized_copy with an allocator (0x555180).
__declspec(noinline) TL3A8 * __fastcall TL3STL::UninitCopy8(TL3A8 *first,
                                                            TL3A8 *last,
                                                            TL3A8 *dest,
                                                            TL3ALLOC<TL3A8> &al,
                                                            int tag)
{
    for (; first != last; ++dest, ++first)
        al.construct(dest, *first);
    return dest;
}

// std::uninitialized_fill_n with an allocator (0x555450).
__declspec(noinline) void __fastcall TL3STL::UninitFill8(TL3A8 *first,
                                                         unsigned int count,
                                                         const TL3A8 &value,
                                                         TL3ALLOC<TL3A8> &al,
                                                         int tag)
{
    for (; 0 < count; --count, ++first)
        al.construct(first, value);
}
// std::uninitialized_fill_n that reports the advanced output (0x555650).
TL3A8 *__fastcall TL3UFILL(TL3ALLOC<TL3A8> &al, TL3A8 *first, unsigned int count,
                           const TL3A8 &value)
{
    TL3STL::UninitFill8(first, count, value, al, (int)&value);
    return first + count;
}

