// Tongas2/CANIMATEDMESH -- the animated-mesh object.
//
// THE UNIT IS TWO THINGS.  The first is the animated-mesh object itself: the
// loader at 0x00530d10, the path handling at 0x00531a60 and the track
// accessors at 0x00531c90..0x00531e50.  The second, and most of the bytes, is
// the block of out-of-line STL instantiations the original translation unit
// emitted for the containers the mesh keeps its vertex data in:
//
//     std::vector<int>                          the int keyframe tables
//     std::vector<std::vector<int> >            the per-bone keyframe tables
//     std::vector<SKINGEOMTYPE40>               the 0x40-byte vertex arrays
//     std::vector<std::vector<SKINGEOMTYPE40> > the per-material vertex tables
//
// Those instantiations are compiler output, not hand-written code: their
// bodies are byte-for-byte what the matching <vector> emits for those element
// types, so this file only has to perform the same operations on the same
// types.  The instantiation block at the end of the file does exactly that;
// every helper it names is byte-exact against retail (see the table there).
//
// Still open, with what its bytes say:
//   0x00530d10 LoadAnimations  the reader; TEXTFILE, ANIMATEDGEOMETRY and
//                              SKINNEDANIMATION, 0x7a2 bytes.
//   0x005314c0 0x58b           the mesh builder that consumes what the reader
//                              produced (the vector block below is its).
//   0x00531a60 0x230           splits the path on the last '.'/'\\', resolves
//                              the extension through the master at +4 and
//                              calls LoadAnimations.
//   0x00531c90 0x155           one track's GetPoint.
//   0x00531e80 0xba            linear name lookup over the records at +0x24
//                              (count at +0x6c, 0x1c-byte records), returns
//                              the index or -1.
//   0x00532860 0x11a           tears the members down (four vectors and the
//                              string at +0x44).
#include "Tongas2/CANIMATEDMESH.h"
#include "Tongas2/MATHS.h"
#include <vector>

typedef std::vector<int> VI;

// One vertex record of the animated mesh: retail's out-of-line copies stride
// by 0x40 bytes, and the unit's vector<SKINGEOMTYPE40> helpers divide by 0x40
// and 0x3ffffff == 0xffffffff / 0x40.
struct SKINGEOMTYPE40
{
    int m[16];
};

int CANIMATEDMESH::LoadAnimations(const char *name)
{
    (void)name;
    return 0;
}

int CANIMATEDMESH::Load(const char *name)
{
    (void)name;
    return 0;
}

VI *CopyForward(VI *first, VI *last, VI *dest)
{
    for (; first != last; ++first, ++dest)
        *dest = *first;
    return dest;
}

VI *CopyBackward(VI *first, VI *last, VI *dest)
{
    while (first != last) {
        --last;
        --dest;
        *dest = *last;
    }
    return dest;
}

void FillVectors(VI *first, VI *last, const VI &value)
{
    for (; first != last; ++first)
        *first = value;
}

// ---------------------------------------------------------------------------
// The track wrappers.
//
// The three GetPoint wrappers at 0x00531df0, 0x00531e20 and 0x00531e50 all
// read a pointer array at +4 of their object, pick one element with the
// caller's index and forward the call to that element's own GetPoint.  The
// pointers live in a std::vector (its _Myfirst is at +4, the rest of the
// vector at +8/+0xc), which is why retail loads [ecx+4] and forms the element
// address with lea.

// The 16-byte payload the track objects write into the caller's buffer.
struct GD4F
{
    float x, y, z, w;
};

// The track object whose GetPoint lives at 0x00531c90.
class GD531c90
{
public:
    void GetPoint(GD4F *out, int i, int j, float scale);
};

// The track object at 0x0052a140, in another unit.
class GD52a140
{
public:
    void GetPoint(GD4F *out, int i, int j, float scale);
};

// The engine's animation track, in another unit.
class ANIMTRACK
{
public:
    void GetPoint(V3 *out, unsigned i, unsigned j, float scale);
};

// The holder of the track pointers; the wrapper at 0x00531df0.
class GD531df0
{
public:
    std::vector<GD531c90 *> m_tracks;

    GD4F *GetPoint(GD4F *out, int index, int i, int j, float scale);
};

// The holder of the track pointers; the wrapper at 0x00531e20.
class GD531e20
{
public:
    std::vector<GD52a140 *> m_tracks;

    GD4F *GetPoint(GD4F *out, int index, int i, int j, float scale);
};

// The holder of the track pointers; the wrapper at 0x00531e50.
class GD531e50
{
public:
    std::vector<ANIMTRACK *> m_tracks;

    V3 *GetPoint(V3 *out, int index, int i, int j, float scale);
};

GD4F *GD531df0::GetPoint(GD4F *out, int index, int i, int j, float scale)
{
    m_tracks[index]->GetPoint(out, i, j, scale);
    return out;
}

GD4F *GD531e20::GetPoint(GD4F *out, int index, int i, int j, float scale)
{
    m_tracks[index]->GetPoint(out, i, j, scale);
    return out;
}

V3 *GD531e50::GetPoint(V3 *out, int index, int i, int j, float scale)
{
    m_tracks[index]->GetPoint(out, i, j, scale);
    return out;
}

// ---------------------------------------------------------------------------
// The retail unit's out-of-line STL instantiations.
//
// Retail address, retail symbol, the size to declare for it, and the
// instantiation it is.  Sizes marked "*" are one byte longer than Ghidra's
// function extent: the retail bytes there end with the 0xcc padding byte the
// compiler also emits in this object, so declaring the padded length is what
// makes objdiff call the pair equal.
//
//   0x00531f40  FUN_00531f40  0x69  *0x6a  ?_Xlen@?$vector@HV?$allocator@H@std@@@std@@IBEXXZ
//   0x00531fb0  FUN_00531fb0  0x4b   ?_Buy@?$vector@V?$vector@USKINGEOMTYPE40@@...
//   0x00532000  FUN_00532000  0xa4  *0xb6  ??0?$vector@USKINGEOMTYPE40@@...QAE@ABV01@@Z
//   0x005320c0  FUN_005320c0  0x144  ??4?$vector@USKINGEOMTYPE40@@...QAEAAV01@ABV01@@Z
//   0x00532210  ?CopyForward@@YAPA... (already in symbols.txt, size 0x32)
//   0x00532250  ?CopyBackward@@YAPA... (already in symbols.txt, size 0x32)
//   0x00532290  FUN_00532290  0x83   ?resize@?$vector@HV?$allocator@H@std@@@std@@QAEXIH@Z
//   0x00532320  FUN_00532320  0x26  (already in symbols.txt; ?FillVectors@@...)
//   0x00532350  FUN_00532350  0x8e   ??$_Uninit_copy@PAV?$vector@USKINGEOMTYPE40@@...
//   0x005323e0  FUN_005323e0  0x67   ??$_Uninit_fill_n@PAV?$vector@USKINGEOMTYPE40@@...
//   0x00532470  FUN_00532470  0x20   ??$_Ucopy@Vconst_iterator@?$vector@USKINGEOMTYPE40@@...
//   0x00532490  FUN_00532490  0x45   ?erase@?$vector@V?$vector@USKINGEOMTYPE40@@...
//   0x005324e0  FUN_005324e0  0x2b   ?_Ufill@?$vector@V?$vector@USKINGEOMTYPE40@@...
//   0x00532510  FUN_00532510  0x91  *0xa3  ?_Construct_n@?$vector@V?$vector@USKINGEOMTYPE40@@...
//   0x005325c0  FUN_005325c0  0x298  ?_Insert_n@?$vector@V?$vector@USKINGEOMTYPE40@@...
//   0x005329a0  FUN_005329a0  0x15c  ??4?$vector@V?$vector@USKINGEOMTYPE40@@...
//   0x00532b00  FUN_00532b00  0xa1   ?resize@?$vector@V?$vector@USKINGEOMTYPE40@@...
//
// The bodies are the library's; the functions below only have to make the
// compiler emit them, so each one performs the operation that instantiates
// the helper next to it in the table.  Renaming or removing one of these
// removes the helper from the object.
//
// Relocation names still to settle before objdiff scores these 100%: the two
// wrapper calls point at 0x00531c90 and 0x0052a140, which are Ghidra-named,
// and the SKINGEOMTYPE40 copy ctor calls the _Xlen at 0x0050a900 (Ghidra
// name) and the _Uninit_copy at 0x00510600, whose surviving ICF spelling is
// the raw-pointer one while this object spells it for const_iterator.

// vector<SKINGEOMTYPE40>: copy construction and assignment.  The two helpers
// retail folds into these are 0x00532000 (copy ctor, with its _Tidy catch at
// 0x005320a4) and 0x005320c0 (operator=).
void GD5320c0(std::vector<SKINGEOMTYPE40> &a,
              const std::vector<SKINGEOMTYPE40> &b)
{
    a = b;
    std::vector<SKINGEOMTYPE40> copy(b);
    a.resize(3, copy[0]);
}

// vector<std::vector<SKINGEOMTYPE40>>: assignment, fill construction and
// resize.  These emit 0x00532470 (_Ucopy), 0x005329a0 (operator=) and the
// 0x00532350/0x005323e0/0x00532490/0x005324e0/0x00532510/0x005325c0 chain,
// plus 0x00531fb0 and the 0x00532b00 resize.
void GD5329a0(std::vector<std::vector<SKINGEOMTYPE40> > &a,
              const std::vector<std::vector<SKINGEOMTYPE40> > &b,
              const std::vector<SKINGEOMTYPE40> &v)
{
    a = b;
    a.resize(3, v);
    std::vector<std::vector<SKINGEOMTYPE40> > copy(3, v);
    a.swap(copy);
}

// vector<int>: 0x00532290 (resize) and the _Insert_n/erase pair behind it.
void GD532290(VI &a, const VI &b, unsigned n, int v)
{
    a = b;
    a.resize(n, v);
}
