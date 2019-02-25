/*  Copyright 2010-2012, JP Norair
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
  * @file       otsys/veelite.h
  * @author     JP Norair
  * @version    R100
  * @date       31 July 2012
  * @brief      Veelite virtual data interface for ISO 18000-7 Mode 1/2
  * @ingroup    Veelite
  *
  * Veelite is a virtual data interface that provides access to the main data
  * types used in DASH7 Modes 1/2.  Veelite could be used for anything,
  * as long as the data structures used by DASH7 make sense for the application.
  * The data files are:
  * - ISF: Universal Data Block
  * - GFB: Raw Data Block
  *
  * Veelite uses a virtual memory method, implemented in Veelite Core.  The user
  * does not need to interface with functions from the core.
  *
  * Veelite = Virtual EEPROM Lightweight
  ******************************************************************************
  */

#ifndef __VEELITE_H
#define __VEELITE_H

#include <otstd.h>

#include <otlib/utils.h>
#include <otlib/alp.h>
#include <otsys/veelite_core.h>


#ifdef __C2000__
#   define vl_u8    ot_u16
#else
#   define vl_u8    ot_u8
#endif


/** @typedef vlFILE
  * The FILE structure for veelite.  Much like POSIX FILE, it is only ever used
  * by the client as the pointer vlFILE*
  */
typedef ot_u16 (*vlread_fn)(ot_uint);
typedef ot_u8  (*vlwrite_fn)(ot_uint, ot_u16);
  
  
typedef struct {
    vaddr       header;
    ot_u16      start;
    ot_u16      alloc;
    ot_u16      idmod;
    ot_u16      length;
    ot_u16      flags;
    vlread_fn   read;
    vlwrite_fn  write;
} vlFILE;




/** @typedef vl_header_t
  * The generic form of the header used for OpenTag data files, used for
  * ISF, and GFB.  The mirror field should be set to NULL_vaddr if
  * not used.
  *
  * ot_u16  length      length of data in bytes (0-255)
  * ot_u16  alloc:      max allocation (usually half-word aligned)
  * ot_u8   id:         ID, 0x00 - 0x0FF
  * ot_u8   privileges: 8 bit mod code
  * vaddr   base:       base (start) virtual address
  * vaddr   mirror:     base virtual address of vsram mirrored data (optional)
  */
typedef struct OT_PACKED {
    ot_u16  length;
    ot_u16  alloc;
    ot_u16  idmod;
    vaddr   base;
    vaddr   mirror;
#   if (OT_FEATURE(VLACTIONS) == ENABLED)
    ot_u16  actioncode;
#   endif
#   if (OT_FEATURE(VLMODTIME) == ENABLED)
    ot_u32  modtime;
#   endif
#   if (OT_FEATURE(VLACCTIME) == ENABLED)
    ot_u32  acctime;
#   endif
} vl_header_t;




/** @note vl_link type: this may seem redundant.  It is designed to sit
  *       in RAM, and be sorted and searched.  It only makes sense to use them
  *       if you have a lot of user-created files, which may be out-of-order
  *       or otherwise jumbled in memory, or if you have a machine that is not
  *       at all memory-constrained.  For the alpha/beta implementation, the
  *       vl_link feature data is not included.
  */

/** @typedef vl_link
  * The vl_link type is a file handle for block Data.  Typically, vl_link is
  * stored in volatile memory (SRAM).
  *
  * ot_u8   id:         ID value of the ISF Element (8 bits)
  * ot_u8   reserved:   reserved
  * vl_header_t* header: Pointer to Data file header   
  */
typedef struct {
#   ifdef __C2000__
    ot_u16      id;
#   else
    ot_u8       id;
    ot_u8       reserved;
#   endif
    vl_header_t*  header;
} vl_link;



/// Access Control parameters
#define VL_ACCESS_GUEST     (ot_u8)b00000111
#define VL_ACCESS_USER      (ot_u8)b00111000
#define VL_ACCESS_SU        (ot_u8)b00111111
#define VL_ACCESS_R         (ot_u8)b00100100
#define VL_ACCESS_W         (ot_u8)b00010010
#define VL_ACCESS_X         (ot_u8)b10001001
#define VL_ACCESS_RW        (VL_ACCESS_R | VL_ACCESS_W)
#define VL_ACCESS_CRYPTO    (ot_u8)b01000000

/// File Action flags
#define VL_FLAG_OPENED      (1<<0)
#define VL_FLAG_MODDED      (1<<1)
#define VL_FLAG_RESIZED     (1<<2)



#if (OT_FEATURE(VEELITE) == ENABLED)


// veelite functions

/** @brief initializes the veelite subsystem.  Run after SRAM resets.
  * @param handle       (void*) initialization handle.
  * @retval ot_u8       Returns zero on success, else an error code.
  * @ingroup Veelite
  *
  * The input parameter "handle" is RFU.  NULL may be used.
  */
ot_u8 vl_init(void* handle);



/** @brief Adds a File Action to a specified File
  * @param  block_id    (vlBLOCK) Block ID of file (GFB, ISFB, etc)
  * @param  data_id     (ot_u8) 0-255 file ID of file
  * @param  condition   (ot_u8) Condition to call action: OR'ed combination 
  *                     of VL_FLAG_OPENED, VL_FLAG_MODDED, VL_FLAG_RESIZED.
  * @param  action      (ot_procv) 
  * @retval ot_int      Returns non-negative on success.
  * @ingroup Veelite
  *
  */
ot_int vl_add_action(vlBLOCK block_id, ot_u8 data_id, ot_u8 condition, ot_procv action);



/** @brief Removes File Action from a specified file
  * @param  block_id    (vlBLOCK) Block ID of file (GFB, ISFB, etc)
  * @param  data_id     (ot_u8) 0-255 file ID of file
  * @retval None
  * @ingroup Veelite
  *
  */
void vl_remove_action(vlBLOCK block_id, ot_u8 data_id);



// Multi-FS functions
#if (OT_FEATURE(MULTIFS))
// Functions primarily for use with Multi-FS features.
ot_u8 vl_multifs_init(void** handle);

ot_u8 vl_multifs_deinit(void* handle);

ot_u8 vl_multifs_add(void* handle, void* newfsbase, const id_tmpl* fsid);

ot_u8 vl_multifs_del(void* handle, const id_tmpl* fsid);

ot_u8 vl_multifs_activeid(void* obj, id_tmpl* fsid);

/** @brief Switches to a new FS in the MultiFS system.
  * @param fsid         (id_tmpl*) Filesystem ID to switch to.  May be NULL.
  * @retval ot_u8       Returns zero on success, else an error code.
  * @ingroup Veelite
  *
  * fsid: the identity of the filesystem to switch to.  Providing NULL sets the
  * fs to the default fs of the device.
  *
  * The fsid input is used to specify a filesystem in a Multi-FS configuration.
  * Multi-FS configurations are enabled when OT_FEATURE_MULTIFS is enabled in
  * the application configuration.  If NULL is supplied, the default/local FS
  * is used.
  *
  * @note Multi-FS is useful for gateways that are managing proxy filesystems 
  * of devices they interact with.  Multi-FS usage on endpoints is possible but 
  * this is not the intent of it.
  */
ot_u8 vl_multifs_switch(void* handle, void** getfsbase, const id_tmpl* fsid);


ot_u8 vl_multifs_start(void* handle, void** getfsbase, id_tmpl* fsid);
ot_u8 vl_multifs_next(void* handle, void** getfsbase, id_tmpl* fsid);

#endif




/** @brief  Returns a pointer to the filesystem header (FS Header)
  * @param  fsid        (id_tmpl*) Filesystem ID.  May be NULL.
  * @retval vlFSHEADER* A veelite Filesystem Header pointer
  * @ingroup Veelite
  *
  * The fsid input is used to specify a filesystem in a Multi-FS configuration.
  * Multi-FS configurations are enabled when OT_FEATURE_MULTIFS is enabled in
  * the application configuration.  If NULL is supplied, the default/local FS
  * is used.
  *
  * The return value is a pointer to the FS Header data, or NULL if the fsid
  * supplied as input does not resolve to a Filesystem.
  *
  * @note Multi-FS is useful for gateways that are managing proxy filesystems 
  * of devices they interact with.  Multi-FS usage on endpoints is possible but 
  * this is not the intent of it.
  *
  * @note Reading data from the FS Header is mostly harmless.  Writing data to 
  * the FSHeader may or may not have any effect, and you shouldn't do it anyway
  * unless you know exactly what you're doing.
  */
const vlFSHEADER* vl_get_fsheader(const id_tmpl* fsid);

ot_u32 vl_get_fsalloc(const vlFSHEADER* fshdr);


// General File functions

/** @brief  Returns an active file pointer when supplied an active file descriptor
  * @param  fd          (ot_int) Active file descriptor
  * @retval vlFILE*     A veelite file pointer
  * @ingroup Veelite
  *
  * Behavior is undefined if vl_get_fp() is supplied with an inactive fd.
  */
vlFILE* vl_get_fp(ot_int fd);


/** @brief  Returns an active file descriptor when supplied an active file pointer
  * @param  fp          (vlFILE*) Active file pointer
  * @retval ot_int      file descriptor integer
  * @ingroup Veelite
  *
  * Behavior is undefined if vl_get_fd() is supplied with an inactive fp.
  */
ot_int  vl_get_fd(vlFILE* fp);


/** @brief  Creates a new file
  * @param  fp_new      (vlFILE**) A file pointer handle for new file
  * @param  block_id    (vlBLOCK) Block ID of new file (GFB, ISFB, etc)
  * @param  data_id     (ot_u8) 0-255 file ID of new file
  * @param  mod         (ot_u8) Permissions for new file
  * @param  max_length  (ot_uint) Maximum length for new file (alloc)
  * @param  user_id     (id_tmpl*) User ID that is trying to create new file
  * @retval ot_u8       Return code: 0 on success, non-zero on error
  * @ingroup Veelite
  *
  * Most of the parameters should be self-explanatory.  The user_id parameter is
  * usually passed from the source address of the DASH7 request (if the new file
  * request is coming from DASH7).  If the new file request is called internally
  * by the system (root), you can pass NULL, which will always be resolved to
  * root.
  *
  * The return value is a numerical code.
  * <LI>   0: Success                                           </LI>
  * <LI>   2: File already exists, can't create new one         </LI>
  * <LI>   4: User does not have access to create a new file    </LI>
  * <LI>   6: Not enough room for a new file                    </LI> 
  * <LI> 255: Miscellaneous Error                               </LI>
  */
ot_u8   vl_new(vlFILE** fp_new, vlBLOCK block_id, ot_u8 data_id, ot_u8 mod, ot_uint max_length, const id_tmpl* user_id);


/** @brief  Deletes a file
  * @param  block_id    (vlBLOCK) Block ID of file to delete (GFB, ISFB, etc)
  * @param  data_id     (ot_u8) 0-255 file ID of file to delete
  * @param  user_id     (id_tmpl*) User ID that is trying to create new file
  * @retval ot_u8       Return code: 0 on success, non-zero on error
  * @ingroup Veelite
  *
  * @note Files that are built-in to the DASH7 spec cannot be deleted.  Also,
  * other files declared at compile-time as "stock" files are protected against
  * deletion by any user.
  * 
  * Most of the parameters should be self-explanatory.  The user_id parameter is
  * usually passed from the source address of the DASH7 request (if the delete
  * request is coming from DASH7).  If the delete request is called internally
  * by the system (root), you can pass NULL, which will always be resolved to
  * root.
  *
  * The return value is a numerical code.
  * <LI>   0: Success                                           </LI>
  * <LI>   1: File could not be found                           </LI>
  * <LI>   4: User does not have access to create a new file    </LI>
  * <LI> 255: Miscellaneous Error                               </LI>
  */
ot_u8   vl_delete(vlBLOCK block_id, ot_u8 data_id, const id_tmpl* user_id);


/** @brief  Returns a file header as the vaddr of the header
  * @param  header      (vaddr*) Output header vaddr
  * @param  block_id    (vlBLOCK) Block ID of file header to get
  * @param  data_id     (ot_u8) 0-255 file ID of file header to get
  * @param  mod         (ot_u8) Method of access for file (read, write, etc)
  * @param  user_id     (id_tmpl*) User ID that is trying to create new file
  * @retval ot_u8       Return code: 0 on success, non-zero on error
  * @ingroup Veelite
  *
  * Most of the parameters should be self-explanatory.  The user_id parameter is
  * usually passed from the source address of the DASH7 request (if the request
  * is coming from DASH7).  If the request is called internally by the system 
  * (root), you can pass NULL, which will always be resolved to root.
  *
  * This function is intended for use with File ALP protocols.
  *
  * The return value is a numerical code.
  * <LI>   0: Success                                           </LI>
  * <LI>   1: File could not be found                           </LI>
  * <LI>   4: User does not have sufficient access to this file </LI>
  * <LI> 255: Miscellaneous Error                               </LI>
  */
ot_u8   vl_getheader_vaddr(vaddr* header, vlBLOCK block_id, ot_u8 data_id, ot_u8 mod, const id_tmpl* user_id);


/** @brief  Returns file header as a vl_header_t datastruct
  * @param  header      (vl_header_t*) Output header datastruct
  * @param  block_id    (vlBLOCK) Block ID of file header to get
  * @param  data_id     (ot_u8) 0-255 file ID of file header to get
  * @param  mod         (ot_u8) Method of access for file (read, write, etc)
  * @param  user_id     (id_tmpl*) User ID that is trying get header
  * @retval ot_u8       Return code: 0 on success, non-zero on error
  * @ingroup Veelite
  * @sa vl_getheader_vaddr()
  *
  * This function is a wrapper for vl_getheader_vaddr(), but it also copies the
  * header data into a vl_header_t struct that the user must allocate and supply.
  *
  * This function is intended for use with File ALP protocols.
  */
ot_u8   vl_getheader(vl_header_t* header, vlBLOCK block_id, ot_u8 data_id, ot_u8 mod, const id_tmpl* user_id);


/** @brief  Opens a file from the virtual address of its header
  * @param  header      (vaddr) virtual address of the file header to open
  * @retval vlFILE*     File Pointer (NULL on error)
  * @ingroup Veelite
  *
  * In order to be secure, do not use this function without first getting the
  * header from vl_getheader_vaddr(), which has user authentication.
  *
  * This function is intended for use with File ALP protocols.
  */
vlFILE* vl_open_file(vaddr header);


/** @brief  Get the Modification time of the file (seconds since epoch)
  * @param  vlFILE*     File Pointer of open file
  * @retval ot_u32      Second since epoch that file was last modified
  * @ingroup Veelite
  *
  * If mod time is not enabled on the Veelite build, then 0 will be returned.
  */
ot_u32 vl_getmodtime(vlFILE* fp);


/** @brief  Get the Access time of the file (seconds since epoch)
  * @param  vlFILE*     File Pointer of open file
  * @retval ot_u32      Second since epoch that file was last accessed
  * @ingroup Veelite
  *
  * If access time is not enabled on the Veelite build, then the modtime will be
  * returned.  If the modtime is not enabled on the Veelite build, then 0 will
  * be returned.
  */
ot_u32 vl_getacctime(vlFILE* fp);


/** @brief  Get the Modification time of the file (seconds since epoch)
  * @param  vlFILE*     File Pointer of open file
  * @param  ot_u32 		seconds since epoch for new time
  * @retval ot_u8		Zero on success
  * @ingroup Veelite
  *
  * vl_setmodtime() is not necessary to use.  vl_close() automatically sets file time
  * values.  vl_setmodtime() is a utility function.
  *
  * If modification time is not enabled on the Veelite build, then nothing will happen, 
  * and a non-zero value will be returned (255).
  */
ot_u8 vl_setmodtime(vlFILE* fp, ot_u32 newtime);


/** @brief  Set the Access time of the file (seconds since epoch)
  * @param  vlFILE*     File Pointer of open file
  * @param  ot_u32 		seconds since epoch for new time
  * @retval ot_u8		Zero on success
  * @ingroup Veelite
  *
  * vl_setacctime() is not necessary to use.  vl_close() automatically sets file time
  * values.  vl_setacctime() is a utility function.
  *
  * If access time is not enabled on the Veelite build, then nothing will happen, and a
  * non-zero value will be returned (255).
  */
ot_u8 vl_setacctime(vlFILE* fp, ot_u32 newtime);



/** @brief  Normal File Open Function
  * @param  block_id    (vlBLOCK) Block ID of file to open
  * @param  data_id     (ot_u8) 0-255 file ID of file to open
  * @param  mod         (ot_u8) Method of access for file (read, write, etc)
  * @param  user_id     (id_tmpl*) User ID that is trying to open file
  * @retval vlFILE*     File Pointer (NULL on error)
  * @ingroup Veelite
  *
  * This is the normal function to use to open a file in Veelite.  The other
  * methods for opening are for special cases (they exist to make the filedata
  * ALP run faster and smaller).
  *
  * There are several, specialized alias functions for vl_open().  These should
  * only be used internally (not in protocol routines).  The function aliases 
  * that contain "_su" at the end are super-user (root) calls.
  */
vlFILE* vl_open(vlBLOCK block_id, ot_u8 data_id, ot_u8 mod, const id_tmpl* user_id);
vlFILE* GFB_open_su( ot_u8 id );
vlFILE* ISF_open_su( ot_u8 id );
vlFILE* GFB_open( ot_u8 id, ot_u8 mod, const id_tmpl* user_id );
vlFILE* ISF_open( ot_u8 id, ot_u8 mod, const id_tmpl* user_id );


/** @brief  File Change Mods (like chmod on POSIX)
  * @param  block_id    (vlBLOCK) Block ID of file to chmod
  * @param  data_id     (ot_u8) 0-255 file ID of file to chmod
  * @param  mod         (ot_u8) New mod setting
  * @param  user_id     (id_tmpl*) User ID that is trying to chmod
  * @retval ot_u8     File Pointer (NULL on error)
  * @ingroup Veelite
  *
  * The file mod value in Veelite is one byte.  It looks like this.         <BR>
  * B7: Encryption bit      : (Proprietary) The file data is encrypted      <BR>
  * B6: Runable             : (Proprietary) The file data contains a program<BR>
  * B5: User Read           : User can read                                 <BR>
  * B4: User Write          : User can write                                <BR>
  * B3: Run                 : User can run (if file is runable)             <BR>
  * B2: Guest Read          : Guest can read                                <BR>
  * B1: Guest Write         : Guest can write                               <BR>
  * B0: Guest Run           : Guest can run (if file is runable)            <BR>
  *
  * The return value is a numerical code.
  * <LI>   0: Success                                           </LI>
  * <LI>   1: File could not be found                           </LI>
  * <LI>   4: User does not have sufficient access to this file </LI>
  * <LI> 255: Miscellaneous Error                               </LI>
  * 
  * There are several, specialized alias functions for vl_chmod().  These should
  * only be used internally (not in protocol routines).  The function aliases 
  * that contain "_su" at the end are super-user (root) calls.
  */
ot_u8 vl_chmod(vlBLOCK block_id, ot_u8 data_id, ot_u8 mod, const id_tmpl* user_id);
ot_u8 GFB_chmod_su( ot_u8 id, ot_u8 mod );
ot_u8 ISF_chmod_su( ot_u8 id, ot_u8 mod );



/** @brief  Touch file to update access modification times (like touch in POSIX)
  * @param  block_id    (vlBLOCK) Block ID of file to chmod
  * @param  data_id     (ot_u8) 0-255 file ID of file to chmod
  * @param  flags       (ot_u8) supply VL_FLAG_MODDED to affect modification time
  * @param  user_id     (id_tmpl*) User ID that is trying to chmod
  * @retval ot_u8       0 on success
  * @ingroup Veelite
  *
  * File Access time will always be updated to present time.  File Modification
  * time will also be updated if the modification flag is supplied.
  */
ot_u8 vl_touch(vlBLOCK block_id, ot_u8 data_id, ot_u8 flags, const id_tmpl* user_id);



/** @brief  Reads 16 bits at a time from the open file (GFB, ISF)
  * @param  fp          (vlFILE*) file pointer of open file
  * @param  offset      (ot_uint) byte offset into the file
  * @retval (ot_u16)    16 bits data from the given offset
  * @ingroup Veelite
  *
  * Odd offset values are rounded down.
  */
ot_u16 vl_read( vlFILE* fp, ot_uint offset );


/** @brief  Writes 16 bits at a time to the open file (GFB, ISF)
  * @param  fp          (vlFILE*) file pointer of open file
  * @param  offset      (ot_uint) byte offset into the file
  * @param  data        (ot_u16) 16 bits data to write
  * @retval (ot_u8)     Non-zero on failure
  * @ingroup Veelite
  *
  * Note: to write a odd number of bytes, supply an odd value for
  * offset.  The odd byte in data (upper byte in little endian, lower byte in
  * big endian, or UPPER byte in OpenTag TwoBytes data union) will be written
  * and the even byte will be discarded.
  */
ot_u8 vl_write( vlFILE* fp, ot_uint offset, ot_u16 data );



/** @brief  Loads the contents of a file into a supplied byte-buffer
  * @param  fp          (vlFILE*) file pointer of open file
  * @param  length      (ot_uint) number of bytes to load, starting from beginning of file
  * @param  data        (ot_u8*) byte buffer to load into
  * @retval (ot_uint)   Number of bytes loaded into byte-buffer
  * @ingroup Veelite
  *
  * This function will not read more bytes than the current length of the file.
  */
ot_uint vl_load( vlFILE* fp, ot_uint length, vl_u8* data );



/** @brief  Stores supplied byte-buffer into a file, replacing existing contents
  * @param  fp          (vlFILE*) file pointer of open file
  * @param  length      (ot_uint) number of bytes to store, starting from beginning of file
  * @param  data        (ot_u8*) byte buffer to write to file
  * @retval (ot_u8)     Non-zero on failure
  * @ingroup Veelite
  */
ot_u8 vl_store( vlFILE* fp, ot_uint length, vl_u8* data );
ot_u8 vl_append( vlFILE* fp, ot_uint length, vl_u8* data );


/** @brief  Returns a pointer to the start of the file data.  Use with caution.
  * @param  fp          (vlFILE*) file pointer of open file
  * @retval (ot_u8*)    Returns pointer to file data, or NULL on error
  * @ingroup Veelite
  */
vl_u8* vl_memptr( vlFILE* fp );







/** @brief  Crops (or erases) a file contents without deleting the file
  * @param  fp          (vlFILE*) file pointer of open file
  * @param  offset      (ot_uint) file data offset to crop after
  * @retval (ot_u8)     Non-zero on failure
  * @ingroup Veelite
  *
  * vl_crop() can erase partial contents of a file.  What it does is set the
  * fp->length to a certain value and also alter the hard file header to the 
  * same value.
  *
  * vl_erase() is a wrapper that calls vl_crop(fp, 0)
  
ot_u8 vl_crop(vlFILE* fp, ot_uint offset);
ot_u8 vl_erase(vlFILE* fp);
*/


/** @brief Closes the data read/write session of the open file
  * @param none
  * @retval (ot_u8) : Non-zero on failure
  * @ingroup Veelite
  */
ot_u8 vl_close( vlFILE* fp );

/** @brief Returns the length of the open file (GFB, ISF)
  * @param none
  * @retval (ot_uint) : length in bytes
  * @ingroup Veelite
  */
ot_uint vl_checklength( vlFILE* fp );

/** @brief Returns the length of the open file (GFB, ISF)
  * @param none
  * @retval (ot_uint) : length in bytes
  * @ingroup Veelite
  */
ot_uint vl_checkalloc( vlFILE* fp );






/** @brief Syncs main ISF file data with the data in the mirror
  * @param none
  * @retval ot_u8 : Non-zero on failure
  * @ingroup Veelite
  *
  * Only works on ISF files that are mirrored.  In certain implementations,
  * this function may do nothing at all.  It should really only be used by the
  * root user.
  */
ot_u8 ISF_syncmirror( );

/** @brief loads file data from vworm to data in the mirror
  * @param none
  * @retval ot_u8 : Non-zero on failure
  * @ingroup Veelite
  *
  * Only works on ISF files that are mirrored.  In certain implementations,
  * this function may do nothing at all.
  */
ot_u8 ISF_loadmirror( );











/* @brief Writes the M1TAG struct, which bears certain Mode 1 config data.
  * @param new_m1tag : (M1TAG_struct*) pointer to struct bearing the new data.
  * @retval ot_u8 : Non-zero on failure
  * @ingroup Veelite
  * 
  * M1TAG_write() is typically only used or needed at the time of manufacture or
  * "commission."
  *
ot_u8 M1TAG_write( M1TAG_struct* new_m1tag );

* @brief Gets a copy of the M1TAG struct
  * @param m1tag : (M1TAG_struct*) pointer to struct to return
  * @retval ot_u8 : Non-zero on failure
  * @ingroup Veelite
  * 
  * the m1tag argument must be allocated by the caller, otherwise you will 
  * almost certainly run into a segmentation fault or similar problem!
  *
ot_u8 M1TAG_get( M1TAG_struct* m1tag );
*/



#endif // if (OT_FEATURE(VEELITE) == ENABLED)
#endif // ifndef __VEELITE_H





