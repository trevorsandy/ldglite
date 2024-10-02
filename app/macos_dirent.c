/* * Dirent for Mac       �1997 Mikl�s Fazekas. All rights reversed. *//*    NOTE: This file was written by Mikl�s Fazekas, the developer    of Mesa for MacOS.  I don't believe I've many any substantial    changes.    -Scott Gilroy */#ifdef macintosh        // don't use any of this file in other OSes#include "macos_dirent.h"#include <stdlib.h>#include <assert.h>#include <string.h>#include <Files.h>#include <TextUtils.h>/* * Open a directory.  This means calling PBOpenWD. * The value returned is always the address of opened, or NULL. * (I have as yet no use for multiple open directories; this could * be implemented by allocating memory dynamically.) */DIR *opendir(char *path){    int i;    WDPBRec paramBlock;    char ppath[MAXPATH];    OSErr error;    DIR *pDir = malloc(sizeof(DIR));    if (pDir == nil)        return nil;    strncpy(&ppath[1],path,ppath[0]=strlen(path));    for (i = 0; i < strlen(path); i++)    {        if (ppath[i+1] == '.')            ppath[i+1] = ':';        if (ppath[i+1] == '/')            ppath[i+1] = ':';    }    paramBlock.ioCompletion = nil;    paramBlock.ioWDProcID = 0;    paramBlock.ioWDDirID = 0;    paramBlock.ioNamePtr = (StringPtr)ppath;    paramBlock.ioVRefNum = 0;    error = PBOpenWDSync(&paramBlock);    if (error != noErr)    {        free(pDir);        return nil;    }    pDir->vRefNum = paramBlock.ioVRefNum;    pDir->fileIndex = 1;    return pDir;}/* * Close a directory. */voidclosedir(DIR *dirp){    WDPBRec paramBlock;    paramBlock.ioCompletion = 0;    paramBlock.ioVRefNum = dirp->vRefNum;    PBCloseWDSync(&paramBlock);    free(dirp);}/* * Read the next directory entry. */struct dirent *readdir(DIR *dirp){    CInfoPBRec  paramBlock;    OSErr       error;    assert(dirp);    dirp->dirEntryBuffer.d_name[0] = 0;    paramBlock.dirInfo.ioNamePtr = (StringPtr)dirp->dirEntryBuffer.d_name;    paramBlock.dirInfo.ioVRefNum = dirp->vRefNum;    paramBlock.dirInfo.ioFDirIndex = dirp->fileIndex++;    paramBlock.dirInfo.ioDrDirID = 0;    error = PBGetCatInfoSync(&paramBlock);    if (error != noErr)        return nil;    dirp->dirEntryBuffer.d_ino = dirp->fileIndex;    dirp->dirEntryBuffer.d_reclen = 0;    p2cstr((StringPtr)(dirp->dirEntryBuffer.d_name));    dirp->dirEntryBuffer.d_namlen = strlen(dirp->dirEntryBuffer.d_name);    return &dirp->dirEntryBuffer;}#endif