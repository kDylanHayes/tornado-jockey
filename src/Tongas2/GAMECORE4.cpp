// Tongas2/GAMECORE4 -- the game-code block at 0x4410b0. The string, vector
// and tree members the original translation unit emitted out of line, and the
// small resource accessors that live in the same block.
#define GC4_OUTOFLINE
#define MATRIX_AXIS_METHODS
#define ANG_UPDATE_OUTOFLINE

#include <map>
#include <string>
#include <vector>

#include "Tongas2/ACTOR.h"
#include "Tongas2/ACTORLINK.h"
#include "Tongas2/DECAL.h"
#include "Tongas2/RESOURCES.h"
#include "Tongas2/SAFEINDEX.h"
#include "Tongas2/SCREENDECAL.h"
#include "Tongas2/SOUND.h"
#include "Tongas2/STRING.h"
#include "Tongas2/UNICODESTRING.h"

template class std::basic_string<char>;
template class std::vector<std::basic_string<char> >;
template class std::map<int, RESHANDLE>;
template struct std::less<std::basic_string<char> >;
template std::basic_string<char> std::operator+<char, std::char_traits<char>, std::allocator<char> >(
    const std::basic_string<char> &, const std::basic_string<char> &);
template std::basic_string<char> std::operator+<char, std::char_traits<char>, std::allocator<char> >(
    const std::basic_string<char> &, const char *);
template class CALLBACKLIST<RESOURCESCALLBACK>;

int RESOURCES::GetClassId()
{
    return s_classId;
}

int RESOURCES::GetParentClassId()
{
    return g_classParents[s_classId];
}

const char *RESOURCES::GetClassName()
{
    return g_classNames[s_classId];
}

void RESOURCES::SetSoundVolume(const float &volume)
{
    m_soundVolume = volume;
}

void RESOURCES::SetMusicVolume(const float &volume)
{
    m_musicVolume = volume;
}

float RESOURCES::GetMusicVolume()
{
    return m_musicVolume;
}

float RESOURCES::GetSoundVolume()
{
    return m_soundVolume;
}

void SCREENDECALMASTER::SetForceDuplicates(const bool &force)
{
    m_forceDuplicates = force;
}

void DECALMASTER::SetLoop(const bool &value)
{
    m_loop = value;
}

void DECALMASTER::SetAnimLen(const float &value)
{
    m_animLen = value;
}

void DECALMASTER::SetStartFrame(const int &value)
{
    m_startFrame = value;
}

void ACTOR::SetIsTemplate(const bool &value)
{
    m_isTemplate = value;
}

void ACTOR::SetColliderColorOverride(const bool &value)
{
    m_colliderColorOverride = value;
}

SOUNDMASTER::~SOUNDMASTER()
{
}

SOUNDMANAGER::SOUNDMANAGER()
{
    SafeSprintf(m_fileName, "SOUND.txt");
    m_8 = false;
    m_18 = true;
}

RESOURCEMASTER *SOUNDMANAGER::CreateMaster(const char *name)
{
    return new SOUNDMASTER(this, name);
}

const char *SOUNDMANAGER::GetTypeCode()
{
    return "SNFX";
}

bool SOUNDMANAGER::IsMyType(const char *name)
{
    return strstr(name, ".wav");
}

UNICODESTRING::UNICODESTRING()
{
    m_block = -1;
    m_pages = 0;
    m_buf = 0;
    m_length = 0;
    m_lengthValid = true;
}

UNICODESTRING::~UNICODESTRING() throw()
{
    if (m_pages > 0) {
        if (m_block >= 0)
            ((STRINGMANAGER *)&g_UnicodeStringManager)->Free(m_block, m_pages);
        m_block = -1;
        m_pages = 0;
        m_buf = 0;
        m_length = 0;
        m_lengthValid = true;
    }
}

void UNICODESTRING::Free()
{
    if (m_block >= 0)
        ((STRINGMANAGER *)&g_UnicodeStringManager)->Free(m_block, m_pages);
    m_block = -1;
    m_pages = 0;
    m_buf = 0;
    m_length = 0;
    m_lengthValid = true;
}

void *RESOURCE::Virtual09(const char *name)
{
    (void)name;
    return 0;
}

void *RESOURCES::Virtual09(const char *name)
{
    return FindMaster(name);
}

float ACTORLINK::Link14()
{
    return 1.0f;
}

void ANG::GetAxes(V3 *forward, V3 *up)
{    if (flags & MATRIX_DIRTY)
        UpdateMatrix();
    {
        V3 axis = matrix.GetForward();
        *forward = axis;
    }
    if (flags & MATRIX_DIRTY)
        UpdateMatrix();
    {
        V3 axis = matrix.GetUp();
        *up = axis;
    }
}

class V44AC00
{
public:
    virtual ~V44AC00();
};

V44AC00::~V44AC00()
{
}

class GD440960
{
public:
    virtual ~GD440960();
};

__declspec(noinline) GD440960::~GD440960()
{
}

class GD4413d0
{
public:
    virtual ~GD4413d0();
};

__declspec(noinline) GD4413d0::~GD4413d0()
{
}

class GD441630
{
public:
    virtual ~GD441630();
};

__declspec(noinline) GD441630::~GD441630()
{
}

class GD442440
{
public:
    virtual ~GD442440();
};

__declspec(noinline) GD442440::~GD442440()
{
}

class GD43f600
{
public:
    virtual ~GD43f600();
};

__declspec(noinline) GD43f600::~GD43f600()
{
}

class GD4433c0
{
public:
    virtual ~GD4433c0();
};

__declspec(noinline) GD4433c0::~GD4433c0()
{
}

class GD4434d0
{
public:
    virtual ~GD4434d0();
};

__declspec(noinline) GD4434d0::~GD4434d0()
{
}

class GD548040
{
public:
    virtual ~GD548040();
};

__declspec(noinline) GD548040::~GD548040()
{
}

class GD443f70
{
public:
    virtual ~GD443f70();
};

__declspec(noinline) GD443f70::~GD443f70()
{
}

class GD444450
{
public:
    virtual ~GD444450();
};

__declspec(noinline) GD444450::~GD444450()
{
}

class GD444730
{
public:
    virtual ~GD444730();
};

__declspec(noinline) GD444730::~GD444730()
{
}

class GD444e40
{
public:
    virtual ~GD444e40();
};

__declspec(noinline) GD444e40::~GD444e40()
{
}

class GD4451f0
{
public:
    virtual ~GD4451f0();
};

__declspec(noinline) GD4451f0::~GD4451f0()
{
}

class GD4477d0
{
public:
    virtual ~GD4477d0();
};

__declspec(noinline) GD4477d0::~GD4477d0()
{
}

class GD447e50
{
public:
    virtual ~GD447e50();
};

__declspec(noinline) GD447e50::~GD447e50()
{
}

class GD4482b0
{
public:
    virtual ~GD4482b0();
};

__declspec(noinline) GD4482b0::~GD4482b0()
{
}

class GD4484c0
{
public:
    virtual ~GD4484c0();
};

__declspec(noinline) GD4484c0::~GD4484c0()
{
}

class GD449130
{
public:
    virtual ~GD449130();
};

__declspec(noinline) GD449130::~GD449130()
{
}

class GD44a560
{
public:
    virtual ~GD44a560();
};

__declspec(noinline) GD44a560::~GD44a560()
{
}

class GD44c1b0
{
public:
    virtual ~GD44c1b0();
};

__declspec(noinline) GD44c1b0::~GD44c1b0()
{
}

class GD450e80
{
public:
    virtual ~GD450e80();
};

__declspec(noinline) GD450e80::~GD450e80()
{
}

class GD4512e0
{
public:
    virtual ~GD4512e0();
};

__declspec(noinline) GD4512e0::~GD4512e0()
{
}

class GD4516f0
{
public:
    virtual ~GD4516f0();
};

__declspec(noinline) GD4516f0::~GD4516f0()
{
}

class GD451880
{
public:
    virtual ~GD451880();
};

__declspec(noinline) GD451880::~GD451880()
{
}

class GD452a90
{
public:
    virtual ~GD452a90();
};

__declspec(noinline) GD452a90::~GD452a90()
{
}

class GD453360
{
public:
    virtual ~GD453360();
};

__declspec(noinline) GD453360::~GD453360()
{
}

class GD453c50
{
public:
    virtual ~GD453c50();
};

__declspec(noinline) GD453c50::~GD453c50()
{
}

struct G2GC4GET
{
    char Get31();
    char Get30();
};

char G2GC4GET::Get31()
{
    char *p = *(char **)((char *)this + 0xa4);
    if (p)
        return p[0x31];
    return 0;
}

char G2GC4GET::Get30()
{
    char *p = *(char **)((char *)this + 0xa4);
    if (p)
        return p[0x30];
    return 0;
}

struct G2GC4SET
{
    void SetF0(int value);
};

void G2GC4SET::SetF0(int value)
{
    *(int *)((char *)this + 0xf0) = value;
}

extern "C" void (__stdcall *g_fn656294)(void *);
extern "C" void (__stdcall *g_fn656284)(void *);

struct G2GC4CALL
{
    void CallA();
    void CallB();
};

void G2GC4CALL::CallA()
{
    g_fn656294(*(void **)this);
}

void G2GC4CALL::CallB()
{
    g_fn656284(this);
}

extern "C" void (__stdcall *g_fn656120)(void *);

struct G2GC4INIT
{
    int Init();
};

int G2GC4INIT::Init()
{
    g_fn656120(*(void **)this);
    *(char *)((char *)this + 4) = 1;
    return 0;
}

typedef void (__stdcall *G2GC4RELEASEFN)(void *);

struct G2GC4REL
{
    void Release();
};

void G2GC4REL::Release()
{
    char *p = *(char **)this;
    if (p)
        (*(G2GC4RELEASEFN *)((char *)*(void **)p + 8))(p);
}
