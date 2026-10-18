#include "wt/WTCRYPT.h"

// Not zero-terminated.
char *Xor(const char *data, const char *key, int len, int keyLen)
{
    char *out = new char[len + 1];
    int k = 0;
    for (int i = 0; i < len; i++) {
        out[i] = data[i] ^ key[k];
        k = (k + 1) % keyLen;
    }
    return out;
}

int Hash(const char *text, int len)
{
    int hash = 0;
    if (text) {
        for (int i = 0; i < len; i++)
            hash = (hash << 2) ^ text[i];
        hash &= 0x7fffffff;
    }
    return hash;
}

std::string Encrypt(std::string data, std::string key)
{
    char *encrypted = Xor(data.c_str(), key.c_str(), data.length(), key.length());
    return Base64Encode(encrypted, data.length());
}
