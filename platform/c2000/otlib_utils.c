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
  * @file       /platform/c2000/otlib_utils.c
  * @author     JP Norair
  * @version    R103
  * @date       20 Sept 2014
  * @brief      Additional utils for C2000
  *
  ******************************************************************************
  */


#include <otstd.h>
#include <platform/config.h>

/// Packs 16bit LSBs into 8bit array
void otutils_pack2(void* dst, ot_u16* src, ot_u16 size) {
    ot_u16* dst16 = dst;
    
    while (size > 1) {
        size   -= 2;
        *dst16  = (ot_u16)*src++ << 8;
        *dst16 |= (ot_u16)*src++ & 0xFF;
        dst16++;
    }
    if (size != 0) {
        *dst16  = (ot_u16)*src++ << 8;
        dst16++;
    }
}


void otutils_unpack2(ot_u16* dst, void* src, ot_u16 size) {
    ot_u16* src16 = src;
    
    while (size > 1) {
        size   -= 2;
        *dst++  = ((ot_u16)*src16 & 0xFF00) >> 8;
        *dst++  = ((ot_u16)*src16 & 0x00FF);
        src16++;
    }
    if (size != 0) {
        *dst++  = ((ot_u16)*src16 & 0xFF00) >> 8;
        dst++;
    }
}
