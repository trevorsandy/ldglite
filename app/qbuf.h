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
/***********************************************************************/
#ifndef QBUF_H
#define QBUF_H

/*
 * Allocation policy determines in what order unused buffers
 * are allocated.
*/
#define QBUF_ALLOCATION_MASK 0x3
#define QBUF_LIFO_ALLOCATION_POLICY 0x1
#define QBUF_FIFO_ALLOCATION_POLICY 0x2


typedef struct qbuf_struct {
  char *base_addr;
  long num_buffers;
  long buffer_size;
  long *index_p;
  long index_head;
  long index_tail;
  long policy;
  long pad;
} QBUF_ID;

QBUF_ID *qbufCreate(long buf_num, long size, long policy);
void qbufDestroy(QBUF_ID *qbuf_id);
char *qbufGetPtr(QBUF_ID *qbuf_id);
void qbufReleasePtr(QBUF_ID *qbuf_id, char *ptr);

#endif
