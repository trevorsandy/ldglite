#ifndef PLUGINS_H
#define PLUGINS_H

typedef struct plugstruct_tag {
  char *dllname;
  char *name;
  char *menuentry;
  char *version;
  char *description;
  char *author;
  char *comment;
  char *plugtype;  
} plugstruct;

char *plugin(plugstruct *plug,
	   unsigned char *CompleteText,
	   unsigned char *SelText,
	   unsigned long *SelStart,
	   unsigned long *SelLength,
	   unsigned long *CursoRow,
	   unsigned long *CursorColum);

plugstruct *pluginfo(char *dllname);

#endif // PLUGINS_H
