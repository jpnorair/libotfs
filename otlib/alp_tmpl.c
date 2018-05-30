/* Copyright 2013 JP Norair
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
  * @file       /otlib/alp_tmpl.c
  * @author     JP Norair
  * @version    R101
  * @date       23 Mar 2014
  * @brief      ALP to template conversion functions
  * @ingroup    ALP
  *
  * Functions to convert a stream to templates (OTAPI_tmpl.h) and templates to
  * a stream.
  * 
  ******************************************************************************
  */


#include <otlib/alp.h>
#if ( (OT_FEATURE(ALP) == ENABLED) )


#define _PTR_TEST(X)    (X != NULL)




OT_WEAK void alp_breakdown_u8(ot_queue* in_q, void* data_type) {
    *(ot_u8*)data_type = q_readbyte(in_q); 
}

OT_WEAK void alp_breakdown_u16(ot_queue* in_q, void* data_type) {
    *(ot_u16*)data_type = q_readshort(in_q);
}

OT_WEAK void alp_breakdown_u32(ot_queue* in_q, void* data_type) {
    *(ot_u32*)data_type = q_readlong(in_q);
}

OT_WEAK void alp_stream_u8(ot_queue* out_q, void* data_type) {
    q_writebyte(out_q, *(ot_u8*)data_type);
}

OT_WEAK void alp_stream_u16(ot_queue* out_q, void* data_type) {
    q_writebyte(out_q, *(ot_u16*)data_type);
}

OT_WEAK void alp_stream_u32(ot_queue* out_q, void* data_type) {
    q_writebyte(out_q, *(ot_u32*)data_type);
}





OT_WEAK void alp_breakdown_queue(ot_queue* in_q, void* data_type) {
    ot_u16 queue_length;
    void* queue_front;
    queue_length                        = q_readshort(in_q);
    ((ot_queue*)data_type)->alloc       = queue_length;
    ((ot_queue*)data_type)->options     = in_q->options;
    queue_front                         = q_markbyte(in_q, queue_length);
    ((ot_queue*)data_type)->front       = queue_front;
    ((ot_queue*)data_type)->back        = q_cursor_val((ot_queue*)data_type, queue_length);
    ((ot_queue*)data_type)->getcursor   = q_cursor_val((ot_queue*)data_type, 0);
    ((ot_queue*)data_type)->putcursor   = q_cursor_val((ot_queue*)data_type, 0);
    //((ot_queue*)data_type)->back        = queue_front+queue_length;
    //((ot_queue*)data_type)->getcursor   = queue_front;
    //((ot_queue*)data_type)->putcursor   = queue_front;
}

OT_WEAK void alp_stream_queue(ot_queue* out_q, void* data_type) { 
    if _PTR_TEST(data_type) {
    	ot_int length;
    	length = q_length((ot_queue*)data_type);
        q_writeshort(out_q, ((ot_queue*)data_type)->alloc);
        q_writeshort(out_q, ((ot_queue*)data_type)->options.ushort);
        q_writeshort(out_q, length);
        q_writestring(out_q, ((ot_queue*)data_type)->front, length);     
    }
}




OT_WEAK void alp_breakdown_session_tmpl(ot_queue* in_q, void* data_type) {
    q_readstring(in_q, (ot_u8*)data_type, 6);
}

OT_WEAK void alp_stream_session_tmpl(ot_queue* out_q, void* data_type) {
    if _PTR_TEST(data_type) 
        q_writestring(out_q, (ot_u8*)data_type, 6);
}






OT_WEAK void alp_breakdown_id_tmpl(ot_queue* in_q, void* data_type) {
    ot_int id_length;
    id_length                       = q_readbyte(in_q);
    ((id_tmpl*)data_type)->length   = id_length;
    ((id_tmpl*)data_type)->value    = (id_length==0) ? NULL : q_markbyte(in_q, id_length);
}

OT_WEAK void alp_stream_id_tmpl(ot_queue* out_q, void* data_type) { 
    if _PTR_TEST(data_type) {
        q_writebyte(out_q, ((id_tmpl*)data_type)->length);
        q_writestring(out_q, ((id_tmpl*)data_type)->value, ((id_tmpl*)data_type)->length);
    }
}












#endif


