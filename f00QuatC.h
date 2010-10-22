// This originally came from f00Dave's camera demos.
// I converted it from the nice C++ OOP style (sorry)
// to this mess because the MingW32 version of gcc makes 
// win32 exes 1/3 the size of those created by g++.
// Someday I'll re-OOP it when compiler bloat is reduced...

#ifndef f00QuatCH
#define f00QuatCH

// System includes.
#include "glwinkit.h" //#include <GL/glut.h>
#include "platform.h"

#ifndef WINDOWS
// This stuff gets pulled in by glut.h for windows.
#include "wstubs.h"
#else
// glut 3.7 no longer includes windows.h
#if (GLUT_XLIB_IMPLEMENTATION >= 13)
#include <windows.h>
#endif
#endif

#ifndef M_PI
  // M_PI work-around, required for MS's and some *n?x's compilers.
  // (No, "*n?x" is not a typo, it's a regex ... think about it ;-)
  #ifdef PI
    #define M_PI					PI
  #else
    #define M_PI					3.141592653
  #endif
#endif

typedef struct f00Quat_tag
{
  GLfloat d_val[ 4 ];
} f00Quat;

f00Quat *f00Quat_reset(f00Quat *thisQ);
f00Quat *f00Quat_copy( f00Quat *thisQ, f00Quat *copyFrom );
f00Quat *f00Quat_set_3( f00Quat *thisQ, GLfloat x, GLfloat y, GLfloat z );
f00Quat *f00Quat_set( f00Quat *thisQ, GLfloat angle, GLfloat x, GLfloat y, GLfloat z );
f00Quat *f00Quat_postMult( f00Quat *thisQ, f00Quat *quat );
f00Quat *f00Quat_postMult_3( f00Quat *thisQ, GLfloat x, GLfloat y, GLfloat z );
f00Quat *f00Quat_multAndSet( f00Quat *thisQ, f00Quat *quat1, f00Quat *quat2 );
f00Quat *f00Quat_normalize(f00Quat *thisQ);
void f00Quat_getMatrix(f00Quat *thisQ, GLfloat matrix[ 16 ] );
void f00Quat_getAxisAngle( f00Quat *thisQ, GLfloat *axisX, GLfloat *axisY, GLfloat *axisZ, GLfloat *rotAngle );
void f00Quat_getLatLong(f00Quat *thisQ, GLfloat *latAngle, GLfloat *longAngle );
void f00Quat_getDirectionVector(f00Quat *thisQ, GLfloat *dirX, GLfloat *dirY, GLfloat *dirZ );

#endif

