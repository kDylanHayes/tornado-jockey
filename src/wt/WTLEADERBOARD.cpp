#include <new>

#include "wt/WTLEADERBOARD.h"

WTLEADERBOARD::WTLEADERBOARD()
{
    m_impl = new WTLEADERBOARDIMPL;
}

WTLEADERBOARD::~WTLEADERBOARD()
{
    delete m_impl;
}

std::string WTLEADERBOARD::GetSubmitURL(const char *secretKey, const char *leaderboardKey, int score)
{
    if (!secretKey || !leaderboardKey)
        throw "Secret Key or Leaderboard Key given was invalid.";
    return m_impl->GetSubmitURL(secretKey, leaderboardKey, score);
}

std::string WTLEADERBOARD::GetViewURL(const char *leaderboardKey)
{
    if (!leaderboardKey)
        return m_impl->GetViewURL(std::string());
    return m_impl->GetViewURL(std::string(leaderboardKey));
}
