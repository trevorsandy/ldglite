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
#include "stdio.h"
#include "stdlib.h"
#include "ldliteVR.h"
#include "math.h"
#include "string.h"

char *part_name = NULL; 
vector3d flip_scale;
extern FILE *yyin;
FILE *output_file;
FILE *log_output_file;
char output_file_name[256];
int yyparse();

LDRAW_COMMANDLINE_OPTS ldraw_commandline_opts;

#ifdef USE_QBUF_MALLOC
QBUF_ID *vector_pool;
QBUF_ID *matrix_pool;
QBUF_ID *word_pool;
#endif

/*
 * Allocate space for a string, copy string into space.
 */
char * strsave(s)
     char *s;
{
  char *tmp;
  
  if (s == (char *)NULL) {
	return (char *)NULL;
  }
#ifdef USE_QBUF_MALLOC
  tmp = (char *)qbufGetPtr(word_pool);
#else
  tmp = malloc(128);
#endif
  (void)strcpy(tmp, s);
  return tmp;
}

/*
 * m3 = m2 * m1;
 */
void transform_multiply(vector3d *t1,matrix3d *m1,
				   vector3d *t2, matrix3d *m2,
				   vector3d **t3, matrix3d  **m3)
{
	vector3d *newt;
	matrix3d *newm;

  newm = (matrix3d *)malloc(sizeof(matrix3d));
  newt = (vector3d *)malloc(sizeof(vector3d));
  newm->a = m2->a * m1->a + m2->b * m1->d + m2->c * m1->g;
  newm->b = m2->a * m1->b + m2->b * m1->e + m2->c * m1->h;
  newm->c = m2->a * m1->c + m2->b * m1->f + m2->c * m1->i;
  newm->d = m2->d * m1->a + m2->e * m1->d + m2->f * m1->g;
  newm->e = m2->d * m1->b + m2->e * m1->e + m2->f * m1->h;
  newm->f = m2->d * m1->c + m2->e * m1->f + m2->f * m1->i;
  newm->g = m2->g * m1->a + m2->h * m1->d + m2->i * m1->g;
  newm->h = m2->g * m1->b + m2->h * m1->e + m2->i * m1->h;
  newm->i = m2->g * m1->c + m2->h * m1->f + m2->i * m1->i;
  newt->x = m2->a * t1->x + m2->b * t1->y + m2->c * t1->z + t2->x;
  newt->y = m2->d * t1->x + m2->e * t1->y + m2->f * t1->z + t2->y;
  newt->z = m2->g * t1->x + m2->h * t1->y + m2->i * t1->z + t2->z;
  *m3 = newm;
  *t3 = newt;
}

void rotate_model()
{
	// modify command line option A matrix
	matrix3d m_rot;
	vector3d v_dummy;
	matrix3d *m_temp;
	vector3d *v_temp;
	double angle=10.0*3.1415927/180.0;

	v_dummy.x = 0;
	v_dummy.y = 0;
	v_dummy.z = 0;

	m_rot.a = (float)cos(angle);
	m_rot.b = 0;
	m_rot.c = (float)sin(angle);
	m_rot.d = 0;
	m_rot.e = 1;
	m_rot.f = 0;
	m_rot.g = (float)(-1.0*sin(angle));
	m_rot.h = 0;
	m_rot.i = (float)cos(angle);
    transform_multiply(&v_dummy,&m_rot,
				   &v_dummy, &(ldraw_commandline_opts.A),
				   &v_temp, &m_temp);
	ldraw_commandline_opts.A = *m_temp;
	free(m_temp);
	free(v_temp);
}

void ldlite_parse(char *ldraw_lines)
{
  vector3d *v1;
  matrix3d *m1;
  static int init=0;

  if (!init) {
	  init = 1;
#ifdef USE_QBUF_MALLOC
	    vector_pool = qbufCreate(32*1024,sizeof(vector3d),
			QBUF_FIFO_ALLOCATION_POLICY);
	    matrix_pool = qbufCreate(32*1024,sizeof(matrix3d),
			QBUF_FIFO_ALLOCATION_POLICY);
	    word_pool = qbufCreate(32*1024,128,
			QBUF_FIFO_ALLOCATION_POLICY);
#endif
  } 
  include_stack_ptr = 0;
  transform_stack_ptr=0;
  stepcount = 0;
  current_color[include_stack_ptr] = ldraw_commandline_opts.C;
#ifdef USE_OPENGL
  // put the center at the origin for opengl.
  get_opengl_transforms();

  // v1 = savevec(0.0, 0.0, 0.0);
#endif
  if (ldraw_commandline_opts.output == 1) {
	  v1 = savevec(0.0,
		  0.0,
		  0.0);
	  m1 = savemat(
		  1.0, 
		  0.0,
		  0.0,
		  0.0, 
		  1.0, 
		  0.0,
		  0.0, 
		  0.0, 
		  1.0);
  } else {
	  if (ldraw_commandline_opts.center_at_origin==0)
	  {
		  v1 = savevec(ldraw_commandline_opts.O.x+(zGetRowsize()/2),
			  ldraw_commandline_opts.O.y+(2*zGetColsize()/3),
			  ldraw_commandline_opts.O.z+(float)0.0);
	  }
	  else
	  {
		  v1 = savevec(ldraw_commandline_opts.O.x+(zGetRowsize()/2),
			  ldraw_commandline_opts.O.y+(zGetColsize()/2),
			  ldraw_commandline_opts.O.z+(float)0.0);
	  }
#if defined(USE_OPENGL) && defined(USE_GL_TWIRL)
  m1 = savemat(1.0,0.0,0.0,0.0,1.0,0.0,0.0,0.0,1.0);
#else
  m1 = savemat(
    ldraw_commandline_opts.S * ldraw_commandline_opts.A.a, 
	ldraw_commandline_opts.S * ldraw_commandline_opts.A.b,
	ldraw_commandline_opts.S * ldraw_commandline_opts.A.c,
    ldraw_commandline_opts.S * ldraw_commandline_opts.A.d, 
	ldraw_commandline_opts.S * ldraw_commandline_opts.A.e, 
	ldraw_commandline_opts.S * ldraw_commandline_opts.A.f,
    ldraw_commandline_opts.S * ldraw_commandline_opts.A.g, 
	ldraw_commandline_opts.S * ldraw_commandline_opts.A.h, 
	ldraw_commandline_opts.S * ldraw_commandline_opts.A.i);
#endif
  }

  current_translation[transform_stack_ptr] = v1;
  current_transform[transform_stack_ptr] = m1;

  if(ldraw_commandline_opts.log_output) {
	  log_output_file = fopen("ldlite.log","w+");
  } else {
	  log_output_file = NULL;
  }
  if (ldraw_commandline_opts.output != 0) {
	  if ((output_file = fopen(output_file_name,"w+"))==NULL) {
		  ldraw_commandline_opts.output = 0;
	  }
  }
  {
	  void *buffer_state;
	  void * yy_scan_string(char *ldraw_lines);
	  void yy_delete_buffer(void *buffer_state);
	  
	  yyin=NULL;
	  buffer_state = yy_scan_string(ldraw_lines);
	  yyparse();
	  yy_delete_buffer(buffer_state);
  }
  if (output_file != NULL) {
	  fclose(output_file);
	  output_file = NULL;
  }
  if (log_output_file != NULL) {
	  fclose(log_output_file);
	  log_output_file = NULL;
  }
}



