/*
  ldglite_launcher.c

  Author: Frederic Devernay <frederic.devernay@m4x.org>

  Licence:
  Creative Commons BY-NC-SA 3.0 license
  http://creativecommons.org/licenses/by-nc-sa/3.0/

  This small sample program sets the environment variable
  DYLD_LIBRARY_PATH and executes another binary with the same
  arguments.

  This can be used when a given program was compiled using GCC
  libraries that are different from the system-provided ones,
  e.g. when compiling with a recent version of GCC. In this case, put
  the GCC libraries (and only these) in a directory, and modify
  DYLD_LIBRARY_PATH in the code below to point to the correct
  lcation. These libraries are made to be backward-compatible. Never
  set DYLD_LIBRARY_PATH to a directory containing all your shared
  libraries, because the system frameworks sometimes load shared
  libraries which have very common names (eg the ImageIO framework
  loads a libJPEG.dylib), and may not work anymore.

  In the following example, the binary name is the name of this
  executable with the suffix "-driver", and the DYLD_LIBRARY_PATH is
  %s/../Frameworks/libgcc where %s is the dirname of this
  executable. These should probably be adated to your needs.

  This sample is part of the xcodelegacy source code.
  https://github.com/devernay/xcodelegacy
 */
#if __APPLE__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include <sys/errno.h>
#include <mach-o/dyld.h> /* for _NSGetExecutablePath(char* buf, uint32_t* bufsize); */
#include <limits.h> /* PATH_MAX */

int main(int argc, char *argv[])
{
  char programPathIn[PATH_MAX];
  char programPath[PATH_MAX];
  uint32_t buflen = PATH_MAX;
  _NSGetExecutablePath(programPathIn, &buflen);
  
  /* 
   * add up to '...ldglite' (remove '_launcher') to the program name to get the executable binary path 
   */
  uint32_t suffLen = 9;     /* _launcher */   
  uint32_t pathLen = strlen(programPathIn);
  uint32_t rightTrimLen = pathLen - suffLen;
  strncpy(programPath, programPathIn, rightTrimLen);
  programPath[rightTrimLen] = '\0'; 
  printf("ProgramPath %s\n", programPath);

  /*
   * set the DYLD_LIBRARY_PATH environment variable to the directory containing only a link to libstdc++.6.dylib
   * and re-exec the binart. The re-exec is necessary as the DYLD_LIBRARY_PATH is only read at exec.
   */
  char *dyldLibraryPathDef;
  char *programDir = dirname(programPath);
  if (asprintf(&dyldLibraryPathDef, "DYLD_LIBRARY_PATH=%s/../Frameworks/libgcc", programDir) == -1) {
    fprintf(stderr, "Could not allocate space for defining DYLD_LIBRARY_PATH environment variable\n");
    exit(1);
  }
  printf("Environment varialbe DYLD_LIBRARY_PATH %s\n", dyldLibraryPathDef);
  
  putenv(dyldLibraryPathDef);
  execv(programPath, argv); // note that argv is always NULL-terminated

  fprintf(stderr, "execv(%s(%d), %s(%d), ...) failed: %s\n", programPath, (int)strlen(programPath), argv[0], (int)strlen(argv[0]), strerror(errno));
  exit(1);
}
#endif
