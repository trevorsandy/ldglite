#ifndef UNIX
/***************************************************************/
char *dirname( char *filepath )
{
  char *tmpstr;
  int i, n;

  if (filepath == NULL)
    return NULL;

  n = strlen(filepath);
  for (i = n-1; i >= 0; i--)
  {
    if ((filepath[i] == '/') || (filepath[i] == '\\'))
    {
      tmpstr = malloc(i+1);
      strncpy(tmpstr, filepath, i);
      tmpstr[i] = 0;
      return tmpstr;
    }
  }
  tmpstr = malloc(2);
  strcpy(tmpstr, ".");
  return tmpstr;
}

/***************************************************************/
char *basename( char *filepath )
{
  int i, n;

  if (filepath == NULL)
    return NULL;

  n = strlen(filepath);
  for (i = n-1; i >= 0; i--)
  {
    if ((filepath[i] == '/') || (filepath[i] == '\\'))
    {
      return &filepath[i+1];
    }
  }
  return filepath;
}

#endif

