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
  * @file       /app/csip_c2000/features_config.h
  * @author     JP Norair (jpnorair@indigresso.com)
  * @version    R101
  * @date       31 Aug 2014
  * @brief      Feature configuration
  *
  *
  ******************************************************************************
  */

#ifndef __FEATURES_CONFIG_H
#define __FEATURES_CONFIG_H

///@note You can override some Default Features & Parameters here
#define OT_FEATURE_VLACTIONS            ENABLED          // File Actions in Veelite
#define OT_FEATURE_VLMODTIME            DISABLED         // File Modification Timestamp
#define OT_FEATURE_VLACCTIME            DISABLED

#include <app/features_defaults.h>


#endif 
