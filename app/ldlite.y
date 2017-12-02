/*
 * Grammer for LDRAW
*/

%{
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

%}

%union {
	char *c;
	int i;
	double d;
	vector3d *v;
	matrix3d *m;
}

%token <c> tIDENT
%token <c> tGARBAGE
%token <i> tINT
%token <d> tFLOAT
%token <i> tZERO
%token <i> tONE
%token <i> tTWO
%token <i> tTHREE
%token <i> tFOUR
%token <i> tFIVE
%token <c> tSTEP
%token <c> tPAUSE
%token <c> tWRITE
%token <c> tCLEAR
%token <c> tSAVE
%token <c> tEOL
%token <c> tEOF
%token <c> tROTATE
%token <c> tTRANSLATE
%token <c> tEND
%token <c> tSCALE
%token <c> tTRANSFORM
%token <c> tCOLOR
%token <c> tALIAS
%token <c> tPOINT
%token <c> tMATRIX
%token <c> tFILE

%type <c> PartName
%type <d> Number
%type <c> Words
%type <c> Word
%type <i> Color
%type <v> Position
%type <m> TransMatrix
%%

LdrawModel	: LdrawLines
		;

LdrawLines	: /* empty */
		| LdrawLines LdrawLine
		;

LdrawLine	: StepLine
		| PauseLine
		| WriteLine
		| ClearLine
		| SaveLine
		| ObjectLine
		| LineLine
		| TriLine
		| QuadLine
		| FiveLine
		| TranslateStartLine LdrawLines TranslateEndLine
		| RotateStartLine LdrawLines RotateEndLine
		| ScaleStartLine LdrawLines ScaleEndLine
		| TransformStartLine LdrawLines TransformEndLine
		| ColorLine
		| ColorAliasLine
		| PointAliasLine
		| MatrixAliasLine
		| MPDFileLine
		| BlankLine
		| CommentLine
		| error tEOL
		| error tEOF
		;

Number		: tZERO
		{
			$$ = 0.0;
		}
		| tONE
		{
			$$ = 1.0;
		}
		| tTWO
		{
			$$ = 2.0;
		}
		| tTHREE
		{
			$$ = 3.0;
		}
		| tFOUR
		{
			$$ = 4.0;
		}
		| tFIVE
		{
			$$ = 5.0;
		}
		| tINT
		{
			$$ = 1.0 * ($1);
#ifndef NO_COMMENTS
			printf("tINT: %f\n",$1);
#endif
		}
		| tFLOAT
		{
			$$ =1.0 * ($1);
#ifndef NO_COMMENTS
			printf("tFLOAT: %f\n",$1);
#endif
		}
		;

StepLine	: tZERO tSTEP Words tEOL
		{
		  if ((ldraw_commandline_opts.output == 1)&&(include_stack_ptr <= ldraw_commandline_opts.output_depth )) {
		    fprintf(output_file,"%d %s %s\n",$1, $2, $3);
		  } else {
			zStep(stepcount,1);
			stepcount++;
          }
#ifdef USE_QBUF_MALLOC
		    qbufReleasePtr(word_pool, (char *)$3);
#else
		    free($3);
#endif
		}
		| tZERO tSTEP Words tEOF
		{
		  if ((ldraw_commandline_opts.output == 1)&&(include_stack_ptr <= ldraw_commandline_opts.output_depth )) {
		    fprintf(output_file,"%d %s %s\n",$1, $2, $3);
		  } else {
			zStep(stepcount,1);
			stepcount++;
          }
#ifdef USE_QBUF_MALLOC
		    qbufReleasePtr(word_pool, (char *)$3);
#else
		    free($3);
#endif
		  if (stop_include_file() == (-1)) { YYACCEPT; };
		}
		;

PauseLine	: tZERO tPAUSE Words tEOL
		{
		  if ((ldraw_commandline_opts.output == 1)&&(include_stack_ptr <= ldraw_commandline_opts.output_depth )) {
		    fprintf(output_file,"%d %s %s\n",$1, $2, $3);
	      } else {
			zPause();
          }
#ifdef USE_QBUF_MALLOC
		    qbufReleasePtr(word_pool, (char *)$3);
#else
		    free($3);
#endif
		}
		| tZERO tPAUSE Words tEOF
		{
		  if ((ldraw_commandline_opts.output == 1)&&(include_stack_ptr <= ldraw_commandline_opts.output_depth )) {
		    fprintf(output_file,"%d %s %s\n",$1, $2, $3);
	      } else {
			zPause();
          }
#ifdef USE_QBUF_MALLOC
		    qbufReleasePtr(word_pool, (char *)$3);
#else
		    free($3);
#endif
			if (stop_include_file() == (-1)) { YYACCEPT; };
		}
		;

WriteLine	: tZERO tWRITE Words tEOL
		{
		  if ((ldraw_commandline_opts.output == 1)&&(include_stack_ptr <= ldraw_commandline_opts.output_depth )) {
		    fprintf(output_file,"%d %s %s\n",$1, $2, $3);
	      } else {
			zWrite($3);
          }
#ifdef USE_QBUF_MALLOC
		    qbufReleasePtr(word_pool, (char *)$3);
#else
		    free($3);
#endif
		}
		| tZERO tWRITE Words tEOF
		{
		  if ((ldraw_commandline_opts.output == 1)&&(include_stack_ptr <= ldraw_commandline_opts.output_depth )) {
		    fprintf(output_file,"%d %s %s\n",$1, $2, $3);
	      } else {
			zWrite($3);
          }
#ifdef USE_QBUF_MALLOC
		    qbufReleasePtr(word_pool, (char *)$3);
#else
		    free($3);
#endif
			if (stop_include_file() == (-1)) { YYACCEPT; };
		}
		;

ClearLine	: tZERO tCLEAR Words tEOL
		{
		  if ((ldraw_commandline_opts.output == 1)&&(include_stack_ptr <= ldraw_commandline_opts.output_depth )) {
		    fprintf(output_file,"%d %s %s\n",$1, $2, $3);
	      } else {
			zClear();
		  }
#ifdef USE_QBUF_MALLOC
		    qbufReleasePtr(word_pool, (char *)$3);
#else
		    free($3);
#endif
		}
		| tZERO tCLEAR Words tEOF
		{
		  if ((ldraw_commandline_opts.output == 1)&&(include_stack_ptr <= ldraw_commandline_opts.output_depth )) {
		    fprintf(output_file,"%d %s %s\n",$1, $2, $3);
	      } else {
			zClear();
		  }
#ifdef USE_QBUF_MALLOC
		    qbufReleasePtr(word_pool, (char *)$3);
#else
		    free($3);
#endif
			if (stop_include_file() == (-1)) { YYACCEPT; };
		}
		;

ColorLine	: tZERO tCOLOR Number tIDENT Number Number Number Number Number Number Number Number Number tEOL
		{
		  if ((ldraw_commandline_opts.output == 1)&&(include_stack_ptr <= ldraw_commandline_opts.output_depth )) {
		    fprintf(output_file,"%d not supported: %s %g %s %g %g %g %g %g %g %g %g %g\n",
			  $1, $2, $3, $4, $5, $6, $7, $8, $9, $10, $11, $12, $13);
			/* alias the new color name to grey */
			zcolor_alias(7,$4);
	      } else {
			zcolor_modify((int)$3,$4,(int)$5,(int)$6,(int)$7,(int)$8,(int)$9,(int)$10,(int)$11,(int)$12,(int)$13);
          }
		}
		| tZERO tCOLOR Number tIDENT Number Number Number Number Number Number Number Number Number tEOF
		{
		  if ((ldraw_commandline_opts.output == 1)&&(include_stack_ptr <= ldraw_commandline_opts.output_depth )) {
		    fprintf(output_file,"%d not supported: %s %g %s %g %g %g %g %g %g %g %g %g\n",
			  $1, $2, $3, $4, $5, $6, $7, $8, $9, $10, $11, $12, $13);
			/* alias the new color name to grey */
			zcolor_alias(7,$4);
	      } else {
			zcolor_modify((int)$3,$4,(int)$5,(int)$6,(int)$7,(int)$8,(int)$9,(int)$10,(int)$11,(int)$12,(int)$13);
          }
			if (stop_include_file() == (-1)) { YYACCEPT; };
		}
		;

ColorAliasLine	: tZERO tALIAS tIDENT Color tEOL
		{
			zcolor_alias((int)$4,$3);
		    if (is_current_file_not_cached()) {
#ifdef USE_QBUF_MALLOC
		      qbufReleasePtr(word_pool, (char *)$3);
#else
		      free($3);
#endif
		    }
		}
		| tZERO tALIAS tIDENT Color tEOF
		{
			zcolor_alias((int)$4,$3);
		    if (is_current_file_not_cached()) {
#ifdef USE_QBUF_MALLOC
		      qbufReleasePtr(word_pool, (char *)$3);
#else
		      free($3);
#endif
		    }
			if (stop_include_file() == (-1)) { YYACCEPT; };
		}
		;

PointAliasLine	: tZERO tPOINT tIDENT Position tEOL
		{
			zpoint_alias($3,$4);
		    if (is_current_file_not_cached()) {
#ifdef USE_QBUF_MALLOC
		      qbufReleasePtr(word_pool, (char *)$3);
#else
		      free($3);
#endif
		    }
		}
		| tZERO tPOINT tIDENT Position tEOF
		{
			zpoint_alias($3,$4);
		    if (is_current_file_not_cached()) {
#ifdef USE_QBUF_MALLOC
		      qbufReleasePtr(word_pool, (char *)$3);
#else
		      free($3);
#endif
            }
			if (stop_include_file() == (-1)) { YYACCEPT; };
		}
		;

MatrixAliasLine	: tZERO tMATRIX tIDENT TransMatrix tEOL
		{
			zmatrix_alias($3,$4);
		    if (is_current_file_not_cached()) {
#ifdef USE_QBUF_MALLOC
		      qbufReleasePtr(word_pool, (char *)$3);
#else
		      free($3);
#endif
		    }
		}
		| tZERO tMATRIX tIDENT TransMatrix tEOF
		{
			zmatrix_alias($3,$4);
		    if (is_current_file_not_cached()) {
#ifdef USE_QBUF_MALLOC
		      qbufReleasePtr(word_pool, (char *)$3);
#else
		      free($3);
#endif
		    }
			if (stop_include_file() == (-1)) { YYACCEPT; };
		}
		;

MPDFileLine	: tZERO tFILE tIDENT tEOL
		{
            if (cache_mpd_subfiles($3) == 0) {
			  mpd_subfile_name = $3;
			} else {
#ifdef USE_QBUF_MALLOC
		    qbufReleasePtr(word_pool, (char *)$3);
#else
		    free($3);
#endif
            }
			YYACCEPT;
		}
		| tZERO tFILE tIDENT tEOF
		{
            if (cache_mpd_subfiles($3) == 0) {
			  mpd_subfile_name = $3;
			} else {
#ifdef USE_QBUF_MALLOC
		      qbufReleasePtr(word_pool, (char *)$3);
#else
		      free($3);
#endif
            }
			YYACCEPT;
		}
		;

SaveLine	: tZERO tSAVE Words tEOL
		{
		  if ((ldraw_commandline_opts.output == 1)&&(include_stack_ptr <= ldraw_commandline_opts.output_depth )) {
		    fprintf(output_file,"%d %s %s\n",$1, $2, $3);
	      } else {
			zSave(stepcount);
		  }
			stepcount++;
#ifdef USE_QBUF_MALLOC
		    qbufReleasePtr(word_pool, (char *)$3);
#else
		    free($3);
#endif
		}
		| tZERO tSAVE Words tEOF
		{
		  if ((ldraw_commandline_opts.output == 1)&&(include_stack_ptr <= ldraw_commandline_opts.output_depth )) {
		    fprintf(output_file,"%d %s %s\n",$1, $2, $3);
	      } else {
			zSave(stepcount);
		  }
			stepcount++;
#ifdef USE_QBUF_MALLOC
		    qbufReleasePtr(word_pool, (char *)$3);
#else
		    free($3);
#endif
			if (stop_include_file() == (-1)) { YYACCEPT; };
		}
		;

TranslateStartLine	: tZERO tTRANSLATE Position tEOL
		{
		push_transform($3, NULL);
		}
		| tZERO tTRANSLATE Position tEOF
		{
		push_transform($3, NULL);
		if (stop_include_file() == (-1)) { YYACCEPT; };
		}
		;

TranslateEndLine	: tZERO tTRANSLATE tEND tEOL
		{
		pop_transform();
		}
		| tZERO tTRANSLATE tEND tEOF
		{
		pop_transform();
		if (stop_include_file() == (-1)) { YYACCEPT; };
		}
		;

RotateStartLine	: tZERO tROTATE Number Position tEOL
		{
		  /* axis degree representation */
		  push_rotation($4, $3);
		}
		| tZERO tROTATE Number Position tEOF
		{
		  /* axis degree representation */
		  push_rotation($4, $3);
		  if (stop_include_file() == (-1)) { YYACCEPT; };
		}
		;

RotateEndLine	: tZERO tROTATE tEND tEOL
		{
			pop_transform();
		}
		| tZERO tROTATE tEND tEOF
		{
		    pop_transform();
		if (stop_include_file() == (-1)) { YYACCEPT; };
		}
		;

ScaleStartLine	: tZERO tSCALE Number tEOL
		{
		push_transform(NULL, savemat((float)($3),0,0,0,(float)($3),0,0,0,(float)($3)));
		}
		| tZERO tSCALE Number tEOF
		{
		push_transform(NULL, savemat((float)($3),0,0,0,(float)($3),0,0,0,(float)($3)));
		if (stop_include_file() == (-1)) { YYACCEPT; };
		}
		;

ScaleEndLine	: tZERO tSCALE tEND tEOL
		{
		pop_transform();
		}
		| tZERO tSCALE tEND tEOF
		{
		pop_transform();
		if (stop_include_file() == (-1)) { YYACCEPT; };
		}
		;

TransformStartLine	: tZERO tTRANSFORM Position TransMatrix tEOL
		{
		  push_transform($3, $4);
		}
		| tZERO tTRANSFORM Position TransMatrix tEOF
		{
		  push_transform($3, $4);
		  if (stop_include_file() == (-1)) { YYACCEPT; };
		}
		;

TransformEndLine	: tZERO tTRANSFORM tEND tEOL
		{
		pop_transform();
		}
		| tZERO tTRANSFORM tEND tEOF
		{
		pop_transform();
		if (stop_include_file() == (-1)) { YYACCEPT; };
		}
		;


Word		: tIDENT
		{
		  if (is_current_file_not_cached()) {
			$$ = $1;
		  } else {
#ifdef USE_QBUF_MALLOC
		    $$ = (char *)qbufGetPtr(word_pool);
#else
	        $$ = malloc(128);
#endif
	        strcpy($$,$1);
		  }
#if 0
		  fprintf(stderr,"Got word \"%s\"\n",$1);
		  fflush(stderr);
#endif
		 }
		| tGARBAGE
		{ 
		  if (is_current_file_not_cached()) {
			$$ = $1;
		  } else {
#ifdef USE_QBUF_MALLOC
		    $$ = (char *)qbufGetPtr(word_pool);
#else
	        $$ = malloc(128);
#endif
	        strcpy($$,$1);
		  }
#if 0
          fprintf(stderr,"Got garbage characters \"%s\"\n",$1);
	      fflush(stderr);
#endif
		}
		| Number
		{ 
#ifdef USE_QBUF_MALLOC
	      $$ = (char *)qbufGetPtr(word_pool);
#else
	      $$ = malloc(128);
#endif
		  sprintf($$,"%g",$1); 
#if 0
		  fprintf(stderr,"Got word \"%s\"\n",$$);
		  fflush(stderr);
#endif
		}
		| tSTEP
		{
#ifdef USE_QBUF_MALLOC
	      $$ = (char *)qbufGetPtr(word_pool);
#else
	      $$ = malloc(128);
#endif
		  strcpy($$,$1);
		}
		| tPAUSE
		{
#ifdef USE_QBUF_MALLOC
	      $$ = (char *)qbufGetPtr(word_pool);
#else
	      $$ = malloc(128);
#endif
		  strcpy($$,$1);
		}
		| tWRITE
		{
#ifdef USE_QBUF_MALLOC
	      $$ = (char *)qbufGetPtr(word_pool);
#else
	      $$ = malloc(128);
#endif
		  strcpy($$,$1);
		}
		| tCLEAR
		{
#ifdef USE_QBUF_MALLOC
	      $$ = (char *)qbufGetPtr(word_pool);
#else
	      $$ = malloc(128);
#endif
		  strcpy($$,$1);
		}
		| tSAVE
		{
#ifdef USE_QBUF_MALLOC
	      $$ = (char *)qbufGetPtr(word_pool);
#else
	      $$ = malloc(128);
#endif
		  strcpy($$,$1);
		}
		| tTRANSLATE
		{
#ifdef USE_QBUF_MALLOC
	      $$ = (char *)qbufGetPtr(word_pool);
#else
	      $$ = malloc(128);
#endif
		  strcpy($$,$1);
		}
		| tROTATE
		{
#ifdef USE_QBUF_MALLOC
	      $$ = (char *)qbufGetPtr(word_pool);
#else
	      $$ = malloc(128);
#endif
		  strcpy($$,$1);
		}
		| tEND
		{
#ifdef USE_QBUF_MALLOC
	      $$ = (char *)qbufGetPtr(word_pool);
#else
	      $$ = malloc(128);
#endif
		  strcpy($$,$1);
		}
		| tCOLOR
		{
#ifdef USE_QBUF_MALLOC
	      $$ = (char *)qbufGetPtr(word_pool);
#else
	      $$ = malloc(128);
#endif
		  strcpy($$,$1);
		}
		| tPOINT
		{
#ifdef USE_QBUF_MALLOC
	      $$ = (char *)qbufGetPtr(word_pool);
#else
	      $$ = malloc(128);
#endif
		  strcpy($$,$1);
		}
		| tSCALE
		{
#ifdef USE_QBUF_MALLOC
	      $$ = (char *)qbufGetPtr(word_pool);
#else
	      $$ = malloc(128);
#endif
		  strcpy($$,$1);
		}
		| tTRANSFORM
		{
#ifdef USE_QBUF_MALLOC
	      $$ = (char *)qbufGetPtr(word_pool);
#else
	      $$ = malloc(128);
#endif
		  strcpy($$,$1);
		}
		| tALIAS
		{
#ifdef USE_QBUF_MALLOC
	      $$ = (char *)qbufGetPtr(word_pool);
#else
	      $$ = malloc(128);
#endif
		  strcpy($$,$1);
		}
		| tMATRIX
		{
#ifdef USE_QBUF_MALLOC
	      $$ = (char *)qbufGetPtr(word_pool);
#else
	      $$ = malloc(128);
#endif
		  strcpy($$,$1);
		}
		;

Words		: 
		{
#ifdef USE_QBUF_MALLOC
	      $$ = (char *)qbufGetPtr(word_pool);
#else
	      $$ = malloc(128);
#endif
		  strcpy($$,"");
		}
		| Words Word
		{ 
		// any Word string should be long enough hold all concatinated Words,
		// so we do not need to malloc a new string, just strcat them.
		$$ = $1;
		strcat($1, " ");
		strcat($1, $2);
#ifdef USE_QBUF_MALLOC
		  qbufReleasePtr(word_pool, (char *)$2);
#else
		  free($2);
#endif
		}
		;

CommentLine	: tZERO Words tEOL
		{
#ifndef NO_COMMENTS
		printf("# %s\n",$2);
		fflush(stdout);
#endif
		if ((ldraw_commandline_opts.output == 1)&&(include_stack_ptr <= ldraw_commandline_opts.output_depth )) {
		  fprintf(output_file,"%d %s\n",$1, $2);
	    } else {
		  platform_comment($2, include_stack_ptr);
        }
#ifdef USE_QBUF_MALLOC
		qbufReleasePtr(word_pool, (char *)$2);
#else
		free($2);
#endif
		}
		| tZERO Words tEOF
		{
#ifndef NO_COMMENTS
		printf("# %s\n",$2);
		fflush(stdout);
#endif
		if ((ldraw_commandline_opts.output == 1)&&(include_stack_ptr <= ldraw_commandline_opts.output_depth )) {
		  fprintf(output_file,"%d %s\n",$1, $2);
	    } else {
 		  platform_comment($2, include_stack_ptr);
		}
#ifdef USE_QBUF_MALLOC
		qbufReleasePtr(word_pool, (char *)$2);
#else
		free($2);
#endif
		if (stop_include_file() == (-1)) { YYACCEPT; };
		}
		;

BlankLine	: tEOL
		{
		}
		| tEOF
		{
			if (stop_include_file() == (-1)) { YYACCEPT; };
		}
		;

Color		: Number
		{
			if ($1 == 16.0) {
			  $$ = current_color[include_stack_ptr];
			} else if ($1 == 24.0) {
			  $$ = edge_color(current_color[include_stack_ptr]);
			} else {
			  $$ = (int) $1;
			}
		}
		| tIDENT
		{
			$$ = zcolor_lookup($1);
		    if (is_current_file_not_cached()) {
#ifdef USE_QBUF_MALLOC
		      qbufReleasePtr(word_pool, (char *)$1);
#else
		      free($1);
#endif
		    }
		}
		;

TransMatrix	: Number Number Number Number Number Number Number Number Number
		{
#if 1
			$$ = savemat($1,$2,$3,$4,$5,$6,$7,$8,$9);
#else
			$$ = savemat($9,$8,$7,$6,$5,$4,$3,$2,$1);
#endif
		}
		| tIDENT
		{
		    $$ = zmatrix_lookup($1);
		    if (is_current_file_not_cached()) {
#ifdef USE_QBUF_MALLOC
		      qbufReleasePtr(word_pool, (char *)$1);
#else
		      free($1);
#endif
		    }
		}
		;

Position	: Number Number Number
		{
			double a, b, c;
			vector3d *v;
			a = $1;
			b = $2;
			c = $3;
			v = savevec(a,b,c);
			$$ = v;
#ifndef NO_COMMENTS
			printf("Position (%f, %f, %f)\n",v->x, v->y, v->z);
#endif
		}
		| tIDENT
		{
		    $$ = zpoint_lookup($1);
		    if (is_current_file_not_cached()) {
#ifdef USE_QBUF_MALLOC
		      qbufReleasePtr(word_pool, (char *)$1);
#else
		      free($1);
#endif
		    }
		}
		;

PartName	: tIDENT
		{
		  if (is_current_file_not_cached()) {
			$$ = $1;
		  } else {
#ifdef USE_QBUF_MALLOC
		    $$ = (char *)qbufGetPtr(word_pool);
#else
	        $$ = malloc(128);
#endif
	        strcpy($$,$1);
		  }
		}
		;

ObjectLine	: tONE Color Position TransMatrix PartName tEOL
		{
		if (ldraw_commandline_opts.output == 1) {
			if (include_stack_ptr >= ldraw_commandline_opts.output_depth ) {
  				transform_vec_inplace($3);
  				transform_mat_inplace($4);
				fprintf(output_file,"%d %d %g %g %g %g %g %g %g %g %g %g %g %g %s\n",
					$1, $2, ($3)->x, ($3)->y, ($3)->z,
					($4)->a, ($4)->b, ($4)->c, ($4)->d, ($4)->e, ($4)->f, 
					($4)->g, ($4)->h, ($4)->i, $5);
			} else {
				if (start_include_file($5) == 0) {
					fprintf(output_file,"0 inlining %s\n",$5);
	  				/* update transform matricies */
					push_transform($3, $4);
					znamelist_push();
					current_color[include_stack_ptr] = $2;
				} else {
#ifndef NO_COMMENTS
				printf("# Cannot find %s, ignoring.\n",$5);
#endif
				}
			}
	    } else {
	      if (start_include_file($5) == 0) {
	  	  /* update transform matricies */
		  push_transform($3, $4);
		  znamelist_push();
		  current_color[include_stack_ptr] = $2;
		  } else {
#ifndef NO_COMMENTS
		  printf("# Cannot find %s, ignoring.\n",$5);
#endif
		  }
		}
#ifdef USE_QBUF_MALLOC
		qbufReleasePtr(word_pool, (char *)$5);
#else
		    free($5);
#endif
		}
		| tONE Color Position TransMatrix PartName tEOF
		{
		if (ldraw_commandline_opts.output == 1) {
			if (include_stack_ptr >= ldraw_commandline_opts.output_depth ) {
  				transform_vec_inplace($3);
  				transform_mat_inplace($4);
				fprintf(output_file,"%d %d %g %g %g %g %g %g %g %g %g %g %g %g %s\n",
					$1, $2, ($3)->x, ($3)->y, ($3)->z,
					($4)->a, ($4)->b, ($4)->c, ($4)->d, ($4)->e, ($4)->f, 
					($4)->g, ($4)->h, ($4)->i, $5);
			} else {
				if (start_include_file($5) == 0) {
					fprintf(output_file,"0 inlining %s\n",$5);
	  				/* update transform matricies */
					push_transform($3, $4);
					znamelist_push();
					current_color[include_stack_ptr] = $2;
					defer_stop_include_file();
				} else {
#ifndef NO_COMMENTS
					printf("# Cannot find %s, ignoring.\n",$5);
#endif
					if (stop_include_file() == (-1)) { YYACCEPT; };
				}
			}
		}
#ifdef USE_QBUF_MALLOC
		qbufReleasePtr(word_pool, (char *)$5);
#else
		free($5);
#endif
		}
		;


LineLine	: tTWO Color Position Position tEOL
		{
		transform_vec_inplace($3);
		transform_vec_inplace($4);
		if ((ldraw_commandline_opts.output == 1)&&(include_stack_ptr <= ldraw_commandline_opts.output_depth )) {
		  fprintf(output_file,"%d %d %g %g %g %g %g %g\n",
		  $1, $2, ($3)->x, ($3)->y, ($3)->z,
		  ($4)->x, ($4)->y, ($4)->z);
	    } else {
		  render_line($3, $4, $2);
		}
#ifdef USE_QBUF_MALLOC
		qbufReleasePtr(vector_pool, (char *)$3);
		qbufReleasePtr(vector_pool, (char *)$4);
#else
		free($3);
		free($4);
#endif
		}
		| tTWO Color Position Position tEOF
		{
		transform_vec_inplace($3);
		transform_vec_inplace($4);
		if ((ldraw_commandline_opts.output == 1)&&(include_stack_ptr <= ldraw_commandline_opts.output_depth )) {
		  fprintf(output_file,"%d %d %g %g %g %g %g %g\n",
		  $1, $2, ($3)->x, ($3)->y, ($3)->z,
		  ($4)->x, ($4)->y, ($4)->z);
	    } else {
		  render_line($3, $4, $2);
		}
#ifdef USE_QBUF_MALLOC
		qbufReleasePtr(vector_pool, (char *)$3);
		qbufReleasePtr(vector_pool, (char *)$4);
#else
		free($3);
		free($4);
#endif
		if (stop_include_file() == (-1)) { YYACCEPT; };
		}

		;

TriLine	: tTHREE Color Position Position Position tEOL
		{
		transform_vec_inplace($3);
		transform_vec_inplace($4);
		transform_vec_inplace($5);
		if ((ldraw_commandline_opts.output == 1)&&(include_stack_ptr <= ldraw_commandline_opts.output_depth )) {
		  fprintf(output_file,"%d %d %g %g %g %g %g %g %g %g %g\n",
		  $1, $2, ($3)->x, ($3)->y, ($3)->z,
		  ($4)->x, ($4)->y, ($4)->z, ($5)->x, ($5)->y, ($5)->z);
	    } else {
		  render_triangle($3, $4, $5, $2);
		}
#ifdef USE_QBUF_MALLOC
		qbufReleasePtr(vector_pool, (char *)$3);
		qbufReleasePtr(vector_pool, (char *)$4);
		qbufReleasePtr(vector_pool, (char *)$5);
#else
		free($3);
		free($4);
		free($5);
#endif
		}
		| tTHREE Color Position Position Position tEOF
		{
		transform_vec_inplace($3);
		transform_vec_inplace($4);
		transform_vec_inplace($5);
		if ((ldraw_commandline_opts.output == 1)&&(include_stack_ptr <= ldraw_commandline_opts.output_depth )) {
		  fprintf(output_file,"%d %d %g %g %g %g %g %g %g %g %g\n",
		  $1, $2, ($3)->x, ($3)->y, ($3)->z,
		  ($4)->x, ($4)->y, ($4)->z, ($5)->x, ($5)->y, ($5)->z);
	    } else {
		  render_triangle($3, $4, $5, $2);
		}
#ifdef USE_QBUF_MALLOC
		qbufReleasePtr(vector_pool, (char *)$3);
		qbufReleasePtr(vector_pool, (char *)$4);
		qbufReleasePtr(vector_pool, (char *)$5);
#else
		free($3);
		free($4);
		free($5);
#endif
		if (stop_include_file() == (-1)) { YYACCEPT; };
		}
		;

QuadLine	: tFOUR Color Position Position Position Position tEOL
		{
		transform_vec_inplace($3);
		transform_vec_inplace($4);
		transform_vec_inplace($5);
		transform_vec_inplace($6);
		if ((ldraw_commandline_opts.output == 1)&&(include_stack_ptr <= ldraw_commandline_opts.output_depth )) {
		  fprintf(output_file,"%d %d %g %g %g %g %g %g %g %g %g %g %g %g\n",
		  $1, $2, ($3)->x, ($3)->y, ($3)->z,
		  ($4)->x, ($4)->y, ($4)->z, ($5)->x, ($5)->y, ($5)->z,
		  ($6)->x, ($6)->y, ($6)->z);
	    } else {
  		  render_quad($3, $4, $5, $6, $2);
		}
#ifdef USE_QBUF_MALLOC
		qbufReleasePtr(vector_pool, (char *)$3);
		qbufReleasePtr(vector_pool, (char *)$4);
		qbufReleasePtr(vector_pool, (char *)$5);
		qbufReleasePtr(vector_pool, (char *)$6);
#else
		free($3);
		free($4);
		free($5);
		free($6);
#endif
		}
		| tFOUR Color Position Position Position Position tEOF
		{
		transform_vec_inplace($3);
		transform_vec_inplace($4);
		transform_vec_inplace($5);
		transform_vec_inplace($6);
		if ((ldraw_commandline_opts.output == 1)&&(include_stack_ptr <= ldraw_commandline_opts.output_depth )) {
		  fprintf(output_file,"%d %d %g %g %g %g %g %g %g %g %g %g %g %g\n",
		  $1, $2, ($3)->x, ($3)->y, ($3)->z,
		  ($4)->x, ($4)->y, ($4)->z, ($5)->x, ($5)->y, ($5)->z,
		  ($6)->x, ($6)->y, ($6)->z);
	    } else {
		  render_quad($3, $4, $5, $6, $2);
		}
#ifdef USE_QBUF_MALLOC
		qbufReleasePtr(vector_pool, (char *)$3);
		qbufReleasePtr(vector_pool, (char *)$4);
		qbufReleasePtr(vector_pool, (char *)$5);
		qbufReleasePtr(vector_pool, (char *)$6);
#else
		free($3);
		free($4);
		free($5);
		free($6);
#endif
		if (stop_include_file() == (-1)) { YYACCEPT; };
		}
		;

FiveLine	: tFIVE Color Position Position Position Position tEOL
		{
		transform_vec_inplace($3);
		transform_vec_inplace($4);
		transform_vec_inplace($5);
		transform_vec_inplace($6);
		if ((ldraw_commandline_opts.output == 1)&&(include_stack_ptr <= ldraw_commandline_opts.output_depth )) {
		  fprintf(output_file,"%d %d %g %g %g %g %g %g %g %g %g %g %g %g\n",
		  $1, $2, ($3)->x, ($3)->y, ($3)->z,
		  ($4)->x, ($4)->y, ($4)->z, ($5)->x, ($5)->y, ($5)->z,
		  ($6)->x, ($6)->y, ($6)->z);
	    } else {
		  render_five($3, $4, $5, $6, $2);
		}
#ifdef USE_QBUF_MALLOC
		qbufReleasePtr(vector_pool, (char *)$3);
		qbufReleasePtr(vector_pool, (char *)$4);
		qbufReleasePtr(vector_pool, (char *)$5);
		qbufReleasePtr(vector_pool, (char *)$6);
#else
		free($3);
		free($4);
		free($5);
		free($6);
#endif
		}
		| tFIVE Color Position Position Position Position tEOF
		{
		transform_vec_inplace($3);
		transform_vec_inplace($4);
		transform_vec_inplace($5);
		transform_vec_inplace($6);
		if ((ldraw_commandline_opts.output == 1)&&(include_stack_ptr <= ldraw_commandline_opts.output_depth )) {
		  fprintf(output_file,"%d %d %g %g %g %g %g %g %g %g %g %g %g %g\n",
		  $1, $2, ($3)->x, ($3)->y, ($3)->z,
		  ($4)->x, ($4)->y, ($4)->z, ($5)->x, ($5)->y, ($5)->z,
		  ($6)->x, ($6)->y, ($6)->z);
	    } else {
		  render_five($3, $4, $5, $6, $2);
	    }
#ifdef USE_QBUF_MALLOC
		qbufReleasePtr(vector_pool, (char *)$3);
		qbufReleasePtr(vector_pool, (char *)$4);
		qbufReleasePtr(vector_pool, (char *)$5);
		qbufReleasePtr(vector_pool, (char *)$6);
#else
		free($3);
		free($4);
		free($5);
		free($6);
#endif
		if (stop_include_file() == (-1)) { YYACCEPT; };
		}
		;

%%

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
