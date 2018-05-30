/* Copyright 2010-14 JP Norair
  *
  * Licensed under the OpenTag License, Version 1.0 (the "License");
  * you may not use this file except in compliance with the License.
  * You may obtain a copy of the License at
  *
  * http://www.indigresso.com/wiki/doku.php?id=opentag:license_1_0
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  */
/**
  * @file       /platform/c2000/otlib_queue.c
  * @author     JP Norair
  * @version    R103
  * @date       20 Sept 2014
  * @brief      A module and ADT for buffering data packets.
  * @ingroup    ot_queue
  *
  * This is a custom version of the queue module designed to work with the 
  * TI C2000 devices, which cannot access 8 bit bytes, only 16 bit bytes.
  *
  ******************************************************************************
  */


#include <otstd.h>
#include <platform/config.h>
#include <otlib/delay.h>
#include <otlib/queue.h>
#include <otlib/memcpy.h>

#include <otsys/veelite.h>



/** @todo Add blocking conditions to all queue operations that move cursors. 
  *       Also make succeed/fail return values for all these operations.
  */



/// Queue Options flags
#define GETCURSOR_SHIFT(Q)  ((Q)->options.ushort & 1) 
#define PUTCURSOR_SHIFT(Q)  (((Q)->options.ushort >> 1) & 1)
#define FRONT_SHIFT(Q)      (((Q)->options.ushort >> 2) & 1) 
#define BACK_SHIFT(Q)       (((Q)->options.ushort >> 3) & 1) 






/** Queue "Object" functions
  * ========================
  */

void q_init(ot_queue* q, void* buffer, ot_u16 alloc) {
    q->alloc    = alloc;
    q->front    = buffer;
    q->back     = alloc;    
    q_empty(q);
}


void q_rebase(ot_queue* q, void* buffer) {
    q->front        = buffer;
    q->getcursor    = 0;
    q->putcursor    = 0;
    q->back         = 0;
}


void q_copy(ot_queue* q1, ot_queue* q2) {
    ot_memcpy_2((ot_u8*)q1, (ot_u8*)q2, sizeof(ot_queue));
}






/** Queue "Intrinsics"
  * ==================
  */

OT_INLINE ot_u8 q_getcursor_val(ot_queue* q, ot_int offset) {
    return __byte(q->front, q->getcursor + offset);
}

OT_INLINE ot_u8 q_putcursor_val(ot_queue* q, ot_int offset) {
    return __byte(q->front, q->getcursor + offset);
}

OT_INLINE ot_qcur q_offset(ot_queue* q, ot_int offset) {
    return offset;
}




/** Queue Info functions
  * ====================
  */

ot_int q_length(ot_queue* q) {
    return (ot_int)q->putcursor;
}


ot_int q_span(ot_queue* q) {
    return (ot_int)(q->putcursor - q->getcursor);
}


ot_int q_writespace(ot_queue* q) {
    return (q->back - q->putcursor);
}


ot_int q_readspace(ot_queue* q) {
    return (q->back - q->getcursor);
}






/** Queue Threading/Blocking functions
  * ==================================
  * Block & Lock not supported on C2000.  This doesn't have any affect on 
  * libotfs, these features only used in OpenTag.
  */

OT_INLINE ot_uint q_blocktime(ot_queue* q) {
}

OT_INLINE void q_blockwrite(ot_queue* q, ot_uint blocktime) {
}

void q_lock(ot_queue* q) {
}

void q_unlock(ot_queue* q) {
}




void q_empty(ot_queue* q) {
    q->options.ushort   = 0;
    q->back             = q->alloc;
    q->putcursor        = 0;
    q->getcursor        = 0;
}


void q_rewind(ot_queue* q) {
    ot_int span = q_span(q);

    if (span > 0) {
        q->putcursor = 0;
        while (span > 0) {
            length--;
            q_writebyte(q, q_readbyte(q));
        }
        q->getcursor = 0;
    }
}


void* q_start(ot_queue* q, ot_uint offset, ot_u16 options) {
    q_empty(q);

    if (offset >= q->alloc)
        return NULL;
    
    q->options.ushort  = options;
    q->putcursor      += offset;
    q->getcursor      += offset;
    
    return q->front + (offset>>1);
}


ot_qcur q_markbyte(ot_queue* q, ot_int shift) {
    ot_qcur output;
    output          = q->getcursor;
    q->getcursor   += shift;
    return output;
}


void q_writebyte(ot_queue* q, ot_u8 byte_in) {
    __byte(q->front, q->putcursor) = byte_in;
    q->putcursor++;
}


void q_writeshort(ot_queue* q, ot_uint short_in) {
    q_writebyte(q, (short_in >> 8));
    q_writebyte(q, (short_in & 0xff));
}


void q_writeshort_be(ot_queue* q, ot_uint short_in) {
    q_writebyte(q, (short_in & 0xFF));
    q_writebyte(q, (short_in >> 8));
}



void q_writelong(ot_queue* q, ot_ulong long_in) {
    q_writebyte(q, (long_in >> 24));
    q_writebyte(q, (long_in >> 16) & 0xff);
    q_writebyte(q, (long_in >> 8) & 0xff);
    q_writebyte(q, (long_in >> 0) & 0xff);
}

void q_writelong_be(ot_queue* q, ot_ulong long_in) {
    q_writebyte(q, (long_in >> 0) & 0xff);
    q_writebyte(q, (long_in >> 8) & 0xff);
    q_writebyte(q, (long_in >> 16) & 0xff);
    q_writebyte(q, (long_in >> 24));
}


ot_u8 q_readbyte(ot_queue* q) {
    ot_u8 retval;
    retval = __byte(q->front, q->getcursor);
    q->getcursor++;
}

ot_u16 q_readshort(ot_queue* q) {
    ot_u16 output;
    output  = (ot_u16)q_readbyte(q) << 8;
    output |= (ot_u16)q_readbyte(q);
    
    return output;
}

ot_u16 q_readshort_be(ot_queue* q) {
    ot_u16 output;
    output  = (ot_u16)q_readbyte(q);
    output |= (ot_u16)q_readbyte(q) << 8;
    
    return output;
}

ot_u32 q_readlong(ot_queue* q)  {
    ot_u32 output;
    output  = (ot_u32)q_readbyte(q) << 24;
    output |= (ot_u32)q_readbyte(q) << 16;
    output |= (ot_u32)q_readbyte(q) << 8;
    output |= (ot_u32)q_readbyte(q);
    
    return output;
}

ot_u32 q_readlong_be(ot_queue* q)  {
    ot_u32 output;
    output  = (ot_u32)q_readbyte(q);
    output |= (ot_u32)q_readbyte(q) << 8;
    output |= (ot_u32)q_readbyte(q) << 16;
    output |= (ot_u32)q_readbyte(q) << 24;
    
    return output;
}





ot_int q_writestring(ot_queue* q, ot_u8* string, ot_int length) {
///@note this implementation writes a packed string
    ot_int i;
    
    if (length <= 0) {
        return 0;
    }
    
    i = q_writespace(q);
    if (i < length) {
        length = i;
    } 
    
    for (i=0; i<length; i++) {
        q_writebyte( q, __byte((int*)string, i) );
    }
    
    return length;
}



ot_int q_readstring(ot_queue* q, ot_u8* string, ot_int length) {
///@note this implementation reads-out a packed string
    ot_int i;
    
    if (length <= 0) {
        return 0;
    }
    
    i = q_readspace(q);
    if (i < length) {
        length = i;
    } 
    
    for (i=0; i<length; i++) {
        __byte((int*)string, i) = q_readbyte(q);
    }
    
    return length;
}



ot_int q_movedata(ot_queue* qdst, ot_queue* qsrc, ot_int length) {
    ot_int writespace, readspace, limit;

    if (length <= 0) {
        return 0;
    }
    
    writespace  = q_writespace(qdst);
    readspace   = q_readspace(qsrc);
    limit       = (writespace < readspace) ? writespace : readspace;
    if (limit < length) {
        return length - limit;
    }
    
    limit = length;
    while (limit > 0) {
        limit--;
        q_writebyte(qdst, q_readbyte(qsrc));
    }
    
    return length;
}


#if (defined(__STDC__) || defined (__POSIX__))
#include <stdio.h>

void q_print(ot_queue* q) {
    int length;
    int i;
    int row;
    length = q_length(q);

    printf("ot_queue Length/Alloc: %d/%d\n", length, q->alloc);
    printf("ot_queue Getcursor:    %d\n", (int)(q->getcursor-q->front));
    printf("ot_queue Putcursor:    %d\n", (int)(q->putcursor-q->front));

    for (i=0, row=0; length>0; ) {
        length -= 8;
        row    += (length>0) ? 8 : 8+length;
        printf("%04X: ", i);
        for (; i<row; i++) {
            printf("%02X ", q->front[i]);
        }
        printf("\n");
    }
    printf("\n");
}

#endif

