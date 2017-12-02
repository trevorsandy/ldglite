// ===============================================
// File:         read.cxx
// Description:	 
// Version 1:    
// Current:      26 Sep 1999
// Author(s):    Thomas Kern
// ===============================================

//

/*
 * Copyright (C) 1997-1998  Thomas Kern
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 */


//=====================================================================
//	INCLUDES
//=====================================================================
//
#include <sys/stat.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "platform.h"


#ifndef UNIX
// For some reason this does not work in linux.  Investigate...
#define USE_DIRENT 1
#endif

#ifdef AGL
// This compiles easiest with DJGPP, but does it work?
#define USE_DIRENT 1
#endif

#ifdef _WIN32_NATIVE_FILE_OPS
 #include "windows.h"
#endif

#ifdef USE_DIRENT
#if defined(MAC)
#include "macos_dirent.h"
#elif defined(WINDOWS)
#include "win32_dirent.h"
#else
#include <dirent.h>  // directory operations
#endif
#else // NOT USE_DIRENT
 #include <glob.h>
#endif

#include "dirscan.h"

//=====================================================================
static const int maxDir = 10;
static char scannedDir[10][NAMELENGTH];
static int  dirCount = 0;

//---------------------------------------------------------------------
// ein Verzeichnis unter auslesen
int ScanDirectory(char *dir, char *pattern, int firstfile,
                   char  filelist[MAX_DIR_ENTRIES][NAMELENGTH])
{
  char  wildcard[NAMELENGTH];
  int   lenDir;
  int   filecount = 0;
  int   i;
  UNUSED(lenDir);
#ifdef _WIN32_NATIVE_FILE_OPS
  HANDLE            searchPath = NULL;
  // LPCTSTR == const char *
  WIN32_FIND_DATA   dataStruct;
#endif
#ifdef USE_DIRENT
  DIR           *searchPath = NULL;
  struct dirent *dataStruct;
#else // Unix
  size_t cnt, length;
  glob_t glob_results;
  char **p;
  UNUSED(length);
#endif

  for(i = 0; i < MAX_DIR_ENTRIES; i++)
    strcpy(filelist[i], "");

  concat_path(dir, pattern, wildcard);

#ifdef _WIN32_NATIVE_FILE_OPS
  searchPath = FindFirstFile( (LPCTSTR)wildcard, &dataStruct);
  if( searchPath == INVALID_HANDLE_VALUE ) 
  {
    printf("INVALID_HANDLE_VALUE\n");
    return 0;
  }

  // Skip files before our current window.
  for ( filecount = 0; filecount < firstfile; )
  { 
    concat_path(dir, dataStruct.cFileName, filelist[0]);
    if (isDir(filelist[0]))
      strcpy(filelist[0], "");
    else
      filecount++;
    if (FindNextFile(searchPath, &dataStruct) == 0)
    {
      FindClose(searchPath);
      return 0;
    }
  }
  filecount = 0;
  while( filecount < MAX_DIR_ENTRIES )
  {  
    concat_path(dir, dataStruct.cFileName, filelist[filecount]);
    if (isDir(filelist[filecount]))
      strcpy(filelist[filecount], "");
    else
      filecount++;
    if (FindNextFile(searchPath, &dataStruct) == 0)
      break;
  }
  FindClose(searchPath);

#endif
#ifdef USE_DIRENT
  if( (searchPath = opendir(wildcard)) ) { 
    // Skip files before our current window.
    for ( filecount = 0; filecount < firstfile; )
    { 
      if (!(dataStruct = readdir(searchPath)))
      {
	closedir(searchPath);
	return(0);
      }
      concat_path(dir, dataStruct->d_name, filelist[0]);
      if (isDir(filelist[0]))
	strcpy(filelist[0], "");
      else
	filecount++;
    }
    filecount = 0;
    while( filecount < MAX_DIR_ENTRIES
	   && (dataStruct = readdir(searchPath)) ) {
      concat_path(dir, dataStruct->d_name, filelist[filecount]);
      if (isDir(filelist[0]))
	strcpy(filelist[0], "");
      else
	filecount++;
    }
    closedir(searchPath);
  }
#else // Unix
  glob(wildcard, 0, 0, &glob_results);

  // Skip files before our current window.
  filecount = 0;
  for (p = glob_results.gl_pathv, cnt = glob_results.gl_pathc;
       cnt; p++, cnt--)
  {
    if ( filecount >= firstfile )
      break;
    if (!isDir(*p))
      filecount++;
  }

  /* generate the list.  */
  for ( filecount = 0; cnt; p++, cnt--)
  {
    if (filecount >= MAX_DIR_ENTRIES)
      break;
    if (!isDir(*p))
    {
      strcpy(filelist[filecount], *p);
      filecount++;
    }
  }
  globfree(&glob_results);

#endif 

  return filecount;
}

//---------------------------------------------------------------------
// ein Verzeichnis unter auslesen
int ScanFolder(char *dir, char *pattern, int firstfile,
                   char  filelist[MAX_DIR_ENTRIES][NAMELENGTH])
{
  char  wildcard[NAMELENGTH];
  int   lenDir;
  int   filecount = 0;
  int   i;
  UNUSED(lenDir);
#ifdef _WIN32_NATIVE_FILE_OPS
  HANDLE            searchPath = NULL;
  // LPCTSTR == const char *
  WIN32_FIND_DATA   dataStruct;
#endif
#ifdef USE_DIRENT
  DIR           *searchPath = NULL;
  struct dirent *dataStruct;
#else // Unix
  size_t cnt, length;
  glob_t glob_results;
  char **p;
  UNUSED(length);
#endif
    
  for(i = 0; i < MAX_DIR_ENTRIES; i++)
    strcpy(filelist[i], "");

  concat_path(dir, pattern, wildcard);

#ifdef _WIN32_NATIVE_FILE_OPS
  searchPath = FindFirstFile( (LPCTSTR)wildcard, &dataStruct);
  if( searchPath == INVALID_HANDLE_VALUE ) 
  {
    printf("INVALID_HANDLE_VALUE\n");
    return 0;
  }

  // Skip files before our current window.
  for (filecount = 0; filecount < firstfile; )
  {
    concat_path(dir, dataStruct.cFileName, filelist[0]);
    if (isDir(filelist[0]))
      filecount++;
    else
      strcpy(filelist[0], "");
    if (FindNextFile(searchPath, &dataStruct) == 0)
    {
      FindClose(searchPath);
      return 0;
    }
  }

  filecount = 0;
  while( filecount < MAX_DIR_ENTRIES )
  {
    concat_path(dir, dataStruct.cFileName, filelist[filecount]);
    if (isDir(filelist[filecount]))
      filecount++;
    else
      strcpy(filelist[filecount], "");
    if (FindNextFile(searchPath, &dataStruct) == 0)
      break;
  }
  FindClose(searchPath);

#endif
#ifdef USE_DIRENT
  if( (searchPath = opendir(wildcard)) ) { 
    // Skip files before our current window.
    for ( filecount = 0; filecount < firstfile; )
    { 
      if (!(dataStruct = readdir(searchPath)))
      {
	closedir(searchPath);
	return(0);
      }
      concat_path(dir, dataStruct->d_name, filelist[filecount]);
      if (isDir(filelist[filecount]))
	filecount++;
      else
	strcpy(filelist[filecount], "");
    }
    filecount = 0;
    while( filecount < MAX_DIR_ENTRIES 
	   && (dataStruct = readdir(searchPath)) ) {
      concat_path(dir, dataStruct->d_name, filelist[filecount]);
      if (isDir(filelist[filecount]))
	filecount++;
      else
	strcpy(filelist[filecount], "");
    }
    closedir(searchPath);
  }
#else // Unix

  #ifndef GLOB_PERIOD
  // Some BSD systems do NOT have the GNU implementation of glob,
  // so we must do the GLOB_PERIOD work by hand to get the ".." dir.
  #define GLOB_PERIOD GLOB_APPEND
  glob(".", 0, 0, &glob_results);
  glob("..", GLOB_PERIOD, 0, &glob_results);
  #endif

  glob(wildcard, GLOB_PERIOD, 0, &glob_results);

  // Skip files before our current window.
  filecount = 0;
  for (p = glob_results.gl_pathv, cnt = glob_results.gl_pathc;
       cnt; p++, cnt--)
  {
    if ( filecount >= firstfile )
      break;
    if (isDir(*p))
      filecount++;
  }

  /* generate the list.  */
  for ( filecount = 0; cnt; p++, cnt--)
  {
    if (filecount >= MAX_DIR_ENTRIES)
      break;
    if (isDir(*p))
    {
      strcpy(filelist[filecount], *p);
      filecount++;
    }
  }
  globfree(&glob_results);

#endif 

  return filecount;
}

//---------------------------------------------------------------------
// see if its a directory
int isDir(char *filename)
{
  struct stat st;

  if (stat(filename, &st))
    return 0;

  if (S_ISDIR(st.st_mode))
    return 1;

  return 0;
}

//---------------------------------------------------------------------
// search for a file
boolean CheckFile(char *file, char *modus)
{
  FILE    *Input = NULL;
  boolean  found = false;
  char     fileName[NAMELENGTH];
  char     searchFile[NAMELENGTH];
  int      i;

  strcpy(fileName, basename(file));
  i = 0;
  while( (Input == NULL) && (i < dirCount) ) {
    searchFile[0] = '\0'; // String loeschen
    strcat(searchFile, scannedDir[i]);
    strcat(searchFile, fileName);
    strcpy(file, searchFile);
    Input = fopen(file, modus);
    i++;
  }
  if( Input == NULL ) {              // nicht in Unterverzeichnissen
    strcpy(file, fileName);
    Input = fopen(file, modus);
  }
    
  // letztmalig ueberprufen 
  if(Input) {
    fclose(Input);
    found = true;
  }
  return found;
}

