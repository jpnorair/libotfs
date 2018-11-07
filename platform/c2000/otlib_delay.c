/* Copyright 2017 JP Norair
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
  * @file       /platform/c2000/otlib_delay.c
  * @author     JP Norair
  * @version    R100
  * @date       26 Oct 2017
  * @brief      Delay OTlib Functions for POSIX
  * @ingroup    Delay
  *
  ******************************************************************************
  */

#include <otstd.h>
#include <otplatform.h>
#include <otlib/delay.h>

#if defined (__TMS320F2806x__)
  extern void DSP28x_usDelay(long Count);

	#define CPU_RATE   11.111L   // for a 90MHz CPU clock speed (SYSCLKOUT)
	#define DELAY_US(A)  DSP28x_usDelay(((((long double) A * 1000.0L) / (long double)CPU_RATE) - 9.0L) / 5.0L)
#elif defined (__TMS320F2837x__)
	extern void F28x_usDelay(long LoopCount);

//	#define CPU_RATE   5.00L   // for a 200MHz CPU clock speed (SYSCLKOUT)
	#define CPU_RATE   5.263L   // for a 190MHz CPU clock speed  (SYSCLKOUT)
	#define DELAY_US(A)  F28x_usDelay(((((long double) A * 1000.0L) / (long double)CPU_RATE) - 9.0L) / 5.0L)
#endif


#ifndef EXTF_delay_sti
void delay_sti(ot_u16 sti) {
    delay_us( sti*31 );
}
#endif


#ifndef EXTF_delay_ti
void delay_ti(ot_u16 n) {
    delay_us(n*977);
}
#endif


#ifndef EXTF_delay_ms
void delay_ms(ot_u16 n) {
    delay_us(n*1000);
}
#endif


#ifndef EXTF_delay_us
void delay_us(ot_u16 n) {
    DELAY_US(n);
}
#endif

