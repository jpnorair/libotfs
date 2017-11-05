

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define MCU_TYPE_PTRINT     (int*)
#define MCU_TYPE_PTRUINT    (unsigned int*)
#define MCU_PARAM_ERRPTR    (void*)



extern const ot_u8 overhead_files[];
extern const ot_u8 gfb_stock_files[GFB_TOTAL_BYTES];
extern const ot_u8 isf_stock_files[];


#define FLASH_PAGE_SIZE     512
#define FLASH_START_ADDR    ((unsigned int)overhead_files)


#if OT_FEATURE(DLL_SECURITY) || OT_FEATURE(NL_SECURITY) || OT_FEATURE(VL_SECURITY)
#   include <oteax.h>
//#   define EAXdrv_t eax_ctx
    typedef eax_ctx EAXdrv_t;
#else
//#   define EAXdrv_t ot_uint
    typedef ot_uint EAXdrv_t;
#endif


