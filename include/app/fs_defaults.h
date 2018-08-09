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
  * @file       /app/fs_defaults.h
  * @author     JP Norair (jpnorair@indigresso.com)
  * @version    R101
  * @date       31 Oct 2017
  * @brief      Parent Filesystem Defaults File
  *
  * You need to update this file any time an app is added.
  *
  * For most platforms, this file should be included in only one place:
  * /platform/xxx/fs_defaults.c
  *
  ******************************************************************************
  */

#ifndef __PARENT_FS_DEFAULTS_H
#define __PARENT_FS_DEFAULTS_H


#if defined(APP_csip_master)
#   include <app/csip_master/fs_defaults.h>

#elif defined(APP_csip_slave)
#   include <app/csip_slave/fs_defaults.h>

// Default APP for C2000
#elif defined(APP_csip_c2000) || defined(__C2000__)
#   include <app/csip_c2000/fs_defaults.h>

// Default APP for other conditions
#else
#   include <app/null/fs_defaults.h>

#endif


#endif 
