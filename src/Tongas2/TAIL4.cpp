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

class V5547C4
{
public:
    virtual ~V5547C4();

    int m_4;
};

class V554E60
{
public:
    virtual ~V554E60() {}
};

class GD559a10 : public V5547C4, public V554E60
{
public:
    virtual ~GD559a10();

    std::map<int, int> m_c;
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
    virtual ~GD557e70BASE();

    char pad_04[8];
};

class GD557e70 : public V5547C4, public GD557e70BASE
{
public:
    virtual ~GD557e70();

    std::string m_14;
    std::string m_30;
    char pad_4c[4];
    std::string m_50;
};

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

// Two of the map's members already build byte-identical to unnamed targets:
//   0x5571a0 (183) == ?erase@...string,TAIL4MAPVALUE...  (iterator,iterator)
//   0x559620 (429) == ?_Insert@...string,TAIL4MAPVALUE...
// They only need the symbols.txt rows below; the bodies need no change.

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
            const G2TL3TGT1 &value, char color);

    TL3NODE *m_left;        // 0x00
    TL3NODE *m_parent;      // 0x04
    TL3NODE *m_right;       // 0x08
    G2TL3TGT1 m_value;      // 0x0c
    char m_color;           // 0x30
    char m_isnil;           // 0x31
};

TL3NODE::TL3NODE(TL3NODE *left, TL3NODE *parent, TL3NODE *right,
                 const G2TL3TGT1 &value, char color)
    : m_left(left), m_parent(parent), m_right(right), m_value(value),
      m_color(color), m_isnil(0)
{
}

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
    char *p;
    if (radix < 2 || radix > 36)
        radix = 10;
    if (value < 0) {
        unsigned int magnitude = -value;
        unsigned int quotient = magnitude / radix;
        unsigned int remainder = magnitude % radix;
        *buffer = '-';
        p = buffer + 1;
        if (0 < quotient)
            p = GD559D40(quotient, p, radix);
        *p = GD559DIGITS[remainder];
        p[1] = 0;
        return buffer;
    }
    unsigned int positive = (unsigned int)value;
    unsigned int quotient = positive / radix;
    unsigned int remainder = positive % radix;
    p = buffer;
    if (0 < quotient)
        p = GD559D40(quotient, buffer, radix);
    *p = GD559DIGITS[remainder];
    p[1] = 0;
    return buffer;
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
class GD557a40ITEM
{
public:
    GD557a40ITEM();
    void Call();

    int m_00[6];        // 0x00, 0x18 bytes in all
};

class GD557a40
{
public:
    GD557a40();

    GD557a40ITEM *m_0;
};

GD557a40::GD557a40()
{
    GD557a40ITEM *p = 0;
    p = new GD557a40ITEM();
    m_0 = p;
    if (p)
        p->Call();
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