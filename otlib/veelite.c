/* Copyright 2010-2011, JP Norair
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
  * @file       /otsys/veelite.c
  * @author     JP Norair
  * @version    R100
  * @date       30 Oct 2010
  * @brief      ISO 18000-7 veelite data files
  * @ingroup    Veelite
  *
  * Veelite is based on the GFB + ISF specification of ISO 18000-7 Mode 2.  It
  * may be used for anything, however, as long as the GFB + ISF organization
  * makes sense.
  *
  ******************************************************************************
  */

#include <otstd.h>

#if (OT_FEATURE(VEELITE) == ENABLED)

#include <platform/config.h>
#include <otlib/utils.h>
#include <otlib/auth.h>
#include <otsys/veelite.h>

#if defined(__C2000__)
#   define LSB16(_x)        (_x&0xFF)
#   define MSB16(_x)        (_x>>8)
#   define BYTE1(_x)        (_x&0xFF)
#   define BYTE0(_x)        (_x>>8)
#   define JOIN_2B(B0, B1)  (ot_u16)((((ot_u16)(B1))<<8)|((ot_u16)(B0)))
#endif



// You can open a finite number of files simultaneously
static vlFILE vlfile[OT_PARAM(VLFPS)];

// Two checks for File Pointer Validity
// Bottom option is slower but more robust.  Good for Debug.
#define FP_ISVALID(fp_VAL)  (fp_VAL != NULL)
//#define FP_ISVALID(fp_VAL)  ((fp_VAL >= &vlfile[0]) && (fp_VAL <= &vlfile[OT_PARAM(VLFPS)-1]))



// If creating new files is permitted, then we store a mirror of the filesystem header.
#if (OT_FEATURE(VLNEW) == ENABLED)
static vlFSHEADER vlfs;


#endif




/** @note Boundary Definitions
  * Boundary definitions are based on virtual addressing, so regardless of what
  * is implemented at the lower layer of the filesystem, these boundaries are
  * valid.
  */

#define OCTETS_IN_U16           (2 / sizeof(ot_u16))
#define OCTETS_IN_vlFSHEADER    (sizeof(vlFSHEADER) * OCTETS_IN_U16)
#define OCTETS_IN_vl_header_t   (sizeof(vl_header_t) * OCTETS_IN_U16)

/// Virtual Address Shortcuts for the header blocks (VWORM)
#define GFB_Header_START        ( (OVERHEAD_START_VADDR) + OCTETS_IN_vlFSHEADER )
#define GFB_Header_START_USER   ( (GFB_Header_START) + (GFB_NUM_STOCK_FILES*OCTETS_IN_vl_header_t) )
#define ISF_Header_START        ( (GFB_Header_START) + (GFB_NUM_FILES*OCTETS_IN_vl_header_t) )
#define ISF_Header_START_USER   ( (ISF_Header_START) + (ISF_NUM_STOCK_FILES*OCTETS_IN_vl_header_t) )

/// GFB HEAP Virtual address shortcuts (VWORM)
#define GFB_HEAP_START          GFB_START_VADDR
#define GFB_HEAP_USER_START     (GFB_START_VADDR+(GFB_NUM_STOCK_FILES*GFB_FILE_BYTES))
#define GFB_HEAP_END            (GFB_START_VADDR+GFB_TOTAL_BYTES)

/// ISF HEAP Virtual address shortcuts (VWORM)
#define ISF_HEAP_START          ISF_START_VADDR
#define ISF_HEAP_STOCK_START    ISF_START_VADDR
#define ISF_HEAP_USER_START     (ISF_START_VADDR+ISF_VWORM_STOCK_BYTES)
#define ISF_HEAP_END            (ISF_START_VADDR+ISF_TOTAL_BYTES)

/// ISF MIRROR Virtual address shortcuts (VSRAM)
#if (ISF_MIRROR_HEAP_BYTES > 0)
#   define ISF_MIRROR_BASE      VSRAM_BASE_VADDR
#   undef VSRAM_USED
#   define  VSRAM_USED          1
#endif










typedef ot_u8 (*sub_check)(ot_u8);
typedef vaddr (*sub_vaddr)(ot_u8);
typedef vlFILE* (*sub_new)(ot_u8, ot_u8, ot_u8);


/** VWORM Memory Allocation
  * Base positions and maximum group allocations for data files stored in
  * VWORM.  The values are taken from platform.h.
  * Direct addressing may only work for reads, depending on the nature of the
  * VWORM memory.  NAND Flash, for example, cannot be written to arbitrarily.
  */
#if (CC_SUPPORT == SIM_GCC)
    ot_u16* VWORM_Heap;
    ot_u16* VSRAM_Heap;
#endif



// Private Functions
vlFILE* sub_gfb_new(ot_u8 id, ot_u8 mod, ot_u8 null_arg);
vlFILE* sub_isf_new(ot_u8 id, ot_u8 mod, ot_u8 max_length);
ot_u8 sub_gfb_delete_check(ot_u8 id);
ot_u8 sub_isf_delete_check(ot_u8 id);
vaddr sub_gfb_search(ot_u8 id);
vaddr sub_isf_search(ot_u8 id);


/** @brief Performs mirroring operations on ISF files
  * @param direction : (ot_u8) vworm->vsram or vsram->vworm
  * @retval ot_u8
  *
  * For @c direction @c 0 is vworm->vsram, and non-zero is vsram->vworm
  */
ot_u8 sub_isf_mirror(ot_u8 direction);




vlFILE* sub_new_fp();
vlFILE* sub_new_file(vl_header_t* new_header, vaddr heap_base, vaddr heap_end, vaddr header_base, ot_int header_window );
void sub_delete_file(vaddr del_header);
void sub_copy_header( vaddr header, ot_u16* output_header );

/** @brief Writes a block of data to the header
  * @param addr : (ot_u8*) physical address of the start of the write
  * @param data : (ot_u16*) pointer to half-word aligned data
  * @param length : (ot_uint) number of BYTES to write
  * @retval none
  *
  * @note @c addr @c parameter should be half word aligned (i.e. even).
  *       Behavior is not guaranteed with non half-word aligned addresses
  */
void sub_write_header(vaddr header, ot_u16* data, ot_uint length );



/** @brief Searches for the first empty header
  * @param start_base : (vaddr*) physical pointer to the @c base @c file of a header
  * @param header_size : (size_t) number of bytes in the given header type
  * @param num_headers : (ot_int) number of headers to search through
  * @retval ot_int : Number of headers searched through until empty was found.
  *                  -1 if no headers were found.
  *
  * The return value corresponds to an index in the header array.  For example
  * XXX_Header_Array[retval + offset] is the index of the empty header, where
  * offset is the index of the starting header.
  *
  * @c sub_find_empty_header() @c will always return the nearest header to the front
  * of the array.
  */
vaddr sub_find_empty_header(vaddr header, ot_int num_headers);



/** @brief Searches for an amount of the empty space in the heap
  * @param base : (vaddr*) physical pointer to the @c base @c file of a header
  * @param alloc: (size_t) number of bytes needed to allocate
  * @param header_size : (size_t) number of bytes in the given header type
  * @param num_headers : (ot_int) number of headers to search through
  * @retval vaddr : virtual address of the spot in heap to put data.
  *                 returns @c NULL_vaddr @c if heap has no room
  *
  * @c sub_find_empty_heap() @c runs in quadratic time based on the @c num_headers
  * parameter.  Implementations on systems that are less memory constrained
  * could be made faster.  @c sub_find_empty_heap() @c only runs when adding a ISF
  * file, which may never even happen.
  */
vaddr sub_find_empty_heap(  vaddr heap_base, vaddr heap_end,
                        vaddr header, ot_uint new_alloc, ot_int num_headers);



/** @brief Defragments a given heap space
  * @param base : (vaddr) base vaddr of the heap space to defragment
  * @param size : (size_t) number of bytes in heap space
  * @retval vaddr : Non-zero on failure
  *
  * Run this function to clean out fragmented empty space in a heap.  For
  * example, if @c sub_find_empty_heap() @c returns NULL_vaddr
  *
  * Not implemented at time being.
  */
ot_u8 sub_defragment_heap(vaddr base, ot_uint window);



/** @brief Searches for a header of given ID
  * @param search_id : (ot_u8) ID value to look for
  * @param start_id: (ot_u8*) pointer to first ID to start searching
  * @param header_size : (size_t) number of bytes in the given header type
  * @param num_headers : (ot_int) number of headers to search through
  * @retval ot_int : Number of headers searched through until ID was found.
  *                  -1 if no headers were found.
  */
vaddr sub_header_search(vaddr header, ot_u8 search_id, ot_int num_headers);










// Public Functions


// Veelite init function
#if (CC_SUPPORT == SIM_GCC)
#   include <otplatform.h>
#endif

#ifndef EXTF_vl_init
OT_WEAK ot_u8 vl_init(void* handle) {
    ot_int i;

    /// Initialize environment variables
    for (i=0; i<OT_PARAM(VLFPS); i++) {
        vlfile[i].header   = NULL_vaddr;
        vlfile[i].start    = 0;
        vlfile[i].length   = 0;
        vlfile[i].read     = NULL;
        vlfile[i].write    = NULL;
    }

    /// Initialize core
    /// @note This should be done already in platform_poweron()
    //vworm_init();

    // Copy to mirror
    ISF_loadmirror();

#if (CC_SUPPORT == SIM_GCC)
    // Set up memory files if using Simulator
    VWORM_Heap  = (ot_u16*)&(NAND.ubyte[VWORM_BASE_PHYSICAL]);
    //VSRAM_Heap  = (ot_u16*)&(NAND.ubyte[VSRAM_BASE_PHYSICAL]);    //vsram declared in veelite_core
#endif

    return 0;
}
#endif


// General File Functions
#ifndef EXTF_vl_get_fp
OT_WEAK vlFILE* vl_get_fp(ot_int fd) {
    return &vlfile[fd];
}
#endif


#ifndef EXTF_vl_get_fd
OT_WEAK ot_int vl_get_fd(vlFILE* fp) {
    ot_int fd;

    fd  = (ot_int)((vl_u8*)fp - (vl_u8*)vlfile);
    fd /= sizeof(vlFILE);

    return fd;
}
#endif


#ifndef EXTF_vl_new
OT_WEAK ot_u8 vl_new(vlFILE** fp_new, vlBLOCK block_id, ot_u8 data_id, ot_u8 mod, ot_uint max_length, id_tmpl* user_id) {
#if (OT_FEATURE(VLNEW) == ENABLED)
    vaddr header;
    sub_vaddr search_fn;
    sub_new   new_fn;

    /// 1. Authenticate, when it's not a su call:
    ///    Authentication failure error code is 0x04.
    if (user_id != NULL) {
        if ( auth_check(VL_ACCESS_USER, VL_ACCESS_W, user_id) == 0 ) {
            return 0x04;
        }
    }

    /// 2. Make sure the file does not already exist.
    ///    If it already exists, the error code is 0x02.
    ///    If the block is not available, the error code is 0xFF.
    block_id--;
    switch (block_id) {
        case 0: search_fn   = &sub_gfb_search;
                new_fn      = &sub_gfb_new;
                break;

        case 1: return 0xFF;
                //ISS features removed
                //search_fn   = &sub_isfs_search;
                //new_fn      = &sub_isfs_new;
                //break;

        case 2: search_fn   = &sub_isf_search;
                new_fn      = &sub_isf_new;
                break;

       default: return 0xFF;
    }

    header = search_fn(data_id);
    if (header != NULL_vaddr) {
        return 0x02;
    }

    /// 3. Create the new file.
    ///    Error code for inability to create file is 0x06.
    *fp_new = new_fn(data_id, mod, max_length);
    if (*fp_new == NULL) {
        return 0x06;
    }
    
    /// 4. Update the filesystem header, on successful file creation.
    {   vlBLOCKHEADER* block    = &vlfs.gfb;
        block[block_id].files  += 1;
    }

    return 0;

#else
    return 255;

#endif
}
#endif



#ifndef EXTF_vl_delete
OT_WEAK ot_u8 vl_delete(vlBLOCK block_id, ot_u8 data_id, id_tmpl* user_id) {
#if (OT_FEATURE(VLNEW) == ENABLED)
    vaddr header = NULL_vaddr;
    sub_vaddr   search_fn;
    sub_check   check_fn;

    /// 1. Get the header from the supplied Block ID & Data ID
    block_id--;
    switch (block_id) {
        case 0: check_fn    = &sub_gfb_delete_check;
                search_fn   = &sub_gfb_search;
                break;

        case 1: return 255;
                //isfs features removed
                //check_fn    = &sub_isfs_delete_check;
                //search_fn   = &sub_isfs_search;
                //break;

        case 2: check_fn    = &sub_isf_delete_check;
                search_fn   = &sub_isf_search;
                break;

       default: return 255;
    }

    if (check_fn(data_id) != 0) {
        header = search_fn(data_id);
    }

    /// 2. Bail if header is NULL
    if (header == NULL_vaddr) {
        return 0x01;
    }

    /// 3. Authenticate, when it's not a su call
    if (user_id != NULL) {
#   if !defined(__C2000__)
        ot_uni16 filemod;
        filemod.ushort = vworm_read(header + 4);
        if ( auth_check(filemod.ubyte[1], VL_ACCESS_RW, user_id) == 0 ) {
            return 0x04;
        }
#   else
        ot_u16 filemod = vworm_read(header + 4);
        if ( auth_check(BYTE1(filemod), VL_ACCESS_RW, user_id) == 0 ) {
            return 0x04;
        }
#   endif
    }
    
    /// 4. Delete the file, and update the fs header.
    sub_delete_file(header);
    {   vlBLOCKHEADER* block    = &vlfs.gfb;
        block[block_id].files  -= 1;
    }
    
    return 0;
    
#else
    return 255; //error, delete disabled

#endif
}
#endif



#ifndef EXTF_vl_getheader
OT_WEAK ot_u8 vl_getheader_vaddr(vaddr* header, vlBLOCK block_id, ot_u8 data_id, ot_u8 mod, id_tmpl* user_id) {

    /// 1. Get the header from the supplied Block ID & Data ID
    switch (block_id) {
        case VL_GFB_BLOCKID:    *header = sub_gfb_search(data_id);      break;
        
        //ISS features removed
        //case VL_ISS_BLOCKID:   *header = sub_isfs_search(data_id);     break;
        
        case VL_ISF_BLOCKID:    *header = sub_isf_search(data_id);      break;
        default:                return 255;
    }

    /// 2. Bail if header is NULL
    if (*header == NULL_vaddr) {
        return 0x01;
    }

    /// 3. Authenticate, when it's not a su call
    if (user_id != NULL) {
#   if !defined(__C2000__)
        ot_uni16 filemod;
        filemod.ushort = vworm_read(*header + 4);
        if ( auth_check(filemod.ubyte[1], mod, user_id) == 0 ) {
            return 0x04;
        }
#   else
        ot_u16 filemod;
        filemod = vworm_read(*header + 4);
        if ( auth_check(BYTE0(filemod), mod, user_id) == 0 ) {
            return 0x04;
        }
#   endif
    }

    return 0;
}
#endif



#ifndef EXTF_vl_getheader
OT_WEAK ot_u8 vl_getheader(vl_header_t* header, vlBLOCK block_id, ot_u8 data_id, ot_u8 mod, id_tmpl* user_id) {
    vaddr   header_vaddr = NULL_vaddr;
    ot_u8   output;

    output = vl_getheader_vaddr(&header_vaddr, block_id, data_id, mod, user_id);
    if (output == 0) {
        sub_copy_header( header_vaddr, (ot_u16*)header );
    }

    return output;
}
#endif



#ifndef EXTF_vl_open_file
OT_WEAK vlFILE* vl_open_file(vaddr header) {
    vlFILE* fp;

    fp = sub_new_fp();

    if (fp != NULL) {
        fp->header  = header;
        fp->alloc   = vworm_read(header + 2);               //alloc
        fp->idmod   = vworm_read(header + 4);
        fp->start   = vworm_read(header + 8);               //mirror base addr

        if (fp->start != NULL_vaddr) {
            ot_u16 mlen = fp->start;
            fp->start  += 2;
            fp->write   = &vsram_mark;
            fp->read    = &vsram_read;
            fp->length  = vsram_read(mlen);
        }
        else {
            fp->write   = &vworm_write;
            fp->read    = &vworm_read;
            fp->length  = vworm_read(header + 0);           //length
            fp->start   = vworm_read(header + 6);           //vworm base addr
        }
    }
    return fp;
}
#endif



#ifndef EXTF_vl_open
OT_WEAK vlFILE* vl_open(vlBLOCK block_id, ot_u8 data_id, ot_u8 mod, id_tmpl* user_id) {
    vaddr header = NULL_vaddr;

    if (vl_getheader_vaddr(&header, block_id, data_id, mod, user_id) == 0) {
        return vl_open_file(header);
    }
    return NULL;
}
#endif


///@todo [header]
#ifndef EXTF_vl_chmod
OT_WEAK ot_u8 vl_chmod(vlBLOCK block_id, ot_u8 data_id, ot_u8 mod, id_tmpl* user_id) {
    vaddr header = NULL_vaddr;
    ot_u8 output;

    output = vl_getheader_vaddr(&header, block_id, data_id, VL_ACCESS_RW, user_id);
    if (output == 0) {
#   if !defined(__C2000__)
        ot_uni16 idmod;
        idmod.ubyte[0]  = data_id;
        idmod.ubyte[1]  = mod;
        sub_write_header((header+4), &idmod.ushort, 2);
#   else
        ot_u16 idmod    = JOIN_2B(data_id, mod);
        sub_write_header((header+4), &idmod, 2);
#   endif
    }

    return output;
}
#endif



#ifndef EXTF_vl_read
OT_WEAK ot_u16 vl_read( vlFILE* fp, ot_uint offset ) {
    return fp->read( (ot_uint)(offset+fp->start) );
}
#endif



#ifndef EXTF_vl_write
OT_WEAK ot_u8 vl_write( vlFILE* fp, ot_uint offset, ot_u16 data ) {
    if (offset >= fp->alloc) {
        return 255;
    }
    if (offset >= fp->length) {
        fp->length = offset+2;
    }

    return fp->write( (offset+fp->start), data);
}
#endif


#ifndef EXTF_vl_memptr
///@todo have this bury into the driver layer to give the address of the 
///      file data.  Will return NULL if file is not in the mirror, which is 
///      the only place where it can be guaranteed to be in contiguous RAM.
OT_WEAK vl_u8* vl_memptr( vlFILE* fp ) {
    if (fp->read == &vsram_read) {
        return (vl_u8*)vsram_get(fp->start);
    }
    return NULL;
}
#endif



#ifndef EXTF_vl_load
OT_WEAK ot_uint vl_load( vlFILE* fp, ot_uint length, vl_u8* data ) {
    ot_uint     cursor;

    if (length > fp->length) {
        length = fp->length;
    }
    cursor      = fp->start;        // guaranteed to be 16 bit aligned
    length      = cursor+length;

#   if !defined(__C2000__)
    for (; cursor<length; cursor++) {
        ot_uni16 scratch;
        ot_u8 align = (cursor & 1);
        if (align == 0) {
            scratch.ushort = fp->read(cursor);
        }
        *data++ = scratch.ubyte[align];
    }
#   else
    for (; cursor<length; cursor+=2) {
        *data++ = fp->read(cursor);
    }
#   endif

    return (length - fp->start);
}
#endif


#ifndef EXTF_vl_store
OT_WEAK ot_u8 vl_store( vlFILE* fp, ot_uint length, vl_u8* data ) {
    ot_uint cursor;
    ot_u8   test;

    if (length > fp->alloc) {
        return 255;
    }

    fp->length  = length;
    cursor      = fp->start;
    length      = cursor+length;

    for (test=0; cursor<length; cursor+=2) {
#   if !defined(__C2000__)    
        ot_uni16 scratch;
        scratch.ubyte[0]    = *data++;
        scratch.ubyte[1]    = *data++;
        test               |= fp->write(cursor, scratch.ushort);
#   else
        test               |= fp->write(cursor, *data++);
#   endif
    }
    
    return test;
}
#endif


#ifndef EXTF_vl_append
OT_WEAK ot_u8 vl_append( vlFILE* fp, ot_uint length, vl_u8* data ) {
    ot_uint cursor;
    ot_u8   test = 255;

    length = (fp->length+length);
    if (length <= fp->alloc) {
        cursor      = fp->start + fp->length;
        fp->length  = length;
        length     += cursor;

        for (test=0; cursor<length; cursor+=2) {
#       if !defined(__C2000__)
            ot_uni16 scratch;
            scratch.ubyte[0]    = *data++;
            scratch.ubyte[1]    = *data++;
            test               |= fp->write(cursor, scratch.ushort);
#       else
            test               |= fp->write(cursor, *data++);
#       endif
        }
    }
    
    return test;
}
#endif


#ifndef EXTF_vl_close
OT_WEAK ot_u8 vl_close( vlFILE* fp ) {
    if (FP_ISVALID(fp)) {
        if (fp->read == &vsram_read) {
            ot_u16* mhead;
            mhead   = (ot_u16*)vsram_get(fp->start-2);
            *mhead  = (*mhead != fp->length) ? fp->length : *mhead;
        }
        else if (vworm_read(fp->header+0) != fp->length) {
            sub_write_header( (fp->header+0), &(fp->length), 2);
        }

        // Kill file attributes
        fp->start   = 0;
        fp->length  = 0;
        //fp->header  = NULL_vaddr;
        fp->read    = NULL;
        fp->write   = NULL;

        return 0;
    }
    return 255;
}
#endif



#ifndef EXTF_vl_checklength
OT_WEAK ot_uint vl_checklength( vlFILE* fp ) {
#   ifdef _VL_DEBUG
        return (fp->read != NULL) ? fp->length : 0;
#   else
        return fp->length;
#   endif
}
#endif



#ifndef EXTF_vl_checkalloc
OT_WEAK ot_uint vl_checkalloc( vlFILE* fp ) {
#   ifdef _VL_DEBUG
        return (fp->read != NULL) ? fp->alloc : 0;
#   else
        return fp->alloc;
#   endif
}
#endif






/// Public Block Functions
/// Usually these are just shortcuts

// ISF Mirroring
#define MIRROR_TO_SRAM      0x00
#define MIRROR_TO_FLASH     0xFF


OT_WEAK vlFILE* GFB_open_su( ot_u8 id ) {
#   if ( GFB_HEAP_BYTES > 0 )
        return vl_open(VL_GFB_BLOCKID, id, VL_ACCESS_SU, NULL);
#   else
        return NULL;
#   endif
}


OT_WEAK vlFILE* ISF_open_su( ot_u8 id ) {
    return vl_open(VL_ISF_BLOCKID, id, VL_ACCESS_SU, NULL);
}


OT_WEAK vlFILE* GFB_open( ot_u8 id, ot_u8 mod, id_tmpl* user_id ) {
#   if ( GFB_HEAP_BYTES > 0 )
        return vl_open(VL_GFB_BLOCKID, id, mod, user_id);
#   else
        return NULL;
#   endif
}


OT_WEAK vlFILE* ISF_open( ot_u8 id, ot_u8 mod, id_tmpl* user_id ) {
    return vl_open(VL_ISF_BLOCKID, id, mod, user_id);
}


OT_WEAK ot_u8 GFB_chmod_su( ot_u8 id, ot_u8 mod ) {
#   if ( GFB_HEAP_BYTES > 0 )
        return vl_chmod(VL_GFB_BLOCKID, id, mod, NULL);
#   else
        return ~0;
#   endif
}

OT_WEAK ot_u8 ISF_chmod_su( ot_u8 id, ot_u8 mod ) {
    return vl_chmod(VL_ISF_BLOCKID, id, mod, NULL);
}


OT_WEAK ot_u8 ISF_syncmirror() {
#   if (ISF_MIRROR_HEAP_BYTES > 0)
        return sub_isf_mirror(MIRROR_TO_FLASH);
#   else
        return 0;
#   endif
}

OT_WEAK ot_u8 ISF_loadmirror() {
#   if (ISF_MIRROR_HEAP_BYTES > 0)
        return sub_isf_mirror(MIRROR_TO_SRAM);
#   else
        return 0;
#   endif
}









/// Private Block Functions
vlFILE* sub_gfb_new(ot_u8 id, ot_u8 mod, ot_u8 null_arg) {
#if ((OT_FEATURE(VLNEW) == ENABLED) && ((GFB_HEAP_BYTES > 0) && (GFB_NUM_USER_FILES > 0)))
    vl_header_t  new_header;
    
#   if !defined(__C2000__)
    ot_uni16 idmod;
    idmod.ubyte[0]  = id;
    idmod.ubyte[1]  = mod;
#   else
    ot_u16 idmod    = JOIN_2B(id, mod);
#   endif

    // Fill vl_header_t
    new_header.length   = (ot_u16)0;
    new_header.alloc    = (ot_u16)GFB_FILE_BYTES;
#   if !defined(__C2000__)
    new_header.idmod    = idmod.ushort;
#   else
    new_header.idmod    = idmod;
#   endif
    new_header.mirror   = NULL_vaddr;

    // Find where to put the new data, and if heap is full
    return sub_new_file(&new_header,
                        GFB_HEAP_USER_START,
                        GFB_HEAP_END,
                        GFB_Header_START_USER,
                        GFB_NUM_USER_FILES   );
#else
    return NULL;
#endif
}



vlFILE* sub_isf_new(ot_u8 id, ot_u8 mod, ot_u8 max_length ) {
#if ((OT_FEATURE(VLNEW) == ENABLED) && (ISF_NUM_USER_FILES > 0))
    vl_header_t new_header;
    
#   if !defined(__C2000__)
    ot_uni16 idmod;
    idmod.ubyte[0]  = id;
    idmod.ubyte[1]  = mod;
#   else
    ot_u16 idmod    = JOIN_2B(id, mod);
#   endif

    // Fill vl_header_t
    new_header.length   = (ot_u16)0;
    new_header.alloc    = (ot_u16)max_length;
#   if !defined(__C2000__)
    new_header.idmod    = idmod.ushort;
#   else
    new_header.idmod    = idmod;
#   endif
    new_header.mirror   = NULL_vaddr;

    // determine amount of actual ISF allocation needed (keeping it even)
    new_header.alloc += 1;
    new_header.alloc &= ~1;

    // Find where to put the new data, and if heap is full
    return sub_new_file(&new_header,
                        ISF_HEAP_USER_START,
                        ISF_HEAP_END,
                        ISF_Header_START_USER,
                        ISF_NUM_USER_FILES );
#else
    return NULL;
#endif
}




ot_u8 sub_gfb_delete_check(ot_u8 id) {
#if ((OT_FEATURE(VLNEW) == ENABLED) && ((GFB_HEAP_BYTES > 0) && (GFB_NUM_USER_FILES > 0)))
    return ( id > GFB_NUM_STOCK_FILES );
#else
    return 0;
#endif
}



ot_u8 sub_isf_delete_check(ot_u8 id) {
#if ((OT_FEATURE(VLNEW) == ENABLED) && (ISF_NUM_USER_FILES > 0))
    return ((id >= (ISF_NUM_M1_FILES+ISF_NUM_M2_FILES)) && \
            (id < (256-ISF_NUM_EXT_FILES)) );
#else
    return 0;
#endif
}




vaddr sub_gfb_search(ot_u8 id) {
    return sub_header_search( GFB_Header_START, id, GFB_NUM_USER_FILES );
}



vaddr sub_isf_search(ot_u8 id) {
#   if (OT_FEATURE(VLNEW) == ENABLED)
    // Check IDs added by the user during runtime
    if ( (id >= (ISF_NUM_M1_FILES+ISF_NUM_M2_FILES)) && (id < (256-ISF_NUM_EXT_FILES)) ) {
        return sub_header_search(ISF_Header_START_USER, id, ISF_NUM_USER_FILES);
    }
#   endif

#   if (ISF_NUM_EXT_FILES != 0)
    // Translate EXT IDs to a contiguous indexing with Stock Files
    if (id > (255-ISF_NUM_EXT_FILES)) {
        id = (ot_u8)((ot_int)ISF_NUM_STOCK_FILES + (255-id));
    }
#   endif
    
    return (OCTETS_IN_vl_header_t * id) + ISF_Header_START;
}





ot_u8 sub_isf_mirror(ot_u8 direction) {
#if (ISF_MIRROR_HEAP_BYTES > 0)
#   error "should be no mirror"
    vaddr   header;
    vaddr   header_base;
    vaddr   header_alloc;
    vaddr   header_mirror;
    //vaddr   header_end;
    ot_int  i;
    ot_u16* mirror_ptr;

    // Go through ISF Header array
    header = ISF_Header_START;
    for (i=0; i<ISF_NUM_STOCK_FILES; i++, header+=OCTETS_IN_vl_header_t) {

        //get header data
        header_alloc    = vworm_read(header+2);
        header_base     = vworm_read(header+6);
        header_mirror   = vworm_read(header+8);

        // Copy vworm to mirror if there is a mirror
        // 0. Skip unmirrored or uninitialized, or unallocated files
        // 1. Resolve Mirror Length (in vsram it is right ahead of the data)
        // 2. Load/Save Mirror Data (header_alloc is repurposed)
        if ((header_mirror != NULL_vaddr) && (header_alloc  != 0)) {
        	mirror_ptr = (ot_u16*)vsram_get(header_mirror);
            if (direction == MIRROR_TO_SRAM) { // LOAD
                *mirror_ptr = vworm_read(header+0);
            }
            if (header_base == NULL_vaddr) {	// EXIT if file is mirror-only
            	continue;
            }
            if (direction != MIRROR_TO_SRAM) {  // SAVE
                vworm_write((header+0), *mirror_ptr);
            }

            header_alloc = header_base + *mirror_ptr;
            mirror_ptr++;
            for ( ; header_base<header_alloc; header_base+=2, mirror_ptr++) {
                if (direction == MIRROR_TO_SRAM) { 
                    *mirror_ptr = vworm_read(header_base);
                }
                else { 
                    vworm_write(header_base, *mirror_ptr); 
                }
            }
        }
    }
#endif
    return 0;
}







/// Generic Subroutines

vlFILE* sub_new_fp() {
#if (OT_PARAM(VLFPS) < 8)
    ot_int fd;

    for (fd=0; fd<OT_PARAM(VLFPS); fd++) {
        if (vlfile[fd].read == NULL)
            return &vlfile[fd];
    }
#else
        ///@todo do a binary search
#endif

    return NULL;
}


vlFILE* sub_new_file(vl_header_t* new_header, vaddr heap_base, vaddr heap_end, vaddr header_base, ot_int header_window ) {
#if (OT_FEATURE(VLNEW) == ENABLED)
    //vlFILE* fp;
    //vaddr   new_base    = 0;
    vaddr   header_addr = 0;

    // Find where to put the new header, and if it's full
    header_addr = sub_find_empty_header( header_base, header_window );
    if (header_addr == NULL_vaddr)
        return NULL;

    // Find where to put the new data, and if heap is full
    new_header->base = sub_find_empty_heap( heap_base,
                                            heap_end,
                                            header_base,
                                            (ot_uint)new_header->alloc,
                                            header_window );
    if (new_header->base == NULL_vaddr)
        return NULL;

    // Write header to the header array
    sub_write_header(header_addr, (ot_u16*)new_header, OCTETS_IN_vl_header_t);

    return vl_open_file( header_addr );
#else
    return NULL;
#endif
}



void sub_delete_file(vaddr del_header) {
#if (OT_FEATURE(VLNEW) == ENABLED)
    vaddr   header_base;
    ot_u16  header_alloc;

    header_alloc    = (ot_u16)vworm_read(del_header+2);
    header_base     = (vaddr)vworm_read(del_header+6);

    // Wipe the old data and mark header as deleted
    vworm_wipeblock(header_base, header_alloc);
    vworm_mark((del_header+2), 0);                //alloc
    vworm_mark((del_header+6), NULL_vaddr);       //base
#endif
}



vaddr sub_header_search(vaddr header, ot_u8 search_id, ot_int num_headers) {

    for (; num_headers > 0; num_headers--) {
#       if !defined(__C2000__)
        ot_uni16 idmod;
        ot_u16 base     = vworm_read(header + 6);
        idmod.ushort    = vworm_read(header + 4);
        if ( base != 0 && base != 0xFFFF) {
            if (idmod.ubyte[0] == search_id)
                return header;
        }
        
#       else
        ot_u16 base     = vworm_read(header + 6);
        ot_u16 idmod    = vworm_read(header + 4);
        if ( base != 0 && base != 0xFFFF) {
            if (BYTE0(idmod) == search_id)
                return header;
        }

#       endif

        header += OCTETS_IN_vl_header_t;
    }
    return NULL_vaddr;
}


void sub_copy_header( vaddr header, ot_u16* output_header ) {
    ot_int i;
    ot_int copy_length = (OCTETS_IN_vl_header_t / 2);

    for (i=0; i<copy_length; i++) {
        output_header[i] = vworm_read(header);
        header += 2;
    }
}


void sub_write_header(vaddr header, ot_u16* data, ot_uint length ) {
    ot_int i;

    for (i=0; i<length; i+=2, data++) {
        vworm_write( (header+i), *data);
    }
}


vaddr sub_find_empty_header(vaddr header, ot_int num_headers) {
    vaddr header_base;

    for (; num_headers>0; num_headers--) {
        header_base = vworm_read( (header+6) );

        if ( header_base == NULL_vaddr ) {
            return header;
        }
        header += OCTETS_IN_vl_header_t;
    }

    return NULL_vaddr;
}


/// @todo This checks out in testing, although it is a complex function that
///       probably merits further testing
vaddr sub_find_empty_heap(  vaddr heap_base, vaddr heap_end,
                        vaddr header, ot_uint new_alloc, ot_int num_headers) {
#if (OT_FEATURE(VLNEW) == ENABLED)
    //Search all header combinations to find:
    // - pairs of adjacent files in the heap
    // - gaps between these pairs
    // - the smallest gap that is big enough.
    vaddr   loop1           = header;
    vaddr   loop1_base;
    ot_uint loop1_alloc;
    vaddr   loop2;
    vaddr   loop2_base;

    ot_int  i;
    ot_int  j;
    ot_int  gap;
    ot_int  closest_gap     = (ot_int)(heap_end - heap_base);
    ot_int  bestfit_alloc   = (ot_int)(32767);
    vaddr   bestfit_base    = NULL_vaddr;

    for (i=0; i<num_headers; i++, loop1+=OCTETS_IN_vl_header_t ) {
        loop1_alloc = vworm_read(loop1 + 2);                                    // load alloc (max) from header
        loop1_base  = vworm_read(loop1 + 6);                                    // load base from header

        if ( loop1_base != NULL_vaddr ) {                                       // skip if header is deleted, or empty
            heap_base   = (vaddr)(loop1_base + loop1_alloc);
            loop2       = header;

            for (j=0; j<num_headers; j++, loop2+=OCTETS_IN_vl_header_t ) {
                loop2_base  = vworm_read(loop2 + 6);

                if ( (loop2_base != NULL_vaddr) && (loop2_base > heap_base) ) { // if header is valid ...
                    gap = (ot_int)(loop2_base - heap_base);                     // calc gap between the two files

                    if (gap < closest_gap) {                                    // The closest gap ...
                        closest_gap = gap;                                      // will be between two adjacent ...
                    }                                                           // files in the heap.
                }
            }
        }
        
        // If the gap between loop1 and next file is big enough for the data we need ...
        // and is smaller than other big-enough gaps then it is the gap we will write into.
        if ((closest_gap >= new_alloc) && (closest_gap < bestfit_alloc))  {
            bestfit_alloc   = closest_gap;
            bestfit_base    = heap_base;
        }

    }

    return bestfit_base;
#else
    return NULL_vaddr;
#endif
}




//Save sub_defragment_heap for a rainy day
ot_u8 sub_defragment_heap(vaddr base, ot_uint window) {
    return ~0;
}








#endif

