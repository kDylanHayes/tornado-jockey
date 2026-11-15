#include <deque>
#include <vector>
#include <string>
#include <string.h>
#include <stdexcept>

// Tongas2/ANIMATEDGEOMETRY -- the animated-geometry loader and its track queue.
//
// Verified byte-exact here (compared masked against the delinked target):
//   0x0052f8c0  ??0G2TFGET@@QAE@HHHMMD@Z        0x54
//   0x0052f920  ?Update@G2TFGET@@QAEXM@Z        0xd2
//   0x0052ad70  ?_Xlen@?$vector@HV...           0x69 (+1 int3 the compiler emits)
//   0x00530120  ?_Xlen@?$deque@PAUG2TFGET...    0x69 (+1 int3 the compiler emits)
//   0x0052faa0  ??$copy_backward@PAHPAH@std@@   0x22
// The symbol names/sizes config/symbols.txt carries for the first, second and
// last of these still describe the pre-verification guesses; see the notes at
// each definition.
#include "Tongas2/ANIMATEDGEOMETRY.h"

int ANIMATEDGEOMETRY::Load(void *file, int flags, void *owner)
{
    (void)file;
    (void)flags;
    (void)owner;
    return 0;
}

int ANIMATEDGEOMETRY::GetBoneCount()
{
    return 0;
}

int ANIMATEDGEOMETRY::GetFrameCount()
{
    return 0;
}

int ANIMATEDGEOMETRY::GetVertexCount()
{
    return 0;
}

struct G2AGTGET
{
    void * Get10();
    void * Get4();
};

void * G2AGTGET::Get10()
{
    return (char *)*(void **)((char *)this + 0xc) + 0x10;
}

void * G2AGTGET::Get4()
{
    return (char *)*(void **)((char *)this + 0xc) + 4;
}

// ---------------------------------------------------------------------------
// The geometry-track object: 0x0052f8c0 is its six-argument constructor
// (verified byte-exact) and 0x0052f920 is Update(float), the keyframe stepper.
// The constructor writes +0x00..+0x20, the byte flags +0x28..+0x2d and the
// three floats +0x30/+0x34/+0x38; +0x24 is never touched.  TRINNER::M3 builds
// one of these and TRINNER::M4 scans the +0x2a flag.
// ---------------------------------------------------------------------------
struct G2TFGET
{
    // retail 0x0052f8c0, 84 bytes: six arguments, returns this in eax.
    G2TFGET(int a, int b, int c, float d, float e, char f);

    void *Get5c(int index);
    // retail 0x0052cd50: five arguments, returns int.
    int Make5(int a, int b, void *p, int c, int d);
    // retail 0x0052aa60: three arguments.
    void Make3(int a, int b, void *p);
    // retail 0x0052f920: steps the playing keyframe by dt seconds.
    void Update(float dt);

    // The ctor at 0x0052f8c0 writes every one of these; the two byte flags at
    // +0x29/+0x2a are what TRINNER::M4 scans for.
    int m_000;
    int m_004;
    int m_008;
    int m_00c;
    int m_010;
    float m_014;
    float m_018;
    float m_01c;
    float m_020;
    int m_024;
    char m_028;
    char m_029;
    char m_02a;
    char m_02b;
    char m_02c;
    char m_02d;
    float m_030;
    float m_034;
    float m_038;
};

G2TFGET::G2TFGET(int a, int b, int c, float d, float e, char f)
{
    m_000 = a;
    m_004 = b;
    m_008 = c;
    m_00c = b;
    m_010 = b;
    m_014 = d;
    m_01c = e;
    m_018 = 0.0f;
    m_020 = 0.0f;
    m_028 = f;
    m_029 = 0;
    m_02a = 0;
    m_02b = 0;
    m_02c = 0;
    m_02d = 0;
    m_030 = 0.0f;
    m_034 = 0.0f;
    m_038 = 0.0f;
}

// retail 0x0052f920, 210 bytes: advances the playing keyframe by dt seconds.
void G2TFGET::Update(float dt)
{
    if (!m_029)
        return;
    m_02d = 0;
    if (m_030 > 0.0f) {
        m_030 -= dt;
        m_038 = m_030 / m_034;
        if (m_030 <= 0.0f) {
            m_02c = 1;
            m_038 = 0.0f;
            m_030 = 0.0f;
            return;
        }
    }
    m_020 += dt;
    if (m_008 == m_004) {
        m_020 = 0.0f;
        m_00c = m_004;
        m_010 = m_004;
    } else {
        while (m_020 >= m_01c) {
            m_00c = m_010;
            m_010++;
            m_020 -= m_01c;
            if (m_010 >= m_008) {
                if (!m_028) {
                    m_010 = m_008;
                    m_02b = 1;
                    m_029 = 0;
                    return;
                }
                m_010 = m_004;
                m_02d = 1;
            }
        }
    }
    m_018 = m_014 * m_020;
}

// The track object TRINNER::M3 reads at 0x00530830: a geometry element with
// three ints at +0x3c/+0x40/+0x44 and a duration at +0x48.
struct G2TFTRACK
{
    char m_000[0x3c];
    int m_3c;
    int m_40;
    int m_44;
    float m_48;
};

class TRINNER
{
public:
    int M10(int a, int b);
    int M11(int a, int b, int c, int d);
    // retail 0x0052fff0: deletes every queued element and clears the queue.
    void Clear();
    // retail 0x0052fb90: drops every queued element whose flag at +0x2a is set.
    void M4();
    // retail 0x00530830: builds one queued element out of the track at index.
    void M3(int index, int a2, float speed, float d);

    char m_000[0xc];
    G2TFGET *m_p;
    int m_010;
    G2TFTRACK **m_pTrack;
    G2TFTRACK **m_pTrackEnd;
    int m_01c;
    std::deque<G2TFGET *> m_queue;
};

int TRINNER::M10(int a, int b)
{
    m_p->Make3(a, b, &m_queue);
    return a;
}

int TRINNER::M11(int a, int b, int c, int d)
{
    return m_p->Make5(a, b, &m_queue, c, d);
}

// Residue: retail is 183 bytes, this compiles to 179.  The four-byte
// difference is the `if (p != 0)` guard MSVC keeps in front of the operator
// delete call at retail +0x67..+0x6a (`85 c9 74 09 51`); here the compiler
// folds it because it already knows m_queue[i] is non-null.  The register
// pick at +0x3f (`pop ebx` here, `pop edi` at retail) follows from that.
void TRINNER::Clear()
{
    for (int i = 0; i < m_queue.size(); i++) {
        if (m_queue[i]) {
            delete m_queue[i];
            m_queue[i] = 0;
        }
    }
    m_queue.clear();
}

// retail 0x0052fb90, 386 bytes (the configured size 0x17f cuts the last
// `add esp,0x14; ret`).
//
// Residue: 382 of 386 bytes.  The only difference is the 4-byte
// `test ecx,ecx; je +9` guard MSVC keeps in front of the operator delete call
// at retail +0xa4 (`85 c9 74 09 51`); here the compiler folds it because it
// already knows m_queue[i] is non-null — the same residue Clear() below carries.
// Everything else, including the shift loop, pop_back() and the epilogue,
// matches; the three earlier jump displacements differ only by those 4 bytes.
void TRINNER::M4()
{
    for (int i = 0; i < (int)m_queue.size(); i++) {
        G2TFGET *p = m_queue[i];
        if (p->m_02a) {
            if (m_queue[i]) {
                delete m_queue[i];
                m_queue[i] = 0;
            }
            for (int j = i; j < (int)m_queue.size() - 1; j++)
                m_queue[j] = m_queue[j + 1];
            m_queue.pop_back();
            i--;
        }
    }
}

// retail 0x00530830, 215 bytes.
//
// Residue: 206 of 215 bytes with the same instruction and call structure.  The
// tail of retail keeps the clamped speed on the x87 stack (`fld [speed]` twice,
// `fstp st(0)` fall-through) and holds the track duration in a stack slot loaded
// with `mov edx,[ecx+0x48]`; here MSVC reloads the value into a diamond instead
// (`fld eps; jmp`) and keeps `this` in a frame slot instead of edi.  Register
// colour and x87 scheduling only, so it is banked at this source shape.
void TRINNER::M3(int index, int a2, float speed, float d)
{
    if (m_pTrack != 0 && (unsigned)index < (unsigned)(m_pTrackEnd - m_pTrack) && index >= 0) {
        float f = 1.0f / d;
        G2TFTRACK *t = m_pTrack[index];
        int v3c = t->m_3c;
        int v40 = t->m_40;
        float v48 = t->m_48;
        float g = f / t->m_48;
        G2TFGET *o = new G2TFGET(index, v3c, v40, v48 * d, g, (char)a2);
        o->m_029 = 0;
        o->m_02a = 1;
        float rate = speed <= 0.0f ? 0.0001f : speed;
        o->m_030 = rate;
        o->m_034 = rate;
        o->m_038 = 1.0f;
        m_queue.push_front(o);
    }
}

struct G2AGTTHUNK
{
    void *Get(int index);
};

void *G2AGTTHUNK::Get(int index)
{
    return ((G2TFGET *)*(void **)((char *)this + 0xc))->Get5c(index);
}

class G2AGTDEQ : public std::deque<unsigned short>
{
public:
    void Tidy();
};

void G2AGTDEQ::Tidy()
{
    _Tidy();
}

// ---------------------------------------------------------------------------
// retail 0x0052ff00 / 0x0052ff20 / 0x005300d0 -- the element table of an
// animated geometry: a vector of element pointers that starts at +0x10 of
// the owning object.  Each element carries its name string at +0x20, a count
// at +0x44 and a divisor at +0x48.
// ---------------------------------------------------------------------------
struct GD52ff00Elem
{
    char m_000[0x20];
    std::string m_name;
    char m_03c[8];
    int m_44;
    float m_48;
};

class GD52ff00
{
public:
    void *Get(int index);
    int Find(const std::string &name);
    float Calc(const std::string &name);
    int Lookup(const std::string &name);
    // retail 0x0052fad0, in this unit's tail.
    int M2(int index);

    char m_000[0x10];
    std::vector<void *> m_v;
};

void *GD52ff00::Get(int index)
{
    return (char *)m_v[index] + 0x20;
}

int GD52ff00::Find(const std::string &name)
{
    for (int i = 0; i < m_v.size(); i++) {
        GD52ff00Elem *e = (GD52ff00Elem *)m_v[i];
        if (e->m_name.compare(name) == 0)
            return i;
    }
    return -1;
}

float GD52ff00::Calc(const std::string &name)
{
    int i = Find(name);
    if (i >= m_v.size() || i < 0)
        return 0.0f;
    GD52ff00Elem *e = (GD52ff00Elem *)m_v[i];
    return (float)(unsigned int)e->m_44 / e->m_48;
}

int GD52ff00::Lookup(const std::string &name)
{
    return M2(Find(name));
}

// ---------------------------------------------------------------------------
// retail 0x005309f0 -- std::vector<int>::push_back, inlined into a member of
// a class whose vector starts at offset 0.
// ---------------------------------------------------------------------------
class GD5309f0
{
public:
    void Push(const int &value);

    std::vector<int> m_v;
};

void GD5309f0::Push(const int &value)
{
    m_v.push_back(value);
}

// ---------------------------------------------------------------------------
// retail 0x0052faa0 -- block move helper: copies the whole 4-byte groups of
// [first, last) to the tail of the destination block.
//
// Verified: the body is the 34 bytes at 0x0052faa0 and is byte-identical to
// ??$copy_backward@PAHPAH@std@@YAPAHPAH00@Z as the vector<int> code above
// instantiates it (single relocation, _memmove at +0x1a).  config/symbols.txt
// still takes 0x30 bytes for that auto symbol, which runs past the `ret` into
// the next function's damaged prologue -- it needs 0x0052faa0 ... 0x22.
// ---------------------------------------------------------------------------
void GD52faa0(void *first, void *last, void *end)
{
    int count = (int)((char *)last - (char *)first);
    int words = count >> 2;
    memmove((char *)end - words * 4, first, words * 4);
}

// ---------------------------------------------------------------------------
// retail 0x00530cb0 -- appends a value to the second vector of the object.
// The helper at retail 0x0044a2a0 lives in another unit.
// ---------------------------------------------------------------------------
class GD530cb0
{
public:
    int m_40(int value);
    void Add(int value, int extra);

    std::vector<int> m_v0;
    std::vector<int> m_v1;
};

void GD530cb0::Add(int value, int extra)
{
    m_40(value);
    m_v1.push_back(extra);
}

// ---------------------------------------------------------------------------
// retail 0x00530120 / 0x00530190 -- the length_error throws of the standard
// deque and vector.  They are emitted by the deque<T *> instantiation above
// (through _Growmap) and by vector<int>'s _Insert_n, so no body is needed.
//
// Both retail functions are 105 bytes of code ending in the
// __CxxThrowException call (0x0052ad70..0x0052add8 and 0x00530120..0x00530188)
// and MSVC 13.10.3077 emits one trailing int3 in the same COMDAT, so our
// sections are 0x6a.  config/symbols.txt records 0x69 for both, which trims
// that byte off the target; they need size 0x6a to compare equal.  The same
// size is already used for this symbol in Tongas2/ACTOR, PATHREPOSITORY and
// GUIGROUPCONTROL.
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// retail 0x00530540 -- std::deque<T *>::push_front, emitted standalone because
// the unit pushes into the queue from several places.
// ---------------------------------------------------------------------------
template void std::deque<G2TFGET *>::push_front(G2TFGET *const &);

// ---------------------------------------------------------------------------
// config/symbols.txt changes this unit still needs (all measured against
// orig/Tornado.exe, image base 0x400000):
//
//   0x0052f8c0  ?GetBoneCount@ANIMATEDGEOMETRY@@QAEHXZ  0x54
//        ->       ??0G2TFGET@@QAE@HHHMMD@Z               0x54
//        The function is a six-argument constructor (`ret 0x18`) that fills the
//        object TRINNER::M4 walks; its bytes are already exact.
//
//   0x0052f920  ?GetFrameCount@ANIMATEDGEOMETRY@@QAEHXZ  0xd2
//        ->       ?Update@G2TFGET@@QAEXM@Z               0xd2
//        void Update(float dt): the keyframe stepper, one float argument and
//        no return value; its bytes are already exact.
//
//   0x0052ad70  ?_Xlen@?$vector@HV?$allocator@H@std@@@std@@IBEXXZ  0x69 -> 0x6a
//   0x00530120  ?_Xlen@?$deque@PAUG2TFGET@@...@std@@IBEXXZ         0x69 -> 0x6a
//        (the trailing int3 the compiler emits, see the note above)
//
//   0x0052faa0  auto symbol ??$copy_backward@PAHPAH@std@@YAPAHPAH00@Z 0x30 -> 0x22
//        The current extent swallows the first 14 bytes of the next function.
//
//   0x00530830  ?M3@TRINNER@@QAEHHHHM@Z  0xd7
//        ->       ?M3@TRINNER@@QAEXHHMM@Z 0xd7
//        void M3(int index, int a2, float speed, float d): the third argument
//        is read with `fld`, and no path ever sets eax, so the configured
//        (int,int,int,float)->int signature is a guess.  The body is still 9
//        bytes short of retail (register colour / x87 scheduling).
//
//   0x0052fb90  ?M4@TRINNER@@QAEXXZ  0x17f -> 0x182
//        The configured extent cuts the closing `add esp,0x14; ret`.
//
//   0x0052fad0  ?M2@GD52ff00@@QAEHH@Z  0xba
//        Not reconstructible from this image.  copy_backward ends at
//        0x0052fac1, five int3s pad to 0x0052fac6, and the next function's 19
//        bytes at 0x0052fac7..0x0052fad9 are not code in orig/Tornado.exe
//        (`89 61 9b 9b b0 5b 28 96 b2 b0 de 27 62 aa 6e b9 f6 85 ed`; 0x62
//        BOUND is not an MSVC 7.1 instruction).  The body from 0x0052fada
//        (`push edi; jbe ...; add ecx,0x20; ...`) is intact and the function
//        really spans 0x0052fac7..0x0052fb87, but its prologue cannot be read,
//        and the configured start 0x0052fad0 is 9 bytes inside it.
// ---------------------------------------------------------------------------
