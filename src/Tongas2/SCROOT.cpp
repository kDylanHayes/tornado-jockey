// Tongas2/SCROOT -- the SCRoot object and its loader.
//
// Discovery scaffold: bodies are empty until the unit is matched; each
// function has its exact target size in config/symbols.txt so a later delink
// cannot cut the target short.
//
// The unit's address range also holds the standard-library helpers the
// linker placed next to SCROOT: the vector instantiations the root's node
// lists use.  They are reproduced here by using the same templates, so the
// compiler emits the same COMDAT copies:
//
//   0x0050cdc0  std::vector<V3>::push_back(const V3 &)
//   0x0050ce40  std::vector<int>::push_back(const int &)
//   0x0050ce90  std::vector<GD50ce90>::push_back(const GD50ce90 &)
//   0x0050cf00  std::vector<std::vector<int> >::_Destroy(...)
//   0x0050cf20  std::_Uninit_copy<std::vector<int>, ...>
//   0x0050d2c0  std::_Uninit_fill_n<std::vector<int>, ...>
//   0x0050d590  std::vector<std::vector<int> >::_Ufill(...)
//   0x0050d5c0  std::vector<std::vector<int> >::_Insert_n(...)
#include "Tongas2/SCROOT.h"
#include "Tongas2/MATHS.h"

#include <string.h>
#include <vector>

typedef std::vector<int> VI;
typedef std::vector<VI> VV;

// The sixteen-byte element the root's node list holds.  The retail's
// std::vector<...>::push_back at 0x0050ce90 copies it four words at a time,
// so any sixteen-byte plain-old-data type gives the same code.
class GD50ce90
{
public:
    float x, y, z, w;
};

// 0x0050d0c0 -- one record of the root's list: two indices and two
// positions.  The list keeps the indices in two std::vector<int> and the
// positions in two std::vector<V3>.
class GD50d0c0
{
public:
    void Add(int index0, int index1, const V3 *pos0, const V3 *pos1);

private:
    std::vector<int> m_000;
    std::vector<V3> m_010;
    std::vector<V3> m_020;
    std::vector<int> m_030;
};

void GD50d0c0::Add(int index0, int index1, const V3 *pos0, const V3 *pos1)
{
    m_000.push_back(index0);
    m_010.push_back(*pos0);
    m_020.push_back(*pos1);
    m_030.push_back(index1);
}

// 0x0050cfb0 -- links a node into the root.  The root keeps the nodes in a
// std::vector<int> at +0x124 and stamps itself into the node at +0x120.
class GD50cfb0
{
public:
    void Link(int node);

private:
    char m_000[0x124];
    std::vector<int> m_124;
};

void GD50cfb0::Link(int node)
{
    m_124.push_back(node);
    *(void **)(node + 0x120) = this;
}

// The root's list of lists.  Resizing it is what makes the compiler emit
// the std::vector<std::vector<int> > helpers the linker put in this range.
class GD50cf00
{
public:
    void Resize(unsigned count);

private:
    VV m_000;
};

void GD50cf00::Resize(unsigned count)
{
    m_000.resize(count);
}

// The root's node list of sixteen-byte entries.
class GD50ce90list
{
public:
    void Add(const GD50ce90 *value);

private:
    std::vector<GD50ce90> m_000;
};

void GD50ce90list::Add(const GD50ce90 *value)
{
    m_000.push_back(*value);
}

// 0x0050d3c0 -- the settings table's own record: a weight of one, a flag and
// five index vectors.
class GD50d3c0
{
public:
    GD50d3c0(int a, int b);

private:
    float m_000;
    char m_004;
    char m_005[0x1b];
    std::vector<int> m_020;
    int m_030;
    int m_034;
    int m_038;
    std::vector<int> m_03c;
    std::vector<int> m_050;
    std::vector<int> m_060;
    std::vector<int> m_070;
    int m_080;
};

GD50d3c0::GD50d3c0(int a, int b)
    : m_000(1.0f), m_004(0), m_020(), m_030(a), m_034(b), m_038(0),
      m_03c(), m_050(), m_060(), m_070(), m_080(0)
{
}

SCROOT::SCROOT(int type, const void *data)
{
    (void)type;
    (void)data;
}

void SCROOT::Init()
{
}

void SCROOT::Load(void *data, int a, int b, int c)
{
    (void)data;
    (void)a;
    (void)b;
    (void)c;
}

SCROOT *CreateSCROOT()
{
    return 0;
}
