// Fixed-size tables of items, indexed by handle: SAFEINDEX without the names.
// Placeholder names.
#ifndef TONGAS2_SAFETABLE_H
#define TONGAS2_SAFETABLE_H

#include "Tongas2/SAFEINDEX.h"

template <class T, int N, int KIND>
class SAFETABLE
{
public:
    SAFETABLE()
    {
        m_items = new T *[N];
        m_handles = new RESHANDLE[N];
        m_freeList = new int[N];
        m_nextFree = 0;
        m_numFree = 0;
        m_serial = 0;
        m_count = 0;
    }
    virtual ~SAFETABLE()
    {
        if (m_items) { delete[] m_items; m_items = 0; }
        if (m_handles) { delete[] m_handles; m_handles = 0; }
        if (m_freeList) { delete[] m_freeList; m_freeList = 0; }
    }

    bool IsValidPos(int pos) { return pos >= 0 && pos < N; }

    // Forgets the items, without deleting them.
    void Clear()
    {
        m_nextFree = 0;
        m_numFree = 0;
        m_serial = 0;
        for (int i = 0; i < N; i++)
            m_handles[i].Invalidate();
    }

    int GetFreePos()
    {
        if (m_numFree > 0) {
            m_numFree--;
            if (m_numFree < 0)
                m_numFree = 0;
            return m_freeList[m_numFree];
        }
        if (m_nextFree < N) {
            m_nextFree++;
            if (m_nextFree <= 0)
                m_nextFree = 0;
            return m_nextFree - 1;
        }
        return -1;
    }

    void FreePos(int pos)
    {
        if (!IsValidPos(pos)) {
            if (g_pLog)
                g_pLog->Error("SAFETABLE::FreePos> Error - position not valid");
            return;
        }
        if (m_numFree < N && m_numFree >= 0) {
            m_freeList[m_numFree] = pos;
            m_numFree++;
            m_handles[pos].Invalidate();
        } else if (g_pLog)
            g_pLog->Error("SAFETABLE:FreePos> Invalid delete item list");
    }

    RESHANDLE Insert(T *const &item)
    {
        int pos = GetFreePos();
        if (pos == -1) {
            if (g_pLog)
                g_pLog->Error("SAFETABLE::Insert> ERROR!!! Table is full");
            return RESHANDLE();
        }
        m_items[pos] = item;
        m_handles[pos] = RESHANDLE(m_serial, pos, KIND);
        m_serial++;
        return m_handles[pos];
    }

    // The slot of the item h names, or 0.
    T **Get(const RESHANDLE &h)
    {
        int i = h.index;
        if (i >= 0 && i < N && m_handles[i] == h)
            return &m_items[i];
        return 0;
    }

    // The first slot in use and its index, or 0 and -1.
    T **GetFirst(int *index)
    {
        for (int i = 0; i < N; i++) {
            if (m_handles[i].IsValid()) {
                *index = i;
                return &m_items[i];
            }
        }
        *index = -1;
        return 0;
    }
    // The slot in use after *index, and its index, or 0 and -1.
    T **GetNext(int *index)
    {
        for (int i = *index + 1; i < N; i++) {
            if (m_handles[i].IsValid()) {
                *index = i;
                return &m_items[i];
            }
        }
        *index = -1;
        return 0;
    }

    T **m_items;
    RESHANDLE *m_handles;
    int m_nextFree;
    int *m_freeList;
    int m_numFree;
    int m_serial;
    int m_count;
};

#endif
