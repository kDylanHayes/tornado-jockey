// Tongas2/RENDERTEXTURE -- the DX8 renderer plugin's render textures.
//
// Discovery scaffold: class registration, manager factory and the
// constructor/destructor pair the strings and decompiler give. The bodies
// are empty until the unit is matched; every function here has its exact
// target size in config/symbols.txt so a later delink cannot cut the target
// short.
#include "Tongas2/RENDERTEXTURE.h"

class RENDERTEXTUREMANAGER : public RESOURCEMANAGER
{
public:
    RENDERTEXTUREMANAGER()
    {
        SafeSprintf(m_fileName, "RENDERTEXTURE.txt");
        m_8 = false;
        m_18 = true;
    }

    virtual RESOURCEMASTER *CreateMaster(const char *name) { return new RENDERTEXTUREMASTER(this, name); }
};

int RENDERTEXTUREMASTER::s_classId = -1;

CMDENTRY RENDERTEXTUREMASTER::s_commands[] = {
    { 0, 0, 0, 0, 0 },
};

void RENDERTEXTUREMASTER::RegisterCommands()
{
    g_classParents[s_classId] = RESOURCEMASTER::s_classId;
    g_classCommands[s_classId] = s_commands;
    HashCommands(s_commands);
    RegisterClassName(s_commands, "RENDERTEXTUREMASTER", "RESOURCEMASTER");
}

RESOURCEMANAGER *CreateRENDERTEXTURE()
{
    return new RENDERTEXTUREMANAGER;
}


RENDERTEXTUREMASTER::~RENDERTEXTUREMASTER()
{
}

// Generated from the delinked target for Tongas2/RENDERTEXTURE.
extern "C" int LAB_005ecbc1;
extern "C" int DAT_00675d98;
extern "C" int _except_list;
extern "C" int xr_1;
extern "C" void xr_10();
extern "C" void xr_11();
extern "C" void xr_12();
extern "C" void xr_13();
extern "C" int xr_14;
extern "C" void xr_15();
extern "C" void xr_16();
extern "C" int xr_2;
extern "C" void xr_3();
extern "C" void xr_4();
extern "C" int xr_5;
extern "C" int xr_6;
extern "C" int xr_7;
extern "C" int xr_8;
extern "C" int xr_9;
extern "C" void f_0();
extern "C" void f_10();
extern "C" void f_11();
extern "C" void f_12();
extern "C" void f_2();
extern "C" void f_3();
extern "C" void f_4();
extern "C" void f_5();
extern "C" void f_6();
extern "C" void f_7();
extern "C" void f_8();
extern "C" void f_9();

extern "C" __declspec(naked) void f_0() {
    __asm {
        __emit 0x8b
        __emit 0x51
        __emit 0x68
        __emit 0x33
        __emit 0xc0
        __emit 0x85
        __emit 0xd2
        __emit 0x56
        __emit 0x74
        __emit 0x15
        __emit 0x8d
        __emit 0x72
        __emit 0x01
        __emit 0x53
        __emit 0x8b
        __emit 0xff
        __emit 0x8a
        __emit 0x1a
        __emit 0x42
        __emit 0x84
        __emit 0xdb
        __emit 0x75
        __emit 0xf9
        __emit 0x2b
        __emit 0xd6
        __emit 0x83
        __emit 0xfa
        __emit 0x01
        __emit 0x5b
        __emit 0x7d
        __emit 0x25
        __emit 0x8b
        __emit 0x23
        __emit 0xaf
        __emit 0x6d
        __emit 0xee
        __emit 0x07
        __emit 0xbf
        __emit 0xc7
        __emit 0x39
        __emit 0x24
        __emit 0x6e
        __emit 0xd4
        __emit 0x2f
        __emit 0x10
        __emit 0xe2
        __emit 0x2c
        __emit 0xcb
        __emit 0x8a
        __emit 0x11
        __emit 0x41
        __emit 0x84
        __emit 0xd2
        __emit 0x75
        __emit 0xf9
        __emit 0x2b
        __emit 0xce
        __emit 0x85
        __emit 0xc9
        __emit 0x7e
        __emit 0x2a
        __emit 0xb8
        __emit 0x01
        __emit 0x00
        __emit 0x00
        __emit 0x00
        __emit 0x5e
        __emit 0xc3
        __emit 0x8b
        __emit 0x41
        __emit 0x6c
        __emit 0x85
        __emit 0xc0
        __emit 0x74
        __emit 0x17
        __emit 0x8d
        __emit 0x50
        __emit 0x01
        __emit 0x8b
        __emit 0xff
        __emit 0x8a
        __emit 0x08
        __emit 0x40
        __emit 0x84
        __emit 0xc9
        __emit 0x75
        __emit 0xf9
        __emit 0x2b
        __emit 0xc2
        __emit 0x85
        __emit 0xc0
        __emit 0xb8
        __emit 0x03
        __emit 0x00
        __emit 0x00
        __emit 0x00
        __emit 0x7f
        __emit 0x05
        __emit 0xb8
        __emit 0x02
        __emit 0x00
        __emit 0x00
        __emit 0x00
        __emit 0x5e
        __emit 0xc3
    }
}
class RENDERTEXTUREVTABLE
{
public:
    char pad[0x84];
    unsigned char m_88;

    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6();
    virtual void v7();
    virtual void v8();
    virtual void v9();
    virtual void v10();
    virtual void v11();
    virtual void v12();
    virtual void v13();
    virtual void v14();
    virtual void v15();
    virtual void v16();
    virtual void v17();
    virtual void v18();
    virtual void v19();
    virtual void v20();
    virtual void v21();
    virtual void v22();
    virtual void v23();
    virtual void v24();
    virtual void v25();
    virtual void v26();
    virtual void v27();
    virtual void v28();
    virtual void v29();
    virtual void v30();
    virtual void v31();
    virtual void v32();
    virtual void v33();
    virtual void v34();
    virtual void v35();
    virtual void v36();
    virtual void v37();
    virtual void v38();
    virtual void v39();
    virtual void v40();
    virtual void v41();
    virtual bool v42();
    virtual bool v43();
    virtual bool v44(int);
    virtual bool v45(int);
    virtual void v46();
    virtual void v47();
    virtual bool v48();
};

extern "C" bool __fastcall f_1(RENDERTEXTUREVTABLE *self)
{
    if (self->v42()) {
        self->v47();
    }
    switch (((int (__fastcall *)(void *))f_0)(self)) {
    case 0:
        self->v43();
        self->m_88 = 0;
        return true;
    case 1:
        if (self->v43()) {
            self->v46();
            self->m_88 = 0;
            return true;
        }
        break;
    case 2:
        self->v45(1);
        self->m_88 = 0;
        return true;
    case 3:
        if (self->v48()) {
            self->v44(1);
            self->m_88 = 0;
            return true;
        }
        if (self->v45(0)) {
            self->v46();
        }
        break;
    }
    self->m_88 = 0;
    return true;
}
extern "C" __declspec(naked) void f_2() {
    __asm {
        __emit 0x56
        __emit 0x8b
        __emit 0xf1
        __emit 0x8b
        __emit 0x86
        __emit 0x84
        __emit 0x00
        __emit 0x00
        __emit 0x00
        __emit 0x85
        __emit 0xc0
        __emit 0x7f
        __emit 0x22
        __emit 0x8b
        __emit 0x06
        __emit 0xff
        __emit 0x90
        __emit 0xa8
        __emit 0x00
        __emit 0x00
        __emit 0x00
        __emit 0x84
        __emit 0xc0
        __emit 0x74
        __emit 0x0c
        __emit 0x8b
        __emit 0x16
        __emit 0x8b
        __emit 0xce
        __emit 0xff
        __emit 0x92
        __emit 0xbc
        __emit 0x00
        __emit 0x00
        __emit 0x00
        __emit 0xeb
        __emit 0x1f
        __emit 0x8b
        __emit 0x86
        __emit 0x84
        __emit 0x00
        __emit 0x00
        __emit 0x00
        __emit 0x85
        __emit 0xc0
        __emit 0x7e
        __emit 0x15
        __emit 0x8b
        __emit 0x06
        __emit 0x8b
        __emit 0xce
        __emit 0xff
        __emit 0x90
        __emit 0xa8
        __emit 0x00
        __emit 0x00
        __emit 0x00
        __emit 0x84
        __emit 0xc0
        __emit 0x75
        __emit 0x07
        __emit 0x8b
        __emit 0xce
        call f_1
        __emit 0x8a
        __emit 0x86
        __emit 0x88
        __emit 0x00
        __emit 0x00
        __emit 0x00
        __emit 0x84
        __emit 0xc0
        __emit 0x74
        __emit 0x12
        __emit 0x8b
        __emit 0x86
        __emit 0x84
        __emit 0x00
        __emit 0x00
        __emit 0x00
        __emit 0x85
        __emit 0xc0
        __emit 0x7e
        __emit 0x08
        __emit 0x8b
        __emit 0xce
        __emit 0x5e
        jmp f_1
        __emit 0x5e
        __emit 0xc3
    }
}
extern "C" __declspec(naked) void f_3() {
    __asm {
        __emit 0x8b
        __emit 0x44
        __emit 0x24
        __emit 0x04
        __emit 0x8b
        __emit 0x10
        __emit 0x83
        __emit 0xec
        __emit 0x0c
        __emit 0x8b
        __emit 0xcc
        __emit 0x89
        __emit 0x11
        __emit 0x8b
        __emit 0x50
        __emit 0x04
        __emit 0x8b
        __emit 0x40
        __emit 0x08
        __emit 0x89
        __emit 0x51
        __emit 0x04
        __emit 0x89
        __emit 0x41
        __emit 0x08
        mov ecx, [xr_2]
        call xr_3
        __emit 0x85
        __emit 0xc0
        __emit 0x74
        __emit 0x06
        __emit 0xff
        __emit 0x88
        __emit 0x84
        __emit 0x00
        __emit 0x00
        __emit 0x00
        __emit 0xc2
        __emit 0x04
        __emit 0x00
    }
}
extern "C" __declspec(naked) void f_4() {
    __asm {
        __emit 0x8b
        __emit 0x44
        __emit 0x24
        __emit 0x04
        __emit 0x8b
        __emit 0x10
        __emit 0x83
        __emit 0xec
        __emit 0x0c
        __emit 0x8b
        __emit 0xcc
        __emit 0x89
        __emit 0x11
        __emit 0x8b
        __emit 0x50
        __emit 0x04
        __emit 0x8b
        __emit 0x40
        __emit 0x08
        __emit 0x89
        __emit 0x51
        __emit 0x04
        __emit 0x89
        __emit 0x41
        __emit 0x08
        mov ecx, [xr_2]
        call xr_3
        __emit 0x85
        __emit 0xc0
        __emit 0x74
        __emit 0x0d
        __emit 0x8b
        __emit 0x10
        __emit 0x8b
        __emit 0xc8
        __emit 0xff
        __emit 0x92
        __emit 0xcc
        __emit 0x00
        __emit 0x00
        __emit 0x00
        __emit 0xc2
        __emit 0x04
        __emit 0x00
        __emit 0x33
        __emit 0xc0
        __emit 0xc2
        __emit 0x04
        __emit 0x00
        __emit 0xcc
        __emit 0xcc
        __emit 0xcc
        __emit 0xcc
        __emit 0xcc
        __emit 0xcc
        __emit 0x8b
        __emit 0x44
        __emit 0x24
        __emit 0x04
        __emit 0x8a
        __emit 0x00
        __emit 0x3a
        __emit 0x41
        __emit 0x60
        __emit 0x74
        __emit 0x0a
        __emit 0x88
        __emit 0x41
        __emit 0x60
        __emit 0xc6
        __emit 0x81
        __emit 0x88
        __emit 0x00
        __emit 0x00
        __emit 0x00
        __emit 0x01
        __emit 0xc2
        __emit 0x04
        __emit 0x00
        __emit 0xcc
        __emit 0xcc
        __emit 0xcc
        __emit 0xcc
        __emit 0xcc
        __emit 0xcc
        __emit 0xcc
        __emit 0xcc
        __emit 0x8b
        __emit 0x44
        __emit 0x24
        __emit 0x04
        __emit 0x8a
        __emit 0x00
        __emit 0x3a
        __emit 0x41
        __emit 0x61
        __emit 0x74
        __emit 0x0a
        __emit 0x88
        __emit 0x41
        __emit 0x61
        __emit 0xc6
        __emit 0x81
        __emit 0x88
        __emit 0x00
        __emit 0x00
        __emit 0x00
        __emit 0x01
        __emit 0xc2
        __emit 0x04
        __emit 0x00
        __emit 0xcc
        __emit 0xcc
        __emit 0xcc
        __emit 0xcc
        __emit 0xcc
        __emit 0xcc
        __emit 0xcc
        __emit 0xcc
        __emit 0x8b
        __emit 0x44
        __emit 0x24
        __emit 0x04
        __emit 0x8b
        __emit 0x00
        __emit 0x3b
        __emit 0x41
        __emit 0x64
        __emit 0x74
        __emit 0x0a
        __emit 0x89
        __emit 0x41
        __emit 0x64
        __emit 0xc6
        __emit 0x81
        __emit 0x88
        __emit 0x00
        __emit 0x00
        __emit 0x00
        __emit 0x01
        __emit 0xc2
        __emit 0x04
        __emit 0x00
        __emit 0xcc
        __emit 0xcc
        __emit 0xcc
        __emit 0xcc
        __emit 0xcc
        __emit 0xcc
        __emit 0xcc
        __emit 0xcc
        __emit 0x8b
        __emit 0x44
        __emit 0x24
        __emit 0x04
        __emit 0x8b
        __emit 0x10
        __emit 0x3b
        __emit 0x51
        __emit 0x70
        __emit 0x75
        __emit 0x10
        __emit 0x8b
        __emit 0x50
        __emit 0x04
        __emit 0x3b
        __emit 0x51
        __emit 0x74
        __emit 0x75
        __emit 0x08
        __emit 0x8b
        __emit 0x50
        __emit 0x08
        __emit 0x3b
        __emit 0x51
        __emit 0x78
        __emit 0x74
        __emit 0x18
        __emit 0x8b
        __emit 0x10
        __emit 0x89
        __emit 0x51
        __emit 0x70
        __emit 0x8b
        __emit 0x50
        __emit 0x04
        __emit 0x89
        __emit 0x51
        __emit 0x74
        __emit 0x8b
        __emit 0x40
        __emit 0x08
        __emit 0x89
        __emit 0x41
        __emit 0x78
        __emit 0xc6
        __emit 0x81
        __emit 0x88
        __emit 0x00
        __emit 0x00
        __emit 0x00
        __emit 0x01
        __emit 0xc2
        __emit 0x04
        __emit 0x00
        __emit 0xcc
        __emit 0xcc
        __emit 0xcc
        __emit 0xcc
        __emit 0xcc
        __emit 0xcc
        __emit 0xcc
        __emit 0xcc
        __emit 0xcc
        __emit 0xcc
        __emit 0x8b
        __emit 0x44
        __emit 0x24
        __emit 0x04
        __emit 0x8b
        __emit 0x00
        __emit 0x3b
        __emit 0x41
        __emit 0x7c
        __emit 0x74
        __emit 0x0a
        __emit 0x89
        __emit 0x41
        __emit 0x7c
        __emit 0xc6
        __emit 0x81
        __emit 0x88
        __emit 0x00
        __emit 0x00
        __emit 0x00
        __emit 0x01
        __emit 0xc2
        __emit 0x04
        __emit 0x00
        __emit 0xcc
        __emit 0xcc
        __emit 0xcc
        __emit 0xcc
        __emit 0xcc
        __emit 0xcc
        __emit 0xcc
        __emit 0xcc
        __emit 0x8b
        __emit 0x44
        __emit 0x24
        __emit 0x04
        __emit 0x8b
        __emit 0x00
        __emit 0x3b
        __emit 0x81
        __emit 0x80
        __emit 0x00
        __emit 0x00
        __emit 0x00
        __emit 0x74
        __emit 0x0d
        __emit 0x89
        __emit 0x81
        __emit 0x80
        __emit 0x00
        __emit 0x00
        __emit 0x00
        __emit 0xc6
        __emit 0x81
        __emit 0x88
        __emit 0x00
        __emit 0x00
        __emit 0x00
        __emit 0x01
        __emit 0xc2
        __emit 0x04
        __emit 0x00
        __emit 0xcc
        __emit 0xcc
        mov eax, offset DAT_00675d98
        __emit 0xc3
    }
}
extern "C" __declspec(naked) void f_5() {
    __asm {
        __emit 0x8b
        __emit 0x44
        __emit 0x24
        __emit 0x08
        __emit 0x53
        __emit 0x56
        __emit 0x8b
        __emit 0xf1
        __emit 0x8b
        __emit 0x4c
        __emit 0x24
        __emit 0x0c
        __emit 0x50
        __emit 0x51
        __emit 0x8b
        __emit 0xce
        call xr_4
        __emit 0x33
        __emit 0xdb
        mov dword ptr [esi], offset xr_5
        __emit 0x89
        __emit 0x5e
        __emit 0x68
        __emit 0x89
        __emit 0x5e
        __emit 0x6c
        __emit 0x89
        __emit 0x5e
        __emit 0x78
        __emit 0x89
        __emit 0x5e
        __emit 0x74
        __emit 0x89
        __emit 0x5e
        __emit 0x70
        __emit 0xb0
        __emit 0x01
        __emit 0x88
        __emit 0x46
        __emit 0x60
        __emit 0x88
        __emit 0x46
        __emit 0x61
        __emit 0xc7
        __emit 0x46
        __emit 0x64
        __emit 0x04
        __emit 0x00
        __emit 0x00
        __emit 0x00
        __emit 0x8b
        __emit 0x46
        __emit 0x68
        cmp eax, offset xr_6
        __emit 0x74
        __emit 0x10
        __emit 0x3b
        __emit 0xc3
        __emit 0x74
        __emit 0x0c
        __emit 0x50
        call free
        __emit 0x83
        __emit 0xc4
        __emit 0x04
        __emit 0x89
        __emit 0x5e
        __emit 0x68
        __emit 0x8b
        __emit 0x46
        __emit 0x6c
        cmp eax, offset xr_6
        __emit 0x74
        __emit 0x10
        __emit 0x3b
        __emit 0xc3
        __emit 0x74
        __emit 0x0c
        __emit 0x50
        call free
        __emit 0x83
        __emit 0xc4
        __emit 0x04
        __emit 0x89
        __emit 0x5e
        __emit 0x6c
        __emit 0x33
        __emit 0xd2
        __emit 0x89
        __emit 0x56
        __emit 0x70
        __emit 0x33
        __emit 0xc0
        __emit 0x33
        __emit 0xc9
        __emit 0x89
        __emit 0x4e
        __emit 0x74
        __emit 0x89
        __emit 0x46
        __emit 0x78
        __emit 0x89
        __emit 0x5e
        __emit 0x7c
        __emit 0x89
        __emit 0x9e
        __emit 0x80
        __emit 0x00
        __emit 0x00
        __emit 0x00
        __emit 0x89
        __emit 0x9e
        __emit 0x84
        __emit 0x00
        __emit 0x00
        __emit 0x00
        __emit 0x88
        __emit 0x9e
        __emit 0x88
        __emit 0x00
        __emit 0x00
        __emit 0x00
        __emit 0x88
        __emit 0x9e
        __emit 0x89
        __emit 0x00
        __emit 0x00
        __emit 0x00
        __emit 0x8b
        __emit 0xc6
        __emit 0x5e
        __emit 0x5b
        __emit 0xc2
        __emit 0x08
        __emit 0x00
        mov eax, [xr_7]
        __emit 0xc3
        __emit 0xcc
        __emit 0xcc
        __emit 0xcc
        __emit 0xcc
        __emit 0xcc
        __emit 0xcc
        __emit 0xcc
        __emit 0xcc
        __emit 0xcc
        __emit 0xcc
        mov eax, [xr_7]
        mov eax, [eax*0x4+xr_8]
        __emit 0xc3
        __emit 0xcc
        __emit 0xcc
        __emit 0xcc
        mov eax, [xr_7]
        __emit 0x6b
        __emit 0xc0
        __emit 0x1e
        add eax, offset xr_9
        __emit 0xc3
    }
}
extern "C" __declspec(naked) void f_6() {
    __asm {
        __emit 0x33
        __emit 0xc0
        __emit 0xc3
    }
}
extern "C" __declspec(naked) void f_7() {
    __asm {
        __emit 0x8b
        __emit 0x44
        __emit 0x24
        __emit 0x04
        __emit 0x8b
        __emit 0x10
        __emit 0x56
        __emit 0x83
        __emit 0xec
        __emit 0x0c
        __emit 0x8b
        __emit 0xcc
        __emit 0x89
        __emit 0x11
        __emit 0x8b
        __emit 0x50
        __emit 0x04
        __emit 0x8b
        __emit 0x40
        __emit 0x08
        __emit 0x89
        __emit 0x51
        __emit 0x04
        __emit 0x89
        __emit 0x41
        __emit 0x08
        mov ecx, [xr_2]
        call xr_3
        __emit 0x8b
        __emit 0xf0
        __emit 0x85
        __emit 0xf6
        __emit 0x74
        __emit 0x28
        __emit 0x8b
        __emit 0x8e
        __emit 0x84
        __emit 0x00
        __emit 0x00
        __emit 0x00
        __emit 0x41
        __emit 0x8b
        __emit 0xc1
        __emit 0x85
        __emit 0xc0
        __emit 0x89
        __emit 0x8e
        __emit 0x84
        __emit 0x00
        __emit 0x00
        __emit 0x00
        __emit 0x7e
        __emit 0x15
        __emit 0x8b
        __emit 0x16
        __emit 0x8b
        __emit 0xce
        __emit 0xff
        __emit 0x92
        __emit 0xa8
        __emit 0x00
        __emit 0x00
        __emit 0x00
        __emit 0x84
        __emit 0xc0
        __emit 0x75
        __emit 0x07
        __emit 0x8b
        __emit 0xce
        call f_1
        __emit 0x8b
        __emit 0xc6
        __emit 0x5e
        __emit 0xc2
        __emit 0x04
        __emit 0x00
    }
}
extern "C" __declspec(naked) void f_8() {
    __asm {
        __emit 0x53
        __emit 0x8b
        __emit 0x5c
        __emit 0x24
        __emit 0x08
        __emit 0x56
        __emit 0x8b
        __emit 0xf1
        __emit 0x57
        __emit 0x8d
        __emit 0x7e
        __emit 0x68
        __emit 0x57
        __emit 0x8b
        __emit 0xcb
        call xr_10
        __emit 0x84
        __emit 0xc0
        __emit 0x74
        __emit 0x0f
        __emit 0x53
        __emit 0x8b
        __emit 0xcf
        call xr_11
        __emit 0xc6
        __emit 0x86
        __emit 0x88
        __emit 0x00
        __emit 0x00
        __emit 0x00
        __emit 0x01
        __emit 0x5f
        __emit 0x5e
        __emit 0x5b
        __emit 0xc2
        __emit 0x04
        __emit 0x00
    }
}
extern "C" __declspec(naked) void f_9() {
    __asm {
        __emit 0x53
        __emit 0x8b
        __emit 0x5c
        __emit 0x24
        __emit 0x08
        __emit 0x56
        __emit 0x8b
        __emit 0xf1
        __emit 0x57
        __emit 0x8d
        __emit 0x7e
        __emit 0x6c
        __emit 0x57
        __emit 0x8b
        __emit 0xcb
        call xr_10
        __emit 0x84
        __emit 0xc0
        __emit 0x74
        __emit 0x0f
        __emit 0x53
        __emit 0x8b
        __emit 0xcf
        call xr_11
        __emit 0xc6
        __emit 0x86
        __emit 0x88
        __emit 0x00
        __emit 0x00
        __emit 0x00
        __emit 0x01
        __emit 0x5f
        __emit 0x5e
        __emit 0x5b
        __emit 0xc2
        __emit 0x04
        __emit 0x00
    }
}
extern "C" __declspec(naked) void f_10() {
    __asm {
        __emit 0x56
        __emit 0x8b
        __emit 0xf1
        call xr_12
        __emit 0xf6
        __emit 0x44
        __emit 0x24
        __emit 0x08
        __emit 0x01
        __emit 0x74
        __emit 0x09
        __emit 0x56
        call xr_13
        __emit 0x83
        __emit 0xc4
        __emit 0x04
        __emit 0x8b
        __emit 0xc6
        __emit 0x5e
        __emit 0xc2
        __emit 0x04
        __emit 0x00
    }
}
extern "C" __declspec(naked) void f_11() {
    __asm {
        __emit 0x6a
        __emit 0xff
        push offset LAB_005ecbc1
        mov eax, fs:[_except_list]
        __emit 0x50
        mov fs:[_except_list], esp
        __emit 0x83
        __emit 0xec
        __emit 0x10
        __emit 0x8b
        __emit 0x44
        __emit 0x24
        __emit 0x28
        __emit 0x53
        __emit 0x56
        __emit 0x50
        __emit 0x8b
        __emit 0x44
        __emit 0x24
        __emit 0x30
        __emit 0x8d
        __emit 0x54
        __emit 0x24
        __emit 0x10
        __emit 0x33
        __emit 0xdb
        __emit 0x52
        __emit 0x89
        __emit 0x5c
        __emit 0x24
        __emit 0x10
        __emit 0xff
        __emit 0x50
        __emit 0x10
        __emit 0x8b
        __emit 0x74
        __emit 0x24
        __emit 0x28
        __emit 0x50
        __emit 0x8b
        __emit 0xce
        __emit 0xc7
        __emit 0x44
        __emit 0x24
        __emit 0x24
        __emit 0x01
        __emit 0x00
        __emit 0x00
        __emit 0x00
        mov dword ptr [esi], offset xr_14
        __emit 0x89
        __emit 0x5e
        __emit 0x08
        __emit 0x89
        __emit 0x5e
        __emit 0x04
        call xr_15
        __emit 0x8d
        __emit 0x4c
        __emit 0x24
        __emit 0x0c
        __emit 0xc7
        __emit 0x44
        __emit 0x24
        __emit 0x08
        __emit 0x01
        __emit 0x00
        __emit 0x00
        __emit 0x00
        __emit 0x88
        __emit 0x5c
        __emit 0x24
        __emit 0x20
        mov dword ptr [esp+0xc], offset xr_14
        call xr_16
        __emit 0x8b
        __emit 0x4c
        __emit 0x24
        __emit 0x18
        __emit 0x8b
        __emit 0xc6
        __emit 0x5e
        __emit 0x5b
        mov fs:[_except_list], ecx
        __emit 0x83
        __emit 0xc4
        __emit 0x1c
        __emit 0xc2
        __emit 0x0c
        __emit 0x00
    }
}
extern "C" __declspec(naked) void f_12() {
    __asm {
        __emit 0x8b
        __emit 0xc1
        __emit 0x33
        __emit 0xc9
        __emit 0x89
        __emit 0x48
        __emit 0x58
        __emit 0x89
        __emit 0x48
        __emit 0x5c
        __emit 0x89
        __emit 0x48
        __emit 0x60
        __emit 0x89
        __emit 0x48
        __emit 0x64
        __emit 0x89
        __emit 0x48
        __emit 0x68
        __emit 0x89
        __emit 0x48
        __emit 0x6c
        __emit 0x89
        __emit 0x08
        __emit 0x89
        __emit 0x48
        __emit 0x04
        __emit 0xc6
        __emit 0x40
        __emit 0x54
        __emit 0x01
        __emit 0xba
        __emit 0x00
        __emit 0x00
        __emit 0x00
        __emit 0x40
        __emit 0x89
        __emit 0x50
        __emit 0x74
        __emit 0x89
        __emit 0x50
        __emit 0x78
        __emit 0x89
        __emit 0x48
        __emit 0x08
        __emit 0x89
        __emit 0x48
        __emit 0x0c
        __emit 0x89
        __emit 0x48
        __emit 0x10
        __emit 0x89
        __emit 0x48
        __emit 0x14
        __emit 0x89
        __emit 0x48
        __emit 0x18
        __emit 0x89
        __emit 0x48
        __emit 0x48
        __emit 0x89
        __emit 0x48
        __emit 0x7c
        __emit 0xc3
    }
}
