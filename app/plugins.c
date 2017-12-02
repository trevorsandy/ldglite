#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <float.h>

#include "plugins.h"

//*****************************************************************************
//define the plugin callback funtion type
typedef void WINAPI (TLDDPCallBack) (char *strCBCompleteText, char *strCBSelText);

//define the two plugin funtion types
typedef void WINAPI __declspec(dllimport) (CALLBACK *PLUGIN_INFO)(unsigned char CaseID, char *buffer, unsigned char maxlength);

typedef void WINAPI __declspec(dllexport) (CALLBACK *PROCESS_TEXT)(unsigned char *CompleteText, unsigned char *SelText, unsigned long *SelStart, unsigned long *SelLength, unsigned long *CursoRow, unsigned long *CursorColum, TLDDPCallBack myCallback);

static int plugoutnum = 0;
static char plugoutfilename[512];

//*****************************************************************************
void WINAPI plugCallBack (char *strCBCompleteText, char *strCBSelText)
{
  FILE *fp;

  printf("Results:\n%s\n", strCBCompleteText);
  printf("selected:\n%s\n", strCBSelText);
  if ((!strCBCompleteText || !strlen(strCBCompleteText)) && 
      (!strCBSelText || !strlen(strCBSelText)))
  {
    // Got nothing back from the plugin, so skip the temp file.
    strcpy(plugoutfilename, "");
    return;
  }
  fp = fopen(plugoutfilename, "w+");
  if (!fp)
  {
    strcpy(plugoutfilename, "");
    return;
  }

  // Note: I probably should do this line by line and strip out \r chars.
  fprintf(fp, strCBCompleteText);
  fprintf(fp, "\n");
  fprintf(fp, strCBSelText);

  fclose(fp);

  printf("wrote: %s\n", plugoutfilename);
}

//*****************************************************************************
char *plugin(plugstruct *plug,
	   unsigned char *CompleteText,
	   unsigned char *SelText,
	   unsigned long *SelStart,
	   unsigned long *SelLength,
	   unsigned long *CursoRow,
	   unsigned long *CursorColum)
{
  HANDLE   hTME=NULL; //DLL handle
  PROCESS_TEXT  ProcessText;
  char *p;

  //ensure we are where the DLL will be found
  //SetCurrentDirectory(szDLLInThisDirectory);

  //load up the DLL
  hTME=LoadLibrary(plug->dllname);
  if (hTME == NULL)
  {
    printf("LoadLibrary(%s) == NULL\n", plug->dllname);
    return NULL;
  }

  //get the function we want and lock to our function pointer	
  ProcessText = (PROCESS_TEXT) GetProcAddress(hTME,"ProcessText");
    
  //error check
  if(ProcessText == NULL)
  {
    FreeLibrary(hTME); //free library after use
    printf("ProcessText == NULL\n");
    return NULL;
  }

  strcpy(plugoutfilename, plug->dllname + strlen("plugins\\plugin"));
  if ((p = strrchr(plugoutfilename, '.')) != NULL)
    *p = 0;
  sprintf(plugoutfilename + strlen(plugoutfilename), "%d", ++plugoutnum);
  strcat(plugoutfilename, ".ldr");
  
  ProcessText(CompleteText, SelText, SelStart, SelLength, 
	      CursoRow, CursorColum, plugCallBack);

  FreeLibrary(hTME); //free library after use

  _fpreset(); // Borland DLL fix.
  
  if (strlen(plugoutfilename))
    return plugoutfilename;
  else
    return NULL;
}

//*****************************************************************************
plugstruct *pluginfo(char *dllname)
{
  HANDLE   hTME=NULL; //DLL handle
  PLUGIN_INFO  PluginInfo;
  plugstruct *plug;
  char buffer[256];

  //ensure we are where the DLL will be found
  //SetCurrentDirectory(szDLLInThisDirectory);

  //load up the DLL
  hTME=LoadLibrary(dllname);
  if (hTME == NULL)
  {
    printf("LoadLibrary(%s) == NULL\n", dllname);
    return NULL;
  }

  //get the function we want and lock to our function pointer	
  PluginInfo = (PLUGIN_INFO) GetProcAddress(hTME,"Plugin_Info");
  if (PluginInfo == NULL)
  {
    FreeLibrary(hTME); //free library after use
    printf("PluginInfo == NULL\n");
    return NULL;
  }
  plug = (plugstruct *) calloc(sizeof(plugstruct), 1);
  if (!plug)
  {
    FreeLibrary(hTME); //free library after use
    return NULL;
  }

  plug->dllname = strdup(dllname);

  buffer[0] = 0;
  PluginInfo('\0', buffer, 100);
  printf("PluginInfo(0) = %s\n", buffer);
  plug->name = strdup(buffer);
  
  buffer[0] = 0;
  PluginInfo('\1', buffer, 100);
  printf("PluginInfo(1) = %s\n", buffer);
  plug->menuentry = strdup(buffer);
  
  buffer[0] = 0;
  PluginInfo('\2', buffer, 100);
  printf("PluginInfo(2) = %s\n", buffer);
  plug->version = strdup(buffer);
  
  buffer[0] = 0;
  PluginInfo('\3', buffer, 100);
  printf("PluginInfo(3) = %s\n", buffer);
  plug->description = strdup(buffer);
  
  buffer[0] = 0;
  PluginInfo('\4', buffer, 100);
  printf("PluginInfo(4) = %s\n", buffer);
  plug->author = strdup(buffer);
  
  buffer[0] = 0;
  PluginInfo('\5', buffer, 100);
  printf("PluginInfo(5) = %s\n", buffer);
  plug->comment = strdup(buffer);

  buffer[0] = 0;
  PluginInfo('\6', buffer, 100);
  printf("PluginInfo(6) = %s\n", buffer);
  plug->plugtype = strdup(buffer);
  // Plugintype can be one of the following values
  // 0 = Plugin can be called always
  // 1 = Plugin can only be called if no text is selected
  // 2 = Plugin can only be called if a portion of text is selected

  FreeLibrary(hTME); //free library after use

  _fpreset(); // Borland DLL fix.
  
  return plug;
}
 
