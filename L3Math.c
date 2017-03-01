/*
 *  L3Math.cpp, part of the L3 project for handling LDraw *.dat files
 *  Copyright (C) 1997-1999  Lars C. Hassing (lch@ccieurope.com)
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/*****************************************************************************
  Please do not edit this file. In stead contact me (lch@ccieurope.com)
  to get your changes integrated and you will receive an up-to-date version.
*****************************************************************************/

/* L3Math.cpp  Math routines for L3 program */
/*
970918 lch First version, both for TurboC 2.0 and Visual C++ 5.0
980217 lch More routines added
980427 lch Most routines renamed
980926 lch Version released for ldlite
980930 lch M_PI introduced
990218 lch V3Unit now returns 1 if error (len==0)
990405 lch Added M4V3MulW
990706 lch Improved speed by using multiplications rather than divisions
990801 lch Improved speed by unrolling M4M4Mul
*/

#ifdef USE_OPENGL
#include "math.h"
//extern "C" {
#endif

#ifdef USE_OPENGL
// Gotta prepare for case sensitive file systems.
#include "StdAfx.h"
#else
#include "stdafx.h"
#endif
#include "stdio.h"
#include "stdlib.h"
#include "L3Def.h"
#include "math.h"

#ifndef M_PI
#define M_PI		3.14159265358979323846
#endif

/* #define V3Load(r, x, y, z) r[0]=(x);r[1]=(y);r[2]=(z) */

/* r = a */
void                 V3Assign(float r[4], float a[4])
{
   r[0] = a[0];
   r[1] = a[1];
   r[2] = a[2];
}

/* returns = a . b */
double               V3Dot(float a[4], float b[4])
{
   return (a[0] * b[0] + a[1] * b[1] + a[2] * b[2]);
}

/* returns |a| */
double               V3Length(float a[4])
{
   /* return (sqrt(V3Dot(a, a))); */
   return (sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]));
}

/* r = a / |a|  returns 0 if OK, 1 if error (len==0) */
int                  V3Unit(float r[4], float a[4])
{
   double               l;

/*
   l = V3Length(a);
   if (l == 0.0)
      return(1);
   r[0] = a[0] / l;
   r[1] = a[1] / l;
   r[2] = a[2] / l;
*/
   l = sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]);
   if (l == 0.0)
      return(1);
   l = 1.0 / l;
   r[0] = a[0] * l;
   r[1] = a[1] * l;
   r[2] = a[2] * l;
   return(0);
}

#ifdef V3ADD_USED
/* r = a + b */
void                 V3Add(float r[4], float a[4], float b[4])
{
   r[0] = a[0] + b[0];
   r[1] = a[1] + b[1];
   r[2] = a[2] + b[2];
}
#endif

/* r = (a + b)/2 */
void                 V3Mean(float r[4], float a[4], float b[4])
{
   r[0] = (a[0] + b[0]) / 2.0;
   r[1] = (a[1] + b[1]) / 2.0;
   r[2] = (a[2] + b[2]) / 2.0;
}

/* r = a + t*b */
void                 V3AddScaled(float r[4], float a[4], double t, float b[4])
{
   r[0] = a[0] + t * b[0];
   r[1] = a[1] + t * b[1];
   r[2] = a[2] + t * b[2];
}

/* r = a - b */
void                 V3Sub(float r[4], float a[4], float b[4])
{
   r[0] = a[0] - b[0];
   r[1] = a[1] - b[1];
   r[2] = a[2] - b[2];
}

/* r = a x b */
void                 V3Cross(float r[4], float a[4], float b[4])
{
   r[0] = a[1] * b[2] - a[2] * b[1];
   r[1] = a[2] * b[0] - a[0] * b[2];
   r[2] = a[0] * b[1] - a[1] * b[0];
}

/* returns det(m), m is 3x3 (3x4) matrix */
double               M3Det(float m[3][4]) /* Note argument type !            */
{
   return (m[0][0] * (m[1][1] * m[2][2] - m[2][1] * m[1][2]) +
           m[1][0] * (m[2][1] * m[0][2] - m[0][1] * m[2][2]) +
           m[2][0] * (m[0][1] * m[1][2] - m[1][1] * m[0][2]));
}

/* r = m * p */
void                 M4V4Mul(float r[4], float m[4][4], float p[4])
{
   register int         i,
                        j;
   double               d;

   for (j = 0; j < 4; j++)
   {
      d = 0.0;
      for (i = 0; i < 4; i++)
         d += m[j][i] * p[i];
      r[j] = d;
   }
}

/* r = m * p, uses only p[0..2], assumes m[][3]==1, returns only r[0..2] */
void                 M4V3Mul(float r[4], float m[4][4], float p[4])
{
   r[0] = m[0][0] * p[0] + m[0][1] * p[1] + m[0][2] * p[2] + m[0][3];
   r[1] = m[1][0] * p[0] + m[1][1] * p[1] + m[1][2] * p[2] + m[1][3];
   r[2] = m[2][0] * p[0] + m[2][1] * p[1] + m[2][2] * p[2] + m[2][3];
}

/* r = m * p, uses only p[0..2], adjusts for W value, returns only r[0..2] */
void                 M4V3MulW(float r[4], float m[4][4], float p[4])
{
   double W;
   
/*
   W    =  m[3][0] * p[0] + m[3][1] * p[1] + m[3][2] * p[2] + m[3][3];
   r[0] = (m[0][0] * p[0] + m[0][1] * p[1] + m[0][2] * p[2] + m[0][3]) / W;
   r[1] = (m[1][0] * p[0] + m[1][1] * p[1] + m[1][2] * p[2] + m[1][3]) / W;
   r[2] = (m[2][0] * p[0] + m[2][1] * p[1] + m[2][2] * p[2] + m[2][3]) / W;
*/
   W    =  1.0 / (m[3][0] * p[0] + m[3][1] * p[1] + m[3][2] * p[2] + m[3][3]);
   r[0] = (m[0][0] * p[0] + m[0][1] * p[1] + m[0][2] * p[2] + m[0][3]) * W;
   r[1] = (m[1][0] * p[0] + m[1][1] * p[1] + m[1][2] * p[2] + m[1][3]) * W;
   r[2] = (m[2][0] * p[0] + m[2][1] * p[1] + m[2][2] * p[2] + m[2][3]) * W;
}

/* r = a * b */
void                 M4M4Mul(float r[4][4], float a[4][4], float b[4][4])
{
/*
   register int         i,
                        j,
                        k;
   double               d;

   for (j = 0; j < 4; j++)
   {
      for (i = 0; i < 4; i++)
      {
         d = 0.0;
         for (k = 0; k < 4; k++)
            d += a[j][k] * b[k][i];
         r[j][i] = d;
      }
   }
*/
   r[0][0] = a[0][0] * b[0][0] + a[0][1] * b[1][0] + a[0][2] * b[2][0] + a[0][3] * b[3][0];
   r[0][1] = a[0][0] * b[0][1] + a[0][1] * b[1][1] + a[0][2] * b[2][1] + a[0][3] * b[3][1];
   r[0][2] = a[0][0] * b[0][2] + a[0][1] * b[1][2] + a[0][2] * b[2][2] + a[0][3] * b[3][2];
   r[0][3] = a[0][0] * b[0][3] + a[0][1] * b[1][3] + a[0][2] * b[2][3] + a[0][3] * b[3][3];

   r[1][0] = a[1][0] * b[0][0] + a[1][1] * b[1][0] + a[1][2] * b[2][0] + a[1][3] * b[3][0];
   r[1][1] = a[1][0] * b[0][1] + a[1][1] * b[1][1] + a[1][2] * b[2][1] + a[1][3] * b[3][1];
   r[1][2] = a[1][0] * b[0][2] + a[1][1] * b[1][2] + a[1][2] * b[2][2] + a[1][3] * b[3][2];
   r[1][3] = a[1][0] * b[0][3] + a[1][1] * b[1][3] + a[1][2] * b[2][3] + a[1][3] * b[3][3];

   r[2][0] = a[2][0] * b[0][0] + a[2][1] * b[1][0] + a[2][2] * b[2][0] + a[2][3] * b[3][0];
   r[2][1] = a[2][0] * b[0][1] + a[2][1] * b[1][1] + a[2][2] * b[2][1] + a[2][3] * b[3][1];
   r[2][2] = a[2][0] * b[0][2] + a[2][1] * b[1][2] + a[2][2] * b[2][2] + a[2][3] * b[3][2];
   r[2][3] = a[2][0] * b[0][3] + a[2][1] * b[1][3] + a[2][2] * b[2][3] + a[2][3] * b[3][3];

   r[3][0] = a[3][0] * b[0][0] + a[3][1] * b[1][0] + a[3][2] * b[2][0] + a[3][3] * b[3][0];
   r[3][1] = a[3][0] * b[0][1] + a[3][1] * b[1][1] + a[3][2] * b[2][1] + a[3][3] * b[3][1];
   r[3][2] = a[3][0] * b[0][2] + a[3][1] * b[1][2] + a[3][2] * b[2][2] + a[3][3] * b[3][2];
   r[3][3] = a[3][0] * b[0][3] + a[3][1] * b[1][3] + a[3][2] * b[2][3] + a[3][3] * b[3][3];
}

/* Dir1 becomes unit vector pointing towards center of globe */
void                 globe2dir(float Latitude, float Longitude, float Dir1[4])
{
   double               rLatitude;
   double               rLongitude;
   double               cosLatitude;

   rLongitude = (M_PI/180.0) * Longitude;
   rLatitude = (M_PI/180.0) * Latitude;
   cosLatitude = cos(rLatitude);
   Dir1[0] = -sin(rLongitude) * cosLatitude;
   Dir1[1] = sin(rLatitude);
   Dir1[2] = cos(rLongitude) * cosLatitude;
}

void          CheckPointAgainstBBox(float r[4], float BBox[2][3])
{
   if (r[0] < BBox[0][0])
      BBox[0][0] = r[0];
   if (r[0] > BBox[1][0])
      BBox[1][0] = r[0];
   if (r[1] < BBox[0][1])
      BBox[0][1] = r[1];
   if (r[1] > BBox[1][1])
      BBox[1][1] = r[1];
   if (r[2] < BBox[0][2])
      BBox[0][2] = r[2];
   if (r[2] > BBox[1][2])
      BBox[1][2] = r[2];
}

#ifdef USE_OPENGL
//}
#endif /* __cplusplus */
