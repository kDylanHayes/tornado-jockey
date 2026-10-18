// Builds the URLs of the WildTangent online leaderboard pages. Placeholder
// names.
#ifndef WT_WTLEADERBOARD_H
#define WT_WTLEADERBOARD_H

#include <string>

class WTLEADERBOARDIMPL
{
public:
    WTLEADERBOARDIMPL();
    ~WTLEADERBOARDIMPL();
    // The page that records a score.
    std::string GetSubmitURL(const std::string &secretKey, const std::string &leaderboardKey, int score);
    // The page that shows a leaderboard.
    std::string GetViewURL(const std::string &leaderboardKey);
};

class WTLEADERBOARD
{
public:
    WTLEADERBOARD();
    ~WTLEADERBOARD();
    // Throws a string when either key is NULL.
    std::string GetSubmitURL(const char *secretKey, const char *leaderboardKey, int score);
    std::string GetViewURL(const char *leaderboardKey);

    WTLEADERBOARDIMPL *m_impl;
};

#endif
