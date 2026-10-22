#include "Tongas2/UNICODESTRING.h"

char UNICODESTRING_MANAGER::s_decode[128];
char UNICODESTRING_MANAGER::s_encode[64];

UNICODESTRING_MANAGER g_UnicodeStringManager;

void UNICODESTRING_MANAGER::InitTables()
{
    char n = 0;
    char c;
    for (c = 'A'; c <= 'Z'; c++) {
        s_decode[c] = n;
        s_encode[n++] = c;
    }
    for (c = 'a'; c <= 'z'; c++) {
        s_decode[c] = n;
        s_encode[n++] = c;
    }
    for (c = '0'; c <= '9'; c++) {
        s_decode[c] = n;
        s_encode[n++] = c;
    }
    s_decode['*'] = n;
    s_encode[n++] = '*';
    s_decode['+'] = n;
    s_encode[n] = '+';
}

int UNICODESTRING_MANAGER::GetEncodedLength(const char *text)
{
    int len = strlen(text);
    if (len >= 2 && text[0] == 'W' && text[1] == 'C' && (len - 2) % 3 == 0)
        return (len - 2) / 3;
    return -1;
}

void UNICODESTRING_MANAGER::Decode(wchar_t *out, const char *in, int n)
{
    in += 2;
    for (int i = 0; i < n; i++) {
        wchar_t c = (wchar_t)s_decode[*in++];
        c |= (wchar_t)s_decode[*in++] << 6;
        c |= (wchar_t)s_decode[*in++] << 12;
        *out++ = c;
    }
    *out = 0;
}

void UNICODESTRING_MANAGER::Widen(wchar_t *out, const char *in, int n)
{
    int i;
    for (i = 0; i < n; i++)
        out[i] = (wchar_t)*in++;
    out[i] = 0;
}

void UNICODESTRING_MANAGER::Narrow(char *out, const wchar_t *in, int n)
{
    char *p = out;
    for (int i = 0; i < n; i++) {
        if (in[i] > 0xff)
            *p++ = '?';
        else
            *p++ = (char)in[i];
    }
    *p = 0;
}

UNICODESTRING_MANAGER::UNICODESTRING_MANAGER()
{
    int i;
    for (i = 0; i < NUMPAGES; i++) {
        wchar_t *page = m_pages[i];
        page[0] = 0;
        // A byte count used as an index: lands in the next page.
        page[PAGESIZE - 2] = 0;
        m_used[i] = false;
    }
    m_8014 = 0;
    m_d018 = 0;
    InitTables();
}

void UNICODESTRING_MANAGER::Defrag()
{
    int free = -1;
    for (int i = 0; i < NUMPAGES; i++) {
        if (m_used[i]) {
            if (free >= 0)
            while (m_used[i] && i < NUMPAGES) {
                memcpy((wchar_t *)m_pages + free * PAGESIZE, (wchar_t *)m_pages + i * PAGESIZE, PAGESIZE);
                ((wchar_t *)m_pages)[i * PAGESIZE] = 0;
                m_owners[free] = m_owners[i];
                UNICODESTRING *owner = m_owners[free];
                if (owner->m_block == i) {
                    owner->m_buf = GetPointer(free);
                    owner->m_block = free;
                }
                m_used[i] = false;
                i++;
                m_used[free] = true;
                free++;
            }
        } else if (free == -1)
            free = i;
    }
    m_cache[0].m_index = -1;
    m_cache[0].m_count = 1;
    m_cache[1].m_index = -1;
    m_cache[1].m_count = 1;
}

void UNICODESTRING::FromAnsi(const char *text)
{
    int n = UNICODESTRING_MANAGER::GetEncodedLength(text);
    if (n >= 0) {
        Reserve(n * 2 + 2);
        UNICODESTRING_MANAGER::Decode((wchar_t *)c_str(), text, n);
    } else {
        int len = strlen(text);
        Reserve(len * 2 + 2);
        UNICODESTRING_MANAGER::Widen((wchar_t *)c_str(), text, len);
    }
}

FIXEDSTRING<0x200> UNICODESTRING::ToAnsi()
{
    FIXEDSTRING<0x200> text;
    int n = Length() > 0x1ff ? 0x1ff : Length();
    UNICODESTRING_MANAGER::Narrow(text, c_str(), n);
    return text;
}

void UNICODESTRING_MANAGER::Encode(STRING &out, const wchar_t *in, int n)
{
    out.Reserve(n * 3 + 3);
    char *p = (char *)out.c_str();
    *p++ = 'W';
    *p++ = 'C';
    for (int i = 0; i < n; i++) {
        wchar_t c = *in++;
        *p++ = s_encode[c & 0x3f];
        *p++ = s_encode[(c >> 6) & 0x3f];
        *p++ = s_encode[(c >> 12) & 0x3f];
    }
    *p = 0;
}

void UNICODESTRING::ToEncoded(STRING &out)
{
    UNICODESTRING_MANAGER::Encode(out, c_str(), Length());
}
