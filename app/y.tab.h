typedef union {
    char *c;
    int i;
    double d;
    vector3d *v;
    matrix3d *m;
} YYSTYPE;

#define tIDENT      257
#define tGARBAGE    258
#define tINT        259
#define tFLOAT      260
#define tZERO       261
#define tONE        262
#define tTWO        263
#define tTHREE      264
#define tFOUR       265
#define tFIVE       266
#define tSTEP       267
#define tPAUSE      268
#define tWRITE      269
#define tCLEAR      270
#define tSAVE       271
#define tEOL        272
#define tEOF        273
#define tROTATE     274
#define tTRANSLATE  275
#define tEND        276
#define tSCALE      277
#define tTRANSFORM  278
#define tCOLOR      279
#define tALIAS      280
#define tPOINT      281
#define tMATRIX     282
#define tFILE       283

extern YYSTYPE yylval;
