// Tongas2/INTRO -- the intro screen object at 0x004f9280.
//
// Discovery scaffold: Load is still a stub. The small functions at
// 0x004f97d0-0x004f9c20 are the settings table the intro object also owns;
// they are written here first because their layout is fully recovered.
#include "Tongas2/INTRO.h"

#include <string.h>

// The packed setting table: 47 records of 0x24 bytes whose first word is the
// value, and three float settings after it.
extern "C" char g_settingRecords[];
extern "C" float g_settingFloat0;
extern "C" float g_settingFloat1;
extern "C" float g_settingFloat2;

class SETTINGS
{
public:
    void Init();
    int GetInt(int index);
    __declspec(noinline) void SetInt(int index, int value);
    float GetFloat(int index);
    void SetFloat(int index, float value);
    bool Changed();
    __declspec(noinline) void Clear();
    void Reset();
    void RemovePair(int first, int second);
    void AddPair(unsigned int index, int value);
    void ReplacePair(unsigned int index, int first, int second);
    void ClearAll();
    void ResetKeys();
    __declspec(noinline) int GetDetailLevel();
    void SetQuality();
    void UpdateQuality();

private:
    int m_000[0xff];
    int m_3fc[0xff];
    int m_7f8[0xff];
    int m_bf4[0xff];
    void *m_ff0;
    bool m_ff4;
    char m_ff5[0x1030 - 0xff5];
    int *m_1030;
    float *m_1034;
};

extern int HardwareA();
extern int HardwareB();
extern int HardwareC();

void SETTINGS::SetQuality()
{
    switch (GetDetailLevel()) {
    case 0:
    case 1:
        SetInt(0, 0x280);
        SetInt(1, 0x1e0);
        SetInt(9, 0);
        SetInt(0x15, 1);
        SetInt(0x13, 0);
        SetInt(0x19, 0);
        SetInt(10, 0);
        break;
    case 2:
        SetInt(0, 0x320);
        SetInt(1, 0x258);
        SetInt(9, 0);
        SetInt(0x15, 1);
        SetInt(0x13, 1);
        SetInt(0x19, 0);
        SetInt(10, 0);
        break;
    case 3:
        SetInt(0, 0x400);
        SetInt(1, 0x300);
        SetInt(9, 0);
        SetInt(0x15, 1);
        SetInt(0x13, 1);
        SetInt(0x19, 1);
        SetInt(10, 1);
        break;
    case 4:
        SetInt(0, 0x400);
        SetInt(1, 0x300);
        SetInt(9, 1);
        SetInt(0x15, 1);
        SetInt(0x13, 1);
        SetInt(0x19, 1);
        SetInt(10, 1);
        break;
    }
    GetDetailLevel();
    UpdateQuality();
    Clear();
}

void SETTINGS::Init()
{
    int i;
    unsigned int offset;
    offset = i = 0;
    for (; offset < 0x69c; i++, offset += 0x24)
        m_1030[i] = *(int *)(g_settingRecords + offset);
    m_1034[0] = g_settingFloat0;
    m_1034[1] = g_settingFloat1;
    m_1034[2] = g_settingFloat2;
}

int SETTINGS::GetInt(int index)
{
    return m_1030[index];
}

void SETTINGS::SetInt(int index, int value)
{
    int first = m_1030[0];
    int second = m_1030[1];
    m_1030[index] = value;
    if (first != m_1030[0] || second != m_1030[1])
        m_ff4 = true;
}

float SETTINGS::GetFloat(int index)
{
    return m_1034[index];
}

void SETTINGS::SetFloat(int index, float value)
{
    m_1034[index] = value;
}

bool SETTINGS::Changed()
{
    if (m_ff4) {
        m_ff4 = false;
        return true;
    }
    return false;
}

void SETTINGS::Clear()
{
    if (m_1030[0x21] == 0 && m_1030[0x23] > 0) {
        int first = m_1030[0];
        int second = m_1030[1];
        m_1030[0x23] = 0;
        if (first != m_1030[0] || second != m_1030[1])
            m_ff4 = true;
    }

    int first = m_1030[0];
    int second = m_1030[1];
    m_1030[0x0a] = 0;
    if (first != m_1030[0] || second != m_1030[1])
        m_ff4 = true;
}

void SETTINGS::Reset()
{
    for (int i = 0; i < 0xff; i++) {
        m_3fc[i] = -1;
        m_7f8[i] = -1;
        m_bf4[i] = 0;
    }
    m_7f8[0] = 1;
    m_7f8[1] = 1;
    m_bf4[0] = -1;
    m_bf4[1] = 1;
    m_3fc[2] = 0;
    m_3fc[3] = 3;
    m_3fc[4] = 4;
    m_3fc[5] = 5;
    m_3fc[6] = 2;
    m_3fc[0x0b] = 1;
    m_3fc[9] = 7;
    m_3fc[10] = 6;
}

void SETTINGS::RemovePair(int first, int second)
{
    for (int i = 0; i < 0xff; i++) {
        if (m_7f8[i] == first && m_bf4[i] == second) {
            m_7f8[i] = -1;
            m_bf4[i] = 0;
        }
    }
}

void SETTINGS::AddPair(unsigned int index, int value)
{
    for (int i = 0; i < 0xff; i++) {
        if (m_3fc[i] == value)
            m_3fc[i] = -1;
    }
    if (index < 0xff) {
        m_3fc[index] = value;
        m_7f8[index] = -1;
        m_bf4[index] = 0;
    }
}

void SETTINGS::ReplacePair(unsigned int index, int first, int second)
{
    RemovePair(first, second);
    if (index < 0xff) {
        m_3fc[index] = -1;
        m_7f8[index] = first;
        m_bf4[index] = second;
    }
}

void SETTINGS::ClearAll()
{
    for (int i = 0; i < 0xff; i++) {
        m_000[i] = -1;
        m_3fc[i] = -1;
        m_7f8[i] = -1;
        m_bf4[i] = 0;
    }
}

void SETTINGS::ResetKeys()
{
    for (int i = 0; i < 0xff; i++)
        m_000[i] = -1;
    m_000[0] = 0x25;
    m_000[1] = 0x27;
    m_000[2] = 0x26;
    m_000[3] = 0x28;
    m_000[4] = 0x43;
    m_000[5] = 0x53;
    m_000[6] = 0x52;
    m_000[7] = 0x4d;
    m_000[8] = 0x50;
    m_000[9] = 0x41;
    m_000[10] = 0x5a;
    m_000[0x0b] = 0x20;
    m_000[0x0c] = 0x48;
}

int SETTINGS::GetDetailLevel()
{
    char flag = *(char *)((char *)m_ff0 + 0x51);
    int level = 4;
    int a = HardwareA();
    int b = HardwareB();
    int c = HardwareC();
    if (c < 1)
        c = 0x20;
    if (a < 0x12d)
        a = 1000;
    if (c < 0x28)
        level = 3;
    if (a < 0x514)
        level = 3;
    if (b < 0xc0)
        level = 3;
    if (c < 0x1c)
        level = 2;
    if (a < 900)
        level = 2;
    if (b < 0x96)
        level = 2;
    if (c < 0x11)
        level = 1;
    if (a < 800)
        level = 1;
    if (a < 600)
        level = 0;
    if (b < 0x6e)
        level = 0;
    if (c < 0xf)
        return 0;
    if ((level != 4 && level != 3) || flag == 0)
        return level;
    return 2;
}

void INTRO::Load(const char *name, const char *path)
{
    (void)name;
    (void)path;
}
