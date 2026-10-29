#define STRINGMANAGER_LAYOUT
#include "Tongas2/LOG.h"
#include "Tongas2/STRING.h"

#define PAGESIZE 0x100
#define NUMPAGES 0x1000

STRINGMANAGER g_StringManager;

inline bool STRINGMANAGER::CheckIndex(int page)
{
    if (page < 0 || page >= NUMPAGES) {
        if (g_pLog)
            g_pLog->Error("STRING_MANAGER::CheckIndex>Error: number page %d is out of range", page);
        return false;
    }
    return true;
}

inline char *STRINGMANAGER::GetPointer(const int &page)
{
    if (CheckIndex(page))
        return (char *)m_pages + page * PAGESIZE;
    if (g_pLog)
        g_pLog->Error("STRING_MANAGER::GetPointer>Error: Unable to return pointer to the page %d", page);
    return 0;
}

STRINGMANAGER::STRINGMANAGER()
{
    int i;
    for (i = 0; i < NUMPAGES; i++) {
        char *page = m_pages[i];
        page[0] = 0;
        page[PAGESIZE - 1] = 0;
        m_used[i] = false;
    }
    m_next = 0;
    m_usedPagesCount = 0;
}

void STRINGMANAGER::Defrag()
{
    int free = -1;
    for (int i = 0; i < NUMPAGES; i++) {
        if (m_used[i]) {
            if (free >= 0)
            while (m_used[i] && i < NUMPAGES) {
                memcpy((char *)m_pages + free * PAGESIZE, (char *)m_pages + i * PAGESIZE, PAGESIZE);
                ((char *)m_pages)[i * PAGESIZE] = 0;
                m_owners[free] = m_owners[i];
                STRING *owner = m_owners[free];
                if (owner->m_block == i) {
                    owner->m_buf = GetPointer(free);
                    owner->m_block = free;
                }
                m_used[i] = false;
                i++;
                m_used[free] = true;
                free++;
            }
        } else if (free == -1)
            free = i;
    }
    m_cache[0].m_index = -1;
    m_cache[0].m_state = CACHE::EMPTY;
    m_cache[1].m_index = -1;
    m_cache[1].m_state = CACHE::EMPTY;
}
