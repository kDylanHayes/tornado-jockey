// The TEXTFILE tokenizer, 0x00528610.
//
// Discovery scaffold. The loader at 0x00528610 reports the
// "TEXTFILE - file does not exist" error; the functions through 0x0052d3ff
// read and tokenize the text files the skinned/animated objects load. The
// next object, ANIMATEDGEOMETRY, starts at 0x0052d400. The class name comes
// from the loader string.
#ifndef TONGAS2_TEXTFILE_H
#define TONGAS2_TEXTFILE_H

#include <string>
#include <vector>

class TEXTFILE
{
public:
    TEXTFILE();
    int Load(const char *name);
    int GetToken();

private:
    std::vector<int> m_000;
    std::vector<int> m_010;
    std::vector<int> m_020;
    void *m_030;
    void *m_034;
    char m_038[0x14];
    std::vector<int> m_04c;
    char m_05c[0x14];
    std::vector<std::string> m_070;
    void *m_080;
    std::vector<int> m_084;
};

#endif
