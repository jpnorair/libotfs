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
  * Libotfs is not thread-safe.
  *
  ******************************************************************************
  */

#ifndef __OTFS_H
#define __OTFS_H

/// Std C Headers.  No special rules here
#include <stdint.h>
#include <stddef.h>

/// OpenTag subsystem Headers.  In order for installation of libotfs to work properly,
/// these must use < > brackets and have only single space between #include and <.
/// The installation uses sed to modify these line on the installed copy of this header.

// All headers, for testing.
#include <otplatform.h>
#include <otstd.h>
#include <otlib.h>
// Main headers we want to expose
#include <otlib/alp.h>
#include <otlib/auth.h>
#include <otsys/veelite.h>




typedef struct OT_PACKED {
    uint8_t oui24[3];
    uint8_t ext40[5];
} otfs_eui64_t;


typedef union {
    uint64_t        u64;
    uint32_t        u32[2];
    uint16_t        u16[4];
    
#   if !defined(__C2000__)
    uint8_t         u8[8];
    otfs_eui64_t    eui64;
#   endif
} otfs_id_union;


typedef struct {
    otfs_id_union   fs_id;
    void*           fs_base;
    size_t          fs_alloc;
} otfs_t;


int otfs_defaults(otfs_t* fs, size_t maxalloc);

int otfs_new(const otfs_t* fs);

int otfs_del(const otfs_t* fs);

int otfs_setfs(const uint8_t* eui64_bytes);


#endif
