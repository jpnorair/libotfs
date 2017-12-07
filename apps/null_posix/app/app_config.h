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
  * @file       /apps/null_posix/app/app_config.h
  * @author     JP Norair (jpnorair@indigresso.com)
  * @version    R101
  * @date       31 Oct 2017
  * @brief      Application Configuration File for Null Posix Build
  *
  ******************************************************************************
  */

#ifndef __APP_CONFIG_H
#define __APP_CONFIG_H


#include <app/build_config.h>



/** Filesystem constants, setup, and boundaries <BR>
  * ========================================================================<BR>
  * Before the #include statement below, you can set-up some overriding feature
  * and parameter settings.  Take a look at features_default_config.h to see
  * what the features are.
  */

#define OT_FEATURE_MULTIFS          ENABLED

#include "features_default_config.h"





/** Filesystem constants, setup, and boundaries <BR>
  * ========================================================================<BR>
  * For this app, we are just using the default FS setup, which is located in
  * /apps/_common/
  */
#include "fs_default_config.h"


#endif 
