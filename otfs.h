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
  * @file       /otfs.h
  * @author     JP Norair (jpnorair@indigresso.com)
  * @version    R100
  * @date       31 Oct 2014
  * @brief      Top Level Header for libotfs
  *
  * Only necessary for compiling as libotfs, and when linking to the library.
  *
  ******************************************************************************
  */

#ifndef __OTFS_H
#define __OTFS_H

// All headers, for testing.
#include <otplatform.h>
#include <otstd.h>
#include <otlib.h>

// Main headers we want to expose
#include <otlib/alp.h>
#include <otlib/auth.h>
#include <otsys/veelite.h>

#endif
