#include <new>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <string>

#include "wt/WTINFO.h"
#include "wt/WTCRYPT.h"
#include "wt/WTLEADERBOARD.h"

static const std::string base64_chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

WTLEADERBOARDIMPL::WTLEADERBOARDIMPL()
{
}

WTLEADERBOARDIMPL::~WTLEADERBOARDIMPL()
{
}

std::string Base64Encode(const char *data, int len)
{
    std::string ret;
    for (int i = 0; i < len; i++) {
        char c = (data[i] >> 2) & 0x3f;
        ret += base64_chars[c];
        c = (data[i] & 3) << 4;
        i++;
        if (i < len)
            c |= (data[i] >> 4) & 0xf;
        ret += base64_chars[c];
        if (i < len) {
            c = (data[i] & 0xf) << 2;
            i++;
            if (i < len)
                c |= (data[i] >> 6) & 3;
            ret += base64_chars[c];
        }
        else {
            i++;
            ret += '=';
        }
        if (i < len) {
            c = data[i] & 0x3f;
            ret += base64_chars[c];
        }
        else
            ret += '=';
    }
    return ret;
}

std::string WTLEADERBOARDIMPL::GetSubmitURL(const std::string &secretKey, const std::string &leaderboardKey, int score)
{
    srand((unsigned)time(NULL));
    int salt = rand() % 10000;
    char number[1024];
    _itoa(salt, number, 10);
    std::string encryptedSalt = Encrypt(number, secretKey);
    _itoa(salt, number, 10);
    char scoreText[260];
    memset(scoreText, 0, sizeof(scoreText));
    sprintf(scoreText, "%d", score);
    std::string data = leaderboardKey + "=" + scoreText;
    std::string encryptedData = Encrypt(data, number);
    int len = encryptedData.length() + secretKey.length();
    _itoa(Hash((encryptedData + secretKey).c_str(), len), number, 10);
    std::string hash = "";
    hash += number;
    std::string dp = encryptedSalt + "$" + encryptedData + "$" + hash;
    WTINFO info(true);
    std::string product = info.GetProductCodeName();
    std::string partner = info.GetDPCodeName();
    std::string url = "http://wildboards.wildtangent.com/Redirector/Games/LeaderboardLookup/Decoder.aspx?dp=";
    url += partner;
    url += "&g=";
    url += product;
    url += "&d=";
    url += dp;
    srand(1);
    return url;
}

std::string WTLEADERBOARDIMPL::GetViewURL(const std::string &leaderboardKey)
{
    WTINFO info(true);
    std::string product = info.GetProductCodeName();
    std::string partner = info.GetDPCodeName();
    std::string url = "http://wildboards.wildtangent.com/Redirector/Games/LeaderboardLookup/Decoder.aspx?g=";
    url += product;
    url += "&dp=";
    url += partner;
    url += "&l=";
    url += leaderboardKey;
    return url;
}
