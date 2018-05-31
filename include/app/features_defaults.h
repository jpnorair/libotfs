/* Copyright 2010-2014 JP Norair
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
  * @file       /app/features_defaults.h
  * @author     JP Norair (jpnorair@indigresso.com)
  * @version    R101
  * @date       31 Aug 2014
  * @brief      Feature configuration for a typical app & platform
  *
  ******************************************************************************
  */

#ifndef __FEATURES_DEFAULTS_H
#define __FEATURES_DEFAULTS_H


/// Macro settings: ENABLED, DISABLED, NOT_AVAILABLE
#ifdef ENABLED
#   undef ENABLED
#endif
#define ENABLED  1

#ifdef DISABLED
#   undef DISABLED
#endif
#define DISABLED  0

#ifdef NOT_AVAILABLE
#   undef NOT_AVAILABLE
#endif
#define NOT_AVAILABLE   DISABLED



/** Top Level Device Featureset <BR>
  * ========================================================================<BR>
  * For more information on feature configuration, check the wiki:
  * http://www.indigresso.com/wiki/doku.php?id=opentag:configuration
  *
  * The "Device Featureset" documents compiled-in features.  By changing the
  * setting to ENABLED/DISABLED, you are changing the way OpenTag compiles.
  * Disabling features you don't need will make the build smaller -- sometimes
  * a lot smaller.  Total build sizes tend to range between 10 - 40 KB.
  * 
  * Main device features are ultimately summarized in the DEV_FEATURES_BITMAP
  * constant, defined at the bottom of the section.  This 32 bit bitmap is 
  * converted into BASE64 along with the firmware type (OpenTag) and the version
  * and stored in the "Firmware Version" element of ISF 1 (Device Features).
  * By reading some ISF's (especially Device Features and Protocol List), a 
  * DASH7 gateway can figure out exactly what capabilities this device has.
  */

#define ALP(NAME)                       ALP_##NAME
#ifndef ALP_FILE_MGR
#   define ALP_FILE_MGR                 ENABLED
#endif
#ifndef ALP_LLAPI
#   define ALP_LLAPI                    DISABLED
#endif
#if ALP_SECURE_MGR
#   define ALP_SECURE_MGR               DISABLED
#endif
#ifndef ALP_LOGGER
#   define ALP_LOGGER                   ENABLED
#endif
#ifndef ALP_DASHFORTH
#   define ALP_DASHFORTH                DISABLED
#endif



#define OT_PARAM(VAL)                   OT_PARAM_##VAL
#define OT_FEATURE(VAL)                 OT_FEATURE_##VAL


#ifndef OT_PARAM_VLFPS
#   define OT_PARAM_VLFPS               3                                   // Number of files that can be open simultaneously
#endif
#ifndef OT_PARAM_VLACTIONS
#   define OT_PARAM_VLACTIONS           16                                  // Number of file action applets that can be kept simultaneously
#endif
#ifndef OT_PARAM_BUFFER_SIZE
#   define OT_PARAM_BUFFER_SIZE         (1024)                              // TX and RX application buffers
#endif


#ifndef OT_FEATURE_DASHFORTH
#   define OT_FEATURE_DASHFORTH         NOT_AVAILABLE                       // DASHFORTH Applet VM (server-side), or JIT (client-side)
#endif
#ifndef OT_FEATURE_LOGGER
#   define OT_FEATURE_LOGGER            ENABLED                             // Mpipe-based data logging & printing
#endif
#ifndef OT_FEATURE_ALP
#   define OT_FEATURE_ALP               ENABLED                             // Application Layer Protocol Support
#endif
#ifndef OT_FEATURE_ALPAPI
#   define OT_FEATURE_ALPAPI            (ENABLED && (OT_FEATURE_ALP))       // Application Layer Protocol callable API's
#endif
#ifndef OT_FEATURE_ALPEXT
#   define OT_FEATURE_ALPEXT            DISABLED                             
#endif
#ifndef OT_FEATURE_NDEF
#   define OT_FEATURE_NDEF              DISABLED                            // NDEF wrapper for Messaging API
#endif
#ifndef OT_FEATURE_MULTIFS
#   define OT_FEATURE_MULTIFS           DISABLED
#endif
#ifndef OT_FEATURE_VEELITE
#   define OT_FEATURE_VEELITE           ENABLED                             // Veelite DASH7 File System
#endif
#ifndef OT_FEATURE_VLNVWRITE
#   define OT_FEATURE_VLNVWRITE         ENABLED                             // File writes in Veelite
#endif
#ifndef OT_FEATURE_VLNEW
#   define OT_FEATURE_VLNEW             DISABLED                             // File create/delete in Veelite
#endif
#ifndef OT_FEATURE_VLRESTORE
#   define OT_FEATURE_VLRESTORE         ENABLED                             // File restore in Veelite
#endif
#ifndef OT_FEATURE_VLACTIONS
#   define OT_FEATURE_VLACTIONS         ENABLED                             // File Actions in Veelite
#endif
#ifndef OT_FEATURE_VLMODTIME
#   define OT_FEATURE_VLMODTIME         ENABLED                             // File Modification Timestamp
#endif
#ifndef OT_FEATURE_VL_SECURITY
#   define OT_FEATURE_VL_SECURITY       NOT_AVAILABLE                       // AES128 on pre-shared key, for stored files
#endif
#ifndef OT_FEATURE_DLL_SECURITY
#   define OT_FEATURE_DLL_SECURITY      ENABLED                            // AES128 on pre-shared key, for data-link
#endif
#ifndef OT_FEATURE_NL_SECURITY
#   define OT_FEATURE_NL_SECURITY       NOT_AVAILABLE                       // Network Layer Security & key exchange
#endif
#ifndef OT_FEATURE_TIME
#   define OT_FEATURE_TIME              (DISABLED || OT_FEATURE_DLL_SECURITY || OT_FEATURE_NL_SECURITY)
#endif
#ifndef OT_FEATURE_SENSORS
#   define OT_FEATURE_SENSORS           NOT_AVAILABLE                       // (formal, spec-based sensor config)
#endif





/// For the Device Features
#ifndef DEV_FEATURES_BITMAP
#define DEV_FEATURES_BITMAP (   ((ot_u32)OT_FEATURE_DASHFORTH << 29) | \
                                ((ot_u32)OT_FEATURE_LOGGER << 28) | \
                                ((ot_u32)OT_FEATURE_ALP << 27) | \
                                ((ot_u32)OT_FEATURE_NDEF << 26) | \
                                ((ot_u32)OT_FEATURE_VEELITE << 25) | \
                                ((ot_u32)OT_FEATURE_VLNVWRITE << 24) | \
                                ((ot_u32)OT_FEATURE_VLNEW << 23) | \
                                ((ot_u32)OT_FEATURE_VLRESTORE << 22) | \
                                ((ot_u32)OT_FEATURE_VL_SECURITY << 21) | \
                                ((ot_u32)OT_FEATURE_DLL_SECURITY << 20) | \
                                ((ot_u32)OT_FEATURE_NL_SECURITY << 19) | \
                                ((ot_u32)OT_FEATURE_SENSORS << 18) \
                            )
#endif


#endif 
