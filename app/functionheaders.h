#ifndef FUNCTIONHEADERS_H
#define FUNCTIONHEADERS_H

#include "ldliteVR.h"

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

int access( char const * const filename, int const);
int NukeSavedDepthBuffer(void);
void zcolor_init();
int zReset(long *, long *);
void InitViewMatrix(void);
void znamelist_push();
void znamelist_pop();
int DrawCurPart(int Color);
int Hose1Part(int partnum, int steps);
int registerGlutCallbacks();
void transform_multiply(vector3d *t1,matrix3d *m1,
                   vector3d *t2, matrix3d *m2,
                   vector3d **t3, matrix3d  **m3);
int print_transform(vector3d *t,matrix3d *m);
void rotate_model();

#ifdef	__cplusplus
}
#endif /* __cplusplus */
#endif // FUNCTIONHEADERS_H
