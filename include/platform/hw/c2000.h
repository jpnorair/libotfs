

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <app/config.h>

#include <otsys/types.h>


/// Must define uint8_t and int8_t
typedef unsigned char uint8_t;
typedef signed char int8_t;




extern const ot_u16 overhead_files[];
extern const ot_u16 gfb_stock_files[GFB_TOTAL_BYTES/2];
extern const ot_u16 iss_stock_files[];
extern const ot_u16 isf_stock_files[];


#ifndef __C2000__
#   define __C2000__
#else

#if defined(__TMS320F2806x__)
#   define MCU_PARAM_PORTS  3
#   define FLASH_SIZE       (128*1024)
#   define SRAM_SIZE        (8*1024)    
#   define EEPROM_SIZE      0
#   define TMS320F2806x

///@note C2000ware directory must be added to include directories

#   if defined(__USE_C2000WARE__)
//#   include <device_support/f2806x/common/include/DSP28x_Project.h>
#   include <device_support/f2806x/common/include/F2806x_Cla_defines.h>
#   include <device_support/f2806x/common/include/F2806x_Cla_typedefs.h>
#   include <device_support/f2806x/common/include/F2806x_DefaultISR.h>
#   include <device_support/f2806x/common/include/F2806x_Dma_defines.h>
#   include <device_support/f2806x/common/include/F2806x_EPwm_defines.h>
#   include <device_support/f2806x/common/include/F2806x_Examples.h>
#   include <device_support/f2806x/common/include/F2806x_GlobalPrototypes.h>
#   include <device_support/f2806x/common/include/F2806x_I2c_defines.h>
#   include <device_support/f2806x/common/include/F2806x_SWPrioritizedIsrLevels.h>
#   include <device_support/f2806x/headers/include/F2806x_Adc.h>
#   include <device_support/f2806x/headers/include/F2806x_BootVars.h>
#   include <device_support/f2806x/headers/include/F2806x_Cla.h>
#   include <device_support/f2806x/headers/include/F2806x_Comp.h>
#   include <device_support/f2806x/headers/include/F2806x_CpuTimers.h>
#   include <device_support/f2806x/headers/include/F2806x_DevEmu.h>
#   include <device_support/f2806x/headers/include/F2806x_Device.h>
#   include <device_support/f2806x/headers/include/F2806x_Dma.h>
#   include <device_support/f2806x/headers/include/F2806x_ECan.h>
#   include <device_support/f2806x/headers/include/F2806x_ECap.h>
#   include <device_support/f2806x/headers/include/F2806x_EPwm.h>
#   include <device_support/f2806x/headers/include/F2806x_EQep.h>
#   include <device_support/f2806x/headers/include/F2806x_Gpio.h>
#   include <device_support/f2806x/headers/include/F2806x_HRCap.h>
#   include <device_support/f2806x/headers/include/F2806x_I2c.h>
#   include <device_support/f2806x/headers/include/F2806x_Mcbsp.h>
#   include <device_support/f2806x/headers/include/F2806x_NmiIntrupt.h>
#   include <device_support/f2806x/headers/include/F2806x_PieCtrl.h>
#   include <device_support/f2806x/headers/include/F2806x_PieVect.h>
#   include <device_support/f2806x/headers/include/F2806x_Sci.h>
#   include <device_support/f2806x/headers/include/F2806x_Spi.h>
#   include <device_support/f2806x/headers/include/F2806x_SysCtrl.h>
#   include <device_support/f2806x/headers/include/F2806x_Usb.h>
#   include <device_support/f2806x/headers/include/F2806x_XIntrupt.h>
#   endif

#elif defined(__TMS320F2837x__)
#   define MCU_PARAM_PORTS  3
#   define FLASH_SIZE       (128*1024)
#   define SRAM_SIZE        (8*1024)
#   define EEPROM_SIZE      0

#else
#   warning "Unknown C2000 defined.  Defaulting to TMS320F2806x"
#   define MCU_PARAM_PORTS  3
#   define FLASH_SIZE       (128*1024)
#   define SRAM_SIZE        (8*1024)   
#   define EEPROM_SIZE      0
#   define TMS320F2806x



#endif

#endif



/** Platform Support settings      <BR>
  * ========================================================================<BR>
  */

#define PLATFORM(VAL)           PLATFORM_##VAL
#define PLATFORM_C2000

#ifndef __LITTLE_ENDIAN__
#   error "Endian-ness misdefined, should be __LITTLE_ENDIAN__ (check build_config.h)"
#endif
#define PLATFORM_POINTER_SIZE           2                           // How many 16 bit bytes is a pointer?
#define PLATFORM_ENDIAN16(VAR16)        PLATFORM_ENDIAN16_C(VAR16)  // Big-endian to Platform-endian
#define PLATFORM_ENDIAN32(VAR32)        (((VAR32 & 0x000000FF) << 24) | \
                                         ((VAR32 & 0x0000FF00) << 8) | \
                                         ((VAR32 & 0x00FF0000) >> 8) | \
                                         ((VAR32 & 0xFF000000) >> 24))
#define PLATFORM_ENDIAN16_C(CONST16)    (ot_u16)( (((ot_u16)CONST16) << 8) | (((ot_u16)CONST16) >> 8) )




/** C2000 family MCU settings     <BR>
  * ========================================================================<BR>
  */
#define MCU_FEATURE(VAL)        MCU_FEATURE_##VAL   // FEATURE                  NOTE
#define MCU_FEATURE_SVMONITOR   DISABLED            // Auto Low V powerdown     On many MCUs
#define MCU_FEATURE_CRC16       DISABLED            // CRC16                    Exists on STM32L0, but HW needs to be tested
#define MCU_FEATURE_CRC         MCU_FEATURE_CRC16   // Legacy definition
#define MCU_FEATURE_ECC         DISABLED            // ECC engine 

// True Random Number HW
#define MCU_FEATURE_TRNG        DISABLED
// AES HW
#define MCU_FEATURE_AES128      DISABLED



#define MCU_TYPE(VAL)           MCU_TYPE_##VAL
#define MCU_TYPE_UINT           uint32_t
#define MCU_TYPE_INT            int32_t
#define MCU_TYPE_PTRINT         (int32_t*)
#define MCU_TYPE_PTRUINT        (uint32_t*)


#define MCU_PARAM(VAL)                  MCU_PARAM_##VAL
#define MCU_PARAM_POINTERSIZE           4
#define MCU_PARAM_ERRPTR                (void*)
#define MCU_PARAM_UART_9600BPS          9600
#define MCU_PARAM_UART_28800BPS         28800
#define MCU_PARAM_UART_57600BPS         57600
#define MCU_PARAM_UART_115200BPS        115200
#define MCU_PARAM_UART_250000BPS        250000
#define MCU_PARAM_UART_500000BPS        500000


#define SRAM_START_ADDR         0x0000A000          ///@note uses L4 SARAM block
#define EEPROM_START_ADDR       NULL
#define FLASH_START_ADDR        ((uint32_t)overhead_files)
#define FLASH_START_PAGE        0
#define FLASH_PAGE_SIZE         512
#define FLASH_WORD_BYTES        2
#define FLASH_WORD_BITS         (FLASH_WORD_BYTES*8)
#define FLASH_PAGE_ADDR(VAL)    (FLASH_START_ADDR + ( (VAL) * FLASH_PAGE_SIZE) )
    


#if OT_FEATURE(DLL_SECURITY) || OT_FEATURE(NL_SECURITY) || OT_FEATURE(VL_SECURITY)
#   include <oteax.h>
//#   define EAXdrv_t eax_ctx
    typedef eax_ctx EAXdrv_t;
#else
//#   define EAXdrv_t ot_uint
    typedef ot_uint EAXdrv_t;
#endif


