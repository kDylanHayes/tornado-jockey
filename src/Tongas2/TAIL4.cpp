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
    for (int i = 0; i < count; i++) {
        GD556050ITEM *item = m_58.m_begin + i;
        ::operator delete(item->m_4);
    }
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

// 00559d40 (60 bytes) -- recursive unsigned digit emitter.
static const char GD559DIGITS[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

static char *GD559D40(unsigned int value, char *buffer, unsigned int radix)
{
    unsigned int quotient = value / radix;
    unsigned int remainder = value % radix;
    if (0 < quotient) {
        char *p = GD559D40(quotient, buffer, radix);
        *p = GD559DIGITS[remainder];
        return p + 1;
    }
    else {
        *buffer = GD559DIGITS[remainder];
        return buffer + 1;
    }
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
        p[1] = '\0';
        return buffer;
    }
    unsigned int positive = (unsigned int)value;
    unsigned int quotient = positive / radix;
    unsigned int remainder = positive % radix;
    p = buffer;
    if (0 < quotient)
        p = GD559D40(quotient, buffer, radix);
    *p = GD559DIGITS[remainder];
    p[1] = '\0';
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
