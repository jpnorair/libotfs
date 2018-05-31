/* Copyright 2014 JP Norair
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
  * @file       /otsys/time.h
  * @author     JP Norair
  * @version    R100
  * @date       31 Aug 2014
  * @brief      System Time Interface
  * @defgroup   Time
  * @ingroup    Time
  *  
  ******************************************************************************
  */

#include <otstd.h>


#ifndef __SYS_TIME_H
#define __SYS_TIME_H


///@note possible 48 bit time implementation.  Just a random thought.
//typedef struct OT_PACKED {
//    ot_u32  secs;
//    ot_u16  sti;
//} ot_time;
//
//typedef union {
//    ot_time ot;
//    ot_u16  ushort[3];
//    ot_u8   ubyte[6];
//} ot_time_union;


typedef struct {
    ot_u32 upper;
    ot_u32 clocks;
} ot_time;


/** @brief Initializes Time system using UTC Epoch seconds
  * @param utc      (ot_u32) UTC Epoch seconds
  * @retval None    
  * @sa time_set_utc
  *
  * This function should be run at system startup, even if time isn't resolved 
  * at that point.  time_set_utc() can be used at any point to update the time
  * value.
  */
void time_init_utc(ot_u32 utc);


/** @brief Set Time system using UTC Epoch seconds
  * @param utc      (ot_u32) UTC Epoch seconds
  * @retval None    
  *
  * Used to set the system time to a UTC Epoch seconds value during runtime.
  */
void time_set_utc(ot_u32 utc);


//void time_add_ti(ot_u32 ticks);

ot_u32 time_get_utc(void);

ot_u32 time_uptime_secs(void);

ot_u32 time_uptime(void);




/** @brief Add clocks to the system timer
  * @param clocks   (ot_u32) System clocks to add to system time.
  * @retval None    
  * @sa time_load_now
  * @sa time_add_ti
  *
  * Clocks are usually 1/1024 seconds, but they can be any time value that 
  * is 1/(2^x) seconds.
  *
  * Certain platforms may also implement only time_load_now()
  */
void time_add(ot_u32 clocks);


/** @brief Add ticks to the system timer
  * @param ticks   (ot_u32) System ticks to add to system time.
  * @retval None    
  * @sa time_add
  *
  * This function is just like time_add() except that the time units
  * are always ticks (1/1024).
  */
void time_add_ti(ot_u32 ticks);




/** Driver functions 
  * The following functions are sometimes implemented in the platform driver.
  * They do the low-level work for the system time module.
  */


/** @brief Load system time into a supplied ot_time variable
  * @param now      (ot_time*) system time output
  * @retval None    
  *
  * This is the only driver function that MUST be implemented.  It must
  * pull the current time and load it into the output ("now").
  *
  * The default variant of this function is implemented in otsys/time.c,
  * and it requires that time_add() is called regularly in a scheduler loop.
  * A platform specific variant may be implemented which pulls the time
  * information from a system-level time API.  In this latter case, the 
  * time_add() function is not used.
  */
void time_load_now(ot_time* now);



#endif
