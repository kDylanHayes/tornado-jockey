// The TEXTFILE tokenizer, 0x00528610.
//
// Discovery scaffold. The loader at 0x00528610 reports the
// "TEXTFILE - file does not exist" error; the functions through 0x0052d3ff
// read and tokenize the text files the skinned/animated objects load. The
// next object, ANIMATEDGEOMETRY, starts at 0x0052d400. The class name comes
// from the loader string.
#ifndef TONGAS2_TEXTFILE_H
#define TONGAS2_TEXTFILE_H

class TEXTFILE
{
public:
    int Load(const char *name);
    int GetToken();
    int GetLine();

private:
    char m_000[0x10];
};

#endif
