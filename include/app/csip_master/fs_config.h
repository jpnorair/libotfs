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
  * @file       /app/csip_master/fs_config.h
  * @author     JP Norair (jpnorair@indigresso.com)
  * @version    R101
  * @date       30 Apr 2017
  * @brief      Configuration for typical demo filesystem
  *
  ******************************************************************************
  */

#ifndef __FS_CONFIG_H
#define __FS_CONFIG_H

#include "build_config.h"
#include "features_config.h"

/// MultiFS feature requires that active data is stored in RAM.
/// MultiFS is not for embedded devices.
#if OT_FEATURE(MULTIFS)
#   undef __VLSRAM__
#   define __VLSRAM__
#endif

/// VLSRAM puts all the filesystem in RAM (not necessarily SRAM, but whatever),
/// so additional mirroring into SRAM is disabled.
#if defined(__VLSRAM__)
#   define _NOMIRROR    1
#else
#   define _NOMIRROR    0
#endif



/** Veelite Addressing constants
  * For each of the three types of virtual memory, plus mirroring, which is
  * supported by ISFB files.  Mirroring stores a copy of the IFSB data in
  * RAM (see veelite.h, veelite.c, veelite_core.h, veelite_core.c)
  */

#define VL_WORD             2
#define _ALLOC_OFFSET       (VL_WORD-1)
#define _ALLOC_SHIFT        1
#define _MIRALLOC_OFFSET    _ALLOC_OFFSET
#define _MIRALLOC_SHIFT     _ALLOC_SHIFT



/** 1. File Definition Section <BR>
  * ======================================================================== <BR>
  */

/** ISFB Structure
 *
 * Here is the breakdown of this FS layout:
 * <LI> 0x00 to 0x0F: Standard files for all OTFS/Veelite </LI>
 * <LI> 0x10 to 0x1F: Files to support Common Smart Inverter Profile </LI>
 * <LI> 0x20 to 0x7F: Reserved for future use </LI>
 * <LI> 0x80 to 0x9F: Extended Services Data (not used in this implementation) </LI>
 * <LI> 0xA0 to 0xFE: Proprietary </LI>
 * <LI> 0xFF: OTFS/Veelite Standard "Application Extension" file </LI>
 *
 * Some files have allocations less than 255 bytes.  Many of the files from IDs
 * 0x00 to 0x1F have limited allocations because they are config registers.
 *
 * There are several types of MACROS for handling ISFB constants.  To use, put
 * the name of the ISF into the argument, such as:
 * @c ISF_ID(network_settings) @c
 *
 * The macros are:
 * <LI> @c ISF_ID(file_name) @c :     File ID (0-255) </LI>
 * <LI> @c ISF_MOD(file_name) @c :    File Privilege bitmask (1 byte) </LI>
 * <LI> @c ISF_LEN(file_name) @c :    File Length (0-255) </LI>
 * <LI> @c ISF_MAX(file_name) @c :    Maximum Length of the file Data (0-255) </LI>
 * <LI> @c ISF_ALLOC(file_name) @c :  Allocated Bytes for file (0-256) </LI>
 */

#define ISF_OTFS_FILES      16
#define ISF_CSIP_FILES      9
#define ISF_APPEXT_FILES    1

#define ISF_ID(VAL)                             ISF_ID_##VAL
// OTFS STANDARD FILES -----------------------------------------------------
#define ISF_ID_network_settings                 0x00
#define ISF_ID_device_features                  0x01
#define ISF_ID_channel_configuration            0x02    // Std File, but not used
#define ISF_ID_real_time_scheduler              0x03    // Std File, but not used
#define ISF_ID_hold_scan_sequence               0x04    // Std File, but not used
#define ISF_ID_sleep_scan_sequence              0x05    // Std File, but not used
#define ISF_ID_beacon_transmit_sequence         0x06    // Std File, but not used
#define ISF_ID_isf_list                         0x07
#define ISF_ID_iss_list                         0x08    // Std File, but not used
#define ISF_ID_gfb_file_list                    0x09
#define ISF_ID_location_data_list               0x0A    // Std File, but not used
#define ISF_ID_ipv6_addresses                   0x0B
#define ISF_ID_sensor_list                      0x0C    // Std File, but not used
#define ISF_ID_sensor_alarms                    0x0D    // Std File, but not used
#define ISF_ID_root_authentication_key          0x0E
#define ISF_ID_user_authentication_key          0x0F
// OTFS CSIP FILES -----------------------------------------------------
#define ISF_ID_pwrdata_inv                      0x10
#define ISF_ID_pwrdata_mppt                     0x11
#define ISF_ID_csip_nameplate_inv               0x12
#define ISF_ID_csip_nameplate_mppt              0x13
#define ISF_ID_csip_cfg                         0x14
#define ISF_ID_csip_setarray                    0x15
#define ISF_ID_csip_issuecmd                    0x16
#define ISF_ID_csip_operations                  0x17
#define ISF_ID_csip_issuecontrol                0x18
// OTFS STANDARD APP EXT FILE -----------------------------------------------
#define ISF_ID_application_extension            0xFF


/// ISF Mirror Enabling: <BR>
/// ISFB files can be mirrored in RAM.  Set to 0/1 to Disable/Enable each file
/// mirror.  Mirroring speeds-up file access, but it can consume a lot of RAM.
#define __SET_MIRROR(VAL)                       (VAL && !_NOMIRROR)
#define ISF_ENMIRROR(VAL)                       ISF_ENMIRROR_##VAL
#define ISF_ENMIRROR_network_settings           __SET_MIRROR(1)
#define ISF_ENMIRROR_device_features            __SET_MIRROR(1)
#define ISF_ENMIRROR_channel_configuration      __SET_MIRROR(0)
#define ISF_ENMIRROR_real_time_scheduler        __SET_MIRROR(1)
#define ISF_ENMIRROR_hold_scan_sequence         __SET_MIRROR(0)
#define ISF_ENMIRROR_sleep_scan_sequence        __SET_MIRROR(0)
#define ISF_ENMIRROR_beacon_transmit_sequence   __SET_MIRROR(0)
#define ISF_ENMIRROR_isf_list                   __SET_MIRROR(1)
#define ISF_ENMIRROR_iss_list                   __SET_MIRROR(0)
#define ISF_ENMIRROR_gfb_file_list              __SET_MIRROR(1)
#define ISF_ENMIRROR_location_data_list         __SET_MIRROR(0)
#define ISF_ENMIRROR_ipv6_addresses             __SET_MIRROR(1)
#define ISF_ENMIRROR_sensor_list                __SET_MIRROR(0)
#define ISF_ENMIRROR_sensor_alarms              __SET_MIRROR(0)
#define ISF_ENMIRROR_root_authentication_key    __SET_MIRROR(1)
#define ISF_ENMIRROR_user_authentication_key    __SET_MIRROR(1)
// OTFS CSIP FILES -----------------------------------------------------
#define ISF_ENMIRROR_pwrdata_inv                __SET_MIRROR(1)
#define ISF_ENMIRROR_pwrdata_mppt               __SET_MIRROR(1)
#define ISF_ENMIRROR_csip_nameplate_inv         __SET_MIRROR(1)
#define ISF_ENMIRROR_csip_nameplate_mppt        __SET_MIRROR(1)
#define ISF_ENMIRROR_csip_cfg                   __SET_MIRROR(1)
#define ISF_ENMIRROR_csip_setarray              __SET_MIRROR(1)
#define ISF_ENMIRROR_csip_issuecmd              __SET_MIRROR(1)
#define ISF_ENMIRROR_csip_operations            __SET_MIRROR(1)
#define ISF_ENMIRROR_csip_issuecontrol          __SET_MIRROR(1)
// OTFS STANDARD APP EXT FILE -----------------------------------------------
#define ISF_ENMIRROR_application_extension      __SET_MIRROR(1)


/// ISF file default privileges                                     <BR>
/// Mod Byte: EXrwxrwx                                              <BR>
/// root can always read & write, and he can execute when X is 1    <BR>
/// E:          data is encrypted in storage (not supported atm)    <BR>
/// X:          data is executable (a program)                      <BR>
/// 1st rwx:    read/write/exec for user                            <BR>
/// 2nd rwx:    read/write/exec for guest
#ifdef __DEBUG__
#   define _DEBUG_FILEMOD  b00110110
#else
#   define _DEBUG_FILEMOD  0
#endif

#define ISF_MOD(VAL)                            ISF_MOD_##VAL
#define ISF_MOD_file_standard                   b00000000
#define ISF_MOD_network_settings                b00110100
#define ISF_MOD_device_features                 b00100100
#define ISF_MOD_channel_configuration           ISF_MOD_file_standard
#define ISF_MOD_real_time_scheduler             ISF_MOD_file_standard
#define ISF_MOD_hold_scan_sequence              ISF_MOD_file_standard
#define ISF_MOD_sleep_scan_sequence             ISF_MOD_file_standard
#define ISF_MOD_beacon_transmit_sequence        ISF_MOD_file_standard
#define ISF_MOD_isf_list                        b00100100
#define ISF_MOD_iss_list                        ISF_MOD_file_standard
#define ISF_MOD_gfb_file_list                   b00100100
#define ISF_MOD_location_data_list              ISF_MOD_file_standard
#define ISF_MOD_ipv6_addresses                  b00100100
#define ISF_MOD_sensor_list                     ISF_MOD_file_standard
#define ISF_MOD_sensor_alarms                   ISF_MOD_file_standard
#define ISF_MOD_root_authentication_key         b00000000
#define ISF_MOD_user_authentication_key         b00110000
// OTFS CSIP FILES -----------------------------------------------------
#define ISF_MOD_pwrdata_inv                     (b00100100 | _DEBUG_FILEMOD)
#define ISF_MOD_pwrdata_mppt                    (b00100100 | _DEBUG_FILEMOD)
#define ISF_MOD_csip_nameplate_inv              (b00110100 | _DEBUG_FILEMOD)
#define ISF_MOD_csip_nameplate_mppt             (b00110100 | _DEBUG_FILEMOD)
#define ISF_MOD_csip_cfg                        (b00110000 | _DEBUG_FILEMOD)
#define ISF_MOD_csip_setarray                   (b00110000 | _DEBUG_FILEMOD)
#define ISF_MOD_csip_issuecmd                   (b00110000 | _DEBUG_FILEMOD)
#define ISF_MOD_csip_operations                 (b00010000 | _DEBUG_FILEMOD)
#define ISF_MOD_csip_issuecontrol               (b00110000 | _DEBUG_FILEMOD)
// OTFS STANDARD APP EXT FILE -----------------------------------------------
#define ISF_MOD_application_extension           b00100100


/// Stock ISF file max data lengths (not aligned, just max)
#define ISF_MAX(VAL)                            ISF_MAX_##VAL
#define ISF_MAX_USER_FILE                       255
#define ISF_MAX_network_settings                10
#define ISF_MAX_device_features                 48
#define ISF_MAX_channel_configuration           0
#define ISF_MAX_real_time_scheduler             12
#define ISF_MAX_hold_scan_sequence              0
#define ISF_MAX_sleep_scan_sequence             0
#define ISF_MAX_beacon_transmit_sequence        0
#define ISF_MAX_isf_list                        24  //24 isf
#define ISF_MAX_iss_list                        0
#define ISF_MAX_gfb_file_list                   8   //8 gfb files
#define ISF_MAX_location_data_list              0
#define ISF_MAX_ipv6_addresses                  48
#define ISF_MAX_sensor_list                     0
#define ISF_MAX_sensor_alarms                   0
#define ISF_MAX_root_authentication_key         22
#define ISF_MAX_user_authentication_key         22
#define ISF_MAX_pwrdata_inv                     50
#define ISF_MAX_pwrdata_mppt                    40
#define ISF_MAX_csip_nameplate_inv              96
#define ISF_MAX_csip_nameplate_mppt             8
#define ISF_MAX_csip_cfg                        176
#define ISF_MAX_csip_setarray                   84
#define ISF_MAX_csip_issuecmd                   24
#define ISF_MAX_csip_operations                 16
#define ISF_MAX_csip_issuecontrol               4
#define ISF_MAX_application_extension           256


/// ISF file default length:
/// Some of these elements must get written-to at startup
#define ISF_LEN(VAL)                            ISF_LEN_##VAL
#define ISF_LEN_network_settings                10
#define ISF_LEN_device_features                 48
#define ISF_LEN_channel_configuration           0
#define ISF_LEN_real_time_scheduler             12
#define ISF_LEN_hold_scan_sequence              0
#define ISF_LEN_sleep_scan_sequence             0
#define ISF_LEN_beacon_transmit_sequence        0
#define ISF_LEN_isf_list                        (ISF_CSIP_FILES + ISF_APPEXT_FILES)
#define ISF_LEN_iss_list                        0
#define ISF_LEN_gfb_file_list                   GFB_NUM_FILES
#define ISF_LEN_location_data_list              0
#define ISF_LEN_ipv6_addresses                  0
#define ISF_LEN_sensor_list                     0
#define ISF_LEN_sensor_alarms                   0
#define ISF_LEN_root_authentication_key         22
#define ISF_LEN_user_authentication_key         22
#define ISF_LEN_pwrdata_inv                     50
#define ISF_LEN_pwrdata_mppt                    40
#define ISF_LEN_csip_nameplate_inv              96
#define ISF_LEN_csip_nameplate_mppt             8
#define ISF_LEN_csip_cfg                        176
#define ISF_LEN_csip_setarray                   84
#define ISF_LEN_csip_issuecmd                   24
#define ISF_LEN_csip_operations                 16
#define ISF_LEN_csip_issuecontrol               4
#define ISF_LEN_application_extension           0

/** ========================================================================<BR>
  * End of File Definition Section   <BR>
  */
  
  


/** 3. Automated Data Allocation Calculations <BR> 
  * ========================================================================<BR>
  */

/// Stock Byte calculation is automatic, but you must put in all the files
#define ISF_ALLOC(VAL)          (((ISF_MAX_##VAL + _ALLOC_OFFSET) >> _ALLOC_SHIFT) << _ALLOC_SHIFT)

#define ISF_STOCK_BYTES   ( \
    ISF_ALLOC(network_settings) + \
    ISF_ALLOC(device_features) + \
    ISF_ALLOC(channel_configuration) + \
    ISF_ALLOC(real_time_scheduler) + \
    ISF_ALLOC(hold_scan_sequence) + \
    ISF_ALLOC(sleep_scan_sequence) + \
    ISF_ALLOC(beacon_transmit_sequence) + \
    ISF_ALLOC(isf_list) + \
    ISF_ALLOC(iss_list) + \
    ISF_ALLOC(gfb_file_list) + \
    ISF_ALLOC(location_data_list) + \
    ISF_ALLOC(ipv6_addresses) + \
    ISF_ALLOC(sensor_list) + \
    ISF_ALLOC(sensor_alarms) + \
    ISF_ALLOC(root_authentication_key) + \
    ISF_ALLOC(user_authentication_key) + \
    ISF_ALLOC(pwrdata_inv) + \
    ISF_ALLOC(pwrdata_mppt) + \
    ISF_ALLOC(csip_nameplate_inv) + \
    ISF_ALLOC(csip_nameplate_mppt) + \
    ISF_ALLOC(csip_cfg) + \
    ISF_ALLOC(csip_setarray) + \
    ISF_ALLOC(csip_issuecmd) + \
    ISF_ALLOC(csip_operations) + \
    ISF_ALLOC(csip_issuecontrol) + \
    ISF_ALLOC(application_extension) + \
    0 \
)

/** ========================================================================<BR>
  * End of Automated Data Allocation Calculations <BR> 
  * 
  */




/** 4. File Block Parameters Section <BR> 
  * ========================================================================<BR>
  */

/** Filesystem Overhead Data
  * The front of the filesystem stores file headers.  The amount below must
  * be coordinated with your linker file.
  */
#define OVERHEAD_START_VADDR                0x0000
#define OVERHEAD_TOTAL_BYTES                0x0200
#define ISF_MIRROR_VADDR                    0xC000



/** GFB (Generic File Block)
 * GFB is an unstructured data store.  The typical implementation is to use
 * the GFB data sections to store file metadata, and have the actual file
 * data stored somewhere else (like on flash).
 */
#define GFB_BLOCK_BYTES         32
#define GFB_BLOCKS_PER_FILE     1
#define GFB_NUM_STOCK_FILES     2
#define GFB_NUM_USER_FILES      0
#define GFB_MOD_standard        b00111100

#define GFB_FILE_BYTES          (GFB_BLOCK_BYTES * GFB_BLOCKS_PER_FILE)
#define GFB_START_VADDR         (OVERHEAD_START_VADDR + OVERHEAD_TOTAL_BYTES)
#define GFB_NUM_FILES           (GFB_NUM_STOCK_FILES + GFB_NUM_USER_FILES)
#define GFB_HEAP_BYTES          (GFB_FILE_BYTES*GFB_NUM_STOCK_FILES)

#define GFB_STOCK_BYTES         (GFB_FILE_BYTES * (GFB_NUM_STOCK_FILES))
#define GFB_TOTAL_BYTES         (GFB_FILE_BYTES * (GFB_NUM_STOCK_FILES + GFB_NUM_USER_FILES))


/** ISSB Files (Indexed Short Series Block)
  * ISSB Files are strings of ISF IDs that bundle/batch related ISF's.  ISSs
  * are not all the same length (max length = 16).  Also, make sure that the 
  * TOTAL_BYTES you allocate to the ISSB bank corresponds to the amount set in
  * the linker file.
  *
  * In this configuration, ISS is empty
  */
#define ISS_TOTAL_BYTES         0x0000
#define ISS_NUM_STOCK_FILES     0
#define ISS_NUM_EXT_FILES       0

#define ISS_START_VADDR         (GFB_START_VADDR + GFB_TOTAL_BYTES)
#define ISS_NUM_USER_FILES      ISS_NUM_EXT_FILES
#define ISS_NUM_FILES           (ISS_NUM_STOCK_FILES + ISS_NUM_USER_FILES)

/// These parameters are derived from the stock configuration, which would 
/// normally be described above, but there is no ISS in this system.
#define ISS_STOCK_BYTES         0


/** ISFB (Indexed Short File Block) 
 * The ISFB contains up to 256 files (IDs 0x00 to 0xFF), length <= 255 bytes.
 * As always, make sure that the TOTAL_BYTES allocated to the ISFB matches the
 * value from your linker file.
 *
 * You can set the ISF_TOTAL_BYTES parameter to a certain number of Bytes, but 
 * it must be greater than or equal to ISF_STOCK_BYTES.  If you don't have 
 * file creation enabled, you should set to ISF_STOCK_BYTES.
 */
#define ISF_TOTAL_BYTES         ISF_STOCK_BYTES
#define ISF_NUM_STOCK_FILES     (ISF_OTFS_FILES + ISF_CSIP_FILES)
#define ISF_NUM_EXT_FILES       (ISF_APPEXT_FILES)
#define ISF_NUM_USER_FILES      0  //max allowed user files
#define ISF_START_VADDR         (ISS_START_VADDR + ISS_TOTAL_BYTES)
#define ISF_NUM_FILES           (ISF_NUM_STOCK_FILES + ISF_NUM_USER_FILES + ISF_NUM_EXT_FILES)


#define VL_HEADER_SIZE      (10 + 2*OT_FEATURE(VLACTIONS) + 4*OT_FEATURE(VLMODTIME) + 4*OT_FEATURE(VLACCTIME))
#define OVERHEAD_MIN_BYTES  (VL_HEADER_SIZE * (2 + GFB_NUM_FILES + ISS_NUM_FILES + ISF_NUM_FILES))

#if (OVERHEAD_TOTAL_BYTES < 0)
#   undef OVERHEAD_TOTAL_BYTES
#   define OVERHEAD_TOTAL_BYTES     OVERHEAD_MIN_BYTES
#elif (OVERHEAD_TOTAL_BYTES < OVERHEAD_MIN_BYTES)
#   error "OVERHEAD_TOTAL_BYTES is defined as less than OVERHEAD_MIN_BYTES"
#endif

/** ========================================================================<BR>
  * End of File Block Parameters Section <BR> 
  */




/** 5. Automated Virtual Address Boundary & Mirror Calculations <BR> 
  * ========================================================================<BR>
  */

/// Stock ISF file memory & mirror allocations (aligned, typically 16bit)
#define ISF_MIRALLOC(VAL)       (ISF_ENMIRROR(VAL) * (((ISF_MAX_##VAL + 2 + _MIRALLOC_OFFSET) >> _MIRALLOC_SHIFT) << _MIRALLOC_SHIFT))

/// ISF file base address computation
#define ISF_BASE(VAL)                           ISF_BASE_##VAL
#if _NOMIRROR
#   undef ISF_MIRALLOC
#   define ISF_MIRALLOC(VAL)                    0
#   define ISF_BASE_network_settings            (ISF_START_VADDR)
#   define ISF_BASE_device_features             (ISF_BASE_network_settings+ISF_ALLOC(network_settings))
#   define ISF_BASE_channel_configuration       (ISF_BASE_device_features+ISF_ALLOC(device_features))
#   define ISF_BASE_real_time_scheduler         (ISF_BASE_channel_configuration+ISF_ALLOC(channel_configuration))
#   define ISF_BASE_hold_scan_sequence          (ISF_BASE_real_time_scheduler+ISF_ALLOC(real_time_scheduler))
#   define ISF_BASE_sleep_scan_sequence         (ISF_BASE_hold_scan_sequence+ISF_ALLOC(hold_scan_sequence))
#   define ISF_BASE_beacon_transmit_sequence    (ISF_BASE_sleep_scan_sequence+ISF_ALLOC(sleep_scan_sequence))
#   define ISF_BASE_isf_list                    (ISF_BASE_beacon_transmit_sequence+ISF_ALLOC(beacon_transmit_sequence))
#   define ISF_BASE_iss_list                    (ISF_BASE_isf_list+ISF_ALLOC(isf_list))
#   define ISF_BASE_gfb_file_list               (ISF_BASE_iss_list+ISF_ALLOC(iss_list))
#   define ISF_BASE_location_data_list          (ISF_BASE_gfb_file_list+ISF_ALLOC(gfb_file_list))
#   define ISF_BASE_ipv6_addresses              (ISF_BASE_location_data_list+ISF_ALLOC(location_data_list))
#   define ISF_BASE_sensor_list                 (ISF_BASE_ipv6_addresses+ISF_ALLOC(ipv6_addresses))
#   define ISF_BASE_sensor_alarms               (ISF_BASE_sensor_list+ISF_ALLOC(sensor_list))
#   define ISF_BASE_root_authentication_key     (ISF_BASE_sensor_alarms+ISF_ALLOC(sensor_alarms))
#   define ISF_BASE_user_authentication_key     (ISF_BASE_root_authentication_key+ISF_ALLOC(root_authentication_key))
#   define ISF_BASE_pwrdata_inv                 (ISF_BASE_user_authentication_key+ISF_ALLOC(user_authentication_key))
#   define ISF_BASE_pwrdata_mppt                (ISF_BASE_pwrdata_inv+ISF_ALLOC(pwrdata_inv))
#   define ISF_BASE_csip_nameplate_inv          (ISF_BASE_pwrdata_mppt+ISF_ALLOC(pwrdata_mppt))
#   define ISF_BASE_csip_nameplate_mppt         (ISF_BASE_csip_nameplate_inv+ISF_ALLOC(csip_nameplate_inv))
#   define ISF_BASE_csip_cfg                    (ISF_BASE_csip_nameplate_mppt+ISF_ALLOC(csip_nameplate_mppt))
#   define ISF_BASE_csip_setarray               (ISF_BASE_csip_cfg+ISF_ALLOC(csip_cfg))
#   define ISF_BASE_csip_issuecmd               (ISF_BASE_csip_setarray+ISF_ALLOC(csip_setarray))
#   define ISF_BASE_csip_operations             (ISF_BASE_csip_issuecmd+ISF_ALLOC(csip_issuecmd))
#   define ISF_BASE_csip_issuecontrol           (ISF_BASE_csip_operations+ISF_ALLOC(csip_operations))
#   define ISF_BASE_application_extension       (ISF_BASE_csip_issuecontrol+ISF_ALLOC(csip_issuecontrol))
#   define ISF_BASE_NEXT                        (ISF_BASE_application_extension+ISF_ALLOC(application_extension ))

#else
#   define ISF_BASE_network_settings            (ISF_START_VADDR)
#   define ISF_BASE_device_features             (ISF_BASE_network_settings+ISF_ALLOC(network_settings))
#   define ISF_BASE_channel_configuration       (ISF_BASE_device_features+ISF_ALLOC(device_features))
#   define ISF_BASE_real_time_scheduler         (ISF_BASE_channel_configuration+ISF_ALLOC(channel_configuration))
#   define ISF_BASE_hold_scan_sequence          (ISF_BASE_real_time_scheduler+ISF_ALLOC(real_time_scheduler))
#   define ISF_BASE_sleep_scan_sequence         (ISF_BASE_hold_scan_sequence+ISF_ALLOC(hold_scan_sequence))
#   define ISF_BASE_beacon_transmit_sequence    (ISF_BASE_sleep_scan_sequence+ISF_ALLOC(sleep_scan_sequence))
#   define ISF_BASE_isf_list                    (ISF_BASE_beacon_transmit_sequence+ISF_ALLOC(beacon_transmit_sequence))
#   define ISF_BASE_iss_list                    (ISF_BASE_isf_list+ISF_ALLOC(isf_list))
#   define ISF_BASE_gfb_file_list               (ISF_BASE_iss_list+ISF_ALLOC(iss_list))
#   define ISF_BASE_location_data_list          (ISF_BASE_gfb_file_list+ISF_ALLOC(gfb_file_list))
#   define ISF_BASE_ipv6_addresses              (ISF_BASE_location_data_list+ISF_ALLOC(location_data_list))
#   define ISF_BASE_sensor_list                 (ISF_BASE_ipv6_addresses+ISF_ALLOC(ipv6_addresses))
#   define ISF_BASE_sensor_alarms               (ISF_BASE_sensor_list+ISF_ALLOC(sensor_list))
#   define ISF_BASE_root_authentication_key     (ISF_BASE_sensor_alarms+ISF_ALLOC(sensor_alarms))
#   define ISF_BASE_user_authentication_key     (ISF_BASE_root_authentication_key+ISF_ALLOC(root_authentication_key))
#   define ISF_BASE_pwrdata_inv                 (ISF_BASE_user_authentication_key+ISF_ALLOC(user_authentication_key))
#   define ISF_BASE_pwrdata_mppt                (ISF_BASE_pwrdata_inv+ISF_ALLOC(pwrdata_inv))
#   define ISF_BASE_csip_nameplate_inv          (ISF_BASE_pwrdata_mppt+ISF_ALLOC(pwrdata_mppt))
#   define ISF_BASE_csip_nameplate_mppt         (ISF_BASE_csip_nameplate_inv+ISF_ALLOC(csip_nameplate_inv))
#   define ISF_BASE_csip_cfg                    (ISF_BASE_csip_nameplate_mppt+ISF_ALLOC(csip_nameplate_mppt))
#   define ISF_BASE_csip_setarray               (ISF_BASE_csip_cfg+ISF_ALLOC(csip_cfg))
#   define ISF_BASE_csip_issuecmd               (ISF_BASE_csip_setarray+ISF_ALLOC(csip_setarray))
#   define ISF_BASE_csip_operations             (ISF_BASE_csip_issuecmd+ISF_ALLOC(csip_issuecmd))
#   define ISF_BASE_csip_issuecontrol           (ISF_BASE_csip_operations+ISF_ALLOC(csip_operations))
#   define ISF_BASE_application_extension       (ISF_BASE_csip_issuecontrol+ISF_ALLOC(csip_issuecontrol))
#   define ISF_BASE_NEXT                        (ISF_BASE_application_extension+ISF_ALLOC(application_extension))
#endif


#define ISF_MIRROR(VAL)                         (unsigned short)(((ISF_ENMIRROR_##VAL != 0) - 1) | (ISF_MIRROR_##VAL) )
#define ISF_MIRROR_network_settings             (ISF_MIRROR_VADDR)
#define ISF_MIRROR_device_features              (ISF_MIRROR_network_settings+ISF_MIRALLOC(network_settings))
#define ISF_MIRROR_channel_configuration        (ISF_MIRROR_device_features+ISF_MIRALLOC(device_features))
#define ISF_MIRROR_real_time_scheduler          (ISF_MIRROR_channel_configuration+ISF_MIRALLOC(channel_configuration))
#define ISF_MIRROR_hold_scan_sequence           (ISF_MIRROR_real_time_scheduler+ISF_MIRALLOC(real_time_scheduler))
#define ISF_MIRROR_sleep_scan_sequence          (ISF_MIRROR_hold_scan_sequence+ISF_MIRALLOC(hold_scan_sequence))
#define ISF_MIRROR_beacon_transmit_sequence     (ISF_MIRROR_sleep_scan_sequence+ISF_MIRALLOC(sleep_scan_sequence))
#define ISF_MIRROR_isf_list                     (ISF_MIRROR_beacon_transmit_sequence+ISF_MIRALLOC(beacon_transmit_sequence))
#define ISF_MIRROR_iss_list                     (ISF_MIRROR_isf_list+ISF_MIRALLOC(isf_list))
#define ISF_MIRROR_gfb_file_list                (ISF_MIRROR_iss_list+ISF_MIRALLOC(iss_list))
#define ISF_MIRROR_location_data_list           (ISF_MIRROR_gfb_file_list+ISF_MIRALLOC(gfb_file_list))
#define ISF_MIRROR_ipv6_addresses               (ISF_MIRROR_location_data_list+ISF_MIRALLOC(location_data_list))
#define ISF_MIRROR_sensor_list                  (ISF_MIRROR_ipv6_addresses+ISF_MIRALLOC(ipv6_addresses))
#define ISF_MIRROR_sensor_alarms                (ISF_MIRROR_sensor_list+ISF_MIRALLOC(sensor_list))
#define ISF_MIRROR_root_authentication_key      (ISF_MIRROR_sensor_alarms+ISF_MIRALLOC(sensor_alarms))
#define ISF_MIRROR_user_authentication_key      (ISF_MIRROR_root_authentication_key+ISF_MIRALLOC(root_authentication_key))
#define ISF_MIRROR_pwrdata_inv                  (ISF_MIRROR_user_authentication_key+ISF_MIRALLOC(user_authentication_key))
#define ISF_MIRROR_pwrdata_mppt                 (ISF_MIRROR_pwrdata_inv+ISF_MIRALLOC(pwrdata_inv))
#define ISF_MIRROR_csip_nameplate_inv           (ISF_MIRROR_pwrdata_mppt+ISF_MIRALLOC(pwrdata_mppt))
#define ISF_MIRROR_csip_nameplate_mppt          (ISF_MIRROR_csip_nameplate_inv+ISF_MIRALLOC(csip_nameplate_inv))
#define ISF_MIRROR_csip_cfg                     (ISF_MIRROR_csip_nameplate_mppt+ISF_MIRALLOC(csip_nameplate_mppt))
#define ISF_MIRROR_csip_setarray                (ISF_MIRROR_csip_cfg+ISF_MIRALLOC(csip_cfg))
#define ISF_MIRROR_csip_issuecmd                (ISF_MIRROR_csip_setarray+ISF_MIRALLOC(csip_setarray))
#define ISF_MIRROR_csip_operations              (ISF_MIRROR_csip_issuecmd+ISF_MIRALLOC(csip_issuecmd))
#define ISF_MIRROR_csip_issuecontrol            (ISF_MIRROR_csip_operations+ISF_MIRALLOC(csip_operations))
#define ISF_MIRROR_application_extension        (ISF_MIRROR_csip_issuecontrol+ISF_MIRALLOC(csip_issuecontrol))
#define ISF_MIRROR_NEXT                         (ISF_MIRROR_application_extension+ISF_MIRALLOC(application_extension))


/// Total amount of stock ISF data stored exclusively in mirror
#define ISF_MIRROR_STOCK_BYTES   0
//#define ISF_MIRROR_STOCK_BYTES   ( \
//    ISF_ALLOC()
//)

/// Total amount of stock ISF data stored in ROM
#define ISF_VWORM_STOCK_BYTES   (ISF_STOCK_BYTES - ISF_MIRROR_STOCK_BYTES)

#define ISF_VWORM_HEAP_BYTES    ISF_VWORM_STOCK_BYTES
#define ISF_HEAP_BYTES          ISF_VWORM_HEAP_BYTES
//#define ISF_VWORM_USER_BYTES   (ISF_ALLOC(USER_FILE) * ISF_NUM_USER_FILES)


/// Total amount of allocation to the Mirror
#define ISF_MIRROR_HEAP_BYTES   ((ISF_MIRROR_NEXT) - (ISF_MIRROR_VADDR))


/** ========================================================================<BR>
  * End of Automated Virtual Address Boundary & Mirror Calculations <BR> 
  */



#endif 
