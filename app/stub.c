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
#include "ldliteVR.h"
#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include "string.h"

#ifdef USE_OPENGL
//**********************************************************************
#include "glwinkit.h"
#include "platform.h"

extern int MESA_3_COLOR_FIX;

extern int curstep;
extern int cropping;
extern int panning;
extern int dirtyWindow;
extern int use_quads;
extern float lineWidth;

extern int qualityLines;
extern int DepthOnly;
extern int preprintstep;
extern int dimLevel;
extern float dimAmount;

// Draw Faded Parts for LPub3D -- Trans with no back edges.
extern int TransFadeEffect;
extern int TransFadePercent;
extern int FadeColors[256];
extern int nFadeColors;
// Draw Part Silhouettes for LPub3D.
extern int SilhouetteColors[256];
extern int nSilhouetteColors;
extern int SilhouetteEdge;

// Ambient and diffusion properties for front and back faces.
extern GLfloat full_mat[];
extern GLfloat half_mat[];
extern GLfloat no_mat[];
extern GLfloat no_shininess[];
extern GLfloat lo_shininess[];
extern GLfloat mid_shininess[];
extern GLfloat hi_shininess[];
GLfloat cur_mat[] = { 0.0, 0.0, 0.0, 1.0 };;

GLdouble model_mat[4*4];
GLdouble proj_mat[4*4];
GLint view_mat[4];

float surf_norm[3];
float norm1[3];
float norm2[3];
float norm3[3];
float norm4[3];
float norm_dot;
int norm_turn = 0;

#define STIP_OFFSET 100
int current_stipple = 0;
extern GLubyte stips[8][4*32];
      
GLubyte halftone[] = {
    0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
    0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
    0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
    0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
    0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
    0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
    0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
    0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
    0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
    0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
    0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
    0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
    0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
    0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
    0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
    0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55};

GLubyte halftone25[] = {
    0xAA, 0xAA, 0xAA, 0xAA, 0x00, 0x00, 0x00, 0x00,
    0xAA, 0xAA, 0xAA, 0xAA, 0x00, 0x00, 0x00, 0x00,
    0xAA, 0xAA, 0xAA, 0xAA, 0x00, 0x00, 0x00, 0x00,
    0xAA, 0xAA, 0xAA, 0xAA, 0x00, 0x00, 0x00, 0x00,
    0xAA, 0xAA, 0xAA, 0xAA, 0x00, 0x00, 0x00, 0x00,
    0xAA, 0xAA, 0xAA, 0xAA, 0x00, 0x00, 0x00, 0x00,
    0xAA, 0xAA, 0xAA, 0xAA, 0x00, 0x00, 0x00, 0x00,
    0xAA, 0xAA, 0xAA, 0xAA, 0x00, 0x00, 0x00, 0x00,
    0xAA, 0xAA, 0xAA, 0xAA, 0x00, 0x00, 0x00, 0x00,
    0xAA, 0xAA, 0xAA, 0xAA, 0x00, 0x00, 0x00, 0x00,
    0xAA, 0xAA, 0xAA, 0xAA, 0x00, 0x00, 0x00, 0x00,
    0xAA, 0xAA, 0xAA, 0xAA, 0x00, 0x00, 0x00, 0x00,
    0xAA, 0xAA, 0xAA, 0xAA, 0x00, 0x00, 0x00, 0x00,
    0xAA, 0xAA, 0xAA, 0xAA, 0x00, 0x00, 0x00, 0x00,
    0xAA, 0xAA, 0xAA, 0xAA, 0x00, 0x00, 0x00, 0x00,
    0xAA, 0xAA, 0xAA, 0xAA, 0x00, 0x00, 0x00, 0x00};

GLubyte halftone75[] = {
    0xAA, 0xAA, 0xAA, 0xAA, 0xFF, 0xFF, 0xFF, 0xFF,
    0xAA, 0xAA, 0xAA, 0xAA, 0xFF, 0xFF, 0xFF, 0xFF,
    0xAA, 0xAA, 0xAA, 0xAA, 0xFF, 0xFF, 0xFF, 0xFF,
    0xAA, 0xAA, 0xAA, 0xAA, 0xFF, 0xFF, 0xFF, 0xFF,
    0xAA, 0xAA, 0xAA, 0xAA, 0xFF, 0xFF, 0xFF, 0xFF,
    0xAA, 0xAA, 0xAA, 0xAA, 0xFF, 0xFF, 0xFF, 0xFF,
    0xAA, 0xAA, 0xAA, 0xAA, 0xFF, 0xFF, 0xFF, 0xFF,
    0xAA, 0xAA, 0xAA, 0xAA, 0xFF, 0xFF, 0xFF, 0xFF,
    0xAA, 0xAA, 0xAA, 0xAA, 0xFF, 0xFF, 0xFF, 0xFF,
    0xAA, 0xAA, 0xAA, 0xAA, 0xFF, 0xFF, 0xFF, 0xFF,
    0xAA, 0xAA, 0xAA, 0xAA, 0xFF, 0xFF, 0xFF, 0xFF,
    0xAA, 0xAA, 0xAA, 0xAA, 0xFF, 0xFF, 0xFF, 0xFF,
    0xAA, 0xAA, 0xAA, 0xAA, 0xFF, 0xFF, 0xFF, 0xFF,
    0xAA, 0xAA, 0xAA, 0xAA, 0xFF, 0xFF, 0xFF, 0xFF,
    0xAA, 0xAA, 0xAA, 0xAA, 0xFF, 0xFF, 0xFF, 0xFF,
    0xAA, 0xAA, 0xAA, 0xAA, 0xFF, 0xFF, 0xFF, 0xFF};

int glCurColorIndex = -1;
int glCurLighting = 1;
int hardcolor = 0;

// move lines closer before drawing (only if glPolygonOffset is broken)
float z_line_offset = 0.0; 
//float z_line_offset = 0.95; 

#define SamePoint(p1,p2) ((p1->x == p2->x) && (p1->y == p2->y) && (p1->z == p2->z))

/************************************************************************/
// get the opengl transforms
// so we can perform world2screen xforms to test optional lines.
void get_opengl_transforms(void)
{
  glGetDoublev(GL_MODELVIEW_MATRIX, model_mat);
  glGetDoublev(GL_PROJECTION_MATRIX, proj_mat);
  glGetIntegerv(GL_VIEWPORT, view_mat);
}

/************************************************************************/
// Return 1 if the v2 bends left of v1, -1 if right, 0 if straight ahead.
int turn_vector(double vx1, double vy1, double vx2, double vy2)
{
  /* Pos for left bend, 0 = linear */
  double vec_product = (vx1 * vy2) - (vy1 * vx2);

  if (vec_product > 0.0) return(1);
  if (vec_product < 0.0) return(-1);
  return(0);
}

//**********************************************************************
void
normalize(float v[3])
{
  float r;

  r = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
  if (r == 0.0)
    return;  // This is bad.  Zero length vector.

  v[0] /= r;
  v[1] /= r;
  v[2] /= r;
}

//**********************************************************************
void
crossprod(float v1[3], float v2[3], float result[3])
{
  result[0] = v1[1] * v2[2] - v1[2] * v2[1];
  result[1] = v1[2] * v2[0] - v1[0] * v2[2];
  result[2] = v1[0] * v2[1] - v1[1] * v2[0];
}

//**********************************************************************
float
length(float v[3])
{
  float r = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
  return r;
}

//**********************************************************************
void
normcrossprod(float v1[3], float v2[3], float result[3])
{
  result[0] = v1[1] * v2[2] - v1[2] * v2[1];
  result[1] = v1[2] * v2[0] - v1[0] * v2[2];
  result[2] = v1[0] * v2[1] - v1[1] * v2[0];
  
  normalize(result);
}

//**********************************************************************
void
getnormal(vector3d *p1, vector3d *p2, vector3d *p3, float norm[3])
{
  float d1[3], d2[3];

  d1[0] = p2->x - p1->x;
  d1[1] = p2->y - p1->y;
  d1[2] = p2->z - p1->z;
  
  d2[0] = p3->x - p2->x;
  d2[1] = p3->y - p2->y;
  d2[2] = p3->z - p2->z;

  normcrossprod(d1, d2, norm);
}

//**********************************************************************
double dotprod(float a[3], float b[3])
{
   return (a[0] * b[0] + a[1] * b[1] + a[2] * b[2]);
}

//**********************************************************************
// NOTE:  Probably should just use zcs->a and stop using trans color numbers.
// If using FADE meta cmd, then multiply zcs-> by FADE amount first.
//**********************************************************************
void setup_material(int c, ZCOLOR *zcp, ZCOLOR *zcs)
{
//  if (((c > 32) && (c < 64)) || (zcs->a == 0)) // Translucent colors
// Some bonehead at ldraw.org defined chrome colors in the translucent range.
// And I had a bug for color 32 (I am a bonehead too)
  if (((c > 31) && (c < 60)) || (zcs->a < 255)) // Translucent colors
  {
    glEnable(GL_POLYGON_STIPPLE);
    if (TransFadePercent < 1) // No FADE. Just do normal transparency.
      glPolygonStipple(halftone);
    else { // Interpret FADE percentage as a stipple pattern.
      if (TransFadePercent <= 33)
	glPolygonStipple(halftone75);
      else if (TransFadePercent > 61) // Make FADE=75 use halftone25 when
	glPolygonStipple(halftone25); // applied to transparent parts. 
      else
      glPolygonStipple(halftone);
    }
    //glCallList(current_stipple+STIP_OFFSET);
    current_stipple++;
    current_stipple %= 8;
    //glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, hi_shininess);
  }   
  // Gold, Chrome silver, Electrical Contacts
  else if ((c == 334) || (c == 383) || (c == 494))
  {
    //glColorMaterial(GL_FRONT_AND_BACK, GL_SPECULAR);
    cur_mat[0] = zcp->r/255.0;
    cur_mat[1] = zcp->g/255.0;
    cur_mat[2] = zcp->b/255.0;
    //glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, cur_mat);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, cur_mat);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mid_shininess);
    if (/*(c == 383) ||*/ (c == 494))
      glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, hi_shininess);
    //glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, no_mat);
  }
  if (ldraw_commandline_opts.F & TYPE_F_SHADED_MODE) // (zShading)
    if (!glCurLighting) glEnable(GL_LIGHTING);
  glCurLighting = 1;
  if (glCurColorIndex != -2)
  {
    if (glCurColorIndex != c)
    {
      //glColor3f(((float)zcp->r)/255.0, ((float)zcp->g)/255.0, ((float)zcp->b)/255.0);
      glColor3ub(zcp->r, zcp->g, zcp->b);
      if (MESA_3_COLOR_FIX)
	glFlush();
    }
    glCurColorIndex = c;
  }
}

//**********************************************************************
void unset_material(int c, ZCOLOR *zcp, ZCOLOR *zcs)
{
//  if (((c > 32) && (c < 60)) || (zcs->a == 0)) // Translucent colors
// Some bonehead at ldraw.org defined chrome colors in the translucent range.
// And I had a bug for color 32 (I am a bonehead too)
  if (((c > 31) && (c < 64)) || (zcs->a < 255)) // Translucent colors
  {
    glDisable(GL_POLYGON_STIPPLE);
    //glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, no_shininess);
  }   
  // Gold, Chrome silver, Electrical Contacts
  else if ((c == 334) || (c == 383) || (c == 494))
  {
    //glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, full_mat);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, no_mat);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, no_shininess);
    //glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, no_mat);
  }
}

#if 0
//**********************************************************************
// Shortcut for rendering smooth studs??? 
//**********************************************************************
void render_stud(vector3d *vp1, vector3d *vp2, vector3d *vp3, vector3d *vp4, int c)
{
  ZPOINT p1, p2;
  ZCOLOR zc, zs;
  int i;
  int j;
  int wid;

  if(ldraw_commandline_opts.F & TYPE_F_NO_LINES) {
    return;
  };

#ifdef USE_OPENGL
  if ((ldraw_commandline_opts.M == 'P') || preprintstep ||
      ((ldraw_commandline_opts.M == 'S') && qualityLines))
  {
    // Non-continuous output stop after each step.
#ifndef ALWAYS_REDRAW
   if (dirtyWindow == 0)
   {  
    if (stepcount != curstep) // (Or (dirty and stepcount < curstep))
     if ((panning == 0) || (stepcount > curstep))
      return;  // Do NOT render since we have not reached this step yet.
   }
   else
#endif
    if (stepcount > curstep)
      return;  // Do NOT render since we have not reached this step yet.
  }
#endif

  p1.x = (int)(0.5+vp1->x);
  p1.y = (int)(0.5+vp1->y);
  p1.z = (int)(0.5+Z_SCALE_FACTOR * vp1->z - Z_LINE_OFFSET);
  p2.x = (int)(0.5+vp2->x);
  p2.y = (int)(0.5+vp2->y);
  p2.z = (int)(0.5+Z_SCALE_FACTOR * vp2->z - Z_LINE_OFFSET);
  translate_color(c,&zc,&zs);

  if (glCurColorIndex != -2)
  {
    if (glCurColorIndex != c)
    {
      //Glcolor3f(((float)zc.r)/255.0, ((float)zc.g)/255.0, ((float)zc.b)/255.0);
      glColor3ub(zc.r, zc.g, zc.b);
#ifdef MESA_3_FLUSH_FIX
      glFlush();
#endif
    }
    glCurColorIndex = c;
  }
  quadObj = gluNewQuadric (); 
  gluQuadricNormals (quadObj, GLU_SMOOTH); 
  gluQuadricOrientation(quadObj, GLU_OUTSIDE);
  gluQuadricDrawStyle (quadObj, GLU_FILL);
  gluCylinder (quadObj, 0.3, 0.3, 0.6, 12, 2); 
  gluDisk (quadObj, 0.3, 0.3, 0.6, 12, 2);
  //gluQuadricDrawStyle (quadObj, GLU_LINES);
  gluQuadricDrawStyle (quadObj, GLU_SILHOUETTE);
  gluCylinder (quadObj, 0.3, 0.3, 0.6, 12, 2);
#endif

//**********************************************************************
#endif

#if 1
#define Z_POLY_TOLERANCE (0) // replace pixels within tolerance to avoid rounding problems
#define Z_LINE_TOLERANCE 2   // replace pixels within tolerance to avoid rounding problems
#define Z_LINE_OFFSET (2*Z_SCALE_FACTOR)//3      // move lines closer to viewer before drawing to avoid rounding problems
#else
#define Z_POLY_TOLERANCE (0) // replace pixels within tolerance to avoid rounding problems
#define Z_LINE_TOLERANCE 0   // replace pixels within tolerance to avoid rounding problems
#define Z_LINE_OFFSET 0      // move lines closer to viewer before drawing to avoid rounding problems
#endif

ZIMAGE z;
int zShading;
int zWire;
int zDetailLevel;

void platform_zDraw(ZIMAGE *,void *);
#define INLINE_CALC
#ifndef INLINE_CALC
void calc_zplane(ZPLANE *zpl, ZPOINT *p1, ZPOINT *p2, ZPOINT *p3);
void calc_zplane_z(ZPLANE *zpl, ZPOINT *p);
#endif
int init_zimage(ZIMAGE *zp, int rows, int cols);
void zAddTriangle(ZIMAGE *zp, ZPOINT *p1, ZPOINT *p2, ZPOINT *p3, ZCOLOR *zcp, ZCOLOR *zcs);
void zAddLine(ZIMAGE *zp, ZPOINT *pc1, ZPOINT *pc2, ZCOLOR *zpc, ZCOLOR *zps, int replace);
void zDraw(void *zDC);
int zReset(long *, long *);
int zInvalidate(void);
void zFree(void);
void platform_step(int step, int level, int pause, ZIMAGE *zp);
int zcolor_unalias(int index, char *name);
int zcolor_alias(int index, char *name);
void translate_color(int c, ZCOLOR *zcp, ZCOLOR *zcs);


void render_line(vector3d *vp1, vector3d *vp2, int c)
{
	ZPOINT p1, p2;
	ZCOLOR zc, zs;
	int i;
	int j;
	int wid;

	if(ldraw_commandline_opts.F & TYPE_F_NO_LINES) {
		return;
	};

#ifdef USE_OPENGL
  if ((ldraw_commandline_opts.M == 'P') || preprintstep ||
      ((ldraw_commandline_opts.M == 'S') && qualityLines))
  {
    // Non-continuous output stop after each step.
#ifndef ALWAYS_REDRAW
   if (dirtyWindow == 0)
   {  
    if (stepcount != curstep) // (Or (dirty and stepcount < curstep))
     if ((panning == 0) || (stepcount > curstep))
      return;  // Do NOT render since we have not reached this step yet.
   }
   else
#endif
    if (stepcount > curstep)
      return;  // Do NOT render since we have not reached this step yet.
  }
  if (hardcolor && qualityLines)
  {
    // Drawing a printed poly.  Use thin antialiased lines.
    // I imagine this is because only EDGE lines should ever be drawn wide.
    // And EDGE lines should never be hardcolored.
    // NOTE:  Probably should do something about skipping GL_POINTS below.
    glLineWidth( 1.0 );
    // printf("Edging color %d\n", c);
  }
#endif

	p1.x = (int)(0.5+vp1->x);
	p1.y = (int)(0.5+vp1->y);
	p1.z = (int)(0.5+Z_SCALE_FACTOR * vp1->z - Z_LINE_OFFSET);
	p2.x = (int)(0.5+vp2->x);
	p2.y = (int)(0.5+vp2->y);
	p2.z = (int)(0.5+Z_SCALE_FACTOR * vp2->z - Z_LINE_OFFSET);
	translate_color(c,&zc,&zs);
	zAddLine(&z, &p1, &p2, &zc, &zs, 1);
	if(ldraw_commandline_opts.W > 1) {
		// draw additional lines to make it thicker
		wid = (ldraw_commandline_opts.W)/2;
		p1.x -= wid;
		p2.x -= wid;
		for(i= (-1)*wid; i<=wid; i++) {
			p1.y -= wid;
			p2.y -= wid;
			for (j=(-1)*wid; j<=wid; j++) {
				zAddLine(&z, &p1, &p2, &zc, &zs, 1);
				p1.y++;
				p2.y++;
			}
			p1.y -= (wid+1);
			p2.y -= (wid+1);
			p1.x++;
			p2.x++;
		}
	}
	switch (ldraw_commandline_opts.output)
	{
	case 1:
		// LDRAW output
		// TBD
		break;
	case 2:
		// Rayshade output
		// TBD
		break;
	case 3:
		// Unlike L3P recent versions, ldlite's POV output handles
		// Type2 lines, using cylinders.  L3P would have trouble because 
		// nested POV transforms cause the radius of these cylinders to be
		// distorted, giving an uneven appearance.  
		fprintf(output_file,"cylinder { <%f,%f,%f> <%f,%f,%f> 0.5 texture { Color%d }}\n",
			vp1->x, vp1->y, vp1->z, vp2->x, vp2->y, vp2->z, c);
		break;
	}

#ifdef USE_OPENGL
#if 0
  fprintf(output_file,"render_line(%.2f,%.2f,%.2f) (%.2f,%.2f,%.2f) %d = (%d, %d, %d) or (%d, %d, %d)\n",
	  vp1->x, vp1->y, vp1->z, vp2->x, vp2->y, vp2->z, 
	  c, zc.r, zc.g, zc.b, zs.r, zs.g, zs.b);
#endif

  if (cropping)
  {
    // Gotta convert to screen coords first for opengl.
    GLdouble s1x, s1y, s1z;
    GLdouble s2x, s2y, s2z;

    gluProject((GLdouble)vp1->x, (GLdouble)-vp1->y, (GLdouble)-vp1->z,
	     model_mat, proj_mat, view_mat,
	     &s1x, &s1y, &s1z);

    gluProject((GLdouble)vp2->x, (GLdouble)-vp2->y, (GLdouble)-vp2->z,
	     model_mat, proj_mat, view_mat,
	     &s2x, &s2y, &s2z);

    z.extent_x1 = min(s1x,z.extent_x1);
    z.extent_x1 = min(s2x,z.extent_x1);
    z.extent_x2 = max(s1x,z.extent_x2);
    z.extent_x2 = max(s2x,z.extent_x2);
    z.extent_y1 = min(s1y,z.extent_y1);
    z.extent_y1 = min(s2y,z.extent_y1);
    z.extent_y2 = max(s1y,z.extent_y2);
    z.extent_y2 = max(s2y,z.extent_y2);
  }

#if 0
  if ((zc.r+zc.b+zc.g) < 50) zc.r = zc.g = zc.b = 128;
  else zc.r = zc.g = zc.b = 65;
#endif
  //glDepthFunc(GL_ALWAYS);
  if (ldraw_commandline_opts.F & TYPE_F_SHADED_MODE) // (zShading)
    if (glCurLighting) glDisable(GL_LIGHTING);
  glCurLighting = 0;
  if (glCurColorIndex != -2)
  {
    if (glCurColorIndex != c)
    {
      //Glcolor3f(((float)zc.r)/255.0, ((float)zc.g)/255.0, ((float)zc.b)/255.0);
      glColor3ub(zc.r, zc.g, zc.b);
      if (MESA_3_COLOR_FIX)
	glFlush();
    }
    glCurColorIndex = c;
  }
  if (lineWidth > 0.0)
  {
    glBegin(GL_POINTS);
    // LDRAW has the y value reversed, so negate the y.
    glVertex3f(vp1->x, -vp1->y, -vp1->z+z_line_offset);
    glVertex3f(vp2->x, -vp2->y, -vp2->z+z_line_offset);
    glEnd();
  }
  glBegin(GL_LINES);
  // LDRAW has the y value reversed, so negate the y.
  glVertex3f(vp1->x, -vp1->y, -vp1->z+z_line_offset);
  glVertex3f(vp2->x, -vp2->y, -vp2->z+z_line_offset);
  glEnd();
  //glDepthFunc(GL_LESS);

  // Done Drawing a printed poly antialiased edges.  Fix lineWidth.
  if (hardcolor && qualityLines)
    glLineWidth( lineWidth );

  //glFlush();
#endif
}

void render_triangle(vector3d *vp1, vector3d *vp2, vector3d *vp3, int c)
{
	ZPOINT p1, p2, p3;
	ZCOLOR zc, zs;


#ifdef USE_OPENGL
	if(DepthOnly) {//disable color updates (depth buffer updates only)
	  // Should only render this polygon if the color c is on the fade list.
	  // Otherwise just restore color writes and return right now.
	  glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);
	}
	else
#endif
	if(ldraw_commandline_opts.F & TYPE_F_NO_POLYGONS) {
	  if (!(hardcolor && qualityLines))
	    return;
	  // Drawing a printed poly.  Use thin antialiased lines.
	  glLineWidth( 1.0 );
	};

#ifdef USE_OPENGL
  if ((ldraw_commandline_opts.M == 'P') || preprintstep ||
      ((ldraw_commandline_opts.M == 'S') && qualityLines))
  {
    // Non-continuous output stop after each step.
#ifndef ALWAYS_REDRAW
   if (dirtyWindow == 0)
   {  
    if (stepcount != curstep) // (Or (dirty and stepcount < curstep))
     if ((panning == 0) || (stepcount > curstep))
      return;  // Do NOT render since we have not reached this step yet.
   }
   else
#endif
    if (stepcount > curstep)
      return;  // Do NOT render since we have not reached this step yet.
  }
#endif

#if 1
	p1.x = (int)(0.5+vp1->x);
	p1.y = (int)(0.5+vp1->y);
	p1.z = (int)(0.5+Z_SCALE_FACTOR * vp1->z);
	p2.x = (int)(0.5+vp2->x); 
	p2.y = (int)(0.5+vp2->y);
	p2.z = (int)(0.5+Z_SCALE_FACTOR * vp2->z);
	p3.x = (int)(0.5+vp3->x);
	p3.y = (int)(0.5+vp3->y);
	p3.z = (int)(0.5+Z_SCALE_FACTOR * vp3->z);
	translate_color(c,&zc,&zs);
	zAddTriangle(&z, &p1, &p2, &p3, &zc, &zs);
#endif
	switch (ldraw_commandline_opts.output)
	{
	case 3:
		fprintf(output_file,"triangle { <%f,%f,%f> <%f,%f,%f> <%f,%f,%f> texture { Color%d }}\n",
			vp1->x, vp1->y, vp1->z, vp2->x, vp2->y, vp2->z,
			vp3->x, vp3->y, vp3->z, c);
		break;
	case 2:
		break;
	}

#ifdef USE_OPENGL
#if 0
  fprintf(output_file,"triangle { <%.2f,%.2f,%.2f> <%.2f,%.2f,%.2f> <%.2f,%.2f,%.2f> %d = (%d, %d, %d) or (%d, %d, %d)\n",
	  vp1->x, vp1->y, vp1->z, vp2->x, vp2->y, vp2->z,
	  vp3->x, vp3->y, vp3->z, 
	  c, zc.r, zc.g, zc.b, zs.r, zs.g, zs.b);
#endif

  if (cropping)
  {
    // Gotta convert to screen coords first for opengl.
    GLdouble s1x, s1y, s1z;
    GLdouble s2x, s2y, s2z;
    GLdouble s3x, s3y, s3z;

    gluProject((GLdouble)vp1->x, (GLdouble)-vp1->y, (GLdouble)-vp1->z,
	     model_mat, proj_mat, view_mat,
	     &s1x, &s1y, &s1z);

    gluProject((GLdouble)vp2->x, (GLdouble)-vp2->y, (GLdouble)-vp2->z,
	     model_mat, proj_mat, view_mat,
	     &s2x, &s2y, &s2z);

    gluProject((GLdouble)vp3->x, (GLdouble)-vp3->y, (GLdouble)-vp3->z,
	     model_mat, proj_mat, view_mat,
	     &s3x, &s3y, &s3z);

    z.extent_x1 = min(s1x,z.extent_x1);
    z.extent_x2 = max(s1x,z.extent_x2);
    z.extent_y1 = min(s1y,z.extent_y1);
    z.extent_y2 = max(s1y,z.extent_y2);

    z.extent_x1 = min(s2x,z.extent_x1);
    z.extent_x2 = max(s2x,z.extent_x2);
    z.extent_y1 = min(s2y,z.extent_y1);
    z.extent_y2 = max(s2y,z.extent_y2);

    z.extent_x1 = min(s3x,z.extent_x1);
    z.extent_x2 = max(s3x,z.extent_x2);
    z.extent_y1 = min(s3y,z.extent_y1);
    z.extent_y2 = max(s3y,z.extent_y2);
  }

#ifdef EXPERIMENTAL_STIPPLE
  if (c == 374)
  {
    // Draw stippled gold (334) before drawing the quad
    translate_color(334,&zc,&zs);
    setup_material(334,&zc,&zs); // Gold
    setup_material(46,&zc,&zs); // Stipple
    glPolygonStipple(stips[current_stipple]);

    // LDRAW has the y value reversed, so negate the y.
    {
      glBegin(GL_TRIANGLES);
      if (ldraw_commandline_opts.F & TYPE_F_SHADED_MODE) // (zShading)
      {
	getnormal(vp1, vp2, vp3, surf_norm);
	glNormal3f(surf_norm[0], -surf_norm[1], -surf_norm[2]);
      }
      glVertex3f(vp1->x, -vp1->y, -vp1->z);
      glVertex3f(vp2->x, -vp2->y, -vp2->z);
      glVertex3f(vp3->x, -vp3->y, -vp3->z);
      glEnd();
    }

    unset_material(46,&zc,&zs);
    unset_material(334,&zc,&zs);
    c = 373;  // Use sand violet instead of grey brown.
    c = 19;  // tan  
    translate_color(c,&zc,&zs);
  }
#endif // EXPERIMENTAL_STIPPLE

  setup_material(c,&zc,&zs);
  if (DepthOnly) // Write to z-buffer for ALL pixels of "trans" surfaces.
    glDisable(GL_POLYGON_STIPPLE);
  
  // LDRAW has the y value reversed, so negate the y.
  {
    glBegin(GL_TRIANGLES);
    if (ldraw_commandline_opts.F & TYPE_F_SHADED_MODE) // (zShading)
    {
      getnormal(vp1, vp2, vp3, surf_norm);
      glNormal3f(surf_norm[0], -surf_norm[1], -surf_norm[2]);
    }
    glVertex3f(vp1->x, -vp1->y, -vp1->z);
    glVertex3f(vp2->x, -vp2->y, -vp2->z);
    glVertex3f(vp3->x, -vp3->y, -vp3->z);
    glEnd();
  }

  unset_material(c,&zc,&zs);

  // Done Drawing a printed poly antialiased edges.  Fix lineWidth.
  if ((ldraw_commandline_opts.F & TYPE_F_NO_POLYGONS) &&
      (hardcolor && qualityLines))
    glLineWidth( lineWidth );

  //glFlush();
  if(DepthOnly) //re-enable color updates
    glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE); 
#endif
}

int above_line(vector3d *vp1, vector3d *vp2, vector3d *vp3)
{
	float yslope;
	float y;
	
	if (vp2->x == vp1->x) return 1;

	yslope = (vp2->y - vp1->y) / (vp2->x-vp1->x);
	y = (yslope * (vp3->x-vp1->x)) + vp1->y;
	if (vp3->y > y) {
		return 1;
	} else {
		return 0;
	}
}

void render_quad(vector3d *vp1, vector3d *vp2, vector3d *vp3, vector3d *vp4, int c)
{
	ZPOINT p1, p2, p3;
	ZCOLOR zc, zs;
	int bowtie_test_1;
	int bowtie_test_2;

#ifdef USE_OPENGL
	if(DepthOnly) {//disable color updates (depth buffer updates only)
	  // Should only render this polygon if the color c is on the fade list.
	  // Otherwise just restore color writes and return right now.
	  glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);
	}
	else
#endif
	if(ldraw_commandline_opts.F & TYPE_F_NO_POLYGONS) {
	  if (!(hardcolor && qualityLines))
	    return;
	  // Drawing a printed poly.  Use thin antialiased lines.
	  glLineWidth( 1.0 );
	};

#ifdef USE_OPENGL
  if ((ldraw_commandline_opts.M == 'P') || preprintstep ||
      ((ldraw_commandline_opts.M == 'S') && qualityLines))
  {
    // Non-continuous output stop after each step.
#ifndef ALWAYS_REDRAW
   if (dirtyWindow == 0)
   {  
    if (stepcount != curstep) // (Or (dirty and stepcount < curstep))
     if ((panning == 0) || (stepcount > curstep))
      return;  // Do NOT render since we have not reached this step yet.
   }
   else
#endif
    if (stepcount > curstep)
      return;  // Do NOT render since we have not reached this step yet.
  }
#endif

	translate_color(c,&zc,&zs);
	// draw triangle vp1 vp2 vp3
	p1.x = (int)(0.5+vp1->x);
	p1.y = (int)(0.5+vp1->y);
	p1.z = (int)(0.5+Z_SCALE_FACTOR * vp1->z);
	p2.x = (int)(0.5+vp2->x); 
	p2.y = (int)(0.5+vp2->y);
	p2.z = (int)(0.5+Z_SCALE_FACTOR * vp2->z);
	p3.x = (int)(0.5+vp3->x); 
	p3.y = (int)(0.5+vp3->y);
	p3.z = (int)(0.5+Z_SCALE_FACTOR * vp3->z);
	zAddTriangle(&z, &p1, &p2, &p3, &zc, &zs);
	switch (ldraw_commandline_opts.output)
	{
	case 3:
		fprintf(output_file,"triangle { <%f,%f,%f> <%f,%f,%f> <%f,%f,%f> texture { Color%d }}\n",
			vp1->x, vp1->y, vp1->z, vp2->x, vp2->y, vp2->z,
			vp3->x, vp3->y, vp3->z, c);
		break;
	case 2:
		break;
	}
	// draw triangle vp1 vp3 vp4
	p1.x = (int)(0.5+vp1->x);
	p1.y = (int)(0.5+vp1->y);
	p1.z = (int)(0.5+Z_SCALE_FACTOR * vp1->z);
	p2.x = (int)(0.5+vp3->x);
	p2.y = (int)(0.5+vp3->y);
	p2.z = (int)(0.5+Z_SCALE_FACTOR * vp3->z);
	p3.x = (int)(0.5+vp4->x);
	p3.y = (int)(0.5+vp4->y);
	p3.z = (int)(0.5+Z_SCALE_FACTOR * vp4->z);
	zAddTriangle(&z, &p1, &p2, &p3, &zc, &zs);
	switch (ldraw_commandline_opts.output)
	{
	case 3:
		fprintf(output_file,"triangle { <%f,%f,%f> <%f,%f,%f> <%f,%f,%f> texture { Color%d }}\n",
			vp1->x, vp1->y, vp1->z, vp3->x, vp3->y, vp3->z,
			vp4->x, vp4->y, vp4->z, c);
		break;
	case 2:
		break;
	}
#if 1
#if 0
	// make region convex.
	// draw triangle vp2 vp3 vp4
	p1.x =  vp2->x; p1.y = vp2->y; p1.z = Z_SCALE_FACTOR * vp2->z;
	p2.x = vp3->x; p2.y = vp3->y; p2.z = Z_SCALE_FACTOR * vp3->z;
	p3.x = vp4->x; p3.y = vp4->y; p3.z = Z_SCALE_FACTOR * vp4->z;
	zAddTriangle(&z, &p1, &p2, &p3, &zc, &zs);
#else
	// handle concave quads
	bowtie_test_1 = above_line(vp1, vp3, vp2);
	bowtie_test_2 = above_line(vp1, vp3, vp4);
	if (bowtie_test_1 == bowtie_test_2) {
		// vp2 and vp4 lie on same side of line segment.
		// quad probably has a slice missing.
		// drawing triangle vp2 vp3 vp4 will fill it in.
		p1.x = (int)(0.5+vp2->x);
		p1.y = (int)(0.5+vp2->y);
		p1.z = (int)(0.5+Z_SCALE_FACTOR * vp2->z);
		p2.x = (int)(0.5+vp3->x);
		p2.y = (int)(0.5+vp3->y);
		p2.z = (int)(0.5+Z_SCALE_FACTOR * vp3->z);
		p3.x = (int)(0.5+vp4->x);
		p3.y = (int)(0.5+vp4->y);
		p3.z = (int)(0.5+Z_SCALE_FACTOR * vp4->z);
		zAddTriangle(&z, &p1, &p2, &p3, &zc, &zs);
	}
#endif
#endif

#ifdef USE_OPENGL
#if 0
  fprintf(output_file,"quad { <%.2f,%.2f,%.2f> <%.2f,%.2f,%.2f> <%.2f,%.2f,%.2f> <%.2f,%.2f,%.2f> %d = (%d, %d, %d) or (%d, %d, %d)\n",
	  vp1->x, vp1->y, vp1->z, vp2->x, vp2->y, vp2->z,
	  vp3->x, vp3->y, vp3->z, vp4->x, vp4->y, vp4->z,
	  c, zc.r, zc.g, zc.b, zs.r, zs.g, zs.b);
#endif

  if (cropping)
  {
    // Gotta convert to screen coords first for opengl.
    GLdouble s1x, s1y, s1z;
    GLdouble s2x, s2y, s2z;
    GLdouble s3x, s3y, s3z;
    GLdouble s4x, s4y, s4z;

    gluProject((GLdouble)vp1->x, (GLdouble)-vp1->y, (GLdouble)-vp1->z,
	     model_mat, proj_mat, view_mat,
	     &s1x, &s1y, &s1z);

    gluProject((GLdouble)vp2->x, (GLdouble)-vp2->y, (GLdouble)-vp2->z,
	     model_mat, proj_mat, view_mat,
	     &s2x, &s2y, &s2z);

    gluProject((GLdouble)vp3->x, (GLdouble)-vp3->y, (GLdouble)-vp3->z,
	     model_mat, proj_mat, view_mat,
	     &s3x, &s3y, &s3z);

    gluProject((GLdouble)vp4->x, (GLdouble)-vp4->y, (GLdouble)-vp4->z,
	     model_mat, proj_mat, view_mat,
	     &s4x, &s4y, &s4z);

    z.extent_x1 = min(s1x,z.extent_x1);
    z.extent_x2 = max(s1x,z.extent_x2);
    z.extent_y1 = min(s1y,z.extent_y1);
    z.extent_y2 = max(s1y,z.extent_y2);

    z.extent_x1 = min(s2x,z.extent_x1);
    z.extent_x2 = max(s2x,z.extent_x2);
    z.extent_y1 = min(s2y,z.extent_y1);
    z.extent_y2 = max(s2y,z.extent_y2);

    z.extent_x1 = min(s3x,z.extent_x1);
    z.extent_x2 = max(s3x,z.extent_x2);
    z.extent_y1 = min(s3y,z.extent_y1);
    z.extent_y2 = max(s3y,z.extent_y2);

    z.extent_x1 = min(s4x,z.extent_x1);
    z.extent_x2 = max(s4x,z.extent_x2);
    z.extent_y1 = min(s4y,z.extent_y1);
    z.extent_y2 = max(s4y,z.extent_y2);
  }

  // opengl render it as a quad
#ifdef EXPERIMENTAL_STIPPLE
  if (c == 374)
  {
    // Draw stippled gold (334) before drawing the quad
    translate_color(334,&zc,&zs);
    setup_material(334,&zc,&zs); // Gold
    setup_material(46,&zc,&zs); // Stipple
    glPolygonStipple(stips[current_stipple]);

    // LDRAW has the y value reversed, so negate the y.
    if (use_quads)
    {
      glBegin(GL_QUADS);
      if (ldraw_commandline_opts.F & TYPE_F_SHADED_MODE) // (zShading)
      {
	getnormal(vp1, vp2, vp3, surf_norm);
	glNormal3f(surf_norm[0], -surf_norm[1], -surf_norm[2]);
      }
      glVertex3f(vp1->x, -vp1->y, -vp1->z);
      glVertex3f(vp2->x, -vp2->y, -vp2->z);
      glVertex3f(vp3->x, -vp3->y, -vp3->z);
      glVertex3f(vp4->x, -vp4->y, -vp4->z);
    }
    else
    {
      // Lame attempt to fix concave and bowtie quads gives bad shading.
      // Gonna have to do some real math.
      // Either find the convex hull or 
      // Calculate turn direction with dot product of sequential cross products.
      glBegin(GL_TRIANGLES);
      if (ldraw_commandline_opts.F & TYPE_F_SHADED_MODE) // (zShading)
      {
	getnormal(vp1, vp2, vp3, surf_norm);
#if 0
	if (surf_norm[2] > 0.0)// Cheap hack: Always orient the normal toward eye
	  glNormal3f(-surf_norm[0], surf_norm[1], surf_norm[2]);
	else
#endif
	  glNormal3f(surf_norm[0], -surf_norm[1], -surf_norm[2]);
      }
      glVertex3f(vp1->x, -vp1->y, -vp1->z);
      glVertex3f(vp2->x, -vp2->y, -vp2->z);
      glVertex3f(vp4->x, -vp4->y, -vp4->z);
      if (ldraw_commandline_opts.F & TYPE_F_SHADED_MODE) // (zShading)
      {
	getnormal(vp2, vp3, vp4, surf_norm);
	glNormal3f(surf_norm[0], -surf_norm[1], -surf_norm[2]);
      }
      glVertex3f(vp2->x, -vp2->y, -vp2->z);
      glVertex3f(vp3->x, -vp3->y, -vp3->z);
      glVertex3f(vp4->x, -vp4->y, -vp4->z);
      if (ldraw_commandline_opts.F & TYPE_F_SHADED_MODE) // (zShading)
      {
	getnormal(vp1, vp3, vp4, surf_norm);
	glNormal3f(surf_norm[0], -surf_norm[1], -surf_norm[2]);
      }
      glVertex3f(vp1->x, -vp1->y, -vp1->z);
      glVertex3f(vp3->x, -vp3->y, -vp3->z);
      glVertex3f(vp4->x, -vp4->y, -vp4->z);
    }
    glEnd();

    unset_material(46,&zc,&zs);
    unset_material(334,&zc,&zs);
    c = 373;  // Use sand violet instead of grey brown.
    c = 19;  // tan  
    translate_color(c,&zc,&zs);
  }
#endif // EXPERIMENTAL_STIPPLE

  setup_material(c,&zc,&zs);
  if (DepthOnly) // Write to z-buffer for ALL pixels of "trans" surfaces.
    glDisable(GL_POLYGON_STIPPLE);

  // LDRAW has the y value reversed, so negate the y.
  if (use_quads)
  {
    glBegin(GL_QUADS);
    if (ldraw_commandline_opts.F & TYPE_F_SHADED_MODE) // (zShading)
    {
      getnormal(vp1, vp2, vp3, surf_norm);
      glNormal3f(surf_norm[0], -surf_norm[1], -surf_norm[2]);
    }
    glVertex3f(vp1->x, -vp1->y, -vp1->z);
    glVertex3f(vp2->x, -vp2->y, -vp2->z);
    glVertex3f(vp3->x, -vp3->y, -vp3->z);
    glVertex3f(vp4->x, -vp4->y, -vp4->z);
  }
  else
  {
    // Lame attempt to fix concave and bowtie quads gives bad shading.
    // Gonna have to do some real math.
    // Either find the convex hull or 
    // Calculate turn direction with dot product of sequential cross products.
    glBegin(GL_TRIANGLES);
    if (ldraw_commandline_opts.F & TYPE_F_SHADED_MODE) // (zShading)
    {
      getnormal(vp1, vp2, vp3, surf_norm);
#if 0
      if (surf_norm[2] > 0.0)// Cheap hack: Always orient the normal toward eye
	glNormal3f(-surf_norm[0], surf_norm[1], surf_norm[2]);
      else
#endif
	glNormal3f(surf_norm[0], -surf_norm[1], -surf_norm[2]);
    }
    glVertex3f(vp1->x, -vp1->y, -vp1->z);
    glVertex3f(vp2->x, -vp2->y, -vp2->z);
    glVertex3f(vp4->x, -vp4->y, -vp4->z);
    if (ldraw_commandline_opts.F & TYPE_F_SHADED_MODE) // (zShading)
    {
      getnormal(vp2, vp3, vp4, surf_norm);
      glNormal3f(surf_norm[0], -surf_norm[1], -surf_norm[2]);
    }
    glVertex3f(vp2->x, -vp2->y, -vp2->z);
    glVertex3f(vp3->x, -vp3->y, -vp3->z);
    glVertex3f(vp4->x, -vp4->y, -vp4->z);
    if (ldraw_commandline_opts.F & TYPE_F_SHADED_MODE) // (zShading)
    {
      getnormal(vp1, vp3, vp4, surf_norm);
      glNormal3f(surf_norm[0], -surf_norm[1], -surf_norm[2]);
    }
    glVertex3f(vp1->x, -vp1->y, -vp1->z);
    glVertex3f(vp3->x, -vp3->y, -vp3->z);
    glVertex3f(vp4->x, -vp4->y, -vp4->z);
  }
  glEnd();
  unset_material(c,&zc,&zs);

  // Done Drawing a printed poly antialiased edges.  Fix lineWidth.
  if ((ldraw_commandline_opts.F & TYPE_F_NO_POLYGONS) &&
      (hardcolor && qualityLines))
    glLineWidth( lineWidth );

  //glFlush();
  if(DepthOnly) // re-enable color updates
    glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE); 
#endif
}

void render_five(vector3d *vp1, vector3d *vp2, vector3d *vp3, vector3d *vp4, int c)
{
	ZPOINT p1, p2;
	ZCOLOR zc, zs;
	int i;
	int j;
	int wid;

#ifdef USE_OPENGL
  // Gotta convert to screen coords first for opengl.
  GLdouble s1x, s1y, s1z;
  GLdouble s2x, s2y, s2z;
  GLdouble s3x, s3y, s3z;
  GLdouble s4x, s4y, s4z;
#endif

	if(ldraw_commandline_opts.F & TYPE_F_NO_LINES) {
		return;
	};

#ifdef USE_OPENGL
  if ((ldraw_commandline_opts.M == 'P') || preprintstep ||
      ((ldraw_commandline_opts.M == 'S') && qualityLines))
  {
    // Non-continuous output stop after each step.
#ifndef ALWAYS_REDRAW
   if (dirtyWindow == 0)
   {  
    if (stepcount != curstep) // (Or (dirty and stepcount < curstep))
     if ((panning == 0) || (stepcount > curstep))
      return;  // Do NOT render since we have not reached this step yet.
   }
   else
#endif
    if (stepcount > curstep)
      return;  // Do NOT render since we have not reached this step yet.
  }

  gluProject((GLdouble)vp1->x, (GLdouble)-vp1->y, (GLdouble)-vp1->z,
	     model_mat, proj_mat, view_mat,
	     &s1x, &s1y, &s1z);

  gluProject((GLdouble)vp2->x, (GLdouble)-vp2->y, (GLdouble)-vp2->z,
	     model_mat, proj_mat, view_mat,
	     &s2x, &s2y, &s2z);

  gluProject((GLdouble)vp3->x, (GLdouble)-vp3->y, (GLdouble)-vp3->z,
	     model_mat, proj_mat, view_mat,
	     &s3x, &s3y, &s3z);

  gluProject((GLdouble)vp4->x, (GLdouble)-vp4->y, (GLdouble)-vp4->z,
	     model_mat, proj_mat, view_mat,
	     &s4x, &s4y, &s4z);

  if (1)
  {
#if 0
  fprintf(output_file,"render_five(%.2f,%.2f,%.2f) (%.2f,%.2f,%.2f) %d = (%d, %d, %d) or (%d, %d, %d)\n",
	  vp1->x, vp1->y, vp1->z, vp2->x, vp2->y, vp2->z, 
	  c, zc.r, zc.g, zc.b, zs.r, zs.g, zs.b);
#endif

  translate_color(c,&zc, &zs);

  // Only draw optional line p1-p2 if p3 and p4 are on the same side of p1-p2.
  if (turn_vector(s2x-s1x, s2y-s1y, s3x-s2x, s3y-s2y) != 
      turn_vector(s2x-s1x, s2y-s1y, s4x-s2x, s4y-s2y))
  {
#if 0
  fprintf(output_file,
	  "skip_five(%.2f,%.2f) (%.2f,%.2f), (%.2f,%.2f), (%.2f,%.2f)\n",
	  s1x,s1y,s2x,s2y,s3x,s3y,s4x,s4y);
#endif
    return;
  }

  if (cropping)
  {
    z.extent_x1 = min(s1x,z.extent_x1);
    z.extent_x1 = min(s2x,z.extent_x1);
    z.extent_x2 = max(s1x,z.extent_x2);
    z.extent_x2 = max(s2x,z.extent_x2);
    z.extent_y1 = min(s1y,z.extent_y1);
    z.extent_y1 = min(s2y,z.extent_y1);
    z.extent_y2 = max(s1y,z.extent_y2);
    z.extent_y2 = max(s2y,z.extent_y2);
  }

  //glDepthFunc(GL_ALWAYS);
  if (ldraw_commandline_opts.F & TYPE_F_SHADED_MODE) // (zShading)
    if (glCurLighting) glDisable(GL_LIGHTING);
  glCurLighting = 0;
  if (glCurColorIndex != -2)
  {
    if (glCurColorIndex != c)
    {
      //glColor3f(((float)zc.r)/255.0, ((float)zc.g)/255.0, ((float)zc.b)/255.0);
      glColor3ub(zc.r, zc.g, zc.b);
      if (dimLevel)
	//if (curstep > stepcount)
	glColor3ub(zc.r+100, zc.g+100, zc.b+100);
      if (MESA_3_COLOR_FIX)
	glFlush();
    }
    glCurColorIndex = c;
  }

  if (lineWidth > 1.0)
  {
    glBegin(GL_POINTS);
    // LDRAW has the y value reversed, so negate the y.
    glVertex3f(vp1->x, -vp1->y, -vp1->z+z_line_offset);
    glVertex3f(vp2->x, -vp2->y, -vp2->z+z_line_offset);
    glEnd();
  }
  glBegin(GL_LINES);
  // LDRAW has the y value reversed, so negate the y.
  glVertex3f(vp1->x, -vp1->y, -vp1->z+z_line_offset);
  glVertex3f(vp2->x, -vp2->y, -vp2->z+z_line_offset);
  glEnd();
  //glDepthFunc(GL_LESS);

  //glFlush();
  }
#endif

	if ((vp1->x == vp2->x) && (vp1->y == vp2->y)) return;
	if (vp1->x == vp2->x) {
		// vertical line
		if ((vp3->x > (vp1->x + 0.5)) && (vp4->x < (vp1->x - 0.5))) return;
		if ((vp3->x < (vp1->x-0.5)) && (vp4->x > (vp1->x+0.5))) return;
	} else if (vp1->y == vp2->y) {
		if ((vp3->y > (vp1->y + 0.5)) && (vp4->y < (vp1->y-0.5))) return;
		if ((vp3->y < (vp1->y+0.5)) && (vp4->y > (vp1->y-0.5))) return;
	} else {
		if (above_line(vp1, vp2, vp3) != above_line(vp1,vp2,vp4)) return;
	}
	p1.x = (int)(0.5+vp1->x);
	p1.y = (int)(0.5+vp1->y);
	p1.z = (int)(0.5+Z_SCALE_FACTOR * (vp1->z) - Z_LINE_OFFSET);
	p2.x = (int)(0.5+vp2->x); 
	p2.y = (int)(0.5+vp2->y); 
	p2.z = (int)(0.5+Z_SCALE_FACTOR * (vp2->z) - Z_LINE_OFFSET);
	translate_color(c,&zc, &zs);
	zAddLine(&z, &p1, &p2, &zc, &zs, 1);
	if(ldraw_commandline_opts.W > 1) {
		// draw additional lines to make it thicker
		wid = (ldraw_commandline_opts.W)/2;
		p1.x -= wid;
		p2.x -= wid;
		for(i= (-1)*wid; i<=wid; i++) {
			p1.y -= wid;
			p2.y -= wid;
			for (j=(-1)*wid; j<=wid; j++) {
				zAddLine(&z, &p1, &p2, &zc, &zs, 1);
				p1.y++;
				p2.y++;
			}
			p1.y -= (wid+1);
			p2.y -= (wid+1);
			p1.x++;
			p2.x++;
		}
	}
}


#ifndef INLINE_CALC
void calc_zplane(ZPLANE *zpl, ZPOINT *p1, ZPOINT *p2, ZPOINT *p3)
{
	zpl->A = (float)
		(p1->y * (1.0*p2->z - p3->z)/Z_SCALE_FACTOR +
		p2->y * (1.0*p3->z - p1->z)/Z_SCALE_FACTOR +
		p3->y * (1.0*p1->z - p2->z)/Z_SCALE_FACTOR);
	zpl->B = (float)
		(((1.0*p1->z)/Z_SCALE_FACTOR) * (1.0*p2->x - p3->x) +
		((1.0*p2->z)/Z_SCALE_FACTOR) * (1.0*p3->x - p1->x) +
		((1.0*p3->z)/Z_SCALE_FACTOR) * (1.0*p1->x - p2->x));
	zpl->C = (float)((-1) * (
		p1->x * (1.0*p2->y - p3->y) +
		p2->x * (1.0*p3->y - p1->y) +
		p3->x * (1.0*p1->y - p2->y)));
	zpl->D = (float)
		((-1)* p1->x * (1.0*p2->y * ((1.0*p3->z)/Z_SCALE_FACTOR) - 1.0*p3->y * ((1.0*p2->z)/Z_SCALE_FACTOR)) -
		p2->x * (1.0*p3->y * ((1.0*p1->z)/Z_SCALE_FACTOR) - 1.0*p1->y * ((1.0*p3->z)/Z_SCALE_FACTOR)) -
		p3->x * (1.0*p1->y * ((1.0*p2->z)/Z_SCALE_FACTOR) - 1.0*p2->y * ((1.0*p1->z)/Z_SCALE_FACTOR)));
}

void calc_zplane_z(ZPLANE *zpl, ZPOINT *p)
{
	if (zpl->C != 0.0) {
		p->z = (int) (Z_SCALE_FACTOR*((zpl->D + zpl->A * p->x + zpl->B * p->y) / zpl->C));
	} else {
		p->z = INT_MAX; // do not draw
	}
}
#endif

#ifdef USE_OPENGL

#if defined(UNIX) || defined(MAC) || defined(MACOS_X)
// Stub out a few windows structs for now. 
// Should go back and remove them with #ifdef USE_OPENGL someday.
#include "wstubs.h" 
#else
/* Windows */
#include "wtypes.h"
/* #include "wingdi.h" */
#endif

#else
#include "wtypes.h"
#include "wingdi.h"
#endif

int init_zimage(ZIMAGE *zp, int rows, int cols)
{
	int i,j;

	if ((zp->rows!=rows)||(zp->cols!=cols)) {
		LPBITMAPINFOHEADER bmh;

// Get rid of the zbuffer since we don't want to draw to it AND opengl.
#ifndef USE_OPENGL
		// reset bitmap
		if (zp->zbuffer != NULL) free(zp->zbuffer);
		zp->zbuffer = (int *)malloc(rows * cols * sizeof(int));
		if (zp->zbuffer == NULL) {
			return (-1);
		}
#else
		zp->zbuffer = NULL;
#endif
		if (zp->dib != NULL) free (zp->dib);
#ifdef USE_OPENGL
		// Hmmm, why do I malloc the whole dib?  I SHOULD only need the header...
		// Well, actually the header and bytes 40-42 used below...
		// Since I have it, I should use this for printing, especially BMP8.
		zp->dib = malloc(sizeof(BITMAPINFOHEADER) + 256);
#else
		zp->dib = malloc(sizeof(BITMAPINFOHEADER) + 3*rows*cols);
#endif
		if (zp->dib == NULL) {
			free(zp->zbuffer);
			zp->zbuffer = NULL;
			return (-1);
		}
		zp->r = zp->dib + 42;
		zp->g = zp->dib + 41;
		zp->b = zp->dib + 40;
		bmh = (LPBITMAPINFOHEADER) zp->dib;
		bmh->biSize = sizeof(BITMAPINFOHEADER);
		/* The notion of rows and columns is mixed up ! 
		I don't know if you will just exchange rows and columns all over the source,
		or if you at the same time will use e.g. width and height in stead.
		When this has been fixed, another Width variable is required,
		as the actual width of the DIB must be a multiple of 4.
		*/
		bmh->biWidth = rows;  /* Must be a multiple of 4 */
		bmh->biHeight = -cols;
		bmh->biPlanes = 1;
		bmh->biBitCount = 24;
		bmh->biCompression = BI_RGB;
		bmh->biSizeImage = (bmh->biWidth*bmh->biBitCount+31)/32*4
			* (bmh->biHeight>0?bmh->biHeight:-bmh->biHeight);
		bmh->biXPelsPerMeter = 0;
		bmh->biYPelsPerMeter = 0;
		bmh->biClrUsed = 0;
		bmh->biClrImportant = 0;
		zp->rows = rows;
		zp->cols = cols;
#ifndef USE_OPENGL
		for(i=zp->rows*zp->cols; --i>=0;)
			zp->zbuffer[i] = INT_MAX;
		// memset(zp->dib+40,0xff,3*rows*cols);
		{
			ZCOLOR zcp, zcs;
			// need to speed this up	
			translate_color(ldraw_commandline_opts.B, &zcp, &zcs);
			{
				char *zbp;
				zbp = zp->dib+40;
				while (zbp < ((zp->dib+40)+3*rows*cols-5)) {
					*zbp++ = zcp.b;
					*zbp++ = zcp.g;
					*zbp++ = zcp.r;
					*zbp++ = zcs.b;
					*zbp++ = zcs.g;
					*zbp++ = zcs.r;
				}
			}
		}
#endif
	} else {
#ifndef USE_OPENGL
		ZCOLOR zcp, zcs;
		ZCOLOR *zc;
		int pixel;
		translate_color(ldraw_commandline_opts.B, &zcp, &zcs);
		// same size image, so just reuse bitmap from last time
		for(i=max(0,(zp->extent_x1-1)); i<=zp->extent_x2; i++) {
			for(j=max(0,(zp->extent_y1-1)); j<=zp->extent_y2; j++) {
				pixel = i + zp->rows*j;
				zp->zbuffer[pixel] = INT_MAX;
				pixel += pixel*2;
				if ((i+j)%2) {
					zc = &zcp;
				} else {
					zc = &zcs;
				}
				zp->r[pixel]= zc->r;
				zp->g[pixel]= zc->g;
				zp->b[pixel]= zc->b;
				
			}
		}
#endif
	}
	zp->dirty_x1 = 0;
	zp->dirty_x2 = rows-1;
	zp->dirty_y1 = 0;
	zp->dirty_y2 = cols-1;
	zp->extent_x1 = rows-1;
	zp->extent_x2 = 0;
	zp->extent_y1 = cols-1;
	zp->extent_y2 = 0;
	return 0;
}

void zAddTriangle(ZIMAGE *zp, ZPOINT *p1, ZPOINT *p2, ZPOINT *p3, ZCOLOR *zcp, ZCOLOR *zcs)
{
	int miny;
	int maxy;
	ZPOINT p;
	int miny2;
	int maxy2;
	double slope[3];
	double slope_inv[3];
	double intercept;
	int first_point, second_point;
	int intersection;
	ZPLANE zpl;
	int pixel;
	ZCOLOR *zc;
	double shading_factor = -1.0;
#define TRIVIAL_Z
#ifdef TRIVIAL_Z
	float min_z;
#endif

	if (!zp->zbuffer) {
		return;
	}

#ifdef TRIVIAL_Z
	// This is the smallest z value of any point on the triangle.
	min_z = (float)(min(min(p1->z,p2->z),p3->z)-Z_POLY_TOLERANCE);
#endif


#ifndef INLINE_CALC
	calc_zplane(&zpl, p1, p2, p3);
#else
	zpl.A = (float)
		(p1->y * (1.0*p2->z - p3->z)/Z_SCALE_FACTOR +
		p2->y * (1.0*p3->z - p1->z)/Z_SCALE_FACTOR +
		p3->y * (1.0*p1->z - p2->z)/Z_SCALE_FACTOR);
	zpl.B = (float)
		(((1.0*p1->z)/Z_SCALE_FACTOR) * (1.0*p2->x - p3->x) +
		((1.0*p2->z)/Z_SCALE_FACTOR) * (1.0*p3->x - p1->x) +
		((1.0*p3->z)/Z_SCALE_FACTOR) * (1.0*p1->x - p2->x));
	zpl.C = (float)((-1) * (
		p1->x * (1.0*p2->y - p3->y) +
		p2->x * (1.0*p3->y - p1->y) +
		p3->x * (1.0*p1->y - p2->y)));
	zpl.D = (float)
		((-1)* p1->x * (1.0*p2->y * ((1.0*p3->z)/Z_SCALE_FACTOR) - 1.0*p3->y * ((1.0*p2->z)/Z_SCALE_FACTOR)) -
		p2->x * (1.0*p3->y * ((1.0*p1->z)/Z_SCALE_FACTOR) - 1.0*p1->y * ((1.0*p3->z)/Z_SCALE_FACTOR)) -
		p3->x * (1.0*p1->y * ((1.0*p2->z)/Z_SCALE_FACTOR) - 1.0*p2->y * ((1.0*p1->z)/Z_SCALE_FACTOR)));
#endif
	slope_inv[0] = 0.0;
	slope_inv[1] = 0.0;
	slope_inv[2] = 0.0;

	maxy = min(max(p1->y, max(p2->y, p3->y)),(zp->cols-1));
	miny = max(min(p1->y, min(p2->y, p3->y)),0);
	{
		int x1, x2, y1, y2;

		x1 = max(min(p1->x, min(p2->x, p3->x)),0);
		x2 = min(max(p1->x, max(p2->x, p3->x)),(zp->rows-1));
		y1 = miny;
		y2 = maxy;
		zp->dirty_x1 = min(x1,zp->dirty_x1);
		zp->dirty_x2 = max(x2,zp->dirty_x2);
		zp->dirty_y1 = min(y1,zp->dirty_y1);
		zp->dirty_y2 = max(y2,zp->dirty_y2);
		zp->extent_x1 = min(x1,zp->extent_x1);
		zp->extent_x2 = max(x2,zp->extent_x2);
		zp->extent_y1 = min(y1,zp->extent_y1);
		zp->extent_y2 = max(y2,zp->extent_y2);
	}

	for(p.y=miny; p.y<=maxy; p.y++) {
		first_point = INT_MAX;
		second_point = INT_MIN;
		// first edge
		miny2 = min(p1->y, p2->y);
		maxy2 = max(p1->y, p2->y);
		if ((miny2 != maxy2) && (p.y >= miny2) && (p.y <= maxy2)) {
			// must intersect
			if(p1->x != p2->x) {
				if (slope_inv[0]==0.0) {
					slope[0] = (p2->y - p1->y)/((double)(p2->x - p1->x));
					slope_inv[0] = 1.0/slope[0];
				}
				intercept = p1->y - slope[0] * p1->x;
				intersection = (int)((p.y - intercept) * slope_inv[0]);
			} else {
				intersection = p1->x;
			}
			first_point = min(first_point,intersection);
			second_point = max(second_point, intersection);
		}
		// second edge
		miny2 = min(p2->y, p3->y);
		maxy2 = max(p2->y, p3->y);
		if ((miny2 != maxy2) && (p.y >= miny2) && (p.y <= maxy2)) {
			// must intersect
			if(p2->x != p3->x) {
				if (slope_inv[1]==0.0) {
					slope[1] = (p3->y - p2->y) / ((double)(p3->x - p2->x));
					slope_inv[1] = 1.0/slope[1];
				}
				intercept = p2->y - slope[1] * p2->x;
				intersection = (int)((p.y - intercept) * slope_inv[1]);
			} else {
				intersection = p2->x;
			}
			first_point = min(first_point,intersection);
			second_point = max(second_point, intersection);	
		}
		// third edge
		miny2 = min(p3->y, p1->y);
		maxy2 = max(p3->y, p1->y);
		if ((miny2 != maxy2) && (p.y >= miny2) && (p.y <= maxy2)) {
			// must intersect
			if(p3->x != p1->x) {
				if (slope_inv[2]==0.0) {
					slope[2] = (p1->y - p3->y) / ((double)(p1->x - p3->x));
					slope_inv[2] = 1.0/slope[2];
				}
				intercept = p3->y - slope[2] * p3->x;
				intersection = (int)((p.y - intercept) * slope_inv[2]);
			} else {
				intersection = p3->x;
			}
			first_point = min(first_point,intersection);
			second_point = max(second_point, intersection);
		}
		first_point = max(0,first_point);
		second_point = min(((zp->rows)-1), second_point);
		{
			int x_span = -1;
			{
				for(p.x = first_point; p.x <= second_point; p.x++) {
					{ 
						pixel = p.x + zp->rows*p.y;
#ifdef TRIVIAL_Z
						if (zp->zbuffer[pixel] > min_z)
#endif
						{
							// dither colors
							if ((p.x+p.y)%2) {
								zc = zcp;
							} else {
								zc = zcs;
							}
							if (zc->a > 0) {
#ifndef INLINE_CALC
								calc_zplane_z(&zpl, &p);
#else
								if (zpl.C != 0.0) {
									p.z = (int) (Z_SCALE_FACTOR*((zpl.D + zpl.A * p.x + zpl.B * p.y) / zpl.C));
								} else {
									p.z = INT_MAX; // do not draw
								}
#endif
								if ((p.z > ldraw_commandline_opts.Z) &&
									(zp->zbuffer[pixel] > (p.z-Z_POLY_TOLERANCE))) {
									zp->zbuffer[pixel] = p.z;
									// multiply by 3 to get offset into DIB
									pixel += pixel+pixel;
									switch (ldraw_commandline_opts.emitter) {
									case 1:
										// only calculate once per triangle
										if(shading_factor == -1.0) {
											// Since many of the ldraw dat files have inconsistant normals,
											// shading cannot use the sign of the dot product of the light
											// direction and the surface normal.  I take the fabs() of the
											// dot product, which produces lighting as if there were two
											// lights on opposite sides of the model.
											ZPOINT light;
											light.x = 1; light.y= 1; light.z = 1;
											// correct normal for our exagerated z axis
											shading_factor = (light.x * zpl.A + light.y *zpl.B - light.z*(zpl.C)) /
												sqrt(light.x*light.x + light.y*light.y + light.z*light.z) /
												sqrt(zpl.A * zpl.A + zpl.B*zpl.B + (zpl.C)*(zpl.C));
											// reduce the effect of shading -- we don't want parts ofthe image
											// to be completely blackened out.
											shading_factor = (fabs(shading_factor)/4) + 0.75;
											if ( (shading_factor<0.74) || (shading_factor > 1.0)) {
												// should never happen
												char buf[256];
												sprintf(buf,"Problem with shading, shading_factor = %g",shading_factor);
												zWrite(buf);
												shading_factor = 1.0;
											}
										}
										zp->r[pixel]= (int) (shading_factor * zc->r);
										zp->g[pixel]= (int) (shading_factor* zc->g);
										zp->b[pixel]= (int) (shading_factor*zc->b);
										break;
									case 2:
										// only calculate once per triangle
										if(shading_factor == -1.0) {
											// Since many of the ldraw dat files have inconsistant normals,
											// shading cannot use the sign of the dot product of the light
											// direction and the surface normal.  I take the fabs() of the
											// dot product, which produces lighting as if there were two
											// lights on opposite sides of the model.
											ZPOINT light;
											light.x = 1; light.y= 1; light.z = 1;
											// correct normal for our exagerated z axis
											shading_factor = (light.x * zpl.A + light.y *zpl.B - light.z*(zpl.C)) /
												sqrt(light.x*light.x + light.y*light.y + light.z*light.z) /
												sqrt(zpl.A * zpl.A + zpl.B*zpl.B + (zpl.C)*(zpl.C));
											// reduce the effect of shading -- we don't want parts ofthe image
											// to be completely blackened out.
											shading_factor = (fabs(shading_factor)/4) + 0.75;
											if ( (shading_factor<0.74) || (shading_factor > 1.0)) {
												// should never happen
												char buf[256];
												sprintf(buf,"Problem with shading, shading_factor = %g",shading_factor);
												zWrite(buf);
												shading_factor = 1.0;
											}
											// add "fog" effect, fade to black.
											shading_factor -= (1024.0 * 80.0 * p.z) / INT_MAX; 
											if (shading_factor < 0.01)
											{
												shading_factor = 0.01;
											} else if (shading_factor > 1.0)
											{
												shading_factor = 1.0;
											}
										}
										zp->r[pixel]= (int) (shading_factor * zc->r);
										zp->g[pixel]= (int) (shading_factor* zc->g);
										zp->b[pixel]= (int) (shading_factor*zc->b);
										break;
									case 3:
										// generate illumination based on a point source centered at the origin.
										shading_factor = (p.x * zpl.A + p.y *zpl.B - p.z*(zpl.C)) /
												sqrt(p.x*p.x + p.y*p.y + p.z*p.z) /
												sqrt(zpl.A * zpl.A + zpl.B*zpl.B + (zpl.C)*(zpl.C));
											// reduce the effect of shading -- we don't want parts ofthe image
											// to be completely blackened out.
											shading_factor = (fabs(shading_factor)/2) + 0.5;
											if ( (shading_factor<0.49) || (shading_factor > 1.0)) {
												// should never happen
												char buf[256];
												sprintf(buf,"Problem with shading, shading_factor = %g",shading_factor);
												zWrite(buf);
												shading_factor = 1.0;
											}
										zp->r[pixel]= (int) (shading_factor * zc->r);
										zp->g[pixel]= (int) (shading_factor* zc->g);
										zp->b[pixel]= (int) (shading_factor*zc->b);
										break;
									default:
										zp->r[pixel]= zc->r;
										zp->g[pixel]= zc->g;
										zp->b[pixel]= zc->b;
									}
								}
							}
						}
					}
				}
			}
		}
}
}

void zAddLine(ZIMAGE *zp, ZPOINT *pc1, ZPOINT *pc2, ZCOLOR *zcp, ZCOLOR *zcs, int replace)
{
	float yslope;
	float zslope;
	int x,y,z;
	int temp;
	int transposed=0;
	ZPOINT pA, pB;
	ZPOINT *p1;
	ZPOINT *p2;
	int pixel;
	ZCOLOR *zc;
	int line_width=3;

	
	if (!zp->zbuffer) {
		return;
	}
	{
		int x1, x2, y1, y2;

		x1 = max(min(pc1->x, pc2->x),0);
		x2 = min(max(pc1->x, pc2->x),(zp->rows-1));
		y1 = max(min(pc1->y, pc2->y),0);
		y2 = min(max(pc1->y, pc2->y),(zp->cols-1));
		zp->dirty_x1 = min(x1,zp->dirty_x1);
		zp->dirty_x2 = max(x2,zp->dirty_x2);
		zp->dirty_y1 = min(y1,zp->dirty_y1);
		zp->dirty_y2 = max(y2,zp->dirty_y2);
		zp->extent_x1 = min(x1,zp->extent_x1);
		zp->extent_x2 = max(x2,zp->extent_x2);
		zp->extent_y1 = min(y1,zp->extent_y1);
		zp->extent_y2 = max(y2,zp->extent_y2);
	}

	pA = *pc1;
	pB = *pc2;
	p1 = &pA;
	p2 = &pB;

	// Handle vertical line case
	if (p1->x == p2->x) {
		if(p1->y > p2->y) {
			// swap points
			temp = p1->x;
			p1->x = p2->x;
			p2->x = temp;
			temp=p1->y;
			p1->y=p2->y;
			p2->y = temp;
			temp=p1->z;
			p1->z=p2->z;
			p2->z = temp;
		}
		if (p2->y == p1->y) {
			zslope = 0.0;
		} else {
			zslope = (p2->z - p1->z)/(float)(p2->y-p1->y);
		}
		if ((p1->x >=0) && (p1->x < zp->rows)) {
			for(y= max(0,p1->y); y<= min(((zp->cols)-1),p2->y); y++) {
				z = (int)(zslope * (y-p1->y)) + p1->z;
				pixel = p1->x + zp->rows*y;
				if((pixel>=0) && (pixel < (zp->rows * zp->cols))) {
					if (((replace == 0) && 
						(z > ldraw_commandline_opts.Z) &&
						(zp->zbuffer[pixel] > (z+Z_LINE_TOLERANCE))) ||
						((replace == 1) && 
						(z > ldraw_commandline_opts.Z) &&
						(zp->zbuffer[pixel] >= (z+Z_LINE_TOLERANCE)))){
						// dither colors
						if ((p1->x+y)%2) {
							zc = zcp;
						} else {
							zc = zcs;
						}
						if (zc->a > 0) {
							zp->zbuffer[pixel] = z;
							pixel += pixel+pixel;
							zp->r[pixel]= zc->r;
							zp->g[pixel]= zc->g;
							zp->b[pixel]= zc->b;
						}
					}
				}
			}
		}
		return;
	}
	// Handle horizontal line case
	if (p1->y == p2->y) {
		if(p1->x > p2->x) {
			// swap points
			temp = p1->x;
			p1->x = p2->x;
			p2->x = temp;
			temp=p1->y;
			p1->y=p2->y;
			p2->y = temp;
			temp=p1->z;
			p1->z=p2->z;
			p2->z = temp;
		}
		if (p2->x == p1->x) {
			zslope = 0.0;
		} else {
			zslope = (p2->z - p1->z)/(float)(p2->x-p1->x);
		}
		if ((p1->y >=0) && (p1->y < zp->cols)) {
			for(x= max(0,p1->x); x<= min(((zp->rows)-1),p2->x); x++) {
				z = (int)(zslope * (x-p1->x)) + p1->z;
				pixel = x + zp->rows * p1->y;
				if((pixel>=0) && (pixel < (zp->rows * zp->cols))) {
					if (((replace == 0) && 
						(z > ldraw_commandline_opts.Z) &&
						(zp->zbuffer[pixel] > (z+Z_LINE_TOLERANCE))) ||
						((replace == 1) && 
						(z > ldraw_commandline_opts.Z) &&
						(zp->zbuffer[pixel] >= (z+Z_LINE_TOLERANCE)))){
						// dither colors
						if ((x+p1->y)%2) {
							zc = zcp;
						} else {
							zc = zcs;
						}
						if (zc->a > 0) {
							zp->zbuffer[pixel] = z;
							pixel += pixel+pixel;
							zp->r[pixel]= zc->r;
							zp->g[pixel]= zc->g;
							zp->b[pixel]= zc->b;
						}
					}
				}
			}
		}
		return;
	}
	// Handle diagonals
	if(p1->x > p2->x) {
		// swap points
		temp = p1->x;
		p1->x = p2->x;
		p2->x = temp;
		temp=p1->y;
		p1->y=p2->y;
		p2->y = temp;
		temp=p1->z;
		p1->z=p2->z;
		p2->z = temp;
	}
	yslope = (float)(p2->y - p1->y) / (float)(p2->x-p1->x);
	// Transpose if slope too steep.
	if (fabs(yslope) > 1.0) {
		transposed = 1;
		temp = p1->x;
		p1->x = p1->y;
		p1->y = temp;
		temp = p2->x;
		p2->x = p2->y;
		p2->y = temp;
		if(p1->x > p2->x) {
			// swap points
			temp = p1->x;
			p1->x = p2->x;
			p2->x = temp;
			temp=p1->y;
			p1->y=p2->y;
			p2->y = temp;
			temp=p1->z;
			p1->z=p2->z;
			p2->z = temp;
		}
		yslope = (float)(p2->y - p1->y) / (float)(p2->x-p1->x);
	}
	// Handle diagonals with yslope < 1
	zslope = (p2->z - p1->z)/(float)(p2->x-p1->x);
	for(x= max(0,p1->x); x<= min(( ((transposed==0)?(zp->rows):(zp->cols))-1),p2->x); x++) {
		y = (int)(yslope * (x-p1->x)) + p1->y;
		if ((y>0) && (y < ((transposed==0)?(zp->cols):(zp->rows)))) {
			z = (int)(zslope * (x-p1->x)) + p1->z;
			if (transposed == 0) {
				pixel = x + zp->rows*y;
			} else {
				pixel = y+ zp->rows*x;
			}
			if((pixel>=0) && (pixel < (zp->rows * zp->cols))) {
				if (((replace == 0) && 
					(z > ldraw_commandline_opts.Z) &&
					(zp->zbuffer[pixel] > (z+Z_LINE_TOLERANCE))) ||
					((replace == 1) && 
					(z > ldraw_commandline_opts.Z) &&
					(zp->zbuffer[pixel] >= (z+Z_LINE_TOLERANCE)))){
						// dither colors
						if ((x+y)%2) {
							zc = zcp;
						} else {
							zc = zcs;
						}
						if (zc->a > 0) {
							zp->zbuffer[pixel] = z;
							pixel+=pixel+pixel;
							zp->r[pixel]= zc->r;
							zp->g[pixel]= zc->g;
							zp->b[pixel]= zc->b;
						}
				} 
			}
		}
	}

}

void zDraw(void *zDC)
{
	static int init=0;

	if (z.zbuffer != NULL) {
		platform_zDraw(&z, zDC);
	}
}

int zReset(long *rows, long *cols)
{
	if (init_zimage(&z, *rows, *cols)) {
		// Severe memory problems
		return -1;
	}
	return 0;
}

int zInvalidate()
{
	// Mark whole image as used
	z.dirty_x1 = 0;
	z.dirty_x2 = z.rows-1;
	z.dirty_y1 = 0;
	z.dirty_y2 = z.cols-1;
	z.extent_x1 = 0;
	z.extent_x2 = z.rows-1;
	z.extent_y1 = 0;
	z.extent_y2 = z.cols-1;		
	return 0;
}

void zFree()
{
	if (z.zbuffer != NULL) {
		free(z.zbuffer);
		z.zbuffer = NULL;
	}
	if (z.dib != NULL) {
		free(z.dib);
		z.dib = NULL;
	}
}

void zStep(int step, int pause)
{
	platform_step(step, include_stack_ptr, pause, &z);
}

void zPause(void)
{
	platform_step(-1, include_stack_ptr, 1, &z);
}

void zWrite(char *message)
{
	platform_comment(message, 0);
}

void zClear()
{
  int oldrows, oldcols;

#ifdef USE_OPENGL
  // What a mess!  ldlite uses zGet*size() to center the view.
  // ldglite uses glulookat() so these fns are rigged to return 0.
  oldrows = z.rows;  
  oldcols = z.cols;  // Probably should use Width and Height (to be sure). 
#else
  oldrows = zGetRowsize();
  oldcols = zGetColsize();
#endif
  zReset(&oldrows, &oldcols);

#ifdef USE_OPENGL
  // Got "0 CLEAR"
  // May want to do something else here when in editing mode.
  if ((ldraw_commandline_opts.M == 'P') || preprintstep ||
      ((ldraw_commandline_opts.M == 'S') && qualityLines))
  {
    // Non-continuous output stop after each step.
#ifndef ALWAYS_REDRAW
   if (dirtyWindow == 0)
   {  
    if (stepcount != curstep) // (Or (dirty and stepcount < curstep))
     if ((panning == 0) || (stepcount > curstep))
      return;  // Do NOT render since we have not reached this step yet.
   }
   else
#endif
    if (stepcount > curstep)
      return;  // Do NOT render since we have not reached this step yet.
  }
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#endif	
}

void zSave(int step)
{  // bitmap saves not yet supported
}

int zGetRowsize(void)
{
#ifdef USE_OPENGL 
  // What a mess!  ldlite uses zGet*size() to center the view.
  // ldglite uses glulookat() so these fns are rigged to return 0.
  return 0; // Fix this.  Do I have rows and cols backwards from ldlite?
#else
	return z.rows;
#endif
}

int zGetColsize(void)
{
#ifdef USE_OPENGL
  // What a mess!  ldlite uses zGet*size() to center the view.
  // ldglite uses glulookat() so these fns are rigged to return 0.
  return 0; // Fix this.  Do I have rows and cols backwards from ldlite?
#else
	return z.cols;
#endif
}
