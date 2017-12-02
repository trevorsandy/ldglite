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
 * glwinkit.h - isolates the opengl windowing framework toolkit.
 ************************************************************************/

// I guess the plan here is to eventually wean ldglite off of GLUT.
// Although it's everywhere, GLUT is apparently no longer supported.
//
// SDL is probably the best choice.  It seems to be available 
// everywhere GLUT is, but needs a few extra libs (font, gui) to
// cover everything ldglite gets from glut.
//
// Freeglut shows promise, but is incomplete and only on Windows and X.

/************************************************************************/

#ifndef GLWINKIT_H
#define GLWINKIT_H

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

#ifdef USING_SLUT
#  include "slut.h" // A sleazy implementation of GLUT on SDL.
#else
#  ifdef USING_FREEGLUT
#    include <GL/freeglut.h> // Free implementation of GLUT.
#  else
#    include <GL/glut.h> // The original GLUT.
#  endif
#endif

// Define our generic menu and keyboard event handler fn.
void menuKeyEvent(int key, int x, int y);

// Define some events.  Put them above the ASCII range of 128 or 256.
#define UI_ESCAPE_EVENT (512 + 27)

#endif //GLWINKIT_H
