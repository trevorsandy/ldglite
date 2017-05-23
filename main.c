 /*
 *  LDLITE, a program for viewing *.dat files.
 *  Copyright (C) 1998  Paul J. Gyugyi
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <sys/stat.h> // for polling datfile for updates.

#include "glwinkit.h" //#include <GL/glut.h>

#include "platform.h"
#include "ldliteVR.h"
#include "LDrawIni.h"

#  ifndef WINDOWS
     // This stuff gets pulled in by glut.h for windows.
#    include "wstubs.h"
#  else
     // glut 3.7 no longer includes windows.h
#    if (GLUT_XLIB_IMPLEMENTATION >= 13)
#      include <windows.h>
#    endif
#  endif

#ifdef VERSION_INFO
#ifdef ARCH
char ldgliteVersion[] = "Version "VERSION_INFO" ("ARCH")      ";
#else
char ldgliteVersion[] = "Version "VERSION_INFO"      ";
#endif
#else
char ldgliteVersion[] = "Version 1.3.2      ";
#endif

// Use Glut popup menus if MUI is not available.
#ifndef OFFSCREEN_ONLY
#ifndef TEST_MUI_GUI
#  define USE_GLUT_MENUS 1
#endif
#endif
extern char dirfilepath[256]; // Used by bogus glut popup menu file browser
extern int  mainmenunum;
extern void initializeMenus(void);

#ifdef USE_L3_PARSER
extern void          LoadModelPre(void);
extern int           LoadModel(const char *lpszPathName);
extern void          LoadModelPost(void);
extern void          DrawModel(void);
#endif

//extern void ldlite_parse(char *filename, char *ldraw_lines);
extern void ldlite_parse(char *ldraw_lines);
extern void translate_color(int c, ZCOLOR *zcp, ZCOLOR *zcs);

extern char pathname[256];
extern char primitivepath[256];
extern char partspath[256];
extern char modelspath[256];
extern char datfilepath[256];

char ldconfig[256] = {0};
char ldconfigfilename[256] = {0};

char userpath[256];
char bitmappath[256];

#ifdef USE_L3_PARSER
#define UNKNOWN_PARSER -1
#define LDLITE_PARSER 0
#define L3_PARSER     1

// It's 2008 and the l3 parser works much better with current GPU chips.
// So maybe it's about time to set L3 as the default parser.
int parsername = L3_PARSER; // (was UNKNOWN_PARSER)
#endif

int EPS_OUTPUT_FIGURED_OUT = 0;

char *picfilename = NULL;
char datfilename[256] = {0};
char title[256];
char progname[256];
char progpath[256];

char buf[16*1024]; // sizeof buf copied from render_file() in ldliteView.cpp
int use_uppercase = 0;

#define IMAGE_TYPE_PNG_RGB 1
#define IMAGE_TYPE_PNG_RGBA 2
#define IMAGE_TYPE_BMP8 3
#define IMAGE_TYPE_BMP 4
#define IMAGE_TYPE_PPM 5

int use_png_alpha = 1;

int ldraw_projection_type = 0;  // 1 = perspective, 0 = orthographic.
#define WIDE_ANGLE_VIEW 1
#if WIDE_ANGLE_VIEW
// znear 1.0 gives zfighting with 16bit Mesa Zbuf, 10.0 still does on datsville
double projection_znear = 10.0; 
double projection_zfar = 4000.0;
double projection_fov = 67.38; //L3P default is 67.38 degrees = 2*atan(2/3)
double projection_fromx = 0.0;
double projection_fromy = 0.0;
double projection_fromz = 1000.0;
double projection_depth = 1000; // distance(from, toward);
#else
double projection_znear = 100.0;
double projection_zfar = 4000.0;
double projection_fov = 20.0;
double projection_fromx = 0.0;
double projection_fromy = 0.0;
double projection_fromz = 2000.0;
double projection_depth = 2000; // 500 ??
#endif
double projection_towardx = 0.0;
double projection_towardy = 0.0;
double projection_towardz = 0.0;
double projection_upx = 0.0;
double projection_upy = 1.0;
double projection_upz = 0.0;

double camera_longitude = 0.0;
double camera_latitude = 0.0;
double camera_distance  = 0.0;

int ldraw_image_type = IMAGE_TYPE_BMP8;

// Set the light way up and behind us.  Will this make it too dim?
// NOTE: The LDRAW polys are not CCW compliant so the normals are random
// LdLite uses 2 opposing lights to avoid the problem with normals?
// I attempted this below but it does not seem to work for OpenGL.
// Hmmm, perhaps LdLite just took the fabs() of normals instead.
// If I calculate normals then I could do that too.

// x, y, z, dist divisor.  (divisor = 0 for light at infinite distance)
GLfloat lightposition0[] = { -1000.0, 1000.0, 1000.0, 0.0 };
GLfloat lightposition1[] = { 1000.0, -1000.0, -1000.0, 0.0 };
GLfloat lightcolor0[] =  { 0.5, 0.5, 0.5, 1.0 }; // Half light
GLfloat lightcolor1[] =  { 0.75, 0.75, 0.75, 1.0 }; // bright light

#if 0
GLfloat lightcolorWhite[] =  { 1.0, 1.0, 1.0, 1.0 }; // White light
GLfloat lightcolorBright[] =  { 0.75, 0.75, 0.75, 1.0 }; // bright light
GLfloat lightcolorHalf[] =  { 0.5, 0.5, 0.5, 1.0 }; // Half light
GLfloat lightcolorDim[] =  { 0.25, 0.25, 0.25, 1.0 }; // dim light
#endif

int fogging = 0; // 0 = disabled, 1 = LINEAR, 2 = EXP, 3 = EXP2
GLint fogMode = GL_LINEAR;
GLfloat fogColor[4] = {1.0, 1.0, 1.0, 1.0}; // Fade to white
GLfloat fogDensity = 1.0;
GLfloat fogStart = 0.0;
GLfloat fogEnd = 1.0;

// [Views] swiped from ldraw.ini
// Modified some views to be orthogonal.
char Back[] = "-1,0,0,0,1,0,0,0,-1";
char Left[] = "0,0,1,0,1,0,-1,0,0";
char Right[] = "0,0,-1,0,1,0,1,0,0";
char Above[] = "0,0,1,1,0,0,0,1,0";
char Beneath[] = "0,0,1,-1,0,0,0,-1,0";
char LdrawOblique[] = "1,0,1,0.5,1,-0.5,-1,0,1";
char Oblique[] = "0.707104,0,0.707104,0.353553,0.866025,-0.353553,-0.612372,0.5,0.612372";
char Front[] = "1,0,0,0,1,0,0,0,1";
//char UpsideDown[] = "-1,0,1,-0.5,-1,-0.5,1,0,0";
char UpsideDown[] = "0.707104,0,0.707104,-0.353553,-0.866025,0.353553,0.612372,-0.5,-0.612372";
//char Natural[] = "0.625,0,1.075,0.5375,1.25,-0.3125,-1.25,0,2.5";
char Natural[] = "0.5,0,0.866025,0.433013,0.866025,-0.25,-0.75,0.5,0.433013";
char *m_viewMatrix = LdrawOblique;

// If we separate the ldraw oblique projection from the underlying
// Oblique rotation matrix we get this projection matrix.  Perhaps 
// we can then offer 3 projection types: orthogonal, perspective, oblique.
char ObliqueProjection[] = "1.4142,0,0,0,1.2196,-0.1124,0,-0.7171,1.2247";

//Notes on deriving the Oblique rotation matrix: 
//Rotate 45 degrees and then tilt 30 degrees?
/*
.707104 = sqrt(2) / 2
.353553 = sqrt(2) / 4
.866025 = sin(pi / 3)
.612372 = sqrt(1.5) / 2
*/

extern int glCurColorIndex;
extern float z_line_offset; 

int PolygonOffsetEnabled = 1;
GLfloat PolygonOffsetFactor = 1.0;
GLfloat PolygonOffsetunits = 1.0;

extern ZIMAGE z;

GLint Width = 640;
GLint Height = 480;
GLint XwinPos = 0;
GLint YwinPos = 0;
int main_window = -1;

double twirl_angle = 0.0;
double twirl_increment = 10.0;

static int list_made = 0;

#define USE_DOUBLE_BUFFER

#ifndef AGL
#define USE_OPENGL_STENCIL
#endif

int sc[4];

// Stuff for editing mode
// contents of back buffer after glutSwapBuffers():
#define SWAP_TYPE_UNDEFINED 0 	// unknown
#define SWAP_TYPE_SWAP 1	// former front buffer
#define SWAP_TYPE_COPY 2	// unchanged
#define SWAP_TYPE_NODAMAGE 3	// unchanged even by X expose() events
#define SWAP_TYPE_KTX 4		// use the GL_KTX_buffer_region extension
#define SWAP_TYPE_APPLE 5	// OSX fakes GL_FRONT by drawing in GL_BACK

// Lookup some of these extensions for reference:
// GL_APPLE_flush_render {provides glSwapAPPLE() glFlushRenderAPPLE(), glFinishRenderAPPLE()}
// GLX_SWAP_COPY_OML, GLX_SWAP_METHOD_OML
// GL_WIN_swap_hint
// WGL_SWAP_METHOD_ARB, WGL_SWAP_METHOD_EXT, WGL_SWAP_UNDEFINED_ARB, WGL_SWAP_UNDEFINED_EXT
// WGL_SWAP_COPY_ARB, WGL_SWAP_COPY_EXT, WGL_SWAP_EXCHANGE_ARB, WGL_SWAP_EXCHANGE_EXT
// 

/*
GL_APPLE_flush_render   (Specification pending)
-------------------------------
Normally, in single buffered mode glFlush and glFinish submit the
command stream and copy the resulting image to the screen. This
extension provides glFlushRenderAPPLE and glFinishRenderAPPLE which
just submit pending opengl commands and do not copy the results to the
screen. Also, provides glSwapAPPLE which copies rendered image for the
current context to the screen without needing a context argument and
works in both single and double buffered modes symmetrically.

System: Mac OS X v10.3 and later
Renderers: All 


GL_APPLE_fence   (Specification)
-------------------------------
Provides synchronization primitives that can be inserted into the
OpenGL command stream and later queried for completion.

System: Mac OS X v10.2 "Jaguar" and later
Renderers: All 
*/

// Set default editing mode.
#ifdef MESA
int buffer_swap_mode = SWAP_TYPE_NODAMAGE;
#else
#  ifdef MACOS_X
int buffer_swap_mode = SWAP_TYPE_APPLE;
#  else
int buffer_swap_mode = SWAP_TYPE_UNDEFINED;
#  endif
#endif

int use_stencil_for_XOR = 1;
int NVIDIA_XOR_HACK = 0;
int MESA_3_COLOR_FIX = 0;
int AVOID_FRONT_BUFFER_TEXT = 0;

int show_edit_mode_gui = 1;
int autoscaling = 0;
int editing = 0;
int curpiece = 0;
int curpoint = -1;
int movingpiece = -1;
int StartLineNo = -1;
int DrawToCurPiece = 0;
char editingprevmode = 'C';
int editingkey = -1;
int SOLID_EDIT_MODE = 0;
#define EDIT_LINE_LEN 512
static char eprompt[4][EDIT_LINE_LEN];
static char ecommand[EDIT_LINE_LEN] = "";
static char eresponse[EDIT_LINE_LEN] = "";
float moveXamount = 10.0;
float moveYamount = 8.0;
float moveZamount = 10.0;
float turnCenter[4][4] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int turnAxisVisible = 0;
// staticbuffer is where our non-moving background goes
// screenbuffer is where the final composite goes
int staticbuffer = GL_BACK;
int screenbuffer = GL_FRONT; 
// renderbuffer is where we render to.  Need to know for -ms step saving.
int renderbuffer = GL_FRONT; 
// Buffer pointers and IDs for speedier opengl extension functions.

GLint rBits, gBits, bBits, aBits;
GLint DepthBits = 0;
GLint StencilBits = 0;
GLint ColorAttachments = 0;
GLuint cbuffer_region = 0;
GLuint zbuffer_region = 0;
//static float *zbufdata = NULL;   // NOTE: gotta free when finished editing.
static int *zbufdata = NULL;   // NOTE: gotta free when finished editing.
static char *cbufdata = NULL;   // NOTE: gotta free when finished editing.

/***************************************************************/
extern int Find1PartMatrix(int partnum, float m[4][4]);
extern int Find1Part(int partnum);
extern int Draw1Part(int partnum, int Color);
extern int Get1PartPos(int partnum, float m[4][4]);
extern int Move1Part(int partnum, float m[4][4], int premult);
extern int Rotate1Part(int partnum, float m[4][4]);
extern int Translate1Part(int partnum, float m[4][4]);
extern int Locate1Part(int partnum, float m[4][4], int moveonly);
extern int Color1Part(int partnum, int Color);
extern int Add1Part(int partnum);
extern int Select1Part(int partnum);
extern int UnSelect1Part(int partnum);
extern int Delete1Part(int partnum);
extern int Swap1Part(int partnum, char *SubPartDatName);
extern int Print1Part(int partnum, FILE *f);
extern int Print1Model(char *filename);
extern int Print3Parts(int partnum, char *s1, char *s2, char *s3);
extern int Comment1Part(int partnum, char *Comment);
extern int Switch1Part(int partnum);
extern int Get1PartBox(int partnum, int sc[4]);
extern int Make1Primitive(int partnum, char *str);
extern int GetCurLineType(int partnum);
extern int Inline1Part(int partnum);
extern int DrawTurnAxis(float m[4][4]);

int use_quads = 0;
int curstep = 0;
int cropping = 1;
int panning = 0;
int panlock = 0;
int dirtyWindow = 0;
int pan_start_sx = 0; // Screen Coords
int pan_start_sy = 0;
GLdouble pan_start_x = 0.0;
GLdouble pan_start_y = 0.0;
GLdouble pan_end_x = 0.0;
GLdouble pan_end_y = 0.0;
int pan_start_F;
//int pan_visible = TYPE_F_BBOX_MODE | TYPE_F_NO_POLYGONS; // BBoxWire spin mode.
int pan_visible = TYPE_F_BBOX_MODE | TYPE_F_SHADED_MODE; //BBoxShaded spin mode.
int glutModifiers;
int z_extent_x1;
int z_extent_x2;
int z_extent_y1;
int z_extent_y2;

//#define USE_QUATERNION 1
#ifdef USE_QUATERNION
float qspin[4] = {0.0, 0.0, 1.0, 0.0};
#endif

int drawAxis = 0;
int qualityLines = 0;
float lineWidth = 0.0;
int LineChecking = 0;
int preprintstep = 0;
int dimLevel = 0; // Same as ldraw_commandline_opts.maxlevel=32767;  // a huge number
float dimAmount = 0.0;

int downsample = 0; // decimate output file by 2 with antialias filter.
int upscale = 0;    // upscale everything needed for eventual downsample.

#ifdef TILE_RENDER_OPTION
#include "tr.h"
int tiledRendering = 0;
int TILE_WIDTH = 512;
int TILE_HEIGHT = 512;
int TILE_BORDER = 0;
int TILE_IMAGE_WIDTH = 2000;
int TILE_IMAGE_HEIGHT = 1500;
#endif

int OffScreenRendering = 0;
extern int SetOffScreenRendering();
extern int OffScreenDisplay();
extern int OffScreenRender();

// Opengl implementation details.
char *verstr = "";
char *extstr = "";
char *vendstr = "";
char *rendstr = "";
char *glslverstr = "";

// Camera movement variables
#define MOVE_SPEED 10.0
#define PI 3.1415927
#define PI_180 (PI/180.0)
float fCamX = 0.0;
float fCamY = 0.0;
float fCamZ = 0.0;

float fXRot = 0.0;
float fYRot = 0.0;
float fZRot = 0.0;

void reshape(int width, int height);
void rendersetup(void);
int edit_mode_keyboard(int key, int x, int y);
int edit_mode_fnkeys(int key, int x, int y);
void mouse(int button, int state, int x, int y);
char *stristr(char *src, char *dst);

/***************************************************************/
static int prevlookup = 0;
static char *partlistbuf = NULL;
static int partlookup = 0;
static char **partlistptr = NULL;
static char **partliststr = NULL;
static int partlistsize = 0;
static int partlisttotal = 0;
static int partlistmax = 0;

/***************************************************************/
static char *pluglistbuf = NULL;
static int pluglookup = 0;
static char **pluglistptr = NULL;
static char **plugliststr = NULL;
static int pluglistsize = 0;
static int pluglisttotal = 0;
static int pluglistmax = 0;

#include "plugins.h"

plugstruct **plugins;

/***************************************************************/
char *pastelist = NULL;

/***************************************************************/
char *strrpbrk(const char *szString, const char *szChars)
{
  const char  *p;
  char        *p0, *p1;

  for (p = szChars, p0 = p1 = NULL; p && *p; ++p)
  {
    p1 = strrchr(szString, *p);
    if (p1 && p1 > p0)
      p0 = p1;
  }
  return p0;
}

#ifdef WINDOWS
/***************************************************************/
void pasteMove(int moving)
{
  static int moveCnt = 0;
  static int moveMax = 0;
  static int moveDir = 0;
  int moveKeys[4] = {GLUT_KEY_UP, GLUT_KEY_LEFT, GLUT_KEY_DOWN, GLUT_KEY_RIGHT};

  if (moving <= 0)
  {
    moveCnt = 0;
    moveMax = 0;
    moveDir = -1;
    printf("pasteMove{%d)\n",moving);
    return;
  }

  moveCnt++;

  if (moveCnt > moveMax)
  {
    moveCnt = 0;
    if (moveDir == 1)
      moveMax++;
    else if (moveDir == 3)
      moveMax++;
    moveDir++;
    moveDir &= 3;
  }

  edit_mode_fnkeys(moveKeys[moveDir], 0, 0);
  printf("pasteMove{%d, %d)\n",moving, moveDir);
}

/***************************************************************/
void pasteCommand(int x, int y)
{
  char *str;
  char *dst = &ecommand[1];
  char *whitespace = " \t\r\n";
  char *seps = "\r\n"; // Newline separator chars for DOS, MAC & UNIX.
  char *token;
  char *s;
  char *p;
  int i, n, count;
  int inventory = 0;
  char partstr[255];
  char colorstr[255];
  int color;
  int pastecount = 0;
  float savemove = moveXamount;
	  
  if (pastelist)
  {
    str = pastelist;
    pastelist = NULL;
  }
  else if(OpenClipboard(NULL))
  {
    str = strdup((char*)GetClipboardData(CF_TEXT));
    printf("got <%s> from clipboard\n", str);
    CloseClipboard(); 
  }
  else 
    return;

  // Remove modifier keys before processing the clipboard.
  // (We know for sure that Ctrl-V has GLUT_ACTIVE_CTRL set)
  glutModifiers = 0;

  // Set the move amount to Coarse for a big spiral.
  moveXamount = 100.0; // Coarse movement.
  moveZamount = 100.0;
  moveYamount = 80.0;

  // For partname or filename, do not allow space or tab chars.
  for (i = 0, token = strtok( str, seps );
       token != NULL;
       i++, token = strtok( NULL, seps ))
  {
    color = -1; // Use current color

    printf("got token <%s> from clipboard\n", token);
    
    // token is one line of text.
    if ((ecommand[0] == 'p') || ((i > 0) && (ecommand[0] == 'c')) ||
	(ecommand[0] == 'L') || (ecommand[0] == 'S'))
    {
      // NOTE: Should skip leading white space, trailing white space.
      // Search for first '.' char and grab filename/path around it.
      // Should also allow paste of a full LDRAW type 1 line.
      // Actually should allow paste of many LDRAW lines/files.
      // n = sscanf(token,
      //     "%d %d %f %f %f %f %f %f %f %f %f %f %f %f %s",...)
      // if ((n == 15) && (d == 1))
      //     it's a type 1 LDRAW line.  Keep all info.

      // If working on a part, check for peeron inventory list.
      if ((ecommand[0] == 'p') || ((i > 0) && (ecommand[0] == 'c')))
      {

	//if (strstr(token, "Part # Color Description"))
	if (stristr(token, "Qty") && stristr(token, "PartNum") &&
	    stristr(token, "Color") && stristr(token, "Description"))
	{
	  printf("// Hey, it's an inventory from peeron.com.\n");
	  inventory = 1;
	  token = strtok( NULL, seps); // Move on to the actual inventory.
	  printf("Got token <%s> from clipboard\n", token);
	}
      }

      // Look for '.' and remove trailing, leading whitespace.
      // If no '.' found, focus on the first word.
      if ((!inventory) && (p = strchr(token, '.')))
      {
	// Eliminate trailing whitespace
	if (s = strpbrk(p, whitespace))
	  *s = 0;
	//printf("got trailing <%s> from clipboard\n", token);

	// Eliminate leading whitespace
	*p = 0;
	if (s = strrpbrk(token, whitespace))
	  token = s+1;
	*p = '.';
      }
      else
	token += strspn(token, whitespace); // Eliminate leading whitespace
      //printf("got leading <%s> from clipboard\n", token);
    
      if (!inventory)
      {
	// If not inventory and we still have whitespace, then its a comment.
	if (s = strrpbrk(token, whitespace))
	{
	  if ((i > 0) && (ecommand[0] == 'c'))
	  {
	    edit_mode_keyboard('\n', x, y);
	    edit_mode_keyboard('i', x, y);
	    ecommand[0] == 'p';
	  }
	  if (ecommand[0] == 'p') 
	  {
	    ecommand[0] = 'C';  // Switch to a comment
  	    strcpy(dst, "  ");
  	    strcat(dst, token);
	    continue;
	  }
	}
      }
      else
      {
	n = sscanf(token, "%d", &count);
	//printf("count = %d\n", count);
	p = strpbrk(token, whitespace);
	//printf("got trailing <%s> from clipboard\n", p);
	if ((n == 0) || !p)
	{
	  if ((i > 0) && (ecommand[0] == 'c'))
	  {
	    edit_mode_keyboard('\n', x, y);
	    edit_mode_keyboard('i', x, y);
	  }
	  ecommand[0] = 'C';  // Switch to a comment
	  strcpy(dst, "  ");
	  strcat(dst, token);
	  inventory = 0; // All done with inventory.
	  continue;
	}

	if (strncmp(p, "     ", 4))
	  n = sscanf(p, "   %s    %s", partstr, colorstr); // Found a partname.
	else if (!strncmp(p, "      ", 5))
	{
	  // no part or color on this line.  Sticker sheet or cloth or whatever?
	  sprintf(colorstr, "unknown");
	  strcpy(partstr, p);
	}
	else
	{
	  // No part on this line.  Convert comment into bogus partname.
	  p += strspn(p, whitespace); // Eliminate leading whitespace
	  n = sscanf(p, "%s", colorstr);  // Get the color
	  if (token = strpbrk(p, whitespace))  // Find next whitespace
	    p = token + strspn(token, whitespace); // Eliminate leading whitespace

	  // Eliminate trailing whitespace
	  token = p;
	  for (p += (strlen(token)-1); p >= token; p--)
	  {
	    if ((*p == ' ') || (*p == '\t'))
	      *p = 0;
	    else
	      break;
	  }
	  //printf("filling spaces <%s>\n",token);
	  // Eliminate internal spaces
	  for (p = token; *p; p++)
	    if ((*p == ' ') || (*p == '\t'))
	      *p = '_';
	  //printf("copying token <%s>\n",token);
	  strcpy(partstr, token);
	}
	//printf("setting setting token to part <%s>\n", partstr);
        token = partstr;
	color = zcolor_lookup(colorstr);
	printf("Part = <%s>, color = <%s> = %d\n", token, colorstr, color);
	sprintf(colorstr, "%d", color);
      }
    }

    if (token && strlen(token))
    {
      // If subsequent part, insert last part and start this one.
      if (i > 0)
      {
	edit_mode_keyboard('\n', x, y);
	edit_mode_keyboard('i', x, y);
	pasteMove(pastecount++);
	edit_mode_keyboard('p', x, y);
      }
      else 
	pasteMove(pastecount++);
      strcat(dst, token);

      // Only do more than one if it's a part.
      // NOTE:  Should also do multiple lines for comments.
      if (ecommand[0] != 'p') 
	break;

      if (color >= 0)
      {
	edit_mode_keyboard('\n', x, y);
	edit_mode_keyboard('c', x, y);
	strcat(dst, colorstr);
      }	

      if (inventory)
	for (n = 1; n < count; n++)
	{
	  edit_mode_keyboard('\n', x, y);
	  edit_mode_keyboard('i', x, y);
	  pasteMove(pastecount++);
#if 1
	  edit_mode_keyboard('p', x, y);
	  strcat(dst, token);
	  if (color >= 0)
	  {
	    edit_mode_keyboard('\n', x, y);
	    edit_mode_keyboard('c', x, y);
	    strcat(dst, colorstr);
	  }	
#endif
	}
    }
  }
  
  free(str);
  
  printf("ecommand = <%s>\n", ecommand);
  
  //restore move amount
  if (savemove == 100.0)
    {
      moveXamount = 100.0; // Coarse movement.
      moveZamount = 100.0;
      moveYamount = 80.0;
    }
  else if (savemove == 1.0)
    {
      moveXamount = 1.0; // Fine movement.
      moveZamount = 1.0;
      moveYamount = 1.0;
    }
  else
    {
      moveXamount = 10.0; // Normal movement.
      moveZamount = 10.0;
      moveYamount = 8.0;
    }
    
}
#endif

//---------------MESA TESTING BLOCK----------------------
//#define SIMULATE_MESA 1     
//#define WINTIMER 1
//#define SAVE_COLOR_ALL 1
//#define SAVE_DEPTH_ALL 1
#define MESA_XOR_TEST 1
//---------------MESA TESTING BLOCK----------------------

//---------------OSX MAC TESTING BLOCK----------------------
//#define SIMULATE_APPLE_BUGS 1
//#define WINTIMER 1
//#define SAVE_COLOR_ALL 1
//#define SAVE_DEPTH_ALL 1
//#define MACOS_X_TEST2 1
//---------------OSX MAC TESTING BLOCK----------------------

#ifdef WINTIMER
#include <mmsystem.h>
int starttime, finishtime, elapsedtime;

#pragma comment (lib, "winmm.lib")       /* link with Windows MultiMedia lib */
#endif

#ifdef MACOS_X_TEST2
/***************************************************************/
void SaveColorBuffer(void)
{
#ifdef WINTIMER
  starttime = timeGetTime();
#endif

  {
    glPixelStorei(GL_PACK_ALIGNMENT,1); //4
    glPixelStorei(GL_PACK_ROW_LENGTH,0);
    glPixelStorei(GL_PACK_SKIP_ROWS, 0);
    glPixelStorei(GL_PACK_SKIP_PIXELS, 0);

#ifndef SAVE_COLOR_ALL
    // Gotta figure out the src,dst stuff.  glTranslate()?
    //glRasterPos2i((int)sc[0], (int)sc[1]);
    Get1PartBox(curpiece, sc);
    if (ldraw_commandline_opts.debug_level == 1)
      printf("sbox = %d, %d, %d, %d\n", sc[0], sc[1], sc[2], sc[3]);
    if (cbufdata)
      free (cbufdata);  // NOTE: gotta free this when finished editing.
    cbufdata = (int *) malloc(sc[2] * sc[3] * 4 * sizeof(char));
    glReadBuffer(staticbuffer); // set pixel source
    glReadPixels(sc[0],sc[1],sc[2],sc[3],GL_RGBA,GL_UNSIGNED_BYTE,cbufdata);
#else
#ifndef RESTORE_DEPTH_ALL
    Get1PartBox(curpiece, sc);
    if (ldraw_commandline_opts.debug_level == 1)
      printf("sc_sbox = %d, %d, %d, %d\n", sc[0], sc[1], sc[2], sc[3]);
#endif
    if (cbufdata) // NOTE: gotta free this when finished editing.
    {
      //cbufdata = realloc(zbufdata, Width * Height * sizeof(float));
    }
    else
      cbufdata = (char *) malloc(Width * Height * 4 * sizeof(char));
    glReadBuffer(staticbuffer); // set pixel source
    glReadPixels(0,0,Width,Height,GL_RGBA,GL_UNSIGNED_BYTE,cbufdata);
#endif
}
#ifdef WINTIMER
  finishtime = timeGetTime();
  printf("Save Color Elapsed = %d\n", finishtime-starttime);
#endif
  //NOTE:  I have to reallocate cbufdata whenever we resize the window.
}

/***************************************************************/
void RestoreColorBuffer(void)
{
  int savedirty;

#ifdef WINTIMER
  starttime = timeGetTime();
#endif
  // get fresh copy of static data
  {
    // Gotta fix these later because they get set only once in init().
    //glDisable(GL_COLOR_MATERIAL);
    //glDisable(GL_POLYGON_OFFSET_FILL);
    //glEnable(GL_CULL_FACE);
    //glFrontFace(GL_CW);
    //glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);

    glPixelZoom(1, 1);
    glDisable(GL_STENCIL_TEST);    
    glDisable(GL_FOG);

    glDisable(GL_POLYGON_OFFSET_FILL);
    glDisable(GL_COLOR_MATERIAL);

  /*
   * Disable stuff that's likely to slow down 
   * glDrawPixels.(Omit as much of this as possible, 
   * when you know in advance that the OpenGL state is
   * already set correctly.)
   */
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
        glPixelTransferi(GL_MAP_COLOR, GL_FALSE);
        glPixelTransferi(GL_RED_SCALE, 1);
        glPixelTransferi(GL_RED_BIAS, 0);
        glPixelTransferi(GL_GREEN_SCALE, 1);
        glPixelTransferi(GL_GREEN_BIAS, 0);
        glPixelTransferi(GL_BLUE_SCALE, 1);
        glPixelTransferi(GL_BLUE_BIAS, 0);
        glPixelTransferi(GL_ALPHA_SCALE, 1);
        glPixelTransferi(GL_ALPHA_BIAS, 0);

	glPixelStorei(GL_UNPACK_ALIGNMENT,1); //4
	glPixelStorei(GL_UNPACK_ROW_LENGTH,0);
	glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
	glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);

  // Turn off any smoothing or blending modes.
    glDisable( GL_POINT_SMOOTH ); 
    glDisable(GL_ALPHA_TEST);
    glDisable( GL_LINE_SMOOTH ); 
    glHint( GL_LINE_SMOOTH_HINT, GL_FASTEST ); // GL_NICEST GL_DONT_CARE
    glDisable( GL_BLEND );
    glDisable( GL_POLYGON_SMOOTH ); 
    glHint( GL_POLYGON_SMOOTH_HINT, GL_FASTEST ); // GL_NICEST GL_DONT_CARE
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

    glDisable(GL_LIGHTING);     // Speed up copying
    glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE); //enable color buffer updates
    glDisable( GL_DEPTH_TEST ); 
    glDepthFunc(GL_ALWAYS);

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluOrtho2D(0, Width, 0, Height);
    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();
    glLoadIdentity();
    glRasterPos2i(0, 0);

    glDrawBuffer(screenbuffer); // set pixel destination
#ifndef SAVE_COLOR_ALL
    // Gotta figure out the src,dst stuff.  glTranslate()?
    glRasterPos2i(sc[0], sc[1]);
    if (ldraw_commandline_opts.debug_level == 1)
      printf("bbox = %d, %d, %d, %d\n", sc[0], sc[1], sc[2], sc[3]);
    glDrawPixels(sc[2],sc[3],GL_RGBA,GL_UNSIGNED_BYTE,cbufdata);
#else
#ifdef RESTORE_COLOR_ALL 
	glPixelStorei(GL_UNPACK_ALIGNMENT,1); //4
        glPixelStorei(GL_UNPACK_ROW_LENGTH,0); //Width
        glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
        glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);

    glDrawPixels(Width,Height,GL_RGBA,GL_UNSIGNED_BYTE,cbufdata);
#else
	glPixelStorei(GL_UNPACK_ALIGNMENT,1); //4
        glPixelStorei(GL_UNPACK_ROW_LENGTH,Width); //Width
        glPixelStorei(GL_UNPACK_SKIP_ROWS, sc[1]);
        glPixelStorei(GL_UNPACK_SKIP_PIXELS, sc[0]);
    glRasterPos2i(sc[0], sc[1]);
    if (ldraw_commandline_opts.debug_level == 1)
      printf("bbox = %d, %d, %d, %d\n", sc[0], sc[1], sc[2], sc[3]);
    glDrawPixels(sc[2],sc[3],GL_RGBA,GL_UNSIGNED_BYTE,cbufdata);
        // Set UNPACK back to default for glPolygonStipple()
        glPixelStorei(GL_UNPACK_ROW_LENGTH,0); //Width
        glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
        glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
#endif
#endif

    glEnable(GL_COLOR_MATERIAL);
    if (PolygonOffsetEnabled)
    {
      glEnable(GL_POLYGON_OFFSET_FILL);
    }

    glPopMatrix();
    glEnable( GL_DEPTH_TEST ); 
    glDepthFunc(GL_LESS);
    glDrawBuffer(renderbuffer); // set pixel destination to the render buffer.

    // Reset the projection matrix.
    savedirty = dirtyWindow; 
    reshape(Width, Height);
    dirtyWindow = savedirty; 

    rendersetup();
  }
#ifdef WINTIMER
  finishtime = timeGetTime();
  printf("Restore Color Elapsed = %d\n", finishtime-starttime);
#endif
}
#endif

/***************************************************************/
void CopyColorBuffer(int srcbuffer, int destbuffer)
{
  int savedirty;

#ifdef WINTIMER
  starttime = timeGetTime();
#endif

  if ((srcbuffer == staticbuffer) && (destbuffer == screenbuffer))
  {
    if ((buffer_swap_mode == SWAP_TYPE_COPY)
	|| (buffer_swap_mode == SWAP_TYPE_NODAMAGE)
	|| (buffer_swap_mode == SWAP_TYPE_APPLE) // OSX seems to COPY (according to blender)
	)
    {
      printf("CopyColorBuffer(%s to %s) = glutswapBuffers(mode=%d)\n", 
	     ((srcbuffer==GL_FRONT)? "Front" : "Back"), 
	     ((destbuffer==GL_FRONT)? "Front" : "Back"),
	     buffer_swap_mode);

      glutSwapBuffers(); // Found GL_WIN_swap_hint extension
#ifdef WINTIMER
      finishtime = timeGetTime();
      printf("SwapIn Color Elapsed = %d\n", finishtime-starttime);
#endif
      return;
    }
  }

  printf("CopyColorBuffer(%s to %s)\n", 
	 ((srcbuffer==GL_FRONT)? "Front" : "Back"), 
	 ((destbuffer==GL_FRONT)? "Front" : "Back"));

  glPushAttrib(GL_COLOR_BUFFER_BIT|GL_CURRENT_BIT|GL_DEPTH_BUFFER_BIT|
	       GL_FOG_BIT|GL_LIGHTING_BIT|GL_VIEWPORT_BIT);

  glPixelZoom(1, 1);
  glDisable(GL_STENCIL_TEST);    
  glDisable(GL_FOG);

  glDisable(GL_POLYGON_OFFSET_FILL);
  glDisable(GL_COLOR_MATERIAL);

  glReadBuffer(srcbuffer); // set pixel source
  glDrawBuffer(destbuffer); // set pixel destination

  /*
   * Disable stuff that's likely to slow down 
   * glDrawPixels.(Omit as much of this as possible, 
   * when you know in advance that the OpenGL state is
   * already set correctly.)
   */
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
        glPixelTransferi(GL_MAP_COLOR, GL_FALSE);
        glPixelTransferi(GL_RED_SCALE, 1);
        glPixelTransferi(GL_RED_BIAS, 0);
        glPixelTransferi(GL_GREEN_SCALE, 1);
        glPixelTransferi(GL_GREEN_BIAS, 0);
        glPixelTransferi(GL_BLUE_SCALE, 1);
        glPixelTransferi(GL_BLUE_BIAS, 0);
        glPixelTransferi(GL_ALPHA_SCALE, 1);
        glPixelTransferi(GL_ALPHA_BIAS, 0);


  glDisable( GL_DEPTH_TEST ); // Speed up copying
  glDisable(GL_LIGHTING);     // Speed up copying
  glDisable(GL_BLEND);        // Speed up copying
  glDisable(GL_ALPHA_TEST);   // Speed up copying
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
  glDepthMask(GL_TRUE); // enable updates to depth buffer
  glPopMatrix();
  glEnable(GL_LIGHTING);
  glEnable( GL_DEPTH_TEST ); 
  glDepthFunc(GL_LESS);
  glDrawBuffer(renderbuffer); // set pixel destination to the render buffer.

  glEnable(GL_COLOR_MATERIAL);
  if (PolygonOffsetEnabled)
  {
    glEnable(GL_POLYGON_OFFSET_FILL);
  }

  glPopAttrib();
  
#ifdef WINTIMER
  finishtime = timeGetTime();
  if (srcbuffer == GL_BACK)
    printf("CopyIn Color Elapsed = %d\n", finishtime-starttime);
  else
    printf("CopyOut Color Elapsed = %d\n", finishtime-starttime);
#endif

  savedirty = dirtyWindow; 
  reshape(Width, Height);
  dirtyWindow = savedirty; 
  rendersetup();
}
      
/***************************************************************/
void printModelMat(char *name)
{
  GLdouble model[4*4];

  glGetDoublev(GL_MODELVIEW_MATRIX, model);
  printf("%s(%g,%g,%g,%g, %g,%g,%g,%g %g,%g,%g,%g, %g,%g,%g,%g)\n", name,
	 model[0], model[1] , model[2], model[3],
	 model[4], model[5] , model[6], model[7],
	 model[8], model[9] , model[10], model[11],
	 model[12], model[13] , model[14], model[15]);
}

/***************************************************************/
void parse_view(char *viewMatrix);
void getCamera(float m[4][4], float v[3]);

#include "StdAfx.h" /* For L3Def.h */
#include "L3Def.h" /* For m4v3mul() */

/***************************************************************/
void printPOVMatrix(FILE *f)
{
  float m[4][4] = {
    {1.0,0.0,0.0,0.0},
    {0.0,1.0,0.0,0.0},
    {0.0,0.0,1.0,0.0},
    {0.0,0.0,0.0,1.0}
  };
  float lp[4];
  float lpp[4];
  float bc[4];
  float cc[4];
  float cla[4];
  float sky[4];
  float from[4];
  float toward[4];
  float up[4] = {0, -1, 0, 0}; // up/sky is straight up in ldraw coords.
  double dist[4];
  double angle;
  double scale = 1.0;
  double pct = 0.0;
  int i;
  char orthographicstr[] = "\torthographic\n";
  char perspectivestr[] = "\t//orthographic\n";
  char *projectionstr = orthographicstr;
  UNUSED(lp);
  UNUSED(lpp);

  ZCOLOR zcp, zcs;

  translate_color(ldraw_commandline_opts.B, &zcp, &zcs);
  bc[0] = zcp.r / 255.0;
  bc[1] = zcp.b / 255.0;
  bc[2] = zcp.g / 255.0;
 fprintf(f,"\nbackground { color rgb <%g,%g,%g>}\n", bc[0], bc[1], bc[2]);

  if (1) //(ldraw_projection_type)
  {
    // For the perspective projection we can zoom in/out by scaling the 
    // Camera and look_at points by 1/opts.S.
    scale = ldraw_commandline_opts.S;
  }
  else if (ldraw_commandline_opts.S != 1.0)
  {
    // NOTE: this is not quite right.  PCT scales from the look_at point,
    // but ldraw_commandline_opts.S scales from the origin.
    // Oh well.  Close enough.
    pct = 100.0 * ((1.0/ldraw_commandline_opts.S) - 1.0);
  }

 fprintf(f,"\ncamera {\n");
 fprintf(f,"\t#declare PCT = %g; // Percentage further away\n",pct);
 fprintf(f,"\t#declare STEREO = 0; // Normal view\n");
 fprintf(f,"\t//#declare STEREO =  degrees(atan2(1,12))/2; // Left view\n");
 fprintf(f,"\t//#declare STEREO = -degrees(atan2(1,12))/2; // Right view\n");

  // The LdrawOblique matrix is a projection matrix NOT supported by POV.
  // Substitute Oblique view matrix.
  if (m_viewMatrix == LdrawOblique)
    parse_view(Oblique);

  // Use the TRANSPOSE of the opts.A matrix to rotate view point in the
  // opposite direction than the model would rotate.  This = INVERSE rotation.
  // Also sneak in the opts.S scaling.
  m[0][0] = ldraw_commandline_opts.A.a / scale;
  m[1][0] = ldraw_commandline_opts.A.b / scale;
  m[2][0] = ldraw_commandline_opts.A.c / scale;
  m[0][1] = ldraw_commandline_opts.A.d / scale;
  m[1][1] = ldraw_commandline_opts.A.e / scale;
  m[2][1] = ldraw_commandline_opts.A.f / scale;
  m[0][2] = ldraw_commandline_opts.A.g / scale;
  m[1][2] = ldraw_commandline_opts.A.h / scale;
  m[2][2] = ldraw_commandline_opts.A.i / scale;

  m[0][3] = ldraw_commandline_opts.O.x / scale;
  m[1][3] = ldraw_commandline_opts.O.y / scale;

  // Restore LdrawOblique view matrix if needed.
  if (m_viewMatrix == LdrawOblique)
    parse_view(LdrawOblique);

  from[0] = projection_fromx;
  from[1] = projection_fromy;
  from[2] = projection_fromz;
  from[1] += (Height/6.0);
  from[1] = -from[1]; // Switch to ldraw coords
  from[2] = -from[2]; // Switch to ldraw coords 
  //printf("\t//location <%g,%g,%g>\n",from[0],from[1],from[2]);

  toward[0] = projection_towardx;
  toward[1] = projection_towardy;
  toward[2] = projection_towardz;
  toward[1] += (Height/6.0);
  toward[1] = -toward[1]; // Switch to ldraw coords
  toward[2] = -toward[2]; // Switch to ldraw coords
  //printf("\t//look_at <%g,%g,%g>\n",toward[0],toward[1],toward[2]);

  // Rotate from, toward, and up vectors by inverse of ldraw model matrix.
  M4V3Mul(cc,m,from);
  M4V3Mul(cla,m,toward);
  M4V3Mul(sky,m,up);

  for (i = 0; i < 3; i++)
    dist[i] = cc[i] - cla[i]; // dist[i] = from[i] - toward[i];
  dist[3] = sqrt((dist[0]*dist[0]) + (dist[1]*dist[1]) + (dist[2]*dist[2]));
  dist[3] *= scale; // Undo scaling so angle is calculated correctly.

#if 0
 fprintf(f,"\tlocation <%g,%g,%g>\n",cc[0],cc[1],cc[2]);
#else
 fprintf(f,"\tlocation <%g,%g,%g> +PCT/100.0*<%g,%g,%g>\n",
	 cc[0],cc[1],cc[2], dist[0],dist[1],dist[2]);
  // NOTE: what is the axis to rotate about for l3p STEREO?
  // Is it the perpendicular bisector at the look_from point?
  // figure it out and switch to vaxis_rotate() for location.
#endif
  //printf("\tsky      -y\n");
 fprintf(f,"\tsky <%g,%g,%g>\n",sky[0],sky[1],sky[2]);
 fprintf(f,"\tright    -4/3*x\n");
 fprintf(f,"\tlook_at <%g,%g,%g>\n",cla[0],cla[1],cla[2]);

  if (ldraw_projection_type)
  {
    // POV uses x for Field Of View, GluPerspective() uses y.
    // However, I now convert projection_fov to vertical right before 
    // calling GluPerspective(), so it should be good as is.
    angle = projection_fov; //angle = projection_fov * Width / Height;
    projectionstr = perspectivestr;
  }
  else
  {
    angle = 2.0 * atan(((double)Width / 2.0) / dist[3]);
    angle /= PI_180;
  }

 fprintf(f,"\tangle %g\n", angle);
 fprintf(f,"\trotate   <0,1e-5,0> // Prevent gap between adjecent quads\n");
 fprintf(f,projectionstr);
 fprintf(f,"}\n\n");

#if 0
  // Give the ldglite lighting model a shot.
  for (i = 0; i < 3; i++)
    lp[i] = lightposition0[i];
  lightposition0[1] = -lightposition0[1];  // Switch to ldraw coords
  lightposition0[2] = -lightposition0[2];  // Switch to ldraw coords
  M4V3Mul(lpp,m,lp);
  for (i = 0; i < 3; i++)
  {
    lpp[i] *= 100.0; // Move far away
    lp[i] = lightcolor0[i];
  }
 fprintf(f,"light_source {\n\t<%g,%g,%g>\n\tcolor rgb <%g,%g,%g>\n}\n\n",
	 lpp[0],lpp[1],lpp[2], lp[0],lp[1],lp[2]);
#endif

  {
    char filename[256];
    char *s;
    UNUSED(s);

    concat_path(datfilepath, datfilename, filename);
    if (filename[0] == '.') // I hate the ./filename thing.
      strcpy(filename, datfilename);

   fprintf(f,"l3p -cc%g,%g,%g -cla%g,%g,%g -ca%g %s -b%d\n\n", 
	   cc[0],cc[1],cc[2], cla[0],cla[1],cla[2], angle,
	   filename, ldraw_commandline_opts.B);
  }

  printModelMat("ModelM");
  getCamera(m, up);

 fprintf(f,"%s(%g,%g,%g,%g, %g,%g,%g,%g %g,%g,%g,%g, %g,%g,%g,%g)\n", "Camera",
	 m[0][0], m[0][1] , m[0][2], m[0][3],
	 m[1][0], m[1][1] , m[1][2], m[1][3],
	 m[2][0], m[2][1] , m[2][2], m[2][3],
	 m[3][0], m[3][1] , m[3][2], m[3][3]);
 fprintf(f,"%s(%g,%g,%g)\n", "Offset", up[0], up[1], up[2]);


}

/***************************************************************/
void printLdrawMatrix(FILE *f)
{
  char matrix_string[256];
  char filename[256];
  char *s;

  concat_path(datfilepath, datfilename, filename);
  if (filename[0] == '.') // I hate the ./filename thing.
    strcpy(filename, datfilename);

  s = matrix_string;
  if(ldraw_commandline_opts.B != 15)
  {
    sprintf(s,"-b%d ", ldraw_commandline_opts.B);
    s = matrix_string + strlen(matrix_string);
  }
  if (lineWidth > 1.0)
  {
    sprintf(s,"-w%g ", lineWidth);
    s = matrix_string + strlen(matrix_string);
  }
  if (qualityLines)
  {
    sprintf(s,"-q ");
    s = matrix_string + strlen(matrix_string);
  }
  if (ldraw_projection_type)
  {
    sprintf(s,"-J ");
    s = matrix_string + strlen(matrix_string);
  }
  if (ldraw_commandline_opts.S != 1.0)
  {
    sprintf(s,"-s%g ", ldraw_commandline_opts.S);
    s = matrix_string + strlen(matrix_string);
  }
  if ((ldraw_commandline_opts.O.x != 0.0) ||
      (ldraw_commandline_opts.O.y != 0.0))
  {
    sprintf(s,"-o%g,%g ", ldraw_commandline_opts.O.x, 
	    ldraw_commandline_opts.O.y);
    s = matrix_string + strlen(matrix_string);
  }
  sprintf(s,"-a%g,%g,%g,%g,%g,%g,%g,%g,%g",
	  ldraw_commandline_opts.A.a,
	  ldraw_commandline_opts.A.b,
	  ldraw_commandline_opts.A.c,
	  ldraw_commandline_opts.A.d,
	  ldraw_commandline_opts.A.e,
	  ldraw_commandline_opts.A.f,
	  ldraw_commandline_opts.A.g,
	  ldraw_commandline_opts.A.h,
	  ldraw_commandline_opts.A.i);

 fprintf(f,"%s %s %s\n",progname, matrix_string, filename);
}

/***************************************************************/

//#define USE_GLFONT 1
#ifdef USE_GLFONT
/***************************************************************/
/*
  For speed I want to try using a texture mapped font instead of GLUT.
  This uses the glfont package by Brad Fish.  Details available here:
  http://www.netxs.net/bfish/Glfont.html
  mailto:bfish@netxs.net

  I should probably just pick a font and convert it to C code.
  Then I can just compile it in as arielfnt.c or somesuch name,
  so there will be no problem finding the font at runtime.
*/
/***************************************************************/
#include "glfont.h"

GLFONT Font;
int FontTex = 1;
char *fontname = NULL;
float fontheight = 1.0;
float fontwidth = 1.0;

/***************************************************************/
#endif

#ifdef AGL
/***************************************************************/
// Slim down the program by removing unused allegro drivers.
#include "allegro.h"
BEGIN_DIGI_DRIVER_LIST
END_DIGI_DRIVER_LIST

BEGIN_MIDI_DRIVER_LIST
END_MIDI_DRIVER_LIST

BEGIN_JOYSTICK_DRIVER_LIST
END_JOYSTICK_DRIVER_LIST
#endif

/***************************************************************/
void draw_string(void *font, const char* string) 
{
  while(*string)
    glutStrokeCharacter(font, *string++);
}

/***************************************************************/
void draw_string_bitmap(void *font, const char* string) 
{
  while (*string)
    glutBitmapCharacter(font, *string++);
}

/***************************************************************/
/**
 ** use glut to display a string of characters using a raster font:
 **/
/***************************************************************/
void
DoRasterString( float x, float y, char *s )
{
  char c;			/* one character to print		*/

#ifdef USE_GLFONT
  if (fontname)
  {
    glScalef(fontwidth, fontwidth, 1.0f);

    glEnable(GL_TEXTURE_2D);
    glAlphaFunc(GL_GEQUAL, 0.0625);
    glEnable(GL_ALPHA_TEST);
    glEnable( GL_BLEND );
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glFontBegin (&Font); //Needs to be called before text output
    glFontTextOut (s, x/fontwidth, y/fontwidth+2, 1);
    glFontEnd (); //Needs to be called after text output

    glDisable( GL_BLEND );
    glDisable(GL_ALPHA_TEST);
    glDisable(GL_TEXTURE_2D);
    //glLoadIdentity();
    glScalef(1.0/fontwidth, 1.0/fontwidth, 1.0f);
    return;
  }
#endif
  
  glRasterPos2f( x, y );
  for( ; ( c = *s ) != '\0'; s++ )
  {
#ifndef AGL    
    glutBitmapCharacter( GLUT_BITMAP_HELVETICA_12, c );
#endif
  }
#ifdef AGL    
  //allegro_gl_printf(font, x, y, 1, 0, s);
#endif
}

/***************************************************************/
void
DoMenuString( float x, float y, char *s )
{
  char c;			/* one character to print		*/
  int accelcolor = 0;

#ifdef USE_GLFONT
  char ss[2];

  if (fontname)
  {
    glScalef(fontwidth, fontwidth, 1.0f);

    glEnable(GL_TEXTURE_2D);
    glAlphaFunc(GL_GEQUAL, 0.0625);
    glEnable(GL_ALPHA_TEST);
    glEnable( GL_BLEND );
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glFontBegin (&Font); //Needs to be called before text output
    //glFontTextOut (s, x/fontwidth, y/fontwidth+2, 1);
    ss[1] = 0;
    for( ; ( c = *s ) != '\0'; s++ )
    {
      if (c == '&')
      {
	glColor4f( 0.0, 0.0, 0.5, 1.0 );	/* Current char is dark blue */
	accelcolor = 1;
	continue;
      }
      ss[0] = c; // glutBitmapCharacter()
      glFontTextOut (ss, x/fontwidth, y/fontwidth+2, 1); 
      x += (Font.Char[c-Font.IntStart].dx * fontwidth); // glutBitmapWidth()
      if (accelcolor)
      {
	glColor4f( 0.0, 0.0, 0.0, 1.0 );  // Black
	accelcolor = 0;
      }
    }
    glFontEnd (); //Needs to be called after text output

    glDisable( GL_BLEND );
    glDisable(GL_ALPHA_TEST);
    glDisable(GL_TEXTURE_2D);
    //glLoadIdentity();
    glScalef(1.0/fontwidth, 1.0/fontwidth, 1.0f);

    return;
  }
#endif
  
  glRasterPos2f( x, y );
  for( ; ( c = *s ) != '\0'; s++ )
  {
#ifndef AGL    
    if (c == '&')
    {
      glColor4f( 0.0, 0.0, 0.5, 1.0 );	/* Current char is dark blue */
      glRasterPos2f( x, y );
      accelcolor = 1;
      continue;
    }
    glutBitmapCharacter( GLUT_BITMAP_HELVETICA_12, c );
    x += glutBitmapWidth( GLUT_BITMAP_HELVETICA_12, c );
    if (accelcolor)
    {
      glColor4f( 0.0, 0.0, 0.0, 1.0 );  // Black
      glRasterPos2f( x, y );
      accelcolor = 0;
    }
#endif
  }
#ifdef AGL    
  //allegro_gl_printf(font, x, y, 1, 0, s);
#endif
}

/***************************************************************/
void NewColorPrompt()
{
    int i, c, x, y, w, h;
    char str[] = " 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 19 25 Trans+32";
    char *s;
    ZCOLOR zc, zs;
    int lineheight = 14.0;
    int charwidth = 9.0;

    s = str;
    y = Height-(lineheight*2)-2;
    x = charwidth;
    h = lineheight;
    for(i = -1; ( c = *s ) != '\0'; s++ )
    {
	if (c == ' ')
	{
	    i++;
	    if (i == 16)
		i = 19; // Skip to tan
	    if (i == 20)
		i = 25; // Skip to orange
	    if (i > 25)
		zc.r =  zc.g =  zc.b = 144;
	    else
		translate_color(i,&zc,&zs);
	}
	w = glutBitmapWidth( GLUT_BITMAP_HELVETICA_12, c );
	glColor3ub(zc.r, zc.g, zc.b);
	glBegin(GL_QUADS);
	glVertex2i(x, y);
	glVertex2i(x+w,y);
	glVertex2i(x+w, y+h);
	glVertex2i(x, y+h);
	glEnd();
	if (i == 6)
	    glColor4f( 0.8, 0.8, 0.8, 1.0 );		/* white on brown*/
	else if (i)
	    glColor4f( 0.0, 0.0, 0.0, 1.0 );		/* black  	*/
	else
	    glColor4f( 0.5, 0.5, 0.5, 1.0 );		/* grey  	*/
	glRasterPos2f( x-2, y+2 );
	glutBitmapCharacter( GLUT_BITMAP_HELVETICA_12, c );
	x += w;
    }
}

/***************************************************************/
void clear_edit_mode_gui()
{
  ecommand[0] = 0;
  eprompt[0][0] = 0;
  eprompt[1][0] = 0;
  eprompt[2][0] = 0;
  eprompt[3][0] = 0;

  show_edit_mode_gui |= 2;  // Set the clear gui bit.
}

/***************************************************************/
int edit_mode_gui()
{
  static char eline[4][EDIT_LINE_LEN];
  int savedirty;
  int lineheight = 14.0;
  int charwidth = 9.0;
  char *viewstr;
  char *movestr;

  if (partlookup)
  {
    int i = partlookup - 2; // Subtract 2 to start list with first part red.

    if ((i < partlistsize) && (i >= 0))
      strcpy(eline[1], partlistptr[i]);
    else
      strcpy(eline[1], "");
    i++;
    if (i < partlistsize)
      strcpy(eline[2], partlistptr[i]);
    else
      strcpy(eline[2], "");
    i++;
    if (i < partlistsize)
      strcpy(eline[3], partlistptr[i]);
    else
      strcpy(eline[3], "");

    strcpy(eline[0], eprompt[0]);
    strcat(eline[0], &(ecommand[1]));
  }
  else if (pluglookup)
  {
    int i = pluglookup - 2; // Subtract 2 to start list with first plug red.

    if ((i < pluglistsize) && (i >= 0))
      strcpy(eline[1], pluglistptr[i]);
    else
      strcpy(eline[1], "");
    i++;
    if (i < pluglistsize)
      strcpy(eline[2], pluglistptr[i]);
    else
      strcpy(eline[2], "");
    i++;
    if (i < pluglistsize)
      strcpy(eline[3], pluglistptr[i]);
    else
      strcpy(eline[3], "");

    strcpy(eline[0], eprompt[0]);
    strcat(eline[0], &(ecommand[1]));
  }
  else
  {
  Print3Parts(curpiece, eline[1], eline[2], eline[3]);

  if (strlen(ecommand))
  {
    strcpy(eline[0], eprompt[0]);
    strcat(eline[0], &(ecommand[1]));
    if (strlen(eprompt[1]))
      strcpy(eline[1], eprompt[1]);
  }
  else
  {
    if (m_viewMatrix == Back)
      viewstr = "Back";
    else if (m_viewMatrix == Left)
      viewstr = "Left";
    else if (m_viewMatrix == Right)
      viewstr = "Right";
    else if (m_viewMatrix == Above)
      viewstr = "Above";
    else if (m_viewMatrix == Beneath)
      viewstr = "Beneath";
    else if (m_viewMatrix == Oblique)
      viewstr = "Oblique";
    else if (m_viewMatrix == Front)
      viewstr = "Front";
    else if (m_viewMatrix == UpsideDown)
      viewstr = "UpsideDown";
    else if (m_viewMatrix == Natural)
      viewstr = "Natural";
    else
      viewstr = "Three-D";

    if (moveXamount == 100.0)
      movestr = "Coarse";
    else if (moveXamount == 1.0)
      movestr = "Fine";
    else
      movestr = "Normal";

    sprintf(eline[0],"Line: %d  View: %s  Zoom: x%.3f  Move: %s",
	    curpiece, viewstr, ldraw_commandline_opts.S, movestr);
  }
  }

  // If nothing is happening and hiding the LEDIT GUI just return.
  if ((show_edit_mode_gui == 2) && (ecommand[0] == 0))
  {
    show_edit_mode_gui = 0;
    // NOTE:  I may be able to just copy from the BACK buffer here.
    // Is that what happens when dirtyWindow is 0?
    dirtyWindow = 1;
    glutPostRedisplay();
	// fix osx error: non-void function 'edit_mode_gui' should return a value
    return 0;
  }
  // If nothing is happening and hiding the LEDIT GUI just return.
  else if ((show_edit_mode_gui == 0) && (ecommand[0] == 0))
  {
    printf("glFlush(edit_mode_gui(middle))\n"); glFlush();
	// fix osx error: non-void function 'edit_mode_gui' should return a value
    return 0;
  }
  show_edit_mode_gui &= 1;  // Clear the clear gui bit (2).

  if (AVOID_FRONT_BUFFER_TEXT)
  {
    // Draw into back buffer in editing mode for glutBitmapCharacter() menu speed.
    // MUI renders bitmap text into the back buffer and is fast on RADEON.
    CopyColorBuffer(GL_FRONT, GL_BACK);
    glDrawBuffer(GL_BACK);
  }

  glDisable( GL_DEPTH_TEST ); /* don't test for depth -- just put in front  */
  glDisable(GL_LIGHTING);

  // Turn off any smoothing or blending modes.
  glDisable( GL_POINT_SMOOTH ); 
  glDisable(GL_ALPHA_TEST);
  glDisable( GL_LINE_SMOOTH ); 
  glHint( GL_LINE_SMOOTH_HINT, GL_FASTEST ); // GL_NICEST GL_DONT_CARE
  glDisable( GL_BLEND );
  glDisable( GL_POLYGON_SMOOTH ); 
  glHint( GL_POLYGON_SMOOTH_HINT, GL_FASTEST ); // GL_NICEST GL_DONT_CARE
  glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
  // No need to dither the menu, even if at 8bpp.
  glDisable(GL_DITHER);

  glMatrixMode( GL_PROJECTION );
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D( 0., Width, 0., Height );
  glMatrixMode( GL_MODELVIEW );
  glPushMatrix();
  glLoadIdentity();
  glColor4f( 0.5, 0.5, 0.5, 1.0 );		/* grey  	*/
  glBegin(GL_QUADS);
  glVertex2f(0.0, Height);
  glVertex2f(Width, Height);
  glVertex2f(Width, Height-(lineheight*4.5));
  glVertex2f(0.0, Height-(lineheight*4.5));
  glEnd();
  glColor4f( 0.0, 0.0, 0.0, 1.0 );		/* black  	*/
  if (strlen(ecommand))
  {
    DoMenuString( charwidth, Height - lineheight, eline[0] );
    if (!strncmp(eline[0], "New Color:", 10))
      NewColorPrompt();
    else if (strlen(eprompt[1]))
      DoMenuString( charwidth, Height - lineheight*2.0, eline[1] );
    else
      DoRasterString( charwidth, Height - lineheight*2.0, eline[1] );
  }
  else 
  {
    DoRasterString( charwidth, Height - lineheight, eline[0] );
    DoRasterString( charwidth, Height - lineheight*2.0, eline[1] );
  }
  glColor4f( 0.5, 0.0, 0.0, 1.0 );		/* Current line is dark red */
  DoRasterString( charwidth, Height - lineheight*3.0, eline[2] );
  glColor4f( 0.0, 0.0, 0.0, 1.0 );		/* black  	*/
  DoRasterString( charwidth, Height - lineheight*4.0, eline[3] );
  glPopMatrix();
  glMatrixMode( GL_PROJECTION );
  glPopMatrix();
  glMatrixMode( GL_MODELVIEW );

  glEnable( GL_DEPTH_TEST ); 

  // Reset the projection matrix.
  savedirty = dirtyWindow; 
  reshape(Width, Height);
  dirtyWindow = savedirty; 

  // NOTE: reshape() does NOT seem to prevent a jump when I spin with mouse.
  // I Should probably call glPushAttrib() and glPopAttrib() instead of
  // reshape() and rendersetup().
  rendersetup();

  if (AVOID_FRONT_BUFFER_TEXT)
  {
    {printf("glutSwapBuffers()\n"); glutSwapBuffers();}
    glDrawBuffer(renderbuffer);
  }

  printf("glFlush(edit_mode_gui(bottom))\n"); glFlush();
  return 0;
}

/***************************************************************/
extern FILE *start_bmp(char *filename, int width, int height);
extern void write_bmp(char *filename);
extern FILE *start_ppm(char *filename, int width, int height);
extern void write_ppm(char *filename);
extern void write_targa(char *filename, const GLubyte *buffer, int width, int height);
extern void write_bmp8(char *filename);

#ifdef USE_PNG
#include "png.h"
extern FILE *start_png(char *filename, int width, int height,   
		       png_structp *png_pp, png_infop *info_pp);
extern void write_png(char *filename);
#endif

/***************************************************************/
int platform_write_step_comment(char *comment_string)
{
  int savedirty;

#if 0
  // The reshape() fn seems to hose up the display right before glutSwapBuffers()
  return 0;
#endif

#ifndef ALWAYS_REDRAW
  glEnable( GL_COLOR_LOGIC_OP ); 
  //glEnable( GL_LOGIC_OP_MODE ); 
  glLogicOp(GL_XOR);
#endif

  glMatrixMode( GL_PROJECTION );
  glPushMatrix();
  glLoadIdentity();
#ifdef USE_GLFONT
  if (fontname)
    gluOrtho2D( 0., Width, 0., Height );
  else
#endif
    gluOrtho2D( 0., 100., 0., 100. ); /* "percent units" */
  glMatrixMode( GL_MODELVIEW );
  glPushMatrix();
  glLoadIdentity();
#ifndef ALWAYS_REDRAW
  glColor3f(1.0, 1.0, 1.0); // white
#else
  glColor4f( 0.3, 0.3, 0.3, 0.5 );		/* grey  	*/
#endif
  glDisable( GL_DEPTH_TEST ); /* don't test for depth -- just put in front  */
  DoRasterString( 1.0, 1.0, comment_string );
  glEnable( GL_DEPTH_TEST ); 
  glPopMatrix();

#ifndef ALWAYS_REDRAW
  glLogicOp(GL_COPY);
  glEnable( GL_DEPTH_TEST ); 
  glDisable( GL_COLOR_LOGIC_OP ); 
#endif

  // Reset the projection matrix.
  savedirty = dirtyWindow; 
#if 1
  reshape(Width, Height);
#else
  // The reshape() fn seems to hose up the display right before glutSwapBuffers()
  glMatrixMode( GL_PROJECTION );
  glPopMatrix();
  glMatrixMode( GL_MODELVIEW );
#endif
  dirtyWindow = savedirty; 
  return 0;
}

/***************************************************************/
int platform_step_comment(char *comment_string)
{
  printf("%s\n", comment_string);
  platform_write_step_comment(comment_string);
  return 0;
}

/***************************************************************/
void platform_step_filename(int step, char *filename)
{
  char filepath[256];
  char filenum[32];
  char *dotptr;

  if (picfilename)
  {
    strcpy(filename, picfilename);
    if ((dotptr = strrchr(filename, '#')) != NULL)
    {
      *dotptr = 0;
      if (step != INT_MAX)
      {
	sprintf(filenum,"%0d",step+1);
	strcat(filename,filenum);
      }
      strcat(filename, use_uppercase ? ".BMP" : ".bmp");
    }
    return;
  }
    
#if 0
  concat_path(pathname, use_uppercase ? "BITMAP" : "bitmap", filepath);
#else
  strcpy(filepath, bitmappath);
#endif
  if (ldraw_commandline_opts.rotate == 1) 
  {
    sprintf(filenum,"rot_%0dd",(int)twirl_angle);
    concat_path(filepath, filenum, filename);
#ifdef WINDOWS
    mkdir(filename);
#else
    mkdir(filename,0755);
#endif
    strcpy(filepath, filename);
  }
  concat_path(filepath, datfilename, filename);
  if ((dotptr = strrchr(filename, '.')) != NULL)
    *dotptr = 0;
  if (step != INT_MAX)
  {
    sprintf(filenum,"%0d",step+1);
    strcat(filename,filenum);
  }
  strcat(filename, use_uppercase ? ".BMP" : ".bmp");
}

/***************************************************************/
char * extend_filename(	char *filename)
{
  char *p;

  if ((p = strrchr(filename, '.')) != NULL)
    *p = 0;
  else 
    p = filename + strlen(filename);

#ifdef USE_PNG
  if (ldraw_image_type == IMAGE_TYPE_PNG_RGB)
    strcat(filename, use_uppercase ? ".PNG" : ".png");
  else if (ldraw_image_type == IMAGE_TYPE_PNG_RGBA)
    strcat(filename, use_uppercase ? ".PNG" : ".png");
  else
#endif
  if (ldraw_image_type == IMAGE_TYPE_PPM)
    strcat(filename, use_uppercase ? ".PPM" : ".ppm");
  else
#ifdef USE_BMP8
  if (ldraw_image_type == IMAGE_TYPE_BMP8)
    strcat(filename, use_uppercase ? ".BMP" : ".bmp");
  else
#endif
    strcat(filename, use_uppercase ? ".BMP" : ".bmp");

  return p;
}

/***************************************************************/
// Push, pop extents is used in render for multipass drawing extents.
/***************************************************************/
int p_extent_x1;
int p_extent_x2;
int p_extent_y1;
int p_extent_y2;

/***************************************************************/
void push_extents(void)
{
  // Save the cropping extents from the previous step
  // so I can restore them after zReset().
  p_extent_x1 = z.extent_x1;
  p_extent_x2 = z.extent_x2;
  p_extent_y1 = z.extent_y1;
  p_extent_y2 = z.extent_y2;
}

/***************************************************************/
void pop_extents(void)
{
  // Restore the cropping extents from the previous step if needed.
  z.extent_x1 = min(p_extent_x1, z.extent_x1);
  z.extent_x2 = max(p_extent_x2, z.extent_x2);
  z.extent_y1 = min(p_extent_y1, z.extent_y1);
  z.extent_y2 = max(p_extent_y2, z.extent_y2);
}

/***************************************************************/
// Save, merge extents is used in display for multistep drawing extents.
/***************************************************************/
void save_extents(void)
{
  // Save the cropping extents from the previous step
  // so I can restore them after zReset().
  z_extent_x1 = z.extent_x1;
  z_extent_x2 = z.extent_x2;
  z_extent_y1 = z.extent_y1;
  z_extent_y2 = z.extent_y2;
}

/***************************************************************/
void merge_extents(void)
{
  // Restore the cropping extents from the previous step if needed.
  z.extent_x1 = min(z_extent_x1, z.extent_x1);
  z.extent_x2 = max(z_extent_x2, z.extent_x2);
  z.extent_y1 = min(z_extent_y1, z.extent_y1);
  z.extent_y2 = max(z_extent_y2, z.extent_y2);
}

/***************************************************************/
void platform_step(int step, int level, int pause, ZIMAGE *zp)
{
  char filename[256];

  // Copy substep to front buffer for visual progress.
  // This SLOWS things WAY WAY down!

  if ((zDetailLevel > TYPE_P) && (zDetailLevel < TYPE_MODEL) &&
      (!panning) && (!editing) && (!OffScreenRendering))
  {
    // This adds up at 50 ms per full screen copy at 1280x1024
    // Consider copying only the bounding box of the cur piece.
    CopyColorBuffer(renderbuffer, screenbuffer);
  }

  if (ldraw_commandline_opts.debug_level == 1)
    printf("platform_step(%d, %d, %d) %c\n", step, level, pause,
	   ldraw_commandline_opts.M);

  // This is probably a great place to handle begin & end display lists.
  if (step == INT_MAX) {
    // end of top-level dat file reached
    //platform_step_comment(buf)
    if (ldraw_commandline_opts.debug_level == 1)
      printf("Finished\n");
  } 
  else 	if (step >= 0) {
    //platform_step_commentbuf)
  } 
  else {
    // step == -1 means a redraw partway through a step
  }

  //NOTE:  for some reason (probably -Q) I do this:  zStep(stepcount,0)
  // instead of:  if (ldraw_commandline_opts.output != 1) zStep(INT_MAX, 0);
  // at the end of the render() fn to save/pause after the last step.
  // This makes some of the tests below look different than ldliteView.cpp.

  if ((ldraw_commandline_opts.M != 'C')||(step==INT_MAX)||(step== -1)) { 
  }
  if ((step >= 0 ) && ((ldraw_commandline_opts.M == 'S') || (ldraw_commandline_opts.M == 'F'))) {

    //    printf("Platform_Step(%d, %d, %d, %d, %d) %c\n", 
    //	   step, level, pause, curstep, stepcount,
    //	   ldraw_commandline_opts.M);

    // if something has been drawn, save bitmap
    if ((step == INT_MAX) && (pause == 0)) {
      // do nothing
    } 
    else if (preprintstep) {
      // Early stage of Multistage draw.  Do not save bitmap yet.
    }
    else if (qualityLines && (step != curstep) && (step != INT_MAX)) {
      // qualityLines = Multistage drawing.  Must count steps.
      //	curstep++; // Move on to next step
    }
    else if ((level<=ldraw_commandline_opts.maxlevel) && ((ldraw_commandline_opts.M == 'S')||(step == INT_MAX))) {
      // save bitmap
      platform_step_filename(step, filename);


//*************************************************************************
      // This only happens when keyboard or menu requests a bitmap.
      if ((step == INT_MAX) && (level == 0) && (pause == -1)&& (!picfilename))
      {
	int i = 0;
	char *p = extend_filename(filename);

	while ( (access( filename, 0 )) != -1 )
	{
	  /* The file already exists */
	  i++;
	  sprintf(p,"%0d",i);
	  extend_filename(filename);
	}
      }

//*************************************************************************
//NOTE: Write a fn to calc zp->extents_* by checking zbuf a line at a time.
//        Find min and max x and y coords with modified zbuffer vals.
//        (What is the default zbufer val? 0? infinity?)
//        Use zp->extents in write_xxx() to limit the img to the model.
//      LDLITE calcs zp->extents as it draws the polys and lines but I dont
//        think that will work here since OpenGL does the screen transform.
//*************************************************************************

      if (zp == NULL) // Avoid NULL ptr crash on 'B' when debug printing.
	zp = &z;
      if (ldraw_commandline_opts.debug_level == 1)
	printf("EXTENTS: (%d, %d) -> (%d, %d)\n", 
	       zp->extent_x1, zp->extent_y1, zp->extent_x2, zp->extent_y2);

#ifdef USE_PNG
      if (ldraw_image_type == IMAGE_TYPE_PNG_RGB)
      {
	use_png_alpha = 0;
	write_png(filename);
      }
      else if (ldraw_image_type == IMAGE_TYPE_PNG_RGBA)
      {
	use_png_alpha = 1;
	write_png(filename);
      }
      else
#endif
      if (ldraw_image_type == IMAGE_TYPE_PPM)
	write_ppm(filename);
      else
#ifdef USE_BMP8
      if (ldraw_image_type == IMAGE_TYPE_BMP8)
	write_bmp8(filename);
      else
#endif
	write_bmp(filename);
    }
  }
  if (pause && (ldraw_commandline_opts.M == 'P')&&(step!=INT_MAX))  {
    if (step >= 0) {
      if (ldraw_commandline_opts.debug_level == 1)
	printf("STEP %d level %d pause %d\n", step+1, level, pause);
    }
  }
}

/***************************************************************/
void platform_zDraw(ZIMAGE *zp,void *zDC)
{
  UNUSED(zp);
  UNUSED(zDC);
  if (ldraw_commandline_opts.debug_level == 1)
    printf("zdraw\n");
}

#if defined(UNIX) || defined(MAC) || defined(MACOS_X)
// These should really move to platform.c and platform.h
/***************************************************************/
int GetProfileInt(char *appName, char *appVar, int varDefault)
{
  UNUSED(appName);
  UNUSED(appVar);
  // This should look up appVar in some .INI or .RC file for appName.
  // Windows uses reg key HKCU\Software\Gyugyi Cybernetics\ldlite\Settings
  // Just return the default for now.
  return (varDefault);
}

/***************************************************************/
int GetPrivateProfileInt(char *appName, char *appVar, int varDefault, char *fileName)
{
  UNUSED(appName);
  UNUSED(appVar);
  UNUSED(fileName);
  // Just return the default for now.
  return (varDefault);
}

/***************************************************************/
int GetPrivateProfileString(char *appName, char *appVar, char *varDefault, 
			    char *retString, int strSize, char *fileName)
{
  UNUSED(appName);
  UNUSED(appVar);
  UNUSED(strSize);
  UNUSED(fileName);
  // Just return the default for now.
  strcpy (retString, varDefault);
  return (strlen(varDefault));
}
#endif

/***************************************************************/
int platform_setdir()
{
  extern struct LDrawIniS *LDrawIni;

  char filename[256];
  int i;

  // Redo the ldrawini real dirs whenever we set a new ModelDir.
  // If ModelDir is unknown and model is still found then
  // it's path is *already* being searched and ModelDir gets ".".
  // This means the *current* dir is also searched.  Is that ok?

  // FYI:  LDrawIniComputeRealDirs() expects path *with* model filename.
  concat_path(datfilepath, datfilename, filename);
  i = LDrawIniComputeRealDirs(LDrawIni, 1, 0, filename);
  if (!i){
    printf("Failed to recompute search dirs with ModelDir");
    return(0);
  }

  return(1);
}

/***************************************************************/
void platform_setpath()
{
  char *env_str;

  use_uppercase = 0; // default
  env_str = platform_getenv("LDRAWDIRCASE");
  if (env_str != NULL)
  {
    if (stricmp(env_str, "UPPERCASE") == 0)
      use_uppercase = 1;
  }
  
  env_str = platform_getenv("LDRAWDIR");
  if (env_str != NULL)
  {
    strcpy(pathname, env_str);
  }
  else if (GetPrivateProfileString("LDraw","BaseDirectory","",
			  pathname,256,"ldraw.ini") == 0)
  {
#if defined MACOS_X
    sprintf(pathname, "/Library/ldraw");
#elif defined(UNIX)
    sprintf(pathname, "/usr/local/ldraw");
#elif defined(MAC)
    sprintf(pathname, "Aulus:Code:Lego.CAD:LDRAW");
#elif defined(WINDOWS)
    sprintf(pathname, "c:/ldraw/");
#else
#error unspecified platform in platform_getenv() definition
#endif
  }

  // Get search directories from environment
  int *ErrorCode;
  GetLDrawSearchDirs(ErrorCode);
  //printf("GetLDrawSearchDirs(%d)\n",ErrorCode);

  concat_path(pathname, use_uppercase ? "P" : "p", primitivepath);
  concat_path(pathname, use_uppercase ? "PARTS" : "parts", partspath);

  // Give the user a chance to override some paths for personal data.
  env_str = platform_getenv("LDRAWUSER");
  if (env_str != NULL)
  {
    strcpy(userpath, env_str);
  }
  else if (GetPrivateProfileString("LDraw","UserDirectory","",
			  userpath,256,"ldraw.ini") == 0)
  {
    strcpy(userpath, pathname);
  }

  concat_path(userpath, use_uppercase ? "MODELS" : "models", modelspath);
  concat_path(userpath, use_uppercase ? "BITMAP" : "bitmap", bitmappath);
}

/***************************************************************/
void platform_sethome()
{
  char *env_str;
  char newpath[256];

  // If no filename and not using stdin (--) go home?
  if (!strcmp(datfilename,  " "))
  {
    // if LDRAWUSER != LDRAWDIR, go to MODELS dir in LDRAWUSER path.
    if (strcmp(userpath, pathname))
    {
      printf("chdir(%s)\n", modelspath);
      chdir(modelspath);
    }
    else if (env_str = platform_getenv("USERPROFILE"))
    {
      concat_path(env_str, use_uppercase ? "MY DOCUMENTS" : "My Documents", newpath);
      printf("chdir(%s)\n", newpath);
      chdir(newpath);
    }
    else if (env_str = platform_getenv("HOME"))
    {
      printf("chdir(%s)\n", env_str);
      chdir(env_str);
    }
  }
}

/***************************************************************/
LDrawIniBoolT platform_fixcase(char *path_str)
{
  int i;

  if (use_uppercase)
    for(i=0; i<strlen(path_str); i++) 
      path_str[i] = toupper(path_str[i]);
  else
    for(i=0; i<strlen(path_str); i++) 
      path_str[i] = tolower(path_str[i]);

  return 1;
}

/***************************************************************/
#ifndef WINDOWS
int _strlwr(char *str)
{
  int i;

  for(i=0; i<strlen(str); i++) 
    str[i] = tolower(str[i]);
}
#endif

/***************************************************************/
/*  Global vars for GLUT event handlers
/***************************************************************/
GLfloat zoom = 0.0;

GLboolean selection     = GL_FALSE;	/* rendering to selection buffer */

#define SELECT_BUFFER 32
GLuint select_buffer[SELECT_BUFFER];	/* selection buffer */

GLint mouse_state = -1;
GLint mouse_button = -1;

// 8 random 20% stipple patterns generated with rand()
GLubyte stips[8][4*32] = {
  {
  0x04, 0x86, 0x00, 0x10, 0x20, 0x00, 0x00, 0x30, 
  0xC2, 0x06, 0x10, 0x00, 0x10, 0x41, 0x02, 0x29, 
  0x46, 0x80, 0x00, 0xC9, 0x20, 0x11, 0x00, 0x02, 
  0x08, 0x0A, 0x80, 0x01, 0x09, 0x00, 0x22, 0x10, 
  0x02, 0x00, 0x1A, 0x48, 0x01, 0x80, 0x61, 0x00, 
  0x10, 0x09, 0x30, 0x04, 0x80, 0x01, 0x0C, 0x10, 
  0x01, 0x04, 0x51, 0x10, 0xA0, 0x40, 0x39, 0xB9, 
  0x04, 0x00, 0xAA, 0x01, 0x22, 0x04, 0x96, 0xC0, 
  0x88, 0x48, 0x04, 0x8C, 0x49, 0x01, 0x00, 0x42, 
  0x04, 0x00, 0x09, 0x54, 0x0A, 0x40, 0x64, 0x00, 
  0x4A, 0xA4, 0x19, 0x49, 0x08, 0x40, 0x08, 0x88, 
  0x04, 0x48, 0x80, 0x03, 0x30, 0x90, 0x13, 0x08, 
  0x1A, 0x00, 0x04, 0x19, 0x0A, 0x40, 0x0C, 0x09, 
  0x40, 0x0A, 0xC0, 0x8A, 0x01, 0x00, 0x60, 0x06, 
  0x00, 0x01, 0x88, 0x40, 0x20, 0xC4, 0x83, 0x99, 
  0x02, 0x10, 0x14, 0x94, 0x10, 0x10, 0x20, 0x10, 
  },
  {
  0x02, 0x40, 0x08, 0x01, 0x08, 0x00, 0x40, 0x50, 
  0x70, 0x00, 0xE5, 0x0C, 0x80, 0xCD, 0x80, 0x1A, 
  0x80, 0x41, 0x04, 0x10, 0x70, 0x03, 0x03, 0x88, 
  0x08, 0x03, 0x16, 0x00, 0x01, 0x12, 0x00, 0x80, 
  0x18, 0x00, 0x00, 0x14, 0x24, 0x08, 0x84, 0x08, 
  0x11, 0x42, 0x04, 0x44, 0x00, 0xC7, 0xD0, 0x00, 
  0x20, 0x48, 0x40, 0x89, 0x04, 0xAA, 0x09, 0x88, 
  0x00, 0x82, 0x00, 0x01, 0x18, 0x21, 0x41, 0x01, 
  0x11, 0x04, 0x04, 0x00, 0x40, 0x02, 0x69, 0x90, 
  0x4B, 0x80, 0x08, 0x00, 0x12, 0x10, 0x10, 0x01, 
  0x41, 0x22, 0x24, 0x14, 0x50, 0x0C, 0x80, 0x00, 
  0x01, 0x65, 0x16, 0x08, 0x08, 0x10, 0x02, 0x40, 
  0x0C, 0x30, 0x40, 0xC0, 0x6E, 0xE0, 0x0A, 0x23, 
  0xAC, 0xA0, 0x08, 0x80, 0x53, 0x02, 0x20, 0xC0, 
  0x40, 0x55, 0x42, 0x48, 0x05, 0x04, 0x20, 0x00, 
  0x1D, 0x06, 0x48, 0x88, 0xC8, 0x22, 0x21, 0x41, 
  },
  {
  0x00, 0x02, 0x0C, 0x43, 0x08, 0x02, 0x09, 0x02, 
  0x00, 0x00, 0x54, 0x29, 0x8A, 0x02, 0x80, 0x10, 
  0x09, 0x83, 0x24, 0x0A, 0x01, 0xC4, 0x00, 0x30, 
  0x05, 0x40, 0x88, 0x00, 0x80, 0x22, 0x8C, 0x60, 
  0x00, 0x04, 0x21, 0x32, 0x88, 0x20, 0x28, 0x48, 
  0x91, 0x00, 0x33, 0x14, 0x20, 0x89, 0x01, 0x20, 
  0x00, 0x04, 0x20, 0x00, 0x48, 0x10, 0x90, 0x60, 
  0x81, 0x0B, 0x24, 0x78, 0x86, 0x50, 0x2E, 0x08, 
  0x10, 0x29, 0x02, 0x01, 0x40, 0x42, 0x00, 0x00, 
  0xC4, 0x08, 0x80, 0x02, 0x53, 0x10, 0x05, 0x00, 
  0x90, 0x00, 0x04, 0x00, 0x20, 0x05, 0x04, 0x04, 
  0x90, 0x01, 0x02, 0x20, 0x0C, 0x02, 0x40, 0x28, 
  0x20, 0x10, 0x10, 0x20, 0x01, 0x01, 0x0B, 0x08, 
  0x40, 0x00, 0x40, 0x84, 0x00, 0xA0, 0x08, 0x21, 
  0xF2, 0x03, 0x04, 0x88, 0x20, 0x00, 0x98, 0x00, 
  0x18, 0x10, 0x08, 0x84, 0x04, 0x09, 0x08, 0x04, 
  },
  {
  0x90, 0xD0, 0x58, 0x80, 0xC8, 0xAA, 0x10, 0x80, 
  0x01, 0x05, 0x0C, 0x00, 0x08, 0x08, 0x92, 0x29, 
  0x1B, 0x21, 0x0C, 0x04, 0x08, 0x00, 0x82, 0x28, 
  0x50, 0x85, 0x02, 0x80, 0x19, 0x88, 0x08, 0xD2, 
  0x00, 0x88, 0x00, 0x40, 0x00, 0x00, 0x00, 0x24, 
  0x20, 0x40, 0x00, 0x00, 0x80, 0x50, 0x80, 0x00, 
  0x05, 0x00, 0x44, 0x48, 0xA4, 0x40, 0x80, 0x23, 
  0xC0, 0x20, 0x10, 0x00, 0xC0, 0x80, 0x00, 0x6B, 
  0x92, 0x12, 0x0D, 0x04, 0x20, 0x04, 0x80, 0x5C, 
  0x00, 0x02, 0x42, 0x08, 0xA2, 0x28, 0x04, 0x42, 
  0x10, 0x80, 0x05, 0x80, 0x0F, 0x54, 0x10, 0xC3, 
  0x00, 0x0A, 0x01, 0x12, 0x80, 0x30, 0x10, 0x20, 
  0x00, 0x80, 0x04, 0x08, 0x00, 0x00, 0x41, 0x0A, 
  0x00, 0xA0, 0x00, 0x21, 0x22, 0x88, 0x10, 0xB0, 
  0x05, 0xB9, 0x89, 0x10, 0x34, 0x30, 0x4B, 0x00, 
  0x05, 0x00, 0x10, 0x74, 0xC1, 0x01, 0x07, 0x00, 
  },
  {
  0x60, 0x14, 0x00, 0x04, 0x04, 0x48, 0x20, 0x21, 
  0xE4, 0x08, 0x42, 0x00, 0x01, 0x06, 0x03, 0x40, 
  0x5B, 0x20, 0x00, 0x04, 0x91, 0x25, 0x54, 0x43, 
  0x02, 0x01, 0x04, 0x93, 0x91, 0x81, 0x02, 0x36, 
  0x30, 0x20, 0x44, 0x08, 0x00, 0x19, 0x00, 0x00, 
  0x53, 0x80, 0x01, 0x42, 0x08, 0x01, 0x82, 0x08, 
  0xC1, 0x05, 0x01, 0x01, 0x1C, 0x44, 0x20, 0x83, 
  0x00, 0x01, 0x00, 0x08, 0x20, 0x10, 0x00, 0x28, 
  0x01, 0xC0, 0xA2, 0x00, 0x20, 0x00, 0x00, 0x80, 
  0x00, 0xC1, 0x40, 0x22, 0x00, 0x84, 0x09, 0x00, 
  0x06, 0x89, 0x33, 0x90, 0x08, 0x20, 0x00, 0x84, 
  0x60, 0x01, 0x0A, 0x34, 0x87, 0x08, 0x0A, 0x01, 
  0x1A, 0x02, 0x03, 0x14, 0x10, 0x64, 0x00, 0x4A, 
  0xC2, 0x44, 0x00, 0x04, 0x80, 0x82, 0x90, 0x30, 
  0x18, 0x02, 0x8A, 0xA1, 0x84, 0x00, 0x20, 0x08, 
  0x48, 0xD0, 0x81, 0x55, 0x01, 0xBA, 0x02, 0x40, 
  },
  {
  0x40, 0x1E, 0x00, 0x06, 0x13, 0x03, 0x45, 0x22, 
  0x50, 0x10, 0x22, 0x00, 0x12, 0x06, 0x0E, 0x01, 
  0x02, 0x08, 0xC0, 0x14, 0x34, 0x54, 0x0C, 0x0E, 
  0x80, 0x08, 0x00, 0x45, 0xA2, 0x20, 0x10, 0x98, 
  0x20, 0x7A, 0x41, 0x92, 0x48, 0x88, 0x03, 0xC1, 
  0x51, 0xB0, 0x22, 0x0A, 0x04, 0x08, 0x16, 0x02, 
  0x12, 0x00, 0x08, 0x10, 0x00, 0x80, 0x04, 0x25, 
  0x80, 0x05, 0x02, 0x00, 0x04, 0x20, 0xC1, 0x4B, 
  0x0C, 0x22, 0x00, 0x28, 0x14, 0x20, 0x44, 0x42, 
  0x20, 0x00, 0x00, 0xC4, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x22, 0x80, 0x50, 0xA4, 0x10, 0x84, 0x02, 
  0x81, 0x24, 0xC9, 0xC2, 0x50, 0x48, 0x20, 0x88, 
  0x12, 0x14, 0x82, 0x00, 0x51, 0x24, 0x05, 0x80, 
  0x02, 0xA0, 0x0C, 0x08, 0x82, 0x04, 0x20, 0x80, 
  0x10, 0x0C, 0x20, 0x12, 0x01, 0x02, 0x14, 0x00, 
  0x26, 0x12, 0x50, 0x80, 0xC4, 0x02, 0x11, 0x49, 
  },
  {
  0x00, 0xE0, 0x04, 0x18, 0x00, 0x00, 0x00, 0x82, 
  0x21, 0x04, 0x08, 0x20, 0x03, 0x02, 0x12, 0x02, 
  0x08, 0x04, 0x48, 0x00, 0x21, 0x64, 0x06, 0x08, 
  0x02, 0x30, 0x4C, 0x00, 0x01, 0x00, 0x01, 0x09, 
  0x40, 0x11, 0x04, 0x28, 0x04, 0x28, 0x70, 0x14, 
  0x28, 0x00, 0x28, 0xC1, 0x60, 0x00, 0x00, 0x51, 
  0x52, 0x00, 0xAC, 0x88, 0x24, 0x80, 0x08, 0x42, 
  0x00, 0x12, 0x40, 0x00, 0x05, 0x20, 0x04, 0x01, 
  0x00, 0xAC, 0x40, 0x01, 0x01, 0x63, 0x80, 0x2D, 
  0x18, 0x80, 0x60, 0x09, 0x0C, 0x01, 0xD2, 0x02, 
  0x18, 0x62, 0x00, 0x41, 0x91, 0x20, 0x4C, 0x20, 
  0xC2, 0x30, 0x00, 0x1C, 0x10, 0x00, 0xE8, 0x08, 
  0x88, 0x00, 0x40, 0x55, 0x00, 0xD8, 0x81, 0x86, 
  0x80, 0x28, 0x21, 0x00, 0x23, 0x08, 0x00, 0x0E, 
  0xC0, 0x02, 0x48, 0x30, 0x06, 0x08, 0x14, 0x44, 
  0x60, 0x88, 0x70, 0x0C, 0x88, 0x89, 0x84, 0x82, 
  },
  {
  0xA0, 0x56, 0x10, 0x84, 0x2A, 0x44, 0x4A, 0x84, 
  0x8E, 0x04, 0x82, 0xA0, 0x00, 0x2D, 0x90, 0x03, 
  0x02, 0x02, 0x08, 0x33, 0x8A, 0x10, 0x94, 0x16, 
  0x23, 0x04, 0x00, 0x10, 0x00, 0x11, 0x08, 0x08, 
  0x90, 0x01, 0x01, 0x8D, 0x05, 0x40, 0x02, 0x00, 
  0x21, 0xA0, 0x70, 0x02, 0x40, 0x00, 0x00, 0x80, 
  0x88, 0x2E, 0x0B, 0x44, 0x08, 0x00, 0x13, 0x44, 
  0x28, 0x22, 0x54, 0x04, 0x06, 0x08, 0x48, 0x01, 
  0x00, 0x09, 0x40, 0x30, 0xC0, 0x81, 0x80, 0x00, 
  0x20, 0x00, 0x00, 0x09, 0x82, 0x18, 0x00, 0x10, 
  0x10, 0x02, 0x14, 0x01, 0x00, 0x11, 0xC2, 0x01, 
  0x04, 0x00, 0x08, 0x88, 0x80, 0x88, 0x81, 0x8D, 
  0x80, 0x40, 0x12, 0x51, 0x08, 0x00, 0x80, 0x80, 
  0x00, 0x00, 0x80, 0x04, 0x40, 0x80, 0x5A, 0x58, 
  0x0C, 0x04, 0xB9, 0x00, 0xB0, 0x41, 0xA2, 0x24, 
  0x00, 0x00, 0x00, 0x29, 0x00, 0x00, 0x48, 0x80, 
  },
};

// Ambient and diffusion properties for front and back faces.
// Full ambient and diffusion for R, G, B, alpha ???
GLfloat full_mat[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat half_mat[] = { 0.5, 0.5, 0.5, 1.0 };
GLfloat no_mat[] = { 0.0, 0.0, 0.0, 1.0 };
GLfloat no_shininess[] = { 1.0 };
GLfloat lo_shininess[] = { 5.0 };
GLfloat mid_shininess[] = { 15.0 }; // Seems nice for plastic chrome and gold.
GLfloat hi_shininess[] = { 50.0 }; // { 100.0 };

/***************************************************************/
void linequalitysetup()
{
  int zSolid = ldraw_commandline_opts.F & TYPE_F_NO_LINES;

  //NOTE: This only works well if I draw all the polys first, 
  // then antialias the lines on top of them in a 2nd pass.
  if (qualityLines && !zSolid)
  {

    //TESTING! TESTING! TESTING! TESTING! TESTING! TESTING! TESTING! 
    if ((ldraw_image_type == IMAGE_TYPE_PNG_RGBA) && 
	((ldraw_commandline_opts.M == 'S') || (ldraw_commandline_opts.M == 'F')))
      glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_FALSE); //disable alpha updates
    // This will clip anything blended with the background when saved
    // as a transparent PNG.  Sharp outside edges, but no halo effect.
    // The other cheap alternative is to use -b0 for a dark halo,
    // which is less disturbing on all backgrounds that a white halo.
    //TESTING! TESTING! TESTING! TESTING! TESTING! TESTING! TESTING! 

    glEnable( GL_LINE_SMOOTH ); 
    glHint( GL_LINE_SMOOTH_HINT, GL_NICEST ); // GL_FASTEST GL_DONT_CARE
    glEnable( GL_BLEND );
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // NOTE: I need to saturate the DST alpha somehow.  
    // This should do it, but turns all edges black
    // because when Ad is already 1, min(As,1-Ad) is 0 and not As.  Nuts!
    //glBlendFunc(GL_SRC_ALPHA_SATURATE, GL_ONE_MINUS_SRC_ALPHA);
    // Also smooth edges of printed polygons.
    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    //glEnable( GL_POLYGON_SMOOTH ); 
    //glHint( GL_POLYGON_SMOOTH_HINT, GL_NICEST ); // GL_FASTEST GL_DONT_CARE
#if 1
    if (lineWidth > 1.0)
    {
      // This does NOT seem to work as well as I thought it would.
      // Instead, just draw the points (unantialiased) BEFORE the lines.
      glEnable( GL_POINT_SMOOTH ); 
      glHint( GL_POINT_SMOOTH_HINT, GL_NICEST ); // GL_FASTEST GL_DONT_CARE
      glEnable(GL_ALPHA_TEST);
      glAlphaFunc(GL_GREATER,.5);
    }
#endif
#ifdef LEQUAL_TESTING
    glDepthFunc(GL_LEQUAL);  // Let antialiased lines blend together.
#endif
  }
  else
  {
    //TESTING! TESTING! TESTING! TESTING! TESTING! TESTING! TESTING! 
    if ((ldraw_image_type == IMAGE_TYPE_PNG_RGBA) && 
	((ldraw_commandline_opts.M == 'S') || (ldraw_commandline_opts.M == 'F')))
      glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE); //enable alpha updates
    //TESTING! TESTING! TESTING! TESTING! TESTING! TESTING! TESTING! 

#ifdef LEQUAL_TESTING
    // Watch out!  There is one place where I use GL_EQUAL.
    glDepthFunc(GL_LESS);
#endif
#if 1
    if (lineWidth > 1.0)
    {
      glDisable( GL_POINT_SMOOTH ); 
      glDisable(GL_ALPHA_TEST);
    }
#endif    
    glDisable( GL_LINE_SMOOTH ); 
    glHint( GL_LINE_SMOOTH_HINT, GL_FASTEST ); // GL_NICEST GL_DONT_CARE
    glDisable( GL_BLEND );
    // Return from smoothing edges of printed polygons.
    //glDisable( GL_POLYGON_SMOOTH ); 
    //glHint( GL_POLYGON_SMOOTH_HINT, GL_FASTEST ); // GL_NICEST GL_DONT_CARE
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
  }

  //NOTE:  I should draw round points at the ends of all lines (in stub.c).
  // Otherwise I end up with flat ends (ugly at angled intersections).
  // Apparently OpenGL has no line end style parameter?
  if (lineWidth > 1.0)
  {
    glLineWidth( lineWidth );
    glPointSize( lineWidth -0.5);
  }
  else
  {
    glLineWidth( 1.0 );
    glPointSize( 1.0 );
  }
}

/***************************************************************/
// Moved this out of colormenu, glutPostRedisplay crashes offscreen render.
void setBackgroundColor(int c)
{
  ZCOLOR zc, zs;

  extern ZCOLOR_DEF_TABLE_ENTRY zcolor_table_default[];
  extern ZCOLOR_TABLE_ENTRY *zcolor_table;

  ldraw_commandline_opts.B = c;

#ifdef USE_L3_PARSER
  if ((parsername == L3_PARSER) && (zcolor_table))
    translate_color(c,&zc,&zs);
  else
#endif
  if ((c>=0) && (c<ZCOLOR_TABLE_SIZE))
  {
    // NOTE:  I dont understand the colortable stack used in translate_color()
    // so use the default color_table to avoid bad colors (bad stack ptr?).
    zc.r = zcolor_table_default[c].primary.r;
    zc.b = zcolor_table_default[c].primary.b;
    zc.g = zcolor_table_default[c].primary.g;
  }
  else
    translate_color(c,&zc,&zs);
  glClearColor(((float)zc.r)/255.0,((float)zc.g)/255.0,((float)zc.b)/255.0,0.0);
  if (ldraw_commandline_opts.debug_level == 1)
    printf("clearcolor %d = (%d, %d, %d)\n", c, zc.r, zc.g, zc.b);
}

/***************************************************************/
void colormenu(int c)
{
  setBackgroundColor(c);
  dirtyWindow = 1;  // Do not increment step counter
  glutPostRedisplay();
}

/***************************************************************/
void init(void)
{
  int i;
  UNUSED(i);

    // glSelectBuffer(SELECT_BUFFER, select_buffer);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    if (1) //(zShading)
    {
      glEnable(GL_LIGHT0);
      glLightfv(GL_LIGHT0, GL_POSITION, lightposition0);
      glLightfv(GL_LIGHT0, GL_DIFFUSE, lightcolor0);
      //glLightfv(GL_LIGHT0, GL_AMBIENT, lightcolor1);
#ifdef USE_TWO_SPOT_LIGHTS
      glEnable(GL_LIGHT1);
      glLightfv(GL_LIGHT1, GL_POSITION, lightposition1);
      glLightfv(GL_LIGHT1, GL_DIFFUSE, lightcolor1);
      glLightfv(GL_LIGHT1, GL_AMBIENT, lightcolor1);
#endif
      glEnable(GL_LIGHTING);

      // reverse the normals for back face polys and light them
      // Unfortunately this still doesnt seem the same as front faces.

      // Bright ambient light for the whole scene.
      glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lightcolor1);
#if 1
      //glFrontFace(GL_CCW);
      glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
#else
      glFrontFace(GL_CW);
      glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
#endif

#if 1 
      /* 
      OpenGL's color material feature provides a less expensive way to change
      material parameters. With color material enabled, material colors track
      the current color. This means that instead of using the relatively
      expensive glMaterialfv routine, you can use the glColor3f routine. 
      */
      glEnable(GL_COLOR_MATERIAL);
      glColorMaterial(GL_FRONT_AND_BACK, GL_SPECULAR);
      glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
      glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, full_mat);
      glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, no_mat);
      glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, no_shininess);
      glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, no_mat);
#else
      glDisable(GL_COLOR_MATERIAL);
#endif
    }
      
    if (ldraw_commandline_opts.F & TYPE_F_SHADED_MODE) // (zShading)
      glEnable(GL_LIGHTING);
    else
      glDisable(GL_LIGHTING);

    // glEnable(GL_CULL_FACE);
    // Disable backface culling since the LDRAW parts arent BFC compliant.
    glDisable(GL_CULL_FACE);

    // Set the background to white like ldlite.
    glClearColor(1.0, 1.0, 1.0, 0.0);
    setBackgroundColor(ldraw_commandline_opts.B);

    // Make lines a wider than a pixel so they are not hidden by surfaces.
    // (Not needed because PolygonOffset works so much better)
    //glLineWidth(1.25);

    // Nudge back the zbuffer values of surfaces so the hilited edges show.
    if (PolygonOffsetEnabled)
    {
      glEnable(GL_POLYGON_OFFSET_FILL);
      glPolygonOffset(PolygonOffsetFactor, PolygonOffsetunits);
    }

    linequalitysetup();

#if 0
    // Make command lists for the eight stipple patterns.
#define STIP_OFFSET 100
    for (i = 0; i < 8; i++)
    {
      glNewList(i+STIP_OFFSET, GL_COMPILE);
      glPolygonStipple(&stips[i][0]);
      glEndList();
    }
#endif

#if 0
    // NOTE:  I could achieve L3Lab speeds if I delay rendering studs till last
    // then do occlusion tests on the bounding boxes before rendering.

    // disable updates to color and depth buffer (optional)
    glDepthMask(GL_FALSE);
    glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);
    // enable occlusion test
    glEnable(GL_OCCLUSION_TEST_HP);
    // render bounding geometry
    // gl rendering calls
    // disable occlusion test
    glDisable(GL_OCCLUSION_TEST_HP);
    // enable updates to color and depth buffer
    glDepthMask(GL_TRUE);
    glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
    // read occlusion test result
    glGetBooleanv(GL_OCCLUSION_TEST_RESULT_HP, &result);
    // if (result) render internal geometry
    // else don't render
#endif
}

/***************************************************************/
/*  GLUT event handlers
/***************************************************************/
int exposeEvent(void)
{
#ifdef MACOS_X_TEST1
  return 0;
#endif
#ifdef MACOS_X_TEST2
  return 0;
#endif
  // Check if we had a window expose event lately.
  return glutLayerGet(GLUT_NORMAL_DAMAGED);
}

/***************************************************************/
void visibility(int state)
{
  switch (state)
  {
  case GLUT_HIDDEN:
    printf("visibility = GLUT_HIDDEN\n");
    break;
  case GLUT_FULLY_COVERED:
    printf("visibility = GLUT_FULLY_COVERED\n");
    break;
  case GLUT_FULLY_RETAINED:
    printf("visibility = GLUT_FULLY_RETAINED\n");
    break;
  case GLUT_PARTIALLY_RETAINED:
    printf("visibility = GLUT_PARTIALLY_RETAINED\n");
    break;
  default:
    printf("visibility = UNKNOWN\n");
    break;
  }
}

/***************************************************************/
void VISIBILITY(int state)
{
  switch (state)
  {
  case GLUT_VISIBLE:
    printf("VISIBILITY = GLUT_VISIBLE\n");
    break;
  case GLUT_NOT_VISIBLE:
    printf("VISIBILITY = GLUT_NOT_VISIBLE\n");
    break;
  default:
    printf("VISIBILITY = UNKNOWN\n");
    break;
  }
}

/***************************************************************/
int getDisplayProperties();

/***************************************************************/
void reshapeCB(int width, int height)
{
    printf("reshape(%d, %d) -> (Width=%d, Height=%d)\n",
           Width, Height, width, height);
    reshape(width, height);

    // On OSX I seem to need this flush when the shape changes on startup.
    // Thats usually done to avoid the dock and the menu.
    // Without the flush I lose some of the early geometry.
    // Maybe I should only do this if the size actually changes?
    printf("glFlush(reshapeCB)\n"); glFlush();
}

/***************************************************************/
void reproject()
{
    GLdouble left, right, top, bottom, aspect, znear, zfar, fov;

    aspect = ((GLdouble)Width/(GLdouble)Height);
    left = (GLdouble)-Width / 2.0;
    right = left + (GLdouble)Width;
    bottom = (GLdouble)-Height / 2.0;
    top = bottom + (GLdouble)Height;
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // frustrum = clipping space(left, right, bottom, top, near, far)
    //glFrustum(-3.0, 3.0, -3.0, 3.0, 64, 256);
 
    //fov = projection_fov;
    //Convert from horizontal FOV to vertical for gluPerspective()
    fov =  2.0*atan(tan(PI_180*projection_fov/2.0)/((double)Width/(double)Height))/PI_180;
    znear = projection_znear;
    zfar = projection_zfar;

    // try to get better resolution in depth buffer.  Move near, far.
    if (ldraw_projection_type)
    {
      // fov, aspect, near, far
      gluPerspective(fov, aspect, znear, zfar);
      //glDepthRange(0.0, 1.0); // I do NOT understand this fn.
    }
    else
    {
      // left, right, bottom, top, near, far
      glOrtho(left, right, bottom, top, znear, zfar);
    }

    glMatrixMode(GL_MODELVIEW);
}

/***************************************************************/
void reshape(int width, int height)
{
    GLdouble left, right, top, bottom, aspect, znear, zfar, fov;
    UNUSED(left);
    UNUSED(right);
    UNUSED(top);
    UNUSED(bottom);
    UNUSED(aspect);
    UNUSED(znear);
    UNUSED(zfar);
    UNUSED(fov);

    //printf("reshape(debugging)\n");

    // Check for new opengl context on reshape() calls.
    // Should probably also do this when entering/leaving game mode.
    // NOTE:  I should probably break up this fn so I can skip this check
    // and the dirtyWindow when I just want to reset the projection matrix.
    if (getDisplayProperties())
    {
        // HACK: Exclude from WIN_DIB_OPTION to suppess freeglut ERROR: Function
        // <glutPostRedisplay> called without first calling 'glutInit'. on Windows
#ifndef WIN_DIB_OPTION
        // Uh Oh.  The graphics mode changed.  Dump saved buffers & redraw.
        if (editing)
          NukeSavedDepthBuffer();
        dirtyWindow = 1;
        glutPostRedisplay();
#endif
    }

#if 0
    if (editing)
    {
	// Leave some space at the top of the screen for the "GUI"
	// NOTE: This would work better if reshape() called myreshape()
	// which contains all this code except perhaps the Width, Height =.
	// Also replace all calls to reshape() with myreshape().
	// This way I can tell real reshape events from my calls.
	
      // Scissor is probably easier than viewport
      glEnable(GL_SCISSOR_TEST);
      glScissor(0, 0, Width, Height-40);
    }
    else
      glDisable(GL_SCISSOR_TEST);
#endif
    if (editing)
    {
      if ((width != Width) && (height != Height))
	NukeSavedDepthBuffer();
    }

    Width = width;
    Height = height;

    //    int x, y;
    //    GLdouble pan_x, pan_y, pan_z;

    glViewport(0, 0, Width, Height);

    reproject();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if (ldraw_projection_type)
    {
      // Perhaps this belongs in the ModelView matrix.
      // Supposedly only GluPerspective, glGrustrum, glOrtho* belong in
      // the Projection matrix.
#if 0
      glDepthRange(0.0, 1.0); // I do NOT understand this fn.
#endif
    }

#if 0    
    glGetDoublev(GL_MODELVIEW_MATRIX, model);
    glGetDoublev(GL_PROJECTION_MATRIX, proj);
    glGetIntegerv(GL_VIEWPORT, view);

    // Hmmm, do these convert things to screen coords and vice versa?
    // Convert coords from screen(x,y,0) to world(x,y,z)?
    gluProject((GLdouble)x, (GLdouble)y, 0.0,
	       model, proj, view,
	       &pan_x, &pan_y, &pan_z);
    // Convert coords from world(x,y,z) to screen(x,y,z)?
    gluUnProject((GLdouble)x, (GLdouble)y, pan_z,
		 model, proj, view,
		 &pan_x, &pan_y, &pan_z);
#endif

    
    //glTranslatef(0.0, 0.0, -200.0 + zoom);

    dirtyWindow = 1; 
}

#define USE_F00_CAMERA 1
#ifdef USE_F00_CAMERA
  extern void resetCamera();
  extern void applyCamera();
  extern int specialFunc( int key, int x, int y );
  extern void turnCamera( GLfloat turnX, GLfloat turnY, GLfloat turnZ );
  extern void truckCamera( GLfloat truckBy, int truckX, int truckY, int truckZ );
#endif

/***************************************************************/
// Setup view matrix, lighting, and flat shading before rendering.
// Note any push/pop of model matrix must be done outside of this fn.
void rendersetup(void)
{
  GLdouble fx, fy, fz, tx, ty, tz, ux, uy, uz;

  if (ldraw_commandline_opts.F & TYPE_F_SHADED_MODE) // (zShading)
    glEnable(GL_LIGHTING);
  else
    glDisable(GL_LIGHTING);

  glCurColorIndex = -1;

  glColor3f(1.0, 1.0, 1.0); // White.

  // Turn off dithering for more than 16bpp.  The default is enabled!
  if ((rBits + gBits + bBits) > 16) // ((rBits + gBits + bBits) >= 15)
    glDisable(GL_DITHER);
  else
    glEnable(GL_DITHER);

  glMatrixMode(GL_MODELVIEW);

#ifdef USE_QUATERNION
  // NOTE:  This does NOT work since I'm doing the spin transform BEFORE
  // the gluLookAt() transform, so the model moves around the camera 
  // rather than the camera orbiting the model.
  if (qspin[3] == 0.0)
  {
    glLoadIdentity();
  }
#else
  glLoadIdentity();
#endif

#ifdef USE_F00_CAMERA
  applyCamera();

#ifdef TESTING_F00_CAMERA_CONVERSION
{
  matrix3d *oldm, *m, mat, newm;

  GLdouble model[4*4];

  glGetDoublev(GL_MODELVIEW_MATRIX, model);
  printf("%s(%g,%g,%g,%g, %g,%g,%g,%g %g,%g,%g,%g, %g,%g,%g,%g)\n", "MV",
	 model[0], model[1] , model[2], model[3],
	 model[4], model[5] , model[6], model[7],
	 model[8], model[9] , model[10], model[11],
	 model[12], model[13] , model[14], model[15]);

//#define CONVERT_F00_CAMERA  1
#ifdef CONVERT_F00_CAMERA  
  mat.a = model[0];
  mat.b = model[1];
  mat.c = model[2];
  mat.d = model[4];
  mat.e = model[5];
  mat.f = model[6];
  mat.g = model[8];
  mat.h = model[9];
  mat.i = model[10];

#if 1
  // This applies the new rotation to the model before the view transform.
  // Which feels wrong.  (not like orbiting the camera about the model)
  m = &mat;
  oldm = &(ldraw_commandline_opts.A);
#else
  // This seems to apply the new rotation after the view transform,
  oldm = &mat;
  m = &(ldraw_commandline_opts.A);
#endif

  newm.a = oldm->a * m->a + oldm->b * m->d + oldm->c * m->g;
  newm.b = oldm->a * m->b + oldm->b * m->e + oldm->c * m->h;
  newm.c = oldm->a * m->c + oldm->b * m->f + oldm->c * m->i;
  newm.d = oldm->d * m->a + oldm->e * m->d + oldm->f * m->g;
  newm.e = oldm->d * m->b + oldm->e * m->e + oldm->f * m->h;
  newm.f = oldm->d * m->c + oldm->e * m->f + oldm->f * m->i;
  newm.g = oldm->g * m->a + oldm->h * m->d + oldm->i * m->g;
  newm.h = oldm->g * m->b + oldm->h * m->e + oldm->i * m->h;
  newm.i = oldm->g * m->c + oldm->h * m->f + oldm->i * m->i;

  ldraw_commandline_opts.A = newm;

  projection_fromx -= model[3];
  projection_fromy -= model[7];
  projection_fromz -= model[11];

  // Clear the quaternion camera.
  //resetCamera();
#endif // CONVERT_F00_CAMERA  
}
#endif // TESTING_F00_CAMERA_CONVERSION

#endif // USE_F00_CAMERA  

#define ORBIT_THE_CAMERA_ABOUT_THE_MODEL 1

#ifdef ORBIT_THE_MODEL_ABOUT_THE_CAMERA
  // Do camera translation/rotation BEFORE GluLookAt()
  glRotatef(fXRot, 1.0f, 0.0f, 0.0f);
  glRotatef(360 - fYRot, 0.0f, 1.0f, 0.0f);
  glTranslatef(fCamX, fCamY, fCamZ);        
  // Draw everything else below after gluLookAt()
#endif

  // ldlite_parse seems to offset x,y coords by half the window size.

  // Height/6 moves the origin from halfway to 2/3 of the way down the screen.
  // Original LdLite uses zGetRowsize() and zGetColsize() to do this.
  // I stubbed them to return 0 for OpenGL since its origin is the window
  // center, not the corner.  See LDRAW_COMPATIBLE_CENTER in LdliteVR_main.c.

  // NOTE: I should probably do skip Height/6.0 here and just add Height/6.0
  // to the y values in the glViewport(0, 0, Width, Height) call in reshape()
  fx = projection_fromx;
  fy = projection_fromy + (Height/6.0);
  fz = projection_fromz;
  tx = projection_towardx;
  ty = projection_towardy + (Height/6.0);
  tz = projection_towardz;
  // Hmmm, my up vector is straight up.  This may NOT be perpendicular 
  // to my view vector if it looks down at an angle toward (0,100,0).
  ux = projection_upx;
  uy = projection_upy;
  uz = projection_upz;
  // from, toward, upvector
  gluLookAt(fx, fy, fz, tx, ty, tz, ux, uy, uz);

  // Redo the projection using model sphere to calculate znear, zfar.
  //reproject();

#ifdef ORBIT_THE_CAMERA_ABOUT_THE_MODEL
  // Do camera translation/rotation AFTER the GluLookAt camera transform.
  glRotatef(fXRot, 1.0f, 0.0f, 0.0f);
  glRotatef(-fYRot, 0.0f, 1.0f, 0.0f);
  glTranslatef(fCamX, fCamY, fCamZ);        
  // Draw everything else below
#endif

#ifdef USE_GL_TWIRL
  gluLookAt(400.0, 600.0, 1000.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

  // This does not work right because the ldlite default model matrix
  // has already tilted the model so it twirls tilted if I do this.
  // This WOULD work if I used glulookAt() to set the view angle and
  // substituted an identity matrix for the ldlite model matrix.
  if (twirl_angle != 0.0)
    glRotatef((float)-twirl_angle, 0.0, 1.0, 0.0);
#endif
#ifdef USE_QUATERNION
  printf("spin(%0.2f, %0.2f, %0.2f, %0.2f)\n", 
	 qspin[3], qspin[0], qspin[1], qspin[2]);
  glRotatef(qspin[3], qspin[0], qspin[1], qspin[2]);
#endif

  //glEdgeFlag(GL_FALSE); // Do not draw poly edges?
  glShadeModel(GL_FLAT); // not GL_SMOOTH
  //glShadeModel(GL_SMOOTH); // not GL_SMOOTH

  //glPolygonMode(GL_FRONT,GL_FILL);
  //glPolygonMode(GL_BACK,GL_FILL);
  //glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
  //glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

  if (fogging)
  {
    switch (fogging)
    {
      case 2:
	fogMode = GL_EXP;
	break;
      case 3:
	fogMode = GL_EXP;
	break;
      default:
	fogMode = GL_LINEAR;
	break;
    }
    glEnable(GL_FOG);
    glFogi(GL_FOG_MODE, fogMode);
    glFogfv(GL_FOG_COLOR, fogColor);
    glFogf(GL_FOG_DENSITY, fogDensity);
    //glHint(GL_FOG_HINT, GL_NICEST);
    glHint(GL_FOG_HINT, GL_DONT_CARE);
    glFogf(GL_FOG_START, fogStart);
    glFogf(GL_FOG_END, fogEnd);
  }
  else
    glDisable(GL_FOG);
}

/***************************************************************/
int ldlite_parse_with_rc(char *filename)
{
  // Partly copied from render_file() in ldliteView.cpp
  FILE *fp;
  int bytes = 0;

  fp = fopen(ldconfigfilename,"rb"); // Try the official LDConfig.ldr first.
  if (fp == NULL)
  {
    char filename[256];
    concat_path(datfilepath, ldconfigfilename, filename);
    if (filename[0] == '.') // I hate the ./filename thing.
      strcpy(filename, ldconfigfilename);
    fp = OpenDatFile(filename); // Try the l3p paths after current working dir.
    if (fp == NULL)
    {
      strcpy(filename, primitivepath);
      filename[strlen(filename)-1] = 0;
      strcat(filename, ldconfigfilename);
      fp = OpenDatFile(filename); // Try the l3p paths after current working dir.
    }   
  }
  if (fp == NULL)
    fp = fopen("ldliterc.dat","rb"); // If that fails, try the ldliterc file.
  if (fp != NULL) {
    bytes += fread(buf,1,15*1024,fp);
    fclose(fp);
  }
#if 0
  // NOTE:  I should check the whole search path for this (see lugnet thread)
  // This does NOT work.  Must paste LINES from ldliterc.dat directly into 
  // top level buf.  Otherwise color changes etc are NOT global.
  else
  {
    sprintf(buf,"\n1 16 0 0 0 1 0 0 0 1 0 0 0 1 %s\n", "ldliterc.dat");
    bytes = strlen(buf);
  }
#endif

#ifndef WINDOWS
  // NOTE:  I'm a bit suspicious of quoted filenames on other OSs.  Test it.
  sprintf(&(buf[bytes]),"\n1 16 0 0 0 1 0 0 0 1 0 0 0 1 %s\n", filename);
#else
  sprintf(&(buf[bytes]),"\n1 16 0 0 0 1 0 0 0 1 0 0 0 1 \"%s\"\n", filename);
#endif
  ldlite_parse(buf);
}

/***************************************************************/
int ldlite_parse_colour_meta(char *s)
{
    // Skip whitespace
    for (; *s != 0; s++)
    {
      if ((*s != ' ') && (*s != '\t'))
	break;
    }

    // Intercept the LDConfig.ldr !COLOUR meta command.
    if (strncmp(s,"!COLOUR",7) == 0)
    {
      // 0 !COLOUR Phosphor_White    CODE  21  VALUE #E0FFB0  EDGE #77CC00  ALPHA 250  LUMINANCE 15
      // Gotta handle LUMINANCE n, RUBBER, CHROME, and PEARLESCENT somehow?
      char name[256];
      int i, n, inverse_index, r, g, b, alpha;

      if (ldraw_commandline_opts.debug_level == 1)
	printf("%s\n", s);
      
      n = sscanf(s, "!COLOUR %s CODE %d VALUE #%x EDGE %d ALPHA %d",
		 name, &i, &b, &inverse_index, &alpha);
      if (n == 3) // Retry EDGE as a hex number
      {
	n = sscanf(s, "!COLOUR %s CODE %d VALUE #%x EDGE #%x ALPHA %d",
		   name, &i, &b, &inverse_index, &alpha);
	// Encode EDGE as an L3P extended RGB color.
	inverse_index |= 0x2000000;
      }
      if (n == 4)
      {
	n++;
	alpha = 255;
      }
      if (n != 5)
      {
	if (ldraw_commandline_opts.debug_level == 1)
	  printf("Illegal !COLOUR meta-command syntax %d\n",n);
      }
      else
      {
	r = (b >> 16) & 0xff;
	g = (b >> 8) & 0xff;
	b = b & 0xff;
	zcolor_modify(i,name,inverse_index, r, g, b, alpha, r, g, b, alpha);
      }
      return 1;
    }

    return 0;
}

/***************************************************************/
#define USE_L3_PARSER_AND_BBOX
#ifdef USE_L3_PARSER_AND_BBOX

// Included above now
//#include "StdAfx.h"
//#include "L3Def.h"

void getextents(void)
{
  struct L3LineS       Data;
  int sc[4];
  int n;

  void GetPartBox(struct L3LineS *LinePtr, int sc[4], int clip);

  if (parsername == L3_PARSER)
  {
    memset(&Data, 0, sizeof(Data));
    n = sscanf("1 16 1 0 0 0 1 0 0 0 1 0 0 0",
	       "%d %d %f %f %f %f %f %f %f %f %f %f %f %f",
                 &Data.LineType, &Data.Color,
                 &Data.v[0][0], &Data.v[0][1], &Data.v[0][2],
                 &Data.v[1][0], &Data.v[1][1], &Data.v[1][2],
                 &Data.v[2][0], &Data.v[2][1], &Data.v[2][2],
                 &Data.v[3][0], &Data.v[3][1], &Data.v[3][2]);
    Data.v[3][3] = 1;
    Data.PartPtr = &Parts[0];

    GetPartBox(&Data, sc, 0);  // Skip clipping in GetPartBox().

    printf("sbox = %d, %d, %d, %d\n", sc[0], sc[1], sc[2], sc[3]);

    z.extent_x2 = sc[2];
    z.extent_x1 = sc[0];
    z.extent_y2 = sc[3];
    z.extent_y1 = sc[1];
  }
}
#endif  

/***************************************************************/
// NOTE: this assumes cropping == 1 (which may not be true)
// I should really just get a bounding sphere
// and scale it to fit in the view frustrum.
// Calculate a bounding sphere from the bounding box of the model?
void autoscale(void)
{
  int width = Width;
  int height = Height;
  float xs, ys;
  
  width = z.extent_x2 - z.extent_x1;
  height = z.extent_y2 - z.extent_y1;

  // NOTE: gluProject() seems to clip coords at the window edges
  // so this will not scale down
  printf("W,H = %d, %d\n", width, height);
  if ((width <= 0) || (height <= 0)) return; //break;
  xs = (float) (Width) / (float) width;
  ys = (float) (Height) / (float) height;
  
  printf("X,Y = %0.3f, %0.3f\n", xs, ys);
  // scale up by the smaller amount so it fits both ways.
  if (xs > ys)
    xs = ys;
  ldraw_commandline_opts.S *= xs;
  
  // If its off center then center it.  Does not work too well.
  fCamX -= (float)(z.extent_x2 - Width + z.extent_x1) / 2.0;
  fCamY -= (float)(z.extent_y2 - Height + z.extent_y1) / 2.0;

  printf("S,cX,cY = %0.3f, %0.3f, %0.3f\n", xs, fCamX, fCamY);
}
  
/***************************************************************/
/* render gets called both by "display" (in OpenGL render mode)
   and by "outputEPS" (in OpenGL feedback mode). */
void
render(void)
{
  int rc;
  int client_rect_right;
  int client_rect_bottom;
  int res;
  int drawmode = ldraw_commandline_opts.F;

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();

  rendersetup();

  if (drawAxis)
  {
    glBegin(GL_LINES);
    glColor3f(1.0, 0.0, 0.0); // r
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(1000.0, 0.0, 0.0);
    glColor3f(0.0, 1.0, 0.0); // g
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 500.0, 0.0);
    glColor3f(0.0, 0.0, 1.0); // b
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 0.0, 400.0);
    glEnd();
  }

  if (editing)
  {
    if (turnAxisVisible)
    {
      DrawTurnAxis(turnCenter);
    }
  }

  glColor3f(1.0, 1.0, 1.0); // White.

  client_rect_right = Width;
  client_rect_bottom = Height;

  zcolor_init();

  stepcount = 0;

  rc = zReset(&(client_rect_right),&(client_rect_bottom));
  if (rc != 0) {
    printf("Out of Memory, exiting");
    exit(-1);
  }

  //printModelMat("ModelR");

#ifdef USE_L3_PARSER
  if (parsername == L3_PARSER)
  {
  if (!list_made)
  {
    // LoadModel frees Parts[0] and Lights etc.
    LoadModelPre();
    // L3Time_t ttt = L3GetTime();
    res = LoadModel(datfilename);
    // ttt = L3GetTime() - ttt;
    LoadModelPost();
    // printf("LoadModel: %d",ttt);
    list_made = 1;
  }
#if 1
  if (autoscaling)
  {
    autoscaling = 0;
    InitViewMatrix();
    //reshape(Width, Height);
    getextents();
    autoscale();
  }
#endif
  if (ldraw_commandline_opts.debug_level == 1)
    printf("DrawModel %s\n", datfilename);
#ifdef ONE_BIG_DISPLAY_LIST
  // Displays in 1/3 less time, but does not allow other views
  // except when using opengl to move camera.  (arrow keys)
  // Type 5 lines should be excluded from display list (or drawn afterwards).
  if (list_made < 2)
  {
    glNewList(1, GL_COMPILE);
    DrawModel();
    glEndList();
    list_made = 2;
  }
  glCallList(1);
#else
  if ((qualityLines && ((ldraw_commandline_opts.F & TYPE_F_NO_POLYGONS) == 0))
      || LineChecking)
  {
    ldraw_commandline_opts.F |= TYPE_F_NO_LINES;
    linequalitysetup();
    if (ldraw_commandline_opts.M == 'S')
      preprintstep = 1; //Tell platform_step() NOT to save file now.
    DrawModel();
    ldraw_commandline_opts.F &= ~(TYPE_F_NO_LINES);
    linequalitysetup();
    ldraw_commandline_opts.F |= TYPE_F_NO_POLYGONS; // zWire = 1;
    stepcount = 0; // NOTE: Not sure what effect this will have...
    if (qualityLines)
      z_line_offset += 0.2; // Nudge antialiased lines up in depth buffer.
    preprintstep = 0; //Tell platform_step() its OK to save file now.
    DrawModel();
    if (qualityLines)
      z_line_offset -= 0.2; // Nudge antialiased lines up in depth buffer.
  }
#ifdef USE_OPENGL_OCCLUSION
  else if (editing && !(ldraw_commandline_opts.F & TYPE_F_STUDLESS_MODE))
  {
    ldraw_commandline_opts.F |= TYPE_F_STUDLESS_MODE;
    DrawModel();
    ldraw_commandline_opts.F &= ~(TYPE_F_STUDLESS_MODE);
    ldraw_commandline_opts.F |= TYPE_F_STUDONLY_MODE;
    DrawModel();
    ldraw_commandline_opts.F &= ~(TYPE_F_STUDONLY_MODE);
  }
#endif
  else 
  {
    linequalitysetup();
    DrawModel();
  }
#endif
  }
  else
#endif

  {
#ifdef ONE_BIG_DISPLAY_LIST
  // Nice speedup: Displays in 1/3 time, but does not allow other views.
  if (!list_made)
  {
    glNewList(1, GL_COMPILE);
    znamelist_push();
    ldlite_parse_with_rc(datfilename);
    znamelist_pop();
    glEndList();
    list_made = 1;
  }
  glCallList(1);
#else
  if ((qualityLines && ((ldraw_commandline_opts.F & TYPE_F_NO_POLYGONS) == 0))
      || LineChecking)
  {
    ldraw_commandline_opts.F |= TYPE_F_NO_LINES;
    if (ldraw_commandline_opts.M == 'S')
      preprintstep = 1; //Tell platform_step() NOT to save file now.
  }
  linequalitysetup();
  mpd_subfile_name = NULL; // potential memory leak
  znamelist_push();
  ldlite_parse_with_rc(datfilename);
  if (mpd_subfile_name != NULL) 
  {
    // set file name to first subfile
    if (ldraw_commandline_opts.debug_level == 1)
      printf("Draw MPD %s\n", mpd_subfile_name);

    zcolor_init();

    rc = zReset(&(client_rect_right),&(client_rect_bottom));
    if (rc != 0) {
      printf("Out of Memory, exiting");
      exit(-1);
    }

    znamelist_push();
    // Do NOT call ldlite_parse directly on mpd_subfile_name.  
    // It does not check the file cache for the filename passed to it.
    //**********************************************************************
    // NOTE:  Unfortunately this method calls yy_delete_buffer() which
    // sets yy_current_buffer to NULL, causing a crash on the next redisplay.
    // This is because yy_current_buffer only gets initialized the first time
    // yylex() is called due to the static variable yy_init getting set to 0.
    // So how does the REAL ldlite handle this problem?
    // Perhaps this is why ldliteView.cpp ALWAYS uses the NULL filename way.
    // FIXED??  See my "weak attempt to fix" this in ldlite_parse().
    //**********************************************************************
    ldlite_parse_with_rc(mpd_subfile_name);
    znamelist_pop();
  }
  if ((qualityLines && ((ldraw_commandline_opts.F & TYPE_F_NO_POLYGONS) == 0))
      || LineChecking)
  {
    ldraw_commandline_opts.F &= ~(TYPE_F_NO_LINES);
    linequalitysetup();
    ldraw_commandline_opts.F |= TYPE_F_NO_POLYGONS; // zWire = 1;
    if (qualityLines)
      z_line_offset += 0.2; // Nudge antialiased lines up in depth buffer.
    preprintstep = 0; //Tell platform_step() its OK to save file now.

    zcolor_init();

    stepcount = 0; // NOTE: Not sure what effect this will have...

    push_extents(); // Save the cropping extents before zReset();
    rc = zReset(&(client_rect_right),&(client_rect_bottom));
    if (rc != 0) {
      printf("Out of Memory, exiting");
      exit(-1);
    }
    pop_extents();

    mpd_subfile_name = NULL; // potential memory leak
    znamelist_push();
    ldlite_parse_with_rc(datfilename);
    znamelist_pop();
    if (mpd_subfile_name != NULL) 
    {
      // set file name to first subfile
      if (ldraw_commandline_opts.debug_level == 1)
	printf("Draw MPD %s\n", mpd_subfile_name);

      zcolor_init();

      push_extents(); // Save the cropping extents before zReset();
      rc = zReset(&(client_rect_right),&(client_rect_bottom));
      if (rc != 0) {
	printf("Out of Memory, exiting");
	exit(-1);
      }
      pop_extents();

      znamelist_push();
      ldlite_parse_with_rc(mpd_subfile_name);
      znamelist_pop();
    }

    if (qualityLines)
      z_line_offset -= 0.2; // Nudge antialiased lines up in depth buffer.
  }
#endif
    //if (ldraw_commandline_opts.output != 1) zStep(INT_MAX, 0);
    zStep(stepcount,0);
  }

  glPopMatrix();

  ldraw_commandline_opts.F = drawmode;
}

/***************************************************************/
void DrawScene(void)
{
#if 0
  glDrawBuffer(GL_FRONT);  // Effectively disable double buffer.
#endif

  dirtyWindow = 1; //ALWAYS_REDRAW

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  render();
  if ((ldraw_commandline_opts.M == 'S') && qualityLines)
  {
    //NOTE: This may NOT fix qualityLines for tiled render
    // because it puts us in opts.M = 'P' mode.
    // Could check if (tiledRendering == 1)
    // Or maybe tiled rendering is ok BECAUSE of the forced 'P' mode.
    // If so, the new preprintstep checks in stub.c may hose it up.
    // I may only want to check preprintstep if (opts.M == 'S').
    while (stepcount != curstep)
    {
      save_extents();
      curstep++;
      render();
      merge_extents();
    }
  }
  printf("DrawScene\n");

  //printf("glFlush(DrawScene)\n");
  glFlush();

  dirtyWindow = 0;  // The window is nice and squeaky clean now.

  if (ldraw_commandline_opts.M == 'F')
    platform_step(INT_MAX, 0, -1, NULL);
}

#ifdef TILE_RENDER_OPTION
/***************************************************************/
void TiledDisplay(void)
{
   TRcontext *tr;
   GLubyte *buffer;
   GLubyte *tile;
   FILE *f;
   int more;
   int i,j,k = 0;
   long foffset = 0;
   char *p, c;
   char filename[256];
   UNUSED(i);

#ifdef USE_PNG
   png_structp png_ptr;
   png_infop info_ptr;
   png_text text_ptr[1];
#endif

   GLdouble left, right, top, bottom, aspect, znear, zfar, fov;
   GLdouble width, height;

   width = TILE_IMAGE_WIDTH;
   height = TILE_IMAGE_HEIGHT;

   aspect = ((GLdouble)width/(GLdouble)height);
   left = (GLdouble)-width / 2.0;
   right = left + (GLdouble)width;
   bottom = (GLdouble)-height / 2.0;
   top = bottom + (GLdouble)height;
    
   TILE_WIDTH = Width;
   TILE_HEIGHT = Height;

   // Set Width, Height to total bitmap size for calculations in render().
   Width = TILE_IMAGE_WIDTH;
   Height = TILE_IMAGE_HEIGHT;

   // We must use borders on the tiles if using wide lines.
   if (lineWidth > 1.0)
     TILE_BORDER = (int) floor(lineWidth + 0.5);
   else 
     TILE_BORDER = 0;

   printf("Generating %d by %d image file...\n", TILE_IMAGE_WIDTH, TILE_IMAGE_HEIGHT);

   /* allocate buffer large enough to store one tile */
   tile = malloc(TILE_WIDTH * TILE_HEIGHT * 3 * sizeof(GLubyte));
   if (!tile) {
      printf("Malloc of tile buffer failed!\n");
      return;
   }

   /* allocate buffer to hold a row of tiles */
   buffer = malloc(TILE_IMAGE_WIDTH * TILE_HEIGHT * 3 * sizeof(GLubyte));
   if (!buffer) {
      free(tile);
      printf("Malloc of tile row buffer failed!\n");
      return;
   }

   /* Setup.  Each tile is TILE_WIDTH x TILE_HEIGHT pixels. */
   tr = trNew();
   trTileSize(tr, TILE_WIDTH, TILE_HEIGHT, TILE_BORDER);
   trTileBuffer(tr, GL_RGB, GL_UNSIGNED_BYTE, tile);
   trImageSize(tr, TILE_IMAGE_WIDTH, TILE_IMAGE_HEIGHT);
   trRowOrder(tr, TR_TOP_TO_BOTTOM);

   //fov = projection_fov;
   //Convert from horizontal FOV to vertical for gluPerspective()
   fov =  2.0*atan(tan(PI_180*projection_fov/2.0)/((double)width/(double)height))/PI_180;
   znear = projection_znear;
   zfar = projection_zfar;

   // try to get better resolution in depth buffer.  Move near, far.
   if (ldraw_projection_type)
   {
     // fov, aspect, near, far
     trPerspective(tr, fov, aspect, znear, zfar);
     //glDepthRange(0.0, 1.0); // I do NOT understand this fn.
   }
   else
   {
     // left, right, bottom, top, near, far
     trOrtho(tr, left, right, bottom, top, znear, zfar);
   }

   /*************************************************************/
   // NOTE: For now switch to continuous ('C') mode .
   // Pause ('P') mode would only draw the first step and then exit.
   // Save ('S') mode would save EVERY TILE for every step in platform_step().
   // This is bad since we need to finish ALL tiles and THEN save the step.
   // We *should* put it in 'P' mode to loop through all steps (and twirls).
   // Then figure out how to save different formats at the end of this fn.
   // We just need to automate the next step at the end TiledDisplay().
   // I don't think stepping and/or twirling in myGlutIdle() is good for tiles.
#define TILE_STEPPING 1
#ifdef TILE_STEPPING
   ldraw_commandline_opts.M = 'P'; 
#else
   ldraw_commandline_opts.M = 'C'; 
#endif

   // NOTE: For now disable cropping.
   // I suspect the image extents would be mangled in tiled mode.
   // We would need to fixup the extents after each tile,
   // and clear them before each step.
   // Plus, cropping is *most* useful for *small* pics like parts.
   cropping = 0;
   /*************************************************************/


   /****************** Prepare ppm output file ******************/
   platform_step_filename(curstep, filename);

#ifdef USE_PNG
   if (ldraw_image_type == IMAGE_TYPE_PNG_RGB)
   {
     use_png_alpha = 0;
     f = start_png(filename, width, height, &png_ptr, &info_ptr);
   }
   else if (ldraw_image_type == IMAGE_TYPE_PNG_RGBA)
   {
     // use_png_alpha = 1; 
     use_png_alpha = 0; // Tiled renderer does not support alpha channel.
     f = start_png(filename, width, height, &png_ptr, &info_ptr);
   }
   else
#endif
   if ((ldraw_image_type == IMAGE_TYPE_BMP) ||
       (ldraw_image_type == IMAGE_TYPE_BMP8))
   {
     f = start_bmp(filename, width, height);
     foffset = ftell(f);
   }
   else
   {
     f = start_ppm(filename, TILE_IMAGE_WIDTH, TILE_IMAGE_HEIGHT);
   }

   /* just to be safe... */
   glPixelStorei(GL_PACK_ALIGNMENT, 1);

   /* Draw tiles */
   more = 1;
   while (more) {
      int curColumn;
      trBeginTile(tr);
      curColumn = trGet(tr, TR_CURRENT_COLUMN);
      DrawScene();      /* draw our stuff here */
      more = trEndTile(tr);

      /* save tile into tile row buffer*/
      {
	 int curTileWidth = trGet(tr, TR_CURRENT_TILE_WIDTH);
	 int bytesPerImageRow = TILE_IMAGE_WIDTH*3*sizeof(GLubyte);
	 int bytesPerTileRow = (TILE_WIDTH-2*TILE_BORDER) * 3*sizeof(GLubyte);
	 int xOffset = curColumn * bytesPerTileRow;
	 int bytesPerCurrentTileRow = (curTileWidth-2*TILE_BORDER)*3*sizeof(GLubyte);
	 int i;
	 for (i=0;i<TILE_HEIGHT;i++) {
	    memcpy(buffer + i*bytesPerImageRow + xOffset, /* Dest */
		   tile + i*bytesPerTileRow,              /* Src */
		   bytesPerCurrentTileRow);               /* Byte count*/
	 }
      }
      
      if (curColumn == trGet(tr, TR_COLUMNS)-1) {
	/* write this buffered row of tiles to the file */
	int curTileHeight = trGet(tr, TR_CURRENT_TILE_HEIGHT);
	int bytesPerImageRow = TILE_IMAGE_WIDTH*3*sizeof(GLubyte);
	int i;
	GLubyte *rowPtr;

	// NOTE: This works, but appears to be off by a single pixel on 
	// the horizontal seams.  (even when TILE_BORDER is 0)
	// It looks like we're getting 1 extra row of pixels.
	for (i=TILE_BORDER*2;i<curTileHeight;i++) {
	  /* Remember, OpenGL images are bottom to top.  Have to reverse. */
	  rowPtr = buffer + (curTileHeight-1-i) * bytesPerImageRow;

#ifdef USE_PNG
	  if ((ldraw_image_type == IMAGE_TYPE_PNG_RGB) ||
	      (ldraw_image_type == IMAGE_TYPE_PNG_RGBA))
	  {
	    png_write_row(png_ptr, rowPtr);
	  }
	  else
#endif
	  if ((ldraw_image_type == IMAGE_TYPE_BMP) ||
	      (ldraw_image_type == IMAGE_TYPE_BMP8))
	  {
	    long offset = foffset;
	    offset += (height-k) * width * 3;
	    fseek(f, offset, SEEK_SET);
	    k++;

	    p = rowPtr;
	    for (j = 0; j < width; j++) // RGB -> BGR
	    {
	      c = p[0];
	      p[0] = p[2];
	      p[2] = c;
	      p+=3;
	    }
	    fwrite(rowPtr, 1,width*3, f);
	  }
	  else
	  
	  /************* Save a row to the ppm output file *************/
	  // NOTE: We will have to disable ALPHA (just use RGB) for png files.
	  fwrite(rowPtr, 1, TILE_IMAGE_WIDTH*3, f);
	  /*************************************************************/
	}
      }

   }
   trDelete(tr);

#ifdef USE_PNG
   if ((ldraw_image_type == IMAGE_TYPE_PNG_RGB) ||
       (ldraw_image_type == IMAGE_TYPE_PNG_RGBA))
   {
     text_ptr[0].key = "Software";
     text_ptr[0].text = "LdGLite";
     text_ptr[0].compression = PNG_TEXT_COMPRESSION_NONE;
     png_set_text(png_ptr, info_ptr, text_ptr, 1);
  
     png_write_end(png_ptr, info_ptr);
     png_destroy_write_struct(&png_ptr, &info_ptr);

     fclose(f);
   }
   else
#endif

   /************* Finish and close ppm output file **************/
   fclose(f);
   printf("%s complete.\n", filename);
   /*************************************************************/

   free(tile);
   free(buffer);

   // Restore Width, Height to window (tile) size just in case.
   Width = TILE_WIDTH;
   Height = TILE_HEIGHT;

#ifndef TILE_STEPPING
   exit(0); //NOTE: Gotta handle multiple steps.  Exit only after last step.
#endif

   fprintf(stdout, "Step %d of %d.  ",curstep+1, stepcount+1);
   if (stepcount == curstep)
   {
     // NOTE: What about twirl?
     exit(0);
   }
   else
   {
     curstep++; // Move on to next step
     glutPostRedisplay();
   }
}
#endif

/***************************************************************/
// These symbols are defined in the MAX gfx.h header file
#define GL_KTX_FRONT_REGION 0x0000
#define GL_KTX_BACK_REGION 0x0001
#define GL_KTX_Z_REGION 0x0002
#define GL_KTX_STENCIL_REGION 0x0003

#ifndef WINAPI
#define WINAPI 
#endif

typedef GLuint (WINAPI * PFNGLNEWBUFFERREGIONPROC)(GLenum);
typedef void (WINAPI * PFNGLDELETEBUFFERREGIONPROC)(GLuint);
typedef void (WINAPI * PFNGLREADBUFFERREGIONPROC)(GLuint, GLint, GLint, GLsizei, GLsizei);
typedef void (WINAPI * PFNGLDRAWBUFFERREGIONPROC)(GLuint, GLint, GLint, GLsizei, GLsizei, GLint, GLint);
typedef GLuint (WINAPI * PFNGLBUFFERREGIONENABLEDPROC)(void);

PFNGLNEWBUFFERREGIONPROC glNewBufferRegion = NULL;
PFNGLDELETEBUFFERREGIONPROC glDeleteBufferRegion = NULL;
PFNGLREADBUFFERREGIONPROC glReadBufferRegion = NULL;
PFNGLDRAWBUFFERREGIONPROC glDrawBufferRegion = NULL;
PFNGLBUFFERREGIONENABLEDPROC glBufferRegionEnabled = NULL;

/***************************************************************/
void test_ktx_buffer_region(char *str)
{
  // Reset to defaults in case this is done on resize.
  glNewBufferRegion = NULL;
  glDeleteBufferRegion = NULL;
  glReadBufferRegion = NULL;
  glDrawBufferRegion = NULL;
  glBufferRegionEnabled = NULL;

  if (strstr(str,"GL_KTX_buffer_region") )
  {	
    printf("The GL_KTX_buffer_region extension is available\n");
#ifdef WINDOWS
    // Figure out how to do load extensions in MESA
    // See for example leocad/linux/linux_gl.cpp
#if 0
    glNewBufferRegion = (GLuint (*)(GLenum))
      wglGetProcAddress("glNewBufferRegion");
    glDeleteBufferRegion = (void (*)(GLuint))
      wglGetProcAddress("glDeleteBufferRegion");
    glReadBufferRegion = (void (*)(GLuint, GLint, GLint, GLsizei, GLsizei))
      wglGetProcAddress("glReadBufferRegion");
    glDrawBufferRegion = (void (*)(GLuint, GLint, GLint, GLsizei, GLsizei, GLint, GLint))
      wglGetProcAddress("glDrawBufferRegion");
    glBufferRegionEnabled = (GLuint (*)(void))
      wglGetProcAddress("glBufferRegionEnabled");
#else
    glNewBufferRegion = (PFNGLNEWBUFFERREGIONPROC)
      wglGetProcAddress("glNewBufferRegion");
    printf("The GL_KTX_f1 = %p\n",glNewBufferRegion);
    glDeleteBufferRegion = (PFNGLDELETEBUFFERREGIONPROC)
      wglGetProcAddress("glDeleteBufferRegion");
    printf("The GL_KTX_f2 = %p\n",glDeleteBufferRegion);
    glReadBufferRegion = (PFNGLREADBUFFERREGIONPROC)
      wglGetProcAddress("glReadBufferRegion");
    printf("The GL_KTX_f3 = %p\n",glReadBufferRegion);
    glDrawBufferRegion = (PFNGLDRAWBUFFERREGIONPROC)
      wglGetProcAddress("glDrawBufferRegion");
    printf("The GL_KTX_f4 = %p\n",glDrawBufferRegion);
    glBufferRegionEnabled = (PFNGLBUFFERREGIONENABLEDPROC)
      wglGetProcAddress("glBufferRegionEnabled");
    printf("The GL_KTX_f5 = %p\n",glBufferRegionEnabled);
#endif

    printf("glBufferRegionEnabled() = %d\n", glBufferRegionEnabled());
    if (glBufferRegionEnabled())
    {
      buffer_swap_mode = SWAP_TYPE_KTX;
    }
#else
    // NOT WINDOWS (figure out how to load extensions)
#endif
  }
}

/***************************************************************/
#if defined(WINDOWS)
#include "wglext.h"

PFNWGLCREATEBUFFERREGIONARBPROC		wglCreateBufferRegionARB = NULL;
PFNWGLDELETEBUFFERREGIONARBPROC		wglDeleteBufferRegionARB = NULL;
PFNWGLSAVEBUFFERREGIONARBPROC		wglSaveBufferRegionARB = NULL;
PFNWGLRESTOREBUFFERREGIONARBPROC	wglRestoreBufferRegionARB = NULL;

HANDLE			buffer_handle;
HDC				hDC;

PFNWGLGETEXTENSIONSSTRINGARBPROC        wglGetExtensionsStringARB = NULL;

/***************************************************************/
void test_wgl_arb_buffer_region(char *str)
{
#if 1
  const char *wglstr;
  
  wglGetExtensionsStringARB = wglGetProcAddress("wglGetExtensionsStringARB");
  if (wglGetExtensionsStringARB)
  {
    hDC = wglGetCurrentDC();
    wglstr = wglGetExtensionsStringARB(hDC);
    str = wglstr;
    printf("WGL_EXTENSIONS = %s\n", wglstr);
    
  }
#endif

  if (strstr(str,"WGL_ARB_pbuffer") )
  {	
    // Maybe I should use this one instead of buffer_region.
    printf("The WGL_ARB_pbuffer extension is available\n");
  }
  if (strstr(str,"WGL_ARB_buffer_region") )
  {	
    printf("The WGL_ARB_buffer_region extension is available\n");

    hDC = wglGetCurrentDC();

    wglCreateBufferRegionARB = (PFNWGLCREATEBUFFERREGIONARBPROC)
      wglGetProcAddress("wglCreateBufferRegionARB");
    wglDeleteBufferRegionARB = (PFNWGLDELETEBUFFERREGIONARBPROC)
      wglGetProcAddress("wglDeleteBufferRegionARB");
    wglSaveBufferRegionARB = (PFNWGLSAVEBUFFERREGIONARBPROC)
      wglGetProcAddress("wglSaveBufferRegionARB");
    wglRestoreBufferRegionARB = (PFNWGLRESTOREBUFFERREGIONARBPROC)
      wglGetProcAddress("wglRestoreBufferRegionARB");
    
    //buffer_handle = wglCreateBufferRegionARB( hDC, 0, WGL_BACK_COLOR_BUFFER_BIT_ARB | WGL_DEPTH_BUFFER_BIT_ARB );
  }
}

#endif

/***************************************************************/
int NukeSavedDepthBuffer(void)
{
  if (buffer_swap_mode == SWAP_TYPE_KTX)
  {
    if (cbuffer_region != 0)
      glDeleteBufferRegion(cbuffer_region);
    if (zbuffer_region != 0)
      glDeleteBufferRegion(zbuffer_region);

    cbuffer_region = 0;
    zbuffer_region = 0;
  }
  else if (zbufdata)
  {
    free (zbufdata);  // NOTE: gotta free this when finished editing.
    zbufdata = NULL;
  }

  if (cbufdata)
  {
    free (cbufdata);  // NOTE: gotta free this when finished editing.
    cbufdata = NULL;
  }
  return 0;
}

/***************************************************************/
void SaveDepthBuffer(void)
{
#ifdef WINTIMER
  starttime = timeGetTime();
#endif
  if (buffer_swap_mode == SWAP_TYPE_KTX)
  {
    if (cbuffer_region == 0)
      cbuffer_region = glNewBufferRegion(GL_KTX_BACK_REGION);
    if (zbuffer_region == 0)
      zbuffer_region = glNewBufferRegion(GL_KTX_Z_REGION);

    glReadBufferRegion(zbuffer_region,0,0,Width,Height);
    glReadBufferRegion(cbuffer_region,0,0,Width,Height);
  }
  else
  {
    // Apparently there is only ONE zbuffer shared by front & back buffers.
#ifndef SAVE_DEPTH_ALL
    // Gotta figure out the src,dst stuff.  glTranslate()?
    //glRasterPos2i((int)sc[0], (int)sc[1]);
    Get1PartBox(curpiece, sc);
    if (ldraw_commandline_opts.debug_level == 1)
      printf("sbox = %d, %d, %d, %d\n", sc[0], sc[1], sc[2], sc[3]);
    if (zbufdata)
      free (zbufdata);  // NOTE: gotta free this when finished editing.
    //zbufdata = (float *) malloc(sc[2] * sc[3] * sizeof(float));
    zbufdata = (int *) malloc(sc[2] * sc[3] * sizeof(int));
    glReadBuffer(staticbuffer); // set pixel source
    glReadPixels(sc[0],sc[1],sc[2],sc[3],GL_DEPTH_COMPONENT,GL_UNSIGNED_INT,zbufdata);
#else
#ifndef RESTORE_DEPTH_ALL
    sc[0] = sc[1] = 0;
    sc[2] = Width;
    sc[3] = Height;
    Get1PartBox(curpiece, sc);
    if (ldraw_commandline_opts.debug_level == 1)
      printf("sd_sbox = %d, %d, %d, %d\n", sc[0], sc[1], sc[2], sc[3]);
#endif
	glPixelStorei(GL_PACK_ALIGNMENT,1); //4
        glPixelStorei(GL_PACK_ROW_LENGTH,0); //Width
        glPixelStorei(GL_PACK_SKIP_ROWS,0);
        glPixelStorei(GL_PACK_SKIP_PIXELS,0);

    if (zbufdata) // NOTE: gotta free this when finished editing.
    {
      //zbufdata = realloc(zbufdata, Width * Height * sizeof(float));
    }
    else
      zbufdata = (int *) malloc(Width * Height * sizeof(int));
      //zbufdata = (float *) malloc(sc[2] * sc[3] * sizeof(float));
    glReadBuffer(staticbuffer); // set pixel source
    //glReadPixels(0,0, Width,Height, GL_DEPTH_COMPONENT, GL_FLOAT, zbufdata);
    glReadPixels(0,0,Width,Height,GL_DEPTH_COMPONENT,GL_UNSIGNED_INT,zbufdata);
#endif
  }
#ifdef WINTIMER
  finishtime = timeGetTime();
  printf("Save Depth Elapsed = %d\n", finishtime-starttime);
#endif
  //NOTE:  I have to reallocate zbufdata whenever we resize the window.
}

/***************************************************************/
void RestoreDepthBuffer(void)
{
  int savedirty;

#ifdef WINTIMER
  starttime = timeGetTime();
#endif
  // get fresh copy of static data
  if (buffer_swap_mode == SWAP_TYPE_KTX)
  {
    glDrawBufferRegion(zbuffer_region,0,0,Width,Height,0,0);
  }
  else
  {
    // Gotta fix these later because they get set only once in init().
    //glDisable(GL_COLOR_MATERIAL);
    //glDisable(GL_POLYGON_OFFSET_FILL);
    //glEnable(GL_CULL_FACE);
    //glFrontFace(GL_CW);
    //glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);

    glPixelZoom(1, 1);
    glDisable(GL_STENCIL_TEST);    
    glDisable(GL_FOG);

        glDisable(GL_ALPHA_TEST);
        glDisable(GL_BLEND);
        //glDisable(GL_DEPTH_TEST);
        glDisable(GL_DITHER);
        glDisable(GL_FOG);
        glDisable(GL_LIGHTING);
        glDisable(GL_LOGIC_OP);
        glDisable(GL_STENCIL_TEST);
        glDisable(GL_TEXTURE_1D);
        glDisable(GL_TEXTURE_2D);
        glPixelTransferi(GL_MAP_COLOR, GL_FALSE);
        glPixelTransferi(GL_RED_SCALE, 1);
        glPixelTransferi(GL_RED_BIAS, 0);
        glPixelTransferi(GL_GREEN_SCALE, 1);
        glPixelTransferi(GL_GREEN_BIAS, 0);
        glPixelTransferi(GL_BLUE_SCALE, 1);
        glPixelTransferi(GL_BLUE_BIAS, 0);
        glPixelTransferi(GL_ALPHA_SCALE, 1);
        glPixelTransferi(GL_ALPHA_BIAS, 0);

    glPixelTransferi(GL_DEPTH_SCALE,1);
    glPixelTransferi(GL_DEPTH_BIAS,0);

  // Turn off any smoothing or blending modes.
    glDisable( GL_POINT_SMOOTH ); 
    glDisable(GL_ALPHA_TEST);
    glDisable( GL_LINE_SMOOTH ); 
    glHint( GL_LINE_SMOOTH_HINT, GL_FASTEST ); // GL_NICEST GL_DONT_CARE
    glDisable( GL_BLEND );
    glDisable( GL_POLYGON_SMOOTH ); 
    glHint( GL_POLYGON_SMOOTH_HINT, GL_FASTEST ); // GL_NICEST GL_DONT_CARE
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

    glDisable(GL_LIGHTING);     // Speed up copying
    glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE); //disable color updates
    glDepthMask(GL_TRUE); // enable updates to depth buffer
    glEnable( GL_DEPTH_TEST ); 
    glDepthFunc(GL_ALWAYS);

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluOrtho2D(0, Width, 0, Height);
    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();
    glLoadIdentity();
    glRasterPos2i(0, 0);
#ifndef SAVE_DEPTH_ALL
    // Gotta figure out the src,dst stuff.  glTranslate()?
    glRasterPos2i(sc[0], sc[1]);
    if (ldraw_commandline_opts.debug_level == 1)
      printf("bbox = %d, %d, %d, %d\n", sc[0], sc[1], sc[2], sc[3]);
    glDrawPixels(sc[2],sc[3],GL_DEPTH_COMPONENT,GL_UNSIGNED_INT,zbufdata);
#else
#ifdef RESTORE_DEPTH_ALL
	glPixelStorei(GL_UNPACK_ALIGNMENT,1); //4
        glPixelStorei(GL_UNPACK_ROW_LENGTH,0); //Width
        glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
        glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);

    // This is PAINFULLY slow! (almost a second on TNT)
    // Consider this:
    //   Allocate a vertex buffer, same size as you have pixels on screen. 
    //   Fill in a vertex position for each pixel center 
    //   (to prevent antialiasing issues).
    //   Draw that with glDrawArrays(GL_POINTS,0,Width*Height);
    // 
    // UNSIGNED_INT is about 20% faster on Windows/TNT
    //glDrawPixels(Width,Height,GL_DEPTH_COMPONENT,GL_FLOAT,zbufdata);
    glDrawPixels(Width,Height,GL_DEPTH_COMPONENT,GL_UNSIGNED_INT,zbufdata);
#else
	glPixelStorei(GL_UNPACK_ALIGNMENT,1); //4
        glPixelStorei(GL_UNPACK_ROW_LENGTH,Width); //Width
        glPixelStorei(GL_UNPACK_SKIP_ROWS, sc[1]);
        glPixelStorei(GL_UNPACK_SKIP_PIXELS, sc[0]);
    glRasterPos2i(sc[0], sc[1]);
    if (ldraw_commandline_opts.debug_level == 1)
      printf("bbox = %d, %d, %d, %d\n", sc[0], sc[1], sc[2], sc[3]);
    glDrawPixels(sc[2],sc[3],GL_DEPTH_COMPONENT,GL_UNSIGNED_INT,zbufdata);
        // Set UNPACK back to default for glPolygonStipple()
        glPixelStorei(GL_UNPACK_ROW_LENGTH,0); //Width
        glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
        glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
#endif
#endif
    glPopMatrix();
    glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE); //enable color buffer updates
    glDepthFunc(GL_LESS);

    // Reset the projection matrix.
    savedirty = dirtyWindow; 
    reshape(Width, Height);
    dirtyWindow = savedirty; 

    rendersetup();
  }
#ifdef WINTIMER
  finishtime = timeGetTime();
  printf("Restore Depth Elapsed = %d\n", finishtime-starttime);
#endif
}

//#define TNT2_TEST
/***************************************************************/
int XORcurPiece()
{
  int retval = 0;
  int drawmode = ldraw_commandline_opts.F;

#ifdef MESA_XOR_TEST
  if (buffer_swap_mode == SWAP_TYPE_NODAMAGE) // Mesa
  {
    // Gotta fix Mesa to render to back buffer.  
    // Its 2x faster for normal drawing.  10x faster for XOR and BLEND.
    // NOTE: Copybuffer at 1280x1024 on WinGeneric takes .5 to .8 secs (slow)
    // NOTE: CopyBuffer seems to lose the block under the cursor in the
    //       Windows Generic driver (at least when alpha blended cursor is on).
    glDrawBuffer(staticbuffer); 
  }
#endif

  // Draw only edge lines.  No polygon faces.
  ldraw_commandline_opts.F = TYPE_F_NO_POLYGONS;

  // Consider forcing drawing studlines for slow Mesa XOR.
  //ldraw_commandline_opts.F |= TYPE_F_STUDLESS_MODE;
  //ldraw_commandline_opts.F = (TYPE_F_NO_POLYGONS | TYPE_F_STUDLESS_MODE);
  if ((drawmode & TYPE_F_STUDLINE_MODE) != 0)
    ldraw_commandline_opts.F |= TYPE_F_STUDLINE_MODE;

  // Disable Lighting, glDisable(GL_LIGHTING) gets called in rendersetup()
  ldraw_commandline_opts.F &= ~(TYPE_F_SHADED_MODE); // zShading = 0;

  // Disable antialiasing and linewidths.
  glDisable( GL_LINE_SMOOTH ); 
  glHint( GL_LINE_SMOOTH_HINT, GL_FASTEST ); // GL_NICEST GL_DONT_CARE
  glDisable( GL_BLEND );
  glLineWidth( 1.0 );
  // Should rerun linequalitysetup() after XORcurPiece is over.

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();

  rendersetup();

  // No need to dither the XOR lines, even if at 8bpp.
  glDisable(GL_DITHER);

#ifdef USE_OPENGL_STENCIL
  // Too many lines cancel out in wireframe mode when viewed head on.
  // I could make sure I only XOR each pixel once by setting
  // the stencil buffer at each write but only passing when it's zero.
  if (use_stencil_for_XOR)
  {
    glStencilMask(GL_TRUE);                  // Enable stencil buffer writes.
    glClearStencil(0x0);                     // Set stencil clear color
    glClear(GL_STENCIL_BUFFER_BIT);          // Perhaps just clear the bbox?
    glEnable(GL_STENCIL_TEST);               // Enable the stencil Test.
    glStencilFunc(GL_EQUAL, 0x0, 0x1);  // Stencil test(fn, refbits, bitmask)
    glStencilOp(GL_KEEP, GL_KEEP, GL_INVERT); // S-Buf write fns(sf, zf, zp)
  }
#ifdef TNT2_TEST
  // NOTE: since TNT has the ktx_buffer_region support I could fake the 
  // XOR wireframe using the backup buffer to redraw instead of XORrasing.
  // Always use SOLID_EDIT_MODE but switch to wireframe before DrawCurPart().
  else
  {
#if 1
    glStencilMask(GL_TRUE);                  // Enable stencil buffer writes.
    glClearStencil(0x01);                     // Set stencil clear color
    glClear(GL_STENCIL_BUFFER_BIT);          // Perhaps just clear the bbox?
    glEnable(GL_STENCIL_TEST);               // Enable the stencil Test.
    glStencilFunc(GL_EQUAL, 0x1, 0x1);  // Stencil test(fn, refbits, bitmask)
    glStencilOp(GL_KEEP, GL_ZERO, GL_ZERO); // S-Buf write fns(sf, zf, zp)
#else
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 0x0, 0x0);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
#endif
  }
#endif
#endif

  glDisable(GL_LIGHTING); // No need for lighting
  if (movingpiece == curpiece)
  {
    z_line_offset += 1.0;
    glDepthMask(GL_FALSE); // keep depth test, just disable depth writes

  }
#ifdef TNT2_TEST
  else if (!use_stencil_for_XOR)
  {
    z_line_offset += 1.0;
    //glDepthFunc(GL_ALWAYS);     // New value always wins.
    glDepthMask(GL_FALSE); // keep depth test, just disable depth writes
  }
#endif
  else
    glDisable( GL_DEPTH_TEST ); // don't test for depth -- just put in front
  glEnable( GL_COLOR_LOGIC_OP ); 
  glLogicOp(GL_XOR);
  ldraw_commandline_opts.F |= TYPE_F_XOR_MODE;
  glColor3f(1.0, 1.0, 1.0); // white
  glCurColorIndex = -2;

  retval = Draw1Part(curpiece, 15);
  printf("glFlush(XORcurPiece)\n"); glFlush(); // Force display now.  For OSX, this copies BACK to FRONT.

#ifdef MESA_XOR_TEST
  if (buffer_swap_mode == SWAP_TYPE_NODAMAGE) // Mesa
  {
    //CopyColorBuffer(renderbuffer, screenbuffer); 
    if (!panning)
    {
      {printf("glutSwapBuffers()\n"); glutSwapBuffers();} // Copy the BACK buffer with XOR part to FRONT.
      retval = Draw1Part(curpiece, 15);  // Erase it in the BACK buffer.
      glDrawBuffer(screenbuffer); // Switch to FRONT buffer for LEDIT menu.
    }
  }
#endif

#ifdef MACOS_X_TEST2
  // Apple Fakes GL_FRONT drawing by drawing in the back buffer
  // so we need to erase the XOR after the flush so it doesnt leave a trail.
  // (The flush supposedly forces a copy from BACK to FRONT)
  if ((buffer_swap_mode == SWAP_TYPE_APPLE) && panning)
    retval = Draw1Part(curpiece, 15);
  // The part may flicker during panning because of another flush after the GUI
  // gets drawn, but its better than XOR droppings.
#endif

  glLogicOp(GL_COPY);
  ldraw_commandline_opts.F &= ~(TYPE_F_XOR_MODE);
  if (movingpiece == curpiece)
  {
    z_line_offset -= 1.0;
  }
#ifdef TNT2_TEST
  else if (!use_stencil_for_XOR)
  {
    z_line_offset -= 1.0;
    glDepthFunc(GL_LESS);     // New value always wins.
  }
#endif
#ifdef USE_OPENGL_STENCIL
  glDisable(GL_STENCIL_TEST);
  glStencilMask(GL_FALSE);
#endif
  glDepthMask(GL_TRUE); // Restore depth writes
  glEnable( GL_DEPTH_TEST ); // Restore depth test
  glDisable( GL_COLOR_LOGIC_OP ); 

  // Restore dithering to whatever rendersetup() sets it at.
  if ((rBits + gBits + bBits) > 16) // ((rBits + gBits + bBits) >= 15)
    glDisable(GL_DITHER);
  else
    glEnable(GL_DITHER);

  glCurColorIndex = -1;

  // Move glFlush(); to right after Draw1Part() so I can erase it in BACK buffer.

  glPopMatrix();

  ldraw_commandline_opts.F = drawmode;

  return retval;
}

/***************************************************************/
// Here is a snippet from the web. 
#if 0
glGet(GL_CURRENT_RASTER_POSITION); //0.5000 165.0000 0.5000 1.0000
glGet(GL_DEPTH_BIAS); //0.0000
glGet(GL_DEPTH_BITS); //32.0000
glGet(GL_DEPTH_CLEAR_VALUE); //1.0000
glGet(GL_DEPTH_FUNC); //513.0000
glGet(GL_DEPTH_RANGE); //0.0000 1.0000
glGet(GL_DEPTH_SCALE); //1.0000
glGet(GL_DEPTH_TEST); //0.0000
glGet(GL_DEPTH_WRITEMASK); //1.0000

glPushAttrib(GL_COLOR_BUFFER_BIT|GL_CURRENT_BIT|GL_DEPTH_BUFFER_BIT|GL_FOG_BIT|GL_LIGHTING_BIT|GL_MISC_BIT_EXT|GL_VIEWPORT_BIT);
glViewport(0, 0, 470, 330);
glScissor(0, 0, 470, 330);
glDisable(GL_DEPTH_TEST);
glDisable(GL_FOG);
glMatrixMode(GL_PROJECTION);
glPushMatrix();
glLoadIdentity();
glOrtho(-0.5000f, 470.5000f, -0.5000f, 330.5000f, -1.0000f, 1.0000f);
glMatrixMode(GL_MODELVIEW);
glPushMatrix();
glLoadIdentity();
glDrawBuffer(GL_FRONT_AND_BACK);
glRasterPos2f(0.0000f, 165.0000f);
glCopyPixels(0, 0, 470, 165, GL_COLOR);
glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
// Added these two lines
glEnable(GL_DEPTH_TEST);
glDepthFunc(GL_ALWAYS);     // New value always wins.
// *********************
glCopyPixels(0, 0, 470, 165, GL_DEPTH);
glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
glPopMatrix();
glMatrixMode(GL_PROJECTION);
glPopMatrix();
glMatrixMode(GL_MODELVIEW);
glDrawBuffer(GL_BACK);
glPopAttrib();
#endif

/***************************************************************/
/***************************************************************/
// CopyStaticBuffer is fairly slow but should always work.
// I could speed this up with opengl extensions when I get more time
// Look for:
// WGL_ARB_pbuffer and/or WGL_EXT_pbuffer
// GLX_MESA_copy_sub_buffer / glXCopySubBufferMESA()
// GL_WIN_swap_hint (I have this one in software OpenGL)
//   This may require a pixel format with PFD_SWAP_COPY.  Must read more...
// GLX_SGIX_pbuffer
// Also I wonder if I could copy the depth buffer to the accumulation buffer.
//
// NOTE:  these two extensions are the same thing with different names.
// I should try these first after testing MESA COPY_NODAMAGE stuff.
// WGL_ARB_buffer_region
// GL_KTX_buffer_region

/***************************************************************/
void CopyStaticBuffer(int forcedsave)
{
  int goodZ = 0;

  if (SOLID_EDIT_MODE)
  {
    if (movingpiece != curpiece)
    {
      Select1Part(curpiece);
      // Draw stuff into the staticbuffer
      glDrawBuffer(staticbuffer); 
      
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      render();

      SaveDepthBuffer();
      goodZ = 1; // No need to copy depth back in yet.
    }
    else if (forcedsave)
    {
      SaveDepthBuffer();
      goodZ = 1; // No need to copy depth back in yet.
    }
#ifdef MACOS_X_TEST1
    else
    {
      // Draw stuff into the staticbuffer
      glDrawBuffer(staticbuffer); 
      
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      render();

      SaveDepthBuffer();
      goodZ = 1; // No need to copy depth back in yet.
    }
#endif
    movingpiece = curpiece;
    
    // get fresh copy of static data
    if (buffer_swap_mode == SWAP_TYPE_KTX)
    {
      glDrawBuffer(staticbuffer); 
      if (!goodZ)
      {
	glDrawBufferRegion(zbuffer_region,0,0,Width,Height,0,0);
	glDrawBufferRegion(cbuffer_region,0,0,Width,Height,0,0);
      }
      {printf("glutSwapBuffers()\n"); glutSwapBuffers();} 
      glDrawBuffer(screenbuffer); 
      rendersetup();
      return;
    }

    // get fresh copy of static data
#ifdef MACOS_X_TEST2
    if (goodZ) // If we just rendered on a mac, backup colorbuf to RAM.
    {
      SaveColorBuffer();
      glDrawBuffer(screenbuffer); // set pixel destination
      CopyColorBuffer(staticbuffer, screenbuffer);
    }
    else
      RestoreColorBuffer();
#else
    glDrawBuffer(screenbuffer); // set pixel destination
    CopyColorBuffer(staticbuffer, screenbuffer);
#endif
    if (!goodZ) // Don't bother with z restore if we just rendered.
      RestoreDepthBuffer();
    // screenbuffer is ready for dynamic data
  }
  else
  {
    XORcurPiece(); // Erase the previous piece
    if (movingpiece != curpiece)
    {
      // Render the buffer without the new moving piece.
      Select1Part(curpiece);
      glDrawBuffer(staticbuffer); 
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      render();
      {printf("glutSwapBuffers()\n"); glutSwapBuffers();}
      glDrawBuffer(screenbuffer); 
      UnSelect1Part(curpiece);
    }
  }
}
      
/***************************************************************/
void DrawTNTPiece(void)
{
  int drawmode = ldraw_commandline_opts.F;

  //ldraw_commandline_opts.F |= TYPE_F_STUDLESS_MODE;
  //ldraw_commandline_opts.F = (TYPE_F_NO_POLYGONS | TYPE_F_STUDLESS_MODE);
  ldraw_commandline_opts.F = TYPE_F_NO_POLYGONS;
  if ((drawmode & TYPE_F_STUDLINE_MODE) != 0)
    ldraw_commandline_opts.F |= TYPE_F_STUDLINE_MODE;
  ldraw_commandline_opts.F &= ~(TYPE_F_SHADED_MODE); // zShading = 0;

  //glDisable(GL_LIGHTING);

  glColor3f(1.0, 1.0, 1.0); // white
  glCurColorIndex = -2;

  z_line_offset += 1.0;

#if 1
  glLogicOp(GL_XOR);
  ldraw_commandline_opts.F |= TYPE_F_XOR_MODE;

  glDepthFunc(GL_LESS);
  glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE); //disable color updates
  glEnable( GL_COLOR_LOGIC_OP ); 
  DrawCurPart(15);

  glDepthFunc(GL_EQUAL);     // New value always wins.
  glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE); //enable color buffer updates
  DrawCurPart(15);

  glLogicOp(GL_COPY);
  ldraw_commandline_opts.F &= ~(TYPE_F_XOR_MODE);
  glDepthFunc(GL_LESS);     // New value always wins.
  glDisable( GL_COLOR_LOGIC_OP ); 

  // Restore the depth buffer
  if (buffer_swap_mode == SWAP_TYPE_KTX)
  {
    glDrawBuffer(staticbuffer); 
    glDrawBufferRegion(zbuffer_region,0,0,Width,Height,0,0);
    glDrawBuffer(screenbuffer); 
    rendersetup();
  }

#else
  DrawCurPart(15);
#endif

  z_line_offset -= 1.0;

  ldraw_commandline_opts.F = drawmode;
}

/***************************************************************/
void DrawMovingPiece(void)
{
  if (SOLID_EDIT_MODE)
  {
    Get1PartBox(curpiece, sc);
    if (ldraw_commandline_opts.debug_level == 1)
      printf("dp_sbox = %d, %d, %d, %d\n", sc[0], sc[1], sc[2], sc[3]);
#ifndef SAVE_DEPTH_ALL
    // Save depth buffer BEFORE drawing the current part into it.
    SaveDepthBuffer();
#else
    // If not using depth box then the whole window was saved so
    // we can skip the save here.  Its only needed if the box moves.
#endif
#ifdef MACOS_X_TEST2
#ifndef SAVE_COLOR_ALL
    SaveColorBuffer();
#endif
#endif
    if (ldraw_commandline_opts.F & TYPE_F_SHADED_MODE) // (zShading)
      glEnable(GL_LIGHTING);
    else
      glDisable(GL_LIGHTING);
    if (SOLID_EDIT_MODE == NVIDIA_XOR_HACK)
      DrawTNTPiece();
    else
      DrawCurPart(-1);
  }
  else
  {
    XORcurPiece();
  }
  printf("glFlush(DrawMovingPiece)\n"); glFlush();
}

/***************************************************************/
void display(void)
{
  int res;

  show_edit_mode_gui &= 1;  // Clear the clear gui bit (2).

#ifdef PART_BOX_TEST
  extern void Print1PartBox();

  Print1PartBox();
#endif

#ifdef TILE_RENDER_OPTION
  if (tiledRendering == 1)
  {
    TiledDisplay();
    return;
  }
#endif

  if (OffScreenRendering == 1)
  {
    OffScreenDisplay();
    return;
  }

  printf("Display()\n");

  if (editing) 
  {
    renderbuffer = screenbuffer;
    glDrawBuffer(screenbuffer); 
#ifdef MESA_XOR_TEST
    if (buffer_swap_mode == SWAP_TYPE_NODAMAGE) // Mesa
    {
      // Draw into back buffer in editing mode for XOR speed on moving part.
      renderbuffer = staticbuffer;
      glDrawBuffer(staticbuffer); 
    }
#endif
    if (res = exposeEvent()) // Expose event?
    {
      printf("EXPOSED window during editing\n");
      dirtyWindow = 1;
    }

    if (panning || dirtyWindow)
    {
      if (panning)
      {
	glDrawBuffer(staticbuffer); 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	render();
#ifdef MESA_XOR_TEST
	// Delay swap until after XOR moving part for Mesa.
	if (buffer_swap_mode != SWAP_TYPE_NODAMAGE) // Mesa
#endif
	{printf("glutSwapBuffers()\n"); glutSwapBuffers();}
	glDrawBuffer(renderbuffer); 
      }
      if (SOLID_EDIT_MODE)
      {
	if (!panning)
	{
	  if (movingpiece == curpiece)
	  {
	    glDrawBuffer(staticbuffer); 
	    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	    render();
	    SaveDepthBuffer();
	    CopyStaticBuffer(1);
	  }
	  else 
	  {
	    glDrawBuffer(renderbuffer); 
	    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	    render();
	  }	  
	}
	rendersetup();
	if (movingpiece == curpiece)
	  DrawMovingPiece();
	else
	  XORcurPiece();
      }
      else
      {
	if (!panning)
	{
	  if (movingpiece == curpiece)
	  {
	    Select1Part(curpiece);
	    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	    render();
	    UnSelect1Part(curpiece);
	  }
	  else
	  {
	    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	    render();
	  }
	}
	rendersetup();
	XORcurPiece();
      }
#ifdef MESA_XOR_TEST
      if (buffer_swap_mode == SWAP_TYPE_NODAMAGE) // Mesa
      {
	renderbuffer = screenbuffer;
	glDrawBuffer(screenbuffer); 
	// Solid moving part must be drawn in GL_FRONT if not panning.
	if (!SOLID_EDIT_MODE || (movingpiece != curpiece) || panning)
	  {printf("glutSwapBuffers()\n"); glutSwapBuffers();}
      }
#endif
      if (!panning)
      {
	edit_mode_gui();
      }
    }

    dirtyWindow = 0;  // The window is nice and squeaky clean now.

    return;
  }

#ifdef USE_DOUBLE_BUFFER
  // If this is just an expose event (and !panning), then restore from backup.
  if ((!panning) && exposeEvent() && (dirtyWindow == 0))
  {
    CopyColorBuffer(renderbuffer, screenbuffer);
    return;
  }

  if (panning)
    renderbuffer = GL_BACK;  // Enable double buffer for spin mode.
  else if (zDetailLevel > TYPE_P)
    renderbuffer = GL_BACK;  // double buffer mode.
  else
#endif
    renderbuffer = GL_FRONT;  // Effectively disable double buffer.

  glDrawBuffer(renderbuffer);

  if (res = exposeEvent())
    dirtyWindow = 1;

  //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  // Non-continuous output stop after each step.
  if (ldraw_commandline_opts.M == 'P')
  {
    // Do not increment step right after (or during) panning.
    if (panning || dirtyWindow)
    {
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    else
    {
      if (stepcount == curstep)
      {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	curstep = 0; // Reset to first step
      }
      else
      {
#ifndef ALWAYS_REDRAW
	// XOR erase the previous comment.
	sprintf(buf,"Step %d of %d.  ",curstep+1, stepcount+1);
	strcat(buf, "Click on drawing to continue.");
	platform_write_step_comment(buf);

	// Save the cropping extents from the previous step
	// so I can restore them after zReset().
	save_extents();
#else
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#endif
	curstep++; // Move on to next step
      }
    }
  }
  else
  {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }

#ifdef AGL
  // This works!  Does glClear switch to back buffer after clearing?
  // But GlutSwapBuffer() should also switch to FRONT before
  // Unhiding the cursor.
  if (!panning) // AMesa has trouble with GL_FRONT in double buffer mode.
    glDrawBuffer(GL_FRONT);
#endif

  render();
  if ((ldraw_commandline_opts.M == 'S') && qualityLines)
  {
    //NOTE: should do this in DrawScene() to fix qualityLines 
    // in tiled render and offscreen render.
    while (stepcount != curstep)
    {
      save_extents();
      curstep++;
      render();
      merge_extents();
    }
  }
  
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();

  // Non-continuous output stop after each step.
  if (ldraw_commandline_opts.M == 'P')
  {
    // Do not display step comment during panning.
    if (!panning)
    {
      sprintf(buf,"Step %d of %d.  ",curstep+1, stepcount+1);
      if (stepcount == curstep)
         strcat(buf, "Finished.");
      else 
         strcat(buf, "Click on drawing to continue.");
      platform_step_comment(buf);
      
#ifndef ALWAYS_REDRAW
      // Restore the cropping extents from the previous step if needed.
      if (curstep > 0)
	merge_extents();
#endif
    }
  }

  //printf("glFlush(Display)\n"); glFlush();

  glPopMatrix();

  //  if (!selection)
  //    {printf("glutSwapBuffers()\n"); glutSwapBuffers();}

  if (renderbuffer == GL_FRONT)
    {printf("glFinish(Display)\n"); glFinish();}
  else
    {printf("Skipping glFinish(Display)\n");}

#ifdef USE_DOUBLE_BUFFER
  if (panning)
    {printf("glutSwapBuffers()\n"); glutSwapBuffers();}
  else if (zDetailLevel > TYPE_P)
    // Get every last update (including text) into the front buffer.
    CopyColorBuffer(renderbuffer, screenbuffer);
  else 
  {
    // Copy every last update (including text) into the back buffer.
    renderbuffer = GL_BACK; // Pretend we rendered it there (for expose events)
    CopyColorBuffer(screenbuffer, staticbuffer);
  }
#endif

  dirtyWindow = 0;  // The window is nice and squeaky clean now.

  // If we just want the output files then quit when idle.
  if ((ldraw_commandline_opts.output == 1) ||
      (ldraw_commandline_opts.M == 'S') || (ldraw_commandline_opts.M == 'F'))
  {
    if (ldraw_commandline_opts.M == 'F')
      platform_step(INT_MAX, 0, -1, NULL);
      
    // Do NOT quit just yet if we need to twirl.
    if (ldraw_commandline_opts.rotate != 1) 
      exit(0); //quit the program
  }

#if 0
  // This does NOT work.  
  // glutLayerGet returns the SAME value as when display() starts.
  // It does NOT get updated until after display is finished.
  // Delaying CopyColorBuffer() until glutidle did NOT work either.
  // So, how can I tell if the display was successful? 
  // Draw to back buffer and then copy to front?
  if (res = exposeEvent())
  {
    printf("DAMAGED during redisplay\n");
    dirtyWindow = 1;
  }
  else
  {
    printf("UNdamaged redisplay\n");
    //CopyColorBuffer(screenbuffer, staticbuffer);    
  }
#endif

  //printf("glFinish(Display)\n"); glFinish();

}

/***************************************************************/
void initCamera(void)
{
  fCamX = 0.0;
  fCamY = 0.0;
  fCamZ = 0.0;
  fXRot = 0.0;
  fYRot = 0.0;
  fZRot = 0.0;

#ifdef USE_F00_CAMERA
  resetCamera();
#endif
}

void rotate_about(float x, float y, float z, float degrees);

/***************************************************************/
void parse_view(char *viewMatrix)
{
      sscanf(viewMatrix,"%f,%f,%f,%f,%f,%f,%f,%f,%f",
	     &(ldraw_commandline_opts.A.a),
	     &(ldraw_commandline_opts.A.b),
	     &(ldraw_commandline_opts.A.c),
	     &(ldraw_commandline_opts.A.d),
	     &(ldraw_commandline_opts.A.e),
	     &(ldraw_commandline_opts.A.f),
	     &(ldraw_commandline_opts.A.g),
	     &(ldraw_commandline_opts.A.h),
	     &(ldraw_commandline_opts.A.i));
}

/***************************************************************/
void EraseCurPiece(void)
{
  if ((SOLID_EDIT_MODE) && (movingpiece == curpiece))
  {
#ifdef MESA_XOR_TEST
    if (buffer_swap_mode == SWAP_TYPE_NODAMAGE)
    {
      rendersetup();
      // Make sure the old piece is drawn in back buffer, not just the front.
      glDrawBuffer(staticbuffer);
      glDepthFunc(GL_EQUAL); // Tricky!  We already drew it in the front buf.
      DrawCurPart(-1); //Draw1Part(curpiece, -1);
      glDepthFunc(GL_LESS);
      printf("glFlush(EraseCurPiece)\n"); glFlush(); // Force drawing now.
      glDrawBuffer(renderbuffer);
    }
#endif
    UnSelect1Part(curpiece);
    movingpiece = -1;
  }
  else
    XORcurPiece(); // Erase the previous piece
}

/***************************************************************/
void TranslateCurPiece(float m[4][4])
{
  CopyStaticBuffer(0);
  Translate1Part(curpiece, m);
  movingpiece = curpiece;
  DrawMovingPiece();
  if (ldraw_commandline_opts.debug_level == 1)
      Print1Part(curpiece, stdout);
  edit_mode_gui();
}

/***************************************************************/
void UnLightCurPiece(void)
{
  EraseCurPiece();
  if ((DrawToCurPiece)
      || ((SOLID_EDIT_MODE) && (NVIDIA_XOR_HACK))
      )
  {
    //if (movingpiece != curpiece)
    {
      rendersetup();
      Draw1Part(curpiece, -1);
    }
  }
}

/***************************************************************/
void HiLightNewPiece(int piecenum)
{
  curpiece = Find1Part(piecenum);
  if (movingpiece >= 0)
  {
    movingpiece = -1;
    if (SOLID_EDIT_MODE)
    {
      glDepthMask(GL_TRUE); // enable updates to depth buffer
      dirtyWindow = 1;
      glutPostRedisplay();
      if (ldraw_commandline_opts.debug_level == 1)
	  Print1Part(curpiece, stdout);
      edit_mode_gui();
      return;
    }
    else
    {
      glDrawBuffer(staticbuffer); 
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      render();
      {printf("glutSwapBuffers()\n"); glutSwapBuffers();}
      glDrawBuffer(screenbuffer); 
    }
  }
  XORcurPiece();
  if (ldraw_commandline_opts.debug_level == 1)
      Print1Part(curpiece, stdout);
  edit_mode_gui();
}

/***************************************************************/
void HiLightCurPiece(int piecenum)
{
  UnLightCurPiece();
  HiLightNewPiece(piecenum);
}

/***************************************************************/
void InsertNewPiece(void)
{
#if 0  
  if (movingpiece >= 0)
  {
    if (SOLID_EDIT_MODE)
    {
      EraseCurPiece(); // Unselect current piece
      // Add the current moving piece into the staticbuffer
      // Dont bother, CopyStaticBuffer gets a new copy of z-buffer.
      //glDrawBuffer(staticbuffer); 
      //DrawCurPart(-1); 
      //glDrawBuffer(screenbuffer); 
    }
    else
    {
      glDrawBuffer(staticbuffer); 
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      render();
      {printf("glutSwapBuffers()\n"); glutSwapBuffers();}
      glDrawBuffer(screenbuffer); 
    }
  }
  else 
    EraseCurPiece();
  if (DrawToCurPiece)
  {
    //if (movingpiece != curpiece)
    {
      rendersetup();
      Draw1Part(curpiece, -1);
    }
  }
  curpiece = Add1Part(curpiece);
  if (SOLID_EDIT_MODE)
  {
    movingpiece = -1; // Make sure movingpiece != curpiece so we save z-buffer.
    CopyStaticBuffer(1); 
  }
  movingpiece = curpiece;
  DrawMovingPiece();
  if (ldraw_commandline_opts.debug_level == 1)
      Print1Part(curpiece, stdout);
  edit_mode_gui();
#else
  UnLightCurPiece();
  movingpiece = -1; // Make sure movingpiece != curpiece so we save z-buffer.
  curpiece = Add1Part(curpiece);
  HiLightNewPiece(curpiece);
#endif
}

/***************************************************************/
void DelCurPiece(void)
{
  CopyStaticBuffer(0); 
  if (SOLID_EDIT_MODE)
  {
    // CopyStaticBuffer(0) selects current piece in SOLID_EDIT_MODE
  }
  else
    Select1Part(curpiece);
  Delete1Part(curpiece);
  movingpiece = -1;
  curpiece--;
  if (curpiece < 0) curpiece = 0;
  XORcurPiece();
  if (ldraw_commandline_opts.debug_level == 1)
      Print1Part(curpiece, stdout);
  edit_mode_gui();
}

/***************************************************************/
void SetTitle(int showit)
{
  char filename[256];

  // Change the title of the window to show the new dat filename.
  concat_path(datfilepath, datfilename, filename);
  if (filename[0] == '.') // I hate the ./filename thing.
    sprintf(title, "%s - %s", progname, datfilename);
  else
    sprintf(title, "%s - %s", progname, filename);

  // NOTE:  This might crash in fullscreen game mode.  Should check first.
  if (showit)
    glutSetWindowTitle(title);
}

/***************************************************************/
void loadnewdatfile(char *datpath, char *datfile)
{
  strcpy(datfilename, datfile);
  strcpy(datfilepath, datpath);
  strcpy(dirfilepath, datpath);
  platform_setdir();

  SetTitle(1); // Change title of the window to show the new dat filename.
  
  if (editing) 
  {
    UnSelect1Part(curpiece); // UnSelect part before Loading
    curpiece = 0;
    movingpiece = -1;
  }

#ifdef USE_L3_PARSER
  if (parsername == L3_PARSER)
    list_made = 0; // Gotta reparse the file.
#endif
  curstep = 0; // Reset to first step
  dirtyWindow = 1;

  glutPostRedisplay();
}

/***************************************************************/
void saveasdatfile(char *datpath, char *datfile)
{
#ifdef USE_L3_PARSER
  int i = -1;

  strcpy(datfilename, datfile);
  strcpy(datfilepath, datpath);
  strcpy(dirfilepath, datpath); // I think this is only used by glut menus.
  platform_setdir();

  if (editing)
    i = UnSelect1Part(curpiece); // Link part back in before printing
  Print1Model(datfilename);
  SetTitle(1); // Change the title of the window.
  if (i != -1)
    Select1Part(curpiece); // Unlink part again if needed.
#endif
}

/***************************************************************/
int resetpartlist()
{
  memcpy(partlistptr, partliststr, sizeof (char *) *partlisttotal);
  partlistsize = partlisttotal;
  partlookup = 1;
}

/***************************************************************/
char *loadpartlist(void)
{
  char filename[256];
  FILE *fp;
  long filesize;
  char *buffer;
  unsigned char *s;
  int i;
  char seps[] = "\r\n"; 
  
  partlookup = 0;

  if (partlistbuf)
  {
    resetpartlist();
    if (prevlookup > 0)
      partlookup = prevlookup;
    return(partlistbuf);
  }
  
  concat_path(pathname, use_uppercase ? "PARTS.LST" : "parts.lst", filename);
  fp = fopen(filename, "rb");
  if (!fp)
  {
    printf("Could not open %s\n", filename);
    return(NULL);
  }

  // How big is the file.
  fseek (fp , 0 , SEEK_END);
  filesize = ftell (fp);
  rewind (fp);

  // allocate enough memory to read the entire file.
  filesize++; // add an extra NULL char to the end just in case.
  buffer = (char*) malloc (filesize);
  if (!buffer) 
    return(NULL);

  fread (buffer,1,filesize,fp);
  fclose (fp);
  
  buffer[filesize-1] = 0;

  partlistmax = 2000;
  partliststr = (char **) calloc(sizeof (char *), partlistmax);

  // Save a pointer to each line
  for (i = 0, s = strtok( buffer, seps );
       s != NULL;
       s = strtok( NULL, seps ), i++ )
  {
    if (i >= partlistmax)
    {
      partlistmax += 1000;
      partliststr = realloc(partliststr, partlistmax * sizeof(char *));
    }
    partliststr[i] = s;
  }
  printf("Found %d parts\n", i);

  if (i == 0)
  {
    free(buffer);
    buffer = NULL;
    return(NULL);
  }

  partlisttotal = i;
  partlistptr = (char **) calloc(sizeof(char *), partlisttotal);
  resetpartlist();

  partlistbuf = buffer;
  return(partlistbuf);
}

/***************************************************************/
char *stristr(char *src, char *dst)
{
  char *n;
  char ss[256];
  char dd[256];

  // Copy and lowercase
  if (strlen(src) > 255)
    src = strdup(src);
  else 
  {
    strcpy(ss, src);
    src = ss;
  }
  _strlwr(src);
  
  // Copy and lowercase
  if (strlen(dst) > 255)
    dst = strdup(dst);
  else 
  {
    strcpy(dd, dst);
    dst = dd;
  }
  _strlwr(dst);
  
  n = strstr(src, dst);

  if (strlen(src) > 255)
    free(src);
  if (strlen(dst) > 255)
    free(dst);

  return n;
}

/***************************************************************/
int limitpartlist(char *str)
{
  int i, j, n;
  UNUSED(i);

  j = 0;
  for (n = 0; n < partlisttotal; n++)
    if (stristr(partliststr[n], str))
    {
      partlistptr[j++] = partliststr[n];
    }

  if (j > 0)
  {
    partlistsize = j;
    partlookup = 1;
  }
  else
    resetpartlist();

  return 0;
}

/***************************************************************/
int resetpluglist()
{
  memcpy(pluglistptr, plugliststr, sizeof (char *) *pluglisttotal);
  pluglistsize = pluglisttotal;
  pluglookup = 1;

  return 0;
}

/***************************************************************/
int saveprevlookup()
{
  int n;

  for (n = 0; n < partlisttotal; n++)
    if ( partlistptr[partlookup] == partliststr[n] )
      return n;

  return 0;
}

/***************************************************************/
#  include <dirent.h>
#  include <sys/stat.h>
#  include <unistd.h>
  
/***************************************************************/
char *loadpluglist(void)
{
  char plugpath[512];
  char filename[512];
	  
  DIR			*dirp;
  struct dirent		*dir;
  struct stat		statbuf;
  int			i = 0;
  int			n = 0;

  long filesize;
  char *buffer;
  unsigned char *s;
  char seps[] = "\r\n"; 

#ifdef WINDOWS
  char plugext[] = ".dll"; 
#else
  char plugext[] = ".so"; 
#endif
  
  pluglookup = 0;

  if (pluglistbuf)
  {
    resetpluglist();
    return(pluglistbuf);
  }

  // allocate enough memory to read the entire file.
  filesize = 2000;
  buffer = (char*) malloc (filesize);
  if (!buffer) 
    return(NULL);

  concat_path(progpath, "plugins", plugpath);
  strcpy(buffer, "");
  
  if ((dirp = opendir(plugpath)) == NULL) 
    return(NULL);

  while ((dir = readdir(dirp)) != NULL) {
    if (dir->d_name[0] == '.')
      continue;
    stat(dir->d_name,&statbuf);
    if ((statbuf.st_mode & S_IFMT) == S_IFDIR) // Skip directories
      continue;
    
    strcpy(filename, dir->d_name);
    filename[strlen("plugin")] = 0;

    if (stricmp(filename, "plugin"))
      continue;
    
    strcpy(filename, (dir->d_name + strlen("plugin")));
    if ((i = strlen(filename)) < (strlen(plugext)+1))
      continue;
    
    i -= strlen(plugext);
    if (stricmp(filename+i, plugext))
      continue;
		
    filename[i] = 0;
    if ((strlen(buffer) + strlen(filename) + 2) > filesize)
    {
      filesize += 1000;
      buffer = realloc(buffer, filesize * sizeof(char));
    }
    strcat(buffer, filename);
    strcat(buffer, "\n");
    n++;
  }

  pluglistmax = n;
  plugliststr = (char **) calloc(sizeof (char *), pluglistmax);

  // Save a pointer to each line
  for (i = 0, s = strtok( buffer, seps );
       s != NULL;
       s = strtok( NULL, seps ), i++ )
  {
    if (i >= pluglistmax)
    {
      pluglistmax += 1000;
      plugliststr = realloc(plugliststr, pluglistmax * sizeof(char *));
    }
    plugliststr[i] = s;
  }
  printf("Found %d plugins\n", i);

  if (i == 0)
  {
    free(buffer);
    buffer = NULL;
    return(NULL);
  }

  pluglisttotal = i;
  pluglistptr = (char **) calloc(sizeof(char *), pluglisttotal);
  resetpluglist();

  pluglistbuf = buffer;

  plugins = (plugstruct **) calloc(sizeof(plugstruct *), pluglisttotal);
  for (i = 0; i < pluglisttotal; i++)
  {
    strcpy(plugpath, "plugins");
    concat_path(plugpath, "plugin", filename);
    strcat(filename, plugliststr[i]);
    strcat(filename, plugext);
    plugins[i] = pluginfo(filename);
  }

  return(pluglistbuf);
}

/***************************************************************/
int runplugin(int n)
{
  UNUSED(n);
  int i;
  char pline[4][100];
  char *partname;

  unsigned char CompleteText[8192] = "";
  unsigned char SelText[200] = "";
  unsigned long SelStart = 0;
  unsigned long SelLength = 0;
  unsigned long CursoRow = 0;
  unsigned long CursorColum = 0;

  strcpy(&(ecommand[1]), pluglistptr[pluglookup - 1]);
  printf("selected plugin %s\n",&(ecommand[1]));

  for (i = 0; i < pluglisttotal; i++)
  {
    if (plugliststr[i] == pluglistptr[pluglookup - 1])
      break;
  }

  if (i >= pluglisttotal)
    return -1;

  if (plugins[i] == NULL)
    return -1;

  if (!strncmp(plugins[i]->plugtype, "2", 1))
  {
    // Requires a selection to work.
    // Gotta ask for a number of lines to select.
    Print3Parts(curpiece, pline[1], pline[2], pline[3]);

    // Plugintype can be one of the following values
    // 0 = Plugin can be called always
    // 1 = Plugin can only be called if no text is selected
    // 2 = Plugin can only be called if a portion of text is selected

    if ( !strncmp(pluglistptr[pluglookup - 1], "BezierCurves", 12))
    {
      strcpy(CompleteText, pline[1]);
      strcat(CompleteText, "\n");
      strcat(CompleteText, pline[2]);
      strcat(CompleteText, "\n");
      strcpy(SelText, CompleteText);
      SelLength = strlen(SelText);
    }
    else
    {
      strcpy(CompleteText, pline[1]);
      strcpy(SelText, CompleteText);
      strcat(CompleteText, "\n");
      SelLength = strlen(SelText);
    }
      
  }

  partname = plugin(plugins[i], CompleteText, SelText, &SelStart, &SelLength, 
		    &CursoRow, &CursorColum);

  if (partname)
  {
#ifdef WINDOWS
    // Sneak the new stuff in via the CUTnPASTE buffer.
    HWND GetHwndFocus();
    HWND hwnd;

    hwnd = GetHwndFocus();

    pastelist = strdup(partname);
    //SendKeyToCurrentApp( 'V' );
    PostMessage(hwnd, WM_KEYDOWN, 'V', 0x8001);
#else
    // We have some memory troubles on return from the plugin.  Yech!
    // Might be a busted opengl context.  SaveDepthBuffer() chokes.
    InsertNewPiece();

    CopyStaticBuffer(0);
    movingpiece = curpiece;
    if (strrchr(partname, '.') == NULL)
      strcat(partname, use_uppercase ? ".DAT" : ".dat");
    Swap1Part(curpiece, partname);
    DrawMovingPiece();
    if (ldraw_commandline_opts.debug_level == 1)
      Print1Part(curpiece, stdout);
#endif
  }
  return 0;
}

/***************************************************************/
int limitpluglist(char *str)
{
  int i, j, n;
  UNUSED(i);

  j = 0;
  for (n = 0; n < pluglisttotal; n++)
    if (stristr(plugliststr[n], str))
    {
      pluglistptr[j++] = plugliststr[n];
    }

  if (j > 0)
  {
    pluglistsize = j;
    pluglookup = 1;
  }
  else
    resetpluglist();
  return 0;
}

/***************************************************************/
void move1matrix(float m[4][4], float dx, float dy, float dz)
{
#if 0
  // Consider projecting (dx,dy,dz) from modelview to world coords.
  // Whichever component is largest is the axis to move along.
  // Or use dot product of (dx,dy,dz) with projected(1,1,1) ???
  M4M4Mul(m,m_m,LinePtr->v); // Adjust center point of part by view matrix.
  printf("Project(%.3f, %.3f, %.3f)\n", x, y, z);
#endif
  
  if (m_viewMatrix == Back)
  {
    m[0][3] -= (dx * moveXamount);
    m[1][3] -= (dy * moveYamount);
    m[2][3] -= (dz * moveZamount);
  }
  else if (m_viewMatrix == Left)
  {
    m[0][3] -= (dz * moveXamount);
    m[1][3] -= (dy * moveYamount);
    m[2][3] += (dx * moveZamount);
  }
  else if (m_viewMatrix == Right)
  {
    m[0][3] += (dz * moveXamount);
    m[1][3] -= (dy * moveYamount);
    m[2][3] -= (dx * moveZamount);
  }
  else if (m_viewMatrix == Above)
  {
    m[0][3] -= (dy * moveXamount);
    m[1][3] += (dz * moveYamount);
    m[2][3] += (dx * moveZamount);
  }
  else if (m_viewMatrix == Beneath)
  {
    m[0][3] += (dy * moveXamount);
    m[1][3] -= (dz * moveYamount);
    m[2][3] += (dx * moveZamount);
  }
  else if (m_viewMatrix == Front)
  {
    m[0][3] += (dx * moveXamount);
    m[1][3] -= (dy * moveYamount);
    m[2][3] += (dz * moveZamount);
  }
  else
  {
    m[0][3] += (dx * moveXamount);
    m[1][3] -= (dz * moveYamount);
    m[2][3] += (dy * moveZamount);
  }
}

/***************************************************************/
void enterEditMode()
{
    if ((glutModifiers & GLUT_ACTIVE_CTRL) != 0)
      SOLID_EDIT_MODE = 1;
    else 
      SOLID_EDIT_MODE = NVIDIA_XOR_HACK; // 0
    if ((glutModifiers & GLUT_ACTIVE_ALT) != 0)
    {
      //ldraw_commandline_opts.F |= TYPE_F_STUDLESS_MODE;
      ldraw_commandline_opts.F |= TYPE_F_STUDLINE_MODE;
      DrawToCurPiece = 1;
    }
    editing ^= 1;
    // if (ldraw_commandline_opts.debug_level == 1)
    printf("Editing mode =  %d{%d}\n", editing, SOLID_EDIT_MODE);
    
    // Just in case.  Make sure we draw to GL_FRONT buffer.
    renderbuffer = screenbuffer;
    glDrawBuffer(screenbuffer); 

    // Switch to continuous mode.
    editingprevmode = ldraw_commandline_opts.M;
    ldraw_commandline_opts.M = 'C';
    //ldraw_commandline_opts.poll = 0; // Disable polling 
    
    if (editingprevmode != 'C')
      if (stepcount != curstep)
	glutPostRedisplay();  // gotta redisplay to draw the whole thing.

    curstep = 0; // Reset to first step
    dirtyWindow = 1;

    curpiece = 0;
    movingpiece = -1;
    if (parsername == LDLITE_PARSER)
    {
      parsername = L3_PARSER;
      use_quads = 1;
      list_made = 0; // Gotta reparse the file.
      glutPostRedisplay();
    }
    else
    {
      XORcurPiece();
      if (ldraw_commandline_opts.debug_level == 1)
	  Print1Part(curpiece, stdout);
      edit_mode_gui();
    }
}

/***************************************************************/
void leaveEditMode()
{
  if (SOLID_EDIT_MODE)
  {
    glDepthMask(GL_TRUE); // Just to be make sure 
    if (movingpiece == curpiece)
    {
      UnSelect1Part(curpiece);
      movingpiece = -1;
    }
  }
  // if (parsername == L3_PARSER)
  editing ^= 1;
  // if (ldraw_commandline_opts.debug_level == 1)
  printf("Editing mode =  %d\n", editing);
  
  XORcurPiece(); // Erase the previous piece
  
  // Restore previous continuous mode.
  ldraw_commandline_opts.F &= ~(TYPE_F_STUDLINE_MODE);
  ldraw_commandline_opts.M = editingprevmode;
  dirtyWindow = 1;
  glutPostRedisplay();
}

/***************************************************************/
int edit_mode_fnkeys(int key, int x, int y)
{
  UNUSED(x);
  UNUSED(y);
  int i;
  float m[4][4] = {
    {1.0,0.0,0.0,0.0},
    {0.0,1.0,0.0,0.0},
    {0.0,0.0,1.0,0.0},
    {0.0,0.0,0.0,1.0}
  };

  //printf("key = %d = '%c' (%08x)\n",key,key, glutModifiers);
  if (!editing) // See if we need to enter edit mode
  {
    if (key != GLUT_KEY_INSERT)
      return 0; // Not already editing and not entering editing mode.

    enterEditMode();

    return 1;
  }

  // I dont think there are any modified LEDIT fn keys.
  // But enterEditMode() above does check for modifiers.
  if ((glutModifiers & GLUT_ACTIVE_ALT) ||
      (glutModifiers & GLUT_ACTIVE_SHIFT) ||
      (glutModifiers & GLUT_ACTIVE_CTRL))
  {
    return 0;
  }

  // If we have a command going then work on that.
  if (i = strlen(ecommand))
  {
    if (partlookup)
    {
      switch(key) {
      case GLUT_KEY_PAGE_UP:
	partlookup -= 10;
	if (partlookup < 1)
	  partlookup = 1;
	edit_mode_gui(); // Redisplay the GUI
	break;
      case GLUT_KEY_PAGE_DOWN:
	partlookup += 10;
	if (partlookup > partlistsize)
	  partlookup = partlistsize;
	edit_mode_gui(); // Redisplay the GUI
	break;
      case GLUT_KEY_UP:
	partlookup -= 1;
	if (partlookup < 1)
	  partlookup = 1;
	edit_mode_gui(); // Redisplay the GUI
	break;
      case GLUT_KEY_DOWN:
	partlookup += 1;
	if (partlookup > partlistsize)
	  partlookup = partlistsize;
	edit_mode_gui(); // Redisplay the GUI
	break;
      default:
	if (ldraw_commandline_opts.debug_level == 1)
	  printf("fnkey = %d = '%c'\n",key,key);
	edit_mode_gui(); // Redisplay the GUI
	break;
      }
    return 1;
    }

    if (pluglookup)
    {
      switch(key) {
      case GLUT_KEY_PAGE_UP:
	pluglookup -= 10;
	if (pluglookup < 1)
	  pluglookup = 1;
	edit_mode_gui(); // Redisplay the GUI
	break;
      case GLUT_KEY_PAGE_DOWN:
	pluglookup += 10;
	if (pluglookup > pluglistsize)
	  pluglookup = pluglistsize;
	edit_mode_gui(); // Redisplay the GUI
	break;
      case GLUT_KEY_UP:
	pluglookup -= 1;
	if (pluglookup < 1)
	  pluglookup = 1;
	edit_mode_gui(); // Redisplay the GUI
	break;
      case GLUT_KEY_DOWN:
	pluglookup += 1;
	if (pluglookup > pluglistsize)
	  pluglookup = pluglistsize;
	edit_mode_gui(); // Redisplay the GUI
	break;
      default:
	if (ldraw_commandline_opts.debug_level == 1)
	  printf("fnkey = %d = '%c'\n",key,key);
	edit_mode_gui(); // Redisplay the GUI
	break;
      }
    return 1;
    }

    switch(key) {
    default:
      if (ldraw_commandline_opts.debug_level == 1)
	  printf("fnkey = %d = '%c'\n",key,key);
      edit_mode_gui(); // Redisplay the GUI
      break;
    }
    return 1;
  }
  
  switch(key) {
  case GLUT_KEY_INSERT:
    leaveEditMode();
    break;
  case GLUT_KEY_PAGE_UP:
    HiLightCurPiece(curpiece-1);
    break;
  case GLUT_KEY_PAGE_DOWN:
    HiLightCurPiece(curpiece+1);
    break;
  case GLUT_KEY_RIGHT:
    move1matrix(m, 1.0, 0.0, 0.0);
    //    m[0][3] += moveXamount;
    TranslateCurPiece(m);
    break;
  case GLUT_KEY_LEFT:
    move1matrix(m, -1.0, 0.0, 0.0);
    //    m[0][3] -= moveXamount;
    TranslateCurPiece(m);
    break;
  case GLUT_KEY_UP:
    move1matrix(m, 0.0, 1.0, 0.0);
    //    m[2][3] += moveZamount;
    TranslateCurPiece(m);
    break;
  case GLUT_KEY_DOWN:
    move1matrix(m, 0.0, -1.0, 0.0);
    //    m[2][3] -= moveZamount;
    TranslateCurPiece(m);
    break;
  case GLUT_KEY_HOME:
    move1matrix(m, 0.0, 0.0, 1.0);
    //    m[1][3] -= moveYamount;
    TranslateCurPiece(m);
    break;
  case GLUT_KEY_END:
    move1matrix(m, 0.0, 0.0, -1.0);
    //    m[1][3] += moveYamount;
    TranslateCurPiece(m);
    break;
  default:
    break;
  }
  return 1;
}

/***************************************************************/
#define MAIN_MENU_ID 	'/'
#define FILE_MENU_ID 	'F'
#define EDIT_MENU_ID 	'E'
#define VIEW_MENU_ID 	'V'
#define TURN_MENU_ID 	'T'
#define PIECE_MENU_ID 	'P'
#define OPTS_MENU_ID 	'O'

#define OFFSET_MENU_ID 	'o'
#define OFFSET_X_ID	'1'
#define OFFSET_Y_ID	'2'
#define OFFSET_Z_ID	'3'
#define OFFSET_V_ID	'4'

#define FILE_LOAD_ID 	'L'
#define FILE_SAVE_ID 	'S'
#define FILE_EXIT_ID 	'E'

#define TURN_X_ID	'X'
#define TURN_Y_ID	'Y'
#define TURN_Z_ID	'Z'

// C-X, C-Y, C-Z, C-O
#define ROTATE_X_ID	24
#define ROTATE_Y_ID	25
#define ROTATE_Z_ID	26
#define ROTATE_O_ID	15

#define MOVE_X_ID	'x'
#define MOVE_Y_ID	'y'
#define MOVE_Z_ID	'z'
#define MOVE_V_ID	'v'

#define GOTO_MENU_ID	'G'
#define COLOR_MENU_ID	'c'
#define PART_SWAP_ID	'p'
#define PART_SCALE_ID	's'
#define PART_MATRIX_ID	'm'
#define PART_LOC_ID	'l'

#define EDIT_LINE_ID    'e'
#define EDIT_COMMENT_ID 'C'
#define EDIT_HOSE_ID    'h'
#define EDIT_PLUGIN_ID  'g'

#define LINETYPE_2_ID	2
#define LINETYPE_3_ID	3
#define LINETYPE_4_ID	4
#define LINETYPE_5_ID	5

/*****************************************************************************/
char *ScanPoints(float m[4][4], int numpoints, char *str)
{
    int  i, j;
    char seps[] = "()[]{}<> ,\t"; // Allow parens and commas for readability.
    char *token;
      
    for (i = 0, j = 0,token = strtok( str, seps );
	 token != NULL;
	 token = strtok( NULL, seps ), i++ )
    {
      if (i > 2)
      {
	i = 0;
	j++;
      }
      if (j >= numpoints)
	break;
      sscanf(token, "%f", &m[j][i]);
    }
    return token; // This is not NULL if there is more of str left to parse.
}

/*****************************************************************************/
char *ScanPOINTS(double m[4][4], int numpoints, char *str)
{
    int  i, j;
    char seps[] = "()[]{}<> ,\t"; // Allow parens and commas for readability.
    char *token;
      
    for (i = 0, j = 0,token = strtok( str, seps );
	 token != NULL;
	 token = strtok( NULL, seps ), i++ )
    {
      if (i > 2)
      {
	i = 0;
	j++;
      }
      if (j >= numpoints)
	break;
      sscanf(token, "%lf", &m[j][i]);
    }
    return token; // This is not NULL if there is more of str left to parse.
}

/***************************************************************/
char *getfilename(char *s, char *filename)
{
  // Allow filenames containing spaces and/or in quotes.

  // Strip away leading whitespace (spaces and tabs).
  s += strspn(s, " \t");
  // Remove leading quotes
  if (*s == '\"')
    s++;

  if (filename)
    strcpy(filename, s);
  else 
    filename = strdup(s);

  // Eliminate trailing whitespace.  AKA TrimRight() in L3Input.cpp.
  for (s = filename + (strlen(filename)-1); s >= filename; s--)
  {
    if ((*s == ' ') || (*s == '\t'))
      *s = 0;
    else
      break;
  }
  // Remove trailing quotes.
  if ((s = strrchr(filename, '\"')) != NULL)
    *s = 0;
  
  return(filename);
}

/***************************************************************/
int edit_mode_keyboard(int key, int x, int y)
{
  UNUSED(x);
  UNUSED(y);

  int newview = 0;
  char c;
  
  float m[4][4] = {
    {1.0,0.0,0.0,0.0},
    {0.0,1.0,0.0,0.0},
    {0.0,0.0,1.0,0.0},
    {0.0,0.0,0.0,1.0}
  };
  float v[4][4];
  double angle;
  float f;
  int i, color;
  char partname[256];
  char *token;

  if (!editing)
    return 0;

  // If we have a command going then work on that.
  if (i = strlen(ecommand))
  {
    if (key == 27)
    {
      // Abort the command
      ecommand[0] = 0;
      prevlookup = saveprevlookup();
      partlookup = 0;
      pluglookup = 0;
      clear_edit_mode_gui();
      edit_mode_gui();
      return 1;
    }
    if (ecommand[0] == '/')
    {
      // Try to get submenu command
      switch(key) {
      case 'f':
	sprintf(eprompt[0], "File: ");
	sprintf(eprompt[1], "&Load &Save &Exit");
	ecommand[0] = toupper(key);
	ecommand[1] = 0;
	edit_mode_gui();
	break;
      case 'e':
	sprintf(eprompt[0], "Edit: ");
	sprintf(eprompt[1], "&Insert &Delete &Swap &Line-type  &Hoser &Plugins");
	ecommand[0] = toupper(key);
	ecommand[1] = 0;
	edit_mode_gui();
	break;
      case 'v':
	sprintf(eprompt[0], "View: ");
	sprintf(eprompt[1], "&Front &Right &Left &Back &Over &Under &Three-&D &In &Center");
	ecommand[0] = toupper(key);
	ecommand[1] = 0;
	edit_mode_gui();
	break;
      case 'p':
	sprintf(eprompt[0], "Piece: ");
	sprintf(eprompt[1], "&File &Color &Goto  &Location &Scale &Matrix  &Inline");
	ecommand[0] = toupper(key);
	ecommand[1] = 0;
	edit_mode_gui();
	break;
      case 'o':
	sprintf(eprompt[0], "Options: ");
	sprintf(eprompt[1], "&Line-as-stud &Start-at-line &Draw-to-current");
	ecommand[0] = toupper(key);
	ecommand[1] = 0;
	edit_mode_gui();
	break;
      case 't':
	sprintf(eprompt[0], "Turn: ");
	sprintf(eprompt[1], "&X-axis &Y-axis &Z-axis &Center-set  &Origin &Axis &Rotate");
	ecommand[0] = toupper(key);
	ecommand[1] = 0;
	edit_mode_gui();
	break;
      case 'r':
	sprintf(eprompt[0], "Rotate: ");
	sprintf(eprompt[1], "&X-axis &Y-axis &Z-axis");
	ecommand[0] = toupper(key);
	ecommand[1] = 0;
	edit_mode_gui();
	break;
      case 'h':
	show_edit_mode_gui ^= 1;
	show_edit_mode_gui |= 2;
	clear_edit_mode_gui();
	edit_mode_gui();
	break;
      case 'q':
	exit(0);
	break;
      default:
	break;
      }
      return 1;
    }
    if (ecommand[0] == 'o') // Offset Menu
    {
      // Try to get submenu command
      clear_edit_mode_gui();
      sprintf(eprompt[0], "Offset %c-axis: ", toupper(key));
      if (key == 'x')
	ecommand[0] = '1';
      if (key == 'y')
	ecommand[0] = '2';
      if (key == 'z')
	ecommand[0] = '3';
      if (key == 'v')
	ecommand[0] = '4';
      ecommand[1] = 0;
      edit_mode_gui();
      return 1;
    }
    if (ecommand[0] == 'R') // Rotate Menu
    {
      // Try to get submenu command
      switch(key) {
      case 'x':
      case 'y':
      case 'z':
	clear_edit_mode_gui();
	sprintf(eprompt[0], "Rotate %c-axis angle: ", toupper(key));
	ecommand[0] = key - 0x60; // change 'x' to C-X etc.
	ecommand[1] = 0;
	edit_mode_gui();
	return 1;
      }
      return 1;
    }
    if (ecommand[0] == 'T') // Turn Menu
    {
      // Try to get submenu command
      switch(key) {
      case 'x':
      case 'y':
      case 'z':
	clear_edit_mode_gui();
	sprintf(eprompt[0], "Turn %c-axis angle: ", toupper(key));
	ecommand[0] = toupper(key);
	ecommand[1] = 0;
	edit_mode_gui();
	return 1;
      case 'c':
	// Center set axis
	if (Get1PartPos(curpiece, turnCenter) == 0)
          turnCenter[0][3] = turnCenter[1][3] = turnCenter[2][3] = 0.0;
	printf("Turn Center at %f, %f, %f\n", turnCenter[0][3], turnCenter[1][3], turnCenter[2][3]);
	clear_edit_mode_gui();
	if (turnAxisVisible)
	{
	  dirtyWindow = 1;
	  glutPostRedisplay();
	}
	else
	  edit_mode_gui();
	return 1;
      case 'o':
        sprintf(eprompt[0], "Turn Origin (x y z): ");
	eprompt[1][0] = 0;
	ecommand[0] = key - 0x60; // change 'o' to C-O.
	ecommand[1] = 0;
	edit_mode_gui();
	return 1;
      case 'a':
	turnAxisVisible ^= 1;
	// printf("TurnAxisVisible = %d\n", turnAxisVisible);
	clear_edit_mode_gui();
	dirtyWindow = 1;
	glutPostRedisplay();
	return 1;
      case 'r':
	// Switch to the rotate menu
	sprintf(eprompt[0], "Rotate: ");
	sprintf(eprompt[1], "&X-axis &Y-axis &Z-axis");
	ecommand[0] = toupper(key);
	ecommand[1] = 0;
	edit_mode_gui();
	break;
      }
      return 1;
    }
    if (ecommand[0] == 'F') // File Menu
    {
      // Try to get submenu command
      switch(key) {
      case 'l':
	clear_edit_mode_gui();
	sprintf(eprompt[0], "Load file: ");
	ecommand[0] = toupper(key);
	sprintf(&(ecommand[1]), datfilename);
	edit_mode_gui();
	return 1;
      case 's':
	clear_edit_mode_gui();
	sprintf(eprompt[0], "Save as: ");
	ecommand[0] = toupper(key);
	sprintf(&(ecommand[1]), datfilename);
	edit_mode_gui();
	return 1;
      case 'e':
	exit(0);
      }
      return 1;
    }
    if (ecommand[0] == 'E') // Edit Menu
    {
      // Try to get submenu command
      switch(key) {
      case 'i':
	ecommand[0] = 0; // wipe the command char
	clear_edit_mode_gui();
	InsertNewPiece();
	return 1;
      case 'd':
	ecommand[0] = 0; // wipe the command char
	clear_edit_mode_gui();
	DelCurPiece();
	return 1;
      case 's':
	ecommand[0] = 0; // wipe the command char
	clear_edit_mode_gui();
	UnLightCurPiece();
	Switch1Part(curpiece);
	HiLightNewPiece(curpiece);
	return 1;
      case 'l':
	clear_edit_mode_gui();
	sprintf(eprompt[0], "Line Type: ");
	sprintf(eprompt[1], "&Piece &Comment &Step &2 &3 &4 &5");
	ecommand[0] = 'e'; // EDIT_LINE_ID
	edit_mode_gui();
	return 1;
      case 'h':
	clear_edit_mode_gui();
	sprintf(eprompt[0], "Hoser steps: ");
	ecommand[0] = 'h'; // EDIT_HOSE_ID
	ecommand[1] = 0;
	edit_mode_gui();
	return 1;
      case 'p':
	clear_edit_mode_gui();
	sprintf(eprompt[0], "Plugins: ");
	ecommand[0] = 'g'; // EDIT_PLUGIN_ID
	ecommand[1] = 0;
	loadpluglist();
	edit_mode_gui(); // Redisplay the GUI
	return 1;
      }
      return 1;
    }
    if (ecommand[0] == 'e') // Edit Linetype Menu
    {
      // Try to get submenu command
      switch(key) {
      case 'p':
	sprintf(eprompt[0], "New Part: ");
	eprompt[1][0] = 0;
	ecommand[0] = 'p';
	ecommand[1] = 0;
	edit_mode_gui();
	return 1;
      case 'c':
	sprintf(eprompt[0], "Comment: ");
	eprompt[1][0] = 0;
	ecommand[0] = 'C';
	ecommand[1] = 0;
	edit_mode_gui();
	return 1;
      case 's':
	EraseCurPiece();
	movingpiece = -1;
	Comment1Part(curpiece, "STEP");
	glDepthMask(GL_TRUE); // enable updates to depth buffer
	dirtyWindow = 1;
	glutPostRedisplay();
	if (ldraw_commandline_opts.debug_level == 1)
	    Print1Part(curpiece, stdout);
	clear_edit_mode_gui();
	return 1;
      case '2':
      case '3':
      case '4':
      case '5':
	sprintf(eprompt[0], "Enter Coords for Type %c Primitive: ", key);
	eprompt[1][0] = 0;
	ecommand[0] = key - '0';
	ecommand[1] = 0;
	edit_mode_gui();
	return 1;
      }
      return 1;
    }
    if (ecommand[0] == 'V') // View Menu
    {
      // Try to get submenu command
      switch(key) {
      case 'f':
	m_viewMatrix = Front;
	newview = 1;
	break;
      case 'r':
	m_viewMatrix = Right;
	newview = 1;
	break;
      case 'l':
	m_viewMatrix = Left;
	newview = 1;
	break;
      case 'b':
	m_viewMatrix = Back;
	newview = 1;
	break;
      case 'o':
	m_viewMatrix = Above;
	newview = 1;
	break;
      case 'u':
	m_viewMatrix = Beneath;
	newview = 1;
	break;
      case 't':
	m_viewMatrix = LdrawOblique;
	newview = 1;
	break;
      case 'i':
	clear_edit_mode_gui();
	HiLightCurPiece(curpiece);
	ldraw_commandline_opts.S *= (1.0 / 0.5);
	dirtyWindow = 1;
	glutPostRedisplay();
	return 1;
      case 'c':
	{
      clear_edit_mode_gui();
	  if (Find1PartMatrix(curpiece, m))
	  {
	    //gluProject((GLdouble)x, (GLdouble)y, (GLdouble)z, model, proj, view, &sx, &sy, &sz);
	    //printf("Center(%0.2f, %0.2f, %0.2f)\n", m[0][3],m[1][3],m[2][3]);
	    m[0][3] -= (ldraw_commandline_opts.O.x + (zGetRowsize()/2));
	    m[1][3] -= (ldraw_commandline_opts.O.y + (2*zGetColsize()/3));
	    m[2][3] -= (ldraw_commandline_opts.O.z);
	    //printf("Offset(%0.2f, %0.2f, %0.2f)\n", m[0][3],m[1][3],m[2][3]);
	    ldraw_commandline_opts.O.x = -m[0][3];
	    ldraw_commandline_opts.O.y = -m[1][3];
	    ldraw_commandline_opts.O.z = -m[2][3];
	    initCamera(); // Reset the camera position for any stock views.
	    dirtyWindow = 1;
	    glutPostRedisplay();
	  }
	  else
	    edit_mode_gui();
	}
	return 1;
      }
      if (newview)
      {
	clear_edit_mode_gui();
	HiLightCurPiece(curpiece); // unselect any moving parts
	parse_view(m_viewMatrix);
	initCamera(); // Reset the camera position for any stock views.
	dirtyWindow = 1;
	glutPostRedisplay();
      }
      return 1;
    }
    if (ecommand[0] == 'P') // Piece Menu
    {
      // Try to get submenu command
      switch(key) {
      case 'f':
	sprintf(eprompt[0], "New Part: ");
	eprompt[1][0] = 0;
	ecommand[0] = 'p';
	ecommand[1] = 0;
	edit_mode_gui();
	return 1;
      case 'c':
        sprintf(eprompt[0], "New Color: ");
	eprompt[1][0] = 0;
	ecommand[0] = 'c';
	ecommand[1] = 0;
	edit_mode_gui();
	return 1;
      case 'g':
	sprintf(eprompt[0], "Goto Line: ");
	eprompt[1][0] = 0;
	ecommand[0] = toupper(key);
	ecommand[1] = 0;
	edit_mode_gui();
	return 1;
      case 'l':
        sprintf(eprompt[0], "New Location (x y z): ");
	eprompt[1][0] = 0;
	ecommand[0] = 'l';
	ecommand[1] = 0;
	edit_mode_gui();
	return 1;
      case 's':
        sprintf(eprompt[0], "New Scale (sx sy sz): ");
	eprompt[1][0] = 0;
	ecommand[0] = 's';
	ecommand[1] = 0;
	edit_mode_gui();
	return 1;
      case 'm':
        sprintf(eprompt[0], "New Matrix: ");
	sprintf(eprompt[1], "(x y z)(a b c d e f h i j)");
	//eprompt[1][0] = 0;
	ecommand[0] = 'm';
	ecommand[1] = 0;
	edit_mode_gui();
	return 1;
      case 'i':
	// Inline the current piece
	clear_edit_mode_gui();
	UnLightCurPiece();
	Inline1Part(curpiece);
	HiLightNewPiece(curpiece);
	return 1;
      }
      return 1;
    }
    if (ecommand[0] == 'O') // Options Menu
    {
      // Try to get submenu command
      switch(key) {
      case 'l':
	clear_edit_mode_gui();
	//ldraw_commandline_opts.F ^= TYPE_F_STUDLESS_MODE;
	ldraw_commandline_opts.F ^= TYPE_F_STUDLINE_MODE;
	dirtyWindow = 1;
	glutPostRedisplay();
	return 1;
      case 's':
	// Start display at curpiece
	StartLineNo = curpiece;
	clear_edit_mode_gui();
	edit_mode_gui();
	return 1;
      case 'd':
	// Toggle draw to curpiece
	DrawToCurPiece ^= 1;
	clear_edit_mode_gui();
	edit_mode_gui();
	return 1;
      }
      return 1;
    }

    // Not looking for a submenu command.  Just process the keystroke.
    switch(key) {
    case '\n':
    case '\r':
      // Process the command
      c = ecommand[0]; // get the command char
      ecommand[0] = 0; // wipe the command char
      clear_edit_mode_gui();
      switch (c){
      case 'G':
	// Goto piece command
	sscanf(&(ecommand[1]),"%d", &i);
	if (DrawToCurPiece)
	{
	  if (i > curpiece+1)
	  {
#if 1
	    int j;
	    //rendersetup();
	    UnLightCurPiece();
	    for (j=curpiece+1; j<i; j++)
	      Draw1Part(j, -1);
	    HiLightNewPiece(i);
#else
	    dirtyWindow = 1;
	    glutPostRedisplay();
	    if (ldraw_commandline_opts.debug_level == 1)
		Print1Part(curpiece, stdout);
	    clear_edit_mode_gui();
#endif
	    break;
	  }
	}
	HiLightCurPiece(i);
	break;
      case 'c':
	sscanf(&(ecommand[1]),"%i", &color);
	CopyStaticBuffer(0);//It would be nice to recolor without "moving" it.
	movingpiece = curpiece;
	Color1Part(curpiece, color);
	DrawMovingPiece();
	if (ldraw_commandline_opts.debug_level == 1)
	    Print1Part(curpiece, stdout);
	edit_mode_gui();
	break;
      case 'p':
	//sscanf(&(ecommand[1]),"%s", partname);
	if (partlookup)
	{
	  // Use the part from the lookup list. 
	  strcpy(&(ecommand[1]), partlistptr[partlookup - 1]);
	  if (token = strpbrk(&(ecommand[1])," \t"))
	    *token = 0;

	  prevlookup = saveprevlookup();
	  partlookup = 0;
	}
	else if (ecommand[1] == 0)
	{
	  // Part lookup?
	  sprintf(eprompt[0], "New Part: ");
	  ecommand[0] = 'p';
	  //StashPart0();
	  loadpartlist();
	  edit_mode_gui(); // Redisplay the GUI
	  return 1;
	}
	getfilename(&(ecommand[1]), partname);
	CopyStaticBuffer(0);
	movingpiece = curpiece;
	if (strrchr(partname, '.') == NULL)
	  strcat(partname, use_uppercase ? ".DAT" : ".dat");
	Swap1Part(curpiece, partname);
	DrawMovingPiece();
	if (ldraw_commandline_opts.debug_level == 1)
	    Print1Part(curpiece, stdout);
	edit_mode_gui();
	break;
      case 'l':
	v[0][0] = v[0][1] = v[0][2] = 0.0; // Default location to (0, 0, 0)
	ScanPoints(v, 1, &(ecommand[1]));
	m[0][3] = v[0][0];
	m[1][3] = v[0][1];
	m[2][3] = v[0][2];
	printf("Locating at %f, %f, %f\n", v[0][0], v[0][1], v[0][2]);
	CopyStaticBuffer(0);//It would be nice to relocate without "moving" it.
	Locate1Part(curpiece, m, 1);
	movingpiece = curpiece;
	DrawMovingPiece();
	if (ldraw_commandline_opts.debug_level == 1)
	  Print1Part(curpiece, stdout);
	edit_mode_gui();
	break;
      case 's':
	v[0][0] = v[0][1] = v[0][2] = 1.0; // Default scale to (1, 1, 1)
	ScanPoints(v, 1, &(ecommand[1]));
	m[0][0] = v[0][0];
	m[1][1] = v[0][1];
	m[2][2] = v[0][2];
	printf("Scaling by %f, %f, %f\n", m[0][0], m[1][1], m[2][2]);
	CopyStaticBuffer(0);//It would be nice to rescale without "moving" it.
	movingpiece = curpiece;
        //Move1Part(curpiece, m, 1);
	Move1Part(curpiece, m, 2);
	DrawMovingPiece();
	if (ldraw_commandline_opts.debug_level == 1)
	    Print1Part(curpiece, stdout);
	edit_mode_gui();
	break;
      case 'm':
	memset(&v, 0, sizeof(v)); // Probably SHOULD default to identity matrix.
	ScanPoints(v, 4, &(ecommand[1]));
	m[0][0] = v[1][0]; m[1][0] = v[1][1]; m[2][0] = v[1][2];
	m[0][1] = v[2][0]; m[1][1] = v[2][1]; m[2][1] = v[2][2];
	m[0][2] = v[3][0]; m[1][2] = v[3][1]; m[2][2] = v[3][2];
	m[0][3] = v[0][0]; m[1][3] = v[0][1]; m[2][3] = v[0][2];
	printf("Orientation = %f, %f, %f %f, %f, %f %f, %f, %f %f, %f, %f\n", 
	       v[0][0], v[0][1], v[0][2],
	       v[1][0], v[1][1], v[1][2],
	       v[2][0], v[2][1], v[2][2],
	       v[3][0], v[3][1], v[3][2]);
	CopyStaticBuffer(0);//It would be nice to reorient without "moving" it.
	Locate1Part(curpiece, m, 0);
	movingpiece = curpiece;
	DrawMovingPiece();
	if (ldraw_commandline_opts.debug_level == 1)
	  Print1Part(curpiece, stdout);
	edit_mode_gui();
	break;
      case 'x':
	sscanf(&(ecommand[1]),"%f", &f);
	m[0][3] += f;
	TranslateCurPiece(m);
	break;
      case 'z':
	sscanf(&(ecommand[1]),"%f", &f);
	m[2][3] += f;
	TranslateCurPiece(m);
	break;
      case 'y':
	sscanf(&(ecommand[1]),"%f", &f);
	m[1][3] += f;
	TranslateCurPiece(m);
	break;
      case 'v': // Translate x, y, z, or v = vector.
	v[0][0] = v[0][1] = v[0][2] = 0.0;
	ScanPoints(v, 1, &(ecommand[1]));
	m[0][3] += v[0][0];
	m[1][3] += v[0][1];
	m[2][3] += v[0][2];
	TranslateCurPiece(m);
	break;
      case 'L':
	// Load filename if new
	getfilename(&(ecommand[1]), partname);
	printf("loading file: %s\n", partname);
	if (strcmp(partname,datfilename))
	{
	  strcpy(datfilename, partname);
	  curstep = 0; // Reset to first step
	  dirtyWindow = 1;
	  UnSelect1Part(curpiece); // UnSelect part before Loading
	  curpiece = 0;
	  movingpiece = -1;
	  list_made = 0; // Gotta reparse the file.
	  SetTitle(1); // Change the title of the window.
	  glutPostRedisplay();
	}
	else
	  edit_mode_gui();
	break;
      case 'S':
	// Save as filename
	getfilename(&(ecommand[1]), datfilename);
	printf("Save as: %s\n", datfilename);
	i = UnSelect1Part(curpiece); // Link part back in before printing
	Print1Model(datfilename);
	SetTitle(1); // Change the title of the window.
	if (i != -1)
	  Select1Part(curpiece); // Unlink part again if needed.
	edit_mode_gui();
	break;
      case 24:
	c += 0x60;
      case 'X':
	sscanf(&(ecommand[1]),"%f", &f);
	angle = f;
	printf("Rotate about %c by %f\n",c,angle);
	CopyStaticBuffer(0);
	angle *= PI_180;
	m[1][1] = (float)cos(angle);
	m[1][2] = (float)(-1.0*sin(angle));
	m[2][1] = (float)sin(angle);
	m[2][2] = (float)cos(angle);
	movingpiece = curpiece;
	if (c == 'X')
	{
	  m[0][3] = turnCenter[0][3];
	  m[1][3] = turnCenter[1][3];
	  m[2][3] = turnCenter[2][3];
	  Move1Part(curpiece, m, 0);
	}
	else
	  Move1Part(curpiece, m, 2);
	DrawMovingPiece();
	if (ldraw_commandline_opts.debug_level == 1)
	    Print1Part(curpiece, stdout);
	edit_mode_gui();
	break;
      case 25:
	c += 0x60;
      case 'Y':
	sscanf(&(ecommand[1]),"%f", &f);
	angle = f;
	printf("Rotate about %c by %f\n",c,angle);
	CopyStaticBuffer(0);
	angle *= PI_180;
	m[0][0] = (float)cos(angle);
	m[0][2] = (float)sin(angle);
	m[2][0] = (float)(-1.0*sin(angle));
	m[2][2] = (float)cos(angle);
	movingpiece = curpiece;
	if (c == 'Y')
	{
	  m[0][3] = turnCenter[0][3];
	  m[1][3] = turnCenter[1][3];
	  m[2][3] = turnCenter[2][3];
	  Move1Part(curpiece, m, 0);
	}
	else
	  Move1Part(curpiece, m, 2);
	DrawMovingPiece();
	if (ldraw_commandline_opts.debug_level == 1)
	    Print1Part(curpiece, stdout);
	edit_mode_gui();
	break;
      case 26:
	c += 0x60;
      case 'Z':
	sscanf(&(ecommand[1]),"%f", &f);
	angle = f;
	printf("Rotate about %c by %f\n",c,angle);
	CopyStaticBuffer(0);
	angle *= PI_180;
	m[0][0] = (float)cos(angle);
	m[1][0] = (float)sin(angle);
	m[0][1] = (float)(-1.0*sin(angle));
	m[1][1] = (float)cos(angle);
	movingpiece = curpiece;
	if (c == 'Z')
	{
	  m[0][3] = turnCenter[0][3];
	  m[1][3] = turnCenter[1][3];
	  m[2][3] = turnCenter[2][3];
	  Move1Part(curpiece, m, 0);
	}
	else
	  Move1Part(curpiece, m, 2);
	DrawMovingPiece();
	if (ldraw_commandline_opts.debug_level == 1)
	    Print1Part(curpiece, stdout);
	edit_mode_gui();
	break;
      case 15:
	v[0][0] = v[0][1] = v[0][2] = 0.0; // Default location to (0, 0, 0)
	ScanPoints(v, 1, &(ecommand[1]));
	turnCenter[0][3] = v[0][0];
	turnCenter[1][3] = v[0][1];
	turnCenter[2][3] = v[0][2];
	printf("Turn Center at %f, %f, %f\n", v[0][0], v[0][1], v[0][2]);
	if (turnAxisVisible)
	{
	  dirtyWindow = 1;
	  glutPostRedisplay();
	}
	else
	  edit_mode_gui();
	break;
      case '1':
	sscanf(&(ecommand[1]),"%f", &f);
	ldraw_commandline_opts.O.x = -f;
	edit_mode_gui();
	break;
      case '2':
	sscanf(&(ecommand[1]),"%f", &f);
	ldraw_commandline_opts.O.y = -f;
	edit_mode_gui();
	break;
      case '3':
	sscanf(&(ecommand[1]),"%f", &f);
	ldraw_commandline_opts.O.z = -f;
	edit_mode_gui();
	break;
      case '4':
	m[0][0] = m[0][1] = m[0][2] = 0.0;
	ScanPoints(m, 1, &(ecommand[1]));
	ldraw_commandline_opts.O.x = -m[0][0];
	ldraw_commandline_opts.O.y = -m[0][1];
	ldraw_commandline_opts.O.z = -m[0][2];
	edit_mode_gui();
	break;
      case 'C':
	strcpy(partname, &(ecommand[1]));
	EraseCurPiece();
	movingpiece = -1;
	Comment1Part(curpiece, partname);
	glDepthMask(GL_TRUE); // enable updates to depth buffer
	dirtyWindow = 1;
	glutPostRedisplay();
	if (ldraw_commandline_opts.debug_level == 1)
	    Print1Part(curpiece, stdout);
	clear_edit_mode_gui();
	break;
      case 'h':
	// Hoser.  Get number of steps.  Similar to inline but for 4 lines.
	sscanf(&(ecommand[1]),"%d", &i);
	// Inline the current piece
	clear_edit_mode_gui();
	i = Hose1Part(curpiece, i); // Inline1Part(curpiece);
	// Steal some code from the goto command.
	if (i > curpiece+1)
	  {
	    int j;
	    //rendersetup();
	    UnLightCurPiece();
	    for (j=curpiece+1; j<i; j++)
	      Draw1Part(j, -1);
	    HiLightNewPiece(i);
	    break;
	  }
	else 
	  i = curpiece;
	HiLightCurPiece(i);
	return 1;
      case 'g':
	ecommand[0] = 0; // wipe the command char
	clear_edit_mode_gui();
	if (pluglookup)
	{
	  // Use the plug from the lookup list. 
	  runplugin(pluglookup);
	  pluglookup = 0;
	}
	edit_mode_gui();
	break;
      case 2:
      case 3:
      case 4:
      case 5:
	EraseCurPiece();
	if ((c == 3) || (c == 4))
	  color = 16; // default color for filled primitives.
	else
	  color = 24; // default edge color for line primitives.
	sprintf(partname, "%d %d %s", (int)c, color, &(ecommand[1]));
	Make1Primitive(curpiece, partname);
	HiLightCurPiece(curpiece);
	edit_mode_gui();
	break;
      default:
	edit_mode_gui();
	break;
      }
      break; // End of command finished processing.

    case 22: // Paste on Windows (CTRL-V)
#ifdef WINDOWS
      pasteCommand(x, y);
      edit_mode_keyboard('\n', x, y);
#endif
      break;
    case 8: // Backspace
    case 127: // Delete
      if (i > 1) // Don't backspace past the command char.
	ecommand[i-1] = 0;
      if (partlookup)
	limitpartlist(&(ecommand[1]));
      if (pluglookup)
	limitpluglist(&(ecommand[1]));
      edit_mode_gui(); // Redisplay the GUI
      break;
    case '?': // Part lookup?
      if (ecommand[0] == 'p')
      {
	//StashPart0();
	if (partlookup)
	{
	  // Got ? when already looking up part.  Display the selected part?
	  // Use the part from the lookup list. 
	  strcpy(&(eresponse[1]), partlistptr[partlookup - 1]);
	  if (token = strpbrk(&(eresponse[1])," \t"))
	    *token = 0;

	  for (i = 1; eresponse[i] == ' '; i++); // Strip leading spaces
	  strcpy(partname, &(eresponse[i]));
	  CopyStaticBuffer(0);
	  movingpiece = curpiece;
	  if (strrchr(partname, '.') == NULL)
	    strcat(partname, use_uppercase ? ".DAT" : ".dat");
	  Swap1Part(curpiece, partname);
	  DrawMovingPiece();
	  if (ldraw_commandline_opts.debug_level == 1)
	    Print1Part(curpiece, stdout);
	  edit_mode_gui(); // Redisplay the GUI
	  break;
	}
	loadpartlist();
	if (ecommand[1])
	  limitpartlist(&(ecommand[1]));
	edit_mode_gui(); // Redisplay the GUI
	break;
      }
    default:
      // printf("KEY = %d = '%c'\n",key,key);
      // Just add to the command
      ecommand[i] = key;
      ecommand[i+1] = 0;
      if (partlookup)
	limitpartlist(&(ecommand[1]));
      if (pluglookup)
	limitpluglist(&(ecommand[1]));
      edit_mode_gui(); // Redisplay the GUI
      break;
    }
    return 1;
  }
  // End of edit mode command menu section.

  // Look for normal editmode keys
  switch(key) {
  case 27:
  case '/':
    sprintf(eprompt[0], "&File &Edit &View &Turn &Rotate &Piece &Options &Hide  &Quit");
    ecommand[0] = '/';
    ecommand[1] = 0;
    edit_mode_gui();
    return 1;
  case 'a':
    CopyStaticBuffer(0);
    angle = (3.1415927/2.0);
    m[0][0] = 0.0;  //(float)cos(angle);
    m[0][2] = 1.0;  //(float)sin(angle);
    m[2][0] = -1.0; //(float)(-1.0*sin(angle));
    m[2][2] = 0.0;  //(float)cos(angle);
    movingpiece = curpiece;
    Move1Part(curpiece, m, 1);
    DrawMovingPiece();
    if (ldraw_commandline_opts.debug_level == 1)
	Print1Part(curpiece, stdout);
    edit_mode_gui();
    return 1;
  case 'i':
    InsertNewPiece();
    return 1;
  case 'e':
    // printf("Erase and redraw screen.\n");
    dirtyWindow = 1;
    glutPostRedisplay();
    return 1;
  case 'w':
    UnLightCurPiece();
    Switch1Part(curpiece);
    HiLightNewPiece(curpiece);
    return 1;
  case '\n':
  case '\r':
    // If in piece moving mode, switch back to picking mode.
    HiLightCurPiece(curpiece); 
    return 1;
  case 'x':
  case 'y':
  case 'z':
    sprintf(eprompt[0], "%c: ", key);
    ecommand[0] = key;
    ecommand[1] = 0;
    edit_mode_gui();
    return 1;
  case 'v': // Translate x, y, z, or v = vector.
    sprintf(eprompt[0], "vector (x y z): ");
    ecommand[0] = key;
    ecommand[1] = 0;
    edit_mode_gui();
    return 1;
  case 'm':
    if (moveXamount == 10.0)
    {
      moveXamount = 100.0; // Coarse movement.
      moveZamount = 100.0;
      moveYamount = 80.0;
    }
    else if (moveXamount == 100.0)
    {
      moveXamount = 1.0; // Fine movement.
      moveZamount = 1.0;
      moveYamount = 1.0;
    }
    else
    {
      moveXamount = 10.0; // Normal movement.
      moveZamount = 10.0;
      moveYamount = 8.0;
    }
    edit_mode_gui();
    return 1;
  case 'c':
    sprintf(eprompt[0], "New Color: ");
    ecommand[0] = 'c';
    ecommand[1] = 0;
    edit_mode_gui();
    return 1;
  case 'p':
    sprintf(eprompt[0], "New Part: ");
    ecommand[0] = 'p';
    ecommand[1] = 0;
    edit_mode_gui();
    return 1;
  case 'o':
    sprintf(eprompt[0], "Offset: ");
    sprintf(eprompt[1], "X-axis Y-axis Z-axis   Vector");
    ecommand[0] = key;
    ecommand[1] = 0;
    edit_mode_gui();
    return 1;
  case 'd':
  case 127: // Delete
    DelCurPiece();
    return 1;
#ifdef WINDOWS
  case 22: // Paste on Windows (CTRL-V) 
    edit_mode_keyboard('i', x, y);
    edit_mode_keyboard('p', x, y);
    pasteCommand(x, y);
    edit_mode_keyboard('\n', x, y);
    return 1;
#endif
  }

  return 0;
}

/***************************************************************/
void fnkeys(int key, int x, int y)
{
  GLdouble pan_x, pan_y, pan_z;
  UNUSED(pan_x);
  UNUSED(pan_y);
  UNUSED(pan_z);

  float angle;

  glutModifiers = glutGetModifiers(); // Glut doesn't like this in motion() fn.
  
  if (ldraw_commandline_opts.debug_level == 1)
    printf("key(%c) = (%d, %d)\n", key, key, glutModifiers);

  if (glutModifiers & GLUT_ACTIVE_ALT)
  {
    if (glutModifiers & GLUT_ACTIVE_SHIFT)
      angle = 45.0;
    else
      angle = 5.0;
  }
  else if (glutModifiers & GLUT_ACTIVE_SHIFT)
    angle = 0.0; //angle = 1.0;
  else 
    angle = 0.0;

#ifdef USE_L3_PARSER
  if (edit_mode_fnkeys(key, x, y))
    return;
#endif

    /*
      This will rotate you around x and y and translate you along x, y, and z. 
      It shouldn't take much to add rotation around the z axis.
    */
    
    // Add modifier check so I can:
    //   spin about the 3 axis with 
    //   slide along 3 axis with shifted keys
    //   orbit about the center of the viewpoint with ctrl modified keys?
    //
    // glutGetModifiers() return masks.
    //   GLUT_ACTIVE_SHIFT
    //   GLUT_ACTIVE_CTRL
    //   GLUT_ACTIVE_ALT
    
    // The PG_UP, PG_DN keys seem to zoom in and out (only in perspective mode)
    // You can NOT zoom in or out in orthographic mode, only scale.
  switch(key) {
#ifdef USE_F00_CAMERA
  case GLUT_KEY_LEFT:
    if (specialFunc( key, x, y ))
      break;
    if (angle == 0.0)
      ldraw_commandline_opts.O.x -= 100;
    rotate_about(0.0, 1.0, 0.0, angle );
    break;
  case GLUT_KEY_RIGHT:
    if (specialFunc( key, x, y ))
      break;
    if (angle == 0.0)
      ldraw_commandline_opts.O.x += 100;
    rotate_about(0.0, 1.0, 0.0, -angle );
    break;
  case GLUT_KEY_UP:
    if (specialFunc( key, x, y ))
      break;
    if (angle == 0.0)
      ldraw_commandline_opts.O.y -= 100;
    rotate_about(1.0, 0.0, 0.0, -angle );
    break;
  case GLUT_KEY_DOWN:
    if (specialFunc( key, x, y ))
      break;
    if (angle == 0.0)
      ldraw_commandline_opts.O.y += 100;
    rotate_about(1.0, 0.0, 0.0, angle );
    break;
  case GLUT_KEY_HOME:
    if (specialFunc( key, x, y ))
      break;
    rotate_about(0.0, 0.0, 1.0, angle );
    break;
  case GLUT_KEY_END:
    if (specialFunc( key, x, y ))
      break;
    rotate_about(0.0, 0.0, 1.0, -angle );
    break;
  case GLUT_KEY_PAGE_UP:
  case GLUT_KEY_PAGE_DOWN:
    specialFunc( key, x, y );
    break;
#else
  case GLUT_KEY_PAGE_UP:
    fCamX += (float)sin(fYRot*PI_180) * MOVE_SPEED * 20;
    fCamZ += (float)cos(fYRot*PI_180) * MOVE_SPEED * 20;
    fCamY += (float)sin(fXRot*PI_180) * MOVE_SPEED * 20;
    break;
  case GLUT_KEY_PAGE_DOWN:
    fCamX -= (float)sin(fYRot*PI_180) * MOVE_SPEED * 20;
    fCamZ -= (float)cos(fYRot*PI_180) * MOVE_SPEED * 20;
    fCamY -= (float)sin(fXRot*PI_180) * MOVE_SPEED * 20;
    break;
  case GLUT_KEY_RIGHT:
    fYRot -= MOVE_SPEED / 2;
    if(fYRot < -360.0)			
      fYRot += 360.0;
    break;
  case GLUT_KEY_LEFT:
    fYRot += MOVE_SPEED / 2;
    if(fYRot > 360.0)
      fYRot -= 360.0;	
    break;
  case GLUT_KEY_UP:
    fXRot -= MOVE_SPEED / 2;
    if(fXRot < -360.0)
      fXRot += 360.0;
    break;
  case GLUT_KEY_DOWN:
    fXRot += MOVE_SPEED / 2;
    if(fXRot > 360.0)
      fXRot -= 360.0;
    break;
#endif
  case GLUT_KEY_F1:
    panlock ^= 1;
    if (panlock)  // Start panning.
    {
      mouse(GLUT_LEFT_BUTTON, GLUT_DOWN, x, y);
      panning = 1;
    }
    else
      mouse(GLUT_LEFT_BUTTON, GLUT_UP, x, y);
    break;
#if 0
  case GLUT_KEY_F1:
    rotate_about(1.0, 0.0, 0.0, 5.0 );
    break;
  case GLUT_KEY_F2:
    rotate_about(0.0, 1.0, 0.0, 5.0 );
    break;
  case GLUT_KEY_F3:
    rotate_about(1.0, 0.0, 0.0, -5.0 );
    break;
  case GLUT_KEY_F4:
    rotate_about(0.0, 1.0, 0.0, -5.0 );
    break;
  case GLUT_KEY_F5:
    rotate_about(1.0, 0.0, 0.0, 1.0 );
    break;
  case GLUT_KEY_F6:
    rotate_about(1.0, 0.0, 0.0, -1.0 );
    break;
#endif
#ifdef TEST_MUI_GUI
  case GLUT_KEY_F8:
    mui_test();
    return;
#endif
#ifndef AGL
#if (GLUT_XLIB_IMPLEMENTATION >= 13)
  case GLUT_KEY_F9:
    glutLeaveGameMode();
    if (ldraw_commandline_opts.debug_level == 1)
      printf("Window %d -> %d\n", glutGetWindow(), main_window);
    if ( glutGetWindow() != main_window )
    {
      if (main_window == -1)
        main_window = glutCreateWindow(title);
      else
        glutSetWindow(main_window); 
    }
    registerGlutCallbacks();
    init();
    break;
  case GLUT_KEY_F10:
    //glutGameModeString("800x600:16@60") ;
    //glutGameModeString("800x600:16") ;
    glutGameModeString("640x480") ;
    // if the width is different to -1
    if (glutGameModeGet(GLUT_GAME_MODE_WIDTH) != -1)
    {
      glutEnterGameMode();		// enter full screen mode
      registerGlutCallbacks();
      init();
    }
    break;
#endif
#endif
  case GLUT_KEY_F12:
    printLdrawMatrix(stdout);
    printPOVMatrix(stdout);
    return;
  default:
    return;
  }

  if (ldraw_commandline_opts.debug_level == 1)
  {
    printf("Cam = (%.2f, %.2f, %.2f)\n", fCamX, fCamY, fCamZ);
    printf("Rot = (%.2f, %.2f, %.2f)\n", fXRot, fYRot, fZRot);
  }

  // The LdrawOblique matrix is a projection matrix.
  // Substitute Oblique view matrix so we can rotate it.
  if (m_viewMatrix == LdrawOblique)
  {
    m_viewMatrix = Oblique;
    parse_view(m_viewMatrix);
  }
  else //if (editing) 
  { // lets call any random orientation oblique, just dont parse it.
    m_viewMatrix = Oblique;
  }

  dirtyWindow = 1;
  glutPostRedisplay();

  /*
    Then, all you have to do is this
    at the beginning of your display function to implement your camera.
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();	
    // Do camera translation/rotation
    glRotatef(fXRot, 1.0f, 0.0f, 0.0f);
    glRotatef(360 - fYRot, 0.0f, 1.0f, 0.0f);
    glTranslatef(fCamX, fCamY, fCamZ);        
    // Draw everything else below
  */
}

void outputEPS(int size, int doSort, char *filename);

/***************************************************************/
void menuKeyEvent(int key, int x, int y)
{
  int newview = 0;
  char c;
  
  float m[4][4] = {
    {1.0,0.0,0.0,0.0},
    {0.0,1.0,0.0,0.0},
    {0.0,0.0,1.0,0.0},
    {0.0,0.0,0.0,1.0}
  };
  double angle;
  float f;
  int i, color;
  char partname[256];
  UNUSED(f);
  UNUSED(i);
  UNUSED(m);
  UNUSED(angle);
  UNUSED(color);
  UNUSED(partname);

  glutModifiers = glutGetModifiers(); // Glut doesn't like this in motion() fn.

  if (pastelist)
    key = 22;

  if (ldraw_commandline_opts.debug_level == 1)
    printf("key(%c) = (%d, %d)\n", key, key, glutModifiers);

  if (editing)
  {
    if (edit_mode_keyboard(key, x, y))
      return;
  }

  if (EPS_OUTPUT_FIGURED_OUT)
  {
  // Allow eps output with ALT-e key combo someday...
  if (((glutModifiers & GLUT_ACTIVE_ALT) != 0) && (tolower(key) == 'e'))
  {
    if ((glutModifiers & GLUT_ACTIVE_CTRL) != 0)
      key = 16;
    else if (key == 'e')
      key = 14;
    else 
      key = 15;
  }
  switch(key) {
  case 14:
    dirtyWindow = 1;
    glutSetCursor(GLUT_CURSOR_WAIT);
    outputEPS(512000, 1, "render.eps"); // Sorted EPS
    glutSetCursor(GLUT_CURSOR_INHERIT);
    return;
  case 15:
    dirtyWindow = 1;
    glutSetCursor(GLUT_CURSOR_WAIT);
    outputEPS(512000, 0, "render.eps"); // UnSorted EPS
    glutSetCursor(GLUT_CURSOR_INHERIT);
    return;
  case 16:
    dirtyWindow = 1;
    glutSetCursor(GLUT_CURSOR_WAIT);
    outputEPS(512000, 0, NULL); // Debug the selection buffer.
    glutSetCursor(GLUT_CURSOR_INHERIT);
    return;
  }
  }

    switch(key) {
#define AUTOSCALE_OPTION 1
#ifdef AUTOSCALE_OPTION
    case 'S':
        autoscale();
	reshape(Width, Height);
	break;
#endif
    case 6: // toggle edit mode
        if (editing)
	  leaveEditMode();
	else
	{
	  glutModifiers |= GLUT_ACTIVE_CTRL; //SOLID_EDIT_MODE = 1;
	  glutModifiers &= ~GLUT_ACTIVE_ALT; // leave studs and drawtocur alone.
	  enterEditMode();
	}
	break;
    case 2: // Zoom out
        ldraw_commandline_opts.S *= 0.9;
	dirtyWindow = 1; //reshape(Width, Height);
	break;
    case 3: // Zoom in
        ldraw_commandline_opts.S *= (1.0 / 0.9);
	dirtyWindow = 1; //reshape(Width, Height);
	break;
    case '-': // Half Size (scale dn)
        //printf("key = %d = '%c' (%08x)\n",key,key, glutModifiers);
	if ((glutModifiers & GLUT_ACTIVE_ALT) != 0)
	  ldraw_commandline_opts.S *= 0.9;
	else
	  ldraw_commandline_opts.S *= 0.5;
	dirtyWindow = 1; //reshape(Width, Height);
	break;
    case '+': // Double Size (scale up)
        //printf("key = %d = '%c' (%08x)\n",key,key, glutModifiers);
	if ((glutModifiers & GLUT_ACTIVE_ALT) != 0)
	  ldraw_commandline_opts.S *= (1.0 / 0.9);
	else
	  ldraw_commandline_opts.S *= (1.0 / 0.5);
	dirtyWindow = 1; //reshape(Width, Height);
	break;
    case '0':
      m_viewMatrix = LdrawOblique;
      if (ldraw_projection_type)
      { // LdrawObliqe only makes sense with orthographic projection.
	ldraw_projection_type = 0;
	parse_view(m_viewMatrix);
	reshape(Width, Height);
      }
      newview = 1;
      break;
    case '1':
      m_viewMatrix = Back;
      newview = 1;
      break;
    case '2':
      m_viewMatrix = Left;
      newview = 1;
      break;
    case '3':
      m_viewMatrix = Right;
      newview = 1;
      break;
    case '4':
      m_viewMatrix = Above;
      newview = 1;
      break;
    case '5':
      m_viewMatrix = Beneath;
      newview = 1;
      break;
    case '6':
      m_viewMatrix = Oblique;
      newview = 1;
      break;
    case '7':
      m_viewMatrix = Front;
      newview = 1;
      break;
    case '8':
      m_viewMatrix = UpsideDown;
      newview = 1;
      break;
    case '9':
      m_viewMatrix = Natural;
      newview = 1;
      break;
    case 'j':
      ldraw_projection_type = 0;
      if (m_viewMatrix == LdrawOblique)
      {
	m_viewMatrix = Oblique;
	parse_view(m_viewMatrix);
      }
      reshape(Width, Height);
      break;
    case 'J':
      ldraw_projection_type = 1;
      if (m_viewMatrix == LdrawOblique)
      {
	m_viewMatrix = Oblique;
	parse_view(m_viewMatrix);
      }
      reshape(Width, Height);
      break;
    case 'f':
      ldraw_commandline_opts.F ^= TYPE_F_STUDLESS_MODE;
      reshape(Width, Height);
      break;
    case 'n':
      ldraw_commandline_opts.F &= ~(TYPE_F_NO_POLYGONS); // zWire = 0;
      ldraw_commandline_opts.F &= ~(TYPE_F_SHADED_MODE); // zShading = 0;
      reshape(Width, Height);
      break;
    case 'h':
      ldraw_commandline_opts.F &= ~(TYPE_F_NO_POLYGONS); // zWire = 0;
      ldraw_commandline_opts.F |= TYPE_F_SHADED_MODE; // zShading = 1;
      reshape(Width, Height);
      break;
    case 'l':
      ldraw_commandline_opts.F |= TYPE_F_NO_POLYGONS; // zWire = 1;
      ldraw_commandline_opts.F &= ~(TYPE_F_SHADED_MODE); // zShading = 0;
      reshape(Width, Height);
      break;
    case 'B': // Bitmap
    case 'b':
      if (key == 'B')
      {
	ldraw_image_type = IMAGE_TYPE_PNG_RGB;
	if (glutModifiers & GLUT_ACTIVE_ALT)
	  ldraw_image_type = IMAGE_TYPE_PNG_RGBA;
      }
      else 
      {
	ldraw_image_type = IMAGE_TYPE_BMP8;
	if (glutModifiers & GLUT_ACTIVE_ALT)
	  ldraw_image_type = IMAGE_TYPE_BMP;
      }
      c = ldraw_commandline_opts.M;
      ldraw_commandline_opts.M = 'S';
      platform_step(INT_MAX, 0, -1, NULL);
      ldraw_commandline_opts.M = c;
      return;
    case 'V':
      if (pan_visible == (TYPE_F_BBOX_MODE | TYPE_F_NO_POLYGONS) )
	pan_visible = (TYPE_F_NO_POLYGONS | TYPE_F_STUDLESS_MODE);
      else if (pan_visible == (TYPE_F_NO_POLYGONS | TYPE_F_STUDLESS_MODE) )
	pan_visible = TYPE_F_INVISIBLE;
      else if (pan_visible == (TYPE_F_INVISIBLE) )
	pan_visible = (TYPE_F_BBOX_MODE | TYPE_F_SHADED_MODE);
      else if (pan_visible == (TYPE_F_BBOX_MODE | TYPE_F_SHADED_MODE) )
	pan_visible = (TYPE_F_STUDLESS_MODE | TYPE_F_SHADED_MODE);
      else if (pan_visible == (TYPE_F_STUDLESS_MODE | TYPE_F_SHADED_MODE) )
	pan_visible = (TYPE_F_BBOX_MODE | TYPE_F_NO_POLYGONS);
      else 
	pan_visible = (TYPE_F_BBOX_MODE | TYPE_F_NO_POLYGONS);
      return;
    case 's':
      // NOTE: I could toggle the menu strings but that would require
      // setting them right initially, and making sure its not fullscreen.
      // Right now that means checking  if (ldraw_commandline_opts.V_x >= -1)
      //glutSetMenu(opts); // Reset the current menu to the main menu.
      if (ldraw_commandline_opts.M == 'C')
      {	
	ldraw_commandline_opts.M = 'P';
	//glutChangeToMenuEntry(3, "Continuous         ", 'c');
      }
      else
      {	
	ldraw_commandline_opts.M = 'C'; //Switch to continuous output.
	//glutChangeToMenuEntry(3, "Pause              ", 'c');
      }
      curstep = 0; // Reset to first step
      dirtyWindow = 1;
      break; //return;
    case 'g':
      ldraw_commandline_opts.poll ^= 1;
      return;
    case 'G':
      ldraw_commandline_opts.debug_level ^= 1;
      return;
#ifdef USE_L3_PARSER
    case 'r': // Reader (parser)
      if (editing) 
	return; // Do not switch parsers while editing.  Bad idea!
      if (parsername == LDLITE_PARSER)
      {
	parsername = L3_PARSER;
	use_quads = 1;
      }
      else
      {
	parsername = LDLITE_PARSER;
	use_quads = 0;
      }
      list_made = 0; // Gotta reparse the file.
      curstep = 0; // Reset to first step
      dirtyWindow = 1;
      break;
#endif
    case 'Q':
    case 'q':
      qualityLines ^= 1;
      dirtyWindow = 1;
      break;
    case 27:
    case UI_ESCAPE_EVENT:
	exit(0);
	break;
    case '\r':
    case ' ':
	break;
#ifdef WINDOWS
    case 22: // Paste on Windows (CTRL-V)
      // Consider doing something special here if they drag in multiple files.
      // Perhaps I could make an MPD file out of it?
      // For now, just load the first file.
      strcpy(ecommand, "L");
      pasteCommand(x, y);
      if (ecommand[1])
	loadnewdatfile(dirname(ecommand+1),basename(ecommand+1));
      ecommand[0] = 0;
      break;
#endif
    default:
	return;
    }
    if (newview)
    {
      parse_view(m_viewMatrix);
      initCamera(); // Reset the camera position for any stock views.
      dirtyWindow = 1;
    }

    glutPostRedisplay();
}

/***************************************************************/
void keyboard(unsigned char key, int x, int y)
{
  menuKeyEvent((int)key, x, y);
}

/***************************************************************/
void rotate_the_model(double y_angle, double x_angle)
{
  // modify command line option A matrix
  matrix3d xm_rot;
  matrix3d ym_rot;
  vector3d v_dummy;
  matrix3d *m_temp;
  vector3d *v_temp;
  
  y_angle *= (3.1415927/180.0);
  x_angle *= (3.1415927/180.0);
  
  v_dummy.x = 0;
  v_dummy.y = 0;
  v_dummy.z = 0;

  xm_rot.a = (float)cos(y_angle);
  xm_rot.b = 0;
  xm_rot.c = (float)sin(y_angle);
  xm_rot.d = 0;
  xm_rot.e = 1;
  xm_rot.f = 0;
  xm_rot.g = (float)(-1.0*sin(y_angle));
  xm_rot.h = 0;
  xm_rot.i = (float)cos(y_angle);

  ym_rot.a = 1;
  ym_rot.b = 0;
  ym_rot.c = 0;
  ym_rot.d = 0;
  ym_rot.e = (float)cos(x_angle);
  ym_rot.f = (float)(-1.0*sin(x_angle));
  ym_rot.g = 0;
  ym_rot.h = (float)sin(x_angle);
  ym_rot.i = (float)cos(x_angle);
  transform_multiply(&v_dummy,&ym_rot,
		     &v_dummy,&xm_rot,
		     &v_temp, &m_temp);
  xm_rot = *m_temp;

  free(m_temp);
  free(v_temp);

  transform_multiply(&v_dummy,&xm_rot,
		     &v_dummy,&(ldraw_commandline_opts.A),
		     &v_temp, &m_temp);
  ldraw_commandline_opts.A = *m_temp;

  free(m_temp);
  free(v_temp);
}

/***************************************************************/
// Rotate by angle degrees about an arbitrary vector.
// Code borrowed from push_rotation() and push_transform() in y.tab.c
// This works but transform_multiply() from ldliteVR_main.c appears buggy.
void rotate_about(float x, float y, float z, float degrees)
{
  // convert axis - degrees into rotation matrix
  matrix3d *m, *mat;
  matrix3d newm;
  matrix3d *oldm; 
  vector3d t = {0.0, 0.0, 0.0};
  double a,b,c,s, veclen_inv, sin2a;

  // Don't bother with bad axis or 0 rotations.
  if (degrees == 0.0) 
    return;
  if ((x == 0.0) && (y == 0.0) && (z == 0.0))
    return;

  // convert axis and degrees into a quaternion
  veclen_inv = 1.0/sqrt(x*x + y*y + z*z);
  sin2a = sin((3.1415927*degrees)/360.0);
  a = sin2a * x * veclen_inv;
  b = sin2a * y * veclen_inv;
  c = sin2a * z * veclen_inv;
  s = cos((3.1415927*degrees)/360.0);

  // convert quaternion into a rotation matrix.
  m = mat = (matrix3d *)malloc(sizeof(matrix3d));

  m->a = (float)(1 - 2*b*b-2*c*c);
  m->b = (float)(2*a*b - 2*s*c);
  m->c = (float)(2*a*c + 2*s*b);
  m->d = (float)(2*a*b+2*s*c);
  m->e = (float)(1 - 2*a*a - 2*c*c);
  m->f = (float)(2*b*c - 2*s*a);
  m->g = (float)(2*a*c - 2*s*b);
  m->h = (float)(2*b*c + 2*s*a);

  m->i = (float)(1 - 2*a*a - 2*b*b);

#if 0
  // This applies the new rotation to the model before the view transform.
  // Which feels wrong.  (not like orbiting the camera about the model)
  oldm = &(ldraw_commandline_opts.A);
#else
  // This seems to apply the new rotation after the view transform,
  // but also seems to warp the image.  Why???
  // Actually It only seems to happen if I start with the oblique, natural,
  // or upside-down transforms.  What's so strange about those transforms.
  // Do they actually shear the model in a way we don't see until we rotate?
  //
  // Hmmm this is the same problem transform_multiply() suffers from...
  // That's why transform_multiply works for twirl but not for mouse spin.
  // Twirl angle is applied before the view transform, but spin is after.
  // So what is up with those 3 view matrices, and is this the L3Lab problem.
  oldm = m;
  m = &(ldraw_commandline_opts.A);
#endif

  newm.a = oldm->a * m->a + oldm->b * m->d + oldm->c * m->g;
  newm.b = oldm->a * m->b + oldm->b * m->e + oldm->c * m->h;
  newm.c = oldm->a * m->c + oldm->b * m->f + oldm->c * m->i;
  newm.d = oldm->d * m->a + oldm->e * m->d + oldm->f * m->g;
  newm.e = oldm->d * m->b + oldm->e * m->e + oldm->f * m->h;
  newm.f = oldm->d * m->c + oldm->e * m->f + oldm->f * m->i;
  newm.g = oldm->g * m->a + oldm->h * m->d + oldm->i * m->g;
  newm.h = oldm->g * m->b + oldm->h * m->e + oldm->i * m->h;
  newm.i = oldm->g * m->c + oldm->h * m->f + oldm->i * m->i;

  if (ldraw_commandline_opts.debug_level == 1)
  {
    fprintf(stdout,"Transform old:\n");
    print_transform(&t, &(ldraw_commandline_opts.A));
    fprintf(stdout,"Transform current:\n");
    print_transform(&t, &newm);
  }

  ldraw_commandline_opts.A = newm;

  free(mat);
}

/***************************************************************/
// Rotate by angle degrees about an arbitrary vector.
void old_rotate_about(float x, float y, float z, float angle)
{
  extern  void normalize(float v[3]);

  // modify command line option A matrix
  matrix3d m_rot;
  vector3d v_dummy;
  matrix3d *m_temp;
  vector3d *v_temp;
  float c, s;
  float v[3];
  
  // Don't bother with bad axis or 0 rotations.
  if (angle == 0.0) 
    return;
  if ((x == 0.0) && (y == 0.0) && (z == 0.0))
    return;

  v[0] = x;
  v[1] = y;
  v[2] = z;

  normalize(v);
  x = v[0];
  y = v[1];
  z = v[2];

#if USE_QUATERNION
  qspin[0] = -v[0];
  qspin[1] = v[1];
  qspin[2] = v[2];
  qspin[3] = angle;
#else
  angle *= (3.1415927/180.0);
  c = cos(angle);
  s = sin(angle);

  v_dummy.x = 0;
  v_dummy.y = 0;
  v_dummy.z = 0;

  m_rot.a = (float)(x*x*(1-c)+c);
  m_rot.b = (float)(y*x*(1-c)+(z*s));
  m_rot.c = (float)(x*z*(1-c)-(y*s));
  m_rot.d = (float)(x*y*(1-c)-(z*s));
  m_rot.e = (float)(y*y*(1-c)+c);
  m_rot.f = (float)(y*z*(1-c)+(x*s));
  m_rot.g = (float)(x*z*(1-c)+(y*s));
  m_rot.h = (float)(y*z*(1-c)-(x*s));
  m_rot.i = (float)(z*z*(1-c)+c);

  transform_multiply(&v_dummy,&(ldraw_commandline_opts.A),
		     &v_dummy,&m_rot,
		     &v_temp, &m_temp);
  ldraw_commandline_opts.A = *m_temp;

  free(m_temp);
  free(v_temp);
#endif
}

/***************************************************************/
void UnProjectMouse(int x, int y, GLdouble *px, GLdouble *py, GLdouble *pz)
{
  GLdouble model[4*4];
  GLdouble proj[4*4];
  GLint view[4];
  GLdouble pan_x, pan_y, pan_z;

  // Load an identity matrix before dealing with the mouse.
  // Otherwise we get some odd jumpy mouse effects.
  // It gets reset before rendering by rendersetup() in render().
  glMatrixMode( GL_MODELVIEW );
  glPushMatrix();
  glLoadIdentity();
  glGetDoublev(GL_MODELVIEW_MATRIX, model);
  glPopMatrix();
  glGetDoublev(GL_PROJECTION_MATRIX, proj);
  glGetIntegerv(GL_VIEWPORT, view);
  gluUnProject((GLdouble)x, (GLdouble)y, 1.0,
	       model, proj, view,
	       &pan_x, &pan_y, &pan_z);
  pan_y = -pan_y;
  
  *px = pan_x;
  *py = pan_y;
  *pz = pan_z;

#if 0
    printf("view(%d, %d, %d, %d)\n", view[0], view[1] , view[2], view[3]);
    printf("model(%g,%g,%g,%g, %g,%g,%g,%g %g,%g,%g,%g, %g,%g,%g,%g)\n", 
	   model[0], model[1] , model[2], model[3],
	   model[4], model[5] , model[6], model[7],
	   model[8], model[9] , model[10], model[11],
	   model[12], model[13] , model[14], model[15]);
    printf("proj(%g,%g,%g,%g, %g,%g,%g,%g, "
	   "%g,%g,%g,%g, %g,%g,%g,%g)\n", 
	   proj[0], proj[1] , proj[2], proj[3],
	   proj[4], proj[5] , proj[6], proj[7],
	   proj[8], proj[9] , proj[10], proj[11],
	   proj[12], proj[13] , proj[14], proj[15]);
#endif
}

/***************************************************************/
void mouse(int button, int state, int x, int y)
{
  GLdouble pan_x, pan_y, pan_z, x_angle, y_angle, angle, depth;
  UNUSED(x_angle);
  UNUSED(y_angle);

  glutModifiers = glutGetModifiers(); // Glut doesn't like this in motion() fn.

  mouse_state = state;
  mouse_button = button;
  
#ifdef TEST_MUI_GUI
  if (button == GLUT_RIGHT_BUTTON) {
    mui_test();
    return;
  }
#endif

#ifdef WINDOWS
#else
  // Middle button is often used for PASTE in X Windows.
  // But on Windows pushing mouse-wheel is middle but, bad choice for paste!)
  // And the Windows glut-wheel patch does 10 pix middle drags for wheel turn.
  // On XFree 4.x mouse-wheel forward = button 4, backwards = button 5.
  // In glut.h GLUT_L,M,R = 0,1,2.  So if wheel = 4,5, what is 3?
  if (button == GLUT_MIDDLE_BUTTON) {
    printf("GLUT_MIDDLE_BUTTON ");
    if (state == GLUT_DOWN)
      printf("GLUT_DOWN\n");
    else
    {
      printf("GLUT_UP\n");
      keyboard(22, x, y); // Ctrl-V = PASTE.
    }
    return;
  }
#endif

  if (button != GLUT_LEFT_BUTTON) {
    return;
  }

  // Only count mouse button down GLUT_UP
  if (state == GLUT_DOWN)
  {
    //printModelMat("ModelD");

    UnProjectMouse(x, y, &pan_x, &pan_y, &pan_z);

    pan_start_x = pan_end_x = pan_x;
    pan_start_y = pan_end_y = pan_y;

    if (ldraw_commandline_opts.debug_level == 1)
      printf("pdn(%d, %d), -> (%0.2f, %0.2f, %0.2f)\n", x, y, pan_x, pan_y, pan_z);

    pan_start_sx = x; // Save start point for when we pass hysteresis.
    pan_start_sy = y;
    // Do not hide or warp the pointer until past hysteresis.
#ifdef VISIBLE_SPIN_CURSOR
    //glutSetCursor(GLUT_CURSOR_INHERIT);
#else
    //glutSetCursor(GLUT_CURSOR_NONE);
#endif
    //glutWarpPointer(Width/2, Height/2);

    if (!panlock)
      panning = 0;
    return;
  }
  else if (panlock)
    return;
  else if (panning)
  {
    // Restore wireframe and stud draw modes.
    ldraw_commandline_opts.F = pan_start_F;
    LineChecking = 0;

    // The LdrawOblique matrix is a projection matrix.
    // Substitute Oblique view matrix so we can rotate it.
    if (m_viewMatrix == LdrawOblique)
    {
      m_viewMatrix = Oblique;
      parse_view(m_viewMatrix);
    }
    else //if (editing) 
    { // lets call any random orientation oblique, just dont parse it.
      m_viewMatrix = Oblique;
    }
 
    UnProjectMouse(x, y, &pan_x, &pan_y, &pan_z);

    if (ldraw_commandline_opts.debug_level == 1)
      printf("pup(%d, %d), -> (%0.2f, %0.2f, %0.2f)\n", x, y, pan_x, pan_y, pan_z);
    // NOTE:  This would be a great place to rotate or pan the model.

    //glRotated(pan_x-pan_start_x, 1.0, 0.0, 0.0);
    //glRotated(pan_y-pan_start_y, 0.0, 1.0, 0.0);

    // Convert from world coords across screen plane to angle thru origin.
    depth = projection_depth;

#if 0
    pan_z = pan_x - pan_start_x;
    y_angle = atan2(pan_z, depth);
    y_angle *= -1.0;
    y_angle *= (180.0/3.1415927);
    if (ldraw_commandline_opts.debug_level == 1)
      printf("rotating(%0.2f - %0.2f = %0.2f) by angle %0.2f\n", pan_x, pan_start_x, pan_z, y_angle);

    pan_z = pan_y - pan_start_y;
    x_angle = atan2(pan_z, depth);
    x_angle *= -1.0;
    x_angle *= (180.0/3.1415927);
    if (ldraw_commandline_opts.debug_level == 1)
      printf("rotating(%0.2f - %0.2f = %0.2f) by angle %0.2f\n", pan_y, pan_start_y, pan_z, x_angle);
    //rotate_the_model(0.0, x_angle);
    //rotate_the_model(y_angle, 0.0 );
    rotate_the_model(y_angle, x_angle );
#else
    pan_x -= pan_start_x;
    pan_y -= pan_start_y;
    pan_y = -pan_y; //Beats me why.
#if DEPTH_BASED_MOUSE_SPIN
    pan_z = max(fabs(pan_x), fabs(pan_y));
    angle = atan2(pan_z, depth);
    //angle *= -1.0;
    angle *= 5.0;
    angle *= (180.0/3.1415927);
#else
    //angle = 2.0 * max(fabs(x*3.1415927/Width), fabs(y*3.1415927/Width));
    angle = 0.5 * max(fabs(x-(Width/2.0)), fabs(y-(Height/2.0)));
#endif
    if (ldraw_commandline_opts.debug_level == 1)
      printf("rotating about(%0.2f, %0.2f) by angle %0.2f\n", pan_y, -pan_x, angle);
#ifdef USE_F00_CAMERA
    if (glutModifiers & GLUT_ACTIVE_CTRL)
    {
    }
    else
#endif
    rotate_about(pan_y, -pan_x, 0.0, angle );
#endif

    panning = 0;
    dirtyWindow = 1;  // Do not increment step counter on post pan redraw.
    glutSetCursor(GLUT_CURSOR_INHERIT);
#ifndef NOT_WARPING
    glutWarpPointer(Width/2, Height/2);
#endif
  }

  glutPostRedisplay();
}

/***************************************************************/
void
motion(int x, int y)
{
  GLdouble pan_x, pan_y, pan_z, x_angle, y_angle, angle, depth;
  GLdouble p_x, p_y;
  //int glutModifiers;  // Glut doesn't like this here!

  // Introduce a delay to avoid too many printfs during debugging.
  //sleep(100); 

  if (mouse_state == GLUT_DOWN && mouse_button == GLUT_LEFT_BUTTON) {
    // Load an identity matrix before dealing with the mouse.
    // Otherwise we get some odd jumpy mouse effects.
    // It gets reset before rendering by rendersetup() in render().
    //glMatrixMode( GL_MODELVIEW );
    //glLoadIdentity();

    UnProjectMouse(x, y, &pan_x, &pan_y, &pan_z);

    if (ldraw_commandline_opts.debug_level == 1)
      printf("pan(%d, %d), -> (%0.2f, %0.2f, %0.2f)\n", x, y, pan_x, pan_y, pan_z);
    // NOTE:  This would be a nice place to rotate or pan the model.
    //        (If only the redisplay were more realtime)
    
    if (panning == 0)
    {
      // Add some hysteresis before we start panning.
      // NOTE:  This should really be done in screen coords, 
      // not world coords that have gone thru gluUnProject.
      // In fact the whole spinning thing is messed up when done 
      // in perspective mode (too fast) because I think I use the 
      // rear plane of the viewing volume for the z coord.
      // Should I use -1.0 for the z coord to get the front plane?
      if((fabs(pan_x -pan_start_x) < 4) && (fabs(pan_y -pan_start_y) < 4))
      {
	if (ldraw_commandline_opts.debug_level == 1)
	  printf("hysteresis = %0.2f, %0.2f\n", 
		 fabs(pan_x -pan_start_x), fabs(pan_y -pan_start_y));
	return;
      }
      // Save draw modes, then switch to wireframe and turn off studs.
      pan_start_F = ldraw_commandline_opts.F;
      ldraw_commandline_opts.F = pan_visible; 

      // Pretend we skipped hysteresis check and warped pointer right away.
#ifndef NOT_WARPING
      x += (Width/2) - pan_start_sx;
      y += (Height/2) - pan_start_sy;
#endif
    }
#ifdef VISIBLE_SPIN_CURSOR
    glutSetCursor(GLUT_CURSOR_INHERIT);
#else
    glutSetCursor(GLUT_CURSOR_NONE);
#endif
    panning = 1;

    // Check for shift or ctrl mouse drag changes on the fly.
    //glutModifiers = glutGetModifiers(); // Glut doesn't like this here!
#ifdef USE_F00_CAMERA
    if (glutModifiers & GLUT_ACTIVE_CTRL)
      ldraw_commandline_opts.F = pan_visible; 
    else 
#endif
    if (glutModifiers & GLUT_ACTIVE_ALT)
    {
      if (glutModifiers & GLUT_ACTIVE_SHIFT)
	ldraw_commandline_opts.F = (TYPE_F_SHADED_MODE); 
      else 
	ldraw_commandline_opts.F = (TYPE_F_STUDLESS_MODE | TYPE_F_SHADED_MODE);
    }
    else 
      ldraw_commandline_opts.F = pan_visible; 

#ifdef NEW_SPIN_MODE_TEST
    // Add this to the rest of the spin modes in opts.F
    if (pan_visible & TYPE_F_NO_POLYGONS)
      LineChecking = 1;
#endif

    if ((ldraw_commandline_opts.F & TYPE_F_INVISIBLE) == TYPE_F_INVISIBLE)
    { // Draw the rubberband line
      //printf("MOTION(%0.2f, %0.2f, %0.2f)\n", pan_x, -pan_y, pan_z);

      // Disable antialiasing and linewidths.
      glDisable( GL_LINE_SMOOTH ); 
      glHint( GL_LINE_SMOOTH_HINT, GL_FASTEST ); // GL_NICEST GL_DONT_CARE
      glDisable( GL_BLEND );
      glLineWidth( 1.0 );
      // Should rerun linequalitysetup() after rubberband is over.

      glDisable( GL_DEPTH_TEST ); /* don't test for depth -- just put in front  */
      glEnable( GL_COLOR_LOGIC_OP ); 
      //glEnable( GL_LOGIC_OP_MODE ); 
      glLogicOp(GL_XOR);
      glColor3f(1.0, 1.0, 1.0); // white
      glBegin(GL_LINES);
      glVertex3f(pan_start_x, pan_start_y, pan_z);
      glVertex3f(pan_end_x, pan_end_y, pan_z);
      glEnd();
      glBegin(GL_LINES);
      glVertex3f(pan_start_x, pan_start_y, pan_z);
      glVertex3f(pan_x, pan_y, pan_z);
      glEnd();
      glLogicOp(GL_COPY);
      glEnable( GL_DEPTH_TEST ); 
      glDisable( GL_COLOR_LOGIC_OP ); 
      //glDisable( GL_LOGIC_OP_MODE ); 
      printf("glFlush(motion)\n"); glFlush();

      pan_end_x = pan_x;
      pan_end_y = pan_y;
    }
    else    // Lets rotate the wireframe.
    {
      p_x = pan_x; //Save next pan_start coords.
      p_y = pan_y;

      // The LdrawOblique matrix is a projection matrix.
      // Substitute Oblique view matrix so we can rotate it.
      if (m_viewMatrix == LdrawOblique)
      {
	m_viewMatrix = Oblique;
	parse_view(m_viewMatrix);
      }

      // Convert from world coords across screen plane to angle thru origin.
      depth = projection_depth;

      pan_x -= pan_start_x;
      pan_y -= pan_start_y;
      pan_y = -pan_y; //Beats me why.
#if DEPTH_BASED_MOUSE_SPIN
      pan_z = max(fabs(pan_x), fabs(pan_y));
      angle = atan2(pan_z, depth);
      //angle *= -1.0;
      angle *= 5.0;
      angle *= (180.0/3.1415927);
#else
#  ifndef NOT_WARPING
      //angle = 2.0 * max(fabs(x*3.1415927/Width), fabs(y*3.1415927/Width));
      angle = 0.5 * max(fabs(x-(Width/2.0)), fabs(y-(Height/2.0)));
#  else
      angle = 0.5 * max(fabs(x-pan_start_sx), fabs(y-pan_start_sy));
#  endif
#endif
      if (ldraw_commandline_opts.debug_level == 1)
	printf("ROTATING about(%0.2f, %0.2f) by angle %0.2f\n", pan_y, -pan_x, angle);
#ifdef USE_F00_CAMERA
      if (glutModifiers & GLUT_ACTIVE_CTRL)
      {
	pan_x = x-(Width/2.0); // dx
	pan_y = y-(Height/2.0); // dy
	if ((pan_x != 0.0) || (pan_y != 0.0))
	{
	  if (glutModifiers & GLUT_ACTIVE_SHIFT)
	  {
	    y_angle = angle * (pan_x / (fabs(pan_x) + fabs(pan_y)));
	    x_angle = angle * (pan_y / (fabs(pan_x) + fabs(pan_y)));
	    y_angle /= 10.0; // make smaller turns
	    x_angle /= 10.0;
	    turnCamera( (GLfloat)(x_angle), (GLfloat)y_angle, 0.0 );
	  }
	  else if (glutModifiers & GLUT_ACTIVE_ALT)
	  {
#if 0	    
	    if (pan_x != 0)
	      truckCamera( pan_x, 1, 0, 0 ); // left, right
#else
	    if (pan_x != 0)
	    {
	      y_angle = angle * (pan_x / (fabs(pan_x) + fabs(pan_y)));
	      y_angle /= 10.0; // make smaller turns
	      turnCamera( 0.0, (GLfloat)y_angle, 0.0 );
	    }
#endif
	    if (pan_y != 0)
	    {
	      if (ldraw_projection_type == 1)
		truckCamera( pan_y, 0, 0, 1 ); // forward, backward
	      else
	      {
		// Trucking camera is not visible in orthographic mode.  Scale instead.
		ldraw_commandline_opts.S *= (1.0 - (0.001 * pan_y));
	      }
	    }
		
	  }
	  else
	  {
	    if (pan_x != 0)
	      truckCamera( pan_x, 1, 0, 0 ); // left, right 
	    if (pan_y != 0)
	      truckCamera( -pan_y, 0, 1, 0 ); // up, down
	  }
	    
	}
      }
      else
#endif
      rotate_about(pan_y, -pan_x, 0.0, angle );

      pan_start_x = p_x; //Set next pan_start coords.
      pan_start_y = p_y;

#ifndef NOT_WARPING
      // Set next pan_start coords to center of screen if far enough away.
      // This should prevent the problem where we stop spinning when the 
      // mouse gets to the edge of the screen.
      if ((p_x > 10) || (p_x < -10) || (p_y > 10) || (p_y < -10))
      {
	//printf("WARP(%0.2f, %0.2f)\n", p_x, p_y);
	glutWarpPointer(Width/2, Height/2);
	pan_start_x = 0;
	pan_start_y = 0;
      }
#else
      pan_start_sx = x; // Save new start point.
      pan_start_sy = y;
#endif

      //printModelMat("ModelM");

      glutPostRedisplay();
    }
  }

}

/***************************************************************/
void
passive_motion(int x, int y)
{
  if (panlock)
    motion(x,y);
}

/************************************************************************/
extern int cached_file_stack_index;

#if 0
// I suspect I should probably call this fn (stolen from yylex())
// every time polling says to reparse the DAT file.
// For now I just reset cached_file_stack_index to 0 and ignore
// any memory leaks that may happen.
extern int include_stack_ptr;
extern CACHED_STREAM cached_streams[MAX_CACHED_FILES];
extern CACHED_STREAM *cached_file_stack[MAX_INCLUDE_DEPTH];

/************************************************************************/
void yy_init_cache() // This should go in lex.yy.c right before yylex()
{
  int i;

  cached_file_stack_index = 0;

  // NOTE: There is some stuff in here that got malloced and should be freed.
  // (Specifically chs->tokens and chs->values)
  include_stack_ptr = 0;
  for (i=0; i<MAX_INCLUDE_DEPTH; i++) {
    cached_file_stack[i] = NULL;
  }
  for (i=0; i<MAX_CACHED_FILES; i++) {
    cached_streams[i].valid = CHS_UNUSED;
    if (chs->tokens != NULL)
      free(chs->tokens)
    if (chs->values != NULL)
      free(chs->values)
  }
  ldlite_profile.cached_files=0;
  ldlite_profile.uncached_files=0;
  ldlite_profile.cache_hits=0;
}
#endif

/***************************************** myGlutIdle() ***********/
void myGlutIdle( void )
{
    char filename[256];
    static int init=0;
    static time_t last_file_time;
    struct stat datstat;
    int ret;

#if 0
    // Can we tell here if the display went well?  It looks like we CANNOT.
    if (exposeEvent())
      printf("DAMAGED in idle\n");
#endif

#ifdef WINDOWS
    Sleep(1); // Glut is a CPU hog.  Give back a millisecond.
    // need to use usleep on other platforms (add to platform.c)
#endif

  /* According to the GLUT specification, the current window is
     undefined during an idle callback.  So we need to explicitly change
     it if necessary */
#ifndef AGL
#if (GLUT_XLIB_IMPLEMENTATION >= 13)
  if (ldraw_commandline_opts.V_x >= -1)
#endif
#endif
  if ( glutGetWindow() != main_window )
    glutSetWindow(main_window);

  if (ldraw_commandline_opts.rotate == 1) 
  {
    printf("IDLE: stepcount = %d, curstep = %d\n",stepcount,curstep);
    if (ldraw_commandline_opts.M == 'P')
      if (stepcount != curstep)
      {
	// Do all the steps BEFORE twirling.
	// make them click the steps when in pause mode like ldlite.
	//glutPostRedisplay();
	return;
      }
    // rotate model
    twirl_angle += twirl_increment;
    if (twirl_angle >= 360.0)
      {
	// All done rotating
	ldraw_commandline_opts.rotate = 0;
	twirl_angle = 0.0;
	// If we just want the output files then quit when idle.
	if ((ldraw_commandline_opts.output == 1) ||
	    (ldraw_commandline_opts.M == 'S') || (ldraw_commandline_opts.M == 'F'))
	  exit(0); //quit the program
	else
	  return;
      }
    //stepcount = 0;
    // This looks funny (not orthogonal) 
    // Perhaps cumulative round off errors in the modeling matrix?
#ifdef USE_GL_TWIRL
    // This is done with the GL model matrix.
#else
    rotate_model();
#endif
    glutPostRedisplay();
    return;
  }

  // NOTE: Should I move polling up before twirl mode?
  if (ldraw_commandline_opts.poll == 1) 
  {
    // Gotta handle polling.
    // Check to see if timestamp on filename has changed.
    concat_path(datfilepath, datfilename, filename);
    ret = stat(filename, &datstat);
    if (!init) 
    {
      init = 1;
      printf("Polling %s = %d\n", filename, datstat.st_mtime);
    }
    else if (ret == -1)
    {
      // printf("Polling %s = error\n", filename);
    }
    else if (datstat.st_mtime != last_file_time)
    {
      printf("Reloading %s = %d\n", filename, last_file_time);
      if (editing)
      {
	// Unselect piece before reloading from poll.
	EraseCurPiece();
	movingpiece = -1;
      }
#ifdef USE_L3_PARSER
      if (parsername == L3_PARSER)
	list_made = 0; // Gotta reparse the file.
      else 
#endif
      {
	cached_file_stack_index=0;  // restart file cache.  Huge mem leak???
	// yy_init_cache() // Fix the huge memory leak???
      }
      curstep = 0; // Reset to first step
      dirtyWindow = 1;
      glutPostRedisplay();
    }
    last_file_time = datstat.st_mtime;
  }

#if 0
  // NOTE:  This does NOT seem to wait till the display() fn is finished.
  // If we just want the output files then quit when idle.
  if ((ldraw_commandline_opts.output == 1) ||
      (ldraw_commandline_opts.M == 'S') || (ldraw_commandline_opts.M == 'F'))
  {
    // Draw the last step.
    if ((ldraw_commandline_opts.M == 'S') || (ldraw_commandline_opts.M == 'F'))
      platform_step(INT_MAX, 0, -1, NULL);
    // quit the program
    exit(0);
  }
#endif
}

/***************************************************************/
// Stolen from the constructor in ldliteCommandLineInfo.cpp
void CldliteCommandLineInfo()
{
#if 0
  ldraw_commandline_opts.A.a=1.0;
  ldraw_commandline_opts.A.b=0.0;
  ldraw_commandline_opts.A.c=1.0;
  ldraw_commandline_opts.A.d=0.5;
  ldraw_commandline_opts.A.e=1.0;
  ldraw_commandline_opts.A.f=-0.5;
  ldraw_commandline_opts.A.g=-1.0;
  ldraw_commandline_opts.A.h=0.0;
  ldraw_commandline_opts.A.i=1.0;
#else
  parse_view(m_viewMatrix);
#endif
  ldraw_commandline_opts.B=15;
  ldraw_commandline_opts.C=7;
  ldraw_commandline_opts.F=0;
  ldraw_commandline_opts.M= 'P';
  ldraw_commandline_opts.O.x=0.0;
  ldraw_commandline_opts.O.y=0.0;
  ldraw_commandline_opts.O.z=0.0;
  ldraw_commandline_opts.S=1.0;
  ldraw_commandline_opts.V_x=0;
  ldraw_commandline_opts.V_y=0;
  ldraw_commandline_opts.poll=0;
  ldraw_commandline_opts.output=0;
  ldraw_commandline_opts.output_depth=0;
  ldraw_commandline_opts.rotate=0;
  ldraw_commandline_opts.debug_level=0;
  ldraw_commandline_opts.log_output=0;
  ldraw_commandline_opts.W=1;
  ldraw_commandline_opts.Z=INT_MIN;
  ldraw_commandline_opts.clip=1;
  ldraw_commandline_opts.image_filetype=4; // BMP24
  ldraw_commandline_opts.center_at_origin=0;
  ldraw_commandline_opts.emitter = 0;
  ldraw_commandline_opts.maxlevel=32767;  // a huge number

  if (zShading)
    ldraw_commandline_opts.F |= TYPE_F_SHADED_MODE;
  if (zWire)
    ldraw_commandline_opts.F |= TYPE_F_NO_POLYGONS; // zWire = 1;
}

/***************************************************************/
void PrintParams(int *argc, char **argv)
{
  char filename[256];
  char randname[256];
  char datname[256];
  FILE *fp;
  char *p;
  int i;

  strcpy(filename, "ldglite");
  
  for (i = 1; i < *argc; i++)
  {
    p = argv[i];
    if ((p[0] != '+') && (p[0] != '-'))
    {
      // It must be a filename.  Save it for parsing.
      strcpy(filename, basename(argv[i]));
      strcpy(datname, argv[i]);
    }
  }

  if ((p = strrchr(filename, '.')) != NULL)
    *p = 0;
  else 
    p = filename + strlen(filename);

  strcat(filename, "_lpub");
  
  sprintf(randname, "%s_%d", filename, (rand()%100));
  strcpy(filename, randname);
  strcat(randname, ".ldr");

  strcat(filename, ".bat");
			      
  fp = fopen(filename, "w+");
  if (!fp)
  {
    printf("Could not open %s\n", filename);
    return;
  }

  for (i = 0; i < *argc; i++)
  {
    p = argv[i];
    fprintf(fp, "%s ", p);
  }
  fprintf(fp, "\n");

  fclose(fp);

  sprintf(filename, "copy %s %s", datname, randname);
  system(filename);
}

/***************************************************************/
// Mostly stolen from ParseParam() in ldliteCommandLineInfo.cpp
void ParseParams(int *argc, char **argv)
{
  char *pszParam;
  int i, n, x, y;

  char type;
  int mode;
  int camera_globe_set = 0;
  int camera_znear_set = 0;
  int camera_zfar_set = 0;

  if (!strlen(datfilename)) // Init datfilename if none parsed yet...
  { 
  // Initialize datfilepath to none so we can take commands from stdin.
  strcpy(datfilename, " ");
#if defined(UNIX)
  strcpy(datfilepath, "./");
#elif defined(MACOS_X)
  strcpy(datfilepath, "./");
#elif defined(MAC)
  strcpy(datfilepath, "");
#elif defined(WINDOWS)
  strcpy(datfilepath, "./");
#else
#error unspecified platform in ParseParams() definition
#endif

  strcpy(dirfilepath, datfilepath);
  }  

  for (i = 1; i < *argc; i++)
  {
    pszParam = argv[i];
    if (pszParam[0] == '+') 
    {
      int n, wx, wy;
      n = sscanf(pszParam,"%c%d,%d",&type, &wx, &wy);
      if ((n > 1) && (wx >= 0))
	XwinPos = wx;
      if ((n > 2) && (wy >= 0))
	YwinPos = wy;
    }
    else if (pszParam[0] == '=')
    {
      // remove '='
      memmove(pszParam, pszParam+1, strlen(pszParam));
      if (pszParam[0])
      {
         char ldconfigfilepath[256];
         strcpy(ldconfigfilename, basename(pszParam));
         strcpy(ldconfigfilepath, dirname(pszParam));
         strcat(ldconfigfilepath, ldconfigfilename);
         strcpy(ldconfig, localize_path(ldconfigfilepath));
         printf("LDConfig = (%s)\n", ldconfig);
      }
    }
    else if (pszParam[0] != '-') 
    {
      // It must be a filename.  Save it for parsing.
      strcpy(datfilename, basename(argv[i]));
      strcpy(datfilepath, dirname(argv[i]));
      strcpy(dirfilepath, datfilepath);
#if 0
      chdir(datfilepath); // problem with chdir to dir with spaces in win32.
#endif
      // we used to put a trailing / on datfilepath
    }
    else
    {
      pszParam++; // skip over the dash char.

      switch(pszParam[0]) {
      case '-':
	if (!strcmp(datfilename,  " "))
	  //strcpy(datfilename, ""); // read from stdin if no filename.
	  strcpy(datfilename, "-"); // read from stdin if no filename.
	break;
      case 'A':
      case 'a':
	sscanf(pszParam,"%c%f,%f,%f,%f,%f,%f,%f,%f,%f",
	       &type,
	       &(ldraw_commandline_opts.A.a),
	       &(ldraw_commandline_opts.A.b),
	       &(ldraw_commandline_opts.A.c),
	       &(ldraw_commandline_opts.A.d),
	       &(ldraw_commandline_opts.A.e),
	       &(ldraw_commandline_opts.A.f),
	       &(ldraw_commandline_opts.A.g),
	       &(ldraw_commandline_opts.A.h),
	       &(ldraw_commandline_opts.A.i));
	break;
      case 'B':
      case 'b':
	sscanf(pszParam,"%c%i",&type,&(ldraw_commandline_opts.B));
	break;
      case 'C':
      case 'c':
	if (toupper(pszParam[1]) == 'A') // Camera FOV angle.
	{
	  float g;
	  sscanf(&pszParam[2],"%f",&g);
	  printf("FOV = %g\n", g);
	  if ((g >= 0.0) && (g <= 360.0))
	    projection_fov = g;
	}
        else if (toupper(pszParam[1]) == 'C') // Camera location.
	{
	  //float v[4][4];
	  double v[4][4];
	  v[0][0] = v[0][1] = v[0][2] = 0.0;
	  ScanPOINTS(v, 1, &(pszParam[2]));
	  printf("CAM = (%lg, %lg, %lg)\n", v[0][0], v[0][1], v[0][2]);
	  projection_fromx = v[0][0];
	  projection_fromy = -v[0][1]; // L3P uses LDRAW y (-OpenGL y).
	  projection_fromz = v[0][2];
	}
        else if ((toupper(pszParam[1]) == 'O') || // Object Origin to look at.
		 ((toupper(pszParam[1]) == 'L') && 
		  (toupper(pszParam[2]) == 'A')))
	{
	  float v[4][4];
	  v[0][0] = v[0][1] = v[0][2] = 0.0;
	  if (toupper(pszParam[1]) == 'O')
	    ScanPoints(v, 1, &(pszParam[2]));
	  else
	    ScanPoints(v, 1, &(pszParam[3]));
	  printf("LOOK AT (%g, %g, %g)\n", v[0][0], v[0][1], v[0][2]);
	  projection_towardx = v[0][0];
	  projection_towardy = -v[0][1]; // L3P uses LDRAW y (-OpenGL y).
	  projection_towardz = v[0][2];
	}
        else if ((toupper(pszParam[1]) == 'U') || // Camera up vector.
		 ((toupper(pszParam[1]) == 'S') && 
		  (toupper(pszParam[2]) == 'K') && 
		  (toupper(pszParam[3]) == 'Y')))
	{
	  float v[4][4];
	  v[0][0] = v[0][1] = v[0][2] = 0.0;
	  if (toupper(pszParam[1]) == 'U')
	    ScanPoints(v, 1, &(pszParam[2]));
	  else
	    ScanPoints(v, 1, &(pszParam[4]));
	  printf("UP = (%g, %g, %g)\n", v[0][0], v[0][1], v[0][2]);
	  projection_upx = v[0][0];
	  projection_upy = -v[0][1]; // L3P uses LDRAW y (-OpenGL y).
	  projection_upz = v[0][2];
	}
        else if (toupper(pszParam[1]) == 'R') // Camera roll vector.
	{
	  // Todo...
	}
        else if (toupper(pszParam[1]) == 'G') // Camera location (on Globe)
	{
	  //float v[4][4];
	  double v[4][4];
	  v[0][0] = v[0][1] = v[0][2] = 0.0;
	  ScanPOINTS(v, 1, &(pszParam[2]));
	  printf("FROM = (%lg, %lg, %lg)\n", v[0][0], v[0][1], v[0][2]);
	  camera_latitude = v[0][0];
	  camera_longitude = v[0][1];
	  camera_distance  = v[0][2];
	  camera_globe_set = 1;
	}
	else
	  n = sscanf(pszParam,"%c%i",&type,&(ldraw_commandline_opts.C));
	break;
      case 'D':
      case 'd':
	{
	  int lev;
	  n = sscanf(pszParam,"%c%d,%g",&type,&lev, &dimAmount);
	  if (n == 3)
	  {
	    dimLevel = lev;
	    if (dimAmount < 0.0)
	      dimAmount = 0.0;
	    if (dimAmount > 1.0)
	      dimAmount = 1.0;
	    printf("Dimlevel = %d, %g\n", dimLevel, dimAmount);
	  }
	  else 
	    ldraw_commandline_opts.maxlevel = lev;
	}
	break;
      case 'E':
      case 'e':
	if ((toupper(pszParam[1]) == 'P') && (toupper(pszParam[2]) == 'S'))
	    EPS_OUTPUT_FIGURED_OUT = 1;
	else
	{
	  int j;
	  float pfact, punit;
	  j = sscanf(pszParam,"%c%f,%f,%f",&type,&z_line_offset,&pfact,&punit);
	  if (z_line_offset > 1.0)
	    z_line_offset = 1.0;
	  printf("z_line_offset = %f\n", z_line_offset);
	  if (pszParam[0] == 'E')      // For Capital E use z_line_offset
	    PolygonOffsetEnabled = 0;  // instead of glpolygonoffset.
	  if (j == 4)
	  {
	    PolygonOffsetFactor = pfact;
	    PolygonOffsetunits = punit;
	    printf("polygonoffset(%f, %f)\n", pfact, punit);
	  }
	}
	break;
      case 'F':
      case 'f':
	{
	  char c;
	  sscanf(pszParam,"%c%c",&type,&c);
	  c = toupper(c);
	  switch (c) {
	  case 'H': // mnemonic = sHading? =Hi quality?
	    ldraw_commandline_opts.F |= TYPE_F_SHADED_MODE; // zShading = 1; 
	    break;
	  case 'S':
	    if (toupper(pszParam[2]) == 'L')
	      ldraw_commandline_opts.F |= TYPE_F_STUDLINE_MODE;
	    else
	      ldraw_commandline_opts.F |= TYPE_F_STUDLESS_MODE;
	    break;
	  case 'W':
	  case 'L':
	    ldraw_commandline_opts.F |= TYPE_F_NO_POLYGONS; // zWire = 1;
	    break;
	  case 'N': // mnemonic = normal (no shading)
	    ldraw_commandline_opts.F &= ~(TYPE_F_NO_POLYGONS); // zWire = 0;
	    ldraw_commandline_opts.F &= ~(TYPE_F_SHADED_MODE); // zShading = 0 
	    break;
	  case 'R':
	  case 'E':
	    ldraw_commandline_opts.F |= TYPE_F_NO_LINES; // no Edgelines
	    break;
	  case 'F': // Fog.
	    {
	      float v[4][4];
	      v[0][0] = 1,0;   // LINEAR
	      v[0][1] = fogEnd; // or fogDensity
	      v[0][2] = fogStart;
	      v[1][0] = fogColor[0];
	      v[1][1] = fogColor[1];
	      v[1][2] = fogColor[2];
	      ScanPoints(v, 2, &(pszParam[2]));
	      printf("fogging = (%g, %g, %g)\n", v[0][0], v[0][1], v[0][2]);
	      fogging = (int) v[0][0];
	      if (fogging < 1) fogging = 1;
	      if (fogging > 3) fogging = 3;
	      if (fogging == 1)
	      {
		fogStart = v[0][1];
		fogEnd = v[0][2];
	      }
	      else
		fogDensity = v[0][1];
	      printf("fogColor = (%g, %g, %g)\n", v[1][0], v[1][1], v[1][2]);
	      fogColor[0] = v[1][0];
	      fogColor[1] = v[1][1];
	      fogColor[2] = v[1][2];
	    }
	  case 'M': // Motion? Mouse Movement?
	    if ((pszParam[2] == '0') && (toupper(pszParam[3]) == 'X'))
	      sscanf(pszParam,"%c%c%x",&type,&c,&pan_visible);
	    else
	      sscanf(pszParam,"%c%c%d",&type,&c,&pan_visible);
	    break;
	  }
	}
	break;
      case 'G':
      case 'g':
#ifdef USE_GLFONT
	if (toupper(pszParam[1]) == 'F') // Glfont file name
	{
	  if (pszParam[2])
	  {
	    fontname = strdup(&pszParam[2]);
	    printf("Loading glfont %s\n", fontname);
	  }
	  else
	    fontname = strdup("ariel.glf");
	}
	else
#endif
	  ldraw_commandline_opts.debug_level = 1;
	break;
      case 'i':
      case 'I':
	sscanf(pszParam,"%c%d",&type,&ldraw_image_type);
	// Turn off cropping (image size = window size) if negative.
	if (ldraw_image_type < 0)
	{
	  ldraw_image_type *= -1;
	  cropping = 0;
	}
	break;
      case 'J':
	ldraw_projection_type = 1; // Use Perspective projection.
	/* LdrawOblique only makes sense in ortho projection. */
	if (m_viewMatrix == LdrawOblique)
	{
	  m_viewMatrix = Oblique; // Turn off LdrawOblique model/proj matrix.
	  parse_view(m_viewMatrix);
	}
	break;
      case 'j':
	ldraw_projection_type = 0; // Use Orthographic projection.
	/* Also disable LdrawOblique.  Press 0 key to view LdrawOblique. */
	if (m_viewMatrix == LdrawOblique)
	{
	  m_viewMatrix = Oblique; // Turn off LdrawOblique model/proj matrix.
	  parse_view(m_viewMatrix);
	}
	break;
      case 'K':
      case 'k':
	ldraw_commandline_opts.center_at_origin=1;
	break;
      case 'L':
      case 'l':
	// -l3 forces l3 parser, -ld forces ldlite parser, -l sets logging.
	if (pszParam[1] == '3')
	{
	  parsername = L3_PARSER;
	  use_quads = 1;
	  printf("Parser = L3\n");
	}
	else if (toupper(pszParam[1]) == 'D')
	{
	  parsername = LDLITE_PARSER;
	  use_quads = 0;
	  printf("Parser = LDLite\n");
	}
	else if (pszParam[1] == 'e')
	{
	  SOLID_EDIT_MODE = 0;
	  editing = 1;
	  _strlwr(pszParam);
	  if (strstr(pszParam, "ledit"))
	  {
	    ldraw_commandline_opts.F |= TYPE_F_STUDLINE_MODE;
	    DrawToCurPiece = 1;
	  }
	}
	else if (pszParam[1] == 'E')
	{
	  SOLID_EDIT_MODE = 1;
	  editing = 1;
	  _strlwr(pszParam);
	  if (strstr(pszParam, "ledit"))
	  {
	    ldraw_commandline_opts.F |= TYPE_F_STUDLINE_MODE;
	    DrawToCurPiece = 1;
	  }
	}
	else if (pszParam[1] == 'c') //Spotlight coords
	{
	  float v[4][4];
	  char *colorstring;
	  v[0][0] = -1.0;
	  v[0][1] = v[0][2] = 1.0;
	  v[1][0] = v[1][1] = v[1][2] = 0.5;
	  ScanPoints(v, 2, &(pszParam[2]));
	  printf("LightPos = (%g, %g, %g)\n", v[0][0], v[0][1], v[0][2]);
	  lightposition0[0] = v[0][0];
	  lightposition0[1] = v[0][1];
	  lightposition0[2] = v[0][2];
	  printf("LightColor = (%g, %g, %g)\n", v[1][0], v[1][1], v[1][2]);
	  lightcolor0[0] = v[1][0];
	  lightcolor0[1] = v[1][1];
	  lightcolor0[2] = v[1][2];
      UNUSED(colorstring);
	}
	else if (pszParam[1] == 'C') // Ambient light color
	{
	  float v[4][4];
	  v[0][0] = v[0][1] = v[0][2] = 0.75;
	  ScanPoints(v, 1, &(pszParam[2]));
	  printf("AmbientColor = (%g, %g, %g)\n", v[0][0], v[0][1], v[0][2]);
	  lightcolor1[0] = v[0][0];
	  lightcolor1[1] = v[0][1];
	  lightcolor1[2] = v[0][2];
	}
	else 
	  ldraw_commandline_opts.log_output = 1;
	break;
      case 'M':
      case 'm':
	sscanf(pszParam,"%c%c",&type,&(ldraw_commandline_opts.M));
	// Uppercase S = save steps AND render it offscreen if possible.
	if (ldraw_commandline_opts.M == 'S')
	  OffScreenRendering = SetOffScreenRendering();
	// Uppercase F = save final step AND render it offscreen if possible.
	if (ldraw_commandline_opts.M == 'F')
	  OffScreenRendering = SetOffScreenRendering();
	ldraw_commandline_opts.M = toupper(ldraw_commandline_opts.M);
	if (pszParam[2])
	  picfilename = strdup(&pszParam[2]);
	break;
      case 'O':
      case 'o':
	{
	  float dx, dy;
	  sscanf(pszParam,"%c%f,%f",&type, &dx, &dy);
	  ldraw_commandline_opts.O.x = dx;
	  ldraw_commandline_opts.O.y = dy;
	  ldraw_commandline_opts.O.z = 0.0;
	}
	break;
      case 'N':
      case 'n':
	sscanf(pszParam,"%c%d",&type, &zDetailLevel);
	if (zDetailLevel >= TYPE_MODEL)
	  zDetailLevel = TYPE_MODEL;
	else if (zDetailLevel >= TYPE_PART)
	  zDetailLevel = TYPE_PART;
	else 
	  zDetailLevel = TYPE_P;
	break;
      case 'P':
      case 'p':
	ldraw_commandline_opts.poll= 1;
	break;
      case 'Q':
      case 'q':
	qualityLines = 1;
	break;
      case 'R':
      case 'r':
	sscanf(pszParam,"%c%s",&type, &output_file_name);
	ldraw_commandline_opts.output=1;
	ldraw_commandline_opts.output_depth=1;
	printf("Save (%s)\n", output_file_name);
	break;
      case 's':
	//downsample = 1;
      case 'S':
#if 0
	sscanf(pszParam,"%c%g",&type,&(ldraw_commandline_opts.S));
#else
	{
	  float s;
	  sscanf(pszParam,"%c%g",&type,&s);
	  if (s != 0.0)
	    ldraw_commandline_opts.S = s;
	  else
	    autoscaling = 1;
	}
#endif
	break;
      case 'T':
      case 't':
	ldraw_commandline_opts.rotate = 1;
	break;
#ifdef TILE_RENDER_OPTION
      case 'U':
      case 'u':
	// We only do this for non-interactive modes like -MS. (check later)
	tiledRendering = 1;
	sscanf(pszParam,"%c%d,%d", 
	       &type, &TILE_IMAGE_WIDTH, &TILE_IMAGE_HEIGHT);
	break;
#endif
      case 'V':
      case 'v':
	if (strstr(pszParam, ","))
	{
	  sscanf(pszParam,"%c%d,%d", &type, &x, &y);
	  if ((x > 0) && (y > 0))
	  {
	    ldraw_commandline_opts.V_x = x;
	    ldraw_commandline_opts.V_y = y;
	  }
	  break;
	}
	sscanf(pszParam,"%c%d",&type, &mode);
	switch(mode) {
	case -2:
	  ldraw_commandline_opts.V_x=-2;
	  ldraw_commandline_opts.V_y=-2;
	  break;
	case -1:
	  ldraw_commandline_opts.V_x=-1;
	  ldraw_commandline_opts.V_y=-1;
	  break;
	case 0:
	  ldraw_commandline_opts.V_x=320;
	  ldraw_commandline_opts.V_y=200;
	  break;
	case 1:
	  ldraw_commandline_opts.V_x=640;
	  ldraw_commandline_opts.V_y=200;
	  break;
	case 2:
	  ldraw_commandline_opts.V_x=640;
	  ldraw_commandline_opts.V_y=350;
	  break;
	case 3:
	  ldraw_commandline_opts.V_x=640;
	  ldraw_commandline_opts.V_y=480;
	  break;
	case 4:
	  ldraw_commandline_opts.V_x=800;
	  ldraw_commandline_opts.V_y=600;
	  break;
	case 5:
	  ldraw_commandline_opts.V_x=1024;
	  ldraw_commandline_opts.V_y=768;
	  break;
	case 6:
	  ldraw_commandline_opts.V_x=1280;
	  ldraw_commandline_opts.V_y=1024;
	case 7:
	  ldraw_commandline_opts.V_x=1600;
	  ldraw_commandline_opts.V_y=1024;
	case 8:
	  ldraw_commandline_opts.V_x=1600;
	  ldraw_commandline_opts.V_y=1200;
	  break;
	}
	break;
#ifdef WINDOWS
      case '&':
	FreeConsole();
	// NOTE: Perhaps I should compile with -mwindows for no console
	// and then add a console later with AllocConsole() if no -&
	// Windows sucks!!!  This should be handled by the cmd line shell!
	// Perhaps I should write a intermedite app DETACH.EXE which puts it
	// runs its commandline as a background process.
	// NOTE:  On win2k (and perhaps) NT you must use quotes: "-&"
	// Apparently & means something (but not run in background) :-(
	break;
#endif
      case 'W':
      case 'w':
	sscanf(pszParam,"%c%f",&type, &lineWidth);
	if (lineWidth < 0.0)
	  lineWidth = 0.0;
	break;
      case 'X':
      case 'x':
	drawAxis = 1;
	break;
      case 'Z':
      case 'z':
	{
#if 0
	  double g;
	  sscanf(pszParam,"%c%g",&type,&g);
	  ldraw_commandline_opts.Z = (int) (Z_SCALE_FACTOR * g + 0.5);
#else
	  float g;
	  sscanf(pszParam,"%c%f",&type,&g);
	  if (pszParam[0] == 'z')
	  {
	    projection_znear = g;
	    camera_znear_set = 1;
	  }
	  else // (pszParam[0] == 'Z')
	  {
	    projection_zfar = g;
	    camera_zfar_set = 1;
	  }
	  printf("ZClip = (%g, %g)\n", projection_znear, projection_zfar);
#endif
	}
	break;
      case '2': // Downsample when writing image file, and/or prescale up.  
	{
	  char *p;
	  for (p = pszParam; p; p = strchr(p, ','))
	  {
	    int j, n;
        UNUSED(n);

	    if (*p == ',')
	      p++; // skip over the comma char.
	    if (2 == sscanf(p,"%d%c",&j, &type)) {
	      if (tolower(type) == 'g') // G for Gaussian blur filtered decimation.
		downsample = 1; // Someday this could be j-1 for bigger filter.
	      else if (tolower(type) == 'x') // X for eg. 2x upscale.
		upscale = j;
	    }
	  }
	  printf("Downsample = (%d, %d)\n", downsample, upscale);
	break;
	}
      }
    }
  }
  
  // Init ldconfigfilename and ldconfig with LDraw LDConfig default if not set yet
  if (!strlen(ldconfig))
  {
    sprintf(ldconfigfilename, "LDConfig.ldr");
    concat_path(pathname, ldconfigfilename, ldconfig);
    printf("Using default LDConfig (%s)\n", ldconfig);
  }

  // Tiled rendering does not work offscreen.  Just use one big bitmap.
  if (OffScreenRendering && tiledRendering)
  {
    tiledRendering = 0;
    x = TILE_IMAGE_WIDTH;
    y = TILE_IMAGE_HEIGHT;
    if ((x > 0) && (y > 0))
    {
      ldraw_commandline_opts.V_x = x;
      ldraw_commandline_opts.V_y = y;
    }
  }

#ifdef FORCE_2G2X
  // DEBUG.  DEBUG.  DEBUG.  DEBUG.  DEBUG.  DEBUG.  DEBUG. 
  // This is a nice spot to add test settings for LPub3d.
  // Force -2g2x, set background to black, and lighting to head on.   
  if (OffScreenRendering) // Scale up render window.
  {
    downsample = 1;
    if (upscale == 0) 
      upscale = 2;
    ldraw_commandline_opts.B = 0x2000000;
    lightposition0[0] = 0.0;
    lightposition0[1] = 1000.0;
    lightposition0[2] = 1000.0;
    lightposition0[3] = 0.0;
  }
  // DEBUG.  DEBUG.  DEBUG.  DEBUG.  DEBUG.  DEBUG.  DEBUG. 
#endif

  // If upscaling (in preparation for eventual downsample) then scale up scene.
  if (upscale) {
    ldraw_commandline_opts.S *= upscale;
    lineWidth *= upscale;
    if (OffScreenRendering || camera_globe_set) // Scale up render window.
    {
      camera_distance *= upscale;
      ldraw_commandline_opts.V_x *= upscale;
      ldraw_commandline_opts.V_y *= upscale;
      ldraw_commandline_opts.O.x *= upscale;
      ldraw_commandline_opts.O.y *= upscale;
      ldraw_commandline_opts.O.z *= upscale;
    }
    upscale = 1; // only apply once.  OSX can call ParseParams twice.
  }

  if (camera_globe_set)
  {				 
    float v[4][4];
    double distance;
    double lo, la;
    double x, y, z;
    UNUSED(distance);
    UNUSED(x);
    UNUSED(y);
    UNUSED(z);

    if (camera_distance <= 0.0)
    {
      camera_distance = projection_fromz;
      //distance = sqrt(x*x + y*y + z*z);
    }
    else // Adjust clip planes for camera_distance (if not manually set).
    {
      if (camera_distance > projection_fromz)
	if (camera_zfar_set == 0)
	  projection_zfar = camera_distance + 3000;
      if (camera_distance > 3000)
	if (camera_znear_set == 0)
	  projection_znear = 100;
      if (camera_distance > 10000)
	if (camera_znear_set == 0)
	  projection_znear = camera_distance - 3000;
    }

    printf("Znear, Zfar = %g, %g\n", projection_znear, projection_zfar);

#if 0
    // Since spin is based on Oblique, subtract -cg30,45.
    lo = 3.1415927 * (camera_longitude-45) / 180.0;
    projection_fromz = camera_distance * cos(lo);
    projection_fromx = camera_distance * sin(lo);
    la = 3.1415927 * (camera_latitude -30.0) / 180.0;
    projection_fromy = camera_distance * sin(la);
#else
    // Base spin on Front but call it Oblique, subtract nothing.
    lo = 3.1415927 * camera_longitude / 180.0;
    projection_fromz = camera_distance * cos(lo);
    projection_fromx = camera_distance * sin(lo);
    la = 3.1415927 * camera_latitude / 180.0;
    projection_fromy = camera_distance * sin(la);
#endif

    projection_fromz *= cos(la);
    projection_fromx *= cos(la);

    // Err, should I also add projection_toward to projection_from
    // so the globe is centered around the lookat point.  What does
    // L3P do?  Test it with a small stack of bricks.

    // What happens to the up vector for negative latitudes?
    // The new l3p says -cr roll vector defaults to 0.
    
    v[0][0] = projection_fromx;
    v[0][1] = projection_fromy;
    v[0][2] = projection_fromz;
    printf("CAM = (%g, %g, %g)\n", v[0][0], v[0][1], v[0][2]);

    // Yuck!  Gotta get rid of LdrawOblique!
    // L3p defaults to Oblique view (-cg30,45) but uses front view for -cg.
    // L3p puts the origin at the center of model bbox, not ldraw (0,0,0).
    // Maybe I should offset by the center of the model bbox to match it.
    // Unfortunately I don't have that until I parse the model...

    // When would it not be oblique in parseparams?  ALWAYS switch to Front for -cg.
    // The real problem here is if someone passed a matrix in via -a.
    // I'd still be *calling* it oblique, but it could be anything.
    // Maybe we want to move the model by it and then apply the camera settings.
    // Gotta think about that.  For now, just switch to front.
    //if (m_viewMatrix == LdrawOblique) 
    {
      m_viewMatrix = Front;
      parse_view(m_viewMatrix);
      m_viewMatrix = Oblique; // Use Front, but call it oblique for spin.
    }

    // Does the view matrix get set back to oblique when I use the arrows?
    // Arrow left, then back right.  The view is not the same.  What is up?
    // Did this happen in version 1.1.5?

    // I think panning works from Oblique, so I should go with that too.
    // That doesn't seem to work though.  I get both tilts.  I must make
    // it work with Oblique.  That's used everywhere.
  }
}

/***************************************************************/
int InitInstance()
{
  char *env_str;

  // Probably should use basename(argv[0]) instead of "ldlite"
  // Big switch from ldlite.  Turn shading is on by default.
  zShading = GetProfileInt("ldlite","shading",1);
  //	zDetailLevel = TYPE_P; // should checkmark the menu item later!
  zDetailLevel = GetProfileInt("ldlite","detail",TYPE_P);
  zWire = GetProfileInt("ldlite","wireframe",0);

  // Initialize the default ldglite commandline info structure.
  CldliteCommandLineInfo();

  // Get any default command line args from the environment.
  env_str = platform_getenv("LDGLITEARGS");
  if (env_str != NULL)
  {
    int  i;
    char *argv[32];
    char seps[] = " \t"; 
    char *token;
    char *str;

    str = strdup(env_str);
    argv[0] = NULL;
    for (i = 1,token = strtok( str, seps );
	 token != NULL;
	 token = strtok( NULL, seps ))
    {
      argv[i] = token;
      i++;
      if (i == 32)
      {
	ParseParams(&i, argv);
	i = 1;
      }
    }
    
    if (i >1)
      ParseParams(&i, argv);
  }
  return 0;
}

/***************************************************************/
// Called when user wants to open a new file
int setfilename(char *newfile)
{
  strcpy(datfilename, basename(newfile));
  strcpy(datfilepath, dirname(newfile));
  strcpy(dirfilepath, datfilepath);

  platform_setdir();
  return 0;
}


/***************************************************************/
int setGlutCallbacks()
{
  // I think I have to reregister all of these again for gamemode.
  glutDisplayFunc(display);
  glutReshapeFunc(reshapeCB);
  glutKeyboardFunc(keyboard);
  glutSpecialFunc(fnkeys);
  glutMouseFunc(mouse);
  glutMotionFunc(motion);
  glutIdleFunc(myGlutIdle);
  glutPassiveMotionFunc(passive_motion);
  glutWindowStatusFunc(visibility);
  glutVisibilityFunc(VISIBILITY);
  glutMenuStateFunc(NULL); //#ifdef TEST_MUI_GUI
  return 0;
}

/***************************************************************/
int registerGlutCallbacks()
{
  // I think I have to reregister all of these again for gamemode.
  setGlutCallbacks();
#ifdef USE_GLUT_MENUS
  glutSetMenu(mainmenunum); // Reset the current menu to the main menu.
  glutAttachMenu(GLUT_RIGHT_BUTTON); // And reattach it
#endif
  return 0;
}

/***************************************************************/
char *
str_replace ( const char *string, const char *substr, const char *replacement, int *tokcount){
  char *tok = NULL;
  char *newstr = NULL;
  char *oldstr = NULL;
  char *head = NULL;
  int count = 0;

  /* if either substr or replacement is NULL, duplicate string a let caller handle it */
  if ( substr == NULL || replacement == NULL ) return strdup (string);
  newstr = strdup (string);
  head = newstr;
  while ( (tok = strstr ( head, substr ))){
    oldstr = newstr;
    newstr = malloc ( strlen ( oldstr ) - strlen ( substr ) + strlen ( replacement ) + 1 );
    /*failed to alloc mem, free old string and return NULL */
    if ( newstr == NULL ){
      free (oldstr);
      return NULL;
    } else {
      count++;
    }
    memcpy ( newstr, oldstr, tok - oldstr );
    memcpy ( newstr + (tok - oldstr), replacement, strlen ( replacement ) );
    memcpy ( newstr + (tok - oldstr) + strlen( replacement ), tok + strlen ( substr ), strlen ( oldstr ) - strlen ( substr ) - ( tok - oldstr ) );
    memset ( newstr + strlen ( oldstr ) - strlen ( substr ) + strlen ( replacement ) , 0, 1 );
    /* move back head right after the last replacement */
    head = newstr + (tok - oldstr) + strlen( replacement );
    free (oldstr);
  }
  *tokcount = count;
  return newstr;
}

/***************************************************************/
int getDisplayProperties()
{
  char *str = NULL;
  char *fboSupport = NULL;
  char *formatted_extstr = NULL;
  char substr[] = " ";
  char replacement[] = "\n";
  int newcontext = 0;
  int extcount = 0;

#ifdef SIMULATE_APPLE_BUGS
  if (strcmp(verstr, "1.1 APPLE-1.1"))
  {	     
    verstr = strdup("1.1 APPLE-1.1");
    vendstr = strdup("Apple");
    rendstr = strdup("Generic");
    newcontext = 1;
  }
#else
  str = (char *) glGetString(GL_VERSION);
  if (str && strcmp(str, verstr))
  {
    newcontext = 1;
    verstr = strdup(str);
  }
  str = (char *)glGetString(GL_VENDOR);
  if (str && strcmp(str, vendstr))
  {
    newcontext = 1;
    vendstr = strdup(str);
  }
#ifndef WIN_DIB_OPTION
  str = (char *)glGetString(GL_SHADING_LANGUAGE_VERSION);
  if (str && strcmp(str, glslverstr))
  {
    newcontext = 1;
    glslverstr = strdup(str);
  }
#endif
  str = (char *)glGetString(GL_RENDERER);
  if (str && strcmp(str, rendstr))
  {
    newcontext = 1;
    rendstr = strdup(str);
  }
  str = (char *) glGetString(GL_EXTENSIONS);
  if (str && strcmp(str, extstr))
  {
    newcontext = 1;
    extstr = strdup(str);
    fboSupport = strstr(extstr, "GL_ARB_framebuffer_object");
    formatted_extstr = str_replace (extstr, substr, replacement, &extcount);
  }
#endif

  // Reset all context dependent stuff when new context detected
  if (!newcontext)
    return newcontext;

  printf("\n");
  printf("OpenGL Driver Info\n");
  printf("====================\n");
  printf("GL_VENDOR = %s\n", vendstr);
  printf("GL_VERSION = %s\n", verstr);
  printf("GL_RENDERER = %s\n", rendstr);
#ifndef WIN_DIB_OPTION
  printf("GL_SHADING_LANGUAGE_VERSION = %s\n", glslverstr);
#endif
  printf("\n");
  printf("OpenGL Extensions\n");
  printf("====================\n");
  printf("Number of OpenGL Extensions = %d\n", extcount);
  printf("%s\n", formatted_extstr);
  printf("\n");
  printf("OpenGL Bits\n");
  printf("====================\n");
  glGetIntegerv(GL_RED_BITS, &rBits);
  glGetIntegerv(GL_GREEN_BITS, &gBits);
  glGetIntegerv(GL_BLUE_BITS, &bBits);
  glGetIntegerv(GL_ALPHA_BITS, &aBits);
  printf("GL_RGBA_BITS (R,G,B,A) = (%d, %d, %d, %d)\n", rBits, gBits, bBits, aBits);

  glGetIntegerv(GL_DEPTH_BITS, &DepthBits);
  printf("GL_DEPTH_BITS = %d\n", DepthBits);

  // Change the default znear for skimpy depth buffers like the Mesa Default.
  if (projection_znear > 100.0)
    { /* znear was probably already adjusted by -cg camera globe.  Leave it alone. */ }
  else if (DepthBits < 24) 
    projection_znear = 100.0; 
  else if (strstr(vendstr, "ATI") || strstr(verstr, "ATI"))
  {
    // The Mac ATI Rage 128 has bleeding edgelines even at 24bit depth.
    projection_znear = 100.0; 
  }

  glGetIntegerv(GL_STENCIL_BITS, &StencilBits);
  printf("GL_STENCIL_BITS = %d\n", StencilBits);

  // Reset to default buffer_swap_mode in case this is done on resize.
#ifdef MESA
  buffer_swap_mode = SWAP_TYPE_NODAMAGE;
#else
#  ifdef MACOS_X
  buffer_swap_mode = SWAP_TYPE_APPLE;
#  else
  buffer_swap_mode = SWAP_TYPE_UNDEFINED;
#  endif
#endif

  /*
    NOTE:  DRI libGL used in in indirect mode sends GLX protocol messages
    to the X server which are executed by the GLcore renderer.  Stand-alone
    Mesa's non-DRI libGL doesn't know anything about GLX.  It effectively
    translates OpenGL calls into Xlib calls.

    You can force indirect rendering mode by setting the
    LIBGL_ALWAYS_INDIRECT environment variable.  

    Hmmm, so maybe I should NOT make assumptions about SWAP for indirect.  
  */

  // This will leave "Mesa Offscreen" with SWAP_TYPE_UNDEFINED (OK)
  if (!strcmp(rendstr, "Mesa X11")) // || !strcmp(rendstr,"Mesa GLX Indirect"))
    {
      // MESA 4.0 no longer seems to #define MESA so check GL_RENDERER instead.
      buffer_swap_mode = SWAP_TYPE_NODAMAGE;
    }
  if (strstr(rendstr, "Mesa"))
    {
      if (strstr(verstr, "Mesa 3.1") ||
          strstr(verstr, "Mesa 3.2") ||
          strstr(verstr, "Mesa 3.3") ||
          strstr(verstr, "Mesa 3.4"))
        {
          MESA_3_COLOR_FIX = 1;
          printf("MESA_COLOR_FIX = on\n");
        }
    }

  if (strstr(extstr, "GL_WIN_swap_hint"))
  {
    if ((!strcmp(verstr, "1.1.0")) &&
	(!strcmp(vendstr, "Microsoft Corporation")) &&
	(!strcmp(rendstr, "GDI Generic"))
	)
    {
      // Assume Microsoft software opengl which blits rather than page flips.
      buffer_swap_mode = SWAP_TYPE_COPY;
    }
  }

#if defined(WINDOWS)
  test_wgl_arb_buffer_region(extstr);
#endif

  test_ktx_buffer_region(extstr);

#ifndef TNT2_TEST
  // Stencil works with Microsoft Generic and Mesa, but not with the TNT2.
  if (strstr(vendstr, "NVIDIA"))
  {
    printf("Stencil buffer disabled for XOR with NVIDIA driver.\n");
    use_stencil_for_XOR = 0;

    if (buffer_swap_mode == SWAP_TYPE_KTX)
    {
      NVIDIA_XOR_HACK = 2;
      if (SOLID_EDIT_MODE == 0)
	SOLID_EDIT_MODE = NVIDIA_XOR_HACK;
    }
  }
#endif

  if ((!strcmp(verstr, "1.1 APPLE-1.1")) &&
      (!strcmp(vendstr, "Apple")) &&
      (!strcmp(rendstr, "Generic"))
      )
  {
    printf("Stencil buffer disabled for XOR with Apple driver.\n");
    // Generic Apple driver has problem with XOR similar to NVIDIA.
    use_stencil_for_XOR = 0;

    // The Mac ATI Rage has bleeding edgelines even at 24bit depth.
    // But it may use the Apple software driver when has <= 8MB VRAM.
    projection_znear = 100.0; 
  }

  if (!strnicmp(rendstr, "RADEON 7000", 10))
  {
    printf("RADEON 7000 found.  Avoiding drawing in GL_FRONT buffer.\n");
    AVOID_FRONT_BUFFER_TEXT = 1;
    if (zDetailLevel < TYPE_PART) // Draw parts in back buffer,
      zDetailLevel = TYPE_PART;   // Copy to front after each part. (slow)
  }

#ifdef SIMULATE_MESA      
  buffer_swap_mode = SWAP_TYPE_NODAMAGE;
  if (NVIDIA_XOR_HACK)
  {
    if (SOLID_EDIT_MODE == NVIDIA_XOR_HACK)
      SOLID_EDIT_MODE = 0;
    printf("Editing mode =  %d{%d}\n", editing, SOLID_EDIT_MODE);
    NVIDIA_XOR_HACK = 0;
  }
#endif

#ifdef GENERIC_MS_TEST
  buffer_swap_mode = SWAP_TYPE_COPY;
#endif

#ifdef UNDEFINED_SWAP_TEST
  buffer_swap_mode = SWAP_TYPE_UNDEFINED;
#endif

#ifdef USE_DOUBLE_BUFFER
  // NOTE: Mesa segfaults if I do this BEFORE CreateWindow/EnterGameMode
  //glDrawBuffer(GL_FRONT_AND_BACK); // Hmm, why did I want FRONT_AND_BACK? 
#ifdef TILE_RENDER_OPTION
  if (tiledRendering == 0)
#endif
    glDrawBuffer(GL_FRONT);  // Effectively disable double buffer.
#endif

  printf("\n");
  printf("OpenGL Buffer Status\n");
  printf("====================\n");
#ifndef WIN_DIB_OPTION
  if (fboSupport)
  {
      printf("OpenGL Framebuffer Object supported.\n");
      glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS_EXT, &ColorAttachments);
      printf("OpenGL Max Color Attachments = %d\n", ColorAttachments);
  } else {
      printf("OpenGL framebuffer object NOT supported.\n");
  }
#endif
  printf("Buffer Swap Mode = %d\n", buffer_swap_mode);

  free (formatted_extstr);

  printf("\n");
  printf("LDGLite Processing\n");
  printf("====================\n");
  return newcontext;
}

#ifdef WINDOWS
#    include <shellapi.h> // Just in case
/***************************************************************/
HWND GetHwndFocus()
{ 
  DWORD foregroundThreadID; // foreground window thread
  DWORD ourThreadID; // our active thread 
  HWND hwndFocus; 

  // Check to see if we are the foreground thread 
  foregroundThreadID = GetWindowThreadProcessId(GetForegroundWindow(), NULL); 
  ourThreadID = GetCurrentThreadId(); 

  // If not, attach our thread's 'input' to the foreground thread's 
  if (foregroundThreadID != ourThreadID) 
    AttachThreadInput(foregroundThreadID, ourThreadID, TRUE); 

  // Who has focus? 
  hwndFocus = GetFocus(); 

  // If we attached our thread, detach it now 
  if (foregroundThreadID != ourThreadID) 
    AttachThreadInput(foregroundThreadID, ourThreadID, FALSE); 

  return hwndFocus;
}
  
/***************************************************************/
void SendKeyToCurrentApp( int key ) 
{ 
  HWND hwndFocus; 

  // Who has focus? 
  hwndFocus = GetHwndFocus(); 
  
  // Send desired keystroke to whoever has focus 
  if (key) 
  { 
    PostMessage(hwndFocus, WM_KEYDOWN, VK_PRIOR, 0x8001); 
    PostMessage(hwndFocus, WM_KEYUP, VK_PRIOR, 0xc001); 
  } 
  else 
  { 
    PostMessage(hwndFocus, WM_KEYDOWN, VK_NEXT, 0x8001); 
    PostMessage(hwndFocus, WM_KEYUP, VK_NEXT, 0xc001); 
  } 
} 

WNDPROC  wpOrigMsgProc;

/***************************************************************/
// Intercept LDList and Windows Explorer style Drag and Drop events.
/***************************************************************/
LRESULT APIENTRY 
MsgSubClassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  COPYDATASTRUCT *cds;
  int iRet;

  // Intercept LDList style WM_COPYDATA messages 
  // and convert string to WM_KEYDOWN, WM_KEYUPs.
  if (uMsg == WM_COPYDATA)
  {
    struct {
      int X, Y;
      char s[256];
    } *CopyRecord;

    char *foo;

    cds = (COPYDATASTRUCT *) lParam;
    // printf("WM_COPYDATA %d, %d, %p\n",cds->cbData, cds->dwData, cds->lpData);

    // Look for first msg from LDList to tell it we accept the drag in our window.
    if ((cds) && (cds->dwData == 0) && (cds->cbData == 0) && (cds->lpData == NULL))
    {
      // iRet = CallWindowProc(wpOrigMsgProc, hwnd, uMsg, wParam, lParam);
      printf("Accepting DragNdrop from LDList\n");
      return 1; // Accept the drag.
    }
    // 2nd msg from LDList contains screen(X,Y) drop point and the part text string.
    else if ((cds) && (cds->dwData == 1) && 
	(cds->cbData == sizeof(*CopyRecord)) && (cds->lpData))
    {
      CopyRecord = cds->lpData;
      // printf("Copyrecord = %d, %d, %s\n", CopyRecord->X, CopyRecord->Y, CopyRecord->s);
      pastelist = strdup(CopyRecord->s);

      // No need to forward or accept this message.  LDList ignores any return code.
      // I could possibly convert X,Y to window coords and insert there, but not now.
      //RecHnd = WindowFromPoint(p);   
      //ScreenToClient(p);
    }
    else // Who knows what this is.  Pass it along to glut.
    {
      iRet = CallWindowProc(wpOrigMsgProc, hwnd, uMsg, wParam, lParam);
      // printf("CallWindowProc() = %d\n", iRet);
      return iRet; 
    }
  }
  // Intercept Windows Explorer style Drag and Drop messages.
  else if (uMsg == WM_DROPFILES)
  {
    UINT  uNumFiles;
    TCHAR szNextFile [MAX_PATH];
    HDROP hdrop = (HDROP) wParam;
    UINT uFile;
    char *s;
    char *p;
    int n = 0;

    printf("WM_DROPFILES\n");

    // Get the # of files being dropped.
    uNumFiles = DragQueryFile ( hdrop, -1, NULL, 0 );

    for ( uFile = 0; uFile < uNumFiles; uFile++ )
    {
      // Get the next filename from the HDROP info.
      if ( DragQueryFile ( hdrop, uFile, szNextFile, MAX_PATH ) > 0 )
      {
	printf("WM_DROPFILE <%s>\n", szNextFile);
	n += strlen(szNextFile) + 1;
      }
    }

    pastelist = (char *) calloc(sizeof (char), n);
    for ( uFile = 0; uFile < uNumFiles; uFile++ )
    {
      if (uFile > 0)
	strcat(pastelist, "\n");
      if ( DragQueryFile ( hdrop, uFile, szNextFile, MAX_PATH ) > 0 )
      {
	s = strdup (szNextFile);
	platform_fixcase(s);
	if (p = strstr(s, partspath))
	  strcpy(szNextFile, p + strlen(partspath) + 1);
	else if (p = strstr(s, modelspath))
	  strcpy(szNextFile, p + strlen(modelspath) + 1);
	else if (p = strstr(s, primitivepath))
	  strcpy(szNextFile, p + strlen(primitivepath) + 1);
	else if (p = strstr(s, datfilepath))
	  strcpy(szNextFile, p + strlen(datfilepath) + 1);
	printf("WM_dropfile <%s>\n", szNextFile);
	strcat(pastelist, szNextFile);
	free(s);
      }
    }

    // Free up memory.
    DragFinish ( hdrop );

    // Post a message to GLUT to let it know about the Drop Event.

    //GetKeyState(VK_SHIFT) & 0x80000000;
    //GetKeyState(VK_CONTROL) & 0x80000000;
  }
  else
    return CallWindowProc(wpOrigMsgProc, hwnd, uMsg, wParam, lParam);

  // Post a ctrl-V to glut so it knows we have a paste/drop event.
#if 1
  //iRet = PostMessage(hwnd, WM_KEYDOWN, VK_CONTROL, 0x8001);
  iRet = PostMessage(hwnd, WM_KEYDOWN, 'V', 0x8001);
  //iRet = PostMessage(hwnd, WM_KEYUP, 'V', 0xc001);
  //iRet = PostMessage(hwnd, WM_KEYUP, VK_CONTROL, 0xc001);
#else
  iRet = PostMessage(hwnd, WM_KEYDOWN, VK_ESCAPE, 0x8001);
  iRet = PostMessage(hwnd, WM_KEYUP, VK_ESCAPE, 0xc001);
#endif

  // Call the original glut window proc.  
  // It should just ignore the msg types we intercept here anyhow.
  return CallWindowProc(wpOrigMsgProc, hwnd, uMsg, wParam, lParam);
}

#endif

/***************************************************************/
void capturePasteEvents(void)
{
#ifdef WINDOWS

  HWND hwnd; 

  // Who has focus? 
  // hwnd = GetHwndFocus(); 

  hwnd = FindWindow("GLUT", title);
  printf("FindWindow(%d)\n", hwnd);
  if (hwnd == NULL)
    hwnd = FindWindow("GLUT", NULL);
  printf("FindWindow(%d)\n", hwnd);
  if (hwnd == NULL)
    return;

  DragAcceptFiles(hwnd, TRUE);

// old code not x86_64 compatable
//#if 0
//  wpOrigMsgProc =
//    (WNDPROC) SetWindowLong(NULL, GWL_WNDPROC, (long) MsgSubClassProc);
//#else
//  wpOrigMsgProc =
//    (WNDPROC) SetWindowLong(hwnd, GWL_WNDPROC, (long) MsgSubClassProc);
//#endif

  // x86_64 compatable
  wpOrigMsgProc =
    (WNDPROC) SetWindowLongPtr(hwnd, GWLP_WNDPROC, (long) MsgSubClassProc);

#endif
}

/***************************************************************/
int 
main(int argc, char **argv)
{
  char filename[256];
  FILE *fp;
  int  fd;
  int  fd_new_stdout;
  int exitcode;
  char *str, *verstr, *extstr, *vendstr, *rendstr;
  unsigned int displaymode;
  int i;
  int needargs = 0;
  UNUSED(str);
  UNUSED(verstr);
  UNUSED(extstr);
  UNUSED(vendstr);
  UNUSED(rendstr);
  UNUSED(i);

  printf("\n");
  printf("LDGLite %s\n", ldgliteVersion);
  printf("====================\n");

  printf("Arguments = ");
  for (i = 0; i < argc; i++)
    printf("%s ", argv[i]);
  printf("\n");

  platform_startup(&argc, &argv);
	
  // glutInit moved first, so that GL can have as many args as it can recognize
  // and because glutInit is what actually fills argv on the Mac.
#if defined(MAC)
  glutInit(&argc, argv);
#endif

  InitInstance();
  platform_setpath();

#if !defined(MAC)
#  ifdef MACOS_X
  // I hope OSX glutinit() will ADD the filename of a dropped file to argv.
  // (I know it removes some things.)
  // So I call glutInit ahead of ParseParams.
  /* put paths of all files dropped onto the app into argv */
  // Probably should look for one of those -psn* args before doing this.
  // (glutInit() removes -psn* args, but does not insert the filenames)
  for (i = 1; i < argc; i++)
    if (!strncmp(argv[i], "-psn_", 5))
    {
      needargs = 1;
      break;
    }
#    ifdef USING_CARBON
#include "getargv.h"
  // This replaces all args with stuff from the finder.
  // Assume there will be no other args if if comes from the finder.
  // That may not be the case if I figure out how to get ldglite.command
  // script to call the ldglite executable in the bundle.
  // Maybe I should call the bundled executable l3glite.
  if (needargs)
  {
    argc = FTMac_GetArgv(&argv);
  }
#  elif defined(USING_COCOA)
// TODO - define Cocoa version of drag and drop
// For LPub3D this feature is not needed.
// However, just to maintain backward compatability
// I'll take a look at this later.
#  endif

  if (OffScreenRendering == 0)
  {
    //The GLUT framework will chdir to the Resources folder of your 
    // application bundle on glutInit!  Ouch!
    char cwdpath[512];
    getcwd(cwdpath, 512);
    glutInit(&argc, argv);
    chdir(cwdpath); // problem with chdir to dir with spaces in win32.
  }
#  endif
#endif

  //PrintParams(&argc, argv);

  ParseParams(&argc, argv);

#ifdef MACOS_X
  // Go work in HOME dir for icon clickers.
  platform_sethome();
#endif

  // Setup things that need to know the directory containing the model.
  platform_setdir();

#ifdef MACOS_X
  // Finder seems to run apps from root, so chdir to dirfilepath or HOME.
  if (strcmp(datfilename,  " ")) {
    if (needargs) {
      if (strlen(dirfilepath) > 2)
	chdir(dirfilepath);
      else if (platform_getenv("HOME"))
	chdir(platform_getenv("HOME"));
    }
  }
#endif

#if !defined(MAC)
#  ifndef MACOS_X
  // Get params first so we can skip this if rendering in OSMesa
  if (OffScreenRendering == 0)
  {
    glutInit(&argc, argv);
  }
#  endif
#endif

  Width = ldraw_commandline_opts.V_x;
  Height = ldraw_commandline_opts.V_y;

  if ((Width <= 0) || (Height <= 0))
  {
    // OSMesa does not seem to like glutGet(GLUT_SCREEN_WIDTH)
    if (OffScreenRendering == 0) 
    {
      if ((Width < 0) && (Height < 0))
      {
	//glutFullScreen();  // This shows no window decorations.
	Width = ldraw_commandline_opts.V_x = glutGet(GLUT_SCREEN_WIDTH);
	Height = ldraw_commandline_opts.V_y = glutGet(GLUT_SCREEN_HEIGHT);
      }   
      else
      {
	// get the screen size and subtract a fudge factor for window borders
	Width = ldraw_commandline_opts.V_x = glutGet(GLUT_SCREEN_WIDTH) - 8;
	Height = ldraw_commandline_opts.V_y = glutGet(GLUT_SCREEN_HEIGHT) - 32;
#ifdef MACOS_X
	{
	  int w,h;
	  void GetAvailablePos(int *w, int *h);

	  // Fix this to return top and left as well.
	  // Then set XwinPos, YwinPos to avoid a side mounted dock.
	  // By the way, glut docs say X,YWinpos should default to -1,-1.
	  // Which lets the window mgr decide where to place it.  
	  // Maybe I should try that first?
          //XwinPos = -1; YwinPos = -1;  // Yuck, makes OSX place it weird.
	  GetAvailablePos(&w, &h);
	  Width = ldraw_commandline_opts.V_x = w;
	  Height = ldraw_commandline_opts.V_y = h;
	  printf( "Screen resolution: %d, %d\n", w, h );
	}
#endif
#if defined(MAC)
	// Set default window small for MACs to get cmdline args.
	Width = 640;
	Height = 480;
#endif
      }
    }
#ifdef OSMESA_OPTION
#if OSMESA_MAJOR_VERSION * 100 + OSMESA_MINOR_VERSION >= 400
    else // if (OSMESA) // We could check rendstr, but we fetch that later...
    {
      // NOTE: It would be NICE if Mesa Glut called these in 
      // glutGet(GLUT_SCREEN_*) functions instead of blowing the stack.
      OSMesaGetIntegerv(OSMESA_MAX_WIDTH, &Width);
      OSMesaGetIntegerv(OSMESA_MAX_HEIGHT, &Height);
    }
#endif
#endif

    if ((Width <= 0) || (Height <= 0))
    {
      Width = ldraw_commandline_opts.V_x = 1280;
      Height = ldraw_commandline_opts.V_y = 1024;
    }
  }
  else if (OffScreenRendering == 0) 
  {
    // Using cmdline size, move away from the top left corner if possible.
#if 0
    if ((Width + 32) < glutGet(GLUT_SCREEN_WIDTH))
      XwinPos = 32;
    if ((Height + 32) < glutGet(GLUT_SCREEN_HEIGHT))
      YwinPos = 32;
#endif
  }

#if 0
  sprintf(output_file_name, "%s.log", datfilename);
  output_file = fopen(output_file_name,"w+");
#endif

  // Get the path to the program to find the plugins directory
  if (GetExecName(argv[0], progname, 256) == 0)
  {
    strcpy(progpath, dirname(progname));
  }
  else
  {
    strcpy(progpath, "");
  }

  strcpy(progname, basename(argv[0]));
  // SetTitle(0);

#ifdef USE_L3_PARSER
  // If the parser type is not specified on the commandline 
  // then set it here based on the program name.  
  if (parsername == UNKNOWN_PARSER)
  {
    parsername = LDLITE_PARSER; // default to ldlite parser.
    if ((stricmp("l3glite", progname) == 0) ||
	(stricmp("l3glite.exe", progname) == 0))
    {
      parsername = L3_PARSER;

      // The L3 parser fixes bowtie quads once during parsing.
      // and (I hope) turns concave quads into 2 triangles?
      // So I don't need my slow 3 triangle hack.
      use_quads = 1;
    }
  }
#endif


  //DEBUG stuff
  if (OffScreenRendering)
    printf("Rendering OffScreen\n");
  else
    printf("Rendering OnScreen\n");


  if (OffScreenRendering == 1)
  {
    OffScreenRender();
    exit(0);
  }

  SetTitle(0);

  displaymode = GLUT_RGB | GLUT_DEPTH;
#ifdef USE_ALPHA_BUFFER
  // This is needed for the current transparent png code to work.
  // Its somewhat wasteful though because it defaults to 8 bits of
  // alpha on many opengl drivers.  I really only need one bit of
  // say a stencil buffer to make this work.  Or I can try to use
  // the depth buffer to determine if a pixel is still background.
  // This would require no extra bitplanes, but might be slower.
  // There may be problems with any of these approaches in LEDIT
  // emulation mode because I use these buffers for many purposes.
  displaymode |= GLUT_ALPHA;
#endif
#ifdef USE_DOUBLE_BUFFER
  displaymode |= GLUT_DOUBLE;
#endif
#ifdef USE_OPENGL_STENCIL
  displaymode |= GLUT_STENCIL;
#endif
#ifdef TILE_RENDER_OPTION
  if (tiledRendering)
    displaymode = GLUT_RGB | GLUT_DEPTH;
#endif

  glutInitDisplayMode(displaymode);
  if (! glutGet(GLUT_DISPLAY_MODE_POSSIBLE))
    displaymode &= ~GLUT_ALPHA;
  glutInitDisplayMode(displaymode);
  if (! glutGet(GLUT_DISPLAY_MODE_POSSIBLE))
  {
    printf("Stencil buffer disabled for XOR with current openglDriver.\n");
    use_stencil_for_XOR = 0;
    displaymode &= ~GLUT_STENCIL;
  }
  glutInitDisplayMode(displaymode);
  if (! glutGet(GLUT_DISPLAY_MODE_POSSIBLE))
  {
    //glutInitDisplayMode(GLUT_SINGLE);
    printf("Cannot get double buffer from GLUT with current opengl Driver.\n");
    return 0;
  }

#ifndef AGL
#if (GLUT_XLIB_IMPLEMENTATION >= 13)
  if (ldraw_commandline_opts.V_x < -1)
  {
    //glutGameModeString("800x600:16@60") ;
    glutGameModeString("640x480@60") ;
    // if the width is different to -1
    if (glutGameModeGet(GLUT_GAME_MODE_WIDTH) != -1)
    {
      glutEnterGameMode();		// enter full screen mode
    }
  }
  else
#endif
#endif
  {
    printf("glutInitWindowSize(%d, %d)\n",Width, Height);
    glutInitWindowSize(Width, Height);
    printf("glutInitPosition(%d, %d)\n",XwinPos, YwinPos);
    glutInitWindowPosition(XwinPos, YwinPos);

    main_window = glutCreateWindow(title);

    if (ldraw_commandline_opts.V_x < 0)
    {
      ldraw_commandline_opts.V_x = Width;
      printf("glutFullScreen(%d, %d)\n", ldraw_commandline_opts.V_x, Width);
      glutFullScreen();  // This shows no window decorations.
    }   
  }

  getDisplayProperties();

  // Should be registerGlutCallbacks() but mainmenu not created yet.
  setGlutCallbacks(); 
  initializeMenus();

#ifdef USE_GLFONT
  if (fontname)
  {
    int n = 0;
    char *TexBytes;

    /* just to be safe... */
    //glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Unpacking alignement not used.
    glPixelStorei(GL_PACK_ALIGNMENT, 2); // Padded to even numbered bytes.
    glPixelStorei(GL_PACK_ALIGNMENT, 1); // Use if data in glfont is unpadded.

    glFontCreate (&Font, fontname, FontTex); //Creates a glFont
    // void glFontDestroy (Font); //Deletes a glFont
    
    fontheight = Font.Char[0].ty2 * Font.TexHeight;
    fontwidth = fontheight/Font.Char[0].dy;

    printf("Loaded glfont %s (%g, %g) (%d, %d, %d, %d)\n", fontname,
	   fontwidth, fontheight,
	   Font.TexWidth, Font.TexHeight, Font.IntStart, Font.IntEnd);
#if 1
    fp = fopen(fontname, "rb");
    n = sizeof(GLFONT);
    n += sizeof(GLFONTCHAR) * (Font.IntEnd - Font.IntStart + 1);
    fseek (fp , n, SEEK_SET);
    n = Font.TexWidth * Font.TexHeight * 2;
  	
    TexBytes = (char *)malloc(n);
    fread(TexBytes, sizeof(char), n, fp);
    fclose(fp);

    if ((fp = fopen ("glfariel.c", "w")) != NULL)
    {
      fprintf(fp, "\nGLFONT arielFont = {\n  {%d, %d, %d, %d, %d},\n", Font.Tex,
	     Font.TexWidth, Font.TexHeight, Font.IntStart, Font.IntEnd);
      fprintf(fp, "\nGLFONTCHAR arielChars = {\n");
      for (n = 0; n < 1+Font.IntEnd-Font.IntStart; n++)
      {
	fprintf(fp,"  {%g, %g, %g, %g, %g, %g},\n", 
		Font.Char[n].dx, Font.Char[n].dy,
		Font.Char[n].tx1, Font.Char[n].ty1, 
		Font.Char[n].tx2, Font.Char[n].ty2);
      }
      fprintf(fp, "  }\n");
      fprintf(fp, "\nchar arielTex[] = {\n  ");
      for (n = 0; n < (Font.TexWidth * Font.TexHeight * 2); n+=2)
      {
	fprintf(fp, "0x%02X, 0x%02X, ", TexBytes[n], TexBytes[n+1]);
	if (n % 8 == 0)
	  fprintf(fp, "\n ");
      }
	      
      fprintf(fp, "  }\n");
      fclose (fp);
    }
#endif

    // dy/dx = ratio of height to width of each char
    // dy is always the same number (the ratio of height to the avg char)
    // dx varies depending on the width of the char.  It averages 1.
    printf("(%g, %g) (%g, %g, %g, %g)\n", Font.Char[n].dx, Font.Char[n].dy,
	   Font.Char[n].tx1, Font.Char[n].ty1, 
	   Font.Char[n].tx2, Font.Char[n].ty2);

  }
#endif

  initCamera();
  init();

  // Check if this is l3gledit or ldgledit or -le.  If so then editing = 1.
  strcpy(filename, progname);
  _strlwr(filename);
  if ((editing == 1) || (strstr(filename, "ledit")))
  {
    editing = 1;
    //SOLID_EDIT_MODE = 0;

    // if (ldraw_commandline_opts.debug_level == 1)
    printf("Editing mode =  %d{%d}\n", editing, SOLID_EDIT_MODE);
    
    // Switch to continuous mode.
    editingprevmode = ldraw_commandline_opts.M;
    ldraw_commandline_opts.M = 'C';
    //ldraw_commandline_opts.poll = 0; // Disable polling 
    
    curstep = 0; // Reset to first step
    dirtyWindow = 1;

    curpiece = 0;
    movingpiece = -1;

    parsername = L3_PARSER;
    use_quads = 1;
    list_made = 0; // Gotta reparse the file.
  }

#ifdef AGL
  // Gotta stop writing to the console in DOS mode
  // NOTE: I could do this for everyone with a commandline opt. (quiet mode)
  // Make the message filename part of the option. 
  // If the filename is empty, then be completely silent.
  // Consider ldraw_commandline_opts.log_output (cmdline -l) (goes to ldlite.log)
  ldraw_commandline_opts.log_output = 1;
#endif

  if (ldraw_commandline_opts.log_output)
  {
    printf("Redirecting stdout to %s\n", "ldglite.log");
    if ((fp = fopen ("ldglite.log", "w")) != NULL)
    {
      fd = fileno (fp);
    }
    close(2);            // close stderr
    dup(1);               // redirect stderr to stdout?
    if (close (1) != -1) // close stdout
    {
      if (fp != NULL)
      {
	fd_new_stdout = dup (fd); // redirect printfs to stdout to the file
      }
    }
  }

  capturePasteEvents();
   
  glutMainLoop();

  exitcode = 0; // no error
  
  return exitcode;
}

