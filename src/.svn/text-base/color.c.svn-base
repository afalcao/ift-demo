/*
    Copyright (C) <2010> <Alexandre Xavier Falcão and Thiago Vallin Spina>

    This file is part of IFT-demo.

    IFT-demo is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    IFT-demo is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with IFT-demo.  If not, see <http://www.gnu.org/licenses/>.

    please see full copyright in COPYING file.
    -------------------------------------------------------------------------

    written by A.X. Falcão <afalcao@ic.unicamp.br> and by T.V. Spina
    <tvspina@liv.ic.unicamp.br>, 2010

*/

#include "color.h"

void RGB2Lab(float* r, float* g, float * b)
{

    float tresh = 0.008856;

    float Xn  = 0.950456;
    float Yn  = 1.0;
    float Zn  = 1.088854;

    float r13    = 1.0/3.0;
    float r16116 = 16.0/116.0;
    float cnst   = 7.787;

    float R0 = (*r)/255.0;
    float G0 = (*g)/255.0;
    float B0 = (*b)/255.0;

    R0 = powf(R0,2.2); /// gamma
    G0 = powf(G0,2.2); /// gamma
    B0 = powf(B0,2.2); /// gamma

    // sRGB (d65) Matrix Cxr
    float X =  0.412453 * R0 + 0.357580 * G0 + 0.180423 * B0;
    float Y =  0.212671 * R0 + 0.715160 * G0 + 0.072169 * B0;
    float Z =  0.019334 * R0 + 0.119193 * G0 + 0.950227 * B0;

    float xr = X/Xn;
    float yr = Y/Yn;
    float zr = Z/Zn;

    float L, A, B;
    float xr0, yr0, zr0;

    if (xr > tresh)
    {
        xr0 = pow(xr, r13);
    }
    else
    {
        xr0 = cnst * xr + r16116;
    }

    if (yr > tresh)
    {
        yr0 = pow(yr, r13);
    }
    else
    {
        yr0 = cnst * yr + r16116;
    }

    if (zr > tresh)
    {
        zr0 = pow(zr, r13);
    }
    else
    {
        zr0 = cnst * zr + r16116;
    }

    if (yr > tresh)
    {
        L = 116.0 * pow(yr, r13) - 16.0;
    }
    else
    {
        L = 903.3 * yr;
    }

    A = 500.0 * (xr0 - yr0);
    B = 200.0 * (yr0 - zr0);

    *r = L;
    *g = A;
    *b = B;
}

void Lab2RGB(float* l, float* a, float* b)
{

    float thresh1 = 0.008856;
    float thresh2 = 0.206893;

    float Xn = 0.950456;
    float Yn = 1.000;
    float Zn = 1.088854;

    float r13    = 1.0/3.0;
    float r16116 = 16.0/116.0;
    float cnst   = 7.787;

    float X, Y, Z;
    float L = *l;
    float A = *a;
    float B = *b;

    float P = (L+16.0)/116.0;

    if (L > 7.9996)
    {
        Y = Yn * P * P * P;
    }
    else
    {
        Y = L / 903.3;
    }

    float yr = Y/Yn, fy;
    if ( yr > thresh1 )
    {
        fy = pow(yr, r13);
    }
    else
    {
        fy = cnst * yr + r16116;
    }

    float fx = A / 500.0 + fy;
    float fz = fy - B / 200.0;

    if (fx > thresh2 )
    {
        X = Xn * fx * fx * fx;
    }
    else
    {
        X = Xn/cnst * ( fx - r16116 );
    }

    if (fz > thresh2 )
    {
        Z = Zn * fz * fz * fz;
    }
    else
    {
        Z = Zn/cnst * ( fz - r16116 );
    }

    // sRGB (d65) Matrix Crx
    float R1 =   3.240479 * X - 1.537150 * Y - 0.498535 * Z;
    float G1 = - 0.969256 * X + 1.875992 * Y + 0.041556 * Z;
    float B1 =   0.055648 * X - 0.204043 * Y + 1.057311 * Z;

   R1 = powf(R1,1/2.2); /// gamma
   G1 = powf(G1,1/2.2); /// gamma
   B1 = powf(B1,1/2.2); /// gamma


    *l = MAX(0.0, MIN(255.0, 255.0*R1));
    *a = MAX(0.0, MIN(255.0, 255.0*G1));
    *b = MAX(0.0, MIN(255.0, 255.0*B1));


}


