// The display. Placeholder names.
#ifndef TONGAS2_DISPLAY_H
#define TONGAS2_DISPLAY_H

// Defined elsewhere: the object at 0x6d09b8; only what other units use of it.
class DISPLAY
{
public:
    virtual void Virtual00();
    virtual void Virtual01();
    virtual void Virtual02();
    virtual void Virtual03();
    virtual void Virtual04();
    virtual void Virtual05();
    virtual void Virtual06();
    virtual void Virtual07();
    virtual void Virtual08();
    virtual void Virtual09();
    virtual void Virtual10();
    virtual void Virtual11();
    // Seconds since the last call.
    virtual float GetElapsedTime();
    virtual void Virtual13();
    virtual void Virtual14();
    virtual void Virtual15();
    virtual void Virtual16();
    virtual void Virtual17();
    virtual void Virtual18();
    virtual void Virtual19();
    virtual void Virtual20();
    virtual void Virtual21();
    virtual void Virtual22();
    virtual void Virtual23();
    virtual void Virtual24();
    virtual void Virtual25();
    virtual void Virtual26();
    // The size of the screen in pixels.
    virtual int GetWidth();
    virtual int GetHeight();
    virtual void Virtual29();
    virtual void Virtual30();
    // Whether the screen's shape calls for heights to be corrected, and the
    // correction.
    virtual bool NeedsAspectFix();
    virtual void FixHeight(float *height);
    virtual void Virtual33(float *height);
};
extern DISPLAY *g_pDisplay;

#endif
