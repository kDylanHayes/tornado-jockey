// Fixed-size tables of resources, indexed by handle and hashed by name.
// Placeholder names.
#ifndef TONGAS2_SAFEINDEX_H
#define TONGAS2_SAFEINDEX_H

#include <new>
#include <string.h>

#include "Tongas2/LOG.h"

// Defined elsewhere.
int HashString(const char *str, int size);
// _vsnprintf into a 512-byte buffer.
int SafeSprintf(char *buf, const char *format, ...) throw();
// The same for wide text, up to 0x1fe characters.
int SafeSwprintf(wchar_t *buf, const wchar_t *format, ...) throw();

// Names an item in a SAFEINDEX: the slot, the insertion serial that was
// current when it went in (so stale handles to a reused slot fail), and the
// kind of table.
struct RESHANDLE
{
    RESHANDLE() : serial(-1), index(-1), kind(0) {}
    RESHANDLE(int serial_, int index_, int kind_) : serial(serial_), index(index_), kind(kind_) {}
    bool IsValid() const { return serial >= 0 && index >= 0; }
    void Invalidate() { serial = -1; index = -1; }
    bool operator==(const RESHANDLE &other) const
    {
        return serial == other.serial && index == other.index && kind == other.kind;
    }
    int serial;
    int index;
    int kind;
};

// A stack of N items.
template <class T, int N>
class LIFO
{
public:
    LIFO()
    {
        m_data = new T[N];
        m_top = 0;
        m_count = 0;
    }
    virtual ~LIFO()
    {
        if (m_data) {
            delete[] m_data;
            m_data = 0;
        }
    }
    bool Push(const T &value)
    {
        if (!m_data)
            return false;
        if (m_top < N && m_top >= 0) {
            m_data[m_top] = value;
            m_top++;
            m_count++;
            return true;
        }
        if (g_pLog)
            g_pLog->Error("LIFO::Push> Error - Table full");
        return false;
    }

    T *m_data;
    int m_top;
    int m_count;
};

template <class T, int N, int H, int KIND>
class SAFEINDEX
{
public:
    SAFEINDEX()
    {
        m_items = new T *[N];
        m_freeList = new int[N];
        m_next = new int[N];
        m_prev = new int[N];
        m_buckets = new int[H];
        m_bucketOf = new int[N];
        m_bucketTail = new int[H];
        m_nextFree = 0;
        m_numFree = 0;
        m_serial = 0;
        m_count = 0;
        int i;
        for (i = 0; i < H; i++)
            m_buckets[i] = -1;
        for (i = 0; i < N; i++) {
            m_next[i] = -1;
            m_prev[i] = -1;
            m_items[i] = 0;
        }
    }
    virtual ~SAFEINDEX()
    {
        Clear();
        if (m_items) { delete[] m_items; m_items = 0; }
        if (m_freeList) { delete[] m_freeList; m_freeList = 0; }
        if (m_next) { delete[] m_next; m_next = 0; }
        if (m_prev) { delete[] m_prev; m_prev = 0; }
        if (m_buckets) { delete[] m_buckets; m_buckets = 0; }
        if (m_bucketOf) { delete[] m_bucketOf; m_bucketOf = 0; }
        if (m_bucketTail) { delete[] m_bucketTail; m_bucketTail = 0; }
    }

    void Clear()
    {
        m_nextFree = 0;
        m_numFree = 0;
        m_serial = 0;
        int i;
        for (i = 0; i < H; i++)
            m_buckets[i] = -1;
        for (i = 0; i < N; i++) {
            m_next[i] = -1;
            m_prev[i] = -1;
            if (m_items[i]) {
                delete m_items[i];
                m_items[i] = 0;
            }
        }
        m_count = 0;
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

    void FreePos(int pos, bool destroy)
    {
        if (!IsValidPos(pos)) {
            if (g_pLog)
                g_pLog->Error("SAFEINDEX::FreePos> Error - position not valid");
            return;
        }
        if (m_numFree < N && m_numFree >= 0) {
            m_freeList[m_numFree] = pos;
            m_numFree++;
            if (destroy) {
                if (m_items[pos]) {
                    delete m_items[pos];
                    m_items[pos] = 0;
                }
            } else {
                RESHANDLE &h = m_items[pos]->m_handle;
                h.serial = -1;
                h.index = -1;
                m_items[pos] = 0;
            }
        } else if (g_pLog)
            g_pLog->Error("SAFEINDEX:FreePos> Invalid delete item list");
        m_count--;
    }

    void HashInsert(int bucket, int pos, T *item)
    {
        if (m_buckets[bucket] == -1) {
            m_buckets[bucket] = pos;
            m_bucketOf[pos] = bucket;
            m_next[pos] = -1;
            m_prev[pos] = -1;
            m_bucketTail[bucket] = pos;
        } else {
            m_bucketOf[pos] = bucket;
            m_next[m_bucketTail[bucket]] = pos;
            m_next[pos] = -1;
            m_prev[pos] = m_bucketTail[bucket];
            m_bucketTail[bucket] = pos;
        }
    }

    void HashRemove(int pos)
    {
        if (m_buckets[m_bucketOf[pos]] == pos)
            m_buckets[m_bucketOf[pos]] = m_next[pos];
        if (m_next[pos] != -1)
            m_prev[m_next[pos]] = m_prev[pos];
        if (m_prev[pos] != -1)
            m_next[m_prev[pos]] = m_next[pos];
        if (m_bucketTail[m_bucketOf[pos]] == pos)
            m_bucketTail[m_bucketOf[pos]] = m_prev[pos];
    }

    bool IsValidHash(int hash)
    {
        if (hash >= 0 && hash < H)
            return true;
        return false;
    }
    bool IsValidPos(int pos) { return pos >= 0 && pos < N; }

    T *Find(const char *name)
    {
        int hash = HashString(name, H);
        if (!IsValidHash(hash))
            return 0;
        int i = m_buckets[hash];
        static int s_steps;
        s_steps = 0;
        while (i != -1) {
            s_steps++;
            if (s_steps > N && g_pLog)
                g_pLog->Error("SAFEINDEX:: ALERT!!!! - MAX WHILE STEPS!!! - SAFEINDEX!! searching: %s   -->   %d", name, i);
            if (!_strnicmp(name, m_items[i]->m_path.c_str(), 0x1ff))
                return m_items[i];
            i = m_next[i];
        }
        return 0;
    }

    // The first item and its slot, or 0 and -1.
    T *GetFirst(int *index)
    {
        for (int i = 0; i < N; i++) {
            if (m_items[i]) {
                *index = i;
                return m_items[i];
            }
        }
        *index = -1;
        return 0;
    }
    // The item after the one in slot *index, and its slot, or 0 and -1.
    T *GetNext(int *index)
    {
        for (int i = *index + 1; i < N; i++) {
            if (m_items[i]) {
                *index = i;
                return m_items[i];
            }
        }
        *index = -1;
        return 0;
    }

    T *Get(const RESHANDLE &h)
    {
        int i = h.index;
        if (i < 0 || i >= N)
            return 0;
        T *item = m_items[i];
        if (!item || h.serial != item->m_handle.serial || i != item->m_handle.index || h.kind != item->m_handle.kind)
            return 0;
        return item;
    }

    void Remove(const RESHANDLE &h)
    {
        if (Get(h)) {
            int i = h.index;
            if (IsValidPos(i)) {
                FreePos(i, true);
                if (m_buckets[m_bucketOf[i]] == i)
                    m_buckets[m_bucketOf[i]] = m_next[i];
                if (m_next[i] != -1)
                    m_prev[m_next[i]] = m_prev[i];
                if (m_prev[i] != -1)
                    m_next[m_prev[i]] = m_next[i];
                if (m_bucketTail[m_bucketOf[i]] == i)
                    m_bucketTail[m_bucketOf[i]] = m_prev[i];
            }
        }
    }

    // Takes an item out of the table without deleting it.
    void Release(const RESHANDLE &h)
    {
        int i = h.index;
        HashRemove(i);
        FreePos(i, false);
    }

    void DirectChangeName(const RESHANDLE &h, const char *name)
    {
        T *item = Get(h);
        if (item) {
            item->m_path = name;
            int i = h.index;
            if (IsValidPos(i))
                HashRemove(i);
            int hash = HashString(item->m_path.c_str(), H);
            if (!IsValidHash(hash) && g_pLog)
                g_pLog->Error("SAFEINDEX::DirectChangeName> ERROR Invalid dispersion for %s", item->m_path.c_str());
            HashInsert(hash, i, item);
        }
    }

    // What to do when an item with the same name is already indexed.
    // Returns whether the new item should be inserted.
    bool New(T **item, int mode, int *hash, RESHANDLE *out)
    {
        if (mode) {
            char name[512];
            name[0] = '\0';
            name[511] = '\0';
            T *old = Find((*item)->m_path.c_str());
            if (old) {
                switch (mode) {
                case 1:
                    SafeSprintf(name, "%s%d", old->m_path.c_str(), old->m_handle.serial);
                    DirectChangeName(old->m_handle, name);
                    break;
                case 2:
                    SafeSprintf(name, "%s%d", old->m_path.c_str(), m_serial);
                    (*item)->m_path = name;
                    *hash = HashString((*item)->m_path.c_str(), H);
                    if (!IsValidHash(*hash)) {
                        if (g_pLog)
                            g_pLog->Error("SAFEINDEX::New> ERROR Ivalid dispersion for %s", (*item)->m_path.c_str());
                        *out = RESHANDLE();
                        return false;
                    }
                    break;
                case 3:
                    (*item)->m_handle = old->m_handle;
                    delete old;
                    m_items[(*item)->m_handle.index] = *item;
                    *out = (*item)->m_handle;
                    m_count--;
                    return false;
                case 4:
                    if (*item) {
                        delete *item;
                        *item = 0;
                    }
                    *out = old->m_handle;
                    return false;
                case 5:
                    SafeSprintf(name, "%s%d", old->m_path.c_str(), old->m_handle.serial);
                    DirectChangeName(old->m_handle, name);
                    m_handles.Push(old->m_handle);
                    return true;
                case 6:
                    m_pending.Push(*item);
                    *out = old->m_handle;
                    return false;
                case 7:
                    *out = old->m_handle;
                    return false;
                }
            }
        }
        return true;
    }

    RESHANDLE Insert(T **item, int mode)
    {
        int hash = HashString((*item)->m_path.c_str(), H);
        if (!IsValidHash(hash)) {
            if (g_pLog)
                g_pLog->Error("SAFEINDEX::New> ERROR Invalid dispersion for %s", (*item)->m_path.c_str());
            return RESHANDLE();
        }
        RESHANDLE handle;
        if (!New(item, mode, &hash, &handle))
            return handle;
        int pos = GetFreePos();
        if (pos == -1) {
            if (g_pLog)
                g_pLog->Error("SAFEINDEX::Insert> ERROR!!! Table is full");
            return RESHANDLE();
        }
        m_items[pos] = *item;
        m_items[pos]->m_handle = RESHANDLE(m_serial, pos, KIND);
        m_serial++;
        m_count++;
        HashInsert(hash, pos, *item);
        return m_items[pos]->m_handle;
    }

    void ChangeName(RESHANDLE &h, const char *name, int mode)
    {
        int i = h.index;
        T *item = Get(h);
        if (item) {
            item->m_path = name;
            HashRemove(i);
            int hash = HashString(item->m_path.c_str(), H);
            if (!IsValidHash(hash) && g_pLog)
                g_pLog->Error("SAFEINDEX::DirectChangeName> ERROR Invalid dispersion for %s", item->m_path.c_str());
            if (New(&item, mode, &hash, &h))
                HashInsert(hash, i, item);
        }
    }

    T **m_items;
    int m_nextFree;
    int *m_freeList;
    int m_numFree;
    int *m_next;
    int *m_prev;
    int *m_buckets;
    int *m_bucketOf;
    int *m_bucketTail;
    int m_serial;
    int m_count;
    LIFO<RESHANDLE, N> m_handles;
    LIFO<T *, N> m_pending;
};

#endif
