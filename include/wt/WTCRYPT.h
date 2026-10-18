#ifndef WT_WTCRYPT_H
#define WT_WTCRYPT_H

#include <string>

// The leaderboard's score obfuscation. Base64Encode lives with the
// leaderboard code; the rest is in its own file, because GetSubmitURL
// treats Hash as a call that may throw (an inlining or nothrow-aware
// compiler in the same file would not).

std::string Base64Encode(const char *data, int len);
char *Xor(const char *data, const char *key, int len, int keyLen);
int Hash(const char *text, int len);
std::string Encrypt(std::string data, std::string key);

#endif
