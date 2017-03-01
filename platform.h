/*
 * Platform specific code for ldglite                          
 *                                                             
 * Concerned with:                                             
 *    pathname handling     
 *    non-standard library functions                                   
*/



#ifndef PLATFORM_H
#define PLATFORM_H

#include <string.h>
#include <stdlib.h>


// Flags indicating what functions are lacking on this platform

#if defined(MACOS_X) 
// Look for MACOS_X *before* UNIX since its a special case of UNIX
// and defines both.

// unistd.h declares chdir() on Mac, Unix?
#include <unistd.h>

#define LACKS_STRDUP 0
#define LACKS_STRICMP 1
#define LACKS_STRNICMP 1
#define LACKS_DIRNAME 1
#ifndef AGL_OFFSCREEN_OPTION
#define LACKS_BASENAME 1
#endif

#elif defined(UNIX) 

// unistd.h declares chdir() on Mac, Unix?
#include <unistd.h>
// when compiling with qmake, strdup is available.
#if defined(USING_QMAKE)
    #define LACKS_STRDUP 0
#else
    #define LACKS_STRDUP 1
#endif
#define LACKS_STRICMP 1
#define LACKS_STRNICMP 1
#define LACKS_DIRNAME 0
#define LACKS_BASENAME 0

// Get dirname and basename from libgen.h (on Redhat 9 linux, anyhow)
#include <libgen.h>

#elif defined(MAC)

// unistd.h declares chdir() on Mac, Unix?
#include <unistd.h>

#define LACKS_STRDUP 1
#define LACKS_STRICMP 1
#define LACKS_STRNICMP 1
#define LACKS_DIRNAME 1
#define LACKS_BASENAME 1

#else  // Windows MS VC++

#define LACKS_STRDUP 0
#define LACKS_STRICMP 0
#define LACKS_STRNICMP 0
#define LACKS_DIRNAME 1
#define LACKS_BASENAME 1

// Use convenient flag WINDOWS to look consistent
#ifndef WINDOWS
#define WINDOWS 1
#endif

// glut 3.7 no longer includes windows.h
#if (GLUT_XLIB_IMPLEMENTATION >= 13)
# ifdef CALLBACK
#  undef CALLBACK
# endif
# include <windows.h>
#endif

// Differentiate between VC++ and MINGW32 (or Cygnus).
#ifdef _MSC_VER
#  ifndef S_ISDIR
#    ifdef S_IFDIR
#      define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#    else
#      define S_ISDIR(m) 0
#    endif
#  endif /* !S_ISDIR */
#  define sleep _sleep
#endif

#endif

#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

// Declarations as needed

#if LACKS_STRDUP
char * strdup( const char *str );
#endif

#if LACKS_STRICMP
int stricmp( const char *str1, const char *str2 );
#endif

#if LACKS_STRNICMP
int strnicmp( const char *str1, const char *str2, size_t n );
#endif

#if LACKS_DIRNAME
char * dirname( const char *filepath );
#endif

#if LACKS_BASENAME
char * basename( const char *filepath );
#endif

// Plain Declarations

char *concat_path(const char *path1, const char *path2, char *result);
char *localize_path(char *inoutPath);
char *platform_getenv(const char *var);
void platform_comment(char *message, int level);
int platform_startup(int *argcp, char ***argv);

int GetExecName(char *argv0, char *buf, int buflen);

#endif // PLATFORM_H



