// ***************************************************************************
//
//  platform.c 
//  platform-dependent path handling and supplemental string library routines
//  platform_startup() is a place for any conditional code required by platform
//
// ***************************************************************************




#include "platform.h"

#if defined(MAC)
#include "macprefs.h"
#include "macstartup.h"
#endif


// ***************************************************************
//  platform_startup() for any conditional code required by platform
// ***************************************************************
int platform_startup(int *argcp, char ***argv)
{
	#if defined(MAC)
	return macStartup(argcp, argv);
	#endif
    return 0;
}

// ***************************************************************
//  strdup - malloc space and copy the string
// ***************************************************************

#if LACKS_STRDUP

char *	strdup(const char *str)
{
  char *newstr;
	
  if (str == NULL)
  {
    return NULL;
  }
  newstr = malloc(strlen(str) + 1);
  strcpy(newstr, str);
	
  return newstr;
}

#endif

// NOTE: we could use strcasecmp() and strncasecmp() in linux.

// ***************************************************************
//  stricmp - case insensitive strcmp() 
// ***************************************************************
#if LACKS_STRICMP

int stricmp(const char *str1, const char *str2) 
{
  const unsigned char * ptr1 = (unsigned char *) str1;
  const unsigned char * ptr2 = (unsigned char *) str2;
  unsigned char c1, c2;
	
  while ((c1 = toupper(*ptr1++)) == (c2 = toupper(*ptr2++)))
  {
    if (!c1)
    {
      return(0); // end of both strings reached, so they match
    }
  }
  // first non-matching char was reached, including possibly 0 on one or the other string
  return(c1 - c2);
}

#endif


// ***************************************************************
//  strnicmp - case insensitive strncmp()
// ***************************************************************
#if LACKS_STRNICMP

int strnicmp(const char *str1, const char *str2, size_t n) 
{
  const unsigned char * ptr1 = (unsigned char *) str1;
  const unsigned char * ptr2 = (unsigned char *) str2;
  unsigned char c1, c2;
	
  while (--n)
  {
    if ((c1 = toupper(*ptr1++)) != (c2 = toupper(*ptr2++)))
    {
      return (c1-c2);
    }
    else if (!c1)
    {
      break;
    }
  }

  return(0); // went n chars with no mismatch
}

#endif





// ***********************************************************************
//  dirname - return the part of a pathname that is the directory (or ./)
// ***********************************************************************

#if LACKS_DIRNAME

char *dirname( const char *filepath )
{
  char *tmpstr, *ptr;
  int i;

  if (filepath == NULL)
  {
    return NULL;
  }
  
#if defined(MAC)

  if ( (ptr = strrchr(filepath, ':')) )

#elif defined(WINDOWS)

  if ( (ptr = strrchr(filepath, '\\')) || (ptr = strrchr(filepath, '/')) )

#elif defined(MACOS_X)

  if ( (ptr = strrchr(filepath, '/'))  || (ptr = strrchr(filepath, ':')) )

#elif defined(UNIX)

  if ( (ptr = strrchr(filepath, '/')) )

#else
#error ambiguous platform in dirname() definition
#endif
  {
    i = ptr - filepath;
    tmpstr = malloc(i+1);
    strncpy(tmpstr, filepath, i); // do not include trailing separator
    tmpstr[i] = 0;
  }
  else
  {
#if defined(MACOS_X)

    tmpstr = strdup("./");

#elif defined(MAC)

    tmpstr = strdup("");

#elif defined(WINDOWS)

    tmpstr = strdup(".\\");

#elif defined(UNIX)

    tmpstr = strdup("./");

#else
#error unspecified platform in dirname() definition
#endif
  }
  return tmpstr;
}
#endif

// ********************************************************************
//  basename - return the part of a pathname that is NOT the directory
// ********************************************************************

#if LACKS_BASENAME

char *basename( const char *filepath )
{
  char *tmpstr, *ptr;

  if (filepath == NULL)
  {
    return NULL;
  }

  
#if defined(MAC)

  if ( (ptr = strrchr(filepath, ':')) ) 

#elif defined(WINDOWS)

  if ( (ptr = strrchr(filepath, '\\')) || (ptr = strrchr(filepath, '/')) )

#elif defined(MACOS_X)

  if ( (ptr = strrchr(filepath, '/'))  || (ptr = strrchr(filepath, ':')) )

#elif defined(UNIX)

  if ( (ptr = strrchr(filepath, '/')) )

#else
#error unspecified platform in basename() definition
#endif
  {
    // If there isn't anything after the last separator, the result is a 0-length string
    tmpstr = malloc(strlen(ptr+1)+1);
    tmpstr = strdup(ptr+1);
  }
  else
  {
    // dup the string, so caller can safely free whatever we return
    tmpstr = strdup(filepath);
  }
  return tmpstr;
}
#endif

// ********************************************************************
// strlastchar - return last char of string (convenience function)
// ********************************************************************

char strlastchar(const char *str1)
{
  int i;
  i = strlen(str1);
  if (i>0) { i--; }
  return str1[i];
}

// ***************************************************************************
//  localize_path - change embedded directory separators from / or \\
// ***************************************************************************
char *
localize_path(char *inoutPath)
{
#if defined(UNIX)
  char separator = '/';

#elif defined(MACOS_X)
  char separator = '/';

#elif defined(MAC)
  char separator = ':';
  
#elif defined(WINDOWS)
  char separator = '\\';
  
#else
#error unspecified platform in localize_path() definition
#endif
	
  int i;
  
  for(i=0; i<strlen(inoutPath); i++) 
  {
    /* Localize Directory Separators */
    if ((inoutPath[i] == '/') 
	|| (inoutPath[i] == '\\')
#if defined(MACOS_X)
	|| (inoutPath[i] == ':')
#endif
	)
    {
      inoutPath[i] = separator; 
    }
    
  }
  return inoutPath;
}

// ******************************************************************************
//  concat_path - concatenate path1 and path2 with directory separator if needed
// ******************************************************************************

char *concat_path(const char *path1, const char *path2, char *result)
{
  char *ptr;
  int i;
	
  if (!result)
  {
    result = malloc(strlen(path1)+strlen(path2)+2);
  }

  ptr = result;
  ptr[0] = 0;
	
	
  strcat(ptr, path1); ptr += strlen(ptr);
  
#if defined(MAC)
  if (strlastchar(path1) != ':')
  {
    strcat(ptr, ":"); ptr += strlen(ptr);
  }

#elif defined(MACOS_X)
  if ((strlastchar(path1) != '/') && (strlastchar(path1) != ':'))
  {
    strcat(ptr, "/"); ptr += strlen(ptr);
  }
  
#elif defined(UNIX)
  if (strlastchar(path1) != '/')
  {
    strcat(ptr, "/"); ptr += strlen(ptr);
  }

#elif defined(WINDOWS)
  if ((strlastchar(path1) != '/') && (strlastchar(path1) != '\\'))
  {
    strcat(ptr, "\\"); ptr += strlen(ptr);
  }
  
#else
#error unspecified platform in format_path() definition
#endif
  
  strcat(ptr, path2); ptr += strlen(ptr);
  
  localize_path(result); // Just in case of embedded separators.
  // Subparts use embedded \s and hires primitives use embedded \48

  return result;
}

// ********************************************************************************
//  platform_getenv - wrapper for getenv, in case we want to make a nice interface
// ********************************************************************************

char *platform_getenv(const char *var)
{
#if defined(UNIX)
  return getenv(var);

#elif defined(MACOS_X)
  return getenv(var);

#elif defined(MAC)
  return macprefs_getenv(var);
  
#elif defined(WINDOWS)
  return getenv(var);
  
#else
#error unspecified platform in platform_getenv() definition
#endif
		
}

/***************************************************************/
#include <stdio.h>

void platform_comment(char *message, int level)
{
  // Handy place to parse new META commands unhandled by ldlite parser.
  extern int ldlite_parse_colour_meta(char *s);

  // Intercept the LDConfig.ldr !COLOUR meta command.
  if (ldlite_parse_colour_meta(message))
    return;

  if (level == 0)
    printf("comment %s\n", message);
}


/***************************************************************/

#ifndef _MSC_VER
  #include <sys/file.h>
#endif
#include <sys/stat.h>
#include <sys/types.h>

#if defined(WINDOWS)
#  include <windows.h>
#endif

#if defined(UNIX)
#include <sys/param.h>
#include <limits.h> // For realpath() on Red Hat 9.

#define PATHSIZE MAXPATHLEN
#else
#ifndef MAXNAMLEN
#define MAXNAMLEN 256
#endif
#define PATHSIZE MAXNAMLEN
#endif

/***********************************************************************/
long get_file_mode(char *path)
{
  struct stat     stats;
  
  if ( stat( path, &stats) < 0 ) 
    return ( -1 );
  return ( stats.st_mode );
}

/***********************************************************************/
/*  Strip off first element of path into buf.  Returns new path. */
char *getPathElement(char *path, char *buf)
{
  while ( (*buf = *path++) != '\0' ) {
    if (*buf == ':') {
      *buf = '\0';
      return(path);
    } ++buf;
  } return(0);
}

/***********************************************************************/
int GetExecName(char *argv0, char *buf, int buflen)
{
  char    *path;
  char    *getenv();
  
#ifdef WINDOWS
  GetModuleFileName(NULL, buf, buflen);
  return(0);
#else
  // getexecname() on Solaris??

  char    tmpbuf[PATHSIZE];

  // Handle absolute paths.  Easy!
  if (*argv0 == '/') {
    strcpy( buf, argv0 );
    return(0);
  }

  // Handle relative paths.  Almost easy.  Work off of getcwd()
  if (*argv0 == '.' && *(argv0+1) == '/') {
    strcpy(buf, argv0 + 2);
    goto found;
  }
  if (*argv0 == '.' && *(argv0+1) == '.' && *(argv0+2) == '/' ) {
    strcpy(buf, argv0);
    goto found;
  }

  // Search for the executable in the PATH environment variable.
  path = getenv("PATH");
  while(path != 0) {
    path = getPathElement(path, buf);
    if (*buf == '\0')
      continue;  /* ignore empty components (::) */
    strcat(buf, "/");
    strcat(buf, argv0);
    if (access(buf, X_OK) == 0) {
      /* watch out for directories that happen to match */
      if((get_file_mode(buf) & S_IFMT) == S_IFDIR) continue;
      /* Got it.  Now if it is not absolute, just prepend cwd */
      if (*buf != '/') {
	
      found:
	strncpy(tmpbuf, buf, PATHSIZE/sizeof(char));
#if 0	
	if ( getcwd(buf, (PATHSIZE/sizeof(char))-1) == 0 ) break;
	strncat(buf, "/", PATHSIZE/sizeof(char));
	strncat( buf, tmpbuf, PATHSIZE/sizeof(char));
#else
	if ( getcwd(buf, buflen-1) == 0 ) break;
	strncat(buf, "/", buflen);
	strncat( buf, tmpbuf, buflen);
#endif	
	if ( realpath(buf, tmpbuf) == NULL)
	  return 1;
	
	//strncpy(buf, tmpbuf, PATHSIZE/sizeof(char));
	strncpy(buf, tmpbuf, buflen);

	return 2;
      }
      return(0);
    }
  }
  
  /* Not found */
  *buf = 0;
  return(-1);
#endif
}



