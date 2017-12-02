
/*  A Bison parser, made from ldlite.y
    by GNU Bison version 1.28  */

#define YYBISON 1  /* Identify Bison output.  */

#define	tIDENT	257
#define	tGARBAGE	258
#define	tINT	259
#define	tFLOAT	260
#define	tZERO	261
#define	tONE	262
#define	tTWO	263
#define	tTHREE	264
#define	tFOUR	265
#define	tFIVE	266
#define	tSTEP	267
#define	tPAUSE	268
#define	tWRITE	269
#define	tCLEAR	270
#define	tSAVE	271
#define	tEOL	272
#define	tEOF	273
#define	tROTATE	274
#define	tTRANSLATE	275
#define	tEND	276
#define	tSCALE	277
#define	tTRANSFORM	278
#define	tCOLOR	279
#define	tALIAS	280
#define	tPOINT	281
#define	tMATRIX	282
#define	tFILE	283

#include <string.h>
#include "stdio.h"
#include "stdlib.h"
#include "ldliteVR.h"
#include "math.h"
#define NO_COMMENTS
#undef NO_LINES

char yyfilename[256];			/* Input filename */
char *ldraw_model=NULL;
int tmp_i;
void yyerror ( char * );
int stepcount=0;
char *mpd_subfile_name;

vector3d *current_translation[MAX_INCLUDE_DEPTH];
matrix3d *current_transform[MAX_INCLUDE_DEPTH];
int current_color[MAX_INCLUDE_DEPTH];
int current_type[MAX_INCLUDE_DEPTH];
int push_transform(vector3d *t,matrix3d *m);
void znamelist_push();
int print_transform(vector3d *t,matrix3d *m);
int is_current_file_not_cached(void);
void platform_comment(char *, int level);
int edge_color(int);
matrix3d * savemat(float a, float b, float c,
		   float d, float e, float f,
		   float g, float h, float i);


typedef union {
	char *c;
	int i;
	double d;
	vector3d *v;
	matrix3d *m;
} YYSTYPE;
#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		213
#define	YYFLAG		-32768
#define	YYNTBASE	30

#define YYTRANSLATE(x) ((unsigned)(x) <= 283 ? yytranslate[x] : 65)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     1,     3,     4,     5,     6,
     7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
    17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
    27,    28,    29
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     2,     3,     6,     8,    10,    12,    14,    16,    18,
    20,    22,    24,    26,    30,    34,    38,    42,    44,    46,
    48,    50,    52,    54,    56,    59,    62,    64,    66,    68,
    70,    72,    74,    76,    78,    83,    88,    93,    98,   103,
   108,   113,   118,   133,   148,   154,   160,   166,   172,   178,
   184,   189,   194,   199,   204,   209,   214,   219,   224,   230,
   236,   241,   246,   251,   256,   261,   266,   272,   278,   283,
   288,   290,   292,   294,   296,   298,   300,   302,   304,   306,
   308,   310,   312,   314,   316,   318,   320,   322,   323,   326,
   330,   334,   336,   338,   340,   342,   352,   354,   358,   360,
   362,   369,   376,   382,   388,   395,   402,   410,   418,   426
};

static const short yyrhs[] = {    31,
     0,     0,    31,    32,     0,    34,     0,    35,     0,    36,
     0,    37,     0,    43,     0,    60,     0,    61,     0,    62,
     0,    63,     0,    64,     0,    44,    31,    45,     0,    46,
    31,    47,     0,    48,    31,    49,     0,    50,    31,    51,
     0,    38,     0,    39,     0,    40,     0,    41,     0,    42,
     0,    55,     0,    54,     0,     1,    18,     0,     1,    19,
     0,     7,     0,     8,     0,     9,     0,    10,     0,    11,
     0,    12,     0,     5,     0,     6,     0,     7,    13,    53,
    18,     0,     7,    13,    53,    19,     0,     7,    14,    53,
    18,     0,     7,    14,    53,    19,     0,     7,    15,    53,
    18,     0,     7,    15,    53,    19,     0,     7,    16,    53,
    18,     0,     7,    16,    53,    19,     0,     7,    25,    33,
     3,    33,    33,    33,    33,    33,    33,    33,    33,    33,
    18,     0,     7,    25,    33,     3,    33,    33,    33,    33,
    33,    33,    33,    33,    33,    19,     0,     7,    26,     3,
    56,    18,     0,     7,    26,     3,    56,    19,     0,     7,
    27,     3,    58,    18,     0,     7,    27,     3,    58,    19,
     0,     7,    28,     3,    57,    18,     0,     7,    28,     3,
    57,    19,     0,     7,    29,     3,    18,     0,     7,    29,
     3,    19,     0,     7,    17,    53,    18,     0,     7,    17,
    53,    19,     0,     7,    21,    58,    18,     0,     7,    21,
    58,    19,     0,     7,    21,    22,    18,     0,     7,    21,
    22,    19,     0,     7,    20,    33,    58,    18,     0,     7,
    20,    33,    58,    19,     0,     7,    20,    22,    18,     0,
     7,    20,    22,    19,     0,     7,    23,    33,    18,     0,
     7,    23,    33,    19,     0,     7,    23,    22,    18,     0,
     7,    23,    22,    19,     0,     7,    24,    58,    57,    18,
     0,     7,    24,    58,    57,    19,     0,     7,    24,    22,
    18,     0,     7,    24,    22,    19,     0,     3,     0,     4,
     0,    33,     0,    13,     0,    14,     0,    15,     0,    16,
     0,    17,     0,    21,     0,    20,     0,    22,     0,    25,
     0,    27,     0,    23,     0,    24,     0,    26,     0,    28,
     0,     0,    53,    52,     0,     7,    53,    18,     0,     7,
    53,    19,     0,    18,     0,    19,     0,    33,     0,     3,
     0,    33,    33,    33,    33,    33,    33,    33,    33,    33,
     0,     3,     0,    33,    33,    33,     0,     3,     0,     3,
     0,     8,    56,    58,    57,    59,    18,     0,     8,    56,
    58,    57,    59,    19,     0,     9,    56,    58,    58,    18,
     0,     9,    56,    58,    58,    19,     0,    10,    56,    58,
    58,    58,    18,     0,    10,    56,    58,    58,    58,    19,
     0,    11,    56,    58,    58,    58,    58,    18,     0,    11,
    56,    58,    58,    58,    58,    19,     0,    12,    56,    58,
    58,    58,    58,    18,     0,    12,    56,    58,    58,    58,
    58,    19,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
    81,    84,    85,    88,    89,    90,    91,    92,    93,    94,
    95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
   105,   106,   107,   108,   109,   110,   113,   117,   121,   125,
   129,   133,   137,   144,   153,   167,   184,   197,   213,   226,
   242,   255,   271,   282,   296,   307,   321,   332,   346,   357,
   371,   384,   399,   413,   430,   434,   441,   445,   452,   457,
   465,   469,   476,   480,   487,   491,   498,   502,   509,   513,
   521,   538,   555,   568,   577,   586,   595,   604,   613,   622,
   631,   640,   649,   658,   667,   676,   685,   696,   705,   720,
   737,   757,   760,   766,   776,   789,   797,   810,   823,   836,
   851,   892,   927,   946,   969,   991,  1016,  1042,  1071,  1097
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","tIDENT",
"tGARBAGE","tINT","tFLOAT","tZERO","tONE","tTWO","tTHREE","tFOUR","tFIVE","tSTEP",
"tPAUSE","tWRITE","tCLEAR","tSAVE","tEOL","tEOF","tROTATE","tTRANSLATE","tEND",
"tSCALE","tTRANSFORM","tCOLOR","tALIAS","tPOINT","tMATRIX","tFILE","LdrawModel",
"LdrawLines","LdrawLine","Number","StepLine","PauseLine","WriteLine","ClearLine",
"ColorLine","ColorAliasLine","PointAliasLine","MatrixAliasLine","MPDFileLine",
"SaveLine","TranslateStartLine","TranslateEndLine","RotateStartLine","RotateEndLine",
"ScaleStartLine","ScaleEndLine","TransformStartLine","TransformEndLine","Word",
"Words","CommentLine","BlankLine","Color","TransMatrix","Position","PartName",
"ObjectLine","LineLine","TriLine","QuadLine","FiveLine", NULL
};
#endif

static const short yyr1[] = {     0,
    30,    31,    31,    32,    32,    32,    32,    32,    32,    32,
    32,    32,    32,    32,    32,    32,    32,    32,    32,    32,
    32,    32,    32,    32,    32,    32,    33,    33,    33,    33,
    33,    33,    33,    33,    34,    34,    35,    35,    36,    36,
    37,    37,    38,    38,    39,    39,    40,    40,    41,    41,
    42,    42,    43,    43,    44,    44,    45,    45,    46,    46,
    47,    47,    48,    48,    49,    49,    50,    50,    51,    51,
    52,    52,    52,    52,    52,    52,    52,    52,    52,    52,
    52,    52,    52,    52,    52,    52,    52,    53,    53,    54,
    54,    55,    55,    56,    56,    57,    57,    58,    58,    59,
    60,    60,    61,    61,    62,    62,    63,    63,    64,    64
};

static const short yyr2[] = {     0,
     1,     0,     2,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     3,     3,     3,     3,     1,     1,     1,
     1,     1,     1,     1,     2,     2,     1,     1,     1,     1,
     1,     1,     1,     1,     4,     4,     4,     4,     4,     4,
     4,     4,    14,    14,     5,     5,     5,     5,     5,     5,
     4,     4,     4,     4,     4,     4,     4,     4,     5,     5,
     4,     4,     4,     4,     4,     4,     5,     5,     4,     4,
     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     0,     2,     3,
     3,     1,     1,     1,     1,     9,     1,     3,     1,     1,
     6,     6,     5,     5,     6,     6,     7,     7,     7,     7
};

static const short yydefact[] = {     2,
     0,     0,    88,     0,     0,     0,     0,     0,    92,    93,
     3,     4,     5,     6,     7,    18,    19,    20,    21,    22,
     8,     2,     2,     2,     2,    24,    23,     9,    10,    11,
    12,    13,    25,    26,    88,    88,    88,    88,    88,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,    95,
    33,    34,    27,    28,    29,    30,    31,    32,    94,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,    99,     0,     0,     0,     0,     0,
     0,     0,     0,     0,    71,    72,    74,    75,    76,    77,
    78,    90,    91,    80,    79,    81,    84,    85,    82,    86,
    83,    87,    73,    89,     0,     0,     0,     0,     0,    88,
    14,    88,    15,    88,    16,    88,    17,    35,    36,    37,
    38,    39,    40,    41,    42,    53,    54,     0,     0,    55,
    56,    63,    64,    97,     0,     0,     0,     0,     0,     0,
    51,    52,     0,     0,     0,     0,     0,     0,     0,     0,
     0,    59,    60,    98,     0,    67,    68,     0,    45,    46,
    47,    48,    49,    50,   100,     0,   103,   104,     0,     0,
     0,     0,     0,     0,     0,     0,     0,   101,   102,   105,
   106,     0,     0,    57,    58,    61,    62,    65,    66,    69,
    70,     0,     0,   107,   108,   109,   110,     0,     0,     0,
     0,     0,     0,     0,     0,    96,     0,     0,    43,    44,
     0,     0,     0
};

static const short yydefgoto[] = {   211,
     1,    11,    76,    12,    13,    14,    15,    16,    17,    18,
    19,    20,    21,    22,   111,    23,   113,    24,   115,    25,
   117,   104,    49,    26,    27,    60,   136,    77,   166,    28,
    29,    30,    31,    32
};

static const short yypact[] = {-32768,
    10,    15,   392,   487,   487,   487,   487,   487,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,    49,
   497,    49,   497,    49,    24,    27,    32,    34,   201,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,   497,
   497,   497,   497,   497,   156,   359,   372,   385,   227,   253,
   279,   305,   331,   497,-32768,    49,    23,    25,   507,    36,
   487,   497,   507,    28,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,   507,   497,   497,   497,   497,   409,
-32768,   426,-32768,   443,-32768,   460,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,    55,    49,-32768,
-32768,-32768,-32768,-32768,    49,    72,    49,    74,   102,   104,
-32768,-32768,    45,   110,   497,   497,   497,    77,   105,   171,
    97,-32768,-32768,-32768,    49,-32768,-32768,    49,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,   119,-32768,-32768,   123,   497,
   497,   125,   129,   131,   134,    49,    49,-32768,-32768,-32768,
-32768,   137,   143,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,    49,    49,-32768,-32768,-32768,-32768,    49,    49,    49,
    49,    49,    49,    49,    49,-32768,    49,   151,-32768,-32768,
    62,    64,-32768
};

static const short yypgoto[] = {-32768,
     1,-32768,    -4,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,   -23,-32768,-32768,     0,   -74,   -11,-32768,-32768,
-32768,-32768,-32768,-32768
};


#define	YYLAST		519


static const short yytable[] = {    59,
    59,    59,    59,    59,    61,    62,    63,    64,   140,    -1,
     2,    69,    70,    71,    72,    73,     3,     4,     5,     6,
     7,     8,    65,    66,    67,    68,    81,     9,    10,    82,
   143,    79,    33,    34,    83,    74,    84,    78,   137,    80,
   130,   131,   132,   133,   103,   141,   142,   165,   105,   106,
   107,   108,   109,    51,    52,    53,    54,    55,    56,    57,
    58,   212,   128,   213,   103,   103,   103,   103,   103,     0,
   139,   129,   152,   153,   135,     0,    59,     0,   135,    75,
   138,    51,    52,    53,    54,    55,    56,    57,    58,   156,
   157,   159,   160,     0,   144,   145,   146,   147,   172,    75,
   135,    51,    52,    53,    54,    55,    56,    57,    58,    51,
    52,    53,    54,    55,    56,    57,    58,     0,   175,   161,
   162,   163,   164,     0,   154,     0,   173,   167,   168,     0,
   155,     0,   158,   169,   170,   171,   178,   179,     0,    79,
   180,   181,   184,   185,    74,    78,   186,   187,   188,   189,
   176,   190,   191,   177,   194,   195,     2,     0,   182,   183,
   196,   197,   110,     4,     5,     6,     7,     8,   209,   210,
     0,   192,   193,     9,    10,    51,    52,    53,    54,    55,
    56,    57,    58,     0,     0,     0,     0,   198,   199,     0,
     0,     0,   174,   200,   201,   202,   203,   204,   205,   206,
   207,     0,   208,    85,    86,    51,    52,    53,    54,    55,
    56,    57,    58,    87,    88,    89,    90,    91,    92,    93,
    94,    95,    96,    97,    98,    99,   100,   101,   102,    85,
    86,    51,    52,    53,    54,    55,    56,    57,    58,    87,
    88,    89,    90,    91,   118,   119,    94,    95,    96,    97,
    98,    99,   100,   101,   102,    85,    86,    51,    52,    53,
    54,    55,    56,    57,    58,    87,    88,    89,    90,    91,
   120,   121,    94,    95,    96,    97,    98,    99,   100,   101,
   102,    85,    86,    51,    52,    53,    54,    55,    56,    57,
    58,    87,    88,    89,    90,    91,   122,   123,    94,    95,
    96,    97,    98,    99,   100,   101,   102,    85,    86,    51,
    52,    53,    54,    55,    56,    57,    58,    87,    88,    89,
    90,    91,   124,   125,    94,    95,    96,    97,    98,    99,
   100,   101,   102,    85,    86,    51,    52,    53,    54,    55,
    56,    57,    58,    87,    88,    89,    90,    91,   126,   127,
    94,    95,    96,    97,    98,    99,   100,   101,   102,     2,
     0,     0,     0,     0,     0,   112,     4,     5,     6,     7,
     8,     0,     2,     0,     0,     0,     9,    10,   114,     4,
     5,     6,     7,     8,     0,     2,     0,     0,     0,     9,
    10,   116,     4,     5,     6,     7,     8,     0,     0,     0,
     0,     0,     9,    10,    35,    36,    37,    38,    39,     0,
     0,    40,    41,     0,    42,    43,    44,    45,    46,    47,
    48,    35,    36,    37,    38,    39,     0,     0,    40,   148,
     0,    42,    43,    44,    45,    46,    47,    48,    35,    36,
    37,    38,    39,     0,     0,   149,    41,     0,    42,    43,
    44,    45,    46,    47,    48,    35,    36,    37,    38,    39,
     0,     0,    40,    41,     0,   150,    43,    44,    45,    46,
    47,    48,    35,    36,    37,    38,    39,     0,     0,    40,
    41,     0,    42,   151,    44,    45,    46,    47,    48,    50,
     0,    51,    52,    53,    54,    55,    56,    57,    58,    75,
     0,    51,    52,    53,    54,    55,    56,    57,    58,   134,
     0,    51,    52,    53,    54,    55,    56,    57,    58
};

static const short yycheck[] = {     4,
     5,     6,     7,     8,     5,     6,     7,     8,    83,     0,
     1,    35,    36,    37,    38,    39,     7,     8,     9,    10,
    11,    12,    22,    23,    24,    25,     3,    18,    19,     3,
   105,    43,    18,    19,     3,    40,     3,    42,     3,    44,
    18,    19,    18,    19,    49,    18,    19,     3,    60,    61,
    62,    63,    64,     5,     6,     7,     8,     9,    10,    11,
    12,     0,    74,     0,    69,    70,    71,    72,    73,    -1,
    82,    76,    18,    19,    79,    -1,    81,    -1,    83,     3,
    81,     5,     6,     7,     8,     9,    10,    11,    12,    18,
    19,    18,    19,    -1,   106,   107,   108,   109,    22,     3,
   105,     5,     6,     7,     8,     9,    10,    11,    12,     5,
     6,     7,     8,     9,    10,    11,    12,    -1,    22,    18,
    19,    18,    19,    -1,   129,    -1,    22,    18,    19,    -1,
   135,    -1,   137,   145,   146,   147,    18,    19,    -1,   151,
    18,    19,    18,    19,   149,   150,    18,    19,    18,    19,
   155,    18,    19,   158,    18,    19,     1,    -1,   170,   171,
    18,    19,     7,     8,     9,    10,    11,    12,    18,    19,
    -1,   176,   177,    18,    19,     5,     6,     7,     8,     9,
    10,    11,    12,    -1,    -1,    -1,    -1,   192,   193,    -1,
    -1,    -1,    22,   198,   199,   200,   201,   202,   203,   204,
   205,    -1,   207,     3,     4,     5,     6,     7,     8,     9,
    10,    11,    12,    13,    14,    15,    16,    17,    18,    19,
    20,    21,    22,    23,    24,    25,    26,    27,    28,     3,
     4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
    14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
    24,    25,    26,    27,    28,     3,     4,     5,     6,     7,
     8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
    18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
    28,     3,     4,     5,     6,     7,     8,     9,    10,    11,
    12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
    22,    23,    24,    25,    26,    27,    28,     3,     4,     5,
     6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
    16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
    26,    27,    28,     3,     4,     5,     6,     7,     8,     9,
    10,    11,    12,    13,    14,    15,    16,    17,    18,    19,
    20,    21,    22,    23,    24,    25,    26,    27,    28,     1,
    -1,    -1,    -1,    -1,    -1,     7,     8,     9,    10,    11,
    12,    -1,     1,    -1,    -1,    -1,    18,    19,     7,     8,
     9,    10,    11,    12,    -1,     1,    -1,    -1,    -1,    18,
    19,     7,     8,     9,    10,    11,    12,    -1,    -1,    -1,
    -1,    -1,    18,    19,    13,    14,    15,    16,    17,    -1,
    -1,    20,    21,    -1,    23,    24,    25,    26,    27,    28,
    29,    13,    14,    15,    16,    17,    -1,    -1,    20,    21,
    -1,    23,    24,    25,    26,    27,    28,    29,    13,    14,
    15,    16,    17,    -1,    -1,    20,    21,    -1,    23,    24,
    25,    26,    27,    28,    29,    13,    14,    15,    16,    17,
    -1,    -1,    20,    21,    -1,    23,    24,    25,    26,    27,
    28,    29,    13,    14,    15,    16,    17,    -1,    -1,    20,
    21,    -1,    23,    24,    25,    26,    27,    28,    29,     3,
    -1,     5,     6,     7,     8,     9,    10,    11,    12,     3,
    -1,     5,     6,     7,     8,     9,    10,    11,    12,     3,
    -1,     5,     6,     7,     8,     9,    10,    11,    12
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */

/* This file comes from bison-1.28.  */

/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

#ifndef YYSTACK_USE_ALLOCA
#ifdef alloca
#define YYSTACK_USE_ALLOCA
#else /* alloca not defined */
#ifdef __GNUC__
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi) || (defined (__sun) && defined (__i386))
#define YYSTACK_USE_ALLOCA
#include <alloca.h>
#else /* not sparc */
/* We think this test detects Watcom and Microsoft C.  */
/* This used to test MSDOS, but that is a bad idea
   since that symbol is in the user namespace.  */
#if (defined (_MSDOS) || defined (_MSDOS_)) && !defined (__TURBOC__)
#if 0 /* No need for malloc.h, which pollutes the namespace;
	 instead, just don't use alloca.  */
#include <malloc.h>
#endif
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
/* I don't know what this was needed for, but it pollutes the namespace.
   So I turned it off.   rms, 2 May 1997.  */
/* #include <malloc.h>  */
 #pragma alloca
#define YYSTACK_USE_ALLOCA
#else /* not MSDOS, or __TURBOC__, or _AIX */
#if 0
#ifdef __hpux /* haible@ilog.fr says this works for HPUX 9.05 and up,
		 and on HPUX 10.  Eventually we can turn this on.  */
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#endif /* __hpux */
#endif
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc */
#endif /* not GNU C */
#endif /* alloca not defined */
#endif /* YYSTACK_USE_ALLOCA not defined */

#ifdef YYSTACK_USE_ALLOCA
#define YYSTACK_ALLOC alloca
#else
#define YYSTACK_ALLOC malloc
#endif

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	goto yyacceptlab
#define YYABORT 	goto yyabortlab
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    { yychar = (token), yylval = (value);			\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { yyerror ("syntax error: cannot back up"); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		yylex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, &yylloc, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval, &yylloc)
#endif
#else /* not YYLSP_NEEDED */
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval)
#endif
#endif /* not YYLSP_NEEDED */
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	yychar;			/*  the lookahead symbol		*/
YYSTYPE	yylval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE yylloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int yynerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int yydebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Define __yy_memcpy.  Note that the size argument
   should be passed with type unsigned int, because that is what the non-GCC
   definitions require.  With GCC, __builtin_memcpy takes an arg
   of type size_t, but it can handle unsigned int.  */

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __yy_memcpy(TO,FROM,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (to, from, count)
     char *to;
     char *from;
     unsigned int count;
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (char *to, char *from, unsigned int count)
{
  register char *t = to;
  register char *f = from;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif



/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
#ifdef __cplusplus
#define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#else /* not __cplusplus */
#define YYPARSE_PARAM_ARG YYPARSE_PARAM
#define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
#endif /* not __cplusplus */
#else /* not YYPARSE_PARAM */
#define YYPARSE_PARAM_ARG
#define YYPARSE_PARAM_DECL
#endif /* not YYPARSE_PARAM */

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
#ifdef YYPARSE_PARAM
int yyparse (void *);
#else
int yyparse (void);
#endif
#endif

int
yyparse(YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;
  int yyerrstatus;	/*  number of tokens to shift before error messages enabled */
  int yychar1 = 0;		/*  lookahead token as an internal (translated) token number */

  short	yyssa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE yyvsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *yyss = yyssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;	/*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE yylsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  int yystacksize = YYINITDEPTH;
  int yyfree_stacks = 0;

#ifdef YYPURE
  int yychar;
  YYSTYPE yylval;
  int yynerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE yylloc;
#endif
#endif

  YYSTYPE yyval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int yylen;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#ifdef YYLSP_NEEDED
  yylsp = yyls;
#endif

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;
#ifdef YYLSP_NEEDED
      YYLTYPE *yyls1 = yyls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if yyoverflow is a macro.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yyls1, size * sizeof (*yylsp),
		 &yystacksize);
#else
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yystacksize);
#endif

      yyss = yyss1; yyvs = yyvs1;
#ifdef YYLSP_NEEDED
      yyls = yyls1;
#endif
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	{
	  yyerror("parser stack overflow");
	  if (yyfree_stacks)
	    {
	      free (yyss);
	      free (yyvs);
#ifdef YYLSP_NEEDED
	      free (yyls);
#endif
	    }
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
#ifndef YYSTACK_USE_ALLOCA
      yyfree_stacks = 1;
#endif
      yyss = (short *) YYSTACK_ALLOC (yystacksize * sizeof (*yyssp));
      __yy_memcpy ((char *)yyss, (char *)yyss1,
		   size * (unsigned int) sizeof (*yyssp));
      yyvs = (YYSTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yyvsp));
      __yy_memcpy ((char *)yyvs, (char *)yyvs1,
		   size * (unsigned int) sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yylsp));
      __yy_memcpy ((char *)yyls, (char *)yyls1,
		   size * (unsigned int) sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

  goto yybackup;
 yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Reading a token: ");
#endif
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
      if (yydebug)
	{
	  fprintf (stderr, "Next token is %d (%s", yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
yyreduce:
  yylen = yyr2[yyn];
  if (yylen > 0)
    yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (yydebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
	fprintf (stderr, "%s ", yytname[yyrhs[i]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif


  switch (yyn) {

case 27:
{
			yyval.d = 0.0;
		;
    break;}
case 28:
{
			yyval.d = 1.0;
		;
    break;}
case 29:
{
			yyval.d = 2.0;
		;
    break;}
case 30:
{
			yyval.d = 3.0;
		;
    break;}
case 31:
{
			yyval.d = 4.0;
		;
    break;}
case 32:
{
			yyval.d = 5.0;
		;
    break;}
case 33:
{
			yyval.d = 1.0 * (yyvsp[0].i);
#ifndef NO_COMMENTS
			printf("tINT: %f\n",yyvsp[0].i);
#endif
		;
    break;}
case 34:
{
			yyval.d =1.0 * (yyvsp[0].d);
#ifndef NO_COMMENTS
			printf("tFLOAT: %f\n",yyvsp[0].d);
#endif
		;
    break;}
case 35:
{
		  if ((ldraw_commandline_opts.output == 1)&&(include_stack_ptr <= ldraw_commandline_opts.output_depth )) {
		    fprintf(output_file,"%d %s %s\n",yyvsp[-3].i, yyvsp[-2].c, yyvsp[-1].c);
		  } else {
			zStep(stepcount,1);
			stepcount++;
          }
#ifdef USE_QBUF_MALLOC
		    qbufReleasePtr(word_pool, (char *)yyvsp[-1].c);
#else
		    free(yyvsp[-1].c);
#endif
		;
    break;}
case 36:
{
		  if ((ldraw_commandline_opts.output == 1)&&(include_stack_ptr <= ldraw_commandline_opts.output_depth )) {
		    fprintf(output_file,"%d %s %s\n",yyvsp[-3].i, yyvsp[-2].c, yyvsp[-1].c);
		  } else {
			zStep(stepcount,1);
			stepcount++;
          }
#ifdef USE_QBUF_MALLOC
		    qbufReleasePtr(word_pool, (char *)yyvsp[-1].c);
#else
		    free(yyvsp[-1].c);
#endif
		  if (stop_include_file() == (-1)) { YYACCEPT; };
		;
    break;}
case 37:
{
		  if ((ldraw_commandline_opts.output == 1)&&(include_stack_ptr <= ldraw_commandline_opts.output_depth )) {
		    fprintf(output_file,"%d %s %s\n",yyvsp[-3].i, yyvsp[-2].c, yyvsp[-1].c);
	      } else {
			zPause();
          }
#ifdef USE_QBUF_MALLOC
		    qbufReleasePtr(word_pool, (char *)yyvsp[-1].c);
#else
		    free(yyvsp[-1].c);
#endif
		;
    break;}
case 38:
{
		  if ((ldraw_commandline_opts.output == 1)&&(include_stack_ptr <= ldraw_commandline_opts.output_depth )) {
		    fprintf(output_file,"%d %s %s\n",yyvsp[-3].i, yyvsp[-2].c, yyvsp[-1].c);
	      } else {
			zPause();
          }
#ifdef USE_QBUF_MALLOC
		    qbufReleasePtr(word_pool, (char *)yyvsp[-1].c);
#else
		    free(yyvsp[-1].c);
#endif
			if (stop_include_file() == (-1)) { YYACCEPT; };
		;
    break;}
case 39:
{
		  if ((ldraw_commandline_opts.output == 1)&&(include_stack_ptr <= ldraw_commandline_opts.output_depth )) {
		    fprintf(output_file,"%d %s %s\n",yyvsp[-3].i, yyvsp[-2].c, yyvsp[-1].c);
	      } else {
			zWrite(yyvsp[-1].c);
          }
#ifdef USE_QBUF_MALLOC
		    qbufReleasePtr(word_pool, (char *)yyvsp[-1].c);
#else
		    free(yyvsp[-1].c);
#endif
		;
    break;}
case 40:
{
		  if ((ldraw_commandline_opts.output == 1)&&(include_stack_ptr <= ldraw_commandline_opts.output_depth )) {
		    fprintf(output_file,"%d %s %s\n",yyvsp[-3].i, yyvsp[-2].c, yyvsp[-1].c);
	      } else {
			zWrite(yyvsp[-1].c);
          }
#ifdef USE_QBUF_MALLOC
		    qbufReleasePtr(word_pool, (char *)yyvsp[-1].c);
#else
		    free(yyvsp[-1].c);
#endif
			if (stop_include_file() == (-1)) { YYACCEPT; };
		;
    break;}
case 41:
{
		  if ((ldraw_commandline_opts.output == 1)&&(include_stack_ptr <= ldraw_commandline_opts.output_depth )) {
		    fprintf(output_file,"%d %s %s\n",yyvsp[-3].i, yyvsp[-2].c, yyvsp[-1].c);
	      } else {
			zClear();
		  }
#ifdef USE_QBUF_MALLOC
		    qbufReleasePtr(word_pool, (char *)yyvsp[-1].c);
#else
		    free(yyvsp[-1].c);
#endif
		;
    break;}
case 42:
{
		  if ((ldraw_commandline_opts.output == 1)&&(include_stack_ptr <= ldraw_commandline_opts.output_depth )) {
		    fprintf(output_file,"%d %s %s\n",yyvsp[-3].i, yyvsp[-2].c, yyvsp[-1].c);
	      } else {
			zClear();
		  }
#ifdef USE_QBUF_MALLOC
		    qbufReleasePtr(word_pool, (char *)yyvsp[-1].c);
#else
		    free(yyvsp[-1].c);
#endif
			if (stop_include_file() == (-1)) { YYACCEPT; };
		;
    break;}
case 43:
{
		  if ((ldraw_commandline_opts.output == 1)&&(include_stack_ptr <= ldraw_commandline_opts.output_depth )) {
		    fprintf(output_file,"%d not supported: %s %g %s %g %g %g %g %g %g %g %g %g\n",
			  yyvsp[-13].i, yyvsp[-12].c, yyvsp[-11].d, yyvsp[-10].c, yyvsp[-9].d, yyvsp[-8].d, yyvsp[-7].d, yyvsp[-6].d, yyvsp[-5].d, yyvsp[-4].d, yyvsp[-3].d, yyvsp[-2].d, yyvsp[-1].d);
			/* alias the new color name to grey */
			zcolor_alias(7,yyvsp[-10].c);
	      } else {
			zcolor_modify((int)yyvsp[-11].d,yyvsp[-10].c,(int)yyvsp[-9].d,(int)yyvsp[-8].d,(int)yyvsp[-7].d,(int)yyvsp[-6].d,(int)yyvsp[-5].d,(int)yyvsp[-4].d,(int)yyvsp[-3].d,(int)yyvsp[-2].d,(int)yyvsp[-1].d);
          }
		;
    break;}
case 44:
{
		  if ((ldraw_commandline_opts.output == 1)&&(include_stack_ptr <= ldraw_commandline_opts.output_depth )) {
		    fprintf(output_file,"%d not supported: %s %g %s %g %g %g %g %g %g %g %g %g\n",
			  yyvsp[-13].i, yyvsp[-12].c, yyvsp[-11].d, yyvsp[-10].c, yyvsp[-9].d, yyvsp[-8].d, yyvsp[-7].d, yyvsp[-6].d, yyvsp[-5].d, yyvsp[-4].d, yyvsp[-3].d, yyvsp[-2].d, yyvsp[-1].d);
			/* alias the new color name to grey */
			zcolor_alias(7,yyvsp[-10].c);
	      } else {
			zcolor_modify((int)yyvsp[-11].d,yyvsp[-10].c,(int)yyvsp[-9].d,(int)yyvsp[-8].d,(int)yyvsp[-7].d,(int)yyvsp[-6].d,(int)yyvsp[-5].d,(int)yyvsp[-4].d,(int)yyvsp[-3].d,(int)yyvsp[-2].d,(int)yyvsp[-1].d);
          }
			if (stop_include_file() == (-1)) { YYACCEPT; };
		;
    break;}
case 45:
{
			zcolor_alias((int)yyvsp[-1].i,yyvsp[-2].c);
		    if (is_current_file_not_cached()) {
#ifdef USE_QBUF_MALLOC
		      qbufReleasePtr(word_pool, (char *)yyvsp[-2].c);
#else
		      free(yyvsp[-2].c);
#endif
		    }
		;
    break;}
case 46:
{
			zcolor_alias((int)yyvsp[-1].i,yyvsp[-2].c);
		    if (is_current_file_not_cached()) {
#ifdef USE_QBUF_MALLOC
		      qbufReleasePtr(word_pool, (char *)yyvsp[-2].c);
#else
		      free(yyvsp[-2].c);
#endif
		    }
			if (stop_include_file() == (-1)) { YYACCEPT; };
		;
    break;}
case 47:
{
			zpoint_alias(yyvsp[-2].c,yyvsp[-1].v);
		    if (is_current_file_not_cached()) {
#ifdef USE_QBUF_MALLOC
		      qbufReleasePtr(word_pool, (char *)yyvsp[-2].c);
#else
		      free(yyvsp[-2].c);
#endif
		    }
		;
    break;}
case 48:
{
			zpoint_alias(yyvsp[-2].c,yyvsp[-1].v);
		    if (is_current_file_not_cached()) {
#ifdef USE_QBUF_MALLOC
		      qbufReleasePtr(word_pool, (char *)yyvsp[-2].c);
#else
		      free(yyvsp[-2].c);
#endif
            }
			if (stop_include_file() == (-1)) { YYACCEPT; };
		;
    break;}
case 49:
{
			zmatrix_alias(yyvsp[-2].c,yyvsp[-1].m);
		    if (is_current_file_not_cached()) {
#ifdef USE_QBUF_MALLOC
		      qbufReleasePtr(word_pool, (char *)yyvsp[-2].c);
#else
		      free(yyvsp[-2].c);
#endif
		    }
		;
    break;}
case 50:
{
			zmatrix_alias(yyvsp[-2].c,yyvsp[-1].m);
		    if (is_current_file_not_cached()) {
#ifdef USE_QBUF_MALLOC
		      qbufReleasePtr(word_pool, (char *)yyvsp[-2].c);
#else
		      free(yyvsp[-2].c);
#endif
		    }
			if (stop_include_file() == (-1)) { YYACCEPT; };
		;
    break;}
case 51:
{
            if (cache_mpd_subfiles(yyvsp[-1].c) == 0) {
			  mpd_subfile_name = yyvsp[-1].c;
			} else {
#ifdef USE_QBUF_MALLOC
		    qbufReleasePtr(word_pool, (char *)yyvsp[-1].c);
#else
		    free(yyvsp[-1].c);
#endif
            }
			YYACCEPT;
		;
    break;}
case 52:
{
            if (cache_mpd_subfiles(yyvsp[-1].c) == 0) {
			  mpd_subfile_name = yyvsp[-1].c;
			} else {
#ifdef USE_QBUF_MALLOC
		      qbufReleasePtr(word_pool, (char *)yyvsp[-1].c);
#else
		      free(yyvsp[-1].c);
#endif
            }
			YYACCEPT;
		;
    break;}
case 53:
{
		  if ((ldraw_commandline_opts.output == 1)&&(include_stack_ptr <= ldraw_commandline_opts.output_depth )) {
		    fprintf(output_file,"%d %s %s\n",yyvsp[-3].i, yyvsp[-2].c, yyvsp[-1].c);
	      } else {
			zSave(stepcount);
		  }
			stepcount++;
#ifdef USE_QBUF_MALLOC
		    qbufReleasePtr(word_pool, (char *)yyvsp[-1].c);
#else
		    free(yyvsp[-1].c);
#endif
		;
    break;}
case 54:
{
		  if ((ldraw_commandline_opts.output == 1)&&(include_stack_ptr <= ldraw_commandline_opts.output_depth )) {
		    fprintf(output_file,"%d %s %s\n",yyvsp[-3].i, yyvsp[-2].c, yyvsp[-1].c);
	      } else {
			zSave(stepcount);
		  }
			stepcount++;
#ifdef USE_QBUF_MALLOC
		    qbufReleasePtr(word_pool, (char *)yyvsp[-1].c);
#else
		    free(yyvsp[-1].c);
#endif
			if (stop_include_file() == (-1)) { YYACCEPT; };
		;
    break;}
case 55:
{
		push_transform(yyvsp[-1].v, NULL);
		;
    break;}
case 56:
{
		push_transform(yyvsp[-1].v, NULL);
		if (stop_include_file() == (-1)) { YYACCEPT; };
		;
    break;}
case 57:
{
		pop_transform();
		;
    break;}
case 58:
{
		pop_transform();
		if (stop_include_file() == (-1)) { YYACCEPT; };
		;
    break;}
case 59:
{
		  /* axis degree representation */
		  push_rotation(yyvsp[-1].v, yyvsp[-2].d);
		;
    break;}
case 60:
{
		  /* axis degree representation */
		  push_rotation(yyvsp[-1].v, yyvsp[-2].d);
		  if (stop_include_file() == (-1)) { YYACCEPT; };
		;
    break;}
case 61:
{
			pop_transform();
		;
    break;}
case 62:
{
		    pop_transform();
		if (stop_include_file() == (-1)) { YYACCEPT; };
		;
    break;}
case 63:
{
		push_transform(NULL, savemat((float)(yyvsp[-1].d),0,0,0,(float)(yyvsp[-1].d),0,0,0,(float)(yyvsp[-1].d)));
		;
    break;}
case 64:
{
		push_transform(NULL, savemat((float)(yyvsp[-1].d),0,0,0,(float)(yyvsp[-1].d),0,0,0,(float)(yyvsp[-1].d)));
		if (stop_include_file() == (-1)) { YYACCEPT; };
		;
    break;}
case 65:
{
		pop_transform();
		;
    break;}
case 66:
{
		pop_transform();
		if (stop_include_file() == (-1)) { YYACCEPT; };
		;
    break;}
case 67:
{
		  push_transform(yyvsp[-2].v, yyvsp[-1].m);
		;
    break;}
case 68:
{
		  push_transform(yyvsp[-2].v, yyvsp[-1].m);
		  if (stop_include_file() == (-1)) { YYACCEPT; };
		;
    break;}
case 69:
{
		pop_transform();
		;
    break;}
case 70:
{
		pop_transform();
		if (stop_include_file() == (-1)) { YYACCEPT; };
		;
    break;}
case 71:
{
		  if (is_current_file_not_cached()) {
			yyval.c = yyvsp[0].c;
		  } else {
#ifdef USE_QBUF_MALLOC
		    yyval.c = (char *)qbufGetPtr(word_pool);
#else
	        yyval.c = malloc(128);
#endif
	        strcpy(yyval.c,yyvsp[0].c);
		  }
#if 0
		  fprintf(stderr,"Got word \"%s\"\n",yyvsp[0].c);
		  fflush(stderr);
#endif
		 ;
    break;}
case 72:
{ 
		  if (is_current_file_not_cached()) {
			yyval.c = yyvsp[0].c;
		  } else {
#ifdef USE_QBUF_MALLOC
		    yyval.c = (char *)qbufGetPtr(word_pool);
#else
	        yyval.c = malloc(128);
#endif
	        strcpy(yyval.c,yyvsp[0].c);
		  }
#if 0
          fprintf(stderr,"Got garbage characters \"%s\"\n",yyvsp[0].c);
	      fflush(stderr);
#endif
		;
    break;}
case 73:
{ 
#ifdef USE_QBUF_MALLOC
	      yyval.c = (char *)qbufGetPtr(word_pool);
#else
	      yyval.c = malloc(128);
#endif
		  sprintf(yyval.c,"%g",yyvsp[0].d); 
#if 0
		  fprintf(stderr,"Got word \"%s\"\n",yyval.c);
		  fflush(stderr);
#endif
		;
    break;}
case 74:
{
#ifdef USE_QBUF_MALLOC
	      yyval.c = (char *)qbufGetPtr(word_pool);
#else
	      yyval.c = malloc(128);
#endif
		  strcpy(yyval.c,yyvsp[0].c);
		;
    break;}
case 75:
{
#ifdef USE_QBUF_MALLOC
	      yyval.c = (char *)qbufGetPtr(word_pool);
#else
	      yyval.c = malloc(128);
#endif
		  strcpy(yyval.c,yyvsp[0].c);
		;
    break;}
case 76:
{
#ifdef USE_QBUF_MALLOC
	      yyval.c = (char *)qbufGetPtr(word_pool);
#else
	      yyval.c = malloc(128);
#endif
		  strcpy(yyval.c,yyvsp[0].c);
		;
    break;}
case 77:
{
#ifdef USE_QBUF_MALLOC
	      yyval.c = (char *)qbufGetPtr(word_pool);
#else
	      yyval.c = malloc(128);
#endif
		  strcpy(yyval.c,yyvsp[0].c);
		;
    break;}
case 78:
{
#ifdef USE_QBUF_MALLOC
	      yyval.c = (char *)qbufGetPtr(word_pool);
#else
	      yyval.c = malloc(128);
#endif
		  strcpy(yyval.c,yyvsp[0].c);
		;
    break;}
case 79:
{
#ifdef USE_QBUF_MALLOC
	      yyval.c = (char *)qbufGetPtr(word_pool);
#else
	      yyval.c = malloc(128);
#endif
		  strcpy(yyval.c,yyvsp[0].c);
		;
    break;}
case 80:
{
#ifdef USE_QBUF_MALLOC
	      yyval.c = (char *)qbufGetPtr(word_pool);
#else
	      yyval.c = malloc(128);
#endif
		  strcpy(yyval.c,yyvsp[0].c);
		;
    break;}
case 81:
{
#ifdef USE_QBUF_MALLOC
	      yyval.c = (char *)qbufGetPtr(word_pool);
#else
	      yyval.c = malloc(128);
#endif
		  strcpy(yyval.c,yyvsp[0].c);
		;
    break;}
case 82:
{
#ifdef USE_QBUF_MALLOC
	      yyval.c = (char *)qbufGetPtr(word_pool);
#else
	      yyval.c = malloc(128);
#endif
		  strcpy(yyval.c,yyvsp[0].c);
		;
    break;}
case 83:
{
#ifdef USE_QBUF_MALLOC
	      yyval.c = (char *)qbufGetPtr(word_pool);
#else
	      yyval.c = malloc(128);
#endif
		  strcpy(yyval.c,yyvsp[0].c);
		;
    break;}
case 84:
{
#ifdef USE_QBUF_MALLOC
	      yyval.c = (char *)qbufGetPtr(word_pool);
#else
	      yyval.c = malloc(128);
#endif
		  strcpy(yyval.c,yyvsp[0].c);
		;
    break;}
case 85:
{
#ifdef USE_QBUF_MALLOC
	      yyval.c = (char *)qbufGetPtr(word_pool);
#else
	      yyval.c = malloc(128);
#endif
		  strcpy(yyval.c,yyvsp[0].c);
		;
    break;}
case 86:
{
#ifdef USE_QBUF_MALLOC
	      yyval.c = (char *)qbufGetPtr(word_pool);
#else
	      yyval.c = malloc(128);
#endif
		  strcpy(yyval.c,yyvsp[0].c);
		;
    break;}
case 87:
{
#ifdef USE_QBUF_MALLOC
	      yyval.c = (char *)qbufGetPtr(word_pool);
#else
	      yyval.c = malloc(128);
#endif
		  strcpy(yyval.c,yyvsp[0].c);
		;
    break;}
case 88:
{
#ifdef USE_QBUF_MALLOC
	      yyval.c = (char *)qbufGetPtr(word_pool);
#else
	      yyval.c = malloc(128);
#endif
		  strcpy(yyval.c,"");
		;
    break;}
case 89:
{ 
		// any Word string should be long enough hold all concatinated Words,
		// so we do not need to malloc a new string, just strcat them.
		yyval.c = yyvsp[-1].c;
		strcat(yyvsp[-1].c, " ");
		strcat(yyvsp[-1].c, yyvsp[0].c);
#ifdef USE_QBUF_MALLOC
		  qbufReleasePtr(word_pool, (char *)yyvsp[0].c);
#else
		  free(yyvsp[0].c);
#endif
		;
    break;}
case 90:
{
#ifndef NO_COMMENTS
		printf("# %s\n",yyvsp[-1].c);
		fflush(stdout);
#endif
		if ((ldraw_commandline_opts.output == 1)&&(include_stack_ptr <= ldraw_commandline_opts.output_depth )) {
		  fprintf(output_file,"%d %s\n",yyvsp[-2].i, yyvsp[-1].c);
	    } else {
		  platform_comment(yyvsp[-1].c, include_stack_ptr);
        }
#ifdef USE_QBUF_MALLOC
		qbufReleasePtr(word_pool, (char *)yyvsp[-1].c);
#else
		free(yyvsp[-1].c);
#endif
		;
    break;}
case 91:
{
#ifndef NO_COMMENTS
		printf("# %s\n",yyvsp[-1].c);
		fflush(stdout);
#endif
		if ((ldraw_commandline_opts.output == 1)&&(include_stack_ptr <= ldraw_commandline_opts.output_depth )) {
		  fprintf(output_file,"%d %s\n",yyvsp[-2].i, yyvsp[-1].c);
	    } else {
 		  platform_comment(yyvsp[-1].c, include_stack_ptr);
		}
#ifdef USE_QBUF_MALLOC
		qbufReleasePtr(word_pool, (char *)yyvsp[-1].c);
#else
		free(yyvsp[-1].c);
#endif
		if (stop_include_file() == (-1)) { YYACCEPT; };
		;
    break;}
case 92:
{
		;
    break;}
case 93:
{
			if (stop_include_file() == (-1)) { YYACCEPT; };
		;
    break;}
case 94:
{
			if (yyvsp[0].d == 16.0) {
			  yyval.i = current_color[include_stack_ptr];
			} else if (yyvsp[0].d == 24.0) {
			  yyval.i = edge_color(current_color[include_stack_ptr]);
			} else {
			  yyval.i = (int) yyvsp[0].d;
			}
		;
    break;}
case 95:
{
			yyval.i = zcolor_lookup(yyvsp[0].c);
		    if (is_current_file_not_cached()) {
#ifdef USE_QBUF_MALLOC
		      qbufReleasePtr(word_pool, (char *)yyvsp[0].c);
#else
		      free(yyvsp[0].c);
#endif
		    }
		;
    break;}
case 96:
{
#if 1
			yyval.m = savemat(yyvsp[-8].d,yyvsp[-7].d,yyvsp[-6].d,yyvsp[-5].d,yyvsp[-4].d,yyvsp[-3].d,yyvsp[-2].d,yyvsp[-1].d,yyvsp[0].d);
#else
			yyval.m = savemat(yyvsp[0].d,yyvsp[-1].d,yyvsp[-2].d,yyvsp[-3].d,yyvsp[-4].d,yyvsp[-5].d,yyvsp[-6].d,yyvsp[-7].d,yyvsp[-8].d);
#endif
		;
    break;}
case 97:
{
		    yyval.m = zmatrix_lookup(yyvsp[0].c);
		    if (is_current_file_not_cached()) {
#ifdef USE_QBUF_MALLOC
		      qbufReleasePtr(word_pool, (char *)yyvsp[0].c);
#else
		      free(yyvsp[0].c);
#endif
		    }
		;
    break;}
case 98:
{
			double a, b, c;
			vector3d *v;
			a = yyvsp[-2].d;
			b = yyvsp[-1].d;
			c = yyvsp[0].d;
			v = savevec(a,b,c);
			yyval.v = v;
#ifndef NO_COMMENTS
			printf("Position (%f, %f, %f)\n",v->x, v->y, v->z);
#endif
		;
    break;}
case 99:
{
		    yyval.v = zpoint_lookup(yyvsp[0].c);
		    if (is_current_file_not_cached()) {
#ifdef USE_QBUF_MALLOC
		      qbufReleasePtr(word_pool, (char *)yyvsp[0].c);
#else
		      free(yyvsp[0].c);
#endif
		    }
		;
    break;}
case 100:
{
		  if (is_current_file_not_cached()) {
			yyval.c = yyvsp[0].c;
		  } else {
#ifdef USE_QBUF_MALLOC
		    yyval.c = (char *)qbufGetPtr(word_pool);
#else
	        yyval.c = malloc(128);
#endif
	        strcpy(yyval.c,yyvsp[0].c);
		  }
		;
    break;}
case 101:
{
		if (ldraw_commandline_opts.output == 1) {
			if (include_stack_ptr >= ldraw_commandline_opts.output_depth ) {
  				transform_vec_inplace(yyvsp[-3].v);
  				transform_mat_inplace(yyvsp[-2].m);
				fprintf(output_file,"%d %d %g %g %g %g %g %g %g %g %g %g %g %g %s\n",
					yyvsp[-5].i, yyvsp[-4].i, (yyvsp[-3].v)->x, (yyvsp[-3].v)->y, (yyvsp[-3].v)->z,
					(yyvsp[-2].m)->a, (yyvsp[-2].m)->b, (yyvsp[-2].m)->c, (yyvsp[-2].m)->d, (yyvsp[-2].m)->e, (yyvsp[-2].m)->f, 
					(yyvsp[-2].m)->g, (yyvsp[-2].m)->h, (yyvsp[-2].m)->i, yyvsp[-1].c);
			} else {
				if (start_include_file(yyvsp[-1].c) == 0) {
					fprintf(output_file,"0 inlining %s\n",yyvsp[-1].c);
	  				/* update transform matricies */
					push_transform(yyvsp[-3].v, yyvsp[-2].m);
					znamelist_push();
					current_color[include_stack_ptr] = yyvsp[-4].i;
				} else {
#ifndef NO_COMMENTS
				printf("# Cannot find %s, ignoring.\n",yyvsp[-1].c);
#endif
				}
			}
	    } else {
	      if (start_include_file(yyvsp[-1].c) == 0) {
	  	  /* update transform matricies */
		  push_transform(yyvsp[-3].v, yyvsp[-2].m);
		  znamelist_push();
		  current_color[include_stack_ptr] = yyvsp[-4].i;
		  } else {
#ifndef NO_COMMENTS
		  printf("# Cannot find %s, ignoring.\n",yyvsp[-1].c);
#endif
		  }
		}
#ifdef USE_QBUF_MALLOC
		qbufReleasePtr(word_pool, (char *)yyvsp[-1].c);
#else
		    free(yyvsp[-1].c);
#endif
		;
    break;}
case 102:
{
		if (ldraw_commandline_opts.output == 1) {
			if (include_stack_ptr >= ldraw_commandline_opts.output_depth ) {
  				transform_vec_inplace(yyvsp[-3].v);
  				transform_mat_inplace(yyvsp[-2].m);
				fprintf(output_file,"%d %d %g %g %g %g %g %g %g %g %g %g %g %g %s\n",
					yyvsp[-5].i, yyvsp[-4].i, (yyvsp[-3].v)->x, (yyvsp[-3].v)->y, (yyvsp[-3].v)->z,
					(yyvsp[-2].m)->a, (yyvsp[-2].m)->b, (yyvsp[-2].m)->c, (yyvsp[-2].m)->d, (yyvsp[-2].m)->e, (yyvsp[-2].m)->f, 
					(yyvsp[-2].m)->g, (yyvsp[-2].m)->h, (yyvsp[-2].m)->i, yyvsp[-1].c);
			} else {
				if (start_include_file(yyvsp[-1].c) == 0) {
					fprintf(output_file,"0 inlining %s\n",yyvsp[-1].c);
	  				/* update transform matricies */
					push_transform(yyvsp[-3].v, yyvsp[-2].m);
					znamelist_push();
					current_color[include_stack_ptr] = yyvsp[-4].i;
					defer_stop_include_file();
				} else {
#ifndef NO_COMMENTS
					printf("# Cannot find %s, ignoring.\n",yyvsp[-1].c);
#endif
					if (stop_include_file() == (-1)) { YYACCEPT; };
				}
			}
		} else {
		  if (start_include_file(yyvsp[-1].c) == 0) {
		    /* update transform matricies */
		    push_transform(yyvsp[-3].v, yyvsp[-2].m);
		    znamelist_push();
		    current_color[include_stack_ptr] = yyvsp[-4].i;
		  } else {
#ifndef NO_COMMENTS
		  printf("# Cannot find %s, ignoring.\n",yyvsp[-1].c);
#endif
		  }
	    }
#ifdef USE_QBUF_MALLOC
		qbufReleasePtr(word_pool, (char *)yyvsp[-1].c);
#else
		free(yyvsp[-1].c);
#endif
		;
    break;}
case 103:
{
		transform_vec_inplace(yyvsp[-2].v);
		transform_vec_inplace(yyvsp[-1].v);
		if ((ldraw_commandline_opts.output == 1)&&(include_stack_ptr <= ldraw_commandline_opts.output_depth )) {
		  fprintf(output_file,"%d %d %g %g %g %g %g %g\n",
		  yyvsp[-4].i, yyvsp[-3].i, (yyvsp[-2].v)->x, (yyvsp[-2].v)->y, (yyvsp[-2].v)->z,
		  (yyvsp[-1].v)->x, (yyvsp[-1].v)->y, (yyvsp[-1].v)->z);
	    } else {
		  render_line(yyvsp[-2].v, yyvsp[-1].v, yyvsp[-3].i);
		}
#ifdef USE_QBUF_MALLOC
		qbufReleasePtr(vector_pool, (char *)yyvsp[-2].v);
		qbufReleasePtr(vector_pool, (char *)yyvsp[-1].v);
#else
		free(yyvsp[-2].v);
		free(yyvsp[-1].v);
#endif
		;
    break;}
case 104:
{
		transform_vec_inplace(yyvsp[-2].v);
		transform_vec_inplace(yyvsp[-1].v);
		if ((ldraw_commandline_opts.output == 1)&&(include_stack_ptr <= ldraw_commandline_opts.output_depth )) {
		  fprintf(output_file,"%d %d %g %g %g %g %g %g\n",
		  yyvsp[-4].i, yyvsp[-3].i, (yyvsp[-2].v)->x, (yyvsp[-2].v)->y, (yyvsp[-2].v)->z,
		  (yyvsp[-1].v)->x, (yyvsp[-1].v)->y, (yyvsp[-1].v)->z);
	    } else {
		  render_line(yyvsp[-2].v, yyvsp[-1].v, yyvsp[-3].i);
		}
#ifdef USE_QBUF_MALLOC
		qbufReleasePtr(vector_pool, (char *)yyvsp[-2].v);
		qbufReleasePtr(vector_pool, (char *)yyvsp[-1].v);
#else
		free(yyvsp[-2].v);
		free(yyvsp[-1].v);
#endif
		if (stop_include_file() == (-1)) { YYACCEPT; };
		;
    break;}
case 105:
{
		transform_vec_inplace(yyvsp[-3].v);
		transform_vec_inplace(yyvsp[-2].v);
		transform_vec_inplace(yyvsp[-1].v);
		if ((ldraw_commandline_opts.output == 1)&&(include_stack_ptr <= ldraw_commandline_opts.output_depth )) {
		  fprintf(output_file,"%d %d %g %g %g %g %g %g %g %g %g\n",
		  yyvsp[-5].i, yyvsp[-4].i, (yyvsp[-3].v)->x, (yyvsp[-3].v)->y, (yyvsp[-3].v)->z,
		  (yyvsp[-2].v)->x, (yyvsp[-2].v)->y, (yyvsp[-2].v)->z, (yyvsp[-1].v)->x, (yyvsp[-1].v)->y, (yyvsp[-1].v)->z);
	    } else {
		  render_triangle(yyvsp[-3].v, yyvsp[-2].v, yyvsp[-1].v, yyvsp[-4].i);
		}
#ifdef USE_QBUF_MALLOC
		qbufReleasePtr(vector_pool, (char *)yyvsp[-3].v);
		qbufReleasePtr(vector_pool, (char *)yyvsp[-2].v);
		qbufReleasePtr(vector_pool, (char *)yyvsp[-1].v);
#else
		free(yyvsp[-3].v);
		free(yyvsp[-2].v);
		free(yyvsp[-1].v);
#endif
		;
    break;}
case 106:
{
		transform_vec_inplace(yyvsp[-3].v);
		transform_vec_inplace(yyvsp[-2].v);
		transform_vec_inplace(yyvsp[-1].v);
		if ((ldraw_commandline_opts.output == 1)&&(include_stack_ptr <= ldraw_commandline_opts.output_depth )) {
		  fprintf(output_file,"%d %d %g %g %g %g %g %g %g %g %g\n",
		  yyvsp[-5].i, yyvsp[-4].i, (yyvsp[-3].v)->x, (yyvsp[-3].v)->y, (yyvsp[-3].v)->z,
		  (yyvsp[-2].v)->x, (yyvsp[-2].v)->y, (yyvsp[-2].v)->z, (yyvsp[-1].v)->x, (yyvsp[-1].v)->y, (yyvsp[-1].v)->z);
	    } else {
		  render_triangle(yyvsp[-3].v, yyvsp[-2].v, yyvsp[-1].v, yyvsp[-4].i);
		}
#ifdef USE_QBUF_MALLOC
		qbufReleasePtr(vector_pool, (char *)yyvsp[-3].v);
		qbufReleasePtr(vector_pool, (char *)yyvsp[-2].v);
		qbufReleasePtr(vector_pool, (char *)yyvsp[-1].v);
#else
		free(yyvsp[-3].v);
		free(yyvsp[-2].v);
		free(yyvsp[-1].v);
#endif
		if (stop_include_file() == (-1)) { YYACCEPT; };
		;
    break;}
case 107:
{
		transform_vec_inplace(yyvsp[-4].v);
		transform_vec_inplace(yyvsp[-3].v);
		transform_vec_inplace(yyvsp[-2].v);
		transform_vec_inplace(yyvsp[-1].v);
		if ((ldraw_commandline_opts.output == 1)&&(include_stack_ptr <= ldraw_commandline_opts.output_depth )) {
		  fprintf(output_file,"%d %d %g %g %g %g %g %g %g %g %g %g %g %g\n",
		  yyvsp[-6].i, yyvsp[-5].i, (yyvsp[-4].v)->x, (yyvsp[-4].v)->y, (yyvsp[-4].v)->z,
		  (yyvsp[-3].v)->x, (yyvsp[-3].v)->y, (yyvsp[-3].v)->z, (yyvsp[-2].v)->x, (yyvsp[-2].v)->y, (yyvsp[-2].v)->z,
		  (yyvsp[-1].v)->x, (yyvsp[-1].v)->y, (yyvsp[-1].v)->z);
	    } else {
  		  render_quad(yyvsp[-4].v, yyvsp[-3].v, yyvsp[-2].v, yyvsp[-1].v, yyvsp[-5].i);
		}
#ifdef USE_QBUF_MALLOC
		qbufReleasePtr(vector_pool, (char *)yyvsp[-4].v);
		qbufReleasePtr(vector_pool, (char *)yyvsp[-3].v);
		qbufReleasePtr(vector_pool, (char *)yyvsp[-2].v);
		qbufReleasePtr(vector_pool, (char *)yyvsp[-1].v);
#else
		free(yyvsp[-4].v);
		free(yyvsp[-3].v);
		free(yyvsp[-2].v);
		free(yyvsp[-1].v);
#endif
		;
    break;}
case 108:
{
		transform_vec_inplace(yyvsp[-4].v);
		transform_vec_inplace(yyvsp[-3].v);
		transform_vec_inplace(yyvsp[-2].v);
		transform_vec_inplace(yyvsp[-1].v);
		if ((ldraw_commandline_opts.output == 1)&&(include_stack_ptr <= ldraw_commandline_opts.output_depth )) {
		  fprintf(output_file,"%d %d %g %g %g %g %g %g %g %g %g %g %g %g\n",
		  yyvsp[-6].i, yyvsp[-5].i, (yyvsp[-4].v)->x, (yyvsp[-4].v)->y, (yyvsp[-4].v)->z,
		  (yyvsp[-3].v)->x, (yyvsp[-3].v)->y, (yyvsp[-3].v)->z, (yyvsp[-2].v)->x, (yyvsp[-2].v)->y, (yyvsp[-2].v)->z,
		  (yyvsp[-1].v)->x, (yyvsp[-1].v)->y, (yyvsp[-1].v)->z);
	    } else {
		  render_quad(yyvsp[-4].v, yyvsp[-3].v, yyvsp[-2].v, yyvsp[-1].v, yyvsp[-5].i);
		}
#ifdef USE_QBUF_MALLOC
		qbufReleasePtr(vector_pool, (char *)yyvsp[-4].v);
		qbufReleasePtr(vector_pool, (char *)yyvsp[-3].v);
		qbufReleasePtr(vector_pool, (char *)yyvsp[-2].v);
		qbufReleasePtr(vector_pool, (char *)yyvsp[-1].v);
#else
		free(yyvsp[-4].v);
		free(yyvsp[-3].v);
		free(yyvsp[-2].v);
		free(yyvsp[-1].v);
#endif
		if (stop_include_file() == (-1)) { YYACCEPT; };
		;
    break;}
case 109:
{
		transform_vec_inplace(yyvsp[-4].v);
		transform_vec_inplace(yyvsp[-3].v);
		transform_vec_inplace(yyvsp[-2].v);
		transform_vec_inplace(yyvsp[-1].v);
		if ((ldraw_commandline_opts.output == 1)&&(include_stack_ptr <= ldraw_commandline_opts.output_depth )) {
		  fprintf(output_file,"%d %d %g %g %g %g %g %g %g %g %g %g %g %g\n",
		  yyvsp[-6].i, yyvsp[-5].i, (yyvsp[-4].v)->x, (yyvsp[-4].v)->y, (yyvsp[-4].v)->z,
		  (yyvsp[-3].v)->x, (yyvsp[-3].v)->y, (yyvsp[-3].v)->z, (yyvsp[-2].v)->x, (yyvsp[-2].v)->y, (yyvsp[-2].v)->z,
		  (yyvsp[-1].v)->x, (yyvsp[-1].v)->y, (yyvsp[-1].v)->z);
	    } else {
		  render_five(yyvsp[-4].v, yyvsp[-3].v, yyvsp[-2].v, yyvsp[-1].v, yyvsp[-5].i);
		}
#ifdef USE_QBUF_MALLOC
		qbufReleasePtr(vector_pool, (char *)yyvsp[-4].v);
		qbufReleasePtr(vector_pool, (char *)yyvsp[-3].v);
		qbufReleasePtr(vector_pool, (char *)yyvsp[-2].v);
		qbufReleasePtr(vector_pool, (char *)yyvsp[-1].v);
#else
		free(yyvsp[-4].v);
		free(yyvsp[-3].v);
		free(yyvsp[-2].v);
		free(yyvsp[-1].v);
#endif
		;
    break;}
case 110:
{
		transform_vec_inplace(yyvsp[-4].v);
		transform_vec_inplace(yyvsp[-3].v);
		transform_vec_inplace(yyvsp[-2].v);
		transform_vec_inplace(yyvsp[-1].v);
		if ((ldraw_commandline_opts.output == 1)&&(include_stack_ptr <= ldraw_commandline_opts.output_depth )) {
		  fprintf(output_file,"%d %d %g %g %g %g %g %g %g %g %g %g %g %g\n",
		  yyvsp[-6].i, yyvsp[-5].i, (yyvsp[-4].v)->x, (yyvsp[-4].v)->y, (yyvsp[-4].v)->z,
		  (yyvsp[-3].v)->x, (yyvsp[-3].v)->y, (yyvsp[-3].v)->z, (yyvsp[-2].v)->x, (yyvsp[-2].v)->y, (yyvsp[-2].v)->z,
		  (yyvsp[-1].v)->x, (yyvsp[-1].v)->y, (yyvsp[-1].v)->z);
	    } else {
		  render_five(yyvsp[-4].v, yyvsp[-3].v, yyvsp[-2].v, yyvsp[-1].v, yyvsp[-5].i);
	    }
#ifdef USE_QBUF_MALLOC
		qbufReleasePtr(vector_pool, (char *)yyvsp[-4].v);
		qbufReleasePtr(vector_pool, (char *)yyvsp[-3].v);
		qbufReleasePtr(vector_pool, (char *)yyvsp[-2].v);
		qbufReleasePtr(vector_pool, (char *)yyvsp[-1].v);
#else
		free(yyvsp[-4].v);
		free(yyvsp[-3].v);
		free(yyvsp[-2].v);
		free(yyvsp[-1].v);
#endif
		if (stop_include_file() == (-1)) { YYACCEPT; };
		;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */


  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = yylloc.first_line;
      yylsp->first_column = yylloc.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */
	  for (x = (yyn < 0 ? -yyn : 0);
	       x < (sizeof(yytname) / sizeof(char *)); x++)
	    if (yycheck[x + yyn] == x)
	      size += strlen(yytname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (yyn < 0 ? -yyn : 0);
		       x < (sizeof(yytname) / sizeof(char *)); x++)
		    if (yycheck[x + yyn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, yytname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      yyerror(msg);
	      free(msg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror("parse error");
    }

  goto yyerrlab1;
yyerrlab1:   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop:   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

yyerrhandle:

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;

 yyacceptlab:
  /* YYACCEPT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 0;

 yyabortlab:
  /* YYABORT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 1;
}


void yyerror(s)
  char *s;
{
  char buf[256];
  sprintf(buf,"Syntax error on line %d", yylineno);
  zWrite(buf);
}

vector3d * savevec(float x, float y, float z)
{
  vector3d *tmp;
  
#ifdef USE_QBUF_MALLOC
  tmp = (vector3d *)qbufGetPtr(vector_pool);
#else
  tmp = (vector3d *)malloc(sizeof(vector3d));
#endif
  tmp->x = x;
  tmp->y = y;
  tmp->z = z;
  return tmp;
}

vector3d * copyvec(vector3d *vec)
{
  vector3d *tmp;
  
#ifdef USE_QBUF_MALLOC
  tmp = (vector3d *)qbufGetPtr(vector_pool);
#else
  tmp = (vector3d *)malloc(sizeof(vector3d));
#endif
  *tmp = *vec;
  return tmp;
}

void transform_vec_inplace(vector3d *v)
{
  vector3d tmp;
  matrix3d *m;
  vector3d *t;
  m = current_transform[transform_stack_ptr];
  t = current_translation[transform_stack_ptr];
  tmp.x = m->a * v->x + m->b * v->y + m->c * v->z + t->x;
  tmp.y = m->d * v->x + m->e * v->y + m->f * v->z + t->y;
  tmp.z = m->g * v->x + m->h * v->y + m->i * v->z + t->z;
  *v = tmp;
  return;
}

void transform_mat_inplace(matrix3d *m)
{
  matrix3d newm;
  matrix3d *oldm;

  oldm = current_transform[transform_stack_ptr];

  newm.a = oldm->a * m->a + oldm->b * m->d + oldm->c * m->g;
  newm.b = oldm->a * m->b + oldm->b * m->e + oldm->c * m->h;
  newm.c = oldm->a * m->c + oldm->b * m->f + oldm->c * m->i;
  newm.d = oldm->d * m->a + oldm->e * m->d + oldm->f * m->g;
  newm.e = oldm->d * m->b + oldm->e * m->e + oldm->f * m->h;
  newm.f = oldm->d * m->c + oldm->e * m->f + oldm->f * m->i;
  newm.g = oldm->g * m->a + oldm->h * m->d + oldm->i * m->g;
  newm.h = oldm->g * m->b + oldm->h * m->e + oldm->i * m->h;
  newm.i = oldm->g * m->c + oldm->h * m->f + oldm->i * m->i;
  *m = newm;
  return;
}

int print_transform(vector3d *t,matrix3d *m)
{
  fprintf(stdout,"Translation: %g %g %g\n",t->x, t->y, t->z);
  fprintf(stdout,"Matrix:      %g %g %g\n",m->a, m->b, m->c);
  fprintf(stdout,"             %g %g %g\n",m->d, m->e, m->f);
  fprintf(stdout,"             %g %g %g\n",m->g, m->h, m->i);
  return 0;
}

void pop_transform()
{
  if(transform_stack_ptr < 0) {
	zWrite("Error in transform stack\n");
	exit(-1);
  }
#ifdef USE_QBUF_MALLOC
  qbufReleasePtr(vector_pool, (char *)current_translation[transform_stack_ptr]);
  qbufReleasePtr(matrix_pool, (char *)current_transform[transform_stack_ptr]);
#else
  free(current_transform[transform_stack_ptr]);
  free(current_translation[transform_stack_ptr]);
#endif
  transform_stack_ptr--;
}

int push_rotation(vector3d *axis, double degrees)
{
  // convert axis - degrees into rotation matrix
  matrix3d *m;
  double a,b,c,s, veclen_inv, sin2a;

	// convert axis and degrees into a quaternion
    veclen_inv = 1.0/sqrt(axis->x*axis->x + axis->y*axis->y + axis->z*axis->z);
	sin2a = sin((3.1415927*degrees)/360.0);
	a = sin2a * axis->x * veclen_inv;
	b = sin2a * axis->y * veclen_inv;
	c = sin2a * axis->z * veclen_inv;
	s = cos((3.1415927*degrees)/360.0);
	// convert quaternion into a rotation matrix.
#ifdef USE_QBUF_MALLOC
      m = (matrix3d *)qbufGetPtr(matrix_pool);
#else
      m = (matrix3d *)malloc(sizeof(matrix3d));
#endif
	m->a = (float)(1 - 2*b*b-2*c*c);
	m->b = (float)(2*a*b - 2*s*c);
	m->c = (float)(2*a*c + 2*s*b);
	m->d = (float)(2*a*b+2*s*c);
	m->e = (float)(1 - 2*a*a - 2*c*c);
	m->f = (float)(2*b*c - 2*s*a);
	m->g = (float)(2*a*c - 2*s*b);
	m->h = (float)(2*b*c + 2*s*a);
	m->i = (float)(1 - 2*a*a - 2*b*b);

	push_transform(NULL,m);
#ifdef USE_QBUF_MALLOC
  qbufReleasePtr(vector_pool, (char *)axis);
#else
  free(axis);
#endif
	return 0;
}

int push_transform(vector3d *t,matrix3d *m)
{
  vector3d *oldt;
  matrix3d *oldm; 
  
  if(transform_stack_ptr >= (MAX_TRANSFORM_DEPTH-1)) {
	zWrite("Transform stack overflow\n");
	exit(-1);
  }
  if (transform_stack_ptr < 0) {
	if (m == NULL) {
#ifdef USE_QBUF_MALLOC
      m = (matrix3d *)qbufGetPtr(matrix_pool);
#else
      m = (matrix3d *)malloc(sizeof(matrix3d));
#endif
	  m->a = 1.0; m->b = 0.0; m->c = 0.0;
	  m->d = 0.0; m->e = 1.0; m->f = 0.0;
	  m->g = 0.0; m->h = 0.0; m->i = 1.0;
	}
	if (t == NULL) {
#ifdef USE_QBUF_MALLOC
     t = (vector3d *)qbufGetPtr(vector_pool);
#else
     t= (vector3d *)malloc(sizeof(vector3d));
#endif
	 t->x = 0.0; t->y = 0.0; t->z = 0.0;
	}
  } else {
  oldm = current_transform[transform_stack_ptr];
  oldt = current_translation[transform_stack_ptr];
#if 0
  fprintf(stdout,"Transform %d:\n",transform_stack_ptr;
  print_transform(oldt, oldm);
  fprintf(stdout,"Transform current:\n");
  print_transform(t, m);
#endif
	if (m == NULL) {
#ifdef USE_QBUF_MALLOC
      m = (matrix3d *)qbufGetPtr(matrix_pool);
#else
      m = (matrix3d *)malloc(sizeof(matrix3d));
#endif
      *m = *oldm;
	} else {
      matrix3d newm;
      newm.a = oldm->a * m->a + oldm->b * m->d + oldm->c * m->g;
      newm.b = oldm->a * m->b + oldm->b * m->e + oldm->c * m->h;
      newm.c = oldm->a * m->c + oldm->b * m->f + oldm->c * m->i;
      newm.d = oldm->d * m->a + oldm->e * m->d + oldm->f * m->g;
      newm.e = oldm->d * m->b + oldm->e * m->e + oldm->f * m->h;
      newm.f = oldm->d * m->c + oldm->e * m->f + oldm->f * m->i;
      newm.g = oldm->g * m->a + oldm->h * m->d + oldm->i * m->g;
      newm.h = oldm->g * m->b + oldm->h * m->e + oldm->i * m->h;
      newm.i = oldm->g * m->c + oldm->h * m->f + oldm->i * m->i;
      *m = newm;
  }
  if (t == NULL) {  
#ifdef USE_QBUF_MALLOC
     t = (vector3d *)qbufGetPtr(vector_pool);
#else
     t= (vector3d *)malloc(sizeof(vector3d));
#endif
     *t = *oldt;
  } else {
      vector3d newt;
      newt.x = oldm->a * t->x + oldm->b * t->y + oldm->c * t->z + oldt->x;
      newt.y = oldm->d * t->x + oldm->e * t->y + oldm->f * t->z + oldt->y;
      newt.z = oldm->g * t->x + oldm->h * t->y + oldm->i * t->z + oldt->z;
      *t = newt;
  }
  }
  transform_stack_ptr++;
  current_transform[transform_stack_ptr] = m;
  current_translation[transform_stack_ptr] = t;
#if 0
  fprintf(stdout,"Transform %d:\n",transform_stack_ptr);
  print_transform(current_translation[transform_stack_ptr],
	          current_transform[transform_stack_ptr]);
#endif
  return 0;
}

matrix3d * savemat(float a, float b, float c,
		   float d, float e, float f,
		   float g, float h, float i)
{
  matrix3d *tmp;
  
#ifdef USE_QBUF_MALLOC
  tmp = (matrix3d *)qbufGetPtr(matrix_pool);
#else
  tmp = (matrix3d *)malloc(sizeof(matrix3d));
#endif
  tmp->a = a;
  tmp->b = b;
  tmp->c = c;
  tmp->d = d;
  tmp->e = e;
  tmp->f = f;
  tmp->g = g;
  tmp->h = h;
  tmp->i = i;
  return tmp;
}

matrix3d * copymat(matrix3d *mat)
{
  matrix3d *tmp;
  
#ifdef USE_QBUF_MALLOC
  tmp = (matrix3d *)qbufGetPtr(matrix_pool);
#else
  tmp = (matrix3d *)malloc(sizeof(matrix3d));
#endif
  *tmp = *mat;
  return tmp;
}
