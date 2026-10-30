// The octree the scene can sort its actors into. Placeholder file name.
#include "Tongas2/CAMERA.h"
#include "Tongas2/LOG.h"
#include "Tongas2/PVS.h"
#include "Tongas2/SRESULT.h"

#include <string.h>

void BOXCORNERS::Set(const V3 &min, const V3 &max)
{
    corners[0].Set(min.x, min.y, max.z);
    corners[1].Set(max.x, min.y, max.z);
    corners[2].Set(min.x, min.y, min.z);
    corners[3].Set(max.x, min.y, min.z);
    corners[4].Set(min.x, max.y, max.z);
    corners[5].Set(max.x, max.y, max.z);
    corners[6].Set(min.x, max.y, min.z);
    corners[7].Set(max.x, max.y, min.z);
}

OCTREENODE::~OCTREENODE()
{
    for (int i = 0; i < 8; i++) {
        if (m_children[i]) {
            delete m_children[i];
            m_children[i] = 0;
        }
    }
}

void OCTREENODE::Update(FRUSTUM *frustum, bool test)
{
    ACTOR *actor;
    if (test) {
        for (actor = m_members; actor; actor = actor->m_next) {
            if (frustum->Classify(actor->GetBox()) != 2)
                actor->m_inView = true;
        }
    } else {
        for (actor = m_members; actor; actor = actor->m_next)
            actor->m_inView = true;
    }
    if (m_split) {
        for (int i = 0; i < 8; i++) {
            if (!test) {
                m_children[i]->Update(frustum, false);
            } else {
                int result = frustum->Classify(&m_children[i]->m_box);
                if (result == 0)
                    m_children[i]->Update(frustum, false);
                else if (result == 1)
                    m_children[i]->Update(frustum, true);
            }
        }
    }
}

void OCTREENODE::Dump(FRUSTUM *frustum, bool test, int *unused, int *visible, int *tested, int *nodes)
{
    ++*nodes;
    ACTOR *actor;
    if (test) {
        for (actor = m_members; actor; actor = actor->m_next) {
            if (frustum->Classify(actor->GetBox()) != 2)
                ++*visible;
            ++*tested;
        }
    } else {
        for (actor = m_members; actor; actor = actor->m_next)
            ++*visible;
    }
    if (m_split) {
        for (int i = 0; i < 8; i++) {
            if (!test) {
                m_children[i]->Dump(frustum, false, unused, visible, tested, nodes);
            } else {
                int result = frustum->Classify(&m_children[i]->m_box);
                if (result == 0)
                    m_children[i]->Dump(frustum, false, unused, visible, tested, nodes);
                else if (result == 1)
                    m_children[i]->Dump(frustum, true, unused, visible, tested, nodes);
            }
        }
    }
}

OCTREE::~OCTREE()
{
    if (m_root) {
        delete m_root;
        m_root = 0;
    }
}

void OCTREE::AddMember(ACTOR *actor)
{
    if (m_root)
        m_root->AddMember(actor);
    else if (g_pLog)
        g_pLog->Error("PVSOctree::AddMember - parent node is null");
}

void OCTREE::Update(FRUSTUM *frustum)
{
    if (m_root)
        m_root->Update(frustum, true);
}

void OCTREE::Dump(FRUSTUM *frustum)
{
    int unused = 0;
    int visible = 0;
    int tested = 0;
    int nodes = 0;
    if (m_root)
        m_root->Dump(frustum, true, &unused, &visible, &tested, &nodes);
}

OCTREENODE::OCTREENODE(const V3 &min, const V3 &max, int depth)
    : m_max(max), m_min(min), m_depth(depth), m_split(false)
{
    memset(m_children, 0, sizeof(m_children));
    m_box.Set(min, max);
}

void OCTREENODE::AddMember(ACTOR *actor)
{
    ZEROV3 boxMax;
    ZEROV3 boxMin;
    actor->GetBoxTop(&boxMin, &boxMax);
    V3 half = (m_max - m_min) * 0.5f;
    if (!m_split && m_depth > 0 && BoxInside(m_min, m_max, boxMin, boxMax)) {
        m_split = true;
        m_children[0] = new OCTREENODE(V3(m_min.x, m_min.y + half.y, m_min.z + half.z), V3(m_min.x + half.x, m_max.y, m_max.z), m_depth - 1);
        m_children[1] = new OCTREENODE(V3(m_min.x + half.x, m_min.y + half.y, m_min.z + half.z), V3(m_max.x, m_max.y, m_max.z), m_depth - 1);
        m_children[2] = new OCTREENODE(V3(m_min.x, m_min.y + half.y, m_min.z), V3(m_min.x + half.x, m_max.y, m_min.z + half.z), m_depth - 1);
        m_children[3] = new OCTREENODE(V3(m_min.x + half.x, m_min.y + half.y, m_min.z), V3(m_max.x, m_max.y, m_min.z + half.z), m_depth - 1);
        m_children[4] = new OCTREENODE(V3(m_min.x, m_min.y, m_min.z + half.z), V3(m_min.x + half.x, m_min.y + half.y, m_max.z), m_depth - 1);
        m_children[5] = new OCTREENODE(V3(m_min.x + half.x, m_min.y, m_min.z + half.z), V3(m_max.x, m_min.y + half.y, m_max.z), m_depth - 1);
        m_children[6] = new OCTREENODE(V3(m_min.x, m_min.y, m_min.z), V3(m_min.x + half.x, m_min.y + half.y, m_min.z + half.z), m_depth - 1);
        m_children[7] = new OCTREENODE(V3(m_min.x + half.x, m_min.y, m_min.z), V3(m_max.x, m_min.y + half.y, m_min.z + half.z), m_depth - 1);
    }
    if (m_split) {
        for (unsigned int i = 0; i < 8; i++) {
            if (BoxInside(m_children[i]->m_min, m_children[i]->m_max, boxMin, boxMax)) {
                m_children[i]->AddMember(actor);
                return;
            }
        }
    }
    PVSNODE::AddMember(actor);
}

OCTREE::OCTREE(V3 min, V3 max, int depth) : m_depth(depth), m_max(max), m_min(min)
{
    m_root = new OCTREENODE(m_min, m_max, m_depth);
}

void OCTREE::Clear()
{
    if (m_root) {
        delete m_root;
        m_root = 0;
    }
    m_root = new OCTREENODE(m_min, m_max, m_depth);
}
