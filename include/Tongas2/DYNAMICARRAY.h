// A table of items that keep their slot: removed slots go on a stack of free
// ones and are reused first. Used by several units; the class name is the
// one its log messages give, the rest are placeholders.
#ifndef TONGAS2_DYNAMICARRAY_H
#define TONGAS2_DYNAMICARRAY_H

#include "Tongas2/LOG.h"

template <class T>
class DYNAMICARRAY
{
public:
    DYNAMICARRAY()
    {
        m_data = 0;
        m_free = 0;
        m_used = 0;
        m_capacity = 0;
        m_growBy = 0;
        Clear();
    }
    virtual ~DYNAMICARRAY();

    // Empties the table, keeping its room.
    void Clear();
    // Makes room for size items, packing the used ones at the start.
    void Resize(const int &size);
    // Puts a copy of item in the slot.
    T *Set(const T &item, const int &slot);
    // Puts a copy of item in a free slot, growing the table if it may.
    T *Insert(const T &item);
    // Whether i is a slot of the table.
    bool IsValid(const int &i) { return i >= 0 && i < m_capacity && m_data; }
    // The nth used item, or 0.
    T *Get(int n);

    T *m_data;
    // The slots given back, as a stack.
    int *m_free;
    bool *m_used;
    int m_capacity;
    int m_count;
    // The top of m_free, -1 when it is empty.
    int m_freeTop;
    // The first slot never used.
    int m_next;
    int m_20[4];
    // How many slots Insert adds when the table is full; 0 for none.
    int m_growBy;
};

template <class T>
DYNAMICARRAY<T>::~DYNAMICARRAY()
{
    if (m_used) {
        delete[] m_used;
        m_used = 0;
    }
    if (m_free) {
        delete[] m_free;
        m_free = 0;
    }
    if (m_data) {
        delete[] m_data;
        m_data = 0;
    }
}

template <class T>
void DYNAMICARRAY<T>::Clear()
{
    if (m_used) {
        for (int i = 0; i < m_capacity; i++) {
            m_used[i] = false;
            m_free[i] = -1;
        }
    }
    m_count = 0;
    m_freeTop = -1;
    m_next = 0;
}

template <class T>
void DYNAMICARRAY<T>::Resize(const int &size)
{
    if (size < m_capacity || size < m_count) {
        if (g_pLog)
            g_pLog->Error("Error - DYNAMICARRAY resize with new size smaller");
        return;
    }
    T *data = new T[size];
    int *free = new int[size];
    bool *used = new bool[size];
    int i;
    for (i = 0; i < size; i++) {
        used[i] = false;
        free[i] = -1;
    }
    m_next = 0;
    m_freeTop = -1;
    if (m_capacity && m_data) {
        for (i = 0; i < m_capacity; i++) {
            if (m_used[i]) {
                data[m_next] = m_data[i];
                used[m_next] = true;
                m_next++;
            }
        }
    }
    if (m_data) {
        delete[] m_data;
        m_data = 0;
    }
    if (m_free) {
        delete[] m_free;
        m_free = 0;
    }
    if (m_used) {
        delete[] m_used;
        m_used = 0;
    }
    m_used = used;
    m_free = free;
    m_data = data;
    m_capacity = size;
}

template <class T>
T *DYNAMICARRAY<T>::Set(const T &item, const int &slot)
{
    m_count++;
    m_used[slot] = true;
    m_data[slot] = item;
    return &m_data[slot];
}

template <class T>
T *DYNAMICARRAY<T>::Insert(const T &item)
{
    if (m_count >= m_capacity) {
        if (m_growBy <= 0) {
            if (g_pLog)
                g_pLog->Error("DYNAMICARRAY:Insert> Error - table full (1)");
            return 0;
        }
        int size = m_growBy + m_capacity;
        Resize(size);
    }
    if (IsValid(m_freeTop)) {
        int slot = m_free[m_freeTop];
        if (!IsValid(slot)) {
            if (g_pLog)
                g_pLog->Error("DYNAMICARRAY:Insert> Error - Invalid table pos in free stack");
            return 0;
        }
        T *result = Set(item, slot);
        m_freeTop--;
        return result;
    }
    if (IsValid(m_next)) {
        T *result = Set(item, m_next);
        m_next++;
        return result;
    }
    if (g_pLog)
        g_pLog->Error("DYNAMICARRAY:Insert> Error - table full (2)");
    return 0;
}

template <class T>
T *DYNAMICARRAY<T>::Get(int n)
{
    int found = 0;
    for (int i = 0; i < m_next + 1; i++) {
        if (IsValid(i) && m_used[i]) {
            if (found == n)
                return &m_data[i];
            found++;
        }
    }
    return 0;
}

#endif
