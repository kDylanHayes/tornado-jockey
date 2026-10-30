// An intrusive list: items derive from LISTNODE and are linked through it.
// Placeholder names.
#ifndef TONGAS2_LIST_H
#define TONGAS2_LIST_H

// A link of an intrusive list.
struct LISTNODE
{
    LISTNODE() : next(0), prev(0) {}
    // Puts node after the last of this one's.
    void Append(LISTNODE *node)
    {
        if (next) {
            next->Append(node);
        } else {
            next = node;
            node->prev = this;
        }
    }

    LISTNODE *next;
    LISTNODE *prev;
};

// The items that are in it, linked through their LISTNODE base.
template <class T>
class LIST
{
public:
    LIST() : m_head(0) {}
    virtual ~LIST();

    // The item after node's, or 0.
    static T *Next(LISTNODE *node) { return static_cast<T *>(node->next); }
    // Puts item at the front.
    void Add(T *item);
    void Remove(T *item);
    // Deletes the items.
    void Clear() throw();

    T *m_head;
};

template <class T>
LIST<T>::~LIST()
{
    T *item = m_head;
    while (item) {
        T *next = static_cast<T *>(item->next);
        Remove(item);
        delete item;
        item = next;
    }
}

template <class T>
void LIST<T>::Add(T *item)
{
    if (m_head)
        item->Append(m_head);
    m_head = item;
}

template <class T>
void LIST<T>::Remove(T *item)
{
    if (m_head == item)
        m_head = static_cast<T *>(item->next);
    if (item->prev) {
        item->prev->next = item->next;
        if (item->next) {
            item->next->prev = item->prev;
            item->next = 0;
        }
        item->prev = 0;
    } else if (item->next) {
        item->next->prev = 0;
        item->next = 0;
    }
}

template <class T>
void LIST<T>::Clear() throw()
{
    T *item = m_head;
    while (item) {
        T *next = static_cast<T *>(item->next);
        Remove(item);
        delete item;
        item = next;
    }
    m_head = 0;
}

#endif
