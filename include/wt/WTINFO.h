// Game-side reader of the WildTangent product settings; only what the
// leaderboard code calls. Placeholder names.
#ifndef WT_WTINFO_H
#define WT_WTINFO_H

#include <string>

class WTINFO
{
public:
    WTINFO(bool load);
    ~WTINFO();
    std::string GetProductCodeName();    // "ProductCodeName"
    std::string GetDPCodeName();         // "DPCodeName", the distribution partner

private:
    char m_data[0x44];
};

#endif
