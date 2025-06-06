/*
 *  L3Def.h, part of the L3 project for handling LDraw *.dat files
 *  Copyright (C) 1997-2000  Lars C. Hassing (lch@ccieurope.com)
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

/******************************************************************************
  Please do not edit this file. In stead contact me (lch@ccieurope.com)
  to get your changes integrated and you will receive an up-to-date version.
******************************************************************************/

/* L3Def.h Header for internal datastructures and functions for L3 program */
/*
970918 lch First version, both for TurboC 2.0 and Visual C++ 5.0
980926 lch Version released for ldlite
981005 lch Support for MPD files
981226 lch Added BBoxES: BBox exclusive studs
990212 lch Added support for TRANSLATE, ROTATE, SCALE, TRANSFORM meta commands
990324 lch Default Part Color added
990405 lch Added M4V3MulW
990419 lch Prepare for L3P32, changing #ifdef __TURBOC__ to #ifdef L3P
990520 lch STUDS revisited
990603 lch ModelTitle and ModelAuthor added
991030 lch MAX_PARTS increased to 2000 if not TurboC
991130 lch Added LineNo to L3LineS if not TurboC
000317 lch Recursion check added
*/

#ifndef L3DEF_INCLUDED
#define L3DEF_INCLUDED

#include <stdio.h>

#ifdef L3P
typedef unsigned short WORD;
typedef unsigned int UINT;
typedef unsigned char BYTE;
typedef int          INT;
typedef long         LONG;
typedef unsigned long DWORD;
typedef DWORD        COLORREF;
#define RGB(r,g,b)          ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))
#define GetRValue(rgb)      ((BYTE)(rgb))
#define GetGValue(rgb)      ((BYTE)(((WORD)(rgb)) >> 8))
#define GetBValue(rgb)      ((BYTE)((rgb)>>16))
#ifdef __TURBOC__
#ifdef UNIX
#define BACKSLASH_CHAR   '/'
#define BACKSLASH_STR    "/"
#else
#define BACKSLASH_CHAR   '\\'
#define BACKSLASH_STR    "\\"
#define  _strlwr  strlwr
#define  malloc   farmalloc
#define  realloc  farrealloc
#define  free     farfree
#endif
#endif
#define true   1
#define TRUE   1
#define false  0
#define FALSE  0

extern FILE         *LogFp;
extern FILE         *PovFp;
#ifdef __TURBOC__
extern unsigned long farcoreleft0;
#endif
extern void          CheckMemoryUsage(void);
#else
#define L3_UPDATEVIEW_NEWMODEL 37913      /* To avoid OnInitialUpdate()      */
#endif

#define MAX_DATA_LEN 1024
#define L3_ARRAY_COUNT(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

#ifdef USE_OPENGL
#ifndef false
#define false 0
#endif
#ifndef true
#define true 1
#endif
#endif

#ifndef _MAX_PATH
#define _MAX_PATH 260
#endif

struct L3LineS                            /* Not too economic with memory... */
{
   int                  LineType;
   int                  Color;
#ifndef L3P
   int                  RandomColor;
#endif
#ifndef __TURBOC__
   int                  LineNo;           /* Used for identical lines check  */
#endif
   float                v[4][4];          /* 2-4 vectors or matrix           */
   struct L3LineS      *NextLine;         /* Next line in chain              */
   struct L3PartS      *PartPtr;          /* LineType 1 uses this part       */
   char                *Comment;          /* Text after ' ' after '0'        */
};

struct L3PartS
{
   char                *DatName;          /* Full or rel.path, w or w/o .DAT */
   struct L3LineS      *FirstLine;        /* Start of chain of lines         */
   int                  nPovObjects;      /* Number of subparts+meshes       */
   int                  nPovObjectsES;    /* Number excluding studs          */
   unsigned int         nStudDotDats;
   long                 FrameLevelObjects;
   float                BBox[2][3];       /* Bounding box[min..max][x..z]    */
#ifndef L3P
   float                BBoxES[2][3];     /* Bounding box Exclusive Studs    */
#endif
   /* FileRead and Resolved are private for L3Input */
   unsigned int         FileRead:1;       /* File has been read into memory  */
   unsigned int         FromPARTS:1;      /* File came from PARTS directory  */
   unsigned int         FromP:1;          /* File came from P directory      */
   unsigned int         Resolved:1;       /* All subparts read and resolved  */
   unsigned int         Recursion:1;      /* Used for recursion check        */
   unsigned int         Empty:1;
   unsigned int         Investigated:1;
#ifndef USE_OPENGL
   unsigned int         Internal:1;       /* For transforms, not in Parts[]  */
   unsigned int         IsStud:1;         /* The part is a stud              */
#else
   unsigned int         Internal:3;       /* For transforms, not in Parts[]  */
   unsigned int         IsStud:1;         /* The part is a stud              */
   unsigned int         IsMPD:1;          /* This is an MPD internal file    */
#endif
};
struct L3PartInternalS
{
   struct L3PartS       Core;
   struct L3PartS      *Father;           /* Backpointer                     */
   struct L3LineS     **LinePtrPtr;       /* Saved value for father          */
   int                  ExpectedEnd;      /* Saved value for father          */
};

struct L3ColorS
{
   int                  Color;
   unsigned int         Printed:1;
};

struct L3LightS
{
   float                Position[4];      /* Lightsource La,Lo,r or x,y,z    */
   float                LightColor[4];
   unsigned int         Globe:1;
};

struct L3PovS
{
   float                CameraPos[4];     /* Camera La,Lo,r or x,y,z         */
   float                BackgroundColor[4];
   int                  DefaultPartColorNumber;
   float                DefaultPartColor[4];
   float                CameraAngle;
   float                SeamWidth;
   float                FloorY;
   int                  FloorType;
   int                  Quality;
   int                  Debug;
   unsigned int         Globe:1;
   unsigned int         FloorYspecified:1;
   unsigned int         UsePovParts:1;
   unsigned int         UseDefaultLights:1;  /* No matter if any light
                                                defined                      */
   unsigned int         Bumps:1;
};

struct PovPartS
{
   char                *DatName;
   int                  FrameLevelObjects;
   float                BBox[2][3];
   char                *PovData;
};

struct L3StudStylePrimitive
{
    char Name[50];
    char Data[MAX_DATA_LEN];
    char DataLogo1[MAX_DATA_LEN];
};

/*
struct msBITMAPINFOHEADER{ // bmih
   DWORD  biSize;
   LONG   biWidth;
   LONG   biHeight;
   WORD   biPlanes;
   WORD   biBitCount
   DWORD  biCompression;
   DWORD  biSizeImage;
   LONG   biXPelsPerMeter;
   LONG   biYPelsPerMeter;
   DWORD  biClrUsed;
   DWORD  biClrImportant;
};
*/

struct L3AreaS
{
   int                  xmin;
   int                  xmax;
   int                  ymin;
   int                  ymax;
};


struct L3CanvasS
{
   void                *dib;              /* Implementation dependent bitmap */
   unsigned char       *bgr;              /* Start of first row: B G R B G.. */
   union
   {
      int                 *I;            /* Integer Z buffer WxH */
      float               *F;            /* Float Z buffer WxH */
   } ZBuffer;
   int                  Width;
   int                  Width4;
   int                  Height;
   struct L3AreaS       Cleared;          /* Area from previous drawing      */
   struct L3AreaS       Dirty;            /* Area recently drawn             */
   struct L3AreaS       Painted;          /* Area of bitmap in use           */
};

struct L3StatS
{
   int                  timeLoadModel;    /* Milliseconds to load model      */
   int                  timeDrawModel;    /* Milliseconds to draw model      */
   int                  timeCalcModel;    /* Milliseconds to calc model      */
   int                  nLineType[6];     /* Total number in drawing         */
   char                 Str[200];         /* Mainly for debug messages       */
};

#ifdef USE_OPENGL
#define MAX_COLORS 4096
#else
#define MAX_COLORS 200
#endif
#ifdef __TURBOC__
#define MAX_PARTS  880
#else
#define MAX_PARTS  8000
#endif
extern int           DoCheck;
extern int           ReadLineTypeMask;
extern char          LDrawDir[_MAX_PATH]; /* Directory of P, PARTS, MODELS   */
extern char          ModelDir[_MAX_PATH]; /* Directory of model              */
extern char         *Dirs[];              /* "\\P\\" "\\Parts\\" "\\Models\\" */
extern struct L3ColorS Colors[MAX_COLORS];
extern int           nColors;
extern struct L3LightS *Lights;
extern int           nLights;
extern struct L3PartS Parts[MAX_PARTS];
extern int           nParts;
extern struct L3PovS L3Pov;
extern struct L3StatS L3Stat;
extern int           WarningLevel;
extern float         DetThreshold;
extern float         DistThreshold;
extern int           ShowAllDists;
extern int           LightDotDat;
extern int           Color24[16];
extern COLORREF      Rgbs[];
#ifndef L3P
extern char          ModelTitle[400];     /* Title of model file             */
extern char          ModelAuthor[400];    /* Author of model file            */
#endif

#define BIT(i)     (1<<(i))

/* L3Math.cpp */
#define V3Load(r, x, y, z) r[0]=(x);r[1]=(y);r[2]=(z)
#define V4Load(r, x, y, z, t) r[0]=(x);r[1]=(y);r[2]=(z);r[3]=(t)
/* r = a */
extern void          V3Assign(float r[4], float a[4]);
/* returns = a . b */
extern double        V3Dot(float a[4], float b[4]);
/* returns |a| */
extern double        V3Length(float a[4]);
/* r = a / |a|  returns 0 if OK, 1 if error (len==0) */
extern int           V3Unit(float r[4], float a[4]);
/* r = (a + b)/2 */
extern void          V3Mean(float r[4], float a[4], float b[4]);
/* r = a + t*b */
extern void          V3AddScaled(float r[4], float a[4], double t, float b[4]);
/* r = a - b */
extern void          V3Sub(float r[4], float a[4], float b[4]);
/* r = a x b */
extern void          V3Cross(float r[4], float a[4], float b[4]);
/* returns det(m), m is 3x3 (3x4) matrix */
extern double        M3Det(float m[3][4]);
/* r = m * p */
extern void          M4V4Mul(float r[4], float m[4][4], float p[4]);
/* r = m * p, uses only p[0..2], assumes m[][3]==1, returns only r[0..2] */
extern void          M4V3Mul(float r[4], float m[4][4], float p[4]);
/* r = m * p, uses only p[0..2], adjusts for W value, returns only r[0..2] */
extern void          M4V3MulW(float r[4], float m[4][4], float p[4]);
/* r = a * b */
extern void          M4M4Mul(float r[4][4], float a[4][4], float b[4][4]);
/* Dir1 becomes unit vector pointing towards center of globe */
extern void          globe2dir(float Latitude, float Longitude, float Dir1[4]);
extern void          CheckPointAgainstBBox(float r[4], float BBox[2][3]);
extern void          ludcmp(float a[8][8], int n, int indx[8], float *d);
extern void          lubksb(float a[8][8], int n, int indx[8], float b[8]);

/* L3Input.cpp */
extern void          DeleteTrailingBackslash(char *Str);
extern int           InitLDrawDir(void);
extern void          CalcPartBBox(struct L3PartS * PartPtr, int DoBBox, int DoCamera);
extern FILE         *OpenDatFile(char *DatName);
extern char         *GetOpenDatFilePath(void);
extern void          LoadModelPre(void);
extern int           LoadModel(const char *lpszPathName);
extern void          LoadModelPost(void);
extern void          FreeParts(void);
extern struct L3LightS *AddLight(void);
extern void          FreeLights(void);
extern int           SaveLine(struct L3LineS *** LinePtrPtrPtr,
                              struct L3LineS * Data, char *Comment);
extern void         GetLDrawSearchDirs(int *ErrorCode);
extern FILE        *GetStudStyleFile(char *DatName, int open_stud);
extern int          IsStudStylePrimitive(const char* FileName);

/* Expose to Main.c and L3Input.c */
extern int           stud_style;      // there are seven stud styles including 5 stud logos
extern int           stud_cylinder_color_enabled;
extern char          ldconfig[256];
extern char          ldconfigfilename[256];

/* L3PoV.cpp */
extern struct PovPartS *FindPovPart(char *DatName);
extern void          PrintPovPartsList(FILE *fp);
#ifdef L3P
extern int           MakePovPartPovfile(void);
#endif
extern void          PrintPovPartOld(FILE *fp, struct L3PartS * PartPtr, long CurColor, float m[4][4]);
extern void          PrintPovPart(FILE *fp, struct L3PartS * PartPtr);
extern int           Color2rgb(int Color, float rgb[]);
extern void          PrintPovHeader(FILE *fp, char *ProgVers, char *InputFullPath);
extern void          PrintPovTrailer(FILE *fp);
extern void          CalcCameraAddPoint(float BBcorner[4]);

/* L3Stat.cpp */
typedef long         L3Time_t;

extern L3Time_t      L3GetTime(void);

#define L3MALLOC_LINETYPE0   0
#define L3MALLOC_LINETYPE1   1
#define L3MALLOC_LINETYPE2   2
#define L3MALLOC_LINETYPE3   3
#define L3MALLOC_LINETYPE4   4
#define L3MALLOC_LINETYPE5   5
#define L3MALLOC_STRINGS     6            /* DatNames                        */
#define L3MALLOC_INTPART     7
#define L3MALLOC_LIGHTS      8
#define L3MALLOC_ZBUFFER     9
#define L3MALLOC_DIB        10
#define L3MALLOC_MAXUSAGE   11
extern void         *L3Malloc(int Usage, unsigned long Size);
extern void         *L3Realloc(int Usage, void *OldMemBlock, unsigned long OldSize, unsigned long NewSize);
extern char         *L3Strdup(int Usage, const char *Str);
extern void          L3Free(int Usage, void *MemBlock, unsigned long Size);
extern int           L3Logging;
extern void          L3Log(char *format,...);

extern void FixDatName(register char *DatName);
extern int  LoadPart(struct L3PartS *PartPtr,
              int IsModel,
              char *ReferencingDatfile);

#endif
