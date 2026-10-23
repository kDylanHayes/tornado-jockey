#include "Tongas2/TEXT.h"
#include "Tongas2/LOG.h"
#include "Tongas2/PATH.h"
#include "Tongas2/SAFEINDEX.h"
#include "Tongas2/TOKENS.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

class TEXTMANAGER : public RESOURCEMANAGER
{
public:
    TEXTMANAGER()
    {
        SafeSprintf(m_fileName, "TEXT.txt");
        m_8 = false;
        m_18 = true;
    }

    virtual RESOURCEMASTER *CreateMaster(const char *name) { return new TEXTMASTER(this, name); }
    virtual const char *GetTypeCode();
};

static RESOURCEMANAGER *CreateTEXT();

CREATEMANAGER g_pfnCreateTEXT = CreateTEXT;

CMDENTRY TEXTMASTER::s_commands[] = {
    { 0, 0, 0, 0, 0 },
};

PROPENTRY TEXTMASTER::s_properties[] = {
    { "ShaderNames", 0, 7, 0, PROP_FIELD(TEXTMASTER, m_shaderNames), false, 8, PROP_SETTER(TEXTMASTER, SetShaderNames) },
    { "SpacingHorizontal", 0, 3, 0, PROP_FIELD(TEXTMASTER, m_spacingHorizontal), false, 1, PROP_SETTER(TEXTMASTER, SetSpacingHorizontal) },
    { "SpacingVertical", 0, 3, 0, PROP_FIELD(TEXTMASTER, m_spacingVertical), false, 1, PROP_SETTER(TEXTMASTER, SetSpacingVertical) },
    { "BaseLine", 0, 3, 0, PROP_FIELD(TEXTMASTER, m_baseLine), false, 1, PROP_SETTER(TEXTMASTER, SetBaseLine) },
    { "DefaultHeight", 0, 3, 0, PROP_FIELD(TEXTMASTER, m_defaultHeight), false, 1, PROP_SETTER(TEXTMASTER, SetDefaultHeight) },
    { "CharactersData", 0, 5, 0, PROP_FIELD(TEXTMASTER, m_charactersData), false, 1, PROP_SETTER(TEXTMASTER, SetCharactersData) },
    { 0, 0, 15, 0, 0, false },
};

void TEXT::SetBullet(wchar_t c)
{
    m_bulletChar = c;
    m_bullet = true;
}

void TEXT::ClearBullet()
{
    m_bullet = false;
}

bool TEXT::IsLoaded()
{
    TEXTMASTER *master = Master();
    int pending = 0;
    for (int i = 0; i < TEXT_MAX_FONTS; i++) {
        RESOURCE *shader = g_pResources->GetResource(master->m_shaders[i]);
        if (shader && !shader->IsLoaded())
            pending++;
    }
    bool loaded = pending <= 0;
    return loaded;
}

void TEXT::SetText(const wchar_t *text)
{
    wcsncpy(m_text, text, TEXT_MAX_CHARS);
}

void TEXT::Start()
{
    int i;
    for (i = 0; i < m_numChars; i++) {
        RESOURCE *decal = g_pResources->GetResource(m_chars[i]);
        if (decal)
            decal->SetActive(true);
    }
    RESOURCE *decal = g_pResources->GetResource(m_chars[i]);
    if (decal)
        decal->SetActive(true);
}

void TEXT::Stop()
{
    for (int i = 0; i < m_numChars; i++) {
        RESOURCE *decal = g_pResources->GetResource(m_chars[i]);
        if (decal && decal->m_active) {
            decal->Stop();
            decal->m_active = false;
        }
    }
}

void TEXT::SetWrapWidth(float width)
{
    m_wrapWidth = width;
}

void TEXTMASTER::RegisterCommands()
{
    g_classParents[s_classId] = RESOURCEMASTER::s_classId;
    g_classCommands[s_classId] = s_commands;
    HashCommands(s_commands);
    RegisterClassName(s_commands, "TEXTMASTER", "RESOURCEMASTER");
}

const char *TEXTMANAGER::GetTypeCode()
{
    return "TEXT";
}

static RESOURCEMANAGER *CreateTEXT()
{
    return new TEXTMANAGER;
}

TEXT::~TEXT()
{
    for (int i = 0; i < m_numChars; i++) {
        RESOURCE *decal = g_pResources->GetResource(m_chars[i]);
        if (decal)
            decal->Virtual20();
        m_chars[i].Invalidate();
    }
}

void TEXT::SetPosition(V3 position)
{
    m_position = position;
    m_onScreen = false;
}

void TEXT::Init()
{
    OBJECT::Init();
    wcsncpy(m_text, L"", TEXT_MAX_CHARS);
    m_color = V3I(255, 255, 255);
    m_lastColor = V3I(255, 255, 255);
    m_bounds.Reset();
    m_bullet = false;
    m_lastLayer = -1;
    m_layer = 255;
}

void TEXTMASTER::SetShaderNames(const int &i, const CSTRING &name)
{
    m_shaderNames[i] = name;
    m_shaders[i] = g_pSHADER->GetHandle(name.c_str());
    m_decals[i] = g_pDECAL->GetHandle(name.c_str());
}

TEXT::TEXT(TEXTMASTER *master, const RESHANDLE &handle)
    : OBJECT(master, handle)
{
    m_alignH = 1;
    m_alignV = 1;
    m_onScreen = false;
    m_numChars = 0;
    m_lastText[0] = 0;
    m_3458 = 0;
    m_lastAlignH = 3;
    m_wrapWidth = -1.0f;
    m_bounds.Reset();
    m_bullet = false;
}

int TEXTMASTER::GetCharFrame(wchar_t c)
{
    unsigned short i = m_charIndex[c];
    if (i == 0xffff) {
        if (c != '\n' && c != '\r' && g_pLog)
            g_pLog->Error("FONT %s - using unicode char 0x%X that is not correctly defined", m_name.c_str(), c);
        return -1;
    }
    return m_chars.Get(i)->frame;
}

TEXTMASTER::TEXTMASTER(RESOURCEMANAGER *manager, const char *name)
    : RESOURCEMASTER(manager, name)
{
    for (g_i = 0; g_i < TEXT_MAX_FONTS; g_i++)
        m_shaderNames[g_i] = "whiteFont";
    m_spacingHorizontal = 5.0f;
    m_spacingVertical = 30.0f;
    m_baseLine = 25.0f;
    m_defaultHeight = 28.0f;
    m_charactersData = L"";
}

int TEXTMASTER::GetClassId()
{
    return s_classId;
}

int TEXTMASTER::GetParentClassId()
{
    return g_classParents[s_classId];
}

const char *TEXTMASTER::GetClassName()
{
    return g_classNames[s_classId];
}

RESOURCE *TEXTMASTER::CreateCopy(const RESHANDLE &handle)
{
    return new TEXT(this, handle);
}

void TEXT::Update()
{
    OBJECT::Update();
    if (!m_active)
        return;

    bool restyle = false;
    bool layout = false;
    bool onScreen = m_onScreen;
    if (onScreen != m_lastOnScreen) {
        m_lastOnScreen = onScreen;
        layout = true;
    }
    if (onScreen) {
        if (m_screenPosition.x != m_lastScreenPosition.x || m_screenPosition.y != m_lastScreenPosition.y) {
            m_lastScreenPosition.x = m_screenPosition.x;
            m_lastScreenPosition.y = m_screenPosition.y;
            layout = true;
        }
    } else if (m_position != m_c0) {
        m_c0 = m_position;
        layout = true;
    }
    const V3 &scale = m_scale;
    if (scale.x != m_d8.x || scale.y != m_d8.y || scale.z != m_d8.z) {
        m_d8 = m_scale;
        layout = true;
    }
    if (m_alignV != m_lastAlignV) {
        m_lastAlignV = m_alignV;
        layout = true;
    }
    if (m_alignH != m_lastAlignH) {
        m_lastAlignH = m_alignH;
        layout = true;
    }
    if (m_lineSpacing != m_lastLineSpacing) {
        m_lastLineSpacing = m_lineSpacing;
        layout = true;
    }
    if (m_layer != m_lastLayer) {
        m_lastLayer = m_layer;
        restyle = true;
    }
    const V3I &color = m_color;
    if (color.x != m_lastColor.x || color.y != m_lastColor.y || color.z != m_lastColor.z) {
        m_lastColor = m_color;
        restyle = true;
    }
    if (m_19c != m_1a0) {
        m_1a0 = m_19c;
        restyle = true;
    }

    TEXTMASTER *master = Master();
    int i;
    if (wcscmp(m_lastText, m_text)) {
        // Puts each "@:expression;" 's value in its place.
        wchar_t *text = m_text;
        wchar_t *at = wcsstr(text, L"@:");
        if (at) {
            wchar_t result[TEXT_MAX_CHARS];
            char expression[0x400];
            wchar_t value[TEXT_MAX_CHARS];
            result[0] = 0;
            value[0] = 0;
            value[TEXT_MAX_CHARS - 1] = 0;
            do {
                *at = 0;
                wcsncat(result, text, TEXT_MAX_CHARS - 1);
                wchar_t *end = wcschr(at + 2, ';');
                if (!end) {
                    if (g_pLog)
                        g_pLog->Error("  Error at scanning command for text");
                    break;
                }
                *end = 0;
                sprintf(expression, "%S", at + 2);
                SRESULT r = g_pConsole->Evaluate(STRING(expression), RESHANDLE());
                switch (r.GetType()) {
                case SRT_INT:
                    SafeSwprintf(value, L"%d", r.GetInt(0));
                    wcsncat(result, value, TEXT_MAX_CHARS - 1);
                    break;
                case SRT_UNICODESTRING:
                    wcsncat(result, r.GetUnicodeString(L""), TEXT_MAX_CHARS - 1);
                    break;
                case SRT_FLOAT:
                    SafeSwprintf(value, L"%f", r.GetFloat(0.0f));
                    wcsncat(result, value, TEXT_MAX_CHARS - 1);
                default:
                    if (g_pLog)
                        g_pLog->Error("Error - Variable in TEXT - Type not supported %s", r.GetTypeName());
                }
                text = end + 1;
                at = wcsstr(text, L"@:");
            } while (at);
            wcsncat(result, text, TEXT_MAX_CHARS - 1);
            wcsncpy(m_text, result, TEXT_MAX_CHARS - 1);
        }

        // Breaks the lines that run past the wrap width.
        if (m_wrapWidth > -1.0f && m_onScreen) {
            wchar_t wrapped[TEXT_MAX_CHARS];
            wchar_t word[TEXT_MAX_CHARS];
            wrapped[0] = 0;
            word[0] = 0;
            int length = wcslen(m_text);
            int n = 0;
            float wordWidth = 0.0f;
            float lineWidth = 0.0f;
            bool first = true;
            bool lineStart = true;
            int lineChars = 0;
            int wordChars = 0;
            wchar_t *p = m_text;
            for (int left = length; left > 0; left--, p++) {
                wchar_t c = *p;
                if (c == ' ' || c == '\n') {
                    word[n] = c;
                    lineWidth = lineWidth + wordWidth;
                    word[n + 1] = 0;
                    lineChars += wordChars;
                    wcsncat(wrapped, word, TEXT_MAX_CHARS - 1);
                    n = 0;
                    wordChars = 0;
                    wordWidth = 0.0f;
                    if (*p == '\n') {
                        lineChars = 0;
                        lineWidth = 0.0f;
                        if (m_bullet) {
                            wcsncat(wrapped, L"  ", TEXT_MAX_CHARS - 1);
                            float space = master->GetCharWidth(' ');
                            lineChars = 2;
                            lineWidth = space + space;
                        }
                    } else {
                        lineWidth = master->GetCharWidth(c) + lineWidth;
                        lineChars++;
                    }
                    word[0] = 0;
                } else {
                    float width = master->GetCharWidth(c);
                    if (m_wrapWidth <= wordChars * master->m_spacingHorizontal + width + wordWidth) {
                        wordChars = 0;
                        word[n] = '\n';
                        word[n + 1] = 0;
                        wcsncat(wrapped, word, TEXT_MAX_CHARS - 1);
                        lineChars = 0;
                        n = 0;
                        lineWidth = 0.0f;
                        wordWidth = 0.0f;
                        word[0] = 0;
                        lineStart = true;
                    }
                    if (first && m_bullet) {
                        word[n++] = m_bulletChar;
                        word[n++] = ' ';
                        first = false;
                        lineStart = false;
                        float space = master->GetCharWidth(' ') + wordWidth;
                        wordWidth = master->GetCharWidth(m_bulletChar) + space;
                        wordChars++;
                    } else if (lineStart && m_bullet) {
                        word[n++] = ' ';
                        word[n++] = ' ';
                        lineStart = false;
                        float space = master->GetCharWidth(' ');
                        wordChars += 2;
                        wordWidth = space + space + wordWidth;
                    }
                    wordWidth = width + wordWidth;
                    word[n] = *p;
                    word[n + 1] = 0;
                    n++;
                    wordChars++;
                    if ((lineChars + wordChars - 1) * master->m_spacingHorizontal + lineWidth + wordWidth > m_wrapWidth) {
                        wcsncat(wrapped, L" \n", TEXT_MAX_CHARS - 1);
                        lineChars = 0;
                        lineWidth = 0.0f;
                        if (m_bullet) {
                            wcsncat(wrapped, L"  ", TEXT_MAX_CHARS - 1);
                            float space = master->GetCharWidth(' ');
                            lineChars = 2;
                            lineWidth = space + space;
                        }
                    }
                }
            }
            word[n] = 0;
            wcsncat(wrapped, word, TEXT_MAX_CHARS - 1);
            wcsncpy(m_text, wrapped, TEXT_MAX_CHARS - 1);
        }

        // Gives each character a decal of its font's page.
        wcsncpy(m_lastText, m_text, TEXT_MAX_CHARS - 1);
        int count = wcslen(m_text);
        for (i = 0; i < count; i++) {
            int frame = master->GetCharFrame(m_text[i]);
            int font = master->GetCharFont(m_text[i]);
            if (frame == -1 || font == -1) {
                m_fonts[i] = -1;
                RESOURCE *old = g_pResources->GetResource(m_chars[i]);
                if (old)
                    old->Virtual20();
                m_chars[i].Invalidate();
                continue;
            }
            if (i >= m_numChars || font != m_fonts[i]) {
                RESOURCE *old = g_pResources->GetResource(m_chars[i]);
                if (old)
                    old->Virtual20();
                m_chars[i].Invalidate();
                DECAL *decal = (DECAL *)g_pResources->GetResource(master->m_decals[font]);
                if (!decal) {
                    if (g_pLog)
                        g_pLog->Error("Can't use decal %d of text %s", font, master->m_name.c_str());
                    m_chars[i] = RESHANDLE();
                    continue;
                }
                decal->SetColor(*(const V3 *)&m_color);
                decal->SetLayer(m_19c);
                decal->SetAlpha(m_layer * (1.0f / 255.0f));
                if (m_active)
                    decal->SetActive(true);
                m_chars[i] = decal->m_handle;
            }
            m_frames[i] = frame;
            m_fonts[i] = font;
        }
        for (i = count; i < m_numChars; i++) {
            RESOURCE *old = g_pResources->GetResource(m_chars[i]);
            if (old)
                old->Virtual20();
            m_chars[i].Invalidate();
        }
        m_numChars = count;
        for (i = 0; i < m_numChars; i++) {
            DECAL *decal = (DECAL *)g_pResources->GetResource(m_chars[i]);
            if (decal)
                decal->m_frame = m_frames[i];
        }
        layout = true;
    }

    if (layout) {
        m_bounds.Reset();
        m_bounds.maxX = m_bounds.maxY = m_bounds.minX = m_bounds.minY = 0.0f;
        bool bounded = false;
        CAMERA *camera = (CAMERA *)g_pCamera;
        V3 origin;
        V3 right;
        V3 down;
        if (m_onScreen) {
            origin = V3(m_screenPosition.x, m_screenPosition.y, 0.0f);
            right = V3(1.0f, 0.0f, 0.0f);
            down = V3(0.0f, 1.0f, 0.0f);
        } else {
            origin = m_position;
            const V3 &a = camera->m_up;
            const V3 &b = camera->m_direction;
            V3 side;
            side = V3(a.y * b.z - a.z * b.y, a.z * b.x - b.z * a.x, b.y * a.x - a.y * b.x);
            float length = side.Length();
            if (length != 0.0f) {
                float r = 1.0f / length;
                side.x = side.x * r;
                side.y = side.y * r;
                side.z = side.z * r;
            } else {
                side.x = side.y = side.z = 0.0f;
            }
            right = side;
            down = camera->m_up * -1.0f;
        }
        float height = camera->m_lens->m_aspect * m_scale.y;
        float width = m_scale.x;
        if (g_pDisplay->NeedsAspectFix())
            g_pDisplay->FixHeight(&height);
        right = right * width;
        down = down * height;
        V2 size(width, height);

        if (m_alignV) {
            float textHeight = 0.0f;
            for (i = 0; i < m_numChars; i++) {
                if (m_text[i] == '\n')
                    textHeight = (master->m_defaultHeight + master->m_spacingVertical) * m_lineSpacing + textHeight;
            }
            textHeight += master->m_defaultHeight;
            if (m_alignV == 1)
                origin -= down * (textHeight * 0.5f);
            else
                origin -= down * textHeight;
        }

        V3 line = origin;
        V3 pos;
        bool lineStart = true;
        for (i = 0; i < m_numChars; i++) {
            if (lineStart) {
                pos = line;
                lineStart = false;
                if (m_alignH) {
                    float lineWidth = 0.0f;
                    int k;
                    for (k = i; k < m_numChars; k++) {
                        wchar_t c = m_text[k];
                        if (c == '\n' || c == '\r')
                            break;
                        lineWidth = master->GetCharWidth(c) + lineWidth;
                    }
                    lineWidth = (k - i - 1) * master->m_spacingHorizontal + lineWidth;
                    if (m_alignH == 1)
                        pos -= right * (lineWidth * 0.5f);
                    else
                        pos -= right * lineWidth;
                }
            }
            wchar_t c = m_text[i];
            float charHeight = master->m_defaultHeight;
            if (c == '\n') {
                float spacing = m_lineSpacing;
                lineStart = true;
                line += down * (charHeight + master->m_spacingVertical) * spacing;
            } else if (c != '\r') {
                float charWidth = master->GetCharWidth(c);
                float drop = master->GetCharDrop(c);
                DECAL *decal = (DECAL *)g_pResources->GetResource(m_chars[i]);
                if (decal) {
                    if (m_onScreen) {
                        decal->m_screenPosition.x = right.x * charWidth * 0.5f + pos.x;
                        decal->m_screenPosition.y = (charHeight * 0.5f + drop) * down.y + pos.y;
                        decal->m_screenSpace = true;
                        float halfWidth = size.x * 0.5f;
                        if (m_bounds.minX > decal->m_screenPosition.x - halfWidth)
                            m_bounds.minX = decal->m_screenPosition.x - halfWidth;
                        if (m_bounds.maxX < halfWidth + decal->m_screenPosition.x)
                            m_bounds.maxX = decal->m_screenPosition.x + halfWidth;
                        float halfHeight = size.y * 0.5f;
                        if (m_bounds.minY > decal->m_screenPosition.y - halfHeight)
                            m_bounds.minY = decal->m_screenPosition.y - halfHeight;
                        if (m_bounds.maxY < halfHeight + decal->m_screenPosition.y)
                            m_bounds.maxY = decal->m_screenPosition.y + halfHeight;
                        if (!bounded) {
                            bounded = true;
                            m_bounds.minX = decal->m_screenPosition.x - halfWidth;
                            m_bounds.maxX = decal->m_screenPosition.x + halfWidth;
                            m_bounds.minY = decal->m_screenPosition.y - halfHeight;
                            m_bounds.maxY = decal->m_screenPosition.y + halfHeight;
                        }
                    } else {
                        decal->SetPosition(pos + right * (charWidth * 0.5f) + down * (charHeight * 0.5f + drop));
                    }
                    decal->SetSize(size);
                }
                pos += right * (charWidth + master->m_spacingHorizontal);
            }
        }
    }

    if (restyle) {
        for (i = 0; i < m_numChars; i++) {
            DECAL *decal = (DECAL *)g_pResources->GetResource(m_chars[i]);
            if (decal) {
                decal->SetLayer(m_19c);
                decal->SetAlpha(m_layer * (1.0f / 255.0f));
                decal->SetColor(*(const V3 *)&m_color);
            }
        }
    }
}

void TEXTMASTER::BuildCharacters()
{
    m_chars.Clear();
    int count = 0x80;
    m_chars.Resize(count);
    memset(m_charIndex, 0xff, sizeof(m_charIndex));
    UNICODETOKENS lines;
    count = 0;
    lines.Split(m_charactersData, L"\n");
    for (int i = 0; i < lines.m_count; i++) {
        unsigned int c = *lines.GetToken(i);
        if (m_charIndex[c] != 0xffff) {
            if (g_pLog)
                g_pLog->Error("TEXTMASTER %s Char 0x%04X has duplicated information!", m_name.c_str(), c);
            continue;
        }
        UNICODETOKENS fields;
        fields.Split(UNICODESTRING(lines.GetToken(i) + 2), L",");
        if (fields.m_count != 4) {
            if (g_pLog)
                g_pLog->Error("TEXTMASTER %s Char 0x%04X has incorrect format!", m_name.c_str(), c);
            continue;
        }
        if (count >= m_chars.m_capacity) {
            int size = m_chars.m_capacity + 0x80;
            m_chars.Resize(size);
        }
        FONTCHAR fc;
        fc.font = _wtol(fields.GetToken(0));
        fc.frame = _wtol(fields.GetToken(1));
        fc.width = (float)_wtof(fields.GetToken(2));
        fc.drop = (float)_wtof(fields.GetToken(3));
        m_chars.Insert(fc);
        m_charIndex[c] = count;
        count++;
    }
}
