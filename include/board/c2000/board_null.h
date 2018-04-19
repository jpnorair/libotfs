/* Copyright 2009-2012 JP Norair
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
  * @file       /board/stdc/board_posix_a.h
  * @author     JP Norair
  * @version    R101
  * @date       24 Mar 2014
  * @brief      Board Configuration for POSIX Type A, on STD C libaries
  * @ingroup    Platform
  *
  * Do not include this file, include OT_platform.h
  ******************************************************************************
  */
  

#ifndef __board_null_H
#define __board_null_H

#include <app/config.h>

#include <platform/hw/c2000.h>




/// Macro settings: ENABLED, DISABLED, NOT_AVAILABLE
#ifdef ENABLED
#   undef ENABLED
#endif
#define ENABLED  1

#ifdef DISABLED
#   undef DISABLED
#endif
#define DISABLED  0

#ifdef NOT_AVAILABLE
#   undef NOT_AVAILABLE
#endif
#define NOT_AVAILABLE   DISABLED




/** MCU Feature settings      <BR>
  * ========================================================================<BR>
  *
  */
#define MCU_CONFIG(VAL)                 MCU_CONFIG_##VAL   // FEATURE 
#define MCU_CONFIG_MULTISPEED           DISABLED         // Allows usage of MF-HF clock boosting
#define MCU_CONFIG_MAPEEPROM            DISABLED
#define MCU_CONFIG_MEMCPYDMA            DISABLED        // MEMCPY DMA should be lower priority than MPIPE DMA
#define MCU_CONFIG_DATAFLASH            DISABLED


/** Platform Memory Configuration <BR>
  * ========================================================================<BR>
  * This is redundant for the NULL application, but it is left here for 
  * reference purposes.
  */

#define FLASH_NUM_PAGES         (FLASH_SIZE/FLASH_PAGE_SIZE)
#define FLASH_FS_ADDR           FLASH_START_ADDR
#define FLASH_FS_PAGES          16
#define FLASH_FS_FALLOWS        0 
#define FLASH_FS_ALLOC          (FLASH_PAGE_SIZE*FLASH_FS_PAGES)





/** Board-based Feature Settings <BR>
  * ========================================================================<BR>
  */
#define BOARD_FEATURE(VAL)              BOARD_FEATURE_##VAL
#define BOARD_PARAM(VAL)                BOARD_PARAM_##VAL




/** Note: Clocking for the Board's MCU      <BR>
  * ========================================================================<BR>
  */





/** Peripheral definitions for this platform <BR>
  * ========================================================================<BR>
  * Superfluous for Null application.
  */
  
#define OT_GPTIM_ID         'R'
#define OT_GPTIM            RTC
#define OT_GPTIM_CLOCK      33300
#define OT_GPTIM_RES        1024
#define OT_GPTIM_SHIFT      0
#define TI_TO_CLK(VAL)      ((OT_GPTIM_RES/1024)*VAL)
#define CLK_TO_TI(VAL)      (VAL/(OT_GPTIM_RES/1024))

#define OT_GPTIM_ERRDIV     32768







/** Basic GPIO Setup <BR>
  * ========================================================================<BR>
  * <LI>MPipe is a UART-based serial interface.  It needs to be specified with
  *     a UART peripheral number and RX/TX port & pin configurations. </LI>
  * <LI>OT Triggers are test outputs, usually LEDs.  They should be configured
  *     to ports & pins where there are LED or other trigger connections.</LI>
  * <LI>The GWN feature is part of a true-random-number generator.  It is
  *     optional.  It needs a port & pin for the ADC input (GWNADC) and also,
  *     optionally, one for a Zener driving output port & pin.  Using a Zener
  *     can greatly accelerate the random number generation process.</LI>
  */




#endif
