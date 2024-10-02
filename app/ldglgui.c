/*
 * Copyright (c) 1993-1997, Silicon Graphics, Inc.
 * ALL RIGHTS RESERVED
 * Permission to use, copy, modify, and distribute this software for
 * any purpose and without fee is hereby granted, provided that the above
 * copyright notice appear in all copies and that both the copyright notice
 * and this permission notice appear in supporting documentation, and that
 * the name of Silicon Graphics, Inc. not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission.
 *
 * THE MATERIAL EMBODIED ON THIS SOFTWARE IS PROVIDED TO YOU "AS-IS"
 * AND WITHOUT WARRANTY OF ANY KIND, EXPRESS, IMPLIED OR OTHERWISE,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY OR
 * FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL SILICON
 * GRAPHICS, INC.  BE LIABLE TO YOU OR ANYONE ELSE FOR ANY DIRECT,
 * SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY
 * KIND, OR ANY DAMAGES WHATSOEVER, INCLUDING WITHOUT LIMITATION,
 * LOSS OF PROFIT, LOSS OF USE, SAVINGS OR REVENUE, OR THE CLAIMS OF
 * THIRD PARTIES, WHETHER OR NOT SILICON GRAPHICS, INC.  HAS BEEN
 * ADVISED OF THE POSSIBILITY OF SUCH LOSS, HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE
 * POSSESSION, USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * US Government Users Restricted Rights
 * Use, duplication, or disclosure by the Government is subject to
 * restrictions set forth in FAR 52.227.19(c)(2) or subparagraph
 * (c)(1)(ii) of the Rights in Technical Data and Computer Software
 * clause at DFARS 252.227-7013 and/or in similar or successor
 * clauses in the FAR or the DOD or NASA FAR Supplement.
 * Unpublished-- rights reserved under the copyright laws of the
 * United States.  Contractor/manufacturer is Silicon Graphics,
 * Inc., 2011 N.  Shoreline Blvd., Mountain View, CA 94039-7311.
 *
 * OpenGL(R) is a registered trademark of Silicon Graphics, Inc.
 */

/***************************************************************/
#ifdef  TEST_MUI_GUI

#include <stdio.h>
#include "glwinkit.h"

#include "platform.h"
#include "ldliteVR.h"
#ifndef WINDOWS
// This stuff gets pulled in by glut.h for windows.
#include "wstubs.h"
#else
// glut 3.7 no longer includes windows.h
#if (GLUT_XLIB_IMPLEMENTATION >= 13)
#include <windows.h>
#endif
#endif

#include <mui/mui.h>

extern char ldgliteVersion[];

extern GLint Width;
extern GLint Height;
extern int  filemenunum;
extern int  dirmenunum;
extern int  mainmenunum;

#ifdef USE_L3_PARSER
extern int parsername;
#endif

extern int ldraw_projection_type;  // 1 = perspective, 0 = orthographic.
extern int pan_visible;
extern int qualityLines;
extern double projection_fov;

extern char Back[];
extern char Left[];
extern char Right[];
extern char Above[];
extern char Beneath[];
extern char LdrawOblique[];
extern char Oblique[];
extern char Front[];
extern char UpsideDown[];
extern char Natural[];
extern char *m_viewMatrix;

extern int mui_singlebuffered;

muiObject *b1, *b2, *b3, *b4, *b5, *b6, *b7, *b8;
muiObject *b9, *b10, *b11, *b12, *b13, *b14, *b15, *b16;
muiObject *l1, *l2, *l3, *l4, *l5, *l6, *l7, *l8;
muiObject *t, *t1, *t2, *tl;
muiObject *pd, *hs, *vs;

int M1, M2, M3; /* menus */

//#define THUMBHEIGHT 20
#define THUMBHEIGHT 10
#define ARROWSPACE 40

int dw = 320;
int dh = 200;
int ow = 0;
int oh = 0;

#define MAIN_UILIST 1
#define FILE_UILIST 2
#define VIEW_UILIST 3
#define DRAW_UILIST 4
#define OPTS_UILIST 5
#define BACK_UILIST 6
#define HELP_UILIST 7

int fileOpenSave = 0;

int viewchoice = 0;
int projchoice = 0;
int panchoice = 0;

void makefileui(char *);
void makeviewui(void);
void makemainui(void);
void makedrawui(void);
void makeoptsui(void);
void makebackui(void);

/***************************************************************/
void muiHide(muiObject *obj, int state)
{
  muiSetVisible(obj, state);
  //muiSetEnable(obj, state);
  //muiSetActive(obj, state);
}

/***************************************************************/
#if 0
#include <mui/gizmo.h>
#else
typedef struct muicons {
    struct muicons  *next;
    muiObject       *object;
} muiCons;

void        muiBackgroundClear(void);
muiCons     *muiGetListCons(int uilist);
#endif

/***************************************************************/
void muiHideAll(int uilist, int state)
{
    muiCons *mcons;

    if ((mcons = muiGetListCons(uilist)) == (muiCons *)0) return;
    muiBackgroundClear();
    while (mcons) {
      muiSetVisible(mcons->object, state);
      //muiSetEnable(mcons->object, state);
      //muiSetActive(mcons->object, state);
      mcons = mcons->next;
    }
}

/***************************************************************/
void muiMoveAll(int uilist, int x, int y)
{
    muiCons *mcons;
    muiObject *obj;

    if ((mcons = muiGetListCons(uilist)) == (muiCons *)0) return;
    //muiBackgroundClear();
    while (mcons) {
      obj = mcons->object;
      obj->xmin += x;
      obj->ymin += y;
      obj->xmax += x;
      obj->ymax += y;
      mcons = mcons->next;
    }
}

/***************************************************************/
void mui_cleanup(void)
{
  //printf("MUI OK callback\n");
  // Gotta clean up mui and go back to whatever.
  // Must free up mui menu stuff.
  glEnable( GL_DEPTH_TEST );
  glScissor(0, 0, Width, Height); // x,y,width,height
  glDisable(GL_SCISSOR_TEST);
  registerGlutCallbacks();
  init();
#if 0
  dirtyWindow = 1;  // Redraw
#else
  reshape(Width, Height); // Gotta reshape since MUI changes the matrices.
#endif
  glutPostRedisplay();
}

/***************************************************************/
void bcallback(muiObject *obj, enum muiReturnValue r)
{
  int i = 0;

  muiHide(obj, 0);

  i = muiGetID(obj);

  switch (i)
  {
  case 1:
    muiHideAll(muiGetUIList(obj), 0);
    makefileui("Open:");
    break;
  case 2:
    muiHideAll(muiGetUIList(obj), 0);
    makefileui("Save:");
    break;
  case 3:
    muiHideAll(muiGetUIList(obj), 0);
    makeviewui();
    break;
  case 4:
    muiHideAll(muiGetUIList(obj), 0);
    makedrawui();
    break;
  case 5:
    muiHideAll(muiGetUIList(obj), 0);
    makeoptsui();
    break;
  case 6:
    muiHideAll(muiGetUIList(obj), 0);
    makebackui();
    break;
  case 7:
    muiSetActiveUIList(MAIN_UILIST);
    muiHideAll(muiGetUIList(obj), 1);
    break;
  case 8:
    muiSetActiveUIList(MAIN_UILIST);
    mui_cleanup();
    exit(0);
    break;
  case 13:
  case 14:
    muiSetActiveUIList(MAIN_UILIST);
    mui_cleanup();
    break;
  default:
    muiHideAll(muiGetUIList(obj), 0);
    makemainui();
  }

  //printf("Button callback %d, %d\n", i, r);
}

#if 0
/***************************************************************/
void muiPrintObject(muiCons *mcons)
{
  muiObject *obj = mcons->object;

  switch (mcons->object->type) {
  case MUI_BUTTON:
    printf("MUI_BUTTON(%p): %d,%d  %d,%d\n", obj, obj->xmin, obj->ymin, obj->xmax, obj->ymax);
    break;
  case MUI_TEXTBOX:
    printf("MUI_TEXTBOX(%p): %d,%d  %d,%d\n", obj, obj->xmin, obj->ymin, obj->xmax, obj->ymax);
    break;
  case MUI_VSLIDER:
    printf("MUI_VSLIDER(%p): %d,%d  %d,%d\n", obj, obj->xmin, obj->ymin, obj->xmax, obj->ymax);
    break;
  case MUI_HSLIDER:
    printf("MUI_HSLIDER(%p): %d,%d  %d,%d\n", obj, obj->xmin, obj->ymin, obj->xmax, obj->ymax);
    break;
  case MUI_TEXTLIST:
    printf("MUI_TEXTLIST(%p): %d,%d  %d,%d\n", obj, obj->xmin, obj->ymin, obj->xmax, obj->ymax);
    break;
  case MUI_RADIOBUTTON:
    printf("MUI_RADIOBUTTON(%p): %d,%d  %d,%d\n", obj, obj->xmin, obj->ymin, obj->xmax, obj->ymax);
    break;
  case MUI_TINYRADIOBUTTON:
    printf("MUI_TINYRADIOBUTTON(%p): %d,%d  %d,%d\n", obj, obj->xmin, obj->ymin, obj->xmax, obj->ymax);
    break;
  case MUI_PULLDOWN:
    printf("MUI_PULLDOWN(%p): %d,%d  %d,%d\n", obj, obj->xmin, obj->ymin, obj->xmax, obj->ymax);
    break;
  case MUI_LABEL:
    printf("MUI_LABEL(%p): %d,%d  %d,%d\n", obj, obj->xmin, obj->ymin, obj->xmax, obj->ymax);
    break;
  case MUI_BOLDLABEL:
    printf("MUI_BOLDLABEL(%p): %d,%d  %d,%d\n", obj, obj->xmin, obj->ymin, obj->xmax, obj->ymax);
    break;
  default:
    printf("MUI_UNKNOWN(%p): %d,%d  %d,%d\n", obj, obj->xmin, obj->ymin, obj->xmax, obj->ymax);
    break;
  }
}
#endif

/***************************************************************/
static void nonmuicallback(int x, int y)
{
  int i;
  muiCons *mcons;

  extern int muiInObject(muiObject *obj, int x, int y);

  //printf("nonMUI callback %d, %d\n", x, y);

#if 0
  //ActiveCons = muiGetListCons(ActiveUIList);
  //obj = muiFastHitInList(ActiveCons, x, y);
  i = muiGetActiveUIList();
  for (mcons = muiGetListCons(i); mcons; mcons = mcons->next)
  {
    muiPrintObject(mcons);
    if (muiInObject(mcons->object, x, y))
      break;
  }

  if (mcons)
    printf("Should have found  %p\n",obj);
  else
    printf("Found nothing\n");
#endif

}

/***************************************************************/
void makemainui(void)
{
  static muiObject *l1, *l2;
  int xmin, ymin, xmax, ymax;

  static int MUIstarted = 0;

  if (MUIstarted == 0)
  {
    muiNewUIList(MAIN_UILIST);  /* makes a MUI display list (number 1) */
    muiNewUIList(FILE_UILIST);
    muiNewUIList(VIEW_UILIST);
    muiNewUIList(DRAW_UILIST);
    muiNewUIList(OPTS_UILIST);
    muiNewUIList(BACK_UILIST);
    muiNewUIList(HELP_UILIST);
    muiSetActiveUIList(MAIN_UILIST);

    b1 = muiNewButton(ow+0, ow+80, oh+dh-25, oh+dh);
    muiLoadButton(b1, "Open");
    muiSetCallback(b1, bcallback);

    b2 = muiNewButton(ow+0, ow+80, oh+dh-50, oh+dh-26);
    muiLoadButton(b2, "Save");
    muiSetCallback(b2, bcallback);

    b3 = muiNewButton(ow+0, ow+80, oh+dh-75, oh+dh-51);
    muiLoadButton(b3, "View");
    muiSetCallback(b3, bcallback);

    b4 = muiNewButton(ow+0, ow+80, oh+dh-100, oh+dh-76);
    muiLoadButton(b4, "Drawing");
    muiSetCallback(b4, bcallback);

    b5 = muiNewButton(ow+0, ow+80, oh+dh-125, oh+dh-101);
    muiLoadButton(b5, "Options");
    muiSetCallback(b5, bcallback);

    b6 = muiNewButton(ow+0, ow+80, oh+dh-150, oh+dh-126);
    muiLoadButton(b6, "Background");
    muiSetCallback(b6, bcallback);

    b7 = muiNewButton(ow+0, ow+80, oh+dh-175, oh+dh-151);
    muiLoadButton(b7, "Help");
    muiSetCallback(b7, bcallback);

    b8 = muiNewButton(ow+0, ow+80, oh+dh-200, oh+dh-176);
    muiLoadButton(b8, "Quit");
    muiSetCallback(b8, bcallback);

    muiSetID(b1, 1);
    muiSetID(b2, 2);
    muiSetID(b3, 3);
    muiSetID(b4, 4);
    muiSetID(b5, 5);
    muiSetID(b6, 6);
    muiSetID(b7, 7);
    muiSetID(b8, 8);

    //l1 = muiNewBoldLabel(ow+60+dw/2-35, oh+dh/2+18, "LdGLite");
    l1 = muiNewLabel(ow+60+dw/2-35, oh+dh/2+18, "LdGLite");
    l2 = muiNewLabel(ow+60+dw/2-55, oh+dh/2-18, ldgliteVersion);

    b13 = muiNewButton(ow+dw-60, ow+dw-34, oh+2, oh+27);
    muiLoadButton(b13, "Ok");
    muiSetCallback(b13, bcallback);
    muiSetID(b13, 13);

    b14 = muiNewButton(ow+dw-30, ow+dw-4, oh+2, oh+27);
    muiLoadButton(b14, "X");
    muiSetCallback(b14, bcallback);
    muiSetID(b14, 14);

    muiSetNonMUIcallback(nonmuicallback);
  }
  else
  {
    muiSetActiveUIList(MAIN_UILIST);
    // Use l4 since its global and exclusive to file menu.
    muiHideAll(muiGetUIList(l1), 1);

    muiAttachUIList(MAIN_UILIST);
  }
  MUIstarted = 1;

  //muiAttachUIList(MAIN_UILIST);
}

/***************************************************************/
/***************************************************************/
void readbutton(muiObject *obj, enum muiReturnValue r)
{
  int i = -1;

  i = muiGetID(obj);

  switch (i)
  {
  case 'J':
    projchoice = 'J';
    break;
  case 'j':
    projchoice = 'j';
    break;
  default:
    viewchoice = i;
  }

  //printf("radio callback %d, %d\n", i, r);
}

/***************************************************************/
void tbcallback(muiObject *obj, enum muiReturnValue value)
{
    char *s;
    double fov = 0.0;

    if (value != MUI_TEXTBOX_RETURN) return;
    s = strdup(muiGetTBString(obj));
    fov = atof(s);

    if (fov <= 0.0)
      fov = projection_fov;
    if (fov >360.0)
      fov = projection_fov;

    printf("FOV = %f\n",fov);
    projection_fov = fov;

    free(s);
    return; //exit(0);
}

/***************************************************************/
void vbcallback(muiObject *obj, enum muiReturnValue r)
{
  int i = 0;

  i = muiGetID(obj);

  muiHide(obj, 0);

  switch (i)
  {
  case 3:
    tbcallback(t2, MUI_TEXTBOX_RETURN);
    muiSetActiveUIList(MAIN_UILIST);
    mui_cleanup();
    if (viewchoice)
      menuKeyEvent(viewchoice, 0, 0);
    if (projchoice)
      menuKeyEvent(projchoice, 0, 0);
    break;
  default:
    muiHideAll(muiGetUIList(obj), 0);
    makemainui();
  }

  //printf("View Button callback %d, %d\n", i, r);
}

/***************************************************************/
void makeviewui(void)
{
  static muiObject *rb1, *rb2, *rb3, *rb4, *rb5, *rb6, *rb7, *rb8;
  static muiObject *rb9, *rb10, *rb11, *rb12, *rb13, *rb14, *rb15, *rb16;
  static muiObject *b3, *b4;
  static muiObject *l1, *l2, *l3, *l4;
  int xmin, ymin, xmax, ymax;

  static int MUIstarted = 0;
  char FOVstr[100];

  if (MUIstarted == 0)
  {
#if 0
    muiNewUIList(VIEW_UILIST);
#else
    muiSetActiveUIList(VIEW_UILIST);
#endif

    rb1 = muiNewTinyRadioButton(ow+2, oh+dh-20);
    rb2 = muiNewTinyRadioButton(ow+2, oh+dh-40);
    rb3 = muiNewTinyRadioButton(ow+2, oh+dh-60);
    rb4 = muiNewTinyRadioButton(ow+2, oh+dh-80);
    rb5 = muiNewTinyRadioButton(ow+2, oh+dh-100);
    rb6 = muiNewTinyRadioButton(ow+2, oh+dh-120);
    rb7 = muiNewTinyRadioButton(ow+2, oh+dh-140);
    rb8 = muiNewTinyRadioButton(ow+2, oh+dh-160);
    rb9 = muiNewTinyRadioButton(ow+2, oh+dh-180);
    rb10 = muiNewTinyRadioButton(ow+2, oh+dh-200);
    //rb11 = muiNewTinyRadioButton(ow+dw/2, oh+dh-20);
    l3 = muiNewBoldLabel(ow+2+dw/2, oh+dh-18, "Projection Type:");
    rb12 = muiNewTinyRadioButton(ow+dw/2, oh+dh-40);
    rb13 = muiNewTinyRadioButton(ow+dw/2, oh+dh-60);
    //rb14 = muiNewTinyRadioButton(ow+dw/2, oh+dh-80);
    //rb15 = muiNewTinyRadioButton(ow+dw/2, oh+dh-100);
    l1 = muiNewLabel(ow+dw/2+2, oh+dh-100, "FOV:");
    t2 = muiNewTextbox(ow+dw/2+40, ow+dw-4, oh+dh-108);
    muiSetActive(t2, 1);
    muiSetCallback(t2, tbcallback);
    sprintf(FOVstr, "%.03f", projection_fov);
    muiSetTBString(t2, FOVstr);
    //rb16 = muiNewTinyRadioButton(ow+dw/2, oh+dh-120);
    //rb17 = muiNewTinyRadioButton(ow+dw/2, oh+dh-140);
    //rb18 = muiNewTinyRadioButton(ow+dw/2, oh+dh-160);
    //rb19 = muiNewTinyRadioButton(ow+dw/2, oh+dh-180);
    //rb20 = muiNewTinyRadioButton(ow+dw/2, oh+dh-200);
    muiLoadButton(rb1, "Ldraw Oblique");
    muiLoadButton(rb2, "Back");
    muiLoadButton(rb3, "Left");
    muiLoadButton(rb4, "Right");
    muiLoadButton(rb5, "Above");
    muiLoadButton(rb6, "Beneath");
    muiLoadButton(rb7, "Oblique");
    muiLoadButton(rb8, "Front");
    muiLoadButton(rb9, "UpsideDown");
    muiLoadButton(rb10, "Natural");
    //muiLoadButton(rb11, "");
    muiLoadButton(rb12, "Orthographic");
    muiLoadButton(rb13, "Perspective");
    //muiLoadButton(rb14, "");
    //muiLoadButton(rb15, "");
    //muiLoadButton(rb16, "");
    muiLinkButtons(rb1, rb2);
    muiLinkButtons(rb2, rb3);
    muiLinkButtons(rb3, rb4);
    muiLinkButtons(rb4, rb5);
    muiLinkButtons(rb5, rb6);
    muiLinkButtons(rb6, rb7);
    muiLinkButtons(rb7, rb8);
    muiLinkButtons(rb8, rb9);
    muiLinkButtons(rb9, rb10);
    //muiLinkButtons(rb10, rb11);
    //muiLinkButtons(rb11, rb12);
    muiLinkButtons(rb12, rb13);
    //muiLinkButtons(rb13, rb14);
    //muiLinkButtons(rb14, rb15);
    //muiLinkButtons(rb15, rb16);
    muiSetCallback(rb1, readbutton);
    muiSetCallback(rb2, readbutton);
    muiSetCallback(rb3, readbutton);
    muiSetCallback(rb4, readbutton);
    muiSetCallback(rb5, readbutton);
    muiSetCallback(rb6, readbutton);
    muiSetCallback(rb7, readbutton);
    muiSetCallback(rb8, readbutton);
    muiSetCallback(rb9, readbutton);
    muiSetCallback(rb10, readbutton);
    //muiSetCallback(rb11, readbutton);
    muiSetCallback(rb12, readbutton);
    muiSetCallback(rb13, readbutton);
    //muiSetCallback(rb14, readbutton);
    //muiSetCallback(rb15, readbutton);
    //muiSetCallback(rb16, readbutton);
    muiSetID(rb1, '0');
    muiSetID(rb2, '1');
    muiSetID(rb3, '2');
    muiSetID(rb4, '3');
    muiSetID(rb5, '4');
    muiSetID(rb6, '5');
    muiSetID(rb7, '6');
    muiSetID(rb8, '7');
    muiSetID(rb9, '8');
    muiSetID(rb10, '9');
    //muiSetID(rb11, 11);
    muiSetID(rb12, 'j');
    muiSetID(rb13, 'J');
    //muiSetID(rb14, 14);
    //muiSetID(rb15, 15);
    //muiSetID(rb16, 16);
    muiClearRadio(rb1);

    b3 = muiNewButton(ow+dw-60, ow+dw-34, oh+2, oh+27);
    muiLoadButton(b3, "Ok");
    muiSetCallback(b3, vbcallback);
    muiSetID(b3, 3);

    b4 = muiNewButton(ow+dw-30, ow+dw-4, oh+2, oh+27);
    muiLoadButton(b4, "X");
    muiSetCallback(b4, vbcallback);
    muiSetID(b4, 4);
  }
  else
  {
    muiSetActiveUIList(VIEW_UILIST);
    muiHideAll(muiGetUIList(b3), 1);
  }

  // Clear the radio buttons and set the correct one active.
  muiClearRadio(rb1);
  if (m_viewMatrix == Back)
    muiSetActive(rb2, 1);
  else if (m_viewMatrix == Left)
    muiSetActive(rb3, 1);
  else if (m_viewMatrix == Right)
    muiSetActive(rb4, 1);
  else if (m_viewMatrix == Above)
    muiSetActive(rb5, 1);
  else if (m_viewMatrix == Beneath)
    muiSetActive(rb6, 1);
  else if (m_viewMatrix == Oblique)
    muiSetActive(rb7, 1);
  else if (m_viewMatrix == Front)
    muiSetActive(rb8, 1);
  else if (m_viewMatrix == UpsideDown)
    muiSetActive(rb9, 1);
  else if (m_viewMatrix == Natural)
    muiSetActive(rb10, 1);
  else
    muiSetActive(rb1, 1);

  muiClearRadio(rb12);
  if (ldraw_projection_type == 1)
    muiSetActive(rb13, 1); // 1 = perspective
  else
    muiSetActive(rb12, 1); // 0 = orthographic

  viewchoice = 0;
  projchoice = 0;

  MUIstarted = 1;

  muiAttachUIList(VIEW_UILIST);
}

/***************************************************************/
void bbcallback(muiObject *obj, enum muiReturnValue r)
{
  int i = 0;

  extern void colormenu(int c);

  i = muiGetID(obj);

  muiHide(obj, 0);

  switch (i)
  {
  case 3:
    muiSetActiveUIList(MAIN_UILIST);
    mui_cleanup();
    if (viewchoice > 11)
      colormenu(viewchoice-4);
    else if (viewchoice)
      colormenu(viewchoice-2);
    break;
  default:
    muiHideAll(muiGetUIList(obj), 0);
    makemainui();
  }

  //printf("Background Button callback %d, %d\n", i, r);
}

/***************************************************************/
void makebackui(void)
{
  static muiObject *rb1, *rb2, *rb3, *rb4, *rb5, *rb6, *rb7, *rb8, *rb9, *rb10;
  static muiObject *rb11, *rb12, *rb13, *rb14, *rb15, *rb16, *rb17, *rb18, *rb19;
  static muiObject *b3, *b4;
  static muiObject *l1, *l2, *l3, *l4;
  int xmin, ymin, xmax, ymax;

  static int MUIstarted = 0;

  if (MUIstarted == 0)
  {
#if 0
    muiNewUIList(BACK_UILIST);
#else
    muiSetActiveUIList(BACK_UILIST);
#endif

    //rb1 = muiNewTinyRadioButton(ow+2, oh+dh-20);
    l1 = muiNewBoldLabel(ow+4, oh+dh-18, "Background Color:");
    rb2 = muiNewTinyRadioButton(ow+2, oh+dh-40);
    rb3 = muiNewTinyRadioButton(ow+2, oh+dh-60);
    rb4 = muiNewTinyRadioButton(ow+2, oh+dh-80);
    rb5 = muiNewTinyRadioButton(ow+2, oh+dh-100);
    rb6 = muiNewTinyRadioButton(ow+2, oh+dh-120);
    rb7 = muiNewTinyRadioButton(ow+2, oh+dh-140);
    rb8 = muiNewTinyRadioButton(ow+2, oh+dh-160);
    rb9 = muiNewTinyRadioButton(ow+2, oh+dh-180);
    //rb10 = muiNewTinyRadioButton(ow+2, oh+dh-200);
    //rb11 = muiNewTinyRadioButton(ow+dw/2, oh+dh-20);
    rb12 = muiNewTinyRadioButton(ow+dw/2, oh+dh-40);
    rb13 = muiNewTinyRadioButton(ow+dw/2, oh+dh-60);
    rb14 = muiNewTinyRadioButton(ow+dw/2, oh+dh-80);
    rb15 = muiNewTinyRadioButton(ow+dw/2, oh+dh-100);
    rb16 = muiNewTinyRadioButton(ow+dw/2, oh+dh-120);
    rb17 = muiNewTinyRadioButton(ow+dw/2, oh+dh-140);
    rb18 = muiNewTinyRadioButton(ow+dw/2, oh+dh-160);
    rb19 = muiNewTinyRadioButton(ow+dw/2, oh+dh-180);
    //muiLoadButton(rb1, "");
    muiLoadButton(rb2, "Black");
    muiLoadButton(rb3, "Blue");
    muiLoadButton(rb4, "Green");
    muiLoadButton(rb5, "Dk Cyan");
    muiLoadButton(rb6, "Red");
    muiLoadButton(rb7, "Purple");
    muiLoadButton(rb8, "Brown");
    muiLoadButton(rb9, "Lt Gray");
    //muiLoadButton(rb10, "");
    //muiLoadButton(rb11, "");
    muiLoadButton(rb12, "Dk Gray");
    muiLoadButton(rb13, "Lt Blue");
    muiLoadButton(rb14, "Lt Green");
    muiLoadButton(rb15, "Lt Cyan");
    muiLoadButton(rb16, "Lt Red");
    muiLoadButton(rb17, "Pink");
    muiLoadButton(rb18, "Yellow");
    muiLoadButton(rb19, "White");
    //muiLoadButton(rb20, "");
    //muiLinkButtons(rb1, rb2);
    muiLinkButtons(rb2, rb3);
    muiLinkButtons(rb3, rb4);
    muiLinkButtons(rb4, rb5);
    muiLinkButtons(rb5, rb6);
    muiLinkButtons(rb6, rb7);
    muiLinkButtons(rb7, rb8);
    muiLinkButtons(rb8, rb9);
    //muiLinkButtons(rb9, rb10);
    //muiLinkButtons(rb10, rb11);
    muiLinkButtons(rb9, rb12); //muiLinkButtons(rb11, rb12);
    muiLinkButtons(rb12, rb13);
    muiLinkButtons(rb13, rb14);
    muiLinkButtons(rb14, rb15);
    muiLinkButtons(rb15, rb16);
    muiLinkButtons(rb16, rb17);
    muiLinkButtons(rb17, rb18);
    muiLinkButtons(rb18, rb19);
    //muiSetCallback(rb1, readbutton);
    muiSetCallback(rb2, readbutton);
    muiSetCallback(rb3, readbutton);
    muiSetCallback(rb4, readbutton);
    muiSetCallback(rb5, readbutton);
    muiSetCallback(rb6, readbutton);
    muiSetCallback(rb7, readbutton);
    muiSetCallback(rb8, readbutton);
    muiSetCallback(rb9, readbutton);
    //muiSetCallback(rb10, readbutton);
    //muiSetCallback(rb11, readbutton);
    muiSetCallback(rb12, readbutton);
    muiSetCallback(rb13, readbutton);
    muiSetCallback(rb14, readbutton);
    muiSetCallback(rb15, readbutton);
    muiSetCallback(rb16, readbutton);
    muiSetCallback(rb17, readbutton);
    muiSetCallback(rb18, readbutton);
    muiSetCallback(rb19, readbutton);
    //muiSetID(rb1, 1);
    muiSetID(rb2, 2);
    muiSetID(rb3, 3);
    muiSetID(rb4, 4);
    muiSetID(rb5, 5);
    muiSetID(rb6, 6);
    muiSetID(rb7, 7);
    muiSetID(rb8, 8);
    muiSetID(rb9, 9);
    //muiSetID(rb10, 10);
    //muiSetID(rb11, 11);
    muiSetID(rb12, 12);
    muiSetID(rb13, 13);
    muiSetID(rb14, 14);
    muiSetID(rb15, 15);
    muiSetID(rb16, 16);
    muiSetID(rb17, 17);
    muiSetID(rb18, 18);
    muiSetID(rb19, 19);
    //muiClearRadio(rb1);

    b3 = muiNewButton(ow+dw-60, ow+dw-34, oh+2, oh+27);
    muiLoadButton(b3, "Ok");
    muiSetCallback(b3, bbcallback);
    muiSetID(b3, 3);

    b4 = muiNewButton(ow+dw-30, ow+dw-4, oh+2, oh+27);
    muiLoadButton(b4, "X");
    muiSetCallback(b4, bbcallback);
    muiSetID(b4, 4);
  }
  else
  {
    muiSetActiveUIList(BACK_UILIST);
    muiHideAll(muiGetUIList(b3), 1);
  }
  // Clear the radio buttons and set the correct one active.
  muiClearRadio(rb2);
  viewchoice = 0;
  // NOTE:  Cannot preset to current background because its not saved.

  MUIstarted = 1;

  muiAttachUIList(BACK_UILIST);
}

/***************************************************************/
void dreadbutton(muiObject *obj, enum muiReturnValue r)
{
  int i, active;

  i = muiGetID(obj);
  active = muiGetActive(obj);

  switch (i)
  {
  case 2:
    if (active)
      viewchoice |= TYPE_F_SHADED_MODE; // zShading = 1;
    else
      viewchoice &= ~(TYPE_F_SHADED_MODE); // zShading = 0;
    break;
  case 3:
    if (active)
      viewchoice &= ~(TYPE_F_NO_POLYGONS); // zWire = 0;
    else
      viewchoice |= TYPE_F_NO_POLYGONS; // zWire = 1;
    break;
  case 4:
    if (active)
      viewchoice &= ~(TYPE_F_NO_LINES); // Edgelines
    else
      viewchoice |= TYPE_F_NO_LINES; // no Edgelines
    break;
  case 5:
    projchoice = active; // qualityLines
    break;

  case 8:
    // Normal studs
    viewchoice &= ~(TYPE_F_STUDLINE_MODE);
    viewchoice &= ~(TYPE_F_STUDLESS_MODE);
    break;
  case 9:
    // Line studs
    viewchoice |= TYPE_F_STUDLINE_MODE;
    viewchoice &= ~(TYPE_F_STUDLESS_MODE);
    break;
  case 10:
    // No studs
    viewchoice &= ~(TYPE_F_STUDLINE_MODE);
    viewchoice |= TYPE_F_STUDLESS_MODE;
    break;

  case 12:
    panchoice = (TYPE_F_BBOX_MODE | TYPE_F_NO_POLYGONS);
    break;
  case 13:
    panchoice = (TYPE_F_NO_POLYGONS | TYPE_F_STUDLESS_MODE);
    break;
  case 14:
    panchoice = (TYPE_F_INVISIBLE);
    break;
  case 15:
    panchoice = (TYPE_F_BBOX_MODE | TYPE_F_SHADED_MODE);
    break;
  case 16:
    panchoice = (TYPE_F_STUDLESS_MODE | TYPE_F_SHADED_MODE);
    break;
  case 17:
    panchoice = (TYPE_F_SHADED_MODE);
    break;

  default:
    break;
  }

  //printf("radio callback %d, %d\n", i, r);
}

/***************************************************************/
void dbcallback(muiObject *obj, enum muiReturnValue r)
{
  int i = 0;

  i = muiGetID(obj);

  muiHide(obj, 0);

  switch (i)
  {
  case 3:
    muiSetActiveUIList(MAIN_UILIST);
    mui_cleanup();
    if (viewchoice != ldraw_commandline_opts.F)
      ldraw_commandline_opts.F = viewchoice;
    qualityLines = projchoice;
    pan_visible = panchoice;
    break;
  default:
    muiHideAll(muiGetUIList(obj), 0);
    makemainui();
  }

  //printf("Drawing Button callback %d, %d\n", i, r);
}

/***************************************************************/
void makedrawui(void)
{
  static muiObject *rb1, *rb2, *rb3, *rb4, *rb5, *rb6, *rb7, *rb8, *rb9, *rb10;
  static muiObject *rb11, *rb12, *rb13, *rb14, *rb15, *rb16, *rb17, *rb18, *rb19;
  static muiObject *b3, *b4;
  static muiObject *l1, *l2, *l3, *l4;
  int xmin, ymin, xmax, ymax;

  static int MUIstarted = 0;

  if (MUIstarted == 0)
  {
#if 0
    muiNewUIList(DRAW_UILIST);
#else
    muiSetActiveUIList(DRAW_UILIST);
#endif

    //rb1 = muiNewTinyRadioButton(ow+2, oh+dh-20);
    l1 = muiNewBoldLabel(ow+4, oh+dh-18, "Drawing Style:");
    rb2 = muiNewTinyRadioButton(ow+2, oh+dh-40);
    rb3 = muiNewTinyRadioButton(ow+2, oh+dh-60);
    rb4 = muiNewTinyRadioButton(ow+2, oh+dh-80);
    rb5 = muiNewTinyRadioButton(ow+2, oh+dh-100);
    //rb6 = muiNewTinyRadioButton(ow+2, oh+dh-120);
    //rb7 = muiNewTinyRadioButton(ow+2, oh+dh-140);
    l2 = muiNewBoldLabel(ow+4, oh+dh-138, "Stud Type:");
    rb8 = muiNewTinyRadioButton(ow+2, oh+dh-160);
    rb9 = muiNewTinyRadioButton(ow+2, oh+dh-180);
    rb10 = muiNewTinyRadioButton(ow+2, oh+dh-200);
    //rb11 = muiNewTinyRadioButton(ow+dw/2, oh+dh-20);
    l3 = muiNewBoldLabel(ow+2+dw/2, oh+dh-18, "Spin Mode:");
    rb12 = muiNewTinyRadioButton(ow+dw/2, oh+dh-40);
    rb13 = muiNewTinyRadioButton(ow+dw/2, oh+dh-60);
    rb14 = muiNewTinyRadioButton(ow+dw/2, oh+dh-80);
    rb15 = muiNewTinyRadioButton(ow+dw/2, oh+dh-100);
    rb16 = muiNewTinyRadioButton(ow+dw/2, oh+dh-120);
    rb17 = muiNewTinyRadioButton(ow+dw/2, oh+dh-140);
    //rb18 = muiNewTinyRadioButton(ow+dw/2, oh+dh-160);
    //rb19 = muiNewTinyRadioButton(ow+dw/2, oh+dh-180);
    //rb20 = muiNewTinyRadioButton(ow+dw/2, oh+dh-200);
    //muiLoadButton(rb1, "");
    muiLoadButton(rb2, "Shading");
    muiLoadButton(rb3, "Surfaces");
    muiLoadButton(rb4, "Edges");
    muiLoadButton(rb5, "AntiAliasing");
    //muiLoadButton(rb6, "");
    //muiLoadButton(rb7, "");
    muiLoadButton(rb8, "Normal");
    muiLoadButton(rb9, "Lines");
    muiLoadButton(rb10, "None");
    //muiLoadButton(rb11, "");
    muiLoadButton(rb12, "Wire Boxes");
    muiLoadButton(rb13, "Wireframe");
    muiLoadButton(rb14, "Dragline");
    muiLoadButton(rb15, "Solid Boxes");
    muiLoadButton(rb16, "Studless");
    muiLoadButton(rb17, "Everything");
    //muiLinkButtons(rb1, rb2);
    //muiLinkButtons(rb2, rb3);
    //muiLinkButtons(rb3, rb4);
    //muiLinkButtons(rb4, rb5);
    //muiLinkButtons(rb5, rb6);
    //muiLinkButtons(rb6, rb7);
    //muiLinkButtons(rb7, rb8);
    muiLinkButtons(rb8, rb9);
    muiLinkButtons(rb9, rb10);
    //muiLinkButtons(rb10, rb11);
    //muiLinkButtons(rb11, rb12);
    muiLinkButtons(rb12, rb13);
    muiLinkButtons(rb13, rb14);
    muiLinkButtons(rb14, rb15);
    muiLinkButtons(rb15, rb16);
    muiLinkButtons(rb16, rb17);
    //muiSetCallback(rb1, dreadbutton);
    muiSetCallback(rb2, dreadbutton);
    muiSetCallback(rb3, dreadbutton);
    muiSetCallback(rb4, dreadbutton);
    muiSetCallback(rb5, dreadbutton);
    //muiSetCallback(rb6, dreadbutton);
    //muiSetCallback(rb7, dreadbutton);
    muiSetCallback(rb8, dreadbutton);
    muiSetCallback(rb9, dreadbutton);
    muiSetCallback(rb10, dreadbutton);
    //muiSetCallback(rb11, dreadbutton);
    muiSetCallback(rb12, dreadbutton);
    muiSetCallback(rb13, dreadbutton);
    muiSetCallback(rb14, dreadbutton);
    muiSetCallback(rb15, dreadbutton);
    muiSetCallback(rb16, dreadbutton);
    muiSetCallback(rb17, dreadbutton);
    //muiSetID(rb1, 1);
    muiSetID(rb2, 2);
    muiSetID(rb3, 3);
    muiSetID(rb4, 4);
    muiSetID(rb5, 5);
    //muiSetID(rb6, 6);
    //muiSetID(rb7, 7);
    muiSetID(rb8, 8);
    muiSetID(rb9, 9);
    muiSetID(rb10, 10);
    //muiSetID(rb11, 11);
    muiSetID(rb12, 12);
    muiSetID(rb13, 13);
    muiSetID(rb14, 14);
    muiSetID(rb15, 15);
    muiSetID(rb16, 16);
    muiSetID(rb17, 17);
    //muiClearRadio(rb1);

    b3 = muiNewButton(ow+dw-60, ow+dw-34, oh+2, oh+27);
    muiLoadButton(b3, "Ok");
    muiSetCallback(b3, dbcallback);
    muiSetID(b3, 3);

    b4 = muiNewButton(ow+dw-30, ow+dw-4, oh+2, oh+27);
    muiLoadButton(b4, "X");
    muiSetCallback(b4, dbcallback);
    muiSetID(b4, 4);
  }
  else
  {
    muiSetActiveUIList(DRAW_UILIST);
    muiHideAll(muiGetUIList(b3), 1);
  }

  // Clear the radio buttons and set the correct one active.
  muiClearRadio(rb8);
  muiClearRadio(rb12);
  viewchoice = ldraw_commandline_opts.F;
  projchoice = qualityLines;
  panchoice = pan_visible;

  if (viewchoice & TYPE_F_SHADED_MODE)
    muiSetActive(rb2, 1);
  else
    muiSetActive(rb2, 0);
  if (viewchoice & TYPE_F_NO_POLYGONS)
    muiSetActive(rb3, 0);
  else
    muiSetActive(rb3, 1);
  if (viewchoice & TYPE_F_NO_LINES)
    muiSetActive(rb4, 0);
  else
    muiSetActive(rb4, 1);
  if (qualityLines)
    muiSetActive(rb5, 1);
  else
    muiSetActive(rb5, 0);

  if (viewchoice & TYPE_F_STUDLESS_MODE)
    muiSetActive(rb10, 1);
  else if (viewchoice & TYPE_F_STUDLINE_MODE)
    muiSetActive(rb9, 1);
  else
    muiSetActive(rb8, 1);

  if (pan_visible == (TYPE_F_NO_POLYGONS | TYPE_F_STUDLESS_MODE) )
    muiSetActive(rb13, 1);
  else if (pan_visible == (TYPE_F_INVISIBLE) )
    muiSetActive(rb14, 1);
  else if (pan_visible == (TYPE_F_BBOX_MODE | TYPE_F_SHADED_MODE) )
    muiSetActive(rb15, 1);
  else if (pan_visible == (TYPE_F_STUDLESS_MODE | TYPE_F_SHADED_MODE) )
    muiSetActive(rb16, 1);
  else if (pan_visible == (TYPE_F_SHADED_MODE) )
    muiSetActive(rb17, 1);
  else // (pan_visible == (TYPE_F_BBOX_MODE | TYPE_F_NO_POLYGONS) )
    muiSetActive(rb12, 1);

  MUIstarted = 1;

  muiAttachUIList(DRAW_UILIST);
}

/***************************************************************/
void oreadbutton(muiObject *obj, enum muiReturnValue r)
{
  int i = -1;
  int active;

  i = muiGetID(obj);
  active = muiGetActive(obj);

  switch (i)
  {
  case 2:
    if (active)
      viewchoice = 'P'; // Pause for steps
    else
      viewchoice = 'C'; // Continuous
    break;
  case 3:
    projchoice = active;
    break;
  case 9:
    panchoice = 0;
    break;
  case 10:
    panchoice = 1;
    break;
  }

  //printf("options radio callback %d, %d\n", i, r);
}

/***************************************************************/
void obcallback(muiObject *obj, enum muiReturnValue r)
{
  int i = 0;

  i = muiGetID(obj);

  muiHide(obj, 0);

  switch (i)
  {
  case 3:
    muiSetActiveUIList(MAIN_UILIST);
    mui_cleanup();
    if (viewchoice != ldraw_commandline_opts.M)
      menuKeyEvent('s', 0, 0);
    if (projchoice != ldraw_commandline_opts.poll)
      menuKeyEvent('g', 0, 0);
#ifdef USE_L3_PARSER
    if (panchoice != parsername)
      menuKeyEvent('r', 0, 0);
#endif
    break;
  default:
    muiHideAll(muiGetUIList(obj), 0);
    makemainui();
  }

  //printf("Options Button callback %d, %d\n", i, r);
}

/***************************************************************/
void makeoptsui(void)
{
  static muiObject *rb1, *rb2, *rb3, *rb4, *rb5, *rb6, *rb7, *rb8;
  static muiObject *rb9, *rb10, *rb11, *rb12, *rb13, *rb14, *rb15, *rb16;
  static muiObject *b3, *b4;
  static muiObject *l1, *l2, *l3, *l4;
  int xmin, ymin, xmax, ymax;

  static int MUIstarted = 0;

  if (MUIstarted == 0)
  {
#if 0
    muiNewUIList(OPTS_UILIST);
#else
    muiSetActiveUIList(OPTS_UILIST);
#endif

    //rb1 = muiNewTinyRadioButton(ow+2, oh+dh-20);
    l1 = muiNewBoldLabel(ow+4, oh+dh-18, "Options:");
    rb2 = muiNewTinyRadioButton(ow+2, oh+dh-40);
    rb3 = muiNewTinyRadioButton(ow+2, oh+dh-60);
    //rb4 = muiNewTinyRadioButton(ow+2, oh+dh-80);
    //rb5 = muiNewTinyRadioButton(ow+2, oh+dh-100);
    //rb6 = muiNewTinyRadioButton(ow+2, oh+dh-120);
    //rb7 = muiNewTinyRadioButton(ow+2, oh+dh-140);
    //rb8 = muiNewTinyRadioButton(ow+2, oh+dh-160);
    l2 = muiNewBoldLabel(ow+4, oh+dh-158, "Parser:");
    rb9 = muiNewTinyRadioButton(ow+2, oh+dh-180);
    rb10 = muiNewTinyRadioButton(ow+2, oh+dh-200);
    //rb11 = muiNewTinyRadioButton(ow+dw/2, oh+dh-20);
    //rb12 = muiNewTinyRadioButton(ow+dw/2, oh+dh-40);
    //rb13 = muiNewTinyRadioButton(ow+dw/2, oh+dh-60);
    //rb14 = muiNewTinyRadioButton(ow+dw/2, oh+dh-80);
    //rb15 = muiNewTinyRadioButton(ow+dw/2, oh+dh-100);
    //rb16 = muiNewTinyRadioButton(ow+dw/2, oh+dh-120);
    //rb17 = muiNewTinyRadioButton(ow+dw/2, oh+dh-140);
    //rb18 = muiNewTinyRadioButton(ow+dw/2, oh+dh-160);
    //rb19 = muiNewTinyRadioButton(ow+dw/2, oh+dh-180);
    //rb20 = muiNewTinyRadioButton(ow+dw/2, oh+dh-200);
    //muiLoadButton(rb1, "");
    muiLoadButton(rb2, "Step Mode");
    muiLoadButton(rb3, "Polling Mode");
    //muiLoadButton(rb4, "");
    //muiLoadButton(rb5, "");
    //muiLoadButton(rb6, "");
    //muiLoadButton(rb7, "");
    //muiLoadButton(rb8, "");
    muiLoadButton(rb9, "Ldlite");
    muiLoadButton(rb10, "L3");
    //muiLoadButton(rb11, "");
    //muiLoadButton(rb12, "");
    //muiLoadButton(rb13, "");
    //muiLoadButton(rb14, "");
    //muiLoadButton(rb15, "");
    //muiLoadButton(rb16, "");
    //muiLinkButtons(rb1, rb2);
    //muiLinkButtons(rb2, rb3);
    //muiLinkButtons(rb3, rb4);
    //muiLinkButtons(rb4, rb5);
    //muiLinkButtons(rb5, rb6);
    //muiLinkButtons(rb6, rb7);
    //muiLinkButtons(rb7, rb8);
    //muiLinkButtons(rb8, rb9);
    muiLinkButtons(rb9, rb10);
    //muiLinkButtons(rb10, rb11);
    //muiLinkButtons(rb11, rb12);
    //muiLinkButtons(rb12, rb13);
    //muiLinkButtons(rb13, rb14);
    //muiLinkButtons(rb14, rb15);
    //muiLinkButtons(rb15, rb16);
    //muiSetCallback(rb1, oreadbutton);
    muiSetCallback(rb2, oreadbutton);
    muiSetCallback(rb3, oreadbutton);
    //muiSetCallback(rb4, oreadbutton);
    //muiSetCallback(rb5, oreadbutton);
    //muiSetCallback(rb6, oreadbutton);
    //muiSetCallback(rb7, oreadbutton);
    //muiSetCallback(rb8, oreadbutton);
    muiSetCallback(rb9, oreadbutton);
    muiSetCallback(rb10, oreadbutton);
    //muiSetCallback(rb11, oreadbutton);
    //muiSetCallback(rb12, oreadbutton);
    //muiSetCallback(rb13, oreadbutton);
    //muiSetCallback(rb14, oreadbutton);
    //muiSetCallback(rb15, oreadbutton);
    //muiSetCallback(rb16, oreadbutton);
    //muiSetID(rb1, 1);
    muiSetID(rb2, 2);
    muiSetID(rb3, 3);
    //muiSetID(rb4, 4);
    //muiSetID(rb5, 5);
    //muiSetID(rb6, 6);
    //muiSetID(rb7, 7);
    //muiSetID(rb8, 8);
    muiSetID(rb9, 9);
    muiSetID(rb10, 10);
    //muiSetID(rb11, 11);
    //muiSetID(rb12, 12);
    //muiSetID(rb13, 13);
    //muiSetID(rb14, 14);
    //muiSetID(rb15, 15);
    //muiSetID(rb16, 16);
    //muiClearRadio(rb1);

    b3 = muiNewButton(ow+dw-60, ow+dw-34, oh+2, oh+27);
    muiLoadButton(b3, "Ok");
    muiSetCallback(b3, obcallback);
    muiSetID(b3, 3);

    b4 = muiNewButton(ow+dw-30, ow+dw-4, oh+2, oh+27);
    muiLoadButton(b4, "X");
    muiSetCallback(b4, obcallback);
    muiSetID(b4, 4);
  }
  else
  {
    muiSetActiveUIList(OPTS_UILIST);
    muiHideAll(muiGetUIList(b3), 1);
  }
  // Clear the radio buttons and set the correct one active.
  muiClearRadio(rb9);
  viewchoice = ldraw_commandline_opts.M;
  projchoice = ldraw_commandline_opts.poll;

  if (ldraw_commandline_opts.M == 'C')
    muiSetActive(rb2, 0);
  else
    muiSetActive(rb2, 1);
  if (ldraw_commandline_opts.poll == 1)
    muiSetActive(rb3, 1);
  else
    muiSetActive(rb3, 0);
#ifdef USE_L3_PARSER
  panchoice = parsername;
  if (parsername)
    muiSetActive(rb10, 1);
  else
    muiSetActive(rb9, 1);
#else
  panchoice = 0;
  muiSetActive(rb9, 1);
  muiSetEnabled(rb10, 0);
#endif

  MUIstarted = 1;

  muiAttachUIList(OPTS_UILIST);
}

/***************************************************************/
/***************************************************************/
#if defined(MAC)
#include "macos_dirent.h"
#elif defined(WINDOWS)
#include "win32_dirent.h"
#else
#include <dirent.h>  // directory operations
#endif
#include <string.h>
#include <sys/stat.h>
#ifdef _MSC_VER
#include <direct.h>
#else
#include <unistd.h>
#endif
#include <stdlib.h>

#ifndef MAXNAMLEN
#define MAXNAMLEN 256
#endif

char    **filelist = NULL;
char    err[80];
char    *dot = ".";
char    *dotdot = "..";
char    directory[300], originaldir[300];
struct stat d, dd;
struct dirent   *dir;

DIR *file;
int off;

int selectedfile = -1;
int selectedprev = -1;

/***************************************************************/
void errormsg(char *s)
{
    fprintf(stderr, "%s\n", s);
}

/***************************************************************/
void prname(void)
{
    directory[0] = '/';
    if (off == 0)
        off = 1;
    directory[off] = 0;
}

/***************************************************************/
int dirlevels(char *s)
{
    int levels;

    for (levels = 0; *s; s++)
    if (*s == '/')
        levels++;
    return(levels);
}

/***************************************************************/
int cat(void)
{
    register i, j;
    char *name = directory + 1; /* I love C */

    i = -1;
    while (dir->d_name[++i] != 0)
    if ((off+i+2) > MAXNAMLEN - 1) {
        prname();
        return 1;
    }
    for(j=off+1; j>=0; --j)
        name[j+i+1] = name[j];
    off=i+off+1;
    name[i] = '/';
    for(--i; i>=0; --i)
        name[i] = dir->d_name[i];
    return 0;
}

/***************************************************************/
/* get the current working directory (the following routines are from pwd.c) */
int pwd(void)
{
#if 1 /* ifndef UNIX or ifndef AGL */
  getcwd(directory, MAXNAMLEN);
#else
  for(off = 0;;) {
    if(stat(dot, &d) < 0) {
      fprintf(stderr, "pwd: cannot stat .!\n");
      return(2);
    }
    if ((file = opendir(dotdot)) == NULL) {
      fprintf(stderr,"pwd: cannot open ..\n");
      return(2);
    }
    if(fstat(file->dd_fd, &dd) < 0) {
      fprintf(stderr, "pwd: cannot stat ..!\n");
      return(2);

    }
    if(stat(dotdot, &dd) < 0) {
      fprintf(stderr, "pwd: cannot stat ..!\n");
      return(2);
    }
    if(chdir(dotdot) < 0) {
      fprintf(stderr, "pwd: cannot chdir to ..\n");
      return(2);
    }
    if(d.st_dev == dd.st_dev) {
      if(d.st_ino == dd.st_ino) {
    prname();
    chdir(directory);
    return(0);
      }
      do
    if ((dir = readdir(file)) == NULL) {
      fprintf(stderr, "pwd: read error in ..\n");
      return(2);
    }
      while (dir->d_ino != d.st_ino);
    }
    else do {
      if((dir = readdir(file)) == NULL) {
    fprintf(stderr, "pwd: read error in ..\n");
    return(2);
      }
      stat(dir->d_name, &dd);
    } while(dd.st_ino != d.st_ino || dd.st_dev != d.st_dev);
    (void)closedir(file);
    if (cat()) {
      chdir(directory);
      return(0);
    }
  }
#endif

  return(0);
}

/***************************************************************/
void freels(void)
{
    char **p;

    p = filelist;
    if (!p)
      return;
    while (*p != 0) {
    free(*p);
    *p = 0;
    p++;
    }
}

extern int isDir(char *filename);

/***************************************************************/
int mystrcmp(char **s1, char **s2)
{
  //return strcmp(*s1,*s2);

  int filetype = 0;

  //Sort with directories followed by files.  Case insensitive.
#if 0
  filetype = isDir(*s2) - isDir(*s1);
#else
  // isDir() does not work because we already stuck the slash at the end of dirs.
  if ((*s1)[strlen(*s1)-1] == '/')
    filetype -= 1;
  if ((*s2)[strlen(*s2)-1] == '/')
    filetype += 1;
#endif

  if (filetype == 0)
    return stricmp(*s1,*s2); // Both directories or both files, sort by name

  return filetype;
}

/***************************************************************/
void ls(void)
{
    DIR         *dirp;
    int         i = 0;
    int         len;
    int                 maxfile = 0;
    struct dirent   *dir;
    struct stat     statbuf;

    if ((dirp = opendir(directory)) == NULL) {
    errormsg("bad directory\n");
    return;
    }
    freels();

    if (filelist)
      free(filelist);
    while ((dir = readdir(dirp)) != NULL)
      maxfile++;
    filelist = (char **) calloc(sizeof(char *), maxfile+1);
    rewinddir(dirp);

    chdir(directory);
    while ((dir = readdir(dirp)) != NULL) {
    if (dir->d_name[0] == '.')
        continue;
    /*f = open(dir->d_name, O_RDONLY);
    if (!f)
        continue;
    if (!okfiletype(getfiletype(f)))
        continue;
    close(f);*/
    stat(dir->d_name,&statbuf);
    len = strlen(dir->d_name) + 1 + (statbuf.st_mode & S_IFDIR? 1 : 0);
    filelist[i] = (char *)malloc(len);
    strcpy(filelist[i], dir->d_name);
    if (statbuf.st_mode & S_IFDIR) {
        filelist[i][len-2] = '/'; filelist[i][len-1] = 0;
    }
    i++;
    if (i >= maxfile)
    {
      // UH OH!  Too many files in directory.
      break;
    }
    }
    filelist[i] = 0;
    qsort(&filelist[0], i, sizeof (char *),
      (int (*)(const void *, const void *))mystrcmp);
    closedir(dirp);
}

/***************************************************************/
/***************************************************************/
int cd(char *s)
{
    char *p = directory;
    extern int strwidth(char *s);

    //printf("cd %s\n", s);
    muiClearTBString(t1);  // Clear out the text box
    if(chdir(s) < 0) {
    fprintf(stderr,"cannot open %s\n",s);
    return -1;
    }
    pwd();
    ls();
    muiSetTLStrings(tl, filelist);
    while (*p && strwidth(p) > 250) /* Make it fit in title bar label.*/
      p++;
    muiChangeLabel(l4, p);
    selectedfile = -1;
    selectedprev = -1;
    muiSetVSValue(vs, 1.0);
    return 0;
}

/***************************************************************/
void writeoutputfile(char *dir, char *file)
{
  extern void loadnewdatfile(char *datpath, char *datfile);
  extern void saveasdatfile(char *datpath, char *datfile);

#if 0
  printf("D:%s\n", dir);
  if (file)
    printf("F:%s\n", file);
#endif

  if (dir && file)
  {
    //Got a dir and file.  Do something with it.
    if (fileOpenSave)
      saveasdatfile(dir, file);
    else
      loadnewdatfile(dir, file);

    mui_cleanup();
  }
}

/***************************************************************/
void    controltltop(muiObject *obj, enum muiReturnValue value)
{
    float sliderval;

    //printf("vs fn %d\n", value);

    if ((value != MUI_SLIDER_RETURN) && (value != MUI_SLIDER_THUMB)
    )
      return;

    sliderval = muiGetVSVal(obj);

    muiSetTLTop(tl, sliderval);

    //printf("sliderval =  %0.4f\n", sliderval);
}

/***************************************************************/
void    handlefileselection(muiObject *obj, enum muiReturnValue value)
{
    char *fname;
    int len;
    int doubleclick = 0;

    //printf("tl fn %d\n", value);

    selectedfile = muiGetTLSelectedItem(obj);
    fname = filelist[selectedfile];
    if (fname == NULL)
      return;
    if (selectedfile == selectedprev)
      doubleclick = 1;
    selectedprev = selectedfile;
    if ((value == MUI_TEXTLIST_RETURN_CONFIRM) || doubleclick)
    {
#if 0
        if (doubleclick)
      printf("DblClick! ");
    printf("Selected file %s\n", fname);
#endif
    len = strlen(fname);
    if ((fname[len-1] == '/') || (fname[len-1] == '\\'))
    {
        fname[len-1] = 0;
        cd(fname);
        return;
    } else {
        writeoutputfile(directory, fname);
        return; //exit(0);
    }
    }
    else if (value != MUI_TEXTLIST_RETURN) return;

    muiSetTBString(t1, fname);
    //muiSetVSValue(vs, 1.0);
}

/***************************************************************/
void handleaccept(muiObject *obj, enum muiReturnValue value)
{
    char *fname;
    int len;

    if (value != MUI_BUTTON_PRESS) return;
    fname = muiGetTBString(t1);
    if (!fname || (strlen(fname) == 0))
    {
      if (selectedfile == -1) return;
      fname = filelist[selectedfile];
    }
    if (fname == NULL)
      return;
    len = strlen(fname);
    if ((fname[len-1] == '/') || (fname[len-1] == '\\'))
    {
    fname[len-1] = 0;
    cd(fname);
    return;
    }
    else if (isDir(fname)) {
    cd(fname);
    return;
    } else {
    writeoutputfile(directory, fname);
    return; //exit(0);
    }
    obj = 0;    /* for lint's sake */
}

/***************************************************************/
void handleoriginal(muiObject *obj, enum muiReturnValue value)
{
    if (value != MUI_BUTTON_PRESS) return;
    cd(originaldir);
    obj = 0;    /* for lint's sake */
}

/***************************************************************/
void handleupdir(muiObject *obj, enum muiReturnValue value)
{
    if (value != MUI_BUTTON_PRESS) return;
    cd("..");
    obj = 0;    /* for lint's sake */
}

/***************************************************************/
void handlecancel(muiObject *obj, enum muiReturnValue value)
{
    if (value != MUI_BUTTON_PRESS) return;
    writeoutputfile(directory, 0);

    muiHideAll(muiGetUIList(obj), 0);
    makemainui();
}

/***************************************************************/
void handletextbox(muiObject *obj, enum muiReturnValue value)
{
    char *s, *slash;

    if (value != MUI_TEXTBOX_RETURN) return;
    // Use strdup() because cd(s) frees the string returned by muiGetTBString.
    s = strdup(muiGetTBString(obj));
    if (0 == cd(s)) {
        if (s) free(s);
    return;
    }
    /* hack up the path, if any */
    slash = strrchr(s, '/');
    if (slash == 0) {
    slash = s-1;    /* to make filename == slash+1 */
    } else {
    if (*s == '/') { /* absolute path */
        strncpy(directory, s, slash-s);
        directory[slash-s] = 0;
    } else {
        strcat(directory, "/");
        strncat(directory, s, slash-s);
    }
    }
     /* now filename == slash+1 */
    writeoutputfile(directory, slash+1);
    free(s);
    return; //exit(0);
}

/***************************************************************/
void makefileui(char *s)
{
  static muiObject *l1, *b1, *b2, *b3, *b4;
  int xmin, ymin, xmax, ymax;

  static int MUIstarted = 0;

  if (MUIstarted == 0)
  {
#if 0
    muiNewUIList(FILE_UILIST);
#else
    muiSetActiveUIList(FILE_UILIST);
#endif
    l4 = muiNewLabel(ow+4, oh+dh-18, "./");

    b1 = muiNewButton(ow+dw-60, ow+dw-34, oh+dh-24, oh+dh-2);
    muiLoadButton(b1, "<-");
    muiSetCallback(b1, handleoriginal);

    b2 = muiNewButton(ow+dw-30, ow+dw-4, oh+dh-24, oh+dh-2);
    muiLoadButton(b2, "Up"); // For the Mac...  muiLoadButton(b2, "..");
    muiSetCallback(b2, handleupdir);

    tl = muiNewTextList(ow+4, oh+30, ow+dw-25, 8);
    muiGetObjectSize(tl, &xmin, &ymin, &xmax, &ymax);
    vs = muiNewVSlider(xmax+1, ymin+2, ymax-1, 0, THUMBHEIGHT);
    muiSetVSValue(vs, 1.0);
    //muiSetVSArrowDelta(vs, (ymax-ymin-10-THUMBHEIGHT-ARROWSPACE)/((sizeof strs)/(sizeof (char *))-9));
    muiSetVSArrowDelta(vs, 1);

    //l2 = muiNewBoldLabel(ow+4, oh+8, "File:");
    l1 = muiNewLabel(ow+4, oh+8, "File:");

    t1 = muiNewTextbox(ow+40, ow+dw-64, oh+0);
    muiSetActive(t1, 1);
    muiSetCallback(t1, handletextbox);

    b3 = muiNewButton(ow+dw-60, ow+dw-34, oh+2, oh+27);
    muiLoadButton(b3, "Ok");
    muiSetCallback(b3, handleaccept);

    b4 = muiNewButton(ow+dw-30, ow+dw-4, oh+2, oh+27);
    muiLoadButton(b4, "X");
    muiSetCallback(b4, handlecancel);

    muiSetCallback(vs, controltltop);
    muiSetCallback(tl, handlefileselection);
  }
  else
  {
    muiSetActiveUIList(FILE_UILIST);
    muiHideAll(muiGetUIList(l1), 1);
    muiClearTBString(t1);
  }
  MUIstarted = 1;

  muiAttachUIList(FILE_UILIST);

  muiChangeLabel(l1, s);
  fileOpenSave = strcmp(s, "Open:");

  muiClearTBString(t1);  // Clear out the text box

  selectedfile = -1;
  selectedprev = -1;

  strcpy(directory, ".");
  cd(directory);
  strcpy(originaldir, directory);
}

/***************************************************************/
// Glut MUI callbacks.  mui_Reshape must be fixed to not be static.
/***************************************************************/
extern void mui_keyboard(unsigned char c, int x, int y);
extern void mui_mouse(int b, int s, int x, int y);
extern void mui_Reshape(int width, int height);
extern void mui_glutmotion(int x, int y);
extern void mui_glutpassivemotion(int x, int y);
extern void mui_drawgeom(void);
extern void mui_menufunc(int state);

/***************************************************************/
void unMUI_keyboard(unsigned char key, int x, int y)
{
  if (key == 27)
  {
    mui_cleanup();
    return;
  }

  mui_keyboard(key, x, y);
}

/***************************************************************/
void unMUI_fnkeys(int key, int x, int y)
{
  float sliderval;

  if (!vs)
    return;
  if (!muiGetVisible(vs))
    return;

  sliderval = muiGetVSVal(vs);

  //printf("sliderval = %g\n", sliderval);

  switch(key) {
  case GLUT_KEY_PAGE_UP:
  case GLUT_KEY_UP:
    //muiSetVSValue(vs, sliderval + 0.01);
    muiSetVSValue(vs, sliderval + 0.02);
    sliderval = muiGetVSVal(vs);
    muiSetTLTop(tl, sliderval);
    glutPostRedisplay();
    break;
  case GLUT_KEY_PAGE_DOWN:
  case GLUT_KEY_DOWN:
    muiSetVSValue(vs, sliderval - 0.01);
    sliderval = muiGetVSVal(vs);
    muiSetTLTop(tl, sliderval);
    glutPostRedisplay();
    break;
  case GLUT_KEY_RIGHT:
  case GLUT_KEY_LEFT:
    break;
  }
}

/***************************************************************/
void unMUI_viewport()
{
  static int MUIstarted = 0;

  // Cannot move the viewport once the widgets are created.
  // Unless we want to move all the widget positions.
  if (MUIstarted == 0)
  {
    ow = (Width-dw)/2;
    oh = (Height-dh)/2;
  }
  else
  {
    int dx = ow;        // Save old offset (x, y)
    int dy = oh;

    ow = (Width-dw)/2;
    oh = (Height-dh)/2; // Calculate new offset (x, y)

    dx = ow - dx;       // Find difference in offset (dx, dy)
    dy = oh - dy;

    if (dx || dy)
    {
      //NOTE: somehow this gives me a grey background on the whole window.
      //printf("muiMoveAll(%d, %d)\n", dx, dy);
      //muiBackgroundClear();
      muiMoveAll(MAIN_UILIST, dx, dy);
      muiMoveAll(FILE_UILIST, dx, dy);
      muiMoveAll(VIEW_UILIST, dx, dy);
      muiMoveAll(DRAW_UILIST, dx, dy);
      muiMoveAll(OPTS_UILIST, dx, dy);
      muiMoveAll(BACK_UILIST, dx, dy);
      muiMoveAll(HELP_UILIST, dx, dy);
    }
  }
  MUIstarted = 1;

  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  gluOrtho2D(0, Width, 0, Height);
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();
  glRasterPos2i(0, 0);

  //Draw a black rectangle just larger than the GUI (a cheesy frame)
  glColor3ub(0, 0, 0);
  glColor4ub(0, 0, 0, 0);
  glBegin(GL_QUADS);
  glVertex2i(ow-1, oh-1);
  glVertex2i(ow+dw+1, oh-1);
  glVertex2i(ow+dw+1, oh+dh+1);
  glVertex2i(ow-1, oh+dh+1);
  glEnd();

  glEnable(GL_SCISSOR_TEST);
  glScissor(ow, oh, dw, dh); // x,y,width,height
  //printf("glScissor(%d, %d, %d, %d)\n",ow, oh, dw, dh); // x,y,width,height
}

/***************************************************************/
void unMUI_Reshape(int width, int height)
{
  Width = width;
  Height = height;

  mui_Reshape(width, height); // Sets glViewport();

  // MUI does not know about the SCISSOR
  // so we need to clear the window then adjust the SCISSOR.
  glDisable(GL_SCISSOR_TEST);
  //muiBackgroundClear();
  //NOTE: This should use the current background color, but we need to save it in main.c
  glClearColor(1.0, 1.0, 1.0, 0.0);
  glClear(GL_COLOR_BUFFER_BIT);
  unMUI_viewport();

  glEnable(GL_SCISSOR_TEST);
}

/***************************************************************/
void mui_menu(int menunum)
{
  // Gotta consider using creating and destroying a subwindow for window mode.
  // For game mode I could copy the screen to the back buffer, then
  // use MUI in singlebuffered mode, then copy back to front buffer when done.

  glDisable( GL_DEPTH_TEST ); /* don't test for depth -- just put in front  */
#if 0
  // Dont bother with this anymore since we replace the GLUT menu with MUI.
  glutSetMenu(mainmenunum); // Reset the current menu to the main menu.
  glutDetachMenu(GLUT_RIGHT_BUTTON); // Detach menu before starting MUI.
#endif

#ifdef SINGLE_BUFFER_MUI
  // Single buffered MUI is flickery.  You see the redraws, back to front.
  glDrawBuffer(GL_FRONT);  // Disable double buffer for MUI.
  mui_singlebuffered = 1;
#else
  glDrawBuffer(GL_BACK);  // Enable double buffer for MUI.
  mui_singlebuffered = 0;

  // Copy the front buffer to the back buffer before enabling scissor.
  // This way MUI will draw on top of the model already in the back buffer.
  // Dont copying if MUI will take over the whole screen, copying is VERY slow.
  glReadBuffer(GL_FRONT); // set pixel source
  glDrawBuffer(GL_BACK); // set pixel destination
  glDisable( GL_DEPTH_TEST ); // Speed up copying
  glDisable(GL_LIGHTING);     // Speed up copying
        glDisable(GL_ALPHA_TEST);
        glDisable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_DITHER);
        glDisable(GL_FOG);
        glDisable(GL_LIGHTING);
        glDisable(GL_LOGIC_OP);
        glDisable(GL_STENCIL_TEST);
        glDisable(GL_TEXTURE_1D);
        glDisable(GL_TEXTURE_2D);
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  gluOrtho2D(0, Width, 0, Height);
  glMatrixMode( GL_MODELVIEW );
  glPushMatrix();
  glLoadIdentity();
  glRasterPos2i(0, 0);
  glDepthMask(GL_FALSE); // disable updates to depth buffer
  glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE); //enable color buffer updates
  glCopyPixels(0, 0, Width, Height, GL_COLOR);
  glPopMatrix();
  glDepthMask(GL_TRUE); // enable updates to depth buffer
  glDepthFunc(GL_LESS);
  glEnable(GL_LIGHTING);
#endif

  // Setup nice rendering context for MUI.
  glDisable( GL_DEPTH_TEST ); /* don't test for depth -- just put in front  */
  glDisable(GL_LIGHTING);
  // glPolygonMode(GL_FRONT, GL_FILL);
  // glPolygonMode(GL_BACK, GL_LINE);
  glDisable(GL_STENCIL_TEST);
  glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE); //enable color buffer updates

  //glViewport(0, 0, Width, Height-40);
  // Scissor is probably easier than viewport
  // Watch out.  Textlist widget uses Scissor in push/popviewport()
  // So we MUST go full window for any gui with a textlist widget.
  unMUI_viewport();

  makemainui();
  muiInit();

  if (menunum == 1)
  {
    muiHideAll(muiGetUIList(b1), 0);
    makefileui("Open:");
  }
  else if (menunum == 2)
  {
    muiHideAll(muiGetUIList(b2), 0);
    makefileui("Save:");
  }

  // ---------------------------------------------------------------------
  // After muiInit() we can insert shim fns to intercept events before MUI.
  // ---------------------------------------------------------------------
  // glutKeyboardFunc(mui_keyboard);
  // glutMouseFunc(mui_mouse);
  // glutReshapeFunc(mui_Reshape);
  // glutMotionFunc(mui_glutmotion);
  // glutPassiveMotionFunc(mui_glutpassivemotion);
  // glutDisplayFunc(mui_drawgeom);
  // glutMenuStateFunc(mui_menufunc);
  glutKeyboardFunc(unMUI_keyboard); // Shim ESC key handler before MUI.
  glutReshapeFunc(unMUI_Reshape);
  glutSpecialFunc(unMUI_fnkeys);
  // ---------------------------------------------------------------------

  glutPostRedisplay(); // Otherwise MUI waits for mouse movement to display.
}

/***************************************************************/
void mui_test(void)
{
  mui_menu(0);
}

#endif

