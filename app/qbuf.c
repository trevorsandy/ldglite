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
// Buffer management routines to replace malloc calls.

#include "stdio.h"
#include "stdlib.h"
#include "qbuf.h"
#include "ldliteVR.h"

#define PRINT_FATAL(x) {printf(x); exit(1);}
#define PRINT_STAT(x)  printf(x)

// buf_size must be a mulitple of 8 bytes
QBUF_ID *qbufCreate(long buf_num, long buf_size, long policy)
{
  QBUF_ID *qbuf_id_tmp;
  static long init=0;
  long i;

  if ((policy & QBUF_ALLOCATION_MASK) != QBUF_FIFO_ALLOCATION_POLICY) {
    PRINT_FATAL("Allocation policies other than a first-in first-out queue have not been implemented yet.\n");
  }

  qbuf_id_tmp = malloc(sizeof(QBUF_ID));

  if (qbuf_id_tmp == NULL) {
    PRINT_FATAL("Error: qbuf_id malloc failed");
  }

  qbuf_id_tmp->base_addr = malloc(buf_num*buf_size);
  if (qbuf_id_tmp->base_addr == NULL) {
    PRINT_FATAL("Error: qbuf array malloc failed");
  }
  qbuf_id_tmp->num_buffers = buf_num;
  qbuf_id_tmp->buffer_size = buf_size;
  qbuf_id_tmp->policy = policy;

  qbuf_id_tmp->index_p = malloc((buf_num+1)*sizeof(long));
  if (qbuf_id_tmp->index_p == NULL) {
    PRINT_FATAL("Error: qbuf index malloc failed");
  }
  qbuf_id_tmp->index_head=0;
  qbuf_id_tmp->index_tail=buf_num;
  for(i=0; i<buf_num; i++) {
    (qbuf_id_tmp->index_p)[i] = (long)(qbuf_id_tmp->base_addr + i * buf_size);
  }
  (qbuf_id_tmp->index_p)[buf_num] = -1;

  return qbuf_id_tmp;
}

void qbufDestroy(QBUF_ID *qbuf_id)
{
  printf("qbufDestroy not yet implemented.\n");
}

// returns 1 if ptr was allocated from qbuf_id,
// returns 0 if not.
int qbufBelongs(QBUF_ID *qbuf_id, char *ptr)
{
    if ((ptr >= qbuf_id->base_addr) &&
        (ptr < (qbuf_id->base_addr + qbuf_id->num_buffers*qbuf_id->buffer_size))) {
        return 1;
    } else {
        return 0;
    }
}

char *qbufGetPtr(QBUF_ID *qbuf_id)
{
  char *next_free_ptr;

  if (qbuf_id->index_head == qbuf_id->index_tail) {
    // use standard malloc as a fallback
    next_free_ptr = (char *)malloc(qbuf_id->buffer_size);
#if 0 // turn off, since decision to stop caching should be made elsewhere.
    {
        char buf[256];
        extern void stop_caching();

        if (ldraw_commandline_opts.debug_level > 0) {
            sprintf(buf,"qbuf %08x is out of memory",qbuf_id);
            zWrite(buf);
        }
        stop_caching();
    }
#endif
  } else {
    next_free_ptr = (char *)(qbuf_id->index_p[qbuf_id->index_head]);
    qbuf_id->index_head = (qbuf_id->index_head+1) % (qbuf_id->num_buffers+1);
  }
  return next_free_ptr;
}

void qbufReleasePtr(QBUF_ID *qbuf_id, char *ptr)
{
    if (qbufBelongs(qbuf_id, ptr) == 1) {
        qbuf_id->index_p[qbuf_id->index_tail] = (long)(ptr);
        qbuf_id->index_tail = (qbuf_id->index_tail+1) % (qbuf_id->num_buffers+1);
    } else {
        // must have been allocated with malloc, so free
        free(ptr);
    }
}
