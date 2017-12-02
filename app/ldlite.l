/*
 * Lex syntax for LDRAW command files.
 *
 * You must manually add #include <stdlib.h> to beginning of 
 * lex.yy.c (right after the #include <stdio.h>), which is
 * produced from this file via flex.  Otherwise, routines using
 * atof() will get garbage numbers.
*/
%{
#include "ldliteVR.h"
#include "y.tab.h"
#include "malloc.h"

#define YY_DECL int my_yylex YY_PROTO(( void ))

YY_BUFFER_STATE include_stack[MAX_INCLUDE_DEPTH];
FILE *fp_stack[MAX_INCLUDE_DEPTH];
int include_stack_ptr = 0;
int transform_stack_ptr = 0;
int defered_flag[MAX_INCLUDE_DEPTH];
typedef struct {
  int cached_files;
  int uncached_files;
  int cache_hits;
} LDLITE_PROFILE;

LDLITE_PROFILE ldlite_profile;

void pop_transform();
void znamelist_pop();

// caching lex defines
#define MAX_CACHED_TOKENS (3*1024)
#define MAX_MPD_CACHED_TOKENS (8*1024)
#define MAX_CACHED_FILES  (12*128)

// bitfield
#define CHS_UNUSED  0
#define CHS_FILLING 1
#define CHS_FILLED  2
#define CHS_PROBLEM 4

// This holds the token stream of a file which has been or
// is being cached.
typedef struct {
	int valid; // one of the CHS_X defines
	char filename[256];
	int ftype; // one of the TYPE_X defines
	int next_token_index;  // points to the next token to be read/written
	int     *tokens;
	YYSTYPE *values;
} CACHED_STREAM;

// This is the heap of cached files
CACHED_STREAM cached_streams[MAX_CACHED_FILES];

CACHED_STREAM *cached_file_stack[MAX_INCLUDE_DEPTH];
int cached_file_stack_index=0;  // points to unused entry

char pathname[256];

%}

%option noyywrap
%option yylineno

alpha	[a-zA-Z]
special	[\.\_,\-\~/\\#:]
hexprefix 0x
digit	[0-9]
hexdigit [0-9a-fA-F]
sign	[\+\-]
exp	[Ee]{sign}?{digit}+
ident	({alpha}|{digit}|{special})+
%%
[ \t\r]		;
[\n]		{yylval.c = "EOL"; return tEOL;}
5			{yylval.i = atoi(yytext); return tFIVE;}
4			{yylval.i = atoi(yytext); return tFOUR;}
3			{yylval.i = atoi(yytext); return tTHREE;}
2			{yylval.i = atoi(yytext); return tTWO;}
1			{yylval.i = atoi(yytext); return tONE;}
0			{yylval.i = atoi(yytext); return tZERO;}
STEP		{yylval.c = "STEP"; return tSTEP;}
PAUSE		{yylval.c = "PAUSE"; return tPAUSE;}
WRITE		{yylval.c = "WRITE"; return tWRITE;}
PRINT		{yylval.c = "PRINT"; return tWRITE;}
CLEAR		{yylval.c = "CLEAR"; return tCLEAR;}
SAVE		{yylval.c = "SAVE"; return tSAVE;}
ROTATE      {yylval.c = "ROTATE"; return tROTATE;}
TRANSLATE   {yylval.c = "TRANSLATE"; return tTRANSLATE;}
SCALE       {yylval.c = "SCALE"; return tSCALE;}
TRANSFORM   {yylval.c = "TRANSFORM"; return tTRANSFORM;}
COLO[U]?R   {yylval.c = "COLOR"; return tCOLOR;}
COLO[U]?RNAME   {yylval.c = "COLORNAME"; return tALIAS;}
POINT       {yylval.c = "POINT"; return tPOINT;}  
MATRIX      {yylval.c = "MATRIX"; return tMATRIX;}
FILE		{yylval.c = "FILE"; return tFILE;}
END         {yylval.c = "END"; return tEND;}

{hexprefix}{hexdigit}+		{
			int a;
			a = 0;
			sscanf(yytext,"%i",&a);
			yylval.i = a; 
			return tINT;
			}

{sign}?{digit}+		{
			int a;
			a = 0;
			a = atoi(yytext);
			yylval.i = a; 
			return tINT;
			}

{sign}?{digit}+"."{digit}*({exp})? |
{sign}?{digit}*"."{digit}+({exp})? |
{sign}?{digit}+{exp}		{
			yylval.d = atof(yytext);
			return tFLOAT;
			}
{ident}		{yylval.c = strsave(yytext); return tIDENT;}

[^ \t\r\n]+	{yylval.c = strsave(yytext); return tGARBAGE;}

<<EOF>> {
	    yylval.c = "EOF";
	    return tEOF;
        }
%%
int start_include_file(char *root_name)
{
  static char filename[256];
  FILE *fp = NULL;
  int ftype=0;
  CACHED_STREAM *found_it;
  
  if ( include_stack_ptr >= (MAX_INCLUDE_DEPTH-1) ) {
	  fprintf( stderr, "Includes nested too deeply" );
	  exit( 1 );
  }
  // look up to see if file is in cache.
  {
	  int i;

	  found_it = NULL;
	  for(i=0; i<cached_file_stack_index; i++) {
		  switch (cached_streams[i].valid) {
		  case CHS_UNUSED:
			  break;
		  case CHS_PROBLEM:
		  case CHS_FILLING:
		  case CHS_FILLED:
			  if(!stricmp(root_name,cached_streams[i].filename)) {
				  found_it = &(cached_streams[i]);
			  }
			  break;
		  }
	  }		  
  }

  if ((found_it) && (found_it->valid == CHS_FILLED)) {
	  // we can replay the cached file
	  include_stack_ptr++;
	  cached_file_stack[include_stack_ptr] = found_it;
	  found_it->next_token_index = 0;
	  current_type[include_stack_ptr] = found_it->ftype;
	  return 0;
  } else {
	  // read it from the file system
	  ftype = TYPE_P;
	  sprintf(filename,"%s/p/%s",pathname, root_name);
	  fp = fopen( filename, "r" );
	  if ( ! fp ) {
		  ftype = TYPE_PART;
		  sprintf(filename,"%s/parts/%s",pathname,root_name);	
		  fp = fopen( filename, "r" );
		  if ( ! fp ) {
			  ftype = TYPE_MODEL;
			  sprintf(filename,"%s/models/%s",pathname,root_name);	
			  fp = fopen( filename, "r" );
			  if ( ! fp ) {
				  ftype = TYPE_OTHER;
				  sprintf(filename,"%s",root_name);	
				  fp = fopen( filename, "r" );
				  if ( ! fp ) {	
					  char buf[300];
					  sprintf(buf,
						  "Warning: can't find file \"%s\" anywhere, ignoring\n",
						  root_name);
					  zWrite(buf);
					  ftype = 0;
				  }
			  }
		  }
	  }
	  if(fp) {
		  include_stack_ptr++;
#if 0
		  fprintf(stderr,"%d: %s\n",
			  include_stack_ptr,filename);fflush(stderr);
#endif
		  fp_stack[include_stack_ptr] = fp;
		  include_stack[include_stack_ptr] = YY_CURRENT_BUFFER;
		  yyin = fp;
		  yy_switch_to_buffer(yy_create_buffer( yyin, YY_BUF_SIZE ) );
		  current_type[include_stack_ptr] = ftype;
		  // if entry in cache is not marked bad, cache this file
		  if (((ftype == TYPE_P) || (ftype == TYPE_PART))
			  && (found_it == NULL)) {
			  CACHED_STREAM *chs = NULL;
			  
			  // find an unused place to store this cached file
			  if (cached_file_stack_index < MAX_CACHED_FILES) {
					  chs = &(cached_streams[cached_file_stack_index]);
					  chs->valid = CHS_FILLING;
					  strcpy(chs->filename, root_name);
					  chs->ftype = ftype;
					  chs->next_token_index = 0;
					  // malloc here
					  chs->tokens =  (int *)malloc(MAX_CACHED_TOKENS*sizeof(int));
					  if (chs->tokens == NULL) {
					    chs->valid = CHS_PROBLEM;
					  } else {
					    chs->values = (YYSTYPE *)malloc(MAX_CACHED_TOKENS*sizeof(YYSTYPE));
						if (chs->values == NULL) {
						  free(chs->tokens);
						  chs->valid = CHS_PROBLEM;
						} else {
					      cached_file_stack_index++;
						}
					  }
			  }
			  // save name and type
			  // set entry in stack so yylex stores the tokens
			  cached_file_stack[include_stack_ptr] = chs;
			  if (ldraw_commandline_opts.debug_level>0){
				  char buf[256];
				  if (chs != NULL) {
					  sprintf(buf,"Caching file %s in slot %d",filename, (cached_file_stack_index-1));
				  } else {
					  sprintf(buf,"No room to cache file %s",filename);
				  }
				  zWrite(buf);
			  }
		  } else {
		      // do not cache this file
			  cached_file_stack[include_stack_ptr] = NULL;
	      }
		  return 0;
	  } else {
		  return -1;
	  }
  }
}

int defer_stop_include_file(void)
{
	static int init=0;
	int i;
	if(!init) {
		init = 1;
		for(i=0; i<MAX_INCLUDE_DEPTH; i++) {
			defered_flag[i] = 0;
		}
	}
	defered_flag[include_stack_ptr-1] = 1;
	return 0;
}

int stop_include_file(void)
{
	if ( include_stack_ptr > 0 ) {	
		if ((cached_file_stack[include_stack_ptr] != NULL) &&
			(cached_file_stack[include_stack_ptr]->valid == CHS_FILLED)) {
			// we were working from cache, so do not close or delete any yy_stuff
			cached_file_stack[include_stack_ptr] = NULL;
			ldlite_profile.cache_hits++;
		} else {
			if ((cached_file_stack[include_stack_ptr] != NULL) &&
				(cached_file_stack[include_stack_ptr]->valid == CHS_FILLING)) {
				cached_file_stack[include_stack_ptr]->valid = CHS_FILLED;
				ldlite_profile.cached_files++;
				if (ldraw_commandline_opts.debug_level>0){
					char buf[256];
					sprintf(buf,"Cached %s, used %d tokens",
						cached_file_stack[include_stack_ptr]->filename,
						cached_file_stack[include_stack_ptr]->next_token_index);
					zWrite(buf);
				}
			} else {
			  ldlite_profile.uncached_files++;
			}
			cached_file_stack[include_stack_ptr] = NULL;
			yy_delete_buffer( YY_CURRENT_BUFFER );
			yy_switch_to_buffer( include_stack[include_stack_ptr] );
			fclose(fp_stack[include_stack_ptr]);
		}
		/* free transform matrix and translation vector */
		pop_transform();
		/* free zcolor table */
		znamelist_pop();

		defered_flag[include_stack_ptr] = 0;
		if (current_type[include_stack_ptr] >= zDetailLevel) {
			zStep(-1,0);
		}
		include_stack_ptr--;
		if (defered_flag[include_stack_ptr] != 0) {
			return stop_include_file();
		} else {
			return include_stack_ptr;
		}
	} else {
		/* free transform matrix and translation vector */
		pop_transform();
		/* free zcolor table */
		znamelist_pop();
		//	  yy_delete_buffer( YY_CURRENT_BUFFER );
		if (ldraw_commandline_opts.debug_level>0){
			char buf[256];
			sprintf(buf,"Profile: %d cached, %d uncached, %d hits",
				ldlite_profile.cached_files,ldlite_profile.uncached_files,
				ldlite_profile.cache_hits);
			zWrite(buf);
		}
		return (-1);
	}
}

void stop_caching()
{
	if (cached_file_stack[include_stack_ptr] != NULL) {
		if (cached_file_stack[include_stack_ptr]->valid == CHS_FILLING) {
			cached_file_stack[include_stack_ptr]->valid = CHS_PROBLEM;
		}
	}
	return;
}

int is_current_file_not_cached()
{
	if (cached_file_stack[include_stack_ptr] == NULL) {
		return 1;
	} else {
		if (cached_file_stack[include_stack_ptr]->valid == CHS_PROBLEM) {
			return 1;
		} else {
			return 0;
		}
	}
}

// TBD: This function probably has memory leaks.
int cache_mpd_subfiles(char *mpd_subfile_name)
{
  // keep stack space light, because this is a recursive function,
  // called for each 0 FILE line in the file.
  CACHED_STREAM *chs = NULL;
  int rc;
  int state;

  // if this file is already cached, return
  // look up to see if file is in cache.
  {
	  int i;

	  for(i=0; i<cached_file_stack_index; i++) {
		  switch (cached_streams[i].valid) {
		  case CHS_UNUSED:
			  break;
		  case CHS_PROBLEM:
		  case CHS_FILLING:
			  if(!stricmp(mpd_subfile_name,cached_streams[i].filename)) {
				  return(-1);
			  }
			  break;
		  case CHS_FILLED:
			  if(!stricmp(mpd_subfile_name,cached_streams[i].filename)) {
				  return(0);
			  }
			  break;
		  }
	  }		  
  }
			  
  // find an unused place to store this cached file
  if (cached_file_stack_index >= MAX_CACHED_FILES) {
    if (ldraw_commandline_opts.debug_level>0){
 	  char buf[256];
	  sprintf(buf,"Not caching MPD subfile %s, too many files already cached.",mpd_subfile_name);
	  zWrite(buf);
    }
    return(-1);
  }
  chs = &(cached_streams[cached_file_stack_index]);
  chs->valid = CHS_FILLING;
  strcpy(chs->filename, mpd_subfile_name);
  chs->ftype = TYPE_PART;  // treat it like a part for purposes of screen updates
  chs->next_token_index = 0;
  // malloc here
  chs->tokens =  (int *)malloc(MAX_MPD_CACHED_TOKENS*sizeof(int));
  if (chs->tokens == NULL) {
    if (ldraw_commandline_opts.debug_level>0){
 	  char buf[256];
	  sprintf(buf,"Not caching MPD subfile %s, out of memory (1).",mpd_subfile_name);
	  zWrite(buf);
    }
	return (-1);
  }
  chs->values = (YYSTYPE *)malloc(MAX_MPD_CACHED_TOKENS*sizeof(YYSTYPE));
  if (chs->values == NULL) {
      free(chs->tokens);
      if (ldraw_commandline_opts.debug_level>0){
 	    char buf[256];
	    sprintf(buf,"Not caching MPD subfile %s, out of memory (2).",mpd_subfile_name);
	    zWrite(buf);
      }
	  return (-1);
  }
  cached_file_stack_index++;

  if (ldraw_commandline_opts.debug_level>0){
	  char buf[256];
	  sprintf(buf,"Caching MPD subfile %s",mpd_subfile_name);
	  zWrite(buf);
  }
  state = 0;
  while (1) {
	rc = my_yylex();
	if (rc == tEOF) {
		chs->tokens[chs->next_token_index] = rc;
		chs->values[chs->next_token_index] = yylval;
		chs->next_token_index++;
		if (chs->valid == CHS_FILLING) {
	        chs->valid = CHS_FILLED;
 	        if (ldraw_commandline_opts.debug_level>0){
					char buf[256];
					sprintf(buf,"Cached %s, used %d tokens",
						chs->filename,
						chs->next_token_index);
					zWrite(buf);
	        }
		}
	  return(0);
	} else {
	  if (chs->next_token_index >= MAX_CACHED_TOKENS) {
		stop_caching();
		if (ldraw_commandline_opts.debug_level>0){ 
			char buf[256];
			sprintf(buf,"MPD subfile %s is too large to cache",chs->filename);
			zWrite(buf);
		}
	  } else {
		chs->tokens[chs->next_token_index] = rc;
		chs->values[chs->next_token_index] = yylval;
		chs->next_token_index++;
	  }
	  switch (state) {
	   case 0:
	    if (rc == tZERO) {
			state = 1;
		}
		break;
	   case 1:
	    if (rc == tFILE) {
		  state = 2;
		} else {
		  state = 0;
		}
		break;
	   case 2:
	    if (rc = tIDENT) {
		  if (chs->valid == CHS_FILLING) {
		    // remove "0 FILE name" from this stream...
		    chs->next_token_index -= 3;
			// and insert an EOF character
		    chs->tokens[chs->next_token_index] = tEOF;
		    chs->values[chs->next_token_index].c  = "EOF";
		    chs->next_token_index++;
            //
		    chs->valid = CHS_FILLED;
		    if (ldraw_commandline_opts.debug_level>0){
					char buf[256];
					sprintf(buf,"Cached %s, used %d tokens",
						chs->filename,
						chs->next_token_index);
					zWrite(buf);
		    }
		  }
		  return cache_mpd_subfiles(yylval.c);
		} else {
		  state = 0;
		}
		break;
	  }
	  if (chs->next_token_index >= MAX_CACHED_TOKENS) {
		stop_caching();
		if (ldraw_commandline_opts.debug_level>0){ 
			char buf[256];
			sprintf(buf,"MPD subfile %s is too large to cache",chs->filename);
			zWrite(buf);
		}
	  }
	}
  }
}

int yylex(void)
{
	static int init=0;
	int rc;

	if(!init) {
		int i;

		init=1;
		include_stack_ptr = 0;
		for (i=0; i<MAX_INCLUDE_DEPTH; i++) {
			cached_file_stack[i] = NULL;
		}
		for (i=0; i<MAX_CACHED_FILES; i++) {
			cached_streams[i].valid = CHS_UNUSED;
		}
		ldlite_profile.cached_files=0;
		ldlite_profile.uncached_files=0;
		ldlite_profile.cache_hits=0;
	}

	if (cached_file_stack[include_stack_ptr] == NULL) {
		// top level .dat file (or caching disabled), do not cache
		return my_yylex();
	} else {
		CACHED_STREAM *chs;
		// we are inside an included file.  Either it is being cached or has already been cached
		chs = cached_file_stack[include_stack_ptr];
		switch(chs->valid) {
		case CHS_UNUSED:
		case CHS_PROBLEM:
			// error condition?
			return my_yylex();
			break;
		case CHS_FILLING:
			rc = my_yylex();
			chs->tokens[chs->next_token_index] = rc;
			chs->values[chs->next_token_index] = yylval;
			chs->next_token_index++;
			if (chs->next_token_index >= MAX_CACHED_TOKENS) {
				stop_caching();
				if (ldraw_commandline_opts.debug_level>0){ 
					char buf[256];
					sprintf(buf,"File %s is too large to cache",chs->filename);
					zWrite(buf);
				}
			}
			return rc;
			break;
		case CHS_FILLED:
			rc = chs->tokens[chs->next_token_index];
			yylval = chs->values[chs->next_token_index];
			chs->next_token_index++;
			return rc;
			break;
		}
	}
}
