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
  * @file       /app/csip_c2000/fs_defaults.h
  * @author     JP Norair
  * @version    R102
  * @date       9 May 2017
  * @brief      Startup Data for default filesystem
  * 
  ******************************************************************************
  */

#ifndef __DEFAULT_FS_STARTUP_H
#define __DEFAULT_FS_STARTUP_H

#include <otstd.h>
#include <board.h>

// Local App FS Configuration
#include "fs_config.h"



/// Constant "_ERS" is the value of an erased byte in nonvolatile memory.
/// Usually it is FF or 00, depending on the way the Flash memory is designed.
#ifndef _ERS
#   define _ERS     0xFF
#endif
#ifndef _ERS16
#   define _ERS16   0xFFFF
#endif

/// The OpenTag startup routine will place IDs generated by programmatic means
/// in this empty space.
#define __VID   _ERS16
#define __UID   _ERS16, _ERS16, _ERS16, _ERS16



/** Default File data allocations
  * ============================================================================
  */

// C2000 is little endian with 16 bit byte.
// We put normal 8 bit bytes into 16 bit byte in network endian order
// MSB = +1
// LSB = 0
#define JOIN_2BYTES(B0, B1) (ot_u16)((((ot_u16)(B1))<<8)|((ot_u16)(B0)))

//#define SPLIT_SHORT(VAL)    (ot_u8)((ot_u16)(VAL) >> 8), (ot_u8)((ot_u16)(VAL) & 0x00FF)
//#define SPLIT_SHORT_LE(VAL) (ot_u8)((ot_u16)(VAL) & 0x00FF), (ot_u8)((ot_u16)(VAL) >> 8)

#define SPLIT_LONG(VAL)     (ot_u16)(((ot_u32)(VAL) >> 16) & 0xFFFF), (ot_u16)((ot_u32)(VAL) & 0xFFFF)
#define SPLIT_LONG_LE(VAL)  (ot_u16)((ot_u32)(VAL) & 0xFFFF), (ot_u16)(((ot_u32)(VAL) >> 16) & 0xFFFF)

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
const ot_u16 overhead_files[] = {
#elif (CC_SUPPORT == GCC)
const ot_u16 overhead_files[] __attribute__ ((section(".vl_ov"))) = {
#elif (CC_SUPPORT == TI_C)
#pragma DATA_SECTION(overhead_files, ".vl_ov")
const ot_u16 overhead_files[] = {
#endif
    /* Filesystem Header: Same size as two file headers (24 bytes) */
    OVERHEAD_TOTAL_BYTES,
#   if (OT_FEATURE(VLACTIONS) == ENABLED)
    SPLIT_SHORT_LE(0),
    SPLIT_SHORT_LE(0),
#   endif
    GFB_TOTAL_BYTES,
    GFB_STOCK_BYTES,
    GFB_NUM_STOCK_FILES,
    ISS_TOTAL_BYTES,
    ISS_STOCK_BYTES,
    ISS_NUM_STOCK_FILES,
    ISF_TOTAL_BYTES,
    ISF_STOCK_BYTES,
    ISF_NUM_STOCK_FILES,
#   if (OT_FEATURE(VLMODTIME) == ENABLED)
    SPLIT_LONG_LE(0),
    SPLIT_LONG_LE(0),
#   endif

    /* GFB Files */
    JOIN_2BYTES(0x00, 0x00), JOIN_2BYTES(0x00, 0x01),                 /* GFB Elements 0 */
    JOIN_2BYTES(0x00, GFB_MOD_standard),
    JOIN_2BYTES(0x00, 0x14), JOIN_2BYTES(0xFF, 0xFF),
    FILE_ACTIONCODE(0,0),
    FILE_MODTIME(0),
    
    /* Mode 2 ISFs, written as little endian */
    JOIN_2BYTES(ISF_LEN(network_settings), 0x00),                       /* Length, little endian */
    ISF_ALLOC(network_settings),                        /* Alloc, little endian */
    JOIN_2BYTES(ISF_ID(network_settings), ISF_MOD(network_settings)),   /* ID , Perms*/
    ISF_BASE(network_settings),
    ISF_MIRROR(network_settings),
    FILE_ACTIONCODE(0,0),
    FILE_MODTIME(0),

    JOIN_2BYTES(ISF_LEN(device_features), 0x00),
    (ISF_ALLOC(device_features)),
    JOIN_2BYTES(ISF_ID(device_features), ISF_MOD(device_features)),
    (ISF_BASE(device_features)),
    (ISF_MIRROR(device_features)),
    FILE_ACTIONCODE(0,0),
    FILE_MODTIME(0),

    JOIN_2BYTES(ISF_LEN(channel_configuration), 0x00),
    (ISF_ALLOC(channel_configuration)),
    JOIN_2BYTES(ISF_ID(channel_configuration), ISF_MOD(channel_configuration)),
    (ISF_BASE(channel_configuration)),
    (ISF_MIRROR(channel_configuration)),
    FILE_ACTIONCODE(0,0),
    FILE_MODTIME(0),

    JOIN_2BYTES(ISF_LEN(real_time_scheduler), 0x00),
    (ISF_ALLOC(real_time_scheduler)),
    JOIN_2BYTES(ISF_ID(real_time_scheduler), ISF_MOD(real_time_scheduler)),
    (ISF_BASE(real_time_scheduler)),
    (ISF_MIRROR(real_time_scheduler)),
    FILE_ACTIONCODE(0,0),
    FILE_MODTIME(0),

    JOIN_2BYTES(ISF_LEN(hold_scan_sequence), 0x00),
    (ISF_ALLOC(hold_scan_sequence)),
    JOIN_2BYTES(ISF_ID(hold_scan_sequence), ISF_MOD(hold_scan_sequence)),
    (ISF_BASE(hold_scan_sequence)),
    (ISF_MIRROR(hold_scan_sequence)),
    FILE_ACTIONCODE(0,0),
    FILE_MODTIME(0),

    JOIN_2BYTES(ISF_LEN(sleep_scan_sequence), 0x00),
    (ISF_ALLOC(sleep_scan_sequence)),
    JOIN_2BYTES(ISF_ID(sleep_scan_sequence), ISF_MOD(sleep_scan_sequence)),
    (ISF_BASE(sleep_scan_sequence)),
    (ISF_MIRROR(sleep_scan_sequence)),
    FILE_ACTIONCODE(0,0),
    FILE_MODTIME(0),

    JOIN_2BYTES(ISF_LEN(beacon_transmit_sequence), 0x00),
    (ISF_ALLOC(beacon_transmit_sequence)),
    JOIN_2BYTES(ISF_ID(beacon_transmit_sequence), ISF_MOD(beacon_transmit_sequence)),
    (ISF_BASE(beacon_transmit_sequence)),
    (ISF_MIRROR(beacon_transmit_sequence)),
    FILE_ACTIONCODE(0,0),
    FILE_MODTIME(0),

    JOIN_2BYTES(ISF_LEN(isf_list), 0x00),
    (ISF_ALLOC(isf_list)),
    JOIN_2BYTES(ISF_ID(isf_list),ISF_MOD(isf_list)),
    (ISF_BASE(isf_list)),
    (ISF_MIRROR(isf_list)),
    FILE_ACTIONCODE(0,0),
    FILE_MODTIME(0),

    JOIN_2BYTES(ISF_LEN(isfs_list), 0x00),
    (ISF_ALLOC(isfs_list)),
    JOIN_2BYTES(ISF_ID(isfs_list), ISF_MOD(isfs_list)),
    (ISF_BASE(isfs_list)),
    (ISF_MIRROR(isfs_list)),
    FILE_ACTIONCODE(0,0),
    FILE_MODTIME(0),

    JOIN_2BYTES(ISF_LEN(gfb_file_list), 0x00),
    (ISF_ALLOC(gfb_file_list)),
    JOIN_2BYTES(ISF_ID(gfb_file_list), ISF_MOD(gfb_file_list)),
    (ISF_BASE(gfb_file_list)),
    (ISF_MIRROR(gfb_file_list)),
    FILE_ACTIONCODE(0,0),
    FILE_MODTIME(0),

    JOIN_2BYTES(ISF_LEN(location_data_list), 0x00),
    (ISF_ALLOC(location_data_list)),
    JOIN_2BYTES(ISF_ID(location_data_list), ISF_MOD(location_data_list)),
    (ISF_BASE(location_data_list)),
    (ISF_MIRROR(location_data_list)),
    FILE_ACTIONCODE(0,0),
    FILE_MODTIME(0),

    JOIN_2BYTES(ISF_LEN(ipv6_addresses), 0x00),
    (ISF_ALLOC(ipv6_addresses)),
    JOIN_2BYTES(ISF_ID(ipv6_addresses), ISF_MOD(ipv6_addresses)),
    (ISF_BASE(ipv6_addresses)),
    (ISF_MIRROR(ipv6_addresses)),
    FILE_ACTIONCODE(0,0),
    FILE_MODTIME(0),

    JOIN_2BYTES(ISF_LEN(sensor_list), 0x00),
    (ISF_ALLOC(sensor_list)),
    JOIN_2BYTES(ISF_ID(sensor_list), ISF_MOD(sensor_list)),
    (ISF_BASE(sensor_list)),
    (ISF_MIRROR(sensor_list)),
    FILE_ACTIONCODE(0,0),
    FILE_MODTIME(0),

    JOIN_2BYTES(ISF_LEN(sensor_alarms), 0x00),
    (ISF_ALLOC(sensor_alarms)),
    JOIN_2BYTES(ISF_ID(sensor_alarms), ISF_MOD(sensor_alarms)),
    (ISF_BASE(sensor_alarms)),
    (ISF_MIRROR(sensor_alarms)),
    FILE_ACTIONCODE(0,0),
    FILE_MODTIME(0),

    JOIN_2BYTES(ISF_LEN(root_authentication_key), 0x00),
    (ISF_ALLOC(root_authentication_key)),
    JOIN_2BYTES(ISF_ID(root_authentication_key), ISF_MOD(root_authentication_key)),
    (ISF_BASE(root_authentication_key)),
    (ISF_MIRROR(root_authentication_key)),
    FILE_ACTIONCODE(0,0),
    FILE_MODTIME(0),

    JOIN_2BYTES(ISF_LEN(user_authentication_key), 0x00),
    (ISF_ALLOC(user_authentication_key)),
    JOIN_2BYTES(ISF_ID(user_authentication_key), ISF_MOD(user_authentication_key)),
    (ISF_BASE(user_authentication_key)),
    (ISF_MIRROR(user_authentication_key)),
    FILE_ACTIONCODE(0,0),
    FILE_MODTIME(0),

    JOIN_2BYTES(ISF_LEN(pv_config), 0x00),
    (ISF_ALLOC(pv_config)),
    JOIN_2BYTES(ISF_ID(pv_config), ISF_MOD(pv_config)),
    (ISF_BASE(pv_config)),
    (ISF_MIRROR(pv_config)),
    FILE_ACTIONCODE(0,0),
    FILE_MODTIME(0),

    JOIN_2BYTES(ISF_LEN(pv_fault_config), 0x00),
    (ISF_ALLOC(pv_fault_config)),monitoring_data
    JOIN_2BYTES(ISF_ID(pv_fault_config), ISF_MOD(pv_fault_config)),
    (ISF_BASE(pv_fault_config)),
    (ISF_MIRROR(pv_fault_config)),
    FILE_ACTIONCODE(0,0),
    FILE_MODTIME(0),

    JOIN_2BYTES(ISF_LEN(nameplate_ratings), 0x00),
    (ISF_ALLOC(nameplate_ratings)),
    JOIN_2BYTES(ISF_ID(nameplate_ratings), ISF_MOD(nameplate_ratings)),
    (ISF_BASE(nameplate_ratings)),
    (ISF_MIRROR(nameplate_ratings)),
    FILE_ACTIONCODE(0,0),
    FILE_MODTIME(0),

    JOIN_2BYTES(ISF_LEN(adjusted_settings), 0x00),
    (ISF_ALLOC(adjusted_settings)),
    JOIN_2BYTES(ISF_ID(adjusted_settings), ISF_MOD(adjusted_settings)),
    (ISF_BASE(adjusted_settings)),
    (ISF_MIRROR(adjusted_settings)),
    FILE_ACTIONCODE(0,0),
    FILE_MODTIME(0),

    JOIN_2BYTES(ISF_LEN(alarms), 0x00),
    (ISF_ALLOC(alarms)),
    JOIN_2BYTES(ISF_ID(alarms), ISF_MOD(alarms)),
    (ISF_BASE(alarms)),
    (ISF_MIRROR(alarms)),
    FILE_ACTIONCODE(0,0),
    FILE_MODTIME(0),

    JOIN_2BYTES(ISF_LEN(opcurve_voltvar), 0x00),
    (ISF_ALLOC(opcurve_voltvar)),
    JOIN_2BYTES(ISF_ID(opcurve_voltvar), ISF_MOD(opcurve_voltvar)),
    (ISF_BASE(opcurve_voltvar)),
    (ISF_MIRROR(opcurve_voltvar)),
    FILE_ACTIONCODE(0,0),
    FILE_MODTIME(0),

    JOIN_2BYTES(ISF_LEN(opcurve_freqwatt), 0x00),
    (ISF_ALLOC(opcurve_freqwatt)),
    JOIN_2BYTES(ISF_ID(opcurve_freqwatt), ISF_MOD(opcurve_freqwatt)),
    (ISF_BASE(opcurve_freqwatt)),
    (ISF_MIRROR(opcurve_freqwatt)),
    FILE_ACTIONCODE(0,0),
    FILE_MODTIME(0),

    JOIN_2BYTES(ISF_LEN(application_extension), 0x00),
    (ISF_ALLOC(application_extension)),
    JOIN_2BYTES(ISF_ID(application_extension), ISF_MOD(application_extension)),
    (ISF_BASE(application_extension)),
    (ISF_MIRROR(application_extension)),
    FILE_ACTIONCODE(0,0),
    FILE_MODTIME(0),
};




#if (GFB_TOTAL_BYTES > 0)
#   if defined(__NO_SECTIONS__)
    const ot_u16 gfb_stock_files[] = { _ERS16 };

#   elif (CC_SUPPORT == GCC)
    const ot_u16 gfb_stock_files[GFB_TOTAL_BYTES/2] __attribute__((section(".vl_gfb"))) = { _ERS16 };
    
#   elif (CC_SUPPORT == TI_C)
#   pragma DATA_SECTION(gfb_stock_files, ".vl_gfb")
    const ot_u16 gfb_stock_files[GFB_TOTAL_BYTES/2] = { _ERS16 };
#   endif
#endif



#if (ISS_TOTAL_BYTES > 0)
#   if defined(__NO_SECTIONS__)
    const ot_u16 iss_stock_files[] = { _ERS };

#   elif (CC_SUPPORT == GCC)
    const ot_u16 iss_stock_files[ISS_TOTAL_BYTES/2] __attribute__((section(".vl_iss"))) = { _ERS };
    
#   elif (CC_SUPPORT == TI_C)
#   pragma DATA_SECTION(gfb_stock_files, ".vl_iss")
    const ot_u16 iss_stock_files[ISS_TOTAL_BYTES/2] = { _ERS };
#   endif
#else

    const ot_u16 iss_stock_files[] = { 0 };
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
const ot_u16 isf_stock_files[] = {
#elif (CC_SUPPORT == GCC)
const ot_u16 isf_stock_files[] __attribute__((section(".vl_isf"))) = {
#elif (CC_SUPPORT == TI_C)
#pragma DATA_SECTION(isf_stock_files, ".vl_isf")
const ot_u16 isf_stock_files[] = {
#endif
    /* network settings: id=0x00, len=10, alloc=10 */
    __VID,                                              /* VID */
    JOIN_2BYTES(0x11, 0x11),                            /* Device Subnet, Beacon Subnet */
    PLATFORM_ENDIAN16_C(OT_ACTIVE_SETTINGS),                    /* Active Setting */
    JOIN_2BYTES(0x00, 1),                               /* Default Device Flags, Beacon Attempts */
    PLATFORM_ENDIAN16_C(2),                                     /* Hold Scan Sequence Cycles */

    /* device features: id=0x01, len=48, alloc=48 */
    __UID,                                              /* UID: 8 bytes*/
    PLATFORM_ENDIAN16_C(OT_SUPPORTED_SETTINGS),                 /* Supported Setting */
    JOIN_2BYTES(255, 1),                                /* Max Frame Length, Max Frames per Packet */
    PLATFORM_ENDIAN16_C(0),                                     /* DLLS Methods */
    PLATFORM_ENDIAN16_C(0),                                     /* NLS Methods */
    PLATFORM_ENDIAN16_C(ISF_TOTAL_BYTES),                       /* ISFB Total Memory */
    PLATFORM_ENDIAN16_C(ISF_TOTAL_BYTES-ISF_HEAP_BYTES),        /* ISFB Available Memory */
    PLATFORM_ENDIAN16_C(0),                                     /* ISFSB Total Memory */
    PLATFORM_ENDIAN16_C(0),                                     /* ISFSB Available Memory */
    PLATFORM_ENDIAN16_C(GFB_TOTAL_BYTES),                       /* GFB Total Memory */
    PLATFORM_ENDIAN16_C(GFB_TOTAL_BYTES-GFB_HEAP_BYTES),        /* GFB Available Memory */
    PLATFORM_ENDIAN16_C(GFB_BLOCK_BYTES),                       /* GFB Block Size */
    JOIN_2BYTES(0, 0),                                  /* RFU, Session Stack Depth */
    JOIN_2BYTES('O','T'), JOIN_2BYTES('v',BV0),         /* Firmware & Version as C-string */
    JOIN_2BYTES(' ',' '),
    JOIN_2BYTES(BT0,BC0), 
    JOIN_2BYTES(BC1,BC2),
    JOIN_2BYTES(BC3,BC4),
    JOIN_2BYTES(BC5,BC6),
    JOIN_2BYTES(BC7, 0),             

    /* channel configuration: id=0x02, len=0, alloc=0 */

    /* real time scheduler: id=0x03, len=0, alloc=0 */

    /* hold scan periods: id=0x04, len=0, alloc=0 */
    /* Period data format in Section X.9.4.5 of Mode 2 spec */

    /* sleep scan periods: id=0x05, len=0, alloc=0 */
    /* Period data format in Section X.9.4.5 of Mode 2 spec */

    /* beacon transmit periods: id=0x06, len=0, alloc=0 */
    /* Period data format in Section X.9.4.7 of Mode 2 spec */ //0x0240

    /* User ISF List: id=0x07, len=1, alloc=24 */
    /* List of Protocols supported (Tentative)*/
    JOIN_2BYTES(0xFF, _ERS), _ERS16, _ERS16, _ERS16,     
    _ERS16, _ERS16, _ERS16, _ERS16, 
    _ERS16, _ERS16, _ERS16, _ERS16,

    /* ISFS list: id=0x08, len=0, alloc=0 */

    /* GFB File List: id=0x09, len=1, alloc=8 */
    JOIN_2BYTES(0x01, _ERS), _ERS16, _ERS16, _ERS16,

    /* Location Data List: id=0x0A, len=0, alloc=0 */

    /* IPv6 Addresses: id=0x0B, len=0, alloc=48 */
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,

    /* Sensor List: id=0x0C, len=0, alloc=0 */

    /* Sensor Alarms: id=0x0D, len=0, alloc=0 */

    /* root auth key:       id=0x0E, len=0, alloc=22 */
    _ERS16, _ERS16, _ERS16, 
    _ERS16, _ERS16, _ERS16, _ERS16, 
    _ERS16, _ERS16, _ERS16, _ERS16, 
    
    /* Admin auth key:      id=0x0F, len=0, alloc=22 */
    _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16, 
    _ERS16, _ERS16, _ERS16, _ERS16, 

    /* Fault Message: id=0x10, len=0, alloc=64 */
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,

    /* Monitoring Data: id=0x11, len=0, alloc=64 */
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,

    /* Nameplate Ratings: id=0x12, len=0, alloc=64 */
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,

    /* Adjusted Settings: id=0x13, len=0, alloc=64 */
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,

    /* Alarms: id=0x14, len=0, alloc=4 */
    _ERS16, _ERS16,

    /* Operational Curve for Volt/VAR: id=0x15, len=0, alloc=208 */
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,

    /* Operational Curve for Freq/Watt: id=0x16, len=0, alloc=208 */
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,

    /* Application Extension: id=0xFF, len=0, alloc=64 */
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
    _ERS16, _ERS16, _ERS16, _ERS16,
};



#endif

