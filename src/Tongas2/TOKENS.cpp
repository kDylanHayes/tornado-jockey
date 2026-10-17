#include <string.h>
#include <wchar.h>

#include "Tongas2/LOG.h"
#include "Tongas2/TOKENS.h"

bool TOKENS::Merge(int first, int last, char sep)
{
    if (first < 0)
        first = 0;
    if (first > m_count - 1)
        first = m_count - 1;
    if (last < 0)
        last = 0;
    if (last > m_count - 1)
        last = m_count - 1;
    if (first >= last)
        return false;

    int i;
    for (i = first + 1; i <= last; i++)
        ((char *)GetToken(i))[-1] = sep;
    int removed = last - first;
    for (i = last + 1; i < m_count; i++)
        m_offsets[i - removed] = m_offsets[i];
    m_count -= removed;
    return true;
}

void TOKENS::AddToken(char *token)
{
    if (m_count != TOKENS_MAX) {
        m_offsets[m_count] = token - m_string.c_str();
        m_count++;
    }
}

bool TOKENS::Split(const STRING &text, const char *delimiters)
{
    m_string = text;
    m_count = 0;
    for (char *token = strtok((char *)m_string.c_str(), delimiters); token; token = strtok(NULL, delimiters))
        AddToken(token);
    return true;
}

bool UNICODETOKENS::Split(const UNICODESTRING &text, const wchar_t *delimiters)
{
    m_string = text;
    for (wchar_t *token = wcstok((wchar_t *)m_string.c_str(), delimiters); token; token = wcstok(NULL, delimiters))
        AddToken(token);
    return true;
}

void TOKENS::Reset()
{
    m_string.Free();
    m_count = 0;
    m_parsed = false;
}

enum
{
    PS_NONE,
    PS_SINGLE,       // '...'
    PS_DOUBLE,       // "..."
    PS_PAREN,        // (...)
    PS_PAREN_SINGLE, // '...' inside (...)
    PS_PAREN_DOUBLE, // "..." inside (...)
    PS_BRACE,        // {...}
    PS_BRACE_SINGLE, // '...' inside {...}
    PS_BRACE_DOUBLE, // "..." inside {...}
    PS_WORD,
    PS_ESCAPE,       // after # in a string
};

bool TOKENS::Parse(STRING &text, bool *incomplete)
{
    if (incomplete)
        *incomplete = false;

    m_string.Reserve(text.Length());
    m_count = 0;
    const char *in = text.c_str();
    char *out = (char *)m_string.c_str();
    int state = PS_NONE;
    int escaped = PS_NONE;
    int parens = 0;
    int braces = 0;

    char c;
    while ((c = *in) != 0) {
        in++;
        switch (state) {
        case PS_NONE:
            if (c == '"') {
                AddToken(out);
                state = PS_DOUBLE;
            } else if (c == '\'') {
                AddToken(out);
                state = PS_SINGLE;
            } else if (c == '{') {
                AddToken(out);
                *out++ = c;
                *out++ = ' ';
                braces = 1;
                state = PS_BRACE;
            } else if (c == '(') {
                AddToken(out);
                state = PS_PAREN;
                parens = 1;
            } else if (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == ';') {
            } else {
                AddToken(out);
                *out++ = c;
                state = PS_WORD;
            }
            break;
        case PS_WORD:
            if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
                *out++ = 0;
                state = PS_NONE;
            } else if (c == '(') {
                *out++ = 0;
                AddToken(out);
                state = PS_PAREN;
                parens = 1;
            } else {
                *out++ = c;
            }
            break;
        case PS_PAREN:
            if (c == '(') {
                parens++;
                *out++ = c;
            } else if (c == '"') {
                *out++ = c;
                state = PS_PAREN_DOUBLE;
            } else if (c == '\'') {
                *out++ = c;
                state = PS_PAREN_SINGLE;
            } else if (c == ')') {
                if (--parens == 0) {
                    *out++ = 0;
                    state = PS_NONE;
                } else {
                    *out++ = ')';
                }
            } else {
                *out++ = c;
            }
            break;
        case PS_PAREN_SINGLE:
            *out++ = c;
            if (c == '\'')
                state = PS_PAREN;
            break;
        case PS_PAREN_DOUBLE:
            *out++ = c;
            if (c == '"')
                state = PS_PAREN;
            break;
        case PS_DOUBLE:
            if (c == '"') {
                *out++ = 0;
                state = PS_NONE;
            } else if (c == '#') {
                state = PS_ESCAPE;
                escaped = PS_DOUBLE;
            } else {
                *out++ = c;
            }
            break;
        case PS_SINGLE:
            if (c == '\'') {
                *out++ = 0;
                state = PS_NONE;
            } else if (c == '#') {
                state = PS_ESCAPE;
                escaped = PS_SINGLE;
            } else {
                *out++ = c;
            }
            break;
        case PS_ESCAPE:
            if (c == ':')
                c = ';';
            else if (c == 'n')
                c = '\n';
            else if (c == 'r')
                c = '\r';
            else if (c == 't')
                c = '\t';
            *out++ = c;
            state = escaped;
            break;
        case PS_BRACE:
            if (c == '{') {
                braces++;
                *out++ = c;
            } else if (c == '"') {
                *out++ = c;
                state = PS_BRACE_DOUBLE;
            } else if (c == '\'') {
                *out++ = c;
                state = PS_BRACE_SINGLE;
            } else if (c == '}') {
                if (--braces == 0) {
                    *out++ = c;
                    *out++ = 0;
                    state = PS_NONE;
                } else {
                    *out++ = c;
                }
            } else {
                *out++ = c;
            }
            break;
        case PS_BRACE_SINGLE:
            *out++ = c;
            if (c == '\'')
                state = PS_BRACE;
            break;
        case PS_BRACE_DOUBLE:
            *out++ = c;
            if (c == '"')
                state = PS_BRACE;
            break;
        }
    }

    if (state == PS_WORD) {
        *out = 0;
    } else if (state != PS_NONE) {
        Reset();
        if (state == PS_BRACE && incomplete) {
            *incomplete = true;
            return false;
        }
        if (g_pLog)
            g_pLog->Error("Parse> Error parsing \"%s\"", text.c_str());
        return false;
    }
    m_parsed = true;
    return true;
}

void TOKENS::Quote(STRING &text)
{
    Reset();
    m_string = text;
    text.Reserve(m_string.Length() * 2 + 2);

    const char *in = m_string.c_str();
    char *out = (char *)text.c_str();
    *out++ = '"';
    char c;
    while ((c = *in) != 0) {
        in++;
        if (c == '#' || c == ';' || c == '"' || c == '\'' || c == '\n' || c == '\r' || c == '\t') {
            *out++ = '#';
            if (c == ';')
                *out = ':';
            else if (c == '\n')
                *out = 'n';
            else if (c == '\t')
                *out = 't';
            else if (c == '\r')
                *out = 'r';
            else
                *out = c;
        } else {
            *out = c;
        }
        out++;
    }
    *out = '"';
    out[1] = 0;
}
