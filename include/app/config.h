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
  * @file       /app/config.h
  * @author     JP Norair (jpnorair@indigresso.com)
  * @version    R101
  * @date       31 Oct 2017
  * @brief      Parent Application Configuration File
  *
  * You need to update this file any time an app is added.
  *
  ******************************************************************************
  */

#ifndef __PARENT_APP_CONFIG_H
#define __PARENT_APP_CONFIG_H


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


#if defined(APP_csip_master)
#   include <app/csip_master/app_config.h>
#   include <app/csip_master/board_config.h>
#   include <app/csip_master/build_config.h>
#   include <app/csip_master/extf_config.h>
//#   include <app/csip_master/features_config.h>
//#   include <app/csip_master/fs_config.h>

#elif defined(APP_csip_slave)
#   include <app/csip_slave/app_config.h>
#   include <app/csip_slave/board_config.h>
#   include <app/csip_slave/build_config.h>
#   include <app/csip_slave/extf_config.h>

#elif defined(APP_csip_c2000)
#   include <app/csip_c2000/app_config.h>
#   include <app/csip_c2000/board_config.h>
#   include <app/csip_c2000/build_config.h>
#   include <app/csip_c2000/extf_config.h>

// Default app is "Null", which is a POSIX Std C target on a 32 or 64 bit x86 or ARM
// machine.
#else
#   include <app/null/app_config.h>
#   include <app/null/board_config.h>
#   include <app/null/build_config.h>
#   include <app/null/extf_config.h>

#endif


// Default Features.
// These can be overriden in the application specific feature configuration
#include "features_defaults.h"



#endif 
