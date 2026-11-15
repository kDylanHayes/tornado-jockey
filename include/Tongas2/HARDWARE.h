// Hardware detection and the Direct3D capability cache, 0x00504d40.
//
// The HARDWARE object (0x00504f90) holds six DWORD capability values at +0x20
// and six byte-level defaults at +0x38; +0x50 is the "detection finished"
// flag. The accessors below are members (this in ecx) and warn through
// MessageBoxA when the detector has not run yet.
#ifndef TONGAS2_HARDWARE_H
#define TONGAS2_HARDWARE_H

int HardwareA();
int HardwareC();
int HardwareD();

class HARDWARE
{
public:
    HARDWARE();
    ~HARDWARE();
    void Detect();

    int HardwareE(int index);
    bool IsSupported(int index);

private:
    char m_000[0x20];
    int m_20[6];
    bool m_38[0x18];
    bool m_50;
    char pad_51[3];
};

#endif
