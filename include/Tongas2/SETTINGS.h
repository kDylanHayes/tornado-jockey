// A placeholder unit for the settings/config object at 0x004fa5b0.
//
// Discovery scaffold. The object begins with a queue (array at 0x4, capacity
// at 0x8, head at 0xc, count at 0x10), then the settings tables at 0x3fc,
// 0x7f8 and 0xbf4 recovered from the INTRO unit. The two std::strings at
// 0xff8 and 0x1014 hold config.dat and default_config.dat. The class name is
// descriptive: no RegisterClassName string names it.
#ifndef TONGAS2_SETTINGS_H
#define TONGAS2_SETTINGS_H

#include <string>

class SETTINGS
{
public:
    SETTINGS(void *owner, std::string file, std::string defaultFile);
    ~SETTINGS();
    void PopFront();
    void ClearAll();
    void Destroy();
    void Load(const char *name);

private:
    char m_000[0x4];
    void **m_004;
    unsigned int m_008;
    unsigned int m_00c;
    unsigned int m_010;
    char m_014[0xff0 - 0x14];
    void *m_ff0;
    bool m_ff4;
    bool m_ff5;
    char m_ff6[2];
    std::string m_ff8;
    std::string m_1014;
    int *m_1030;
    float *m_1034;
    std::string *m_1038;
};

#endif
