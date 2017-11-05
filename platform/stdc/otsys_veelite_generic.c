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
  * @file       /platform/stdc/otsys_veelite_generic.c
  * @author     JP Norair
  * @version    R100
  * @date       3 Nov 2017
  * @brief      SRAM Method for Veelite Core Functions
  * @ingroup    Veelite
  * 
  * Summary:
  * This module is part of the Veelite Core, which contains the low-level read
  * and write filesystem functionality.  This variant is meant for systems that
  * use NAND FLASH as the source of non-volatile memory, and which have enough
  * SRAM to load the entire filesystem into a part of it.  At power down or
  * reset conditions, the contents of the Filesystem SRAM will be written back
  * to Flash.
  *
  ******************************************************************************
  */

#include <otplatform.h>

#include <otsys/veelite_core.h>
#include <otlib/logger.h>
#include <otlib/memcpy.h>



static ot_u32 fsram[FLASH_FS_ALLOC/4];
#define FSRAM ((ot_u16*)fsram)


/// Set Bus Error (code 7) on physical flash access faults (X2table errors).
/// Vector to Access Violation ISR (CC430 Specific)
#if defined(VLX2_DEBUG_ON) && (LOG_FEATURE(FAULTS) == ENABLED)
#   define BUSERROR_CHECK(EXPR, MSGLEN, MSG) \
        do { \
            if (EXPR) { \
                fprintf(stderr, "%s\n", (ot_u8*)MSG); \
            } \
        } while (0)
#else
#   define BUSERROR_CHECK(EXPR, MSGLEN, MSG) do { } while(0)
#endif



/** Generic Veelite Core Function Implementations <BR>
  * ========================================================================<BR>
  * Used for any and all memory topologies
  */
vas_loc vas_check(vaddr addr) {
    ot_s32 scratch;
    
    scratch = addr - VWORM_BASE_VADDR;
    if ((scratch >= 0) && (scratch < VWORM_SIZE))   return in_vworm;
    
    scratch = addr - VSRAM_BASE_VADDR;
    if ((scratch >= 0) && (scratch < VSRAM_SIZE))   return in_vsram;

    return vas_error;
}



/** VWORM Functions <BR>
  * ========================================================================<BR>
  */

#ifndef EXTF_vworm_format
ot_u8 vworm_format( ) {
    return 0;
}
#endif

#ifndef EXTF_vworm_init
ot_u8 vworm_init() {
    ot_memcpy4(fsram, (ot_u32*)FLASH_FS_ADDR, sizeof(fsram)/4);
    return 0;
}
#endif

#ifndef EXTF_vworm_print_table
void vworm_print_table() {
}
#endif

#ifndef EXTF_vworm_save
ot_u8 vworm_save( ) {
/// @note This does nothing for pure STDC implementation, which is entirely 
///       RAM based.  For Microcontroller variant, this can save the file table
///       to nonvolatile memory.
    return 0;
}
#endif

#ifndef EXTF_vworm_read
ot_u16 vworm_read(vaddr addr) {
    ot_u16* data;
    addr   -= VWORM_BASE_VADDR;
    addr   &= ~1;
    data    = (ot_u16*)((ot_u8*)fsram + addr);
    return *data;
}
#endif

#ifndef EXTF_vworm_write
ot_u8 vworm_write(vaddr addr, ot_u16 data) {
    ot_u16* aptr;
    addr   -= VWORM_BASE_VADDR;
    addr   &= ~1;
    aptr    = (ot_u16*)((ot_u8*)fsram + addr);
    *aptr   = data;
    return 0;
}
#endif

#ifndef EXTF_vworm_mark
ot_u8 vworm_mark(vaddr addr, ot_u16 value) {
    return vworm_write(addr, value);
}
#endif

#ifndef EXTF_vworm_mark_physical
ot_u8 vworm_mark_physical(ot_u16* addr, ot_u16 value) {
#   if ((VWORM_SIZE > 0) && (OT_FEATURE_VLNVWRITE == ENABLED))
    BUSERROR_CHECK( (((ot_u32)addr < (ot_u32)fsram) || \
                    ((ot_u32)addr >= (ot_u32)(&fsram[FLASH_FS_ALLOC/4]))), 7, "VLC_"__LINE__);
                    
    *addr = value;
#   endif
    return 0;
}
#endif

#ifndef EXTF_vworm_get
ot_u8* vworm_get(vaddr addr) {
    addr -= VWORM_BASE_VADDR;
    return (ot_u8*)fsram + addr;
}
#endif

#ifndef EXTF_vworm_wipeblock
ot_u8 vworm_wipeblock(vaddr addr, ot_uint wipe_span) {
    return 0;
}
#endif





/** VSRAM Functions <BR>
  * ========================================================================<BR>
  * SRAM version of Veelite doesn't utilize VSRAM
  */

#ifndef EXTF_vsram_read
ot_u16 vsram_read(vaddr addr) {
    return vworm_read(addr);
}
#endif

#ifndef EXTF_vsram_mark
ot_u8 vsram_mark(vaddr addr, ot_u16 value) {
    return vworm_mark(addr, value);
}
#endif

#ifndef EXTF_vsram_mark_physical
ot_u8 vsram_mark_physical(ot_u16* addr, ot_u16 value) {
    return vworm_mark_physical(addr, value);
}
#endif

#ifndef EXTF_vsram_get
ot_u8* vsram_get(vaddr addr) {
    return vworm_get(addr);
}
#endif


