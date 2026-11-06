// The SCRoot object, 0x0050b3e0.
//
// Discovery scaffold. 0x0050b3e0 copies three 16-element blocks and an
// embedded string into a 0x134-byte object; 0x0050b480 creates one with the
// name "SCRoot"; 0x0050b600 is the large loader/updater that calls the
// vector helpers at 0x0050a5a0 and 0x0050a670. The class name comes from the
// string. SKINNEDGEOMETRY's loader starts at 0x0050d910, the next unit.
#ifndef TONGAS2_SCROOT_H
#define TONGAS2_SCROOT_H

class SCROOT
{
public:
    SCROOT(int type, const void *data);
    void Init();
    void Load(void *data, int a, int b, int c);

private:
    char m_000[0x134];
};

SCROOT *CreateSCROOT();

#endif
