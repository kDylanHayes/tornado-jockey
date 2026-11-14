#include "Tongas2/MATHS.h"
// Tongas2/TEXTURERESIZER -- the texture resizer.
//
// Discovery scaffold: bodies are empty until the unit is matched; each
// function has its exact target size in config/symbols.txt so a later delink
// cannot cut the target short.
#include "Tongas2/TEXTURERESIZER.h"
#include "Tongas2/BONEDMODELDX8SD.h"
#include "Tongas2/APPWINDOW.h"

void TEXTURERESIZER::UpdateTextures()
{
}

TEXTURERESIZERNODE::TEXTURERESIZERNODE()
{
    m_0 = 0;
    m_4 = 0;
    m_8 = "";
    m_24 = "";
}

TEXTURERESIZER::TEXTURERESIZER()
{
    m_4 = new TEXTURERESIZERNODE();
}

class G2AGTGET
{
public:
    void *Get10();
    void *Get4();
};

class TRINNER
{
public:
    int M3(int a, int b, int c, float d);
    void M4();
    void M10(int a, int b);
};

class TRX
{
public:
    int F3(int a, int b, int c);
    void F4();
    V3 F5();
    V3 F6();
    bool F7();
    int F10(int a, const int &b);
};

int TRX::F3(int a, int b, int c)
{
    return ((TRINNER *)*(void **)((char *)this + 0x668))->M3(
        *(int *)((char *)this + 0x444), b, c, 1.0f);
}

void TRX::F4()
{
    ((TRINNER *)*(void **)((char *)this + 0x668))->M4();
}

V3 TRX::F5()
{
    return *(V3 *)((G2AGTGET *)*(void **)((char *)this + 0x668))->Get10();
}

V3 TRX::F6()
{
    return *(V3 *)((G2AGTGET *)*(void **)((char *)this + 0x668))->Get4();
}

bool TRX::F7()
{
    return *(void **)((char *)this + 0x668) != 0;
}

int TRX::F10(int a, const int &b)
{
    ((TRINNER *)*(void **)((char *)this + 0x668))->M10(a, b);
    return a;
}

class GD535df0CTX
{
public:
    void m_47(void *arg);
};

class GD535df0 : public BONEDMODELDX8SDRESOURCE
{
public:
    virtual ~GD535df0();

    void *m_668;
};

GD535df0::~GD535df0()
{
    void *value = m_668;
    if (value) {
        GD535df0CTX *display = (GD535df0CTX *)*(void **)((char *)g_pDisplay + 0x2bc);
        display->m_47(value);
    }
}

// FUN_00535be0 (0x00535be0) and FUN_00535cb0 (0x00535cb0): the resizer's two
// list appenders.  Both take the key plus one or two names by value, walk the
// node list to its last entry, hang a fresh node behind it and hand the names
// to the tail node.  The header cannot be extended from this file, so a
// layout-compatible placeholder carries the bodies: the vtable pointer of
// TEXTURERESIZER sits at +0 and its node list head at +4.
class GD535be0
{
public:
    void *m_vptr;
    TEXTURERESIZERNODE *m_4;

    void Add(int key, std::string name);
    void Add2(int key, std::string name1, std::string name2);
};

void GD535be0::Add(int key, std::string name)
{
    if (name != "" && key) {
        TEXTURERESIZERNODE *node = m_4;
        for (; node->m_4; node = node->m_4)
            ;
        TEXTURERESIZERNODE *added = new TEXTURERESIZERNODE();
        node->m_4 = added;
        node->m_0 = key;
        node->m_8.assign(name, 0, std::string::npos);
        return;
    }
}

void GD535be0::Add2(int key, std::string name1, std::string name2)
{
    if (name1 != "" && key) {
        TEXTURERESIZERNODE *node = m_4;
        while (node->m_4)
            node = node->m_4;
        TEXTURERESIZERNODE *added = new TEXTURERESIZERNODE();
        node->m_4 = added;
        node->m_0 = key;
        node->m_8.assign(name1, 0, std::string::npos);
        node->m_24.assign(name2, 0, std::string::npos);
    }
}
