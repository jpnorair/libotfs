/* Copyright 2010-2013 JP Norair
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
  * @file       /otlib/buffers.h
  * @author     JP Norair
  * @version    R100
  * @date       8 Oct 2013
  * @brief      An inline, volatile set of stream buffers used with OT ot_queues
  * @defgroup   Buffers (Buffers Module)
  * @ingroup    Buffers
  *
  ******************************************************************************
  */

#ifndef __BUFFERS_H
#define __BUFFERS_H

#include <otstd.h>
#include <otlib/queue.h>


/// Buffer Partitions
/// Number of partitions allowed is the total allocated size divided by 256.
/// Partitions, hence, are always 256 bytes.
#define BUF_PARTITIONS      (OT_PARAM(BUFFER_SIZE)/256)
#define BUF_PARTITION(VAL)  (otbuf+(256*VAL))


/// Main Buffer (encapsulates buffers for all supported ot_queues)
extern ot_u8 otbuf[OT_PARAM(BUFFER_SIZE)];


#if (OT_FEATURE(NDEF) || OT_FEATURE(ALP))
    /// Analagous to stdin/stdout.  Frequently used ALP application queues
    extern ot_queue otmpin;
    extern ot_queue otmpout;

#endif




/** @brief Initializes the System ot_queues to defaults
  * @param none
  * @retval none
  * @ingroup Buffers
  *
  * The System ot_queues (rxq, txq, otmpin, otmpout) can be manipulated to point
  * to different data elements during the course of their usage.  This function
  * will reset them to their defaults.  Use it whenever you want to reset the
  * queues or during boot-up.
  */
void buffers_init();




#endif
