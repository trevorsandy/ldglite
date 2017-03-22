 /*
 *  LDGLITE, a program for viewing *.dat files.
 *  Copyright (C) 2002  D. Heyse
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

/************************************************************************
 * ldglmenu.c - isolates the glut popup menus for toolkit independence.
 ************************************************************************/

#include "glwinkit.h"
#include "platform.h"
#include "ldliteVR.h"
#include "dirscan.h"

#ifndef TEST_MUI_GUI
#  define USE_GLUT_MENUS 1
#endif

/***************************************************************/
int  filemenunum;
int  dirmenunum;
int  mainmenunum;
int  fovmenunum;
int  spinmenunum;
int  studsmenunum;
int  DateiCount    = 0;
int  FolderCount    = 0;
char DateiListe[MAX_DIR_ENTRIES][NAMELENGTH];
char FolderList[MAX_DIR_ENTRIES][NAMELENGTH];
int  minfilenum    = 0;
char dirpattern[256] = "*";
char filepattern[256] = "*.ldr";
char dirfilepath[256];
char fovstr[32];

void filemenu(int);

/***************************************************************/
extern char ldgliteVersion[];

extern char progname[256];
extern int EPS_OUTPUT_FIGURED_OUT;

extern void colormenu(int c);

extern double projection_fov;
extern int ldraw_projection_type;  // 1 = perspective, 0 = orthographic.
extern int pan_visible;
extern int qualityLines;
extern int dirtyWindow;

/***************************************************************/
void fovmenu(int c)
{
  switch (c)
  {
  case 1:
    projection_fov += 1;
    break;
  case 2:
    projection_fov -= 1;
    break;
  case 3:
    projection_fov += 10;
    break;
  case 4:
    projection_fov -= 10;
    break;
  case 5:
    projection_fov = 30;
    break;
  case 6:
    projection_fov = 45;
    break;
  case 7:
    projection_fov = 60;
    break;
  case 8:
    projection_fov = 90;
    break;
  case 9:
    projection_fov = 67.38;
    break;
  }
  
  if (projection_fov < 0.0)
    projection_fov = 1.0;

  glutSetMenu(fovmenunum); // Reset the current menu to the main menu.
  sprintf(fovstr, "FOV = %0.2f    ", projection_fov);
  glutChangeToMenuEntry(1, fovstr, 0);
  glutSetMenu(mainmenunum); // Reset the current menu to the main menu.

  // Gotta reshape().  Send fake mode setting to accomplish it.
  if (ldraw_projection_type)  // 1 = perspective, 0 = ortho
    menuKeyEvent('J', 0, 0);
  else
    menuKeyEvent('j', 0, 0);
}

/***************************************************************/
void studsmenu(int c)
{
  switch (c)
  {
  case 1:
    // Normal studs
    ldraw_commandline_opts.F &= ~(TYPE_F_STUDLINE_MODE); 
    ldraw_commandline_opts.F &= ~(TYPE_F_STUDLESS_MODE); 
    break;
  case 2:
    // Line studs
    ldraw_commandline_opts.F |= TYPE_F_STUDLINE_MODE;
    ldraw_commandline_opts.F &= ~(TYPE_F_STUDLESS_MODE); 
    break;
  case 3:
    // No studs
    ldraw_commandline_opts.F &= ~(TYPE_F_STUDLINE_MODE); 
    ldraw_commandline_opts.F |= TYPE_F_STUDLESS_MODE;
    break;
  default:
    return;
  }

  dirtyWindow = 1;
  glutPostRedisplay();
}

/***************************************************************/
void spinmenu(int c)
{
  switch (c)
  {
  case 1:
    pan_visible = (TYPE_F_BBOX_MODE | TYPE_F_NO_POLYGONS);
    break;
  case 2:
    pan_visible = (TYPE_F_NO_POLYGONS | TYPE_F_STUDLESS_MODE);
    break;
  case 3:
    pan_visible = (TYPE_F_INVISIBLE);
    break;
  case 4:
    pan_visible = (TYPE_F_BBOX_MODE | TYPE_F_SHADED_MODE);
    break;
  case 5:
    pan_visible = (TYPE_F_STUDLESS_MODE | TYPE_F_SHADED_MODE);
    break;
  case 6:
    pan_visible = (TYPE_F_SHADED_MODE);
  }
}

/***************************************************************/
void menu(int item)
{
#ifdef TEST_MUI_GUI
  if  (item == 1)
  {
    mui_menu(0);
    return;
  }
  else if  (item == 4)
  {
    mui_menu(1);
    return;
  }
  else if  (item == 5)
  {
    mui_menu(2);
    return;
  }
#endif
  if  (item == 1)
  {
    glutSetMenu(mainmenunum); // Reset the current menu to the main menu.
    if (strcmp(filepattern, "*"))
    {
      strcpy(filepattern, "*");
      glutChangeToMenuEntry(3, "Filter - *.ldr     ", 1);
    }
    else
    {
      strcpy(filepattern, "*.ldr");
      glutChangeToMenuEntry(3, "Filter - All Files ", 1);
    }
    filemenu(15); // refresh the file list with files in new dir.
  }
  else
    menuKeyEvent(item, 0, 0);
}

/***************************************************************/
void dirmenu(int item)
{
  int         i, j, len1;
  const int   len3 = WORDLENGTH;
  char        myDir[WORDLENGTH+1];
  char filename[256];

  extern char modelspath[];

  UNUSED(i);
  UNUSED(len1);
  UNUSED(len3);
  UNUSED(filename);

  if (item == 15) // Real Refresh
    minfilenum = 0;
  if (item == 0) // Nothing (seperator line)
    return;
  if (item == 13) // PgUp
  {
    minfilenum -= MAX_DIR_ENTRIES;
    if (minfilenum < 0)
      minfilenum = 0;
    item = 15;
  }
  if (item == 14) // PgDn
  {
    minfilenum += MAX_DIR_ENTRIES;
    item = 15;
  }
  if (item == 15) // Refresh
  {
    strcpy(myDir, dirfilepath);
    FolderCount = ScanFolder(myDir, dirpattern, minfilenum, FolderList);
    if ((FolderCount == 0) && (minfilenum > 0))
    { // Dont PgDn past the last file in the directory.
      if (ldraw_commandline_opts.debug_level == 1)
	printf("Rescanning from file %d", minfilenum);
      minfilenum -= MAX_DIR_ENTRIES; 
      if (minfilenum < 0)
	minfilenum = 0;
      if (ldraw_commandline_opts.debug_level == 1)
	printf("to %d\n", minfilenum);
      FolderCount = ScanFolder(myDir, dirpattern, minfilenum, FolderList);
    }
    if (ldraw_commandline_opts.debug_level == 1)
      printf ("Found %d folders starting at %d in %s\n", FolderCount,minfilenum, myDir);

    glutSetMenu(dirmenunum);
    for(j = 1; j <= MAX_DIR_ENTRIES; j++) 
      glutChangeToMenuEntry(j, basename(FolderList[j-1]), j);
    glutSetMenu(mainmenunum); // Reset the current menu to the main menu.
  }
  else 
  {
    if (ldraw_commandline_opts.debug_level == 1)
      printf("selected dir %d = %s\n", item, FolderList[item-1]);
    if (item <= FolderCount)
    {
      strcpy(dirfilepath, FolderList[item-1]);
      strcpy(myDir, basename(dirfilepath));
      if (stricmp(myDir, ".") == 0)
	strcpy(dirfilepath, dirname(FolderList[item-1]));
      if (stricmp(myDir, "..") == 0)
      {
	// Fetch the absolute path to get to ".\.."
	if (stricmp(dirname(FolderList[item-1]), ".") == 0)
	{
	  getcwd(myDir, WORDLENGTH);
	  strcpy(dirfilepath, dirname(myDir));
	}
	else
	  strcpy(dirfilepath, dirname(dirname(FolderList[item-1])));
	if (ldraw_commandline_opts.debug_level == 1)
	  printf("Now using dir = %s\n", dirfilepath);
      }
      dirmenu(15); // refresh the folder list with folders in new dir.
      filemenu(15); // refresh the file list with files in new dir.
    }
  }
}

/***************************************************************/
void filemenu(int item)
{
  int         i, j, len1;
  const int   len3 = WORDLENGTH;
  char        myDir[WORDLENGTH+1];
  char filename[256];

  extern char modelspath[];

  UNUSED(i);
  UNUSED(len1);
  UNUSED(len3);
  UNUSED(filename);

  if (item == 15) // Real Refresh
    minfilenum = 0;
  if (item == 0) // Nothing (seperator line)
    return;
  if (item == 13) // PgUp
  {
    minfilenum -= MAX_DIR_ENTRIES;
    if (minfilenum < 0)
      minfilenum = 0;
    item = 15;
  }
  if (item == 14) // PgDn
  {
    minfilenum += MAX_DIR_ENTRIES;
    item = 15;
  }
  if (item == 15) // Refresh
  {
    strcpy(myDir, dirfilepath);
    DateiCount = ScanDirectory(myDir, filepattern, minfilenum, DateiListe);
    if ((DateiCount == 0) && (minfilenum > 0))
    { // Dont PgDn past the last file in the directory.
      if (ldraw_commandline_opts.debug_level == 1)
	printf("Rescanning from file %d", minfilenum);
      minfilenum -= MAX_DIR_ENTRIES; 
      if (minfilenum < 0)
	minfilenum = 0;
      if (ldraw_commandline_opts.debug_level == 1)
	printf("to %d\n", minfilenum);
      DateiCount = ScanDirectory(myDir, filepattern, minfilenum, DateiListe);
    }
    if (ldraw_commandline_opts.debug_level == 1)
      printf ("Found %d files starting at %d in %s\n", DateiCount,minfilenum, myDir);

    glutSetMenu(filemenunum);
    for(j = 1; j <= MAX_DIR_ENTRIES; j++) 
      glutChangeToMenuEntry(j, basename(DateiListe[j-1]), j);
    glutSetMenu(mainmenunum); // Reset the current menu to the main menu.
  }
  else 
  {
    if (ldraw_commandline_opts.debug_level == 1)
      printf("selected file %d = %s\n", item, DateiListe[item-1]);
    if (item <= DateiCount)
      loadnewdatfile(dirname(DateiListe[item-1]),basename(DateiListe[item-1]));
  }
}

/***************************************************************/
void initializeMenus(void)
{
#ifdef USE_GLUT_MENUS
  int opts, view, colors, helpmenunum;

#ifndef AGL
#if (GLUT_XLIB_IMPLEMENTATION >= 13)
// Rats, no menus in game mode.  Perhaps GLUI or PUI look good again.
  if (ldraw_commandline_opts.V_x >= -1)
#endif
#endif
  {
  fovmenunum = glutCreateMenu(fovmenu);
  sprintf(fovstr, "FOV = %0.2f    ", projection_fov);
  glutAddMenuEntry(fovstr, 0);
  glutAddMenuEntry("                   ", 0);
  glutAddMenuEntry("+1                 ", 1);
  glutAddMenuEntry("-1                 ", 2);
  glutAddMenuEntry("+10                ", 3);
  glutAddMenuEntry("-10                ", 4);
  glutAddMenuEntry("30                 ", 5);
  glutAddMenuEntry("45                 ", 6);
  glutAddMenuEntry("60                 ", 7);
  glutAddMenuEntry("90                 ", 8);
  glutAddMenuEntry("L3P default (67.38)", 9);

  view = glutCreateMenu(menu);
  glutAddMenuEntry("Ldraw Oblique   ", '0');
  glutAddMenuEntry("Back            ", '1');
  glutAddMenuEntry("Left            ", '2');
  glutAddMenuEntry("Right           ", '3');
  glutAddMenuEntry("Above           ", '4');
  glutAddMenuEntry("Beneath         ", '5');
  glutAddMenuEntry("Oblique         ", '6');
  glutAddMenuEntry("Front           ", '7');
  glutAddMenuEntry("UpsideDown      ", '8');
  glutAddMenuEntry("Natural         ", '9');
  glutAddMenuEntry("                ", '\0');
  glutAddMenuEntry("OrthoGraphic    ", 'j');
  glutAddMenuEntry("Perspective     ", 'J');
  glutAddSubMenu(  "FOV Angle       ", fovmenunum);

#if 0
  //**************************************************************************
  // Consider mirroring the checkboxes of the MUI drawing mode dialog.
  // Unfortunately Windows and OSX use a Proportional font for glut menus.
  // This makes the [ * ] check boxes look bigger than [   ] boxes.
  // What about [ - ] and [ + ]?  Would they be the same size in any font?
  // No, but [ - ] and [   ] are almost the same with Windows large fonts.
  // I've also seen an arrow approach "Shaded --> Flat" or "Shading: On -> Off"
  // and an on/off approach "Shading   On " or "Shading  Off".

  drawstyle = glutCreateMenu(stylesmenu);
  glutAddMenuEntry("[ * ] Shading  ", 1);
  glutAddMenuEntry("[ * ] Surfaces ", 2);
  glutAddMenuEntry("[ * ] Edges    ", 3);
  glutAddMenuEntry("[   ] AntiAlias", 4);

  studs = glutCreateMenu(studsmenu);
  glutAddMenuEntry("[ * ] Normal", 1);
  glutAddMenuEntry("[   ] Lines ", 2);
  glutAddMenuEntry("[   ] None  ", 3);

  spin = glutCreateMenu(spinmenu);
  glutAddMenuEntry("[   ] Wire Boxes ", 1);
  glutAddMenuEntry("[   ] Wireframe  ", 2);
  glutAddMenuEntry("[   ] Dragline   ", 3);
  glutAddMenuEntry("[ * ] Solid Boxes", 4);
  glutAddMenuEntry("[   ] Studless   ", 5);
  glutAddMenuEntry("[   ] Everything ", 6);
  // Zoom should really be a submenu off of the view menu.  Oh Well.

  // Hey, this might work with a tab before the On or Off
  // On Windows Glut, tabs ('\t') jump to the accelerator key field.
  // I can use this to put a check char (* or + or nothing) 
  // followed by a tab and text.
  // I seem to be allowed one tab on Windows (for accelerator key?)
  // I can use this to line up 2 columns in a menu.
  // Either check marks (on the left) or On/Off on the right)
  // Or I can use it to list the accelerator keys.
  // NOTE: on Windows an & will be hidden but will activate a hotkey
  // that only works when that menu is displayed.  Weird!
  // 
  // NOTE:  Unfortuntely on Linux the tab renders as a hollow box

  glutAddMenuEntry("OrthoGraphic \tOn ", 'j');
  glutAddMenuEntry("Perspective  \tOff", 'J');

  glutAddMenuEntry("[ - ]   Wire Boxes ", 0);
  glutAddMenuEntry("[ - ]   Wireframe  ", 0);
  glutAddMenuEntry("[ - ]   Dragline   ", 0);
  glutAddMenuEntry("[   ]   Solid Boxes", 0);
  glutAddMenuEntry("[ - ]   Studless   ", 0);
  glutAddMenuEntry("[ - ]   Everything ", 0);
  // Underscores render as a solid line in most fonts, but they are WIDE.
  glutAddMenuEntry("___________________", 0); 
  glutAddMenuEntry("                   ", 0);

  glutAddMenuEntry("*\t Wire Boxes ", 0);
  glutAddMenuEntry("*\t Wireframe  ", 0);
  glutAddMenuEntry("*\t Dragline   ", 0);
  glutAddMenuEntry(" \t Solid Boxes", 0);
  glutAddMenuEntry("*\t Studless   ", 0);
  glutAddMenuEntry("*\t Everything ", 0);
  glutAddMenuEntry("", 0);
  glutAddMenuEntry(" \t Enlarge    ", 3);
  glutAddMenuEntry(" \t Reduce     ", 2);
  glutAddMenuEntry(" \t Zoom in    ", '+');
  glutAddMenuEntry(" \t Zoom out   ", '-');
  //**************************************************************************
#endif

  studsmenunum = glutCreateMenu(studsmenu);
  glutAddMenuEntry("Normal", 1);
  glutAddMenuEntry("Lines ", 2);
  glutAddMenuEntry("None  ", 3);

  spinmenunum = glutCreateMenu(spinmenu);
  glutAddMenuEntry("Wire Boxes ", 1);
  glutAddMenuEntry("Wireframe  ", 2);
  glutAddMenuEntry("Dragline   ", 3);
  glutAddMenuEntry("Solid Boxes", 4);
  glutAddMenuEntry("Studless   ", 5);
  glutAddMenuEntry("Everything ", 6);

  opts = glutCreateMenu(menu);
  glutAddMenuEntry("Shading         ", 'h');
  glutAddMenuEntry("Linemode        ", 'l');
  glutAddMenuEntry("Normal          ", 'n');
  glutAddMenuEntry("Quality Lines   ", 'q');
#if 0
  glutAddMenuEntry("Studs           ", 'f');
  glutAddMenuEntry("Spin Mode       ", 'V');
#else
  glutAddSubMenu(  "Studs           ", studsmenunum);
  glutAddSubMenu(  "Spin Mode       ", spinmenunum);
#endif
  glutAddMenuEntry("                ", '\0');
  glutAddMenuEntry("Step-Continuous ", 's');
  glutAddMenuEntry("Polling         ", 'g');
#ifdef USE_L3_PARSER
  glutAddMenuEntry("Parser          ", 'r');
#endif
  glutAddMenuEntry("Editing         ", 6);
  glutAddMenuEntry("                ", '\0');
  glutAddMenuEntry("Enlarge         ", 3);
  glutAddMenuEntry("Reduce          ", 2);
  glutAddMenuEntry("Zoom in         ", '+');
  glutAddMenuEntry("Zoom out        ", '-');

  colors = glutCreateMenu(colormenu);
  glutAddMenuEntry("Black              ", 0);
  glutAddMenuEntry("Blue               ", 1);
  glutAddMenuEntry("Green              ", 2);
  glutAddMenuEntry("Dk Cyan            ", 3);
  glutAddMenuEntry("Red                ", 4);
  glutAddMenuEntry("Purple             ", 5);
  glutAddMenuEntry("Brown              ", 6);
  glutAddMenuEntry("Lt Gray            ", 7);
  glutAddMenuEntry("Dk Gray            ", 8);
  glutAddMenuEntry("Lt Blue            ", 9);
  glutAddMenuEntry("Lt Green           ", 10);
  glutAddMenuEntry("Lt Cyan            ", 11);
  glutAddMenuEntry("Lt Red             ", 12);
  glutAddMenuEntry("Pink               ", 13);
  glutAddMenuEntry("Yellow             ", 14);
  glutAddMenuEntry("White              ", 15);

  dirmenunum = glutCreateMenu(dirmenu);
  glutAddMenuEntry("                   ", 1);
  glutAddMenuEntry("                   ", 2);
  glutAddMenuEntry("                   ", 3);
  glutAddMenuEntry("                   ", 4);
  glutAddMenuEntry("                   ", 5);
  glutAddMenuEntry("                   ", 6);
  glutAddMenuEntry("                   ", 7);
  glutAddMenuEntry("                   ", 8);
  glutAddMenuEntry("                   ", 9);
  glutAddMenuEntry("                   ", 10);
  glutAddMenuEntry("                   ", 0);
  glutAddMenuEntry("Page Up            ", 13);
  glutAddMenuEntry("Page Dn            ", 14);

  filemenunum = glutCreateMenu(filemenu);
  glutAddMenuEntry("                   ", 1);
  glutAddMenuEntry("                   ", 2);
  glutAddMenuEntry("                   ", 3);
  glutAddMenuEntry("                   ", 4);
  glutAddMenuEntry("                   ", 5);
  glutAddMenuEntry("                   ", 6);
  glutAddMenuEntry("                   ", 7);
  glutAddMenuEntry("                   ", 8);
  glutAddMenuEntry("                   ", 9);
  glutAddMenuEntry("                   ", 10);
  glutAddMenuEntry("                   ", 0);
  glutAddMenuEntry("Page Up            ", 13);
  glutAddMenuEntry("Page Dn            ", 14);

  helpmenunum = glutCreateMenu(menu);
  glutAddMenuEntry(progname             , '\0');
  glutAddMenuEntry(ldgliteVersion, '\0');

  mainmenunum = glutCreateMenu(menu);
#ifndef TEST_MUI_GUI
  glutAddSubMenu(  "File               ", filemenunum);
  glutAddSubMenu(  "Folder             ", dirmenunum);
  glutAddMenuEntry("Filter - All Files ", 1);
#else
  glutAddMenuEntry("Open               ", 4);
  glutAddMenuEntry("Save               ", 5);
  glutAddMenuEntry("Settings           ", 1);
#endif
  glutAddMenuEntry("                   ", '\0');
  glutAddSubMenu(  "View               ", view);
  glutAddSubMenu(  "Options            ", opts);
  glutAddSubMenu(  "BackGround Color   ", colors);
  glutAddMenuEntry("                   ", '\0');
  glutAddMenuEntry("Bitmap             ", 'B');
  if (EPS_OUTPUT_FIGURED_OUT)
  {
    glutAddMenuEntry("EPS file (sorted)  ", 14); // C-N
    glutAddMenuEntry("EPS file (UNsorted)", 15); // C-O
    glutAddMenuEntry("EPS debug          ", 16); // C-P
  }
  glutAddMenuEntry("                   ", '\0');
  glutAddSubMenu(  "Help               ", helpmenunum);
  glutAddMenuEntry("Quit               ", UI_ESCAPE_EVENT);
#ifdef USE_GLUT_MENUS
  glutAttachMenu(GLUT_RIGHT_BUTTON);
#endif
  view = glutCreateMenu(menu);
  glutAddMenuEntry("Ldraw Oblique   ", '0');

  // Read in the current directories dat filenames.
  dirmenu(15);
  filemenu(15);
  }
#endif /* TEST_MUI_GUI */

}
