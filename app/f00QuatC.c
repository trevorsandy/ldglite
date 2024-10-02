// This originally came from f00Dave's camera demos.
// I converted it from the nice C++ OOP style (sorry)
// to this mess because the MingW32 version of gcc makes
// win32 exes 1/3 the size of those created by g++.
// Someday I'll re-OOP it when compiler bloat is reduced...

#if __BORLANDC__ >= 0x0530
    #include <windows.h>
    #pragma hdrstop
#endif


// Primary include.
#include "f00QuatC.h"

// System includes.
#include <math.h>
//#include <assert.h>

#if 0
// Numerous ways to initialize the quaternion
f00Quat_f00Quat()
{
    reset();
}

f00Quat_f00Quat( const f00Quat& copyFrom )
{
    set( copyFrom );
}

f00Quat_f00Quat( GLfloat x, GLfloat y, GLfloat z )
{
    set( x, y, z );
}

f00Quat_f00Quat( GLfloat angle, GLfloat x, GLfloat y, GLfloat z )
{
    set( angle, x, y, z );
}
#endif

f00Quat *
f00Quat_reset(f00Quat *thisQ)
{
  // The multiplication 'identity' quaternion.  Why not?  :-)
  thisQ->d_val[ 0 ] = 1.0;
  thisQ->d_val[ 1 ] = 0.0;
  thisQ->d_val[ 2 ] = 0.0;
  thisQ->d_val[ 3 ] = 0.0;

  return thisQ;
}


f00Quat *
f00Quat_copy( f00Quat *thisQ, f00Quat *copyFrom )
{
  thisQ->d_val[ 0 ] = copyFrom->d_val[ 0 ];
  thisQ->d_val[ 1 ] = copyFrom->d_val[ 1 ];
  thisQ->d_val[ 2 ] = copyFrom->d_val[ 2 ];
  thisQ->d_val[ 3 ] = copyFrom->d_val[ 3 ];

  return thisQ;
}


f00Quat *
f00Quat_set_3( f00Quat *thisQ, GLfloat x, GLfloat y, GLfloat z )
{
  f00Quat xQp;
  f00Quat yQp;
  f00Quat zQp;
  f00Quat *xQ = &xQp;
  f00Quat *yQ = &yQp;
  f00Quat *zQ = &zQp;

  f00Quat_reset(xQ);
  f00Quat_reset(yQ);
  f00Quat_reset(zQ);

  f00Quat_set(xQ, x, 1.0, 0.0, 0.0 );
  f00Quat_set(yQ, y, 0.0, 1.0, 0.0 );
  f00Quat_set(zQ, z, 0.0, 0.0, 1.0 );

  f00Quat_copy( thisQ, xQ );
  f00Quat_postMult( thisQ, yQ );
  f00Quat_postMult( thisQ, zQ );

  return thisQ;
}


f00Quat *
f00Quat_set( f00Quat *thisQ, GLfloat angle, GLfloat x, GLfloat y, GLfloat z )
{
  GLfloat factor;
  GLfloat scaleBy;
  GLfloat sinHalfAngle;

  // Convert to radians.
  angle *= ( M_PI / 180.0 );

  // Normalize the axis.
  factor = x * x + y * y + z * z;
  if (factor == 0) // assert( factor != 0 );
    return thisQ;

  scaleBy = ( 1.0 / sqrt( factor ) );
  x = x * scaleBy;
  y = y * scaleBy;
  z = z * scaleBy;

  // Build a quaternion!
  thisQ->d_val[ 0 ] = cos( angle / 2.0 );

  sinHalfAngle = sin( angle / 2.0 );
  thisQ->d_val[ 1 ] = x * sinHalfAngle;
  thisQ->d_val[ 2 ] = y * sinHalfAngle;
  thisQ->d_val[ 3 ] = z * sinHalfAngle;

  return thisQ;
}


f00Quat *
f00Quat_postMult( f00Quat *thisQ, f00Quat *quat )
{
  f00Quat tempQp;
  f00Quat *tempQ = &tempQp;
  f00Quat_copy(tempQ, thisQ);

  f00Quat_multAndSet( thisQ, tempQ, quat );
  return thisQ;
}

// This special case gets done often so I do it all here.
f00Quat *
f00Quat_postMult_3( f00Quat *thisQ, GLfloat x, GLfloat y, GLfloat z )
{
  f00Quat quatp;
  f00Quat *quat = &quatp;
  f00Quat tempQp;
  f00Quat *tempQ = &tempQp;

  f00Quat_set_3( quat, x, y, z );
  f00Quat_copy(tempQ, thisQ);
  f00Quat_multAndSet( thisQ, tempQ, quat );

  return thisQ;
}

f00Quat *
f00Quat_multAndSet( f00Quat *thisQ, f00Quat *quat1, f00Quat *quat2 )
{
  thisQ->d_val[ 0 ] =     quat2->d_val[ 0 ] * quat1->d_val[ 0 ]
    - quat2->d_val[ 1 ] * quat1->d_val[ 1 ]
    - quat2->d_val[ 2 ] * quat1->d_val[ 2 ]
    - quat2->d_val[ 3 ] * quat1->d_val[ 3 ];

  thisQ->d_val[ 1 ] =     quat2->d_val[ 0 ] * quat1->d_val[ 1 ]
    + quat2->d_val[ 1 ] * quat1->d_val[ 0 ]
    + quat2->d_val[ 2 ] * quat1->d_val[ 3 ]
    - quat2->d_val[ 3 ] * quat1->d_val[ 2 ];

  thisQ->d_val[ 2 ] =     quat2->d_val[ 0 ] * quat1->d_val[ 2 ]
    - quat2->d_val[ 1 ] * quat1->d_val[ 3 ]
    + quat2->d_val[ 2 ] * quat1->d_val[ 0 ]
    + quat2->d_val[ 3 ] * quat1->d_val[ 1 ];

  thisQ->d_val[ 3 ] =     quat2->d_val[ 0 ] * quat1->d_val[ 3 ]
    + quat2->d_val[ 1 ] * quat1->d_val[ 2 ]
    - quat2->d_val[ 2 ] * quat1->d_val[ 1 ]
    + quat2->d_val[ 3 ] * quat1->d_val[ 0 ];

  return thisQ;
}


f00Quat *
f00Quat_normalize(f00Quat *thisQ)
{
  GLfloat scaleBy;
  GLfloat factor =  thisQ->d_val[ 0 ] * thisQ->d_val[ 0 ] +
    thisQ->d_val[ 1 ] * thisQ->d_val[ 1 ] +
    thisQ->d_val[ 2 ] * thisQ->d_val[ 2 ] +
    thisQ->d_val[ 3 ] * thisQ->d_val[ 3 ];
  if (factor == 0) // assert( factor != 0 );
    return thisQ;

  scaleBy = ( 1.0 / sqrt( factor ) );

  thisQ->d_val[ 0 ] = thisQ->d_val[ 0 ] * scaleBy;
  thisQ->d_val[ 1 ] = thisQ->d_val[ 1 ] * scaleBy;
  thisQ->d_val[ 2 ] = thisQ->d_val[ 2 ] * scaleBy;
  thisQ->d_val[ 3 ] = thisQ->d_val[ 3 ] * scaleBy;

  return thisQ;
}


void
f00Quat_getMatrix(f00Quat *thisQ, GLfloat matrix[ 16 ] )
{
  GLfloat w;
  GLfloat x;
  GLfloat y;
  GLfloat z;

  GLfloat xx;
  GLfloat yy;
  GLfloat zz;

  f00Quat_normalize(thisQ);

  w = thisQ->d_val[ 0 ];
  x = thisQ->d_val[ 1 ];
  y = thisQ->d_val[ 2 ];
  z = thisQ->d_val[ 3 ];

  xx = x * x;
  yy = y * y;
  zz = z * z;

#define M( x, y )  matrix[ x + y * 4 ]

  M( 0, 0 ) = 1.0 - 2.0 * ( yy + zz );
  M( 1, 0 ) = 2.0 * ( x * y + w * z );
  M( 2, 0 ) = 2.0 * ( x * z - w * y );
  M( 3, 0 ) = 0.0;

  M( 0, 1 ) = 2.0 * ( x * y - w * z );
  M( 1, 1 ) = 1.0 - 2.0 * ( xx + zz );
  M( 2, 1 ) = 2.0 * ( y * z + w * x );
  M( 3, 1 ) = 0.0;

  M( 0, 2 ) = 2.0 * ( x * z + w * y );
  M( 1, 2 ) = 2.0 * ( y * z - w * x );
  M( 2, 2 ) = 1.0 - 2.0 * ( xx + yy );
  M( 3, 2 ) = 0.0;

  M( 0, 3 ) = 0.0;
  M( 1, 3 ) = 0.0;
  M( 2, 3 ) = 0.0;
  M( 3, 3 ) = 1.0;

//#undef M( x, y )
#undef M
}


void
f00Quat_getAxisAngle( f00Quat *thisQ, GLfloat *axisX, GLfloat *axisY, GLfloat *axisZ, GLfloat *rotAngle )
{
#define DEF_EPSILON         0.0001
  GLfloat lenOfVector2 = ( thisQ->d_val[ 1 ] * thisQ->d_val[ 1 ] + thisQ->d_val[ 2 ] * thisQ->d_val[ 2 ] + thisQ->d_val[ 3 ] * thisQ->d_val[ 3 ] );
  if( lenOfVector2 < DEF_EPSILON )
  {
    *axisX = 1.0;
    *axisY = 0.0;
    *axisZ = 0.0;
    *rotAngle = 0.0;
  }
  else
  {
    GLfloat invLen = ( 1.0 / sqrt( lenOfVector2 ) );
    *axisX = thisQ->d_val[ 1 ] * invLen;
    *axisY = thisQ->d_val[ 2 ] * invLen;
    *axisZ = thisQ->d_val[ 3 ] * invLen;
    *rotAngle = 2.0 * acos( thisQ->d_val[ 0 ] );
  }
}


#include <stdio.h>
void
f00Quat_getLatLong(f00Quat *thisQ, GLfloat *latAngle, GLfloat *longAngle )
{
  GLfloat dirX;
  GLfloat dirY;
  GLfloat dirZ;
  f00Quat_getDirectionVector(thisQ, &dirX, &dirY, &dirZ );

#define DEF_EPSILON 0.0001
  *latAngle = asin( dirY ) * ( 180.0 / M_PI );
  if( fabs( dirY ) > ( 1.0 - DEF_EPSILON ) )
  {
    // Pointing straight up or down, the longitude makes no difference.
    *longAngle = 0.0;
  }
  else
  {
    if( fabs( dirZ ) < DEF_EPSILON )
      *longAngle = ( dirX > 0.0 ? 90.0 : 270.0 );
    else
    {
      if( dirZ >= 0.0 )
    *longAngle = atan( dirX / dirZ ) * ( 180.0 / M_PI );
      else
    *longAngle = 180.0 + atan( dirX / dirZ ) * ( 180.0 / M_PI );

      if( *longAngle < 0.0 )
    *longAngle += 360.0;
    }
  }
}


void
f00Quat_getDirectionVector(f00Quat *thisQ, GLfloat *dirX, GLfloat *dirY, GLfloat *dirZ )
{
  GLfloat w;
  GLfloat x;
  GLfloat y;
  GLfloat z;

  f00Quat_normalize(thisQ);

  w = thisQ->d_val[ 0 ];
  x = thisQ->d_val[ 1 ];
  y = thisQ->d_val[ 2 ];
  z = thisQ->d_val[ 3 ];

  // Optimized out of the 'getMatrix' function, above.
  *dirX = 2.0 * ( x * z - w * y );
  *dirY = 2.0 * ( y * z + w * x );
  *dirZ = 1.0 - 2.0 * ( x * x + y * y );
}



