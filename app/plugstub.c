#include "platform.h"
#include "plugins.h"

// UNIX stubs for now.

char *plugin(plugstruct *plug,
	   unsigned char *CompleteText,
	   unsigned char *SelText,
	   unsigned long *SelStart,
	   unsigned long *SelLength,
	   unsigned long *CursoRow,
	   unsigned long *CursorColum)
{
  return NULL;
}

plugstruct *pluginfo(char *dllname)
{
  return NULL;
}

