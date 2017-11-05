/* Copyright 2010-2012 JP Norair
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
  * @file       /otlib/logger.c
  * @author     JP Norair
  * @version    R100
  * @date       31 Jul 2012
  * @brief      Default implementation of OpenTag logging functions
  * @ingroup    Logger
  *
  * The default logger uses ALP/NDEF as a wrapping framework and MPipe as the
  * medium.  MPipe uses NDEF headers internally, so when a queue full of NDEF
  * frames is sent to MPipe, the MPipe control task (/otlib/mpipe.c) and the
  * MPipe driver (/otplatform/xxx/mipe_xxx~.c) have all the information they
  * need to do the logging.
  *
  * Considering the above, the function mpipe_txschedule(0) is used to trigger 
  * the MPipe control task.  It does not need any extensive arguments because 
  * all of the necessary information is written onto the NDEF header in the
  * mpipe.alp.out queue.
  *
  * Check the wiki for high-level (and low level) description of the logger.
  * http://www.indigresso.com/wiki/doku.php?id=opentag:otlib:logger
  *
  * @note Ideally, these functions shouldn't be directly manipulating ALP/NDEF
  * data, but for the moment they might do so.
  *
  ******************************************************************************
  */


#include <otstd.h>

#include <otlib/logger.h>
#include <otlib/alp.h>
#include <otlib/queue.h>
#include <otlib/utils.h>




ot_bool logger_header(ot_u8 id_subcode, ot_int payload_length) {
/// Currently does nothing.  Could tie this into printf() or some other I/O means.
    return False;
}



#ifndef EXTF_logger
void logger(ot_u8 subcode, ot_int length, ot_u8* data) {
/// Currently does nothing.  Could tie this into printf() or some other I/O means.
}
#endif



void sub_logmsg(ot_int label_len, ot_int data_len, ot_u8* label, ot_u8* data) {
/// Currently does nothing.  Could tie this into printf() or some other I/O means.
}



#ifndef EXTF_logger_msg
void logger_msg(logmsg_type logcmd, ot_int label_len, ot_int data_len, ot_u8* label, ot_u8* data) {
/// Currently does nothing.  Could tie this into printf() or some other I/O means.
}
#endif


#ifndef EXTF_logger_hexmsg
void logger_hexmsg(ot_int label_len, ot_int data_len, ot_u8* label, ot_u8* data) {
/// Currently does nothing.  Could tie this into printf() or some other I/O means.
}
#endif



#ifndef EXTF_logger_direct
void logger_direct() {
/// Currently does nothing.  Could tie this into printf() or some other I/O means.
}
#endif



#ifndef EXTF_logger_code
void logger_code(ot_int label_len, ot_u8* label, ot_u16 code) {
/// Currently does nothing.  Could tie this into printf() or some other I/O means.
}
#endif




