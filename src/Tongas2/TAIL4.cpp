#include <stdlib.h>
#include <string>
#include <vector>
#include <map>
#include <windows.h>

// Tongas2/TAIL4 -- discovery scaffold for the remaining 0x54eec0-range code.

// The object the +0x04 pointer releases; slot 0 takes the count.
class GD556050OBJ
{
public:
    virtual void Release(int value);
};

// One element of the vector at 0x54: an int and an owned buffer.
class GD556050ITEMVALUE
{
};

struct GD556050ITEM
{
    int m_0;
    void *m_4;
};

// The growable array at +0x58.
struct GD556050ARRAY
{
    GD556050ITEM *m_begin;
    GD556050ITEM *m_end;
    GD556050ITEM *m_cap;
};

// Owns a heap buffer and releases it in its destructor (the word at +0xc).
class GD556050FREE
{
public:
    ~GD556050FREE()
    {
        ::operator delete(m_p);
    }

    void *m_p;
};

class GD556050
{
public:
    virtual ~GD556050();

    GD556050OBJ *m_04;
    int m_08;
    GD556050FREE m_0c;
    std::string m_10;
    std::string m_2c;
    int m_48;
    int m_4c;
    bool m_50;
    char pad_51[0x54 - 0x51];
    int m_54;
    GD556050ARRAY m_58;
};

__declspec(noinline) GD556050::~GD556050()
{
    int count = m_58.m_begin ? (int)(m_58.m_end - m_58.m_begin) : 0;
    for (int i = 0; i < count; i++)
        ::operator delete(m_58.m_begin[i].m_4);
    if (m_04)
        m_04->Release(1);
    if (m_58.m_begin)
        ::operator delete(m_58.m_begin);
    m_58.m_begin = 0;
    m_58.m_end = 0;
    m_58.m_cap = 0;
}

// The two-level base chain: V5547C4 derives from V5547C0 (its destructor at
// 0x5545a0 sets its own vtable and jumps to V5547C0's).  The constructors are
// out of line in other units, so only the base call is visible here.
class V5547C0
{
public:
    V5547C0();
    virtual ~V5547C0();

    int m_4;
};

class V5547C4 : public V5547C0
{
public:
    virtual ~V5547C4();
};

class V554E60
{
public:
    virtual ~V554E60() {}
};

// The tree's stored value: 8 bytes, so a node is 0x34 bytes with the nil flag
// at 0x31 (the head node the constructor installs).
struct TAIL4GDVALUE
{
    int m_0;
    int m_4;
};

class GD559a10 : public V5547C4, public V554E60
{
public:
    GD559a10();
    virtual ~GD559a10();

    std::map<std::basic_string<char>, TAIL4GDVALUE> m_c;
};

__declspec(noinline) GD559a10::~GD559a10()
{
}

class GD557a30 : public GD556050
{
public:
    virtual ~GD557a30() throw();
};

__declspec(noinline) GD557a30::~GD557a30() throw()
{
}

class GD557d80 : public GD556050
{
public:
    virtual ~GD557d80() throw();
};

__declspec(noinline) GD557d80::~GD557d80() throw()
{
}

class GD557db0 : public GD556050
{
public:
    virtual ~GD557db0() throw();
};

__declspec(noinline) GD557db0::~GD557db0() throw()
{
}

class GD557de0 : public GD556050
{
public:
    virtual ~GD557de0() throw();
};

__declspec(noinline) GD557de0::~GD557de0() throw()
{
}

class GD557e10 : public GD556050
{
public:
    virtual ~GD557e10() throw();
};

__declspec(noinline) GD557e10::~GD557e10() throw()
{
}

class GD557e40 : public GD556050
{
public:
    virtual ~GD557e40() throw();
};

__declspec(noinline) GD557e40::~GD557e40() throw()
{
}

class GD559ad0 : public GD556050
{
public:
    virtual ~GD559ad0() throw();
};

__declspec(noinline) GD559ad0::~GD559ad0() throw()
{
}

class GD559aa0 : public V5547C4
{
public:
    virtual ~GD559aa0() throw();
};

__declspec(noinline) GD559aa0::~GD559aa0() throw()
{
}

int __fastcall IsSeparator(char c)
{
    return c == '\\' || c == '/';
}

class TAIL4X
{
public:
    int ReturnArg(int a);
};

int TAIL4X::ReturnArg(int a)
{
    return a;
}

class GD557e70BASE
{
public:
    // Out of line at 0x005c086d in another unit: zeroes the two dwords at +4
    // and +8 and installs the vtable at 0x00687e24.
    GD557e70BASE();
    virtual ~GD557e70BASE();

    char pad_04[8];
};

class GD557e70 : public V5547C4, public GD557e70BASE
{
public:
    // 0x00557f30 (0xc2 bytes): the four-argument constructor. V5547C4's
    // implicit ctor inlines here, so the emitted body is
    //   call V5547C0's ctor; [+0] = V5547C4's vtable;
    //   call GD557e70BASE's ctor on this+8; [+8] = GD557e70's BASE vtable;
    //   [+0] = GD557e70's V5547C4 vtable; then the three string assigns.
    GD557e70(const std::string &a, const std::string &b, int c, const std::string &d);
    virtual ~GD557e70();

    std::string m_14;
    std::string m_30;
    int m_4c;
    std::string m_50;
};

GD557e70::GD557e70(const std::string &a, const std::string &b, int c, const std::string &d)
    : m_14(a)
    , m_30(b)
    , m_4c(c)
    , m_50(d)
{
}

// The five exception factories (0x005580c0, 0x005583b0, 0x005586a0, 0x00558990,
// 0x00558c80; 409 bytes each, any two differing only in the third string and
// the pair of vtables the created object gets).  Each allocates a
// GD557e70-sized object, fills three stack strings - two "unknown"s and the
// class's own message - constructs the exception from (message, "unknown",
// -1, "unknown"), stores the result into the holder's first member and then
// takes the new object's first reference through G2TL3CALL::Call.  The raw
// ::operator new call (not a new-expression) keeps the null test: the
// throwing-new optimisation would delete it.
//
// NOT EXACT YET, measured to the instruction: the best factory shape compiles
// to 355 of 409 bytes.  The head now matches retail interleaved (each string's
// inline default ctor sits right before its assign) and the destructor blocks
// include the RESET - because VC 7.1's own ~basic_string RESETS the object:
// it frees the buffer when _Myres >= 0x10 and then writes the empty state
// (_Myres=0xf, _Mysize=0, _Bx=0), see the banked 0x00552220.  So the tail is
// three plain inlined string destructors; nothing hand-written.
//
// TWO hard negatives now bound the answer:
//
//   * RETAIL HAS NO try/catch AND NO __try.  Every try/catch form compiled
//     here switched the function to an EBP frame (mov esp,ebp / [ebp-X]
//     addressing), while retail addresses everything through [esp+X] - the
//     ESP frame that /GX uses for plain functions.  And both __try forms are
//     refused outright with C2712.  So the original is a plain /GX function.
//
//   * THE DESTRUCTION SITS AT THE MERGED EXIT, GUARDED.  Retail's null path
//     (new returned 0) falls into the main path: the same 'm_0 = obj' store,
//     the guarded Call, then the destructor blocks behind the construction
//     mask (a local set 1, 3, 7 at the three assigns, then
//     'mov eax,[esp+0x10]; or eax,8; test al,4/2/1'), and one epilogue that
//     returns this.  The EH state also reads 6 AT THE MERGE - the strings are
//     still live there in the scope table's eyes, which is only possible if
//     the destruction was laid out at the merge.  Our compiler keeps two
//     exits, so it needs no mask.  Retail's hoisted 0xf, its four live
//     callee-saved registers and its three stack spills all follow from the
//     same decision.
//
// The open question is therefore exactly one codegen choice: what makes
// MSVC 7.1 lay an if-scope's destructors at the shared continuation with a
// guard rather than inside the branch.  Eleven shapes are compiled and
// rejected in this note's history (best three: 355 with return-this and
// m_0/Call inside the strings' scope, 351 void-return variant, 348 with
// m_0/Call after the if).  All use the ESP frame like retail; none produces
// the mask.  The next ideas worth a pass: a source shape whose if-branch and
// else-branch both fall into the same continuation AND the destructors are
// the last thing before the return (e.g. restructuring so the null path
// skips only the construction, not the Call-guard), or comparing against a
// known retail function with the same mask idiom elsewhere in the image.
//
// That comparison is now done: a whole-image scan for 'or eax,8' followed by
// 'test al,4/2/1' finds exactly EIGHT sites, and the other three are in
// Tongas2/TEXTUREDX8CORE - 0x00553aa9 inside FUN_00553b40 (410 bytes),
// 0x00553c57 inside FUN_00553d00 (300 bytes), 0x00553f87 inside
// FUN_00553e70 (410 bytes).  They are the SAME family: identical 'unknown'
// literal at 0x0067dc44, identical assign call at 0x004073b0, identical
// mask local (mov [esp+0x18],3) and state stores.  So the shape unlocks eight
// factories (~3,270 bytes) plus the singletons behind them, and no already-
// matched example of the idiom exists anywhere else in the image to copy
// from.  FUN_00553b40's third literal and its vtables would name the two
// TEXTUREDX8CORE exception classes the same way the five above were named.

GD557e70::~GD557e70()
{
}

class __declspec(novtable) GD558000 : public GD557e70
{
public:
    virtual ~GD558000();
};

__declspec(noinline) GD558000::~GD558000()
{
}

// The retail link kept GD558000's destructor in this block.
void GD558000Release(GD558000 *object)
{
    object->~GD558000();
}

// The config tree at 0xa0f224 / 0xa0f22c: a string-keyed map whose nodes use a
// 0x24-byte value (string key plus an 8-byte payload), node stride 0x34.
struct TAIL4MAPVALUE
{
    int m_0;
    int m_4;
};

template class std::map<std::basic_string<char>, TAIL4MAPVALUE>;

// The same tree instantiated for an int key; erase/_Tidy compile to different
// mangled names but the same bytes as the string-keyed copies above.
template class std::map<int, int>;

// Two of the map's members already build byte-identical to unnamed targets:
//   0x5571a0 (183) == ?erase@...HH... / ...string,TAIL4MAPVALUE... (iterator,iterator)
//   0x559620 (429) == ?_Insert@...string,TAIL4MAPVALUE...
// 0x5571a0's own relocations point at ?EraseHandle@TL3TREE and FUN_005567f0,
// so it is really TL3TREE::Erase(first,last): the bytes are identical to the
// STL body, but naming it with the STL instantiation leaves the callee names
// different, which objdiff still scores as a difference.

// The 8-byte pair the notification list stores.
struct TAIL4PAIR8
{
    int m_0;
    int m_4;
};

template class std::vector<TAIL4PAIR8>;

// The list owner: a growable array of {int, handle} pairs at +0x54.
class TL4LIST
{
public:
    void Add(int value, int *source);

    int m_00[0x54 / 4];             // 0x00
    std::vector<TAIL4PAIR8> m_54;   // 0x54
};

// 00556db0 (60 bytes) -- append one pair, taking the handle out of source.
// Residue: 60 bytes with identical instruction and call structure; the vector
// end pointer lands in edx here and in esi in retail (register colour, 2
// bytes at +0x30).
void TL4LIST::Add(int value, int *source)
{
    TAIL4PAIR8 item;
    item.m_0 = value;
    item.m_4 = 0;
    m_54.push_back(item);
    int handle = *source;
    *source = 0;
    TAIL4PAIR8 *end = m_54.end()._Myptr;
    end[-1].m_4 = handle;
}

// ---------------------------------------------------------------------------
// The notification tree.  A node is 0x34 bytes: three links, a 0x24-byte
// value (a std::string plus a reference-counted handle) and the red/black
// colour and nil flags at 0x30/0x31.
// ---------------------------------------------------------------------------

class G2TL3CALL
{
public:
    void Call();

    int m_0;
    int m_4;
};

#define TAIL4_FACTORY(CLASS, HOLD, LIT)                                              \
    class CLASS : public GD557e70                                                   \
    {                                                                               \
    public:                                                                         \
        CLASS(const std::string &a, const std::string &b, int c,                     \
              const std::string &d);                                                 \
        virtual ~CLASS() throw();                                                    \
    };                                                                              \
                                                                                    \
    inline CLASS::CLASS(const std::string &a, const std::string &b, int c,          \
                         const std::string &d)                                       \
        : GD557e70(a, b, c, d)                                                       \
    {                                                                               \
    }                                                                               \
                                                                                    \
    __declspec(noinline) CLASS::~CLASS() throw()                                    \
    {                                                                               \
    }                                                                               \
                                                                                    \
    class HOLD                                                                      \
    {                                                                               \
    public:                                                                         \
        HOLD *Create();                                                             \
        CLASS *m_0;                                                                 \
    };                                                                              \
                                                                                    \
    HOLD *HOLD::Create()                                                            \
    {                                                                               \
        CLASS *obj = (CLASS *)::operator new(sizeof(CLASS));                        \
        if (obj) {                                                                  \
            std::string a;                                                         \
            a.assign("unknown", 7);                                                 \
            std::string b;                                                         \
            b.assign("unknown", 7);                                                 \
            std::string c;                                                         \
            c.assign(LIT, sizeof(LIT) - 1);                                         \
            ::new ((void *)obj) CLASS(c, b, -1, a);                                 \
            m_0 = obj;                                                              \
            reinterpret_cast<G2TL3CALL *>(obj)->Call();                             \
        } else {                                                                    \
            m_0 = obj;                                                              \
        }                                                                           \
        return this;                                                                \
    }

TAIL4_FACTORY(GD5580c0EX, GD5580c0HOLD, "Unauthorized Access")
TAIL4_FACTORY(GD5583b0EX, GD5583b0HOLD, "IO Exception")
TAIL4_FACTORY(GD5586a0EX, GD5586a0HOLD, "Directory not found")
TAIL4_FACTORY(GD558990EX, GD558990HOLD, "File not found")
TAIL4_FACTORY(GD558c80EX, GD558c80HOLD, "Path too long")

class G2TL3TH1
{
public:
    void Call();
};

class G2TL3TH2
{
public:
    void Call();
};

// One stored notification: a name plus a reference-counted handle.  The copy
// constructor touches 0x20 of the 0x24 bytes; the tail word is left alone.
class G2TL3TGT1
{
public:
    __declspec(noinline) G2TL3TGT1(const G2TL3TGT1 &other);
    ~G2TL3TGT1();

    std::string m_0;        // 0x00
    G2TL3CALL *m_1c;        // 0x1c
    void *m_20;             // 0x20
};

// 005593a0 (106 bytes) -- copy one stored value, taking a new reference.
G2TL3TGT1::G2TL3TGT1(const G2TL3TGT1 &other) : m_0(other.m_0), m_1c(other.m_1c)
{
    if (m_1c)
        m_1c->Call();
}

struct TL3NODE
{
    TL3NODE(TL3NODE *left, TL3NODE *parent, TL3NODE *right,
            const G2TL3TGT1 &value, char color)
        : m_left(left), m_parent(parent), m_right(right), m_value(value),
          m_color(color), m_isnil(0)
    {
    }

    TL3NODE *m_left;        // 0x00
    TL3NODE *m_parent;      // 0x04
    TL3NODE *m_right;       // 0x08
    G2TL3TGT1 m_value;      // 0x0c
    char m_color;           // 0x30
    char m_isnil;           // 0x31
};

class TL3TREE
{
public:
    TL3NODE *Head();
    TL3NODE *Buynode(TL3NODE *left, TL3NODE *parent, TL3NODE *right,
                     const G2TL3TGT1 &value, char color);
    void EraseValue(TL3NODE *node);
    void EraseHandle(TL3NODE *node);

    int m_compare;          // 0x00
    TL3NODE *m_head;        // 0x04
    unsigned int m_size;    // 0x08
};

// 00559540 (126 bytes) -- allocate and initialise one tree node.  The retail
// shape is the STL _Buynode pattern: a caught-throw handler deletes the new
// node, which is what pulls in the ebp frame and the ebx/edi saves.
TL3NODE *TL3TREE::Buynode(TL3NODE *left, TL3NODE *parent, TL3NODE *right,
                          const G2TL3TGT1 &value, char color)
{
    TL3NODE *node = (TL3NODE *)::operator new(sizeof(TL3NODE));
    try {
        ::new ((void *)node) TL3NODE(left, parent, right, value, color);
    } catch (...) {
        ::operator delete(node);
        throw;
    }
    return node;
}

// 00556140 (62 bytes) -- free a subtree, releasing the value first.
void TL3TREE::EraseValue(TL3NODE *rootnode)
{
    for (TL3NODE *pnode = rootnode; !pnode->m_isnil; rootnode = pnode) {
        EraseValue(pnode->m_right);
        pnode = pnode->m_left;
        ((G2TL3TH1 *)rootnode)->Call();
        ::operator delete(rootnode);
    }
}

// 00556b80 (62 bytes) -- free a subtree, releasing the handle first.
void TL3TREE::EraseHandle(TL3NODE *rootnode)
{
    for (TL3NODE *pnode = rootnode; !pnode->m_isnil; rootnode = pnode) {
        EraseHandle(pnode->m_right);
        pnode = pnode->m_left;
        ((G2TL3TH2 *)rootnode)->Call();
        ::operator delete(rootnode);
    }
}

// 00559d40 (60 bytes) -- recursive unsigned digit emitter.
static const char GD559DIGITS[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

static char *GD559D40(unsigned int value, char *buffer, unsigned int radix)
{
    unsigned int quotient = value / radix;
    unsigned int remainder = value % radix;
    if (0 < quotient)
        buffer = GD559D40(quotient, buffer, radix);
    *buffer = GD559DIGITS[remainder];
    return buffer + 1;
}

// 00559d80 (124 bytes) -- signed integer to string in an arbitrary radix.
char *GD559D80(int value, char *buffer, int radix)
{
    char *start = buffer;
    char *p;
    if (radix < 2 || radix > 36)
        radix = 10;
    if (value < 0) {
        unsigned int magnitude = -value;
        unsigned int quotient = magnitude / radix;
        unsigned int remainder = magnitude % radix;
        *start = '-';
        p = start + 1;
        if (0 < quotient)
            p = GD559D40(quotient, p, radix);
        *p = GD559DIGITS[remainder];
        p[1] = 0;
        return start;
    }
    unsigned int positive = (unsigned int)value;
    unsigned int quotient = positive / radix;
    unsigned int remainder = positive % radix;
    p = start;
    if (0 < quotient)
        p = GD559D40(quotient, start, radix);
    *p = GD559DIGITS[remainder];
    p[1] = 0;
    return start;
}

// 00558fe0 (121 bytes) -- reads ProgramFilesDir out of the registry.
int __fastcall GD558FE0(char *buffer, unsigned int size)
{
    HKEY key = NULL;
    int result = 0;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE,
                      "SOFTWARE\\Microsoft\\Windows\\CurrentVersion",
                      0, 1, &key) == 0) {
        DWORD type;
        DWORD length = size;
        if (RegQueryValueExA(key, "ProgramFilesDir", NULL, &type,
                             (LPBYTE)buffer, &length) == 0) {
            if (size != 0)
                buffer[size - 1] = '\0';
            result = 1;
        }
    }
    if (key != NULL)
        RegCloseKey(key);
    return result;
}

// 00557a40 (109 bytes) -- constructor that owns one reference-counted object.
class GD557a40
{
public:
    GD557a40();

    GD559a10 *m_0;
};

GD557a40::GD557a40()
{
    GD559a10 *p = 0;
    p = new GD559a10();
    m_0 = p;
    if (p)
        ((G2TL3CALL *)p)->Call();
}

// 00557290 (128 bytes) -- the map bases at +0xc and installs its head node.
GD559a10::GD559a10()
{
}
// 005563b0 (114 bytes + 0x15 handler) -- the notification tree's node allocator
// for the derived list.  The node ctor stays out of line (00555d20), so this is
// the same caught-throw allocate/construct body as Buynode above.
class GD5563NODE
{
public:
    GD5563NODE(GD5563NODE *left, GD5563NODE *parent, GD5563NODE *right,
               const G2TL3TGT1 &value, char color);

    GD5563NODE *m_left;     // 0x00
    GD5563NODE *m_parent;   // 0x04
    GD5563NODE *m_right;    // 0x08
    G2TL3TGT1 m_value;      // 0x0c
    char m_color;           // 0x30
    char m_isnil;           // 0x31
};

class GD5563TREE
{
public:
    GD5563NODE *Buynode(GD5563NODE *left, GD5563NODE *parent,
                        GD5563NODE *right, const G2TL3TGT1 &value, char color);
};

GD5563NODE *GD5563TREE::Buynode(GD5563NODE *left, GD5563NODE *parent,
                               GD5563NODE *right, const G2TL3TGT1 &value,
                               char color)
{
    GD5563NODE *node = (GD5563NODE *)::operator new(sizeof(GD5563NODE));
    try {
        ::new ((void *)node) GD5563NODE(left, parent, right, value, color);
    } catch (...) {
        ::operator delete(node);
        throw;
    }
    return node;
}

// ---------------------------------------------------------------------------
// Two unimplemented families in this unit, both found by grouping unmatched
// bodies by size (build/scan_clusters.py):
//
//   six 300-byte bodies at 0x00558270, 0x00558560, 0x00558850, 0x00558b40,
//   0x00558e30, 0x00559b80. These ARE twins: any two differ in only 28-31 bytes
//   across 13 short runs (an earlier version of this note compared 0x00558270
//   with 0x00558570, which is 0x10 bytes into the second body, and wrongly
//   concluded they were unrelated). Each is a one-shot singleton accessor:
//
//       T *s_instance;                      // its own global at 0xa0f25c + 4i
//       if (!s_instance) {
//           name = <type descriptor>;       // 0x006c6f18 + per-class descriptor
//           if (*name == 'c') name += 6; else name += 7;   // skip the "c_" prefix
//           s_instance = new T(name, ...);  // ctor at 0x005576a0, vptr 0x0067dfc0
//           ((Vtbl *)s_instance)->V09();    // virtual call through +0x24
//       }
//       return s_instance;
//
//   so the per-class difference is the static at 0xa0f25c/0x60/0x64/0x68/0x6c/
//   0x74 and the type-descriptor pointer at 0x6c6f18 + 0x20i. Modelling one
//   class (vtable + the ctor's signature) gives all six.
// ---------------------------------------------------------------------------
