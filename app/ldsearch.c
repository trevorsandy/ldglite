/* ldsearch.c  Extra Routines for LDrawIni SearchDirs
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#ifdef __TURBOC__
#include <alloc.h>
#endif
#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>
#endif
#include "LDrawIni.h"
#include "LDrawInP.h"

LDRAWINI_BEGIN_STDC

/* Split LDSEARCHDIRS and add dir, dir/p, dir/parts, dir/models for each */
/* split onto the end of the baseline search dirs list. */
/* Returns 1 if OK, 0 on error */
static int SplitLDSearch(const char *LDrawSearchString, int *nDirs, char ***Dirs)
{
   const char    *s;
   const char    *t;
   char          *Dir;
   int            n;
   int            Len;
   char         **iniDirs = *Dirs;

   /* Count number of dir separators '|' */
   for (n = *nDirs+1, s = strchr(LDrawSearchString, '|'); s; s = strchr(s + 1, '|'))
      n+=4;
   *Dirs = (char **) malloc(n * sizeof(char *));
   if (!*Dirs)
      return 0;

   for (n = 0; n < *nDirs; n++)
      (*Dirs)[n] = iniDirs[n];
   free(iniDirs);
   for (n = *nDirs, s = LDrawSearchString; *s;)
   {
      t = s;
      while (*t && *t != '|')
         ++t;
      Len = t - s;
      Dir = (char *) malloc(Len + 1);
      if (!Dir)
         return 0;
      memcpy(Dir, s, Len);
      Dir[Len] = '\0';
      (*Dirs)[n++] = Dir;
      // Add p
      Dir = (char *) malloc(Len + 1 + 2);
      if (!Dir)
         return 0;
      memcpy(Dir, s, Len);
      Dir[Len] = '\0';
      strcat(Dir,"/p");
      (*Dirs)[n++] = Dir;
      // Add parts
      Dir = (char *) malloc(Len + 1 + 6);
      if (!Dir)
         return 0;
      memcpy(Dir, s, Len);
      Dir[Len] = '\0';
      strcat(Dir,"/parts");
      (*Dirs)[n++] = Dir;
      // Add models
      Dir = (char *) malloc(Len + 1 + 7);
      if (!Dir)
         return 0;
      memcpy(Dir, s, Len);
      Dir[Len] = '\0';
      strcat(Dir,"/models");
      (*Dirs)[n++] = Dir;

      s = *t ? t + 1 : t;
   }
   *nDirs = n;
   return 1;
}

/*
Add dirs from LPub3D LDSEARCHDIRS environment var to the dirs in LDrawIni.
*/
int LDSearchDirsGet(struct LDrawIniS * LDrawIni)
{
   struct LDrawIniPrivateDataS *pd;
   const char    *e;

   /* LDrawSearch, read symbolic dirs */
   pd = LDrawIni->PrivateData;

   /* Now read extra LDSEARCHDIRS for LPub3D. */
   if (e = getenv("LDSEARCHDIRS"))
      return SplitLDSearch(e, &pd->nSymbolicSearchDirs, &pd->SymbolicSearchDirs);

   return 1;                    /* None found.  That's ok. */
}                               /* LDSearchDirsGet                       */
