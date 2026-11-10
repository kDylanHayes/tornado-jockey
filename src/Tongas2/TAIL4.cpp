#include <stdlib.h>
#include <string>
#include <vector>
#include <map>

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

// The growable array at +0x58: begin/end/capacity, cleared by freeing each
// element's buffer and then the storage.
struct GD556050ARRAY
{
    GD556050ITEM *m_begin;
    GD556050ITEM *m_end;
    GD556050ITEM *m_cap;

    __forceinline void Clear()
    {
        int count = m_begin ? (int)(m_end - m_begin) : 0;
        for (int i = 0; i < count; i++) {
            GD556050ITEM *item = m_begin + i;
            free(item->m_4);
        }
        if (m_begin)
            free(m_begin);
        m_begin = 0;
        m_end = 0;
        m_cap = 0;
    }
};

// Owns a heap buffer and frees it in its destructor (the word at +0xc).
class GD556050FREE
{
public:
    ~GD556050FREE()
    {
        free(m_p);
    }

    void *m_p;
};

// The non-polymorphic part of the object, shifted to +4 by the derived vptr.
class GD556050BASE
{
public:
    __forceinline ~GD556050BASE()
    {
        if (m_04)
            m_04->Release(1);
    }

    GD556050OBJ *m_04;
    int m_08;
    GD556050FREE m_c;
    std::string m_10;
    std::string m_2c;
    int m_48;
    int m_4c;
    bool m_50;
    char pad_51[0x58 - 0x51];
};

class GD556050 : public GD556050BASE
{
public:
    virtual ~GD556050();

    GD556050ARRAY m_54;
};

__declspec(noinline) GD556050::~GD556050()
{
    m_54.Clear();
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
