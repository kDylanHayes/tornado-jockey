// Hardware detection and the Direct3D capability cache, 0x00504d40.
//
// Discovery scaffold. The class name and method names are descriptive: no
// RegisterClassName string names this object. The helpers at 0x00504d70 and
// 0x00504d40 are the HardwareA/HardwareC functions INTRO already maps, and
// 0x00504dd0 reads the OS version; 0x00504ff0 is the large capability
// detector that fills the name and capability tables.
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

private:
    char m_000[0x54];
};

#endif
