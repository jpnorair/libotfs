

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/// These parameters are sourced from:
/// https://sourceforge.net/p/predef/wiki/Architectures/

#define IS_64BIT    (defined(__x86_64__) || defined(__x86_64)) || \
                    (defined(__amd64__) || defined(__amd64)) || \
                    (defined(__arm__) && defined(__aarch64__)) || \
                    (defined(__ia64__) || defined(_IA64) || defined(__IA64__)) || \
                    (defined(__ppc64__) || defined(ppc64))

#define IS_32BIT    (defined(__i386__) || defined(__i386) || defined(i386)) || \
                    (defined(__i486__) || defined(__i586__) || defined(__i686__)) || \
                    (defined(__m68k__)) || \
                    (defined(__mips__) || defined(mips)) || \
                    (defined(__ppc__) || defined(ppc))

#if IS_64BIT
#   define __64BIT__
#elif IS_32BIT
#   define __32BIT__
#else
#   error "Only 64 and 32 bit architectures are supported in this version"
#endif
                    


/// 64bit platforms
#if defined(__64BIT__)
#   define MCU_TYPE_UINT        uint64_t
#   define MCU_TYPE_INT         int64_t
#   define MCU_TYPE_PTRINT      (int64_t*)
#   define MCU_TYPE_PTRUINT     (uint64_t*)
#   define MCU_PARAM_ERRPTR     (void*)

#elif defined(__32BIT__)
#   define MCU_TYPE_UINT        uint32_t
#   define MCU_TYPE_INT         int32_t
#   define MCU_TYPE_PTRINT      (int32_t*)
#   define MCU_TYPE_PTRUINT     (uint32_t*)
#   define MCU_PARAM_ERRPTR     (void*)

#endif


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


