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
  * @file       /app/modbus_master/fs_defaults.h
  * @author     JP Norair
  * @version    R102
  * @date       9 May 2017
  * @brief      Startup Data for default filesystem
  *
  ******************************************************************************
  */

#ifndef __DEFAULT_FS_STARTUP_C
#define __DEFAULT_FS_STARTUP_C

#include <otstd.h>
#include <board.h>

//#include "board_config.h"
//#include "features_config.h"
#include "fs_config.h"


/// Constant "_ERS" is the value of an erased byte in nonvolatile memory.
/// Usually it is FF or 00, depending on the way the Flash memory is designed.
#ifndef _ERS
#   define _ERS 0xFF
#endif

/// The OpenTag startup routine will place IDs generated by programmatic means
/// in this empty space.
#define __VID   _ERS, _ERS
#define __UID   _ERS, _ERS, _ERS, _ERS, _ERS, _ERS, _ERS, _ERS



/** Default File data allocations
  * ============================================================================
  * - Veelite also uses an additional 1536 bytes for wear leveling
  * - Wear leveling overhead is configurable, but fixed for all FS sizes
  * - Veelite virtual addressing allocations of key sectors below:
  *     Overhead:   0000 to 03FF        (1024 bytes alloc)
  *     ISFSB:      0400 to 049F        (160 bytes alloc)
  *     GFB:        04A0 to 089F        (1024 bytes)
  *     ISFB:       08A0 to 0FFF        (1888 bytes)
  */
#define SPLIT_SHORT(VAL)    (ot_u8)((ot_u16)(VAL) >> 8), (ot_u8)((ot_u16)(VAL) & 0x00FF)
#define SPLIT_LONG(VAL)     (ot_u8)((ot_u32)(VAL) >> 24), (ot_u8)(((ot_u32)(VAL) >> 16) & 0xFF), \
                            (ot_u8)(((ot_u32)(VAL) >> 8) & 0xFF), (ot_u8)((ot_u32)(VAL) & 0xFF)

#define SPLIT_SHORT_LE(VAL) (ot_u8)((ot_u16)(VAL) & 0x00FF), (ot_u8)((ot_u16)(VAL) >> 8)
#define SPLIT_LONG_LE(VAL)  (ot_u8)((ot_u32)(VAL) & 0xFF), (ot_u8)(((ot_u32)(VAL) >> 8) & 0xFF), \
                            (ot_u8)(((ot_u32)(VAL) >> 16) & 0xFF), (ot_u8)((ot_u32)(VAL) >> 24)

#if (OT_FEATURE(VLACTIONS) == ENABLED)
#   define FILE_ACTIONCODE(COND,ID) JOIN_2BYTES(COND, ID)
#else
#   define FILE_ACTIONCODE(COND,ID), 
#endif
#if (OT_FEATURE(VLMODTIME) == ENABLED)
#   define FILE_MODTIME(EPOCH_S) SPLIT_LONG_LE(EPOCH_S)
#else
#   define FILE_MODTIME(EPOCH_S), 
#endif


#ifndef OT_ACTIVE_SETTINGS
#   define OT_ACTIVE_SETTINGS 0
#endif
#ifndef OT_SUPPORTED_SETTINGS
#   define OT_SUPPORTED_SETTINGS 0
#endif




/// These overhead are the Veelite vl_header_t files. They are hard coded,
/// and they must be in the endian of the platform. (Little endian here)

#if defined(__NO_SECTIONS__)
const ot_u8 overhead_files[] = {
#elif (CC_SUPPORT == GCC)
const ot_u8 overhead_files[] __attribute__ ((section(".vl_ov"))) = {
#elif (CC_SUPPORT == TI_C)
#pragma DATA_SECTION(overhead_files, ".vl_ov")
const ot_u8 overhead_files[] = {
#endif
    /* Filesystem Header: Same size as two file headers */
    SPLIT_SHORT_LE(OVERHEAD_TOTAL_BYTES),
#   if (OT_FEATURE(VLACTIONS) == ENABLED)
    SPLIT_SHORT_LE(0),
    SPLIT_SHORT_LE(0),
#   endif
    SPLIT_SHORT_LE(GFB_TOTAL_BYTES),
    SPLIT_SHORT_LE(GFB_STOCK_BYTES),
    SPLIT_SHORT_LE(GFB_NUM_STOCK_FILES),
    SPLIT_SHORT_LE(ISS_TOTAL_BYTES),
    SPLIT_SHORT_LE(ISS_STOCK_BYTES),
    SPLIT_SHORT_LE(ISS_NUM_STOCK_FILES),
    SPLIT_SHORT_LE(ISF_TOTAL_BYTES),
    SPLIT_SHORT_LE(ISF_STOCK_BYTES),
    SPLIT_SHORT_LE(ISF_NUM_STOCK_FILES),
#   if (OT_FEATURE(VLMODTIME) == ENABLED)
    SPLIT_LONG_LE(0),
    SPLIT_LONG_LE(0),
#   endif
    
    /* GFB Files */
    0x00, 0x00, 0x00, 0x01,                 /* GFB Elements 0 */
    0x00, GFB_MOD_standard,
    0x00, 0x14, 0xFF, 0xFF,
    FILE_ACTIONCODE(0,0),
    FILE_MODTIME(0),
    
    /* Mode 2 ISFs, written as little endian */
    ISF_LEN(network_settings), 0x00,                /* Length, little endian */
    SPLIT_SHORT_LE(ISF_ALLOC(network_settings)),    /* Alloc, little endian */
    ISF_ID(network_settings),                       /* ID */
    ISF_MOD(network_settings),                      /* Perms */
    SPLIT_SHORT_LE(ISF_BASE(network_settings)),
    SPLIT_SHORT_LE(ISF_MIRROR(network_settings)),
    FILE_ACTIONCODE(0,0),
    FILE_MODTIME(0),

    ISF_LEN(device_features), 0x00,
    SPLIT_SHORT_LE(ISF_ALLOC(device_features)),
    ISF_ID(device_features),
    ISF_MOD(device_features),
    SPLIT_SHORT_LE(ISF_BASE(device_features)),
    SPLIT_SHORT_LE(ISF_MIRROR(device_features)),
    FILE_ACTIONCODE(0,0),
    FILE_MODTIME(0),

    ISF_LEN(channel_configuration), 0x00,
    SPLIT_SHORT_LE(ISF_ALLOC(channel_configuration)),
    ISF_ID(channel_configuration),
    ISF_MOD(channel_configuration),
    SPLIT_SHORT_LE(ISF_BASE(channel_configuration)),
    SPLIT_SHORT_LE(ISF_MIRROR(channel_configuration)),
    FILE_ACTIONCODE(0,0),
    FILE_MODTIME(0),

    ISF_LEN(real_time_scheduler), 0x00,
    SPLIT_SHORT_LE(ISF_ALLOC(real_time_scheduler)),
    ISF_ID(real_time_scheduler),
    ISF_MOD(real_time_scheduler),
    SPLIT_SHORT_LE(ISF_BASE(real_time_scheduler)),
    SPLIT_SHORT_LE(ISF_MIRROR(real_time_scheduler)),
    FILE_ACTIONCODE(0,0),
    FILE_MODTIME(0),

    ISF_LEN(hold_scan_sequence), 0x00,
    SPLIT_SHORT_LE(ISF_ALLOC(hold_scan_sequence)),
    ISF_ID(hold_scan_sequence),
    ISF_MOD(hold_scan_sequence),
    SPLIT_SHORT_LE(ISF_BASE(hold_scan_sequence)),
    SPLIT_SHORT_LE(ISF_MIRROR(hold_scan_sequence)),
    FILE_ACTIONCODE(0,0),
    FILE_MODTIME(0),

    ISF_LEN(sleep_scan_sequence), 0x00,
    SPLIT_SHORT_LE(ISF_ALLOC(sleep_scan_sequence)),
    ISF_ID(sleep_scan_sequence),
    ISF_MOD(sleep_scan_sequence),
    SPLIT_SHORT_LE(ISF_BASE(sleep_scan_sequence)),
    SPLIT_SHORT_LE(ISF_MIRROR(sleep_scan_sequence)),
    FILE_ACTIONCODE(0,0),
    FILE_MODTIME(0),

    ISF_LEN(beacon_transmit_sequence), 0x00,
    SPLIT_SHORT_LE(ISF_ALLOC(beacon_transmit_sequence)),
    ISF_ID(beacon_transmit_sequence),
    ISF_MOD(beacon_transmit_sequence),
    SPLIT_SHORT_LE(ISF_BASE(beacon_transmit_sequence)),
    SPLIT_SHORT_LE(ISF_MIRROR(beacon_transmit_sequence)),
    FILE_ACTIONCODE(0,0),
    FILE_MODTIME(0),

    ISF_LEN(isf_list), 0x00,
    SPLIT_SHORT_LE(ISF_ALLOC(isf_list)),
    ISF_ID(isf_list),
    ISF_MOD(isf_list),
    SPLIT_SHORT_LE(ISF_BASE(isf_list)),
    SPLIT_SHORT_LE(ISF_MIRROR(isf_list)),
    FILE_ACTIONCODE(0,0),
    FILE_MODTIME(0),

    ISF_LEN(isfs_list), 0x00,
    SPLIT_SHORT_LE(ISF_ALLOC(isfs_list)),
    ISF_ID(isfs_list),
    ISF_MOD(isfs_list),
    SPLIT_SHORT_LE(ISF_BASE(isfs_list)),
    SPLIT_SHORT_LE(ISF_MIRROR(isfs_list)),
    FILE_ACTIONCODE(0,0),
    FILE_MODTIME(0),

    ISF_LEN(gfb_file_list), 0x00,
    SPLIT_SHORT_LE(ISF_ALLOC(gfb_file_list)),
    ISF_ID(gfb_file_list),
    ISF_MOD(gfb_file_list),
    SPLIT_SHORT_LE(ISF_BASE(gfb_file_list)),
    SPLIT_SHORT_LE(ISF_MIRROR(gfb_file_list)),
    FILE_ACTIONCODE(0,0),
    FILE_MODTIME(0),

    ISF_LEN(location_data_list), 0x00,
    SPLIT_SHORT_LE(ISF_ALLOC(location_data_list)),
    ISF_ID(location_data_list),
    ISF_MOD(location_data_list),
    SPLIT_SHORT_LE(ISF_BASE(location_data_list)),
    SPLIT_SHORT_LE(ISF_MIRROR(location_data_list)),
    FILE_ACTIONCODE(0,0),
    FILE_MODTIME(0),

    ISF_LEN(ipv6_addresses), 0x00,
    SPLIT_SHORT_LE(ISF_ALLOC(ipv6_addresses)),
    ISF_ID(ipv6_addresses),
    ISF_MOD(ipv6_addresses),
    SPLIT_SHORT_LE(ISF_BASE(ipv6_addresses)),
    SPLIT_SHORT_LE(ISF_MIRROR(ipv6_addresses)),
    FILE_ACTIONCODE(0,0),
    FILE_MODTIME(0),

    ISF_LEN(sensor_list), 0x00,
    SPLIT_SHORT_LE(ISF_ALLOC(sensor_list)),
    ISF_ID(sensor_list),
    ISF_MOD(sensor_list),
    SPLIT_SHORT_LE(ISF_BASE(sensor_list)),
    SPLIT_SHORT_LE(ISF_MIRROR(sensor_list)),
    FILE_ACTIONCODE(0,0),
    FILE_MODTIME(0),

    ISF_LEN(sensor_alarms), 0x00,
    SPLIT_SHORT_LE(ISF_ALLOC(sensor_alarms)),
    ISF_ID(sensor_alarms),
    ISF_MOD(sensor_alarms),
    SPLIT_SHORT_LE(ISF_BASE(sensor_alarms)),
    SPLIT_SHORT_LE(ISF_MIRROR(sensor_alarms)),
    FILE_ACTIONCODE(0,0),
    FILE_MODTIME(0),

    ISF_LEN(root_authentication_key), 0x00,
    SPLIT_SHORT_LE(ISF_ALLOC(root_authentication_key)),
    ISF_ID(root_authentication_key),
    ISF_MOD(root_authentication_key),
    SPLIT_SHORT_LE(ISF_BASE(root_authentication_key)),
    SPLIT_SHORT_LE(ISF_MIRROR(root_authentication_key)),
    FILE_ACTIONCODE(0,0),
    FILE_MODTIME(0),

    ISF_LEN(user_authentication_key), 0x00,
    SPLIT_SHORT_LE(ISF_ALLOC(user_authentication_key)),
    ISF_ID(user_authentication_key),
    ISF_MOD(user_authentication_key),
    SPLIT_SHORT_LE(ISF_BASE(user_authentication_key)),
    SPLIT_SHORT_LE(ISF_MIRROR(user_authentication_key)),
    FILE_ACTIONCODE(0,0),
    FILE_MODTIME(0),

    ISF_LEN(routing_code), 0x00,
    SPLIT_SHORT_LE(ISF_ALLOC(routing_code)),
    ISF_ID(routing_code),
    ISF_MOD(routing_code),
    SPLIT_SHORT_LE(ISF_BASE(routing_code)),
    SPLIT_SHORT_LE(ISF_MIRROR(routing_code)),
    FILE_ACTIONCODE(0,0),
    FILE_MODTIME(0),

    ISF_LEN(user_id), 0x00,
    SPLIT_SHORT_LE(ISF_ALLOC(user_id)),
    ISF_ID(user_id),
    ISF_MOD(user_id),
    SPLIT_SHORT_LE(ISF_BASE(user_id)),
    SPLIT_SHORT_LE(ISF_MIRROR(user_id)),
    FILE_ACTIONCODE(0,0),
    FILE_MODTIME(0),

    ISF_LEN(optional_command_list), 0x00,
    SPLIT_SHORT_LE(ISF_ALLOC(optional_command_list)),
    ISF_ID(optional_command_list),
    ISF_MOD(optional_command_list),
    SPLIT_SHORT_LE(ISF_BASE(optional_command_list)),
    SPLIT_SHORT_LE(ISF_MIRROR(optional_command_list)),
    FILE_ACTIONCODE(0,0),
    FILE_MODTIME(0),

    ISF_LEN(memory_size), 0x00,
    SPLIT_SHORT_LE(ISF_ALLOC(memory_size)),
    ISF_ID(memory_size),
    ISF_MOD(memory_size),
    SPLIT_SHORT_LE(ISF_BASE(memory_size)),
    SPLIT_SHORT_LE(ISF_MIRROR(memory_size)),
    FILE_ACTIONCODE(0,0),
    FILE_MODTIME(0),

    ISF_LEN(table_query_size), 0x00,
    SPLIT_SHORT_LE(ISF_ALLOC(table_query_size)),
    ISF_ID(table_query_size),
    ISF_MOD(table_query_size),
    SPLIT_SHORT_LE(ISF_BASE(table_query_size)),
    SPLIT_SHORT_LE(ISF_MIRROR(table_query_size)),
    FILE_ACTIONCODE(0,0),
    FILE_MODTIME(0),

    ISF_LEN(table_query_results), 0x00,
    SPLIT_SHORT_LE(ISF_ALLOC(table_query_results)),
    ISF_ID(table_query_results),
    ISF_MOD(table_query_results),
    SPLIT_SHORT_LE(ISF_BASE(table_query_results)),
    SPLIT_SHORT_LE(ISF_MIRROR(table_query_results)),
    FILE_ACTIONCODE(0,0),
    FILE_MODTIME(0),

    ISF_LEN(hardware_fault_status), 0x00,
    SPLIT_SHORT_LE(ISF_ALLOC(hardware_fault_status)),
    ISF_ID(hardware_fault_status),
    ISF_MOD(hardware_fault_status),
    SPLIT_SHORT_LE(ISF_BASE(hardware_fault_status)),
    SPLIT_SHORT_LE(ISF_MIRROR(hardware_fault_status)),
    FILE_ACTIONCODE(0,0),
    FILE_MODTIME(0),

    ISF_LEN(application_extension), 0x00,
    SPLIT_SHORT_LE(ISF_ALLOC(application_extension)),
    ISF_ID(application_extension),
    ISF_MOD(application_extension),
    SPLIT_SHORT_LE(ISF_BASE(application_extension)),
    SPLIT_SHORT_LE(ISF_MIRROR(application_extension)),
    FILE_ACTIONCODE(0,0),
    FILE_MODTIME(0),
};




#if (GFB_TOTAL_BYTES > 0)
#   if defined(__NO_SECTIONS__)
    const ot_u8 gfb_stock_files[] = { _ERS };

#   elif (CC_SUPPORT == GCC)
    const ot_u8 gfb_stock_files[GFB_TOTAL_BYTES] __attribute__((section(".vl_gfb"))) = { _ERS };
    
#   elif (CC_SUPPORT == TI_C)
#   pragma DATA_SECTION(gfb_stock_files, ".vl_gfb")
    const ot_u8 gfb_stock_files[GFB_TOTAL_BYTES] = { _ERS };
#   endif
#endif



#if (ISS_TOTAL_BYTES > 0)
#   if defined(__NO_SECTIONS__)
    const ot_u8 iss_stock_files[] = { _ERS };

#   elif (CC_SUPPORT == GCC)
    const ot_u8 iss_stock_files[ISS_TOTAL_BYTES] __attribute__((section(".vl_iss"))) = { _ERS };
    
#   elif (CC_SUPPORT == TI_C)
#   pragma DATA_SECTION(gfb_stock_files, ".vl_iss")
    const ot_u8 iss_stock_files[ISS_TOTAL_BYTES] = { _ERS };
#   endif
// #else
// 
//     const ot_u8 iss_stock_files[] = { 0 };
#endif



/// Firmware & Version information for ISF1 (Device Features)
/// This will look something like "OTv1  xyyyyyyy" where x is a letter and
/// yyyyyyy is a Base64 string containing a 16 bit build-id and a 32 bit mask
/// indicating the features compiled-into the build.
#include <otsys/version.h>

#define BV0     (ot_u8)(OT_VERSION_MAJOR + 48)
#define BT0     (ot_u8)(OT_BUILDTYPE)
#define BC0     OT_BUILDCODE0
#define BC1     OT_BUILDCODE1
#define BC2     OT_BUILDCODE2
#define BC3     OT_BUILDCODE3
#define BC4     OT_BUILDCODE4
#define BC5     OT_BUILDCODE5
#define BC6     OT_BUILDCODE6
#define BC7     OT_BUILDCODE7

/// This array contains the stock ISF data.  ISF data must be big endian!
#if defined(__NO_SECTIONS__)
const ot_u8 isf_stock_files[] = {
#elif (CC_SUPPORT == GCC)
const ot_u8 isf_stock_files[] __attribute__((section(".vl_isf"))) = {
#elif (CC_SUPPORT == TI_C)
#pragma DATA_SECTION(isf_stock_files, ".vl_isf")
const ot_u8 isf_stock_files[] = {
#endif
    /* network settings: id=0x00, len=10, alloc=10 */
    __VID,                                              /* VID */
    0x11,                                               /* Device Subnet */
    0x11,                                               /* Beacon Subnet */
    SPLIT_SHORT(OT_ACTIVE_SETTINGS),                    /* Active Setting */
    0x00,                                               /* Default Device Flags */
    1,                                                  /* Beacon Attempts */
    SPLIT_SHORT(2),                                     /* Hold Scan Sequence Cycles */

    /* device features: id=0x01, len=48, alloc=48 */
    __UID,                                              /* UID: 8 bytes*/
    SPLIT_SHORT(OT_SUPPORTED_SETTINGS),                 /* Supported Setting */
    255,                                                /* Max Frame Length */
    1,                                                  /* Max Frames per Packet */
    SPLIT_SHORT(0),                                     /* DLLS Methods */
    SPLIT_SHORT(0),                                     /* NLS Methods */
    SPLIT_SHORT(ISF_TOTAL_BYTES),                       /* ISFB Total Memory */
    SPLIT_SHORT(ISF_TOTAL_BYTES-ISF_HEAP_BYTES),        /* ISFB Available Memory */
    SPLIT_SHORT(0),                                     /* ISFSB Total Memory */
    SPLIT_SHORT(0),                                     /* ISFSB Available Memory */
    SPLIT_SHORT(GFB_TOTAL_BYTES),                       /* GFB Total Memory */
    SPLIT_SHORT(GFB_TOTAL_BYTES-GFB_HEAP_BYTES),        /* GFB Available Memory */
    SPLIT_SHORT(GFB_BLOCK_BYTES),                       /* GFB Block Size */
    0,                                                  /* RFU */
    0,                                                  /* Session Stack Depth */
    'O','T','v',BV0,' ',' ',
    BT0,BC0,BC1,BC2,BC3,BC4,BC5,BC6,BC7, 0,             /* Firmware & Version as C-string */

    /* channel configuration: id=0x02, len=0, alloc=0 */

    /* real time scheduler: id=0x03, len=0, alloc=0 */

    /* hold scan periods: id=0x04, len=0, alloc=0 */
    /* Period data format in Section X.9.4.5 of Mode 2 spec */

    /* sleep scan periods: id=0x05, len=0, alloc=0 */
    /* Period data format in Section X.9.4.5 of Mode 2 spec */

    /* beacon transmit periods: id=0x06, len=0, alloc=0 */
    /* Period data format in Section X.9.4.7 of Mode 2 spec */ //0x0240

    /* User ISF List: id=0x07, len=1, alloc=24 */
    0xFF, _ERS, _ERS, _ERS, _ERS, _ERS, _ERS, _ERS,     /* List of Protocols supported (Tentative)*/
    _ERS, _ERS, _ERS, _ERS, _ERS, _ERS, _ERS, _ERS,
    _ERS, _ERS, _ERS, _ERS, _ERS, _ERS, _ERS, _ERS,

    /* ISFS list: id=0x08, len=0, alloc=0 */

    /* GFB File List: id=0x09, len=1, alloc=8 */
    0x01, _ERS, _ERS, _ERS, _ERS, _ERS, _ERS, _ERS,

    /* Location Data List: id=0x0A, len=0, alloc=0 */

    /* IPv6 Addresses: id=0x0B, len=0, alloc=48 */
    _ERS, _ERS, _ERS, _ERS, _ERS, _ERS, _ERS, _ERS,
    _ERS, _ERS, _ERS, _ERS, _ERS, _ERS, _ERS, _ERS,
    _ERS, _ERS, _ERS, _ERS, _ERS, _ERS, _ERS, _ERS,
    _ERS, _ERS, _ERS, _ERS, _ERS, _ERS, _ERS, _ERS,
    _ERS, _ERS, _ERS, _ERS, _ERS, _ERS, _ERS, _ERS,
    _ERS, _ERS, _ERS, _ERS, _ERS, _ERS, _ERS, _ERS,

    /* Sensor List: id=0x0C, len=0, alloc=0 */

    /* Sensor Alarms: id=0x0D, len=0, alloc=0 */

    /* root auth key:       id=0x0E, len=0, alloc=16 */
    _ERS, _ERS, _ERS, _ERS, _ERS, _ERS, _ERS, _ERS,
    _ERS, _ERS, _ERS, _ERS, _ERS, _ERS, _ERS, _ERS,
    
    /* Admin auth key:      id=0x0F, len=0, alloc=16 */
    _ERS, _ERS, _ERS, _ERS, _ERS, _ERS, _ERS, _ERS,
    _ERS, _ERS, _ERS, _ERS, _ERS, _ERS, _ERS, _ERS,

    /* Routing Code: id=0x10, len=0, alloc=0 */

    /* User ID: id=0x11, len=0, alloc=60 */
    _ERS, _ERS, _ERS, _ERS, _ERS, _ERS, _ERS, _ERS,
    _ERS, _ERS, _ERS, _ERS, _ERS, _ERS, _ERS, _ERS,
    _ERS, _ERS, _ERS, _ERS, _ERS, _ERS, _ERS, _ERS,
    _ERS, _ERS, _ERS, _ERS, _ERS, _ERS, _ERS, _ERS,
    _ERS, _ERS, _ERS, _ERS, _ERS, _ERS, _ERS, _ERS,
    _ERS, _ERS, _ERS, _ERS, _ERS, _ERS, _ERS, _ERS,
    _ERS, _ERS, _ERS, _ERS, _ERS, _ERS, _ERS, _ERS,
    _ERS, _ERS, _ERS, _ERS,

    /* Mode 1 Optional Command list: id=0x12, len=0, alloc=0 */

    /* Mode 1 Memory Size: id=0x13, len=0, alloc=0 */

    /* Mode 1 Table Query Size: id=0x14, len=0, alloc=0 */

    /* Mode 1 Table Query Results: id=0x15, len=0, alloc=0 */

    /* HW Fault Status: id=0x16, len=0, alloc=0 */

    /* Application Extension: id=0xFF, len=0, alloc=64 */
    _ERS, _ERS, _ERS, _ERS, _ERS, _ERS, _ERS, _ERS,
    _ERS, _ERS, _ERS, _ERS, _ERS, _ERS, _ERS, _ERS,
    _ERS, _ERS, _ERS, _ERS, _ERS, _ERS, _ERS, _ERS,
    _ERS, _ERS, _ERS, _ERS, _ERS, _ERS, _ERS, _ERS,
    _ERS, _ERS, _ERS, _ERS, _ERS, _ERS, _ERS, _ERS,
    _ERS, _ERS, _ERS, _ERS, _ERS, _ERS, _ERS, _ERS,
    _ERS, _ERS, _ERS, _ERS, _ERS, _ERS, _ERS, _ERS,
    _ERS, _ERS, _ERS, _ERS, _ERS, _ERS, _ERS, _ERS,
};



#endif

