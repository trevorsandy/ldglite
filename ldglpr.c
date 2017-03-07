/*
 *  ldglpr.c   Module for printing pictures of ldlite .dat files
 *  Copyright (C) 2001  DMH
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

extern char buf[10240];
extern int use_uppercase;
extern int use_png_alpha;
extern ZIMAGE z;
extern GLint Width;
extern GLint Height;
extern int cropping;
extern char progname[256];

extern int curstep;
extern int OffScreenRendering;
extern int renderbuffer; 

extern int downsample; 

#ifdef OSMESA_OPTION
#include "GL/osmesa.h"
void *OSbuffer = NULL;
OSMesaContext ctx;
#endif
#ifdef WIN_DIB_OPTION
HGLRC hGLRC;
HBITMAP m_dibSection;           // memory DIB for offscreen rendering
#endif
#ifdef AGL_OFFSCREEN_OPTION
#include <AGL/agl.h>
void *OSbuffer = NULL;
AGLContext ctx;
// NOTE:  Apple currently lists aglSetOffScreen() as unsupported by Carbon.
#endif
#ifdef CGL_OFFSCREEN_OPTION
//#include <CGL/CGLCurrent.h>
//#include <CGL/CGLTypes.h>
#if defined(USING_CARBOM)
#  include <Carbon/Carbon.h>
#elif defined(USING_COCOA)
//#  include <Cocoa/Cocoa.h>
#endif
#include <OpenGL/OpenGL.h>
#include <OpenGL/gl.h>
void *OSbuffer = NULL;
CGLContextObj ctx;
#endif
char *pix;

#ifdef MACOS_X
void GetAvailablePos(int *w, int *h)
{
  /*
  //HIWindowGetAvailablePositioningBounds() // Leopard only?
  //Prior to Leopard, just use GetAvailableWindowPositioningBounds
  Rect rect;
  //GetAvailableWindowPositioningBounds( GetMainDevice(), &rect );
  GetAvailableWindowPositioningBounds( NULL, &rect ); //GetMainDevice() is old.
  *w = rect.right - rect.left;
  *h = rect.bottom - rect.top;

  */

  // test only
  *w = 640;
  *h = 480;

}
#endif

/***************************************************************/
#define BYTE1(i) ((unsigned char) (i & 0x0ff))
#define BYTE2(i) ((unsigned char) ((i / 0x100) & 0x0ff))
#define BYTE3(i) ((unsigned char) ((i / 0x10000) & 0x0ff))
#define BYTE4(i) ((unsigned char) ((i / 0x1000000) & 0x0ff))

/***************************************************************/
FILE *start_bmp(char *filename, int width, int height)
{
  BITMAPINFOHEADER bmhbuf;
  BITMAPFILEHEADER bmfh;
  LPBITMAPINFOHEADER bmh;
  char *p, c;
  FILE *fp;
  char hdr[54];

  printf("Write BMP %s\n", filename);
  if ((fp = fopen(filename,"wb+"))==NULL) {
    printf("Could not open %s\n", filename);
    return(NULL);
  }
  
  //bmh = (LPBITMAPINFOHEADER) z.dib;
  bmh = (LPBITMAPINFOHEADER) &bmhbuf;
  bmh->biSize = 40;
  bmh->biWidth = width;
  bmh->biHeight = height;
  bmh->biPlanes = 1;
  //bmh->biBitCount = 16;
  bmh->biBitCount = 24;

  bmh->biSizeImage = (bmh->biWidth*bmh->biBitCount+31)/32*4
    * (bmh->biHeight>0?bmh->biHeight:-bmh->biHeight);

  p = (char *) &(bmfh.bfType);
  p[0] = 'B';
  p[1] = 'M';
  bmfh.bfReserved1 = 0;
  bmfh.bfReserved2 = 0;
  bmfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
  bmfh.bfSize = (bmfh.bfOffBits + bmh->biSizeImage) / 4;

  bmh->biSizeImage = 0;

#if 0
  // 14 bytes
  fwrite(&bmfh.bfType, 2, 1, fp);
  fwrite(&bmfh.bfSize, 4, 1, fp);
  fwrite(&bmfh.bfReserved1, 2, 1, fp);
  fwrite(&bmfh.bfReserved2, 2, 1, fp);
  fwrite(&bmfh.bfOffBits, 4, 1, fp);
  // 40 bytes
  fwrite(bmh, sizeof(BITMAPINFOHEADER), 1, fp);
#else  
  // Copy the 54 bytes of bmfh and bmh into hdr in intel byte order & packing.
  memset(hdr, 0, 54);
  hdr[0] = 'B';
  hdr[1] = 'M';
  hdr[2] = BYTE1(bmfh.bfSize);
  hdr[3] = BYTE2(bmfh.bfSize);
  hdr[4] = BYTE3(bmfh.bfSize);
  hdr[5] = BYTE4(bmfh.bfSize);
  hdr[10] = BYTE1(bmfh.bfOffBits);
  hdr[11] = BYTE2(bmfh.bfOffBits);
  hdr[12] = BYTE3(bmfh.bfOffBits);
  hdr[13] = BYTE4(bmfh.bfOffBits);

  hdr[14] = 40;
  hdr[15] = 0;
  hdr[16] = 0;
  hdr[17] = 0;
  hdr[18] = BYTE1(bmh->biWidth);
  hdr[19] = BYTE2(bmh->biWidth);
  hdr[20] = BYTE3(bmh->biWidth);
  hdr[21] = BYTE4(bmh->biWidth);
  hdr[22] = BYTE1(bmh->biHeight);
  hdr[23] = BYTE2(bmh->biHeight);
  hdr[24] = BYTE3(bmh->biHeight);
  hdr[25] = BYTE4(bmh->biHeight);
  hdr[26] = 1;
  hdr[27] = 0;
  hdr[28] = 24;
  hdr[29] = 0;
  // The rest are all zeros.
  fwrite(hdr, 54, 1, fp);
#endif

  return(fp);
}

/***************************************************************/
void write_bmp(char *filename)
{
  int i, j;
  char *p, c;
  FILE *fp;

  int width = Width;
  int height = Height;
  GLint xoff = 0;
  GLint yoff = 0;
  pix = &buf[0];
  if (width > 2560)
    pix = (char*)malloc(width*3);

  if (cropping)
  {
    xoff = max(0, z.extent_x1);
    yoff = max(0, z.extent_y1);
    width = min((z.extent_x2 - xoff), (Width - xoff));
    height = min((z.extent_y2 + 1 - yoff), (Height - yoff));
    //width = ((width + 31)/32) * 32; // round to a multiple of 32.
    width = ((width + 3)/4) * 4; // round to a multiple of 4.
    if (ldraw_commandline_opts.debug_level == 1)
      printf("bmpsize = (%d, %d) at (%d, %d)\n", width, height, xoff, yoff);
    if ((width <= 0) || (height <= 0)) return;
  }

  fp = start_bmp(filename, width, height);
  if (fp == NULL)
    return;

  // no pallete since we use RGB

  glReadBuffer(renderbuffer);
  for (i = 0; i < height; i++)
  {
#ifdef OSMESA_OPTION
    if (OffScreenRendering)
    {
      int j;
      char *b = (char *)OSbuffer;
      b += ((i+yoff)*Width +xoff) *4;
      for (j=0; j<width; j++) {
	// MESA or OSmesa bug?  ReadPixels gives GBR instead of RGB???
	pix[3*j] = b[1];
	pix[3*j+1] = b[0];
	pix[3*j+2] = b[2];
	b+=4;
      }
    }
    else
#endif
    {
      glReadPixels(xoff, i+yoff, width, 1, GL_RGB, GL_UNSIGNED_BYTE, pix);
      p = pix;
      for (j = 0; j < width; j++) // RGB -> BGR
      {
        c = p[0];
#ifdef WINDOWS       
        p[0] = p[2];
        p[2] = c;
#else
#ifdef OSMESA_OPTION
	// MESA or OSmesa bug?  ReadPixels gives GBR instead of RGB???
        p[0] = p[1];
        p[1] = c;
#endif
#ifdef CGL_OFFSCREEN_OPTION
        // I assume the BGR is for the BMP file, so follow the lead of Windows.
        p[0] = p[2];
        p[2] = c;
#endif
#endif
        p+=3;
      }
    }
    fwrite(pix, width*3, 1, fp);
  }
  fclose(fp);

  if (width > 2560)
    free(pix);
}

/***************************************************************/
FILE *start_ppm(char *filename, int width, int height)
{
  char *p;
  FILE *fp;

  if ((p = strrchr(filename, '.')) != NULL)
    *p = 0;
  strcat(filename, use_uppercase ? ".PPM" : ".ppm");
  
  printf("Write PPM %s\n", filename);
  
  fp = fopen(filename, "wb");  // open in binary mode (to use unix \n chars)
  if (!fp) {
    printf("Couldn't open image file: %s\n", filename);
    return(NULL);
  }
  fprintf(fp,"P6\n");
  fprintf(fp,"# ppm-file created by %s\n", progname);
  fprintf(fp,"%i %i\n", width, height);
  fprintf(fp,"255\n"); // need unix \n here for some ppm interpreters.
  fclose(fp);
  fp = fopen(filename, "ab");  /* now append binary data */
  if (!fp) {
    printf("Couldn't append to image file: %s\n", filename);
    return(NULL);
  }

  return (fp);
}

/***************************************************************/
void write_ppm(char *filename)
{
  int i, j;

  FILE *fp;
  int width = Width;
  int height = Height;
  GLint xoff = 0;
  GLint yoff = 0;
  pix = &buf[0];
  if (width > 2560)
    pix = (char*)malloc(width*3);

  if (cropping)
  {
    xoff = max(0, z.extent_x1);
    yoff = max(0, z.extent_y1);
    width = min((z.extent_x2 - xoff), (Width - xoff));
    height = min((z.extent_y2 + 1 - yoff), (Height - yoff));
    //width = ((width + 3)/4) * 4; // round to a multiple of 4.
    if (ldraw_commandline_opts.debug_level == 1)
      printf("bmpsize = (%d, %d) at (%d, %d)\n", width, height, xoff, yoff);
    if ((width <= 0) || (height <= 0)) return;
  }

  fp = start_ppm(filename, width, height);
  if (fp == NULL)
    return;

  glReadBuffer(renderbuffer);
  // Write image rows
  //png_write_image(png_ptr, row_pointers);
  for (i = height-1; i >= 0; i--)
  {
#ifdef OSMESA_OPTION
    if (OffScreenRendering)
    {
      int j;
      char *b = (char *)OSbuffer;
      b += ((i+yoff)*Width +xoff) *4;
      for (j=0; j<width; j++) {
	pix[3*j] = b[0];
	pix[3*j+1] = b[1];
	pix[3*j+2] = b[2];
	b+=4;
      }
    }
    else
#endif
    glReadPixels(xoff, i+yoff, width, 1, GL_RGB, GL_UNSIGNED_BYTE, pix);
    fwrite(pix, width*3, 1, fp);
  }

  fclose(fp);

  if (width > 2560)
    free(pix);
}

/***************************************************************/
void
write_targa(char *filename, const GLubyte *buffer, int width, int height)
{
  char *p;
  FILE *f;

  if ((p = strrchr(filename, '.')) != NULL)
    *p = 0;
  strcat(filename, use_uppercase ? ".TGA" : ".tga");

  f = fopen( filename, "w" );
  if (f) {
    int i, x, y;
    const GLubyte *ptr = buffer;
    printf ("Write TGA %s\n", filename);
    fputc (0x00, f);	/* ID Length, 0 => No ID	*/
    fputc (0x00, f);	/* Color Map Type, 0 => No color map included	*/
    fputc (0x02, f);	/* Image Type, 2 => Uncompressed, True-color Image */
    fputc (0x00, f);	/* Next five bytes are about the color map entries */
    fputc (0x00, f);	/* 2 bytes Index, 2 bytes length, 1 byte size */
    fputc (0x00, f);
    fputc (0x00, f);
    fputc (0x00, f);
    fputc (0x00, f);	/* X-origin of Image	*/
    fputc (0x00, f);
    fputc (0x00, f);	/* Y-origin of Image	*/
    fputc (0x00, f);
    fputc (Width & 0xff, f);      /* Image Width	*/
    fputc ((Width>>8) & 0xff, f);
    fputc (Height & 0xff, f);     /* Image Height	*/
    fputc ((Height>>8) & 0xff, f);
    fputc (0x18, f);		/* Pixel Depth, 0x18 => 24 Bits	*/
    fputc (0x20, f);		/* Image Descriptor	*/
    fclose(f);
    f = fopen( filename, "ab" );  /* reopen in binary append mode */
    for (y=height-1; y>=0; y--) {
      for (x=0; x<width; x++) {
	i = (y*width + x) * 4;
	fputc(ptr[i+2], f); /* write blue */
	fputc(ptr[i+1], f); /* write green */
	fputc(ptr[i], f);   /* write red */
      }
    }
    fclose(f);
  }
  else
    printf ("Failed to write tga %s\n", filename);
}

#ifdef USE_PNG
/***************************************************************/
#include "png.h"

/***************************************************************/
static void png_error_fn(png_structp png_ptr, const char *err_msg)
{
	jmp_buf *j;
	j= (jmp_buf*) png_get_error_ptr(png_ptr);
	longjmp(*j, -1);
}

static void png_warning_fn(png_structp png_ptr, const char *warn_msg)
{
	return;
}

/***************************************************************/
FILE *start_png(char *filename, int width, int height,   
		png_structp *png_pp, png_infop *info_pp)
{
  char *p;

  png_structp png_ptr;
  png_infop info_ptr;
  jmp_buf jbuf;
  png_color_16 background;
  FILE *fp;

  if ((p = strrchr(filename, '.')) != NULL)
    *p = 0;
  strcat(filename, use_uppercase ? ".PNG" : ".png");

  printf("Write PNG %s\n", filename);
  
  // Write header stuff
  png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, (void*)(&jbuf),
				    png_error_fn, png_warning_fn);
  if (!png_ptr) 
  {
    printf("No Memory", filename);
    return(NULL);
  }

  info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) {
    png_destroy_write_struct(&png_ptr,(png_infopp)NULL);
    printf("No Memory", filename);
    return(NULL);
  }

  if(setjmp(jbuf)) {
    // we'll get here if an error occurred in any of the following
    // png_ functions
    printf("Aborting PNG file %s", filename);
    png_destroy_write_struct(&png_ptr,(png_infopp)NULL);
    if(fp) fclose(fp);
    return(NULL);
  }

  if ((fp = fopen(filename,"wb+"))==NULL) {
    printf("Could not open %s\n", filename);
    return(NULL);
  }
  
  png_init_io(png_ptr, fp);

  if ( use_png_alpha )
  {
    // Set the default background for transparent image to white.
    // I should just store the current background color in a global and use it.
    // I should also make this a menu option/command line opt.
    // Perhaps ctl-p for the hot key.  
    // Perhaps I need to create 4 image types.  bmp24, bmp8, png, png-alpha
    // and menu/hotkey/cmdline support for cropping modifier.
    background.red = 0xffff;
    background.green = 0xffff;
    background.blue = 0xffff;

    png_set_IHDR(png_ptr, info_ptr, width, height, 8, 
		 PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE,
		 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_set_bKGD(png_ptr, info_ptr, &background);
  }
  else
  {
    png_set_IHDR(png_ptr, info_ptr, width, height, 8, 
		 PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
		 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
  }

  // png_set_bgr(png_ptr);
  // png_set_pHYs(png_ptr, info_ptr, resolution_x, resolution_y, 1);

  png_write_info(png_ptr, info_ptr);
  
  *png_pp = png_ptr;
  *info_pp = info_ptr;
  return(fp);
}

#ifdef CHEESY_2X2_AVG
/***************************************************************/
void write_png_avg(char *filename)
{
  int i, j, k;

  png_structp png_ptr;
  png_infop info_ptr;
  png_text text_ptr[1];
  FILE *fp;

  int width = Width;
  int height = Height;
  GLint xoff = 0;
  GLint yoff = 0;
  char *px, *PX;
  char *PIX = NULL;

  pix = &buf[0];
  if (width > 2560)
    pix = (char*)malloc(width*4); // Alloc rgba even if alpha not used.
  if (downsample)
    PIX = (char*)malloc(width*4); // Alloc rgba even if alpha not used.
  px = pix;

  if (cropping)
  {
    xoff = max(0, z.extent_x1);
    yoff = max(0, z.extent_y1);
    width = min((z.extent_x2 - xoff), (Width - xoff));
    height = min((z.extent_y2 + 1 - yoff), (Height - yoff));
    width = ((width + 3)/4) * 4; // round to a multiple of 4.
    if (ldraw_commandline_opts.debug_level == 1)
      printf("pngsize = (%d, %d) at (%d, %d)\n", width, height, xoff, yoff);
    if ((width <= 0) || (height <= 0)) return;
  }
  
  if (downsample)
    fp = start_png(filename, width/2, height/2, &png_ptr, &info_ptr);
  else
    fp = start_png(filename, width, height, &png_ptr, &info_ptr);
  if (fp == NULL)
    return;

  glReadBuffer(renderbuffer);
  // Write image rows
  //png_write_image(png_ptr, row_pointers);
  for (i = height-1; i >= 0; i--)
  {
   for (k = downsample; k >= 0; k--) {
#ifdef OSMESA_OPTION
    if (OffScreenRendering)
    {
      char *b = (char *)OSbuffer;
      b += ((i+yoff)*Width +xoff) *4;
      for (j=0; j<width; j++) {
	if (use_png_alpha){
	  pix[4*j] = b[0];
	  pix[4*j+1] = b[1];
	  pix[4*j+2] = b[2];
	  pix[4*j+3] = b[3];
	}
	else {
	  pix[3*j] = b[0];
	  pix[3*j+1] = b[1];
	  pix[3*j+2] = b[2];
	}
	b+=4;
      }
    }
    else
#endif
    if (use_png_alpha)
    {
      glReadPixels(xoff, i+yoff, width, 1, GL_RGBA, GL_UNSIGNED_BYTE, pix);
      char *b = pix+3;
      for (j = 0; j < width; j++)
	if (b[4*j])
	  b[4*j] = 0xff; // Convert partial alpha to opaque.
    }
    else
      glReadPixels(xoff, i+yoff, width, 1, GL_RGB, GL_UNSIGNED_BYTE, pix);

    if (k > 0) {
      if (--i < 0) // Skip this line because png header doesn't expect it.
	break; //memcpy(PIX, pix, width*4); 
      pix = PIX;
      continue;
    }  
#if 1
    // Use fast color averaging algorithm from compuphase.com/graphic/scale3.html
    if (downsample) { // Now downsample to one halfwidth row.
      unsigned int a, b, c, m; // 3 pixel values to work with and the underflow mask.
      unsigned int *p;
      m = 0xfefefefe;          // underflow = lowest bit of each color byte.
      pix = px;                // Restore pix ptr to the start of buf.
      PX = PIX;                // px = pointer to top row, PX = bottom row ptr.
      p = (unsigned int *)pix; // p = dest ptr (reuse left half of top row).
      for (j=0; j<width; j++) {
	// Average 2 pixels from one row.
	a = *(unsigned int *)px;
	px += 4;
	b = *(unsigned int *)px;
	a = (((a ^ b) & m) >> 1) + (a & b); 
	// Average 2 pixels from next row.
	c = *(unsigned int *)PX;
	PX += 4;
	b = *(unsigned int *)PX;
	c = (((c ^ b) & m) >> 1) + (c & b); 
	// Average the average pixels to squeeze 2x2 pixels to 1..
	a = (((a ^ c) & m) >> 1) + (a & c); 
	*p++ = a;
	px += 4;
	PX += 4;
      }
      px = pix; // Restore px pointer to the start of pix buf.
    }
#endif
    png_write_row(png_ptr, (unsigned char *)pix);
   }
  }

  text_ptr[0].key = "Software";
  text_ptr[0].text = "LdGLite";
  text_ptr[0].compression = PNG_TEXT_COMPRESSION_NONE;
  png_set_text(png_ptr, info_ptr, text_ptr, 1);
  
  png_write_end(png_ptr, info_ptr);
  png_destroy_write_struct(&png_ptr, &info_ptr);

  fclose(fp);

  if (width > 2560)
    free(pix);
  if (downsample) 
    free(PIX);
      
}
#endif

/***************************************************************/
#include <math.h>
// Gamma correct 8 bits into 16 bits for more precise addititon.
// Then downshift to 12 bits and inverse gamma that back to 8 bits.
double gamma_val        = 2.2;
int GAMMA[256];
unsigned char GAM2RGB[4096]; 

/***************************************************************/
void write_png(char *filename)
{
  int i, j, k;

  png_structp png_ptr;
  png_infop info_ptr;
  png_text text_ptr[1];
  FILE *fp;

  int width = Width;
  int height = Height;
  GLint xoff = 0;
  GLint yoff = 0;
  unsigned char *p, *p0, *p1, *p2;

  pix = &buf[0];
  if (downsample) {
    ZCOLOR zcp, zcs;

    /* Make gamma lookup tables now to avoid pow inside the loops */
    for(i = 0; i< 256; i++)
      GAMMA[i] = (unsigned int) (65535.0 * pow((i/255.0), gamma_val));
    for(i = 0; i< 4096; i++)
      GAM2RGB[i] = (unsigned char) (255.0 * pow((i/4095.0), (1.0 / gamma_val)));

    // Alloc rgba even if alpha not used.
    p0 = (unsigned char*)calloc(width+1, 4);
    p1 = (unsigned char*)calloc(width+1, 4);
    p2 = (unsigned char*)calloc(width+1, 4);
    // Add a topright border of 1 pixel of transparent background for filter.
    translate_color(ldraw_commandline_opts.B, &zcp, &zcs);
    k = (width+1) * (3 + use_png_alpha);
    pix = p0;
    for (j=0; j<k; j++){
      pix[j++] = zcp.r;
      pix[j++] = zcp.g;
      pix[j]   = zcp.b;
      if (use_png_alpha)
	pix[++j] = 0;
    }
    pix = (char *)p2;
    k = k - use_png_alpha;
    pix[k-3] = zcp.r;
    pix[k-2] = zcp.g;
    pix[k-1] = zcp.b;
    pix = (char *)p1;
    pix[k-3] = zcp.r;
    pix[k-2] = zcp.g;
    pix[k-1] = zcp.b;
  }
  else if (width > 2560)
    pix = (char*)malloc(width*4); // Alloc rgba even if alpha not used.

  if (cropping)
  {
    xoff = max(0, z.extent_x1);
    yoff = max(0, z.extent_y1);
    width = min((z.extent_x2 - xoff), (Width - xoff));
    height = min((z.extent_y2 + 1 - yoff), (Height - yoff));
    width = ((width + 3)/4) * 4; // round to a multiple of 4.
    if (ldraw_commandline_opts.debug_level == 1)
      printf("pngsize = (%d, %d) at (%d, %d)\n", width, height, xoff, yoff);
    if ((width <= 0) || (height <= 0)) return;
  }
  
  if (downsample)
    fp = start_png(filename, width/2, height/2, &png_ptr, &info_ptr);
  else
    fp = start_png(filename, width, height, &png_ptr, &info_ptr);
  if (fp == NULL)
    return;

  glReadBuffer(renderbuffer);
  // Write image rows
  //png_write_image(png_ptr, row_pointers);
  for (i = height-1; i >= 0; i--)
  {
   for (k = downsample; k >= 0; k--) {
#ifdef OSMESA_OPTION
    if (OffScreenRendering)
    {
      char *b = (char *)OSbuffer;
      b += ((i+yoff)*Width +xoff) *4;
      for (j=0; j<width; j++) {
	if (use_png_alpha){
	  pix[4*j] = b[0];
	  pix[4*j+1] = b[1];
	  pix[4*j+2] = b[2];
	  pix[4*j+3] = b[3];
	}
	else {
	  pix[3*j] = b[0];
	  pix[3*j+1] = b[1];
	  pix[3*j+2] = b[2];
	}
	b+=4;
      }
    }
    else
#endif
    if (use_png_alpha)
    {
      glReadPixels(xoff, i+yoff, width, 1, GL_RGBA, GL_UNSIGNED_BYTE, pix);
      char *b = pix+3;
      for (j = 0; j < width; j++)
	if (b[4*j])
	  b[4*j] = 0xff; // Convert partial alpha to opaque.
    }
    else
      glReadPixels(xoff, i+yoff, width, 1, GL_RGB, GL_UNSIGNED_BYTE, pix);

    if (k > 0) {
      if (--i < 0) // Skip this line because png header doesn't expect it.
	break; //memcpy(PIX, pix, width*4); 
      pix = (char *)p2;
      continue;
    }  

    if (downsample) { // Now downsample to one halfwidth row.
      unsigned int a, n, N, J; 
      p = p1; // p = dest ptr (reuse left half of top row).
      n = (3 + use_png_alpha);
      N = 2 * n;
      for (j=0,J=0; j<(width*n); j+=n) {
        // Use 3x3 gaussian blur filter (with gamma correction).
	// **************************************************************
	a  = GAMMA[p0[j+0]] >> 4;
	a += GAMMA[p0[j+n]] >> 3;
	a += GAMMA[p0[j+N]] >> 4;
	a += GAMMA[p1[j+0]] >> 3;
	a += GAMMA[p1[j+n]] >> 2;
	a += GAMMA[p1[j+N]] >> 3;
	a += GAMMA[p2[j+0]] >> 4;
	a += GAMMA[p2[j+n]] >> 3;
	a += GAMMA[p2[j+N]] >> 4;
	j++;
	p[J++] = GAM2RGB[(a >> 4) & 0xfff];
	a  = GAMMA[p0[j+0]] >> 4;
	a += GAMMA[p0[j+n]] >> 3;
	a += GAMMA[p0[j+N]] >> 4;
	a += GAMMA[p1[j+0]] >> 3;
	a += GAMMA[p1[j+n]] >> 2;
	a += GAMMA[p1[j+N]] >> 3;
	a += GAMMA[p2[j+0]] >> 4;
	a += GAMMA[p2[j+n]] >> 3;
	a += GAMMA[p2[j+N]] >> 4;
	j++;
	p[J++] = GAM2RGB[(a >> 4) & 0xfff];
	a  = GAMMA[p0[j+0]] >> 4;
	a += GAMMA[p0[j+n]] >> 3;
	a += GAMMA[p0[j+N]] >> 4;
	a += GAMMA[p1[j+0]] >> 3;
	a += GAMMA[p1[j+n]] >> 2;
	a += GAMMA[p1[j+N]] >> 3;
	a += GAMMA[p2[j+0]] >> 4;
	a += GAMMA[p2[j+n]] >> 3;
	a += GAMMA[p2[j+N]] >> 4;
	j++;
	p[J++] = GAM2RGB[(a >> 4) & 0xfff];
	if (use_png_alpha) {
	  a  = p0[j+0] >> 4;
	  a += p0[j+n] >> 3;
	  a += p0[j+N] >> 4;
	  a += p1[j+0] >> 3;
	  a += p1[j+n] >> 2;
	  a += p1[j+N] >> 3;
	  a += p2[j+0] >> 4;
	  a += p2[j+n] >> 3;
	  a += p2[j+N] >> 4;
	  a &= 0xff;
	  j++;
	  p[J++] = a;
	}
      }
      pix = p0; // Swap ptr to third row into first row ptr so we can reuse the row.
      p0 = p2;
      p2 = pix;
      pix = p1; // Setup to read the 2nd and 3rd rows.
    }
    png_write_row(png_ptr, (unsigned char *)pix);

   }
  }

  text_ptr[0].key = "Software";
  text_ptr[0].text = "LdGLite";
  text_ptr[0].compression = PNG_TEXT_COMPRESSION_NONE;
  png_set_text(png_ptr, info_ptr, text_ptr, 1);
  
  png_write_end(png_ptr, info_ptr);
  png_destroy_write_struct(&png_ptr, &info_ptr);

  fclose(fp);

  if (width > 2560)
    free(pix);
  if (downsample) {
    free(p0);
    free(p1);
    free(p2);
  }      
}
#endif

#ifdef USE_BMP8
/***************************************************************/
#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4)
#define BMP_PIXELSIZE(width, height, bits) (((((width) * (bits) + 31)/32) * 4) * height)
#define BMP_HEADERSIZE (3 * 2 + 4 * 12)

/***************************************************************/
// Writes a bmp to a file
// added by LZ
static BOOL SaveBMP8(char* fileName, BYTE* colormappedbuffer, UINT width, UINT height, int colors, RGBQUAD *colormap)
{
  int datasize, cmapsize, byteswritten, row, col;
  long filesize;
  int res1, res2;
  long pixeloffset;
  int bmisize;
  long cols;
  long rows;
  WORD planes;
  long compression;
  long cmpsize;
  long xscale;
  long yscale;
  long impcolors;
  FILE *fp;
  char bm[2];
  BITMAPFILEHEADER bmfh;
  BITMAPINFOHEADER bmh;
  int i;
  int pixbuf;
  int nbits = 0;
  int offset;	// offset into our color-mapped RGB buffer
  BYTE pval;
  char hdr[54];

  cmapsize = colors * 4;
  datasize = BMP_PIXELSIZE(width, height, 8);
  filesize = BMP_HEADERSIZE + cmapsize + datasize;
  res1 = res2 = 0;
  pixeloffset = BMP_HEADERSIZE + cmapsize;
  bmisize = 40;
  cols = width;
  rows = height;
  planes = 1;
  compression =0;
  cmpsize = datasize;
  xscale = 0;
  yscale = 0;
  impcolors = colors;

  fp = fopen(fileName, "wb+");
  if (fp == NULL) 
    return FALSE;
  
  bm[0]='B';
  bm[1]='M';

  // header stuff
  bmfh.bfType=*(WORD *)&bm; 
  bmfh.bfSize= filesize; 
  bmfh.bfReserved1=0; 
  bmfh.bfReserved2=0; 
  bmfh.bfOffBits=pixeloffset; 

  bmh.biSize = bmisize; 
  bmh.biWidth = cols; 
  bmh.biHeight = rows; 
  bmh.biPlanes = planes; 
  bmh.biBitCount = 8;
  bmh.biCompression = compression; 
  bmh.biSizeImage = cmpsize; 
  bmh.biXPelsPerMeter = xscale; 
  bmh.biYPelsPerMeter = yscale; 
  bmh.biClrUsed = colors;
  bmh.biClrImportant = impcolors;

#if 0	
  fwrite(&bmfh, sizeof (BITMAPFILEHEADER), 1, fp);
  fwrite(&bmh, sizeof (BITMAPINFOHEADER), 1, fp);
#else
  // Copy the 54 bytes of bmfh and bmh into hdr in intel byte order & packing.
  memset(hdr, 0, 54);
  hdr[0] = 'B';
  hdr[1] = 'M';
  hdr[2] = BYTE1(bmfh.bfSize);
  hdr[3] = BYTE2(bmfh.bfSize);
  hdr[4] = BYTE3(bmfh.bfSize);
  hdr[5] = BYTE4(bmfh.bfSize);
  hdr[10] = BYTE1(bmfh.bfOffBits);
  hdr[11] = BYTE2(bmfh.bfOffBits);
  hdr[12] = BYTE3(bmfh.bfOffBits);
  hdr[13] = BYTE4(bmfh.bfOffBits);

  hdr[14] = 40;
  hdr[15] = 0;
  hdr[16] = 0;
  hdr[17] = 0;
  hdr[18] = BYTE1(bmh.biWidth);
  hdr[19] = BYTE2(bmh.biWidth);
  hdr[20] = BYTE3(bmh.biWidth);
  hdr[21] = BYTE4(bmh.biWidth);
  hdr[22] = BYTE1(bmh.biHeight);
  hdr[23] = BYTE2(bmh.biHeight);
  hdr[24] = BYTE3(bmh.biHeight);
  hdr[25] = BYTE4(bmh.biHeight);
  hdr[26] = 1;
  hdr[27] = 0;
  hdr[28] = 8;
  hdr[29] = 0;
  hdr[46] = BYTE1(bmh.biClrUsed);
  hdr[47] = BYTE2(bmh.biClrUsed);
  hdr[48] = BYTE3(bmh.biClrUsed);
  hdr[49] = BYTE4(bmh.biClrUsed);
  hdr[50] = BYTE1(bmh.biClrImportant);
  hdr[51] = BYTE2(bmh.biClrImportant);
  hdr[52] = BYTE3(bmh.biClrImportant);
  hdr[53] = BYTE4(bmh.biClrImportant);
  // The rest are all zeros.
  fwrite(hdr, 54, 1, fp);
#endif

  if (cmapsize) 
  {
    for (i = 0; i< colors; i++) 
    {
      putc(colormap[i].rgbRed, fp);
      putc(colormap[i].rgbGreen, fp);
      putc(colormap[i].rgbBlue, fp);
      putc(0, fp);	// dummy
    }
  }

  byteswritten = BMP_HEADERSIZE + cmapsize;

  for (row = 0; row< (int)height; row++) 
  {
    nbits = 0;
    for (col =0 ; col < (int)width; col++) 
    {
      offset = row * width + col; // offset into our color-mapped RGB buffer
      pval = *(colormappedbuffer + offset);
      pixbuf = (pixbuf << 8) | pval;
      nbits += 8;
      
      if (nbits > 8) 
      {
	fclose(fp);
	return FALSE;
      }
      
      if (nbits == 8) 
      {
	putc(pixbuf, fp);
	pixbuf=0;
	nbits=0;
	byteswritten++;
      }
    } // cols
    
    if (nbits > 0) 
    {
      putc(pixbuf, fp);		// write partially filled byte
      byteswritten++;
    }
    
    // DWORD align
    while ((byteswritten -pixeloffset) & 3) {
      putc(0, fp);
      byteswritten++;
    }
    
  }	//rows
  
  fclose(fp);
  return TRUE;
}

/***************************************************************/
// color reduction (Don't remember who wrote it so I can't give proper credit)
#include "quant.h"

/***************************************************************/
void write_bmp8(char *filename)
{
  int width = Width;
  int height = Height;
  GLint xoff = 0;
  GLint yoff = 0;
  BYTE* data;
  RGBQUAD RGBpal[256];
  BYTE tmpPal[3][256];
  BYTE *colormappedBuffer;
  UINT col;
  
  if (cropping)
  {
    xoff = max(0, z.extent_x1);
    yoff = max(0, z.extent_y1);
    width = min((z.extent_x2 - xoff), (Width - xoff));
    height = min((z.extent_y2 + 1 - yoff), (Height - yoff));
    //width = ((width + 31)/32) * 32; // round to a multiple of 32.
    width = ((width + 3)/4) * 4; // round to a multiple of 4.
    if (ldraw_commandline_opts.debug_level == 1)
      printf("bmpsize = (%d, %d) at (%d, %d)\n", width, height, xoff, yoff);
    if ((width <= 0) || (height <= 0)) return;
  }
  
  colormappedBuffer = (BYTE*) malloc (width*height);
  data = (BYTE*)malloc(width*height*3);

  printf("Write BMP8 %s\n", filename);
  
  glReadBuffer(renderbuffer);
#ifdef OSMESA_OPTION
  if (OffScreenRendering)
  {
    int i;
    pix = data;
    for (i = 0; i < height; i++)
    {
      int j;
      char *b = (char *)OSbuffer;
      b += ((i+yoff)*Width +xoff) *4;
      for (j=0; j<width; j++) {
	pix[3*j] = b[0];
	pix[3*j+1] = b[1];
	pix[3*j+2] = b[2];
	b+=4;
      }
      pix += width*3;
    }
  }
  else
#endif
  glReadPixels(xoff, yoff, (GLint)width, (GLint)height, GL_RGB, GL_UNSIGNED_BYTE, data);

  dl1quant(data, colormappedBuffer, width, height, 256, TRUE, tmpPal); 

  for (col = 0; col < 256; col++) 
  {
    RGBpal[col].rgbRed=tmpPal[2][col];
    RGBpal[col].rgbGreen=tmpPal[1][col];
    RGBpal[col].rgbBlue=tmpPal[0][col];
  }

  SaveBMP8(filename, colormappedBuffer, width, height, 256, &RGBpal[0]);
  
  free (colormappedBuffer);

  free(data);
}
#endif

/***************************************************************/
// Platform dependent OffScreen rendering section.
/***************************************************************/
// This is here because its used to generate picture files.
// Probably should also move tiled rendering code here
// and merge with offscreen code so I can do offscreen tiles.
//
// OSmesa is really only useful if you increase the buffer size
// in mesa_config.h and recompile mesalib.
// Otherwise you might as well use xvfb.
/***************************************************************/
extern void DrawScene(void);
extern void platform_step_filename(int step, char *filename);
extern void getDisplayProperties();
extern void initCamera(void);
extern void init(void);
extern void reshape(int width, int height);

/***************************************************************/
int SetOffScreenRendering()
{
#ifdef OSMESA_OPTION
  return 1;
#endif
#ifdef WIN_DIB_OPTION
  return 1;
#endif
#ifdef AGL_OFFSCREEN_OPTION
  return 1;
#endif
#ifdef CGL_OFFSCREEN_OPTION
  return 1;
#endif

  return 0;
}

/***************************************************************/
int OffScreenDisplay()
{
   char filename[256];

#ifdef OSMESA_OPTION
   DrawScene();

   //platform_step_filename(curstep, filename);
   //write_targa(filename, OSbuffer, Width, Height);
#endif
#ifdef WIN_DIB_OPTION
   DrawScene();

   //platform_step_filename(curstep, filename);
#endif
#ifdef AGL_OFFSCREEN_OPTION
   DrawScene();
#endif
#ifdef CGL_OFFSCREEN_OPTION
   DrawScene();
#endif
   return 0;
}

#ifdef WIN_DIB_OPTION
//************************************************************************
int winOffScreenStart()
{
  BITMAPINFO bitmapInfo;
  LPBYTE dib_bits = NULL;         // Pointer to the bits.
  HDC hDC;
  PIXELFORMATDESCRIPTOR pfd;
  int pixelformat;

  // Initialize the bitmapInfo structure
  memset (&bitmapInfo, 0, sizeof(BITMAPINFO));

  bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  bitmapInfo.bmiHeader.biWidth = Width;
  bitmapInfo.bmiHeader.biHeight = Height;
  bitmapInfo.bmiHeader.biPlanes = 1; // Not used, but must be 1
  bitmapInfo.bmiHeader.biBitCount = 24;
  bitmapInfo.bmiHeader.biCompression = BI_RGB;
  bitmapInfo.bmiHeader.biSizeImage = ((Width*3) + 3)/4*4*Height;

  // Create the DIBSection. We can also get direct access to the
  // pixels from here
  m_dibSection = CreateDIBSection (NULL, &bitmapInfo, DIB_RGB_COLORS,
				   (void**)&dib_bits, NULL, 0);

  // We can directly write into this buffer!
  if (dib_bits == NULL)
    return (0);
  
  // Create a DC
  hDC = CreateCompatibleDC (NULL);
  if (hDC == NULL)
    return (0);
  
  // Select the DIB into the DC
  if (!SelectObject (hDC, m_dibSection))
    return (0);
  
  // and then create the OpenGL context
  memset (&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
  pfd.nVersion = 1 ;
  pfd.dwFlags = PFD_DRAW_TO_BITMAP | PFD_SUPPORT_OPENGL | PFD_SUPPORT_GDI;
  pfd.iPixelType = PFD_TYPE_RGBA ; 
#ifdef NOALPHA
  pfd.cColorBits = 24;
#else
  //pfd.cColorBits = 32;
  pfd.cColorBits = 24;
  pfd.cAlphaBits = 8;
#endif
  pfd.cDepthBits = 32;                // 32-bit z-buffer
  pfd.iLayerType = PFD_MAIN_PLANE;

  // This part of the code is the same for creating both window contexts
  // and bitmap contexts
  pixelformat = ChoosePixelFormat(hDC, (const PIXELFORMATDESCRIPTOR *) &pfd);
  // pixelformat returns a valid index of one (1)
  if (pixelformat == 0) 
    return (0);

  if (!SetPixelFormat(hDC, pixelformat, &pfd))
    return (0);

  hGLRC = wglCreateContext (hDC);
  
  wglMakeCurrent(hDC, hGLRC);

  return(1);
}
#endif

#ifdef AGL_OFFSCREEN_OPTION
//************************************************************************
static AGLContext setupAGL(void)
{
  AGLPixelFormat fmt;
  AGLContext     ctx;
  GLboolean      ok;
  GLsizei        rowbytes;
  // AGL_ALL_RENDERERS, AGL_FULLSCREEN, AGL_DOUBLEBUFFER, AGL_ACCELERATED, 
  // AGL_DEPTH_SIZE, 32,
  // Found a note on google groups that DEPTH_SIZE does not work with OFFSCREEN
  GLint          attrib[] = { AGL_RGBA, AGL_PIXEL_SIZE, 32, 
			      AGL_DEPTH_SIZE, 32,
			      AGL_OFFSCREEN, AGL_NONE };
  
  /* Allocate the image buffer */
#if 0
  // NOTE: Who allocates the depth buffer?  
  OSbuffer = malloc( Width * Height * 4 * sizeof(GLubyte) );
#else
  // Try including the depth buffer.  By the way, who allocates depth for OSMesa?
  OSbuffer = malloc( Width * Height * 8 * sizeof(GLubyte) );
#endif

  if (!OSbuffer) {
    printf("Alloc image buffer failed!\n");
    return NULL;
  }

  /* Choose an rgb pixel format */
  fmt = aglChoosePixelFormat(NULL, 0, attrib);
  if ( fmt == NULL )
    return NULL;
  
  /* Create an AGL context */
  ctx = aglCreateContext(fmt, NULL);
  if( ctx == NULL )
    return NULL;
  
  /* Attach the off screen area to the context */
  // NOTE:  Should the depth buffer bytes be included in rowbytes?
  //        Or is this why google groups suggest depth fails for OffScreen?
  rowbytes = Width * 4 * sizeof(GLubyte);
  ok = aglSetOffScreen(ctx, Width, Height, rowbytes, OSbuffer);
  if( !ok )
    return NULL;
  
  /* Make the context the current context */
  ok = aglSetCurrentContext( ctx );
  if( !ok )
    return NULL;
  
  /* The pixel format is no longer needed */
  aglDestroyPixelFormat( fmt );
  
  return ctx;
}

/***************************************************************/
void CleanupAGL( AGLContext ctx )
{
  aglSetCurrentContext( NULL );
  aglSetDrawable( ctx, NULL );
  aglDestroyContext( ctx );
}
#endif

#ifdef CGL_OFFSCREEN_OPTION
//************************************************************************
static CGLContextObj setupCGL(void)
{
  int numPixelFormats = 0; // long numPixelFormats = 0;
  CGLPixelFormatObj pixelFormatObj;
  CGLContextObj  ctx;
  CGLError       ok;
  GLsizei        rowbytes;
  CGLPixelFormatAttribute attribs[] = {
                                       kCGLPFAOffScreen,
                                       //kCGLPFAColorSize, 32,
                                       kCGLPFAColorSize, 24,
                                       kCGLPFAAlphaSize, 8,
				       kCGLPFADepthSize, 32,
                                       //kCGLRGB888A8Bit,
                                       0}; //NULL};

#if 0
   //kCGLRGB888A8Bit,
   //Specifies a format that has 8-32 bits per pixel with an ARGB channel layout, 
   //and the channels located in the following bits: A=7:0, R=23:16, G=15:8, B=7:0. 
   //NOTE: I dont get it.  In the docs it says A above is for Accumulation (not Alpha?)
   // Do I need this???
   //kCGLPFAAccumSize, 8,

   // Here's the code from Ldview.  It uses an unaccelerated pbuffer.
   // I think I only need stencil for moving parts around, but I'd better check...
   CGLPixelFormatAttribute attrs[] =
   {
   	kCGLPFADepthSize, (CGLPixelFormatAttribute)24,
   	kCGLPFAColorSize, (CGLPixelFormatAttribute)24,
   	kCGLPFAAlphaSize, (CGLPixelFormatAttribute)8,
   	kCGLPFAStencilSize, (CGLPixelFormatAttribute)8,
   	kCGLPFAAccelerated, (CGLPixelFormatAttribute)NO,
   	kCGLPFAPBuffer,
   	(CGLPixelFormatAttribute)0, // reserved for kCGLPFARemotePBuffer (if shared context)
   	(CGLPixelFormatAttribute)0
   };
#endif

  /* Allocate the image buffer */
  // NOTE:  4 bytes RGBA + 4 bytes depth per pixel?
  OSbuffer = malloc(Width * Height * 8);

  if (!OSbuffer) {
    printf("Alloc image buffer failed!\n");
    return NULL;
  }

  /* Choose an rgb pixel format */
  CGLChoosePixelFormat (attribs, &pixelFormatObj, &numPixelFormats);
  if( pixelFormatObj == NULL )
    return NULL;

  /* Create a CGL context */
  CGLCreateContext (pixelFormatObj, NULL, &ctx);
  if( ctx == NULL )
    return NULL;

  /* The pixel format is no longer needed */
  CGLDestroyPixelFormat (pixelFormatObj);

  /* Make the context the current context */
  ok = CGLSetCurrentContext (ctx);
  if ( ok != kCGLNoError )
    return NULL;

  /* Attach the off screen area to the context */
  rowbytes = Width * 4; // * sizeof(GLubyte);
  ok = CGLSetOffScreen (ctx, Width, Height, rowbytes, OSbuffer);
  if ( ok != kCGLNoError )
    return NULL;

  return ctx;
}

/***************************************************************/
void CleanupCGL( CGLContextObj ctx )
{
  CGLSetCurrentContext (NULL);
  CGLClearDrawable (ctx);
  CGLDestroyContext (ctx);
}
#endif

/***************************************************************/
int OffScreenRender()
  {
#ifdef OSMESA_OPTION
    /* Create an RGBA-mode context */
#if OSMESA_MAJOR_VERSION * 100 + OSMESA_MINOR_VERSION >= 305
   /* specify Z, stencil, accum sizes */
    ctx = OSMesaCreateContextExt( OSMESA_RGBA, 16, 0, 0, NULL );
#else
    ctx = OSMesaCreateContext( OSMESA_RGBA, NULL );
#endif

    /* Allocate the image buffer */
    OSbuffer = malloc( Width * Height * 4 * sizeof(GLubyte) );
    if (!OSbuffer) {
      printf("Alloc image buffer failed!\n");
      exit(0);
    }


    /* Bind the buffer to the context and make it current */
    if (!OSMesaMakeCurrent( ctx, OSbuffer, GL_UNSIGNED_BYTE, Width, Height )) {
      printf("OSMesaMakeCurrent failed!\n");
      exit(0);
    }
#endif
#ifdef WIN_DIB_OPTION
    if (!winOffScreenStart())
    {
      printf("Problem with DIB\n");
      exit (0);
    }
#endif
#ifdef AGL_OFFSCREEN_OPTION
    ctx = setupAGL();
    if (ctx == NULL) {
      printf("setupAGL failed!\n");
      exit(0);
    }
#endif
#ifdef CGL_OFFSCREEN_OPTION
    ctx = setupCGL();
    if (ctx == NULL) {
      printf("setupCGL failed!\n");
      exit(0);
    }
#endif

    getDisplayProperties();

    initCamera();
    init();

    reshape(Width, Height);

    // Set the background to white like ldlite.
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // NOTE:  Use display() instead when I add offscreen tiled rendering.
    OffScreenDisplay();

#ifdef OSMESA_OPTION
   /* free the image buffer */
   free( OSbuffer );

   /* destroy the context */
   OSMesaDestroyContext( ctx );
#endif
#ifdef WIN_DIB_OPTION
    DeleteObject(m_dibSection);
    
    wglMakeCurrent(NULL,NULL);
    
    wglDeleteContext(hGLRC);

#endif
#ifdef AGL_OFFSCREEN_OPTION
   /* free the image buffer */
   free( OSbuffer );

   CleanupAGL( ctx );
#endif
#ifdef CGL_OFFSCREEN_OPTION
   /* free the image buffer */
   free( OSbuffer );

   CleanupCGL( ctx );
#endif

    return 0;
  }
