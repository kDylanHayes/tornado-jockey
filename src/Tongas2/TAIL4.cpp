#include <string>

// Tongas2/TAIL4 -- discovery scaffold for the remaining 0x54eec0-range code.

class GD556050
{
public:
    virtual ~GD556050();
};

__declspec(noinline) GD556050::~GD556050()
{
}

class V5547C4
{
public:
    virtual ~V5547C4();

    int m_4;
};

class GD559a10
{
public:
    virtual ~GD559a10();
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
