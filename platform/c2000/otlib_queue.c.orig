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
  * This is a custom patch of the queue module designed to work with the 
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



/** Queue "Object" functions
  * ========================
  */

void q_writeshort(ot_queue* q, ot_uint short_in) {
    *q->putcursor++ = short_in >> 8;
    *q->putcursor++ = short_in & 0xFF;
}


void q_writeshort_be(ot_queue* q, ot_uint short_in) {
    q_writeshort(q, short_in);
    //*q->putcursor++ = short_in & 0xFF;
    //*q->putcursor++ = short_in >> 8;
}


void q_writelong(ot_queue* q, ot_ulong long_in) {
    *q->putcursor++ = (long_in >> 24) & 0xFF;
    *q->putcursor++ = (long_in >> 16) & 0xFF;
    *q->putcursor++ = (long_in >> 8) & 0xFF;
    *q->putcursor++ = (long_in >> 0) & 0xFF;
}


ot_u8 q_readbyte(ot_queue* q) {
    return *q->getcursor++;
}


ot_u16 q_readshort(ot_queue* q) {
    ot_u16 data;
    data    = *q->getcursor++ << 8;
    data   += *q->getcursor++;
    
    return data;
}



ot_u16 q_readshort_be(ot_queue* q) {
    return q_readshort(q);
}



ot_u32 q_readlong(ot_queue* q)  {
    ot_u32 data;
    data    = ((ot_u32)*q->getcursor++) << 24;
    data   |= ((ot_u32)*q->getcursor++ & 0xFF) << 16;
    data   |= ((ot_u32)*q->getcursor++ & 0xFF) << 8;
    data   |= ((ot_u32)*q->getcursor++ & 0xFF) << 0;

    return data;
}


