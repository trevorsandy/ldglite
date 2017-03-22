#include "math.h"

#ifdef USE_OPENGL
// Gotta prepare for case sensitive file systems.
#include "StdAfx.h"
#else
#include "stdafx.h"
#endif

#include "stdio.h"
#include "stdlib.h"
#include "L3Def.h"
//#include "math.h"
#include "string.h"

#include "platform.h"

#ifndef false
#define false 0
#endif

extern int Draw1PartPtr(struct L3LineS *LinePtr, int Color);
extern struct L3PartS *FindPart(int Internal, char *DatName);
char                *Strdup(const char *Str);

extern char datfilename[256];
extern float m_m[4][4];

static struct L3LineS *SelectedLinePtr = NULL;

static struct L3PartS Parts0 = {0};

/*****************************************************************************/
int RestorePart0(void)
{
  memcpy(&Parts[0], &Parts0, sizeof(struct L3PartS));
  memset(&Parts0, 0, sizeof(struct L3PartS));
}

/*****************************************************************************/
int StashPart0(void)
{
  if (Parts0.DatName)
  {
    RestorePart0();
	// fix osx error: non-void function 'edit_mode_gui' should return a value
    return 0;
  }
  memcpy(&Parts0, &Parts[0], sizeof(struct L3PartS));
  memset(&Parts[0], 0, sizeof(struct L3PartS));
}

/*****************************************************************************/
int Find1Part(int partnum)
{
    int            i = 0;
    struct L3LineS *LinePtr;

    if (partnum <= 0)
      return 0;

    for (LinePtr = Parts[0].FirstLine; LinePtr; LinePtr = LinePtr->NextLine)
    {
	if (i == partnum)
	    return i;
	i++;
    }
    i--; // not found, return last piece in list.
      
    if (i < 0)
      i = 0;

    return i;
}

/*****************************************************************************/
int Select1Part(int partnum)
{
    int            i = 0;
    struct L3LineS *LinePtr;
    struct L3LineS *PrevPtr;

    PrevPtr = NULL;
    for (LinePtr = Parts[0].FirstLine; LinePtr; LinePtr = LinePtr->NextLine)
    {
	if (i == partnum)
	    break;
	PrevPtr = LinePtr;
	i++;
    }
    if (!LinePtr)
	return -1; //partnum not found

    if (PrevPtr)
    {
	PrevPtr->NextLine = LinePtr->NextLine;
    }
    else
    {
	Parts[0].FirstLine = LinePtr->NextLine;
    }
    if (SelectedLinePtr)
    {
	// Gotta free the old Selected line to avoid memleaks
	// Should also renumber lines whenever we free one.
    }
    SelectedLinePtr = LinePtr;
    return i;
}

/*****************************************************************************/
int UnSelect1Part(int partnum)
{
    int            i = 0;
    struct L3LineS *LinePtr;
    struct L3LineS *PrevPtr;

    if (!SelectedLinePtr)
      return -1;

    PrevPtr = NULL;
    for (LinePtr = Parts[0].FirstLine; LinePtr; LinePtr = LinePtr->NextLine)
    {
	if (i == partnum)
	    break;	    // Found the part to insert before
	PrevPtr = LinePtr;
	i++;
    }

    if (PrevPtr)
    {
      SelectedLinePtr->NextLine = PrevPtr->NextLine;
      PrevPtr->NextLine = SelectedLinePtr;
    }
    else
    {
      SelectedLinePtr->NextLine = Parts[0].FirstLine;
      Parts[0].FirstLine = SelectedLinePtr;
    }

    SelectedLinePtr = NULL;
    return i;
}

/*****************************************************************************/
int Delete1Part(int partnum)
{
    if (!SelectedLinePtr)
      return -1;
    free(SelectedLinePtr);
    SelectedLinePtr = NULL;
    UNUSED(partnum);

    return 0;
}

/*****************************************************************************/
int Get1PartPos(int partnum, float m[4][4])
{
    int            i = 0;
    struct L3LineS *LinePtr;

    if (SelectedLinePtr)
	LinePtr = SelectedLinePtr;

    else 
      for (LinePtr = Parts[0].FirstLine; LinePtr; LinePtr = LinePtr->NextLine)
    {
	if (i == partnum)
	    break;	    // Found the part
	i++;
    }

    if (!LinePtr)
	return 0; //partnum not found

    memcpy(m, LinePtr->v, sizeof(LinePtr->v));

    return 1;
}

/*****************************************************************************/
int Move1Part(int partnum, float m[4][4], int premult)
{
    float          r[4], m1[4][4];
    int            i = 0;
    struct L3LineS *LinePtr;
    float x, y, z;
    
    if (SelectedLinePtr)
	LinePtr = SelectedLinePtr;

    else for (LinePtr = Parts[0].FirstLine; LinePtr; LinePtr = LinePtr->NextLine)
    {
	if (i == partnum)
	    break;
	i++;
    }
    if (!LinePtr)
	return 0; //partnum not found

    switch (LinePtr->LineType)
    {
    case 0:
	break;
    case 1:
        if (premult == 1)
	{
	  M4M4Mul(m1,LinePtr->v,m);
	}
	else if (premult == 2)
	{
          x = LinePtr->v[0][3]; // Save the original origin
	  y = LinePtr->v[1][3];
	  z = LinePtr->v[2][3];
	  LinePtr->v[0][3] = 0; // Rotate around origin (0,0,0)
	  LinePtr->v[1][3] = 0;
	  LinePtr->v[2][3] = 0;
	  M4M4Mul(m1,m,LinePtr->v);
	  m1[0][3] = x;         // Restore the original origin
	  m1[1][3] = y;
	  m1[2][3] = z;
	}
	else
	{
          x = m[0][3]; // Save the original origin
	  y = m[1][3];
	  z = m[2][3];
	  m[0][3] = m[1][3] = m[2][3] = 0;
          LinePtr->v[0][3] -= x; 
	  LinePtr->v[1][3] -= y;
	  LinePtr->v[2][3] -= z;
	  M4M4Mul(m1,m,LinePtr->v);
	  m1[0][3] += x;         // Restore the original origin
	  m1[1][3] += y;
	  m1[2][3] += z;
	}
	//LinePtr->v = m1;
        memcpy(LinePtr->v, m1, sizeof(LinePtr->v));
	break;
    case 2:
    case 3:
    case 4:
    case 5:
        if (premult == 2) // First point of primitive = origin.
	{
		x = LinePtr->v[0][0]; // Save the original origin
		y = LinePtr->v[0][1];
		z = LinePtr->v[0][2];
	}
	for (i=0; i<LinePtr->LineType; i++)
	{
	    if (premult == 2)
	    {
		LinePtr->v[i][0]-=x; // Move Origin to (0,0,0)
		LinePtr->v[i][1]-=y;
		LinePtr->v[i][2]-=z;
	    }
	    if (premult)
	    {
		M4V3Mul(r,m,LinePtr->v[i]);
		LinePtr->v[i][0]=r[0];
		LinePtr->v[i][1]=r[1];
		LinePtr->v[i][2]=r[2];
	    }
	    else
	    {
		M4V3Mul(r,m,LinePtr->v[i]);
		LinePtr->v[i][0]=r[0];
		LinePtr->v[i][1]=r[1];
		LinePtr->v[i][2]=r[2];
	    }
	    if (premult == 2)
	    {
		LinePtr->v[i][0]+=x; // Restore Origin
		LinePtr->v[i][1]+=y;
		LinePtr->v[i][2]+=z;
	    }
	    if (i >= 3) // type five line only has 4 points.
		break;
	}
	break;
    default:
	break;
    }
    
    return 1;
}

/*****************************************************************************/
int Rotate1Part(int partnum, float m[4][4])
{
    float          m1[4][4];
    int            i = 0;
    struct L3LineS *LinePtr;
    
    if (SelectedLinePtr)
	LinePtr = SelectedLinePtr;

    else for (LinePtr = Parts[0].FirstLine; LinePtr; LinePtr = LinePtr->NextLine)
    {
	if (i == partnum)
	    break;
	i++;
    }
    if (!LinePtr)
	return 0; //partnum not found

    switch (LinePtr->LineType)
    {
    case 0:
	break;
    case 1:
	M4M4Mul(m1,LinePtr->v,m);
        memcpy(LinePtr->v, m1, sizeof(LinePtr->v));
	//LinePtr->v = m1;
	break;
    case 2:
    case 3:
    case 4:
    case 5:
	// This should work unless LinePtr->v[i] has bad numbers for linetypes 2,3.
	M4M4Mul(m1,LinePtr->v,m);
        memcpy(LinePtr->v, m1, sizeof(LinePtr->v));
	break;
    default:
	break;
    }
    
    return 1;
}

/*****************************************************************************/
int Translate1Part(int partnum, float m[4][4])
{
    float          m1[4][4];
    int            i = 0;
    struct L3LineS *LinePtr;
    UNUSED(m1);

    if (SelectedLinePtr)
	LinePtr = SelectedLinePtr;

    else for (LinePtr = Parts[0].FirstLine; LinePtr; LinePtr = LinePtr->NextLine)
    {
	if (i == partnum)
	    break;
	i++;
    }
    if (!LinePtr)
	return 0; //partnum not found

    switch (LinePtr->LineType)
    {
    case 0:
	break;
    case 1:
        //M4M4Mul(m1,LinePtr->v,m);
	LinePtr->v[0][3] += m[0][3];
	LinePtr->v[1][3] += m[1][3];
	LinePtr->v[2][3] += m[2][3];
	break;
    case 2:
    case 3:
    case 4:
    case 5:
	for (i=0; i<LinePtr->LineType; i++)
	{
	    LinePtr->v[i][0] += m[0][3];
	    LinePtr->v[i][1] += m[1][3];
	    LinePtr->v[i][2] += m[2][3];
	    if (i >= 3) // type five line only has 4 points.
		break;
	}
	break;
    default:
	break;
    }
    
    return 1;
}

/*****************************************************************************/
int Locate1Part(int partnum, float m[4][4], int moveonly)
{
    float          m1[4][4];
    int            i = 0;
    struct L3LineS *LinePtr;
    
    if (SelectedLinePtr)
	LinePtr = SelectedLinePtr;

    else for (LinePtr = Parts[0].FirstLine; LinePtr; LinePtr = LinePtr->NextLine)
    {
	if (i == partnum)
	    break;
	i++;
    }
    if (!LinePtr)
	return 0; //partnum not found

    switch (LinePtr->LineType)
    {
    case 0:
	break;
    case 1:
        if (moveonly)
	{
	  LinePtr->v[0][3] = m[0][3];
	  LinePtr->v[1][3] = m[1][3];
	  LinePtr->v[2][3] = m[2][3];
	}
	else
	  memcpy(LinePtr->v, m, sizeof(LinePtr->v));
	break;
    case 2:
    case 3:
    case 4:
    case 5:
        if (moveonly)
	{
	  m[0][3] -= LinePtr->v[0][0];
	  m[1][3] -= LinePtr->v[0][1];
	  m[2][3] -= LinePtr->v[0][2];
	  for (i=0; i<LinePtr->LineType; i++)
	  {
	    LinePtr->v[i][0] += m[0][3];
	    LinePtr->v[i][1] += m[1][3];
	    LinePtr->v[i][2] += m[2][3];
	    if (i >= 3) // type five line only has 4 points.
	      break;
	  }
	}
	else
	{
	  // This should work 
	  //unless LinePtr->v[i] has bad numbers for linetypes 2,3.
	  M4M4Mul(m1,LinePtr->v,m);
	  memcpy(LinePtr->v, m1, sizeof(LinePtr->v));
	}
	break;
    default:
	break;
    }
    
    return 1;
}

/*****************************************************************************/
int Color1Part(int partnum, int Color)
{
    int            i = 0;
    struct L3LineS *LinePtr;
    
    if (SelectedLinePtr)
	LinePtr = SelectedLinePtr;

    else for (LinePtr = Parts[0].FirstLine; LinePtr; LinePtr = LinePtr->NextLine)
    {
	if (i == partnum)
	    break;
	i++;
    }
    if (!LinePtr)
	return 0; //partnum not found

    switch (LinePtr->LineType)
    {
    case 0:
	break;
    case 1:
	LinePtr->Color = Color;
	break;
    case 2:
    case 3:
    case 4:
    case 5:
	LinePtr->Color = Color;
	break;
    default:
	break;
    }
    
    return 1;
}

/*****************************************************************************/
int Find1PartMatrix(int partnum, float m[4][4])
{
    int            i = 0;
    struct L3LineS *LinePtr;
    
    if (SelectedLinePtr)
	LinePtr = SelectedLinePtr;

    else for (LinePtr = Parts[0].FirstLine; LinePtr; LinePtr = LinePtr->NextLine)
    {
	if (i == partnum)
	    break;
	i++;
    }
    if (!LinePtr)
	return 0; //partnum not found

    switch (LinePtr->LineType)
    {
    case 0:
	break;
    case 1:
        memcpy(m, LinePtr->v, sizeof(LinePtr->v));
	//printf("Piece (%0.2f, %0.2f, %0.2f)\n", m[0][3],m[1][3],m[2][3]);
	M4M4Mul(m,m_m,LinePtr->v); // Adjust center point of part by view matrix.
	//printf("View  (%0.2f, %0.2f, %0.2f)\n", m[0][3],m[1][3],m[2][3]);
	return 1;
    case 2:
    case 3:
    case 4:
    case 5:
    default:
	break;
    }
    
    return 0;
}

/*****************************************************************************/
int Print1LineP(struct L3LineS *LinePtr, char *s)
{
    float          m[4][4];
    int            j,k;

    //Round very small numbers to 0.0
    if (LinePtr->LineType != 0)
    {
      memcpy(m, LinePtr->v, sizeof(LinePtr->v));
      for (k = 0; k < 4; k++)
	for (j = 0; j < 4; j++)
	{
	  if (fabs(m[k][j]) < 0.000001)
	    m[k][j] = 0.0;
	}
    }

    switch (LinePtr->LineType)
    {
    case 0:
	sprintf(s,"%d %s", LinePtr->LineType, LinePtr->Comment);
      break;
    case 1:
      // Put quotes around filenames with embedded white space.
      // NOTE: This is incompatible with MLCad 3.1.
      if (strpbrk(LinePtr->PartPtr->DatName," \t"))
	sprintf(s,"%d %d %g %g %g %g %g %g %g %g %g %g %g %g \"%s\"",
		LinePtr->LineType, LinePtr->Color,
		m[0][3],m[1][3],m[2][3],
		m[0][0],m[0][1],m[0][2],
		m[1][0],m[1][1],m[1][2],
		m[2][0],m[2][1],m[2][2],
		LinePtr->PartPtr->DatName);
      else
	sprintf(s,"%d %d %g %g %g %g %g %g %g %g %g %g %g %g %s",
		LinePtr->LineType, LinePtr->Color,
		m[0][3],m[1][3],m[2][3],
		m[0][0],m[0][1],m[0][2],
		m[1][0],m[1][1],m[1][2],
		m[2][0],m[2][1],m[2][2],
		LinePtr->PartPtr->DatName);
      break;
    case 2:
	sprintf(s,"%d %d %g %g %g %g %g %g",
		LinePtr->LineType, LinePtr->Color,
		m[0][0],m[0][1],m[0][2],
		m[1][0],m[1][1],m[1][2]);
      break;
    case 3:
	sprintf(s,"%d %d %g %g %g %g %g %g %g %g %g",
		LinePtr->LineType, LinePtr->Color,
		m[0][0],m[0][1],m[0][2],
		m[1][0],m[1][1],m[1][2],
		m[2][0],m[2][1],m[2][2]);
      break;
    case 4:
	sprintf(s,"%d %d %g %g %g %g %g %g %g %g %g %g %g %g",
		LinePtr->LineType, LinePtr->Color,
		m[0][0],m[0][1],m[0][2],
		m[1][0],m[1][1],m[1][2],
		m[2][0],m[2][1],m[2][2],
		m[3][0],m[3][1],m[3][2]);
      break;
    case 5:
	sprintf(s,"%d %d %g %g %g %g %g %g %g %g %g %g %g %g",
		LinePtr->LineType, LinePtr->Color,
		m[0][0],m[0][1],m[0][2],
		m[1][0],m[1][1],m[1][2],
		m[2][0],m[2][1],m[2][2],
		m[3][0],m[3][1],m[3][2]);
      break;
    default:
	sprintf(s,"");
      break;
    }

    return 1;
}

/*****************************************************************************/
int Print1LinePtr(struct L3LineS *LinePtr, int i, char *s)
{
    if (!LinePtr)
    {
      if (i < 0)
	sprintf(s,"--START--");
      else
	sprintf(s,"--END--");
      return 0; //partnum not found
    }

    return Print1LineP(LinePtr, s);
  }

/*****************************************************************************/
int Print3Parts(int partnum, char *s1, char *s2, char *s3)
{
    int            i = 0;
    struct L3LineS *LinePtr = NULL;
    struct L3LineS *PrevPtr = NULL;
    struct L3LineS *NextPtr = NULL;
    
    if (SelectedLinePtr)
    {
      for (PrevPtr = Parts[0].FirstLine; PrevPtr; PrevPtr = PrevPtr->NextLine)
      {
	if (i == (partnum-1))
	{
	  break;
	}
	i++;
      }
      LinePtr = SelectedLinePtr;
      i = partnum;
    }
    else 
      for (LinePtr = Parts[0].FirstLine; LinePtr; LinePtr = LinePtr->NextLine)
    {
	if (i == partnum)
	{
	  break;
	}
	i++;
	PrevPtr = LinePtr;
    }

    if (!LinePtr)
      i = partnum;
    else 
      NextPtr = LinePtr->NextLine;

    Print1LinePtr(PrevPtr, i-1, s1);
    Print1LinePtr(LinePtr, i, s2);
    Print1LinePtr(NextPtr, i+1, s3);
    
    return i;
}

/*****************************************************************************/
int Print1Part(int partnum, FILE *f)
{
  char           s[256];
  int            i = 0;
  struct L3LineS *LinePtr;
  
  if (SelectedLinePtr)
  {
    LinePtr = SelectedLinePtr;
    i = partnum;
  }
  else for (LinePtr = Parts[0].FirstLine; LinePtr; LinePtr = LinePtr->NextLine)
  {
    if (i == partnum)
      break;
    i++;
  }
  
  if (!LinePtr)
    i = partnum;
  i = Print1LinePtr(LinePtr, i, s);
  fprintf(f, "%s\n", s);
  
  return i;
}

/*****************************************************************************/
int Add1Part(int partnum)
{
    float m[4][4] = {
      {1.0,0.0,0.0,0.0},
      {0.0,1.0,0.0,0.0},
      {0.0,0.0,1.0,0.0},
      {0.0,0.0,0.0,1.0}
    };

    int            i = 0;
    struct L3LineS *LinePtr;
    struct L3LineS *PrevPtr;
    struct L3PartS *PartPtr;
    char *SubPartDatName;
    int Color;

    if (SelectedLinePtr)
	UnSelect1Part(SelectedLinePtr->LineNo);

    PrevPtr = NULL;
    for (LinePtr = Parts[0].FirstLine; LinePtr; LinePtr = LinePtr->NextLine)
    {
	PrevPtr = LinePtr;
	if (i == partnum)
	{
	    i++;
	    break; // found the part to insert after.
	}
	i++;
    }

    if (!PrevPtr)
    {
	// I have to make a Parts[0] for the new nameless model.
	// Unless its an old model where we deleted all the pieces.
	PartPtr = &Parts[0];
	if (nParts == 0)
	    nParts = 1;                         /* First time a model is loaded    */
	if (!strlen(datfilename) || (!strcmp(datfilename, " ")))
	    strcpy(datfilename, "model.ldr");
	PartPtr->DatName = Strdup(datfilename);
	if (!PartPtr->DatName)
	    return 0;
	PartPtr->FirstLine = NULL;
	nColors = 0;
	/* Be sure to register default part color */
	Colors[nColors++].Color = L3Pov.DefaultPartColorNumber;

	SubPartDatName = Strdup("3001.dat");
	Color = 0;
    }
    // Check if this is a primitive.
    else if ((PrevPtr->LineType >= 2) && (PrevPtr->LineType <= 5))
    {
        LinePtr = (struct L3LineS *) calloc(sizeof(struct L3LineS), 1);
	if (!LinePtr)
	  return 0;
        memcpy(LinePtr, PrevPtr, sizeof(struct L3LineS));
#ifndef __TURBOC__
	LinePtr->LineNo = i;
#endif
        //memcpy(LinePtr->v, PrevPtr->v, sizeof(LinePtr->v));
	LinePtr->NextLine = PrevPtr->NextLine;
	PrevPtr->NextLine = LinePtr;
	return LinePtr->LineNo;
    }
    else if (!(PrevPtr->PartPtr) || 
	     !(PrevPtr->PartPtr->DatName) ||
	     !(strlen(PrevPtr->PartPtr->DatName)))
    {
	// No previous part, so just use the default.
	SubPartDatName = Strdup("3001.dat");
	Color = 0;
    }
    else
    {
	SubPartDatName = PrevPtr->PartPtr->DatName;
	Color = PrevPtr->Color;
    }


    /* Watch out for uppercase and (back)slashes in filename... */
    FixDatName(SubPartDatName);
    PartPtr = FindPart(0, SubPartDatName);
    if (!PartPtr)
	return 0; //partnum not found
    if (LoadPart(PartPtr, false, Parts[0].DatName) == 2)
    {
	return 0;
    }

    LinePtr = (struct L3LineS *) calloc(sizeof(struct L3LineS), 1);
    if (!LinePtr)
	return 0;

    LinePtr->LineType = 1;
    LinePtr->Color = Color;
#ifndef __TURBOC__
    LinePtr->LineNo = i;
#endif
    if (PrevPtr && (PrevPtr->LineType == 1))
    {
#ifdef REUSE_ORIENTATION
        LinePtr->v = PrevPtr->v;
#else
        memcpy(LinePtr->v, m, sizeof(LinePtr->v));
	//LinePtr->v = m;
        // Just copy the position, but not the orientation.
	LinePtr->v[0][3] += PrevPtr->v[0][3];
	LinePtr->v[1][3] += PrevPtr->v[1][3];
	LinePtr->v[2][3] += PrevPtr->v[2][3];
#endif
    }
    else
        memcpy(LinePtr->v, m, sizeof(LinePtr->v));
        //LinePtr->v = m;
    LinePtr->PartPtr = PartPtr;
    if (PrevPtr)
    {
	LinePtr->NextLine = PrevPtr->NextLine;
	PrevPtr->NextLine = LinePtr;
    }
    else
    {
	Parts[0].FirstLine = LinePtr;
    }

    return LinePtr->LineNo;
}

/*****************************************************************************/
int Swap1Part(int partnum, char *SubPartDatName)
{
    int            i = 0;
    struct L3LineS *LinePtr;
    struct L3PartS *PartPtr;

    if (SelectedLinePtr)
	LinePtr = SelectedLinePtr;

    else for (LinePtr = Parts[0].FirstLine; LinePtr; LinePtr = LinePtr->NextLine)
    {
	if (i == partnum) // found the part
	    break;
	i++;
    }
    if (!LinePtr)
	return 0; //partnum not found

    switch (LinePtr->LineType)
    {
    case 0:
	break;
    case 1:
	/* Watch out for uppercase and (back)slashes in filename... */
	FixDatName(SubPartDatName);
	PartPtr = FindPart(0, SubPartDatName);
	if (PartPtr)
	{
	    if (LoadPart(PartPtr, false, Parts[0].DatName) == 2)
	    {
		return 0;
	    }
	    LinePtr->PartPtr = PartPtr;
	    return 1;
	}
	break;
    case 2:
    case 3:
    case 4:
    case 5:
    default:
	break;
    }
    
    return 0;
}

/*****************************************************************************/
int DrawCurPart(int Color)
{
    return Draw1PartPtr(SelectedLinePtr, Color);
}

#if 0
/*****************************************************************************/
#define METATYPE_TRANSLATE 1
#define METATYPE_ROTATE    2
#define METATYPE_SCALE     3
#define METATYPE_TRANSFORM 4
#define METATYPE_TEXMAP    5

static char         *MetaKeywords[] = {
   NULL,
   "TRANSLATE",
   "ROTATE",
   "SCALE",
   "TRANSFORM",
   "!TEXMAP",
};
#endif

/*****************************************************************************/
int Print1Internal(FILE *f, struct L3PartS *PartPtr)
{
  struct L3LineS *LinePtr;
  
  if (!PartPtr)
    return 0;

  for (LinePtr = PartPtr->FirstLine; LinePtr; LinePtr = LinePtr->NextLine)
  {
    if (LinePtr && (LinePtr->LineType == 1) && LinePtr->Comment)
    {
      Print1Internal(f, LinePtr->PartPtr);
    }
    else
    {
      char s[1024];
      int i;
      Print1LineP(LinePtr, s);
      for (i = 0; i < LinePtr->RandomColor; i++)
	fprintf(f, " ");
      fprintf(f, "%s\n", s);
    }
  }

  //fprintf(f, "0 %s END\n", MetaKeywords[PartPtr->Internal]);
  return 0;
}

/*****************************************************************************/
int Print1Model(char *filename)
{
    FILE *f;
    struct L3LineS *LinePtr;
    int j;
    char s[1024];
    
    printf("Write DAT %s\n", filename);
    //f = fopen(filename, "w+");
    f = fopen(filename, "w");
    if (!f)
    {
	printf("Could not open %s\n", filename);
	return(0);
    }

    for (LinePtr = Parts[0].FirstLine; LinePtr; LinePtr = LinePtr->NextLine)
    {
      if (LinePtr && (LinePtr->LineType == 1) && LinePtr->Comment)
      {
	Print1Internal(f, LinePtr->PartPtr);
	continue;
      }
      Print1LineP(LinePtr, s);
      for (j = 0; j < LinePtr->RandomColor; j++)
	fprintf(f, " ");
      fprintf(f, "%s\n", s);

      //if (ldraw_commandline_opts.debug_level == 1)
      //  fprintf(stdout, "%s\n", s);
    }

    // Print out MPD subfiles after the main file.
    LinePtr = Parts[0].FirstLine;
    if (LinePtr && (LinePtr->LineType == 0))
    {
      char *c;

      // Skip whitespace
      for (c = LinePtr->Comment; *c != 0; c++)
      {
	if ((*c != ' ') && (*c != '\t'))
	  break;
      }
      if (strncmp(c,"FILE ",5) == 0)
      {
	int i;
	printf("MPD file.  Need to save subfiles\n");
	for (i = 1; i < nParts; i++)
	{
	  if (Parts[i].IsMPD)
	  {
	    sprintf(s, "0 FILE %s",Parts[i].DatName);
	    fprintf(f, "%s\n", s);
	    //if (ldraw_commandline_opts.debug_level == 1)
	    //  fprintf(stdout, "%s\n", s);
	    for (LinePtr = Parts[i].FirstLine; 
		 LinePtr; 
		 LinePtr = LinePtr->NextLine)
	    {
	      Print1LineP(LinePtr, s);
	      for (j = 0; j < LinePtr->RandomColor; j++)
		fprintf(f, " ");
	      fprintf(f, "%s\n", s);
	      
	      //if (ldraw_commandline_opts.debug_level == 1)
	      //  fprintf(stdout, "%s\n", s);
	    }
	  }
	}
      }
    }

    fclose(f);
    return 0;
}

/*****************************************************************************/
int Comment1LinePtr(struct L3LineS *LinePtr, char *Comment)
{
    int            Len;

    if (!LinePtr)
	return 0; //partnum not found
    
    if (LinePtr->PartPtr)
    {
      //free(LinePtr->PartPtr);
      LinePtr->PartPtr = NULL;
    }

    if ((LinePtr->LineType == 0) 
	&& LinePtr->Comment
	&& (LinePtr->Comment != (char *) LinePtr->v))
    {
      free(LinePtr->Comment);
      LinePtr->Comment = NULL;
    }

    LinePtr->LineType = 0;
    // SaveLine(&LinePtrPtr, LinePtr, comment);
    Len = strlen(Comment) + 1;
    if (Len > sizeof(LinePtr->v))
    {
      LinePtr->Comment = Strdup(Comment);
      if (!LinePtr->Comment)
      {
	free(LinePtr);
	return (1);
      }
    }
    else
    {
      /* Reuse the 64 bytes of float v[4][4] */
      LinePtr->Comment = (char *) LinePtr->v;
      strcpy(LinePtr->Comment, Comment);
    }

    return 1;
}

/*****************************************************************************/
int Comment1Part(int partnum, char *Comment)
{
    int            i = 0;
    struct L3LineS *LinePtr;

    if (SelectedLinePtr)
	LinePtr = SelectedLinePtr;
    else
    for (LinePtr = Parts[0].FirstLine; LinePtr; LinePtr = LinePtr->NextLine)
    {
	if (i == partnum)
	    break;	    // Found the part
	i++;
    }
    
    if (!LinePtr)
	return 0; //partnum not found
    
    return Comment1LinePtr(LinePtr, Comment);

}

/*****************************************************************************/
extern int editing;
extern int curpiece;
extern int movingpiece;
// Note: should I reset these next 2 to defaults when exiting edit mode?
extern int StartLineNo;
extern int DrawToCurPiece;

static int LineNoCounter = -1;
static int BufALineNo = -1;
/*****************************************************************************/
int Init1LineCounter()
{
  LineNoCounter = 0;
  BufALineNo = -1;
  return 0;
}

/*****************************************************************************/
int BufA1Store(int IsModel, struct L3LineS *LinePtr)
{
  if (!IsModel)
    return 0;
  
  UNUSED(LinePtr);
  BufALineNo = LineNoCounter;

  return BufALineNo;
}

/*****************************************************************************/
int BufA1Retrieve(int IsModel, struct L3LineS *LinePtr)
{

  if (!IsModel)
    return 0;
  
  UNUSED(LinePtr);
  return BufALineNo;
}

/*****************************************************************************/
int Skip1Line(int IsModel, struct L3LineS *LinePtr)
{
  if (!IsModel)
    return 0;
  
  UNUSED(LinePtr);
  LineNoCounter++;

  if (!editing)
    return 0;

  // This needs work.  I should count the lines myself.
  // Make Init1LineCounter() fn and make a static counter in this module.
  // Call Init1LineCounter() in DrawModel() in L3View.cpp.
  // Use the static counter instead of LinePtr->LineNo.
  if (LineNoCounter <= StartLineNo)
  {
    //printf("Skipping %d < %d\n", LinePtr->LineNo, StartLineNo);
    return 1;
  }

  if (!DrawToCurPiece)
    return 0;

  // This needs work.  
  // I need to redraw up to the current piece whenever I pick "goto piece".
  if (LineNoCounter > curpiece)
  {
    //printf("Skipping %d > %d\n", LinePtr->LineNo, curpiece);
    return 1;
  }

  return 0;
}

/*****************************************************************************/
int Switch1Part(int partnum)
{
    int            i = 0;
    struct L3LineS *LinePtr;
    struct L3LineS *PrevPtr;
    struct L3LineS *NextPtr;

    if (SelectedLinePtr)
	LinePtr = SelectedLinePtr;

    PrevPtr = NULL;
    for (LinePtr = Parts[0].FirstLine; LinePtr; LinePtr = LinePtr->NextLine)
    {
	if (i == partnum)
	    break;	    // Found the part
	PrevPtr = LinePtr;
	i++;
    }

    if (!LinePtr)
	return 0; //partnum not found

    if (!LinePtr->NextLine)
	return 0; // next part not found
    
    NextPtr = LinePtr->NextLine;
    if (!PrevPtr)
      Parts[0].FirstLine = NextPtr;
    else
      PrevPtr->NextLine = NextPtr;
    LinePtr->NextLine = NextPtr->NextLine;
    NextPtr->NextLine = LinePtr;

    return 1;
}

/*****************************************************************************/
// This can be used to free any parts temporarily loaded for parts.lst
// Just save nParts, load & show a part, then FreeSomeParts(oldnParts, nParts)
/*****************************************************************************/
extern void FreePart(struct L3PartS * PartPtr);

/*****************************************************************************/
void FreeSomeParts(firstpart, lastpart)
{
   register int         i;
   for (i = firstpart; i < lastpart; i++)
      FreePart(&Parts[i]);
   //nParts = firstpart;
}

/*****************************************************************************/
#include "glwinkit.h"

extern GLdouble model_mat[4*4];
extern GLdouble proj_mat[4*4];
extern GLint view_mat[4];

extern GLint Width;
extern GLint Height;

typedef struct vector3d_struct {
	float x;
	float y;
	float z;
} vector3d;

extern void MakePartBox(struct L3PartS *PartPtr,float m[4][4],vector3d bb3[8]);

/*****************************************************************************/
void GetPartBox(struct L3LineS *LinePtr, int sc[4], int clip)
{
  float          r[4];
  int            i;
  float          r2[4];
  vector3d       bb3d[8];
  float m[4][4];
  struct L3PartS *PartPtr = LinePtr->PartPtr;
  int            numpoints = 8;
  struct L3PartS FakePart;

  GLdouble s0x, s0y, s0z;
  GLdouble s1x, s1y, s1z;
  GLdouble s2x, s2y, s2z;

#if 0
  // NOTE:  Must eventually add support for primitives.
  //if (LinePtr->LineType == 0)
  if (LinePtr->LineType != 1)
  {
    sc[0] = 0;
    sc[1] = 0;
    sc[2] = 1;
    sc[3] = 1;
    return;
  }
#else
  if (LinePtr->LineType == 0)
  {
    sc[0] = 0;
    sc[1] = 0;
    sc[2] = 1;
    sc[3] = 1;
    return;
  }
  else if (LinePtr->LineType != 1)
  {
    //sc[0] = 0;
    //sc[1] = 0;
    //sc[2] = Width;
    //sc[3] = Height;
    //return;

    PartPtr = &FakePart;
    numpoints = LinePtr->LineType;
    if (numpoints > 4)
	numpoints = 4;
    for (i = 0; i < numpoints; i++)
    {
	M4V3Mul(r,m_m,LinePtr->v[i]);
	bb3d[i].x=r[0];
	bb3d[i].y=r[1];
	bb3d[i].z=r[2];
    }
  }
  else
#endif
   
  {
  numpoints = 8;
  M4M4Mul(m,m_m,LinePtr->v); // Adjust center point of part by view matrix.
  MakePartBox(PartPtr, m, bb3d);
  }

  // Gotta convert to screen coords first for opengl.
  s2x = s2y = 0.0;
  s0x = Width;
  s0y = Height;

  for (i = 0; i < numpoints; i++)
  {
    gluProject((GLdouble)bb3d[i].x, (GLdouble)-bb3d[i].y, (GLdouble)-bb3d[i].z,
	       model_mat, proj_mat, view_mat,
	       &s1x, &s1y, &s1z);

    if (s1x < s0x)
      s0x = s1x;
    if (s1y < s0y)
      s0y = s1y;

    if (s1x > s2x)
      s2x = s1x;
    if (s1y > s2y)
      s2y = s1y;
  }

  if (clip)
  {
    // FUDGE FACTOR.
    s0x -= 8;
    s0y -= 8;
    s2x += 8;
    s2y += 8;
    
    if (s0x < 0.0) s0x = 0.0;
    if (s0y < 0.0) s0y = 0.0;
    if (s2x > Width) s2x = Width;
    if (s2y > Height) s2y = Height;
  }

  sc[0] = (int)s0x;
  sc[1] = (int)s0y;
  sc[2] = (int)(s2x - s0x);
  sc[3] = (int)(s2y - s0y);

  if (sc[2] <= 0) 
    sc[2] = 1;
  if (sc[3] <= 0) 
    sc[3] = 1;
}

/*****************************************************************************/
int Get1PartBox(int partnum, int sc[4])
{
    int            i = 0;
    struct L3LineS *LinePtr;

    if (SelectedLinePtr)
	LinePtr = SelectedLinePtr;

    else 
      for (LinePtr = Parts[0].FirstLine; LinePtr; LinePtr = LinePtr->NextLine)
    {
	if (i == partnum)
	    break;	    // Found the part
	i++;
    }

    if (!LinePtr)
	return 0; //partnum not found

    GetPartBox(LinePtr, sc, 1);

    return 1;
}

/*****************************************************************************/
int Make1Primitive(int partnum, char *str)
{
    int            i = 0;
    struct L3LineS *LinePtr;

    struct L3LineS       Data;
    int  n, j;
    char seps[] = "()[]{}<> ,\t"; // Allow parens and commas for readability.
    char *token;
      
    if (SelectedLinePtr)
	LinePtr = SelectedLinePtr;
    else
    for (LinePtr = Parts[0].FirstLine; LinePtr; LinePtr = LinePtr->NextLine)
    {
	if (i == partnum)
	    break;	    // Found the part
	i++;
    }

    if (!LinePtr)
	return 0; //partnum not found
    
#if 1 
    for (n = 0, i = 0, j = 0,token = strtok( str, seps );
	 token != NULL;
	 token = strtok( NULL, seps ), n++, i++ )
    {
      if (n == 0)
	sscanf(token, "%d", &Data.LineType);
      if (n == 1)
	sscanf(token, "%i", &Data.Color);
      else
      {
	if (i > 4)
	{
	  i = 2;
	  j++;
	  if (j > 3) // Maximum of 4 points in a primitive.
	    break;
	}
	sscanf(token, "%f", &Data.v[j][i-2]);
      }
    }
#else
    memset(&Data, 0, sizeof(Data));
    n = sscanf(str, "%d %d %f %f %f %f %f %f %f %f %f %f %f %f",
                 &Data.LineType, &Data.Color,
                 &Data.v[0][0], &Data.v[0][1], &Data.v[0][2],
                 &Data.v[1][0], &Data.v[1][1], &Data.v[1][2],
                 &Data.v[2][0], &Data.v[2][1], &Data.v[2][2],
                 &Data.v[3][0], &Data.v[3][1], &Data.v[3][2]);
#endif
    if (n < 1)
    {
      return 0;  // Oops, must reset LinePtr to something reasonable.
    }

    // Make sure this is a primitive before modifying LinePtr.
    if ((Data.LineType < 2) || (Data.LineType > 5))
	return 0;

    if (LinePtr->PartPtr)
    {
      //free(LinePtr->PartPtr);
      LinePtr->PartPtr = NULL;
    }

    // Copy the structure.
    Data.NextLine = LinePtr->NextLine;
    *LinePtr = Data; 

    return 1;
}

/*****************************************************************************/
int Inline1Part(int partnum)
{
    int            i = 0;
    int            n, Color, CurColor;
    struct L3LineS *LinePtr;
    struct L3LineS *PrevPtr;
    struct L3LineS *NextPtr;
    struct L3LineS *FirstPtr;
    struct L3PartS *PartPtr;
    float          r[4];
    float          m[4][4];
    float          m1[4][4];
    char           Comment[256];

    if (SelectedLinePtr)
	LinePtr = SelectedLinePtr;
    else
    for (LinePtr = Parts[0].FirstLine; LinePtr; LinePtr = LinePtr->NextLine)
    {
	if (i == partnum)
	    break;	    // Found the part
	i++;
    }

    if (!LinePtr)
	return -1; //partnum not found
    
    if (LinePtr->LineType != 1)
      return i;

    // Keep the color, matrix, and first LinePtr of the LinePtr.
    CurColor = LinePtr->Color;
    memcpy(m, LinePtr->v, sizeof(LinePtr->v));
    if (LinePtr->PartPtr)
      FirstPtr = LinePtr->PartPtr->FirstLine;
    else 
      FirstPtr = NULL;
    
    // Convert LinePtr into comments.
    strcpy(Comment, "Inlined: ");
    n = strlen(Comment);
    Print1LinePtr(LinePtr, i, &(Comment[n]));

    //Start the inlined part with a blank comment.
    Comment1LinePtr(LinePtr, " "); 

    //Make 3 more comments.
    NextPtr = LinePtr; 
    for (i=0; i<3; i++)
    {
      LinePtr = NextPtr; 
      NextPtr = (struct L3LineS *) calloc(sizeof(struct L3LineS), 1);
      memcpy(NextPtr, LinePtr, sizeof(struct L3LineS));
      NextPtr->Comment = NULL;
      if (i == 0) //Then add the "Inlined" comment.
	Comment1LinePtr(NextPtr, Comment);
      else
	Comment1LinePtr(NextPtr, " ");
      LinePtr->NextLine = NextPtr; 
    }

    // Insert the inlined part between the last two comment lines.
    PrevPtr = LinePtr;
    for (LinePtr = FirstPtr; LinePtr; LinePtr = LinePtr->NextLine)
    {
      NextPtr = (struct L3LineS *) calloc(sizeof(struct L3LineS), 1);
      memcpy(NextPtr, LinePtr, sizeof(struct L3LineS));

      switch (LinePtr->Color)
      {
      case 16:
	Color = CurColor;
	break;
      case 24:
#if 0
	if (0 <= CurColor  &&  CurColor <= 15)
	  Color = edge_color(CurColor);
	else
	  Color = 0;
#else
	// I dont know why L3View.cpp checks for less than 15,
	// but it breaks the inliner on edges so skip it.
	// NOTE: I should probably remove ALL of these checks
	// and let edge_color handle it.  I think it does.
	// Also, why is edge_color for 16 and 24 set to 0 in lcolors.c
	if (CurColor == 16)
	  Color = 24;
	else if (CurColor == 24)
	  Color = 24;
	else
	  Color = edge_color(CurColor);
#endif
	break;
      default:
	Color = LinePtr->Color;
	break;
      }
      NextPtr->Color = Color;
      switch (LinePtr->LineType)
      {
      case 0:
	NextPtr->Color = LinePtr->Color;
	/* Reuse the 64 bytes of float v[4][4] if possible.  Else strdup*/
	if (LinePtr->Comment != (char *) LinePtr->v)
	{
	  NextPtr->Comment = Strdup(LinePtr->Comment);
	  if (!NextPtr->Comment)
	  {
	    // Uh Oh!  Out of memory!  Time to panic...
	  }
	}
	break;
      case 1:
	M4M4Mul(m1,m,LinePtr->v);
	memcpy(NextPtr->v, m1, sizeof(LinePtr->v));
	PartPtr = (struct L3PartS *) calloc(sizeof(struct L3PartS), 1);
	memcpy(PartPtr, LinePtr->PartPtr, sizeof(struct L3PartS));
	NextPtr->PartPtr = PartPtr;
	break;
      case 2:
      case 3:
      case 4:
      case 5:
	n = LinePtr->LineType;
	if (n > 4)
	  n = 4;
	for (i=0; i<n; i++)
	{
	  M4V3Mul(r,m,LinePtr->v[i]);
	  NextPtr->v[i][0] = r[0];
	  NextPtr->v[i][1] = r[1];
	  NextPtr->v[i][2] = r[2];
	}
      }

      // Insert the next inlined thing into top level linked list.
      NextPtr->NextLine = PrevPtr->NextLine;
      PrevPtr->NextLine = NextPtr;
      PrevPtr = NextPtr;
    }
}

/*****************************************************************************/
extern void hoser(float m[4][4], int color, int steps, int drawline,
		  char *parttext, char *firstparttext);

#define PI 3.1415927

/*****************************************************************************/
int hoseends(char *segname, int color, float m1[4][4], float m2[4][4])
{
    struct L3LineS *NextPtr;
    struct L3LineS *FirstPtr;
    struct L3LineS *LastPtr;
    struct L3PartS *PartPtr;

    FirstPtr = SelectedLinePtr;
    LastPtr = FirstPtr->NextLine;

    // Add a segname part at far end of the hose.
    NextPtr = (struct L3LineS *) calloc(sizeof(struct L3LineS), 1);
    memcpy(NextPtr, LastPtr, sizeof(struct L3LineS));
    PartPtr = (struct L3PartS *) calloc(sizeof(struct L3PartS), 1);
    memcpy(PartPtr, LastPtr->PartPtr, sizeof(struct L3PartS));
    NextPtr->PartPtr = PartPtr;
    NextPtr->Color = color;
    // Link it in
    NextPtr->NextLine = FirstPtr->NextLine; 
    FirstPtr->NextLine = NextPtr;
    // Switch to part 755.dat
    SelectedLinePtr = NextPtr;
    Swap1Part(0, segname);
    //rotate it 180 degrees around X.
    Move1Part(0, m1, 1);
    //Rotate1Part(i, m);
    
    // Add a segname part at far end of the hose.
    NextPtr = (struct L3LineS *) calloc(sizeof(struct L3LineS), 1);
    memcpy(NextPtr, FirstPtr, sizeof(struct L3LineS));
    PartPtr = (struct L3PartS *) calloc(sizeof(struct L3PartS), 1);
    memcpy(PartPtr, FirstPtr->PartPtr, sizeof(struct L3PartS));
    NextPtr->PartPtr = PartPtr;
    NextPtr->Color = color;
    // Link it in
    NextPtr->NextLine = FirstPtr->NextLine; 
    FirstPtr->NextLine = NextPtr;
    // Switch to part 755.dat
    SelectedLinePtr = NextPtr;
    Swap1Part(0, segname);
    //rotate it 180 degrees around X.
    Move1Part(0, m2, 1);
    //Rotate1Part(i, m);
    
    return 0;
}
      
/*****************************************************************************/
int Hose1Part(int partnum, int steps)
{
    float m[4][4] = {
      {1.0,0.0,0.0,0.0},
      {0.0,1.0,0.0,0.0},
      {0.0,0.0,1.0,0.0},
      {0.0,0.0,0.0,1.0}
    };
    float          m1[4][4];
    float          m2[4][4];
    float          v[4] = {0, -50, 0, 1}; // Velocity in y of intermediate control points.
    float          v1[4] = {0, -5, 0, 1}; // Offset in y of intermediate control points.
    float          r[4];

    int            i = 0;
    int            n, Color, CurColor;
    struct L3LineS *LinePtr;
    struct L3LineS *PrevPtr;
    struct L3LineS *NextPtr = NULL;
    struct L3LineS *PinPtr = NULL;
    struct L3LineS *FirstPtr;
    struct L3LineS *LastPtr;
    struct L3PartS *PartPtr;
    char *SubPartDatName;
    char           Comment[256];
    char *parttext = NULL;
    char *firstparttext = NULL;
    double         angle;
    int            drawlines = 0;

    UNUSED(n);
    UNUSED(Color);
    UNUSED(Comment);

    if (steps < 0)
    {
      drawlines = 1;
      steps = -steps;
    }

    if (SelectedLinePtr)
    {
      LinePtr = SelectedLinePtr;
      UnSelect1Part(SelectedLinePtr->LineNo);
    } 
    else
    for (LinePtr = Parts[0].FirstLine; LinePtr; LinePtr = LinePtr->NextLine)
    {
      if (i == partnum)
	break;	    // Found the part
      i++;
    }

    LastPtr = LinePtr;
    if (!LastPtr)
      return -1; //partnum not found
    if (LastPtr->LineType != 1)
      return i;

    for (LinePtr = Parts[0].FirstLine; LinePtr; LinePtr = LinePtr->NextLine)
    {
      if (LinePtr->NextLine == LastPtr)
	break;	    // Found the part
    }

    FirstPtr = LinePtr; 
    if (!FirstPtr)
      return i;
    if (FirstPtr->LineType != 1)
      return i;

    // Get partname and color
    PartPtr = FirstPtr->PartPtr;
    CurColor = LastPtr->Color; // Get color from last part for easier cleanup.
    if (PartPtr)
      SubPartDatName = PartPtr->DatName;
    else
      SubPartDatName = strdup("");

    // Backup SelectedLinePtr
    PrevPtr = SelectedLinePtr;
    // Position the segment insertion point after the plug.
    SelectedLinePtr = FirstPtr;

    if ((stricmp(SubPartDatName, "750.dat") == 0) ||
	(stricmp(SubPartDatName, "752.dat") == 0))
    {
      // Add some 755.dat plug parts at the ends of the hose.
      // rotate them 180 degrees around X.
      angle = PI;
      m[1][1] = (float)cos(angle);
      m[1][2] = (float)(-1.0*sin(angle));
      m[2][1] = (float)sin(angle);
      m[2][2] = (float)cos(angle);
      // m[1][3] = 8.0;
      hoseends("755.dat", CurColor, m, m);

      FirstPtr = LinePtr->NextLine; 
      if (!FirstPtr)
	return i;
      if (FirstPtr->LineType != 1)
	return i;
      
      // Get the names of the hose parts.
      firstparttext = strdup("756.dat");
      FixDatName(firstparttext);
      parttext = strdup("754.dat");
      FixDatName(parttext);

      v1[1] = -5; // Offset in y of intermediate control points.
    }
    if ((stricmp(SubPartDatName, "757.dat") == 0) ||
	(stricmp(SubPartDatName, "760.dat") == 0))
    {
      // Eeek!  757 is upside down for hose making.  (Its a stud not a tube)

      drawlines = 1;

      // Add some 759.dat plug parts at the ends of the hose.
      // rotate one of them 180 degrees around X.
      angle = PI;
      memcpy(m1, m, sizeof(m));
      m1[1][1] = (float)cos(angle);
      m1[1][2] = (float)(-1.0*sin(angle));
      m1[2][1] = (float)sin(angle);
      m1[2][2] = (float)cos(angle);
      // Move the other 759 (the one near the 757) +16 LDU but dont rotate it.
      m[1][3] = 16.0; 

      if (stricmp(SubPartDatName, "757.dat") == 0)
	hoseends("759.dat", CurColor, m1, m);
      else
      {
	hoseends("759.dat", CurColor, m, m1);
	// Move the start of the hose segments by 24
	v1[1] = 24; // Offset in y of intermediate control points.
	v[1] -= 30; // Increase the velocity to clear any technic pins.

	//m[1][3] = -24.0;  
	//NextPtr = FirstPtr;
	//M4M4Mul(m1,FirstPtr->v,m);

	// And reverse the direction of the control point at this end.
	//m1[1][3] = 24.0;  
	//PinPtr = LastPtr;
	//M4M4Mul(m2,LastPtr->v,m1);
	m1[1][3] = 0.0;  
	PinPtr = SelectedLinePtr;
	M4M4Mul(m2,LastPtr->v,m1);
      }

      FirstPtr = LinePtr->NextLine; 
      if (!FirstPtr)
	return i;
      if (FirstPtr->LineType != 1)
	return i;
      
      // Get the names of the hose parts.
      firstparttext = strdup("758.dat");
      FixDatName(firstparttext);
      parttext = strdup("758.dat");
      FixDatName(parttext);

      v1[1] = -5; // Offset in y of intermediate control points.
    }
    else if (stricmp(SubPartDatName, "755.dat") == 0)
    {
      // Found a plug.  Get the names of the hose parts.
      firstparttext = strdup("756.dat");
      FixDatName(firstparttext);
      parttext = strdup("754.dat");
      FixDatName(parttext);

      v1[1] = -5; // Offset in y of intermediate control points.
    }
    else if (stricmp(SubPartDatName, "759.dat") == 0)
    {
      // Found a plug.  Get the names of the hose parts.
      firstparttext = strdup("758.dat");
      FixDatName(firstparttext);
      parttext = strdup("754.dat");
      FixDatName(parttext);

      v1[1] = -5; // Offset in y of intermediate control points.
    }
    else if (stricmp(SubPartDatName, "208.dat") == 0)
    {
      // Found a chain link end.  Get the names of the link parts.
      // Should do something similar to the technic pin method below
      // to orient on the embedded half link in the 208 chain end parts.
      // Maybe search for the torus part t04q3333.dat, except it scales by 3.
      angle = -PI/2.0;
      m[1][1] = (float)cos(angle);
      m[1][2] = (float)(-1.0*sin(angle));
      m[2][1] = (float)sin(angle);
      m[2][2] = (float)cos(angle);
      m[2][3] = 12.0;

      NextPtr = FirstPtr;
      PinPtr = LastPtr;
      M4M4Mul(m1,LinePtr->v,m);
      M4M4Mul(m2,LastPtr->v,m);

      firstparttext = strdup("209.dat");
      FixDatName(firstparttext);
      parttext = strdup("209.dat");
      FixDatName(parttext);

      v1[2] = 2; // Offset in z of intermediate control points.
      v1[1] = -5; // Offset in y of intermediate control points.
      v[1] -= 50; // Higher Velocity in y to stretch the chain.
    }
    else if (stricmp(SubPartDatName, "76.dat") == 0)
    {
      // Found a flex tube.  Get the names of the hose parts.
      firstparttext = strdup("77.dat");
      FixDatName(firstparttext);
      parttext = strdup("77.dat");
      FixDatName(parttext);

      v1[1] = 0; // Offset in y of intermediate control points.
    }
    else if (stricmp(SubPartDatName, "79.dat") == 0)
    {
      // Found a rib.  Get the names of the hose parts.
      firstparttext = strdup("80.dat");
      FixDatName(firstparttext);
      parttext = strdup("80.dat");
      FixDatName(parttext);

      steps -=2;  // Subtract the end ribs from the total.
      v1[1] = -4; // Offset in y of intermediate control points.
      //v[1] *= 2; // Double the velocity to clear technic pins.
      v[1] -= 30; // Increase the velocity to clear any technic pins.
    }
    else if (stricmp(SubPartDatName, "stud3a.dat") == 0)
    {
      // Found a flex axle.  Get the names of the hose parts.
      firstparttext = strdup("s/faxle1.dat"); // Gotta do faxle2 ... faxle5.dat
      FixDatName(firstparttext);
      parttext = strdup("axlehol8.dat");
      FixDatName(parttext);

      v1[1] = 0; // Offset in y of intermediate control points.
    }
    else
    {
      // Default to the names of the ribbed hose parts.
      firstparttext = strdup("79.dat");
      FixDatName(firstparttext);
      parttext = strdup("80.dat");
      FixDatName(parttext);

      v1[1] = -3; // Move the end ribs up 3 to fit on the studs
      //v[1] *= 2; // Double the velocity to clear technic pins.
      // Doubling is preferred, but leads to crimped hoses in some cases.
      // I really should be able to tell what velocity and length to use.
      // Look at the cross products of:
      //   unit vectors multiplied by m1 and m2
      // Also examine the  distance of the ends of the velocity vectors
      // to avoid crimped hoses caused by long velocity vectors.
      v[1] -= 30; // Increase the velocity to clear any technic pins.

      // Automatically locate technic pin ends, otherwise use a topstud.
      PartPtr = FirstPtr->PartPtr;
      for (NextPtr = PartPtr->FirstLine; NextPtr; NextPtr = NextPtr->NextLine)
      {
	PartPtr = NextPtr->PartPtr;
	if (PartPtr && !strncmp(PartPtr->DatName, "connect", 7))
	{
	  M4M4Mul(m1,LinePtr->v,NextPtr->v);
	  break;
	}
      }
      PartPtr = LastPtr->PartPtr;
      for (PinPtr = PartPtr->FirstLine; PinPtr; PinPtr = PinPtr->NextLine)
      {
	PartPtr = PinPtr->PartPtr;
	if (PartPtr && !strncmp(PartPtr->DatName, "connect", 7))
	{
	  M4M4Mul(m2,LastPtr->v,PinPtr->v);
	  break;
	}
      }
    }

    // Get the 4 control points from the part locations.
    if (!NextPtr)
      memcpy(m1, LinePtr->v, sizeof(LinePtr->v));
    M4V4Mul(r,m1,v1);
    m[0][0] = r[0];
    m[0][1] = r[1];
    m[0][2] = r[2];
    m[0][3] = 0;
    if (!NextPtr)
      memcpy(m1, LinePtr->v, sizeof(LinePtr->v));
    M4V4Mul(r,m1,v);
    m[1][0] = r[0];
    m[1][1] = r[1];
    m[1][2] = r[2];
    m[1][3] = 0;
    if (!PinPtr)
      memcpy(m2, LastPtr->v, sizeof(LastPtr->v));
    if (stricmp(SubPartDatName, "208.dat") == 0)
    {
      // Chain links are not centered, so move the 2nd pair of ctrl points a bit.
      v[1] -= 10;
      v1[1] -= 10;
    }  
    M4V4Mul(r,m2,v);
    m[2][0] = r[0];
    m[2][1] = r[1];
    m[2][2] = r[2];
    m[2][3] = 0;
    if (!PinPtr)
      memcpy(m2, LastPtr->v, sizeof(LastPtr->v));
    M4V4Mul(r,m2,v1);
    m[3][0] = r[0];
    m[3][1] = r[1];
    m[3][2] = r[2];
    m[3][3] = 0;

    // Position the segment insertion point after the plug.
    SelectedLinePtr = FirstPtr;

    // Insert the hosed part between the last two comment lines.
    hoser(m, CurColor, steps, drawlines, parttext,firstparttext);

    // Restore SelectedLinePtr
    SelectedLinePtr = PrevPtr;

    return i+steps+2+1; // curpiece + numsteps + 2 plugs + 1 endpart.
}

/*****************************************************************************/
int hoseseg(char *segname, int color, float m[4][4])
{
    struct L3LineS *LinePtr;
    struct L3LineS *PrevPtr;
    struct L3LineS *NextPtr;
    struct L3LineS *FirstPtr;
    struct L3LineS *LastPtr;
    struct L3PartS *PartPtr;

    float m1[4][4] = {
      {1.0,0.0,0.0,0.0},
      {0.0,1.0,0.0,0.0},
      {0.0,0.0,1.0,0.0},
      {0.0,0.0,0.0,1.0}
    };
    UNUSED(PrevPtr);
    UNUSED(FirstPtr);
    UNUSED(LastPtr);
    UNUSED(m1);

    LinePtr = SelectedLinePtr;

    // Add a 755.dat plug part at near end of the hose.
    NextPtr = (struct L3LineS *) calloc(sizeof(struct L3LineS), 1);
    memcpy(NextPtr, LinePtr, sizeof(struct L3LineS));
#if 0
    if (stricmp(segname, "77.dat") == 0)
    {
      // Gotta scale a flex tube from 1 LDU to 4 LDU per seg
      // That way we need less segments.
      m1[1][1] = 4;
      m1[1][3] = -2; // move center up half of 4 LDU
      M4M4Mul(NextPtr->v,m,m1);
    }
    else
      memcpy(NextPtr->v, m, sizeof(LinePtr->v));
#else
    memcpy(NextPtr->v, m, sizeof(LinePtr->v));
#endif
    PartPtr = (struct L3PartS *) calloc(sizeof(struct L3PartS), 1);
    memcpy(PartPtr, LinePtr->PartPtr, sizeof(struct L3PartS));
    NextPtr->PartPtr = PartPtr;
    NextPtr->Color = color;

    // Link it in
    NextPtr->NextLine = LinePtr->NextLine; 
    LinePtr->NextLine = NextPtr;

    // Switch to part 755.dat
    SelectedLinePtr = NextPtr;
    Swap1Part(0, segname);

    return 0;
}

/*****************************************************************************/
int GetCurLineType(int partnum)
{
    int            i = 0;
    struct L3LineS *LinePtr;

    if (SelectedLinePtr)
	LinePtr = SelectedLinePtr;
    else
    for (LinePtr = Parts[0].FirstLine; LinePtr; LinePtr = LinePtr->NextLine)
    {
	if (i == partnum)
	    break;	    // Found the part
	i++;
    }

    if (!LinePtr)
	return -1; //partnum not found
    
    return LinePtr->LineType;
}

/*****************************************************************************/
struct L3PartS *ZeroBase = NULL;
struct L3PartS *TurnAxis = NULL;

/*****************************************************************************/
// Convert Paul Easters nifty zerobase.dat into code so we always have it.
/*****************************************************************************/
int CreateZeroBase()
{
  struct L3LineS *lp;
  struct L3PartS *PartPtr;

  // printf("Creating ZeroBase\n");

  // Create unscaled zerobase.dat part.
  PartPtr = (struct L3PartS *) calloc(sizeof(struct L3PartS), 1);
  PartPtr->DatName = strdup("zerobase.dat");
  PartPtr->FirstLine = (struct L3LineS *) calloc(sizeof(struct L3LineS), 1);
  lp = PartPtr->FirstLine;
  // Axis
  lp->LineType = 2; lp->Color = 4; 
  lp->v[0][0] = -1; lp->v[0][1] = 0; lp->v[0][2] = 0;
  lp->v[1][0] = 1; lp->v[1][1] = 0; lp->v[1][2] = 0;
  lp->NextLine = (struct L3LineS *) calloc(sizeof(struct L3LineS), 1);
  lp = lp->NextLine;
  lp->LineType = 2; lp->Color = 5; 
  lp->v[0][0] = 0; lp->v[0][1] = -1; lp->v[0][2] = 0;
  lp->v[1][0] = 0; lp->v[1][1] = 1; lp->v[1][2] = 0;
  lp->NextLine = (struct L3LineS *) calloc(sizeof(struct L3LineS), 1);
  lp = lp->NextLine;
  lp->LineType = 2; lp->Color = 9; 
  lp->v[0][0] = 0; lp->v[0][1] = 0; lp->v[0][2] = -1;
  lp->v[1][0] = 0; lp->v[1][1] = 0; lp->v[1][2] = 1;

  lp->NextLine = (struct L3LineS *) calloc(sizeof(struct L3LineS), 1);
  lp = lp->NextLine;
  // +x
  lp->LineType = 2; lp->Color = 4; 
  lp->v[0][0] = 1.16; lp->v[0][1] = 0; lp->v[0][2] =  .02;
  lp->v[1][0] =  1.16; lp->v[1][1] = 0; lp->v[1][2] =  -.02;
  lp->NextLine = (struct L3LineS *) calloc(sizeof(struct L3LineS), 1);
  lp = lp->NextLine;
  lp->LineType = 2; lp->Color = 4; 
  lp->v[0][0] = 1.18; lp->v[0][1] = 0; lp->v[0][2] = 0;
  lp->v[1][0] =  1.14; lp->v[1][1] = 0; lp->v[1][2] =  0;
  lp->NextLine = (struct L3LineS *) calloc(sizeof(struct L3LineS), 1);
  lp = lp->NextLine;
  lp->LineType = 2; lp->Color = 4; 
  lp->v[0][0] = 1.04; lp->v[0][1] = 0; lp->v[0][2] =  .04;
  lp->v[1][0] =  1.08; lp->v[1][1] = 0; lp->v[1][2] =  -.04;
  lp->NextLine = (struct L3LineS *) calloc(sizeof(struct L3LineS), 1);
  lp = lp->NextLine;
  lp->LineType = 2; lp->Color = 4; 
  lp->v[0][0] = 1.08; lp->v[0][1] = 0; lp->v[0][2] =  .04;
  lp->v[1][0] =  1.04; lp->v[1][1] = 0; lp->v[1][2] =  -.04;
  lp->NextLine = (struct L3LineS *) calloc(sizeof(struct L3LineS), 1);
  lp = lp->NextLine;
   // -x
  lp->LineType = 2; lp->Color = 4; 
  lp->v[0][0] =  -1.18; lp->v[0][1] = 0; lp->v[0][2] = 0;
  lp->v[1][0] =  -1.14; lp->v[1][1] = 0; lp->v[1][2] =  0;
  lp->NextLine = (struct L3LineS *) calloc(sizeof(struct L3LineS), 1);
  lp = lp->NextLine;
  lp->LineType = 2; lp->Color = 4; 
  lp->v[0][0] =  -1.04; lp->v[0][1] = 0; lp->v[0][2] =  .04;
  lp->v[1][0] =  -1.08; lp->v[1][1] = 0; lp->v[1][2] =  -.04;
  lp->NextLine = (struct L3LineS *) calloc(sizeof(struct L3LineS), 1);
  lp = lp->NextLine;
  lp->LineType = 2; lp->Color = 4; 
  lp->v[0][0] =  -1.08; lp->v[0][1] = 0; lp->v[0][2] =  .04;
  lp->v[1][0] =  -1.04; lp->v[1][1] = 0; lp->v[1][2] =  -.04;
  lp->NextLine = (struct L3LineS *) calloc(sizeof(struct L3LineS), 1);
  lp = lp->NextLine;
   // +y
  lp->LineType = 2; lp->Color = 5; 
  lp->v[0][0] =  .02; lp->v[0][1] = 1.16; lp->v[0][2] = 0;
  lp->v[1][0] =  -.02; lp->v[1][1] =  1.16; lp->v[1][2] =  0;
  lp->NextLine = (struct L3LineS *) calloc(sizeof(struct L3LineS), 1);
  lp = lp->NextLine;
  lp->LineType = 2; lp->Color = 5; 
  lp->v[0][0] =  0; lp->v[0][1] =   1.18; lp->v[0][2] = 0;
  lp->v[1][0] =   0; lp->v[1][1] =    1.14; lp->v[1][2] =  0;
  lp->NextLine = (struct L3LineS *) calloc(sizeof(struct L3LineS), 1);
  lp = lp->NextLine;
  lp->LineType = 2; lp->Color = 5; 
  lp->v[0][0] =  0; lp->v[0][1] =   1.06; lp->v[0][2] = 0;
  lp->v[1][0] =   .04; lp->v[1][1] =  1.04; lp->v[1][2] =  0;
  lp->NextLine = (struct L3LineS *) calloc(sizeof(struct L3LineS), 1);
  lp = lp->NextLine;
  lp->LineType = 2; lp->Color = 5; 
  lp->v[0][0] =  0; lp->v[0][1] =   1.06; lp->v[0][2] = 0;
  lp->v[1][0] =  -.04; lp->v[1][1] =  1.06; lp->v[1][2] =  0;
  lp->NextLine = (struct L3LineS *) calloc(sizeof(struct L3LineS), 1);
  lp = lp->NextLine;
  lp->LineType = 2; lp->Color = 5; 
  lp->v[0][0] =  0; lp->v[0][1] =   1.06; lp->v[0][2] = 0;
  lp->v[1][0] =   .04; lp->v[1][1] =  1.08; lp->v[1][2] =  0;
  lp->NextLine = (struct L3LineS *) calloc(sizeof(struct L3LineS), 1);
  lp = lp->NextLine;
   // -y
  lp->LineType = 2; lp->Color = 5; 
  lp->v[0][0] =  0; lp->v[0][1] =  -1.18; lp->v[0][2] = 0;
  lp->v[1][0] =   0; lp->v[1][1] =   -1.14; lp->v[1][2] =  0;
  lp->NextLine = (struct L3LineS *) calloc(sizeof(struct L3LineS), 1);
  lp = lp->NextLine;
  lp->LineType = 2; lp->Color = 5; 
  lp->v[0][0] =  0; lp->v[0][1] =  -1.06; lp->v[0][2] = 0;
  lp->v[1][0] =   .04; lp->v[1][1] = -1.04; lp->v[1][2] =  0;
  lp->NextLine = (struct L3LineS *) calloc(sizeof(struct L3LineS), 1);
  lp = lp->NextLine;
  lp->LineType = 2; lp->Color = 5; 
  lp->v[0][0] =  0; lp->v[0][1] =  -1.06; lp->v[0][2] = 0;
  lp->v[1][0] =  -.04; lp->v[1][1] = -1.06; lp->v[1][2] =  0;
  lp->NextLine = (struct L3LineS *) calloc(sizeof(struct L3LineS), 1);
  lp = lp->NextLine;
  lp->LineType = 2; lp->Color = 5; 
  lp->v[0][0] =  0; lp->v[0][1] =  -1.06; lp->v[0][2] = 0;
  lp->v[1][0] =   .04; lp->v[1][1] = -1.08; lp->v[1][2] =  0;
  lp->NextLine = (struct L3LineS *) calloc(sizeof(struct L3LineS), 1);
  lp = lp->NextLine;
   // +z
  lp->LineType = 2; lp->Color = 9; 
  lp->v[0][0] =  .02; lp->v[0][1] = 0; lp->v[0][2] = 1.16;
  lp->v[1][0] =  -.02; lp->v[1][1] =  0; lp->v[1][2] =  1.16;
  lp->NextLine = (struct L3LineS *) calloc(sizeof(struct L3LineS), 1);
  lp = lp->NextLine;
  lp->LineType = 2; lp->Color = 9; 
  lp->v[0][0] =  0; lp->v[0][1] =   0; lp->v[0][2] = 1.18;
  lp->v[1][0] =   0; lp->v[1][1] =    0; lp->v[1][2] =  1.14;
  lp->NextLine = (struct L3LineS *) calloc(sizeof(struct L3LineS), 1);
  lp = lp->NextLine;
  lp->LineType = 2; lp->Color = 9; 
  lp->v[0][0] =  .04; lp->v[0][1] = 0; lp->v[0][2] = 1.04;
  lp->v[1][0] =   .04; lp->v[1][1] =  0; lp->v[1][2] =  1.08;
  lp->NextLine = (struct L3LineS *) calloc(sizeof(struct L3LineS), 1);
  lp = lp->NextLine;
  lp->LineType = 2; lp->Color = 9; 
  lp->v[0][0] =  .04; lp->v[0][1] = 0; lp->v[0][2] = 1.04;
  lp->v[1][0] =  -.04; lp->v[1][1] =  0; lp->v[1][2] =  1.08;
  lp->NextLine = (struct L3LineS *) calloc(sizeof(struct L3LineS), 1);
  lp = lp->NextLine;
  lp->LineType = 2; lp->Color = 9; 
  lp->v[0][0] = -.04; lp->v[0][1] = 0; lp->v[0][2] = 1.04;
  lp->v[1][0] =  -.04; lp->v[1][1] =  0; lp->v[1][2] =  1.08;
  lp->NextLine = (struct L3LineS *) calloc(sizeof(struct L3LineS), 1);
  lp = lp->NextLine;
   // -z
  lp->LineType = 2; lp->Color = 9; 
  lp->v[0][0] =  0; lp->v[0][1] =   0; lp->v[0][2] = -1.18;
  lp->v[1][0] =  0; lp->v[1][1] =    0; lp->v[1][2] =  -1.14;
  lp->NextLine = (struct L3LineS *) calloc(sizeof(struct L3LineS), 1);
  lp = lp->NextLine;
  lp->LineType = 2; lp->Color = 9; 
  lp->v[0][0] =  .04; lp->v[0][1] = 0; lp->v[0][2] = -1.04;
  lp->v[1][0] =  .04; lp->v[1][1] =  0; lp->v[1][2] =  -1.08;
  lp->NextLine = (struct L3LineS *) calloc(sizeof(struct L3LineS), 1);
  lp = lp->NextLine;
  lp->LineType = 2; lp->Color = 9; 
  lp->v[0][0] =  .04; lp->v[0][1] = 0; lp->v[0][2] = -1.04;
  lp->v[1][0] = -.04; lp->v[1][1] =  0; lp->v[1][2] =  -1.08;
  lp->NextLine = (struct L3LineS *) calloc(sizeof(struct L3LineS), 1);
  lp = lp->NextLine;
  lp->LineType = 2; lp->Color = 9; 
  lp->v[0][0] = -.04; lp->v[0][1] = 0; lp->v[0][2] = -1.04;
  lp->v[1][0] = -.04; lp->v[1][1] =  0; lp->v[1][2] =  -1.08;

  ZeroBase = PartPtr;
  return 0;
}

/*****************************************************************************/
int CreateTurnAxis()
{
  struct L3LineS *LinePtr;
  struct L3PartS *PartPtr;
  float m[4][4] = {{100,0,0,0}, {0,100,0,0}, {0,0,100,0}, {0,0,0,1}};

  // printf("Creating TurnAxis\n");

  if (ZeroBase == NULL)
    CreateZeroBase();

  // Create a scaled version of ZeroBase part.
  PartPtr = (struct L3PartS *) calloc(sizeof(struct L3PartS), 1);
  PartPtr->DatName = strdup("TurnAxis.dat");
  PartPtr->FirstLine = (struct L3LineS *) calloc(sizeof(struct L3LineS), 1);
  LinePtr = PartPtr->FirstLine;
  LinePtr->LineType = 1; LinePtr->Color = 16;     
  memcpy(LinePtr->v, m, sizeof(LinePtr->v));
  LinePtr->PartPtr = ZeroBase;

  TurnAxis = PartPtr;
  return 0;
}

/*****************************************************************************/
int DrawTurnAxis(float m[4][4])
{
  struct L3LineS *LinePtr;
  int Color = 16;

  if (TurnAxis == NULL)
    CreateTurnAxis();

  LinePtr = TurnAxis->FirstLine;
  LinePtr->v[0][3] = m[0][3];
  LinePtr->v[1][3] = m[1][3];
  LinePtr->v[2][3] = m[2][3];

  return Draw1PartPtr(LinePtr, Color);
}

#ifdef PART_BOX_TEST
/***************************************************************/
void PrintPartBox(struct L3PartS *PartPtr, char *boxfilename)
{
  extern char datfilepath[256];

  char filename[256];
  FILE *fp;
  char *p;

  double x, y, z;
  double dx, dy, dz;

  printf("BBox = (%0.2f,%0.2f,%0.2f) (%0.2f,%0.2f,%0.2f)\n",
	 PartPtr->BBox[0][0],PartPtr->BBox[0][1],PartPtr->BBox[0][2],
	 PartPtr->BBox[1][0],PartPtr->BBox[1][1],PartPtr->BBox[1][2]);

#if 0
  if (boxfilename)
      concat_path(datfilepath, boxfilename, filename);
  else
  {
      strcpy(filename, datfilename);
      if ((p = strrchr(filename, '.')) != NULL)
	  *p = 0;
      strcat(filename, use_uppercase ? ".BOX" : ".box");
      
      printf("Write BOX file %s\n", filename);
  }

  if ((fp = fopen(filename,"w+"))==NULL) {
    printf("Could not open %s\n", filename);
    return;
  }

  dx = (PartPtr->BBox[1][0] - PartPtr->BBox[0][0]) / 2.0;
  dy = (PartPtr->BBox[1][1] - PartPtr->BBox[0][1]) / 2.0;
  dz = (PartPtr->BBox[1][2] - PartPtr->BBox[0][2]) / 2.0;

  x = PartPtr->BBox[0][0] + dx;
  y = PartPtr->BBox[1][1] - dy;
  z = PartPtr->BBox[0][2] + dz;
  
  fprintf(fp,"1 16 %g %g %g %g 0 0 0 %g 0 0 0 %g box.dat\n",x, y, z, dx, dy, dz);
#endif
}

/***************************************************************/
void Print1PartBox()
{
  PrintPartBox(&Parts[0], NULL);
}

#endif
