// Splits a string into tokens in place: the text is copied into m_string and
// each token is a NUL-terminated piece of it, located by its offset.
// Placeholder names.
#ifndef TONGAS2_TOKENS_H
#define TONGAS2_TOKENS_H

#include "Tongas2/STRING.h"

#define TOKENS_MAX 0x1000

class TOKENS
{
public:
    // Joins tokens first..last into one, separated by sep.
    bool Merge(int first, int last, char sep);
    void AddToken(char *token);
    // Tokenizes with strtok.
    bool Split(const STRING &text, const char *delimiters);
    void Reset();
    // Script syntax: words, '...' and "..." strings, (...) and {...} groups,
    // ';' separators and # escapes. Sets *incomplete if a {...} is still open.
    bool Parse(STRING &text, bool *incomplete);
    // Copies text, then rewrites it quoted and escaped for Parse.
    void Quote(STRING &text);

    const char *GetToken(int i)
    {
        if (i < 0 || i >= m_count)
            return "";
        return m_string.c_str() + m_offsets[i];
    }

    STRING m_string;
    int m_count;
    int m_offsets[TOKENS_MAX];
    bool m_parsed;
};

class UNICODETOKENS
{
public:
    // Tokenizes with wcstok.
    bool Split(const UNICODESTRING &text, const wchar_t *delimiters);

    void AddToken(wchar_t *token)
    {
        if (m_count != TOKENS_MAX) {
            m_offsets[m_count] = ((unsigned)token - (unsigned)m_string.c_str()) / sizeof(wchar_t);
            m_count++;
        }
    }

    UNICODESTRING m_string;
    int m_count;
    int m_offsets[TOKENS_MAX];
    bool m_parsed;
};

#endif
