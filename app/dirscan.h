
#define NAMELENGTH        1024
#define WORDLENGTH        256
#define MAX_DIR_ENTRIES   10

#define boolean int
#define false 0
#define true 1

int ScanDirectory(char *, char *, int, char filelist[MAX_DIR_ENTRIES][NAMELENGTH]);
boolean CheckFile(char *, char *);
int isDir(char *);

