// On-screen and in-world text: TEXTMASTER, from TEXT.txt, is a font: the
// decals and shaders of up to eight pages of glyphs, the spacing, and for
// each character the page, frame, width and drop its CharactersData line
// gives. TEXT is one piece of text, laid out as a decal per character,
// with "@:expression;" replaced by the expression's value, optional word
// wrapping and bullets, and alignment about its position.
// Placeholder names apart from the class names and properties.
#ifndef TONGAS2_TEXT_H
#define TONGAS2_TEXT_H

#include "Tongas2/DECAL.h"
#include "Tongas2/APPWINDOW.h"
#include "Tongas2/DYNAMICARRAY.h"
#include "Tongas2/EMITTER.h"
#include "Tongas2/RESOURCE.h"
#include "Tongas2/RESOURCES.h"
#include "Tongas2/SRESULT.h"
#include "Tongas2/STRING.h"

#define TEXT_MAX_FONTS 8
#define TEXT_MAX_CHARS 0x200

// One character of a font.
struct FONTCHAR
{
    float width;
    // How far below the line's middle the glyph sits.
    float drop;
    // Which of the font's decals, and which frame of it.
    int font;
    int frame;
};

class TEXTMASTER : public RESOURCEMASTER
{
public:
    TEXTMASTER(RESOURCEMANAGER *manager, const char *name);

    static void RegisterCommands();

    virtual int GetClassId();
    virtual int GetParentClassId();
    virtual const char *GetClassName();
    virtual SRESULT Execute(CMDENTRY *command, TOKENS **args)
    {
        return (this->*(SRESULT (TEXTMASTER::*)(TOKENS **))command->func)(args);
    }
    virtual RESOURCE *CreateCopy(const RESHANDLE &handle);
    virtual void Prepare() { BuildCharacters(); }

    // Also looks up the page's decal and shader.
    virtual void SetShaderNames(const int &i, const CSTRING &name);
    virtual void SetSpacingHorizontal(const float &spacing) { m_spacingHorizontal = spacing; }
    virtual void SetSpacingVertical(const float &spacing) { m_spacingVertical = spacing; }
    virtual void SetBaseLine(const float &line) { m_baseLine = line; }
    virtual void SetDefaultHeight(const float &height) { m_defaultHeight = height; }
    // Rebuilds the characters if the font is loaded.
    virtual void SetCharactersData(const UNICODESTRING &data)
    {
        m_charactersData = data;
        if (Load())
            BuildCharacters();
    }

    // Reads m_charactersData: a line per character, the character then
    // "font,frame,width,drop".
    void BuildCharacters();
    // The frame of a character's glyph, or -1 (and a warning) if the font
    // has no such character.
    int GetCharFrame(wchar_t c);
    float GetCharWidth(wchar_t c)
    {
        unsigned short i = m_charIndex[c];
        if (i == 0xffff)
            return 0.0f;
        return m_chars.Get(i)->width;
    }
    float GetCharDrop(wchar_t c)
    {
        unsigned short i = m_charIndex[c];
        if (i == 0xffff)
            return 0.0f;
        return m_chars.Get(i)->drop;
    }
    int GetCharFont(wchar_t c)
    {
        unsigned short i = m_charIndex[c];
        if (i == 0xffff)
            return -1;
        return m_chars.Get(i)->font;
    }

    static int s_classId;
    static CMDENTRY s_commands[];
    static PROPENTRY s_properties[];

    CSTRING m_shaderNames[TEXT_MAX_FONTS];
    float m_spacingHorizontal;
    float m_spacingVertical;
    float m_baseLine;
    float m_defaultHeight;
    UNICODESTRING m_charactersData;
    // Each character's index in m_chars, 0xffff if it has none.
    unsigned short m_charIndex[0x10000];
    DYNAMICARRAY<FONTCHAR> m_chars;
    RESHANDLE m_decals[TEXT_MAX_FONTS];
    RESHANDLE m_shaders[TEXT_MAX_FONTS];
};

// A point on the screen, at the corner until set.
struct TEXTPOINT : V2
{
    TEXTPOINT() : V2(0.0f, 0.0f) {}
};

// The screen area text covers.
struct TEXTBOUNDS
{
    TEXTBOUNDS() : maxX(0.0f), maxY(0.0f), minX(0.0f), minY(0.0f) {}
    // Inside out, ready to grow.
    void Reset()
    {
        minX = 1000.0f;
        minY = 1000.0f;
        maxX = -1000.0f;
        maxY = -1000.0f;
    }

    float maxX, maxY;
    float minX, minY;
};

class TEXT : public OBJECT
{
public:
    TEXT(TEXTMASTER *master, const RESHANDLE &handle);
    // Lets the characters' decals go.
    virtual ~TEXT();

    TEXTMASTER *Master() { return (TEXTMASTER *)m_master; }

    virtual void Init();
    virtual void Start();
    virtual void Stop();
    // Rebuilds the characters when the text changes and places them when it
    // or its placement does.
    virtual void Update();
    // Once all the font's shaders are.
    virtual bool IsLoaded();
    virtual V3 GetScaleFactor() { return m_scaleFactor; }
    // Places the text in the world.
    virtual void SetPosition(V3 position);
    virtual void SetSize(V2 size) { SetScale(size.x, size.y, 1.0f); }
    virtual void SetScreenSize(V2 size) { SetScale(size.x, size.y, 1.0f); }

    // Starts each line with c and a space.
    void SetBullet(wchar_t c);
    void ClearBullet();
    void SetText(const wchar_t *text);
    void SetScreenPosition(float x, float y)
    {
        m_screenPosition.x = x;
        m_screenPosition.y = y;
    }
    // Wraps lines longer than width, or not if it is -1.
    void SetWrapWidth(float width);

    RESHANDLE m_chars[TEXT_MAX_CHARS + 1];
    int m_numChars;
    wchar_t m_text[TEXT_MAX_CHARS];
    int m_2010;
    int m_frames[TEXT_MAX_CHARS + 1];
    int m_fonts[TEXT_MAX_CHARS + 1];
    wchar_t m_lastText[TEXT_MAX_CHARS + 1];
    bool m_onScreen;
    bool m_lastOnScreen;
    TEXTPOINT m_lastScreenPosition;
    TEXTPOINT m_screenPosition;
    float m_lineSpacing;
    float m_lastLineSpacing;
    // 0 from the start, 1 about the middle, 2 from the end.
    int m_alignH;
    int m_lastAlignH;
    int m_alignV;
    int m_lastAlignV;
    float m_wrapWidth;
    RESHANDLE m_344c;
    int m_3458;
    V3I m_color;
    V3I m_lastColor;
    TEXTBOUNDS m_bounds;
    bool m_bullet;
    wchar_t m_bulletChar;
};

#endif
