// Tongas2/SETTINGS -- the settings/config object at 0x004fa5b0.
//
// PopFront and ClearAll manage the queue whose array is at 0x4; Destroy is a
// tail call to ClearAll. The constructor creates the settings tables and the
// queue; the config.dat parser is still a scaffold.
#include "Tongas2/SETTINGS.h"

#include <new>
#include <windows.h>

extern "C" char g_settingsEnum[];

class SETTINGSTABLE
{
public:
    void Init();
    void ClearAll();
    void ResetKeys();
    void Reset();
};

SETTINGS::SETTINGS(void *owner, std::string file, std::string defaultFile)
    : m_ff0(owner), m_ff4(false), m_ff5(false), m_ff8(file), m_1014(defaultFile)
{

    m_1030 = new int[0x2f];
    m_1034 = new float[0x2f];
    m_1038 = new std::string[2];

    int i = 0;
    unsigned int offset = 0;
    while (offset < 0x69c) {
        if (*(int *)(g_settingsEnum + offset) != i) {
            MessageBoxA(0, "Settings enumeration is out of order!", "SETUP ERROR", 0x10);
            goto done;
        }
        offset += 0x24;
        i++;
    }
    ((SETTINGSTABLE *)this)->Init();
done:
    ((SETTINGSTABLE *)this)->ClearAll();
    ((SETTINGSTABLE *)this)->ResetKeys();
    ((SETTINGSTABLE *)this)->Reset();
}

void SETTINGS::PopFront()
{
    if (m_010 != 0) {
        std::string *entry = (std::string *)m_004[m_00c];
        entry->~basic_string();
        m_00c++;
        if (m_008 <= m_00c)
            m_00c = 0;
        m_010--;
        if (m_010 == 0)
            m_00c = 0;
    }
}

void SETTINGS::ClearAll()
{
    while (m_010 != 0) {
        if (m_010 != 0) {
            unsigned int index = m_010 - 1 + m_00c;
            if (m_008 <= index)
                index -= m_008;
            std::string *entry = (std::string *)m_004[index];
            entry->~basic_string();
            m_010--;
            if (m_010 == 0)
                m_00c = 0;
        }
    }

    for (unsigned int i = m_008; i > 0;) {
        i--;
        void *entry = m_004[i];
        if (entry != 0)
            ::operator delete(entry);
    }
    if (m_004 != 0)
        ::operator delete(m_004);
    m_008 = 0;
    m_004 = 0;
}

void SETTINGS::Destroy()
{
    ClearAll();
}

void SETTINGS::Load(const char *name)
{
    (void)name;
}
