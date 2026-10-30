// Minimal stand-in for the DirectX 8 SDK's d3dx8math.h: the D3DX types and
// functions the engine uses, with the SDK's inline definitions. D3DX itself
// is linked from the prebuilt d3dx8.lib.
#ifndef TJ_D3DX8_H
#define TJ_D3DX8_H

#include <d3d8.h>

// As the SDK has it for Visual C++ 6 and later.
#define D3DXINLINE __forceinline

struct D3DXVECTOR3 : public D3DVECTOR
{
    D3DXVECTOR3() {}
    D3DXINLINE D3DXVECTOR3(float fx, float fy, float fz)
    {
        x = fx;
        y = fy;
        z = fz;
    }
};

struct D3DXMATRIX : public D3DMATRIX
{
    D3DXMATRIX() {}
};

struct D3DXPLANE
{
    D3DXPLANE() {}
    float a, b, c, d;
};

struct D3DXCOLOR
{
    D3DXCOLOR() {}
    D3DXINLINE D3DXCOLOR(float fr, float fg, float fb, float fa)
    {
        r = fr;
        g = fg;
        b = fb;
        a = fa;
    }

    D3DXINLINE operator DWORD() const
    {
        DWORD dwR = r >= 1.0f ? 0xff : r <= 0.0f ? 0x00 : (DWORD)(r * 255.0f + 0.5f);
        DWORD dwG = g >= 1.0f ? 0xff : g <= 0.0f ? 0x00 : (DWORD)(g * 255.0f + 0.5f);
        DWORD dwB = b >= 1.0f ? 0xff : b <= 0.0f ? 0x00 : (DWORD)(b * 255.0f + 0.5f);
        DWORD dwA = a >= 1.0f ? 0xff : a <= 0.0f ? 0x00 : (DWORD)(a * 255.0f + 0.5f);

        return (dwA << 24) | (dwR << 16) | (dwG << 8) | dwB;
    }

    float r, g, b, a;
};

D3DXINLINE D3DXMATRIX *D3DXMatrixIdentity(D3DXMATRIX *pOut)
{
    pOut->m[0][1] = pOut->m[0][2] = pOut->m[0][3] =
    pOut->m[1][0] = pOut->m[1][2] = pOut->m[1][3] =
    pOut->m[2][0] = pOut->m[2][1] = pOut->m[2][3] =
    pOut->m[3][0] = pOut->m[3][1] = pOut->m[3][2] = 0.0f;

    pOut->m[0][0] = pOut->m[1][1] = pOut->m[2][2] = pOut->m[3][3] = 1.0f;
    return pOut;
}

extern "C" {
D3DXPLANE *WINAPI D3DXPlaneFromPointNormal(D3DXPLANE *pOut, const D3DXVECTOR3 *pPoint, const D3DXVECTOR3 *pNormal);
}

#endif
